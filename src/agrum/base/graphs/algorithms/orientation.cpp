/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Implementation of the orientation algorithms for partially directed graphs.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Christophe GONZALES(_at_AMU)
 */
#include <algorithm>
#include <cstdint>
#include <limits>
#include <list>
#include <map>
#include <queue>
#include <tuple>
#include <vector>

#include <agrum/base/core/configuration/configuration.h>
#include <agrum/base/graphs/algorithms/generic/chordality.h>
#include <agrum/base/graphs/algorithms/generic/cycleDetection.h>
#include <agrum/base/graphs/algorithms/orientation.h>

namespace gum::graph {

  namespace {

    /// gives to the nodes of to the names of the same nodes in from
    void copyNames(const NodeGraphPart& from, NodeGraphPart& to) {
      for (const auto node: from.nodes())
        if (from.hasName(node)) to.setName(node, from.nameFromId(node));
    }

    /// adjacency (arc or edge, in any direction) in a mixed graph
    bool adjacent(const MixedGraph& g, NodeId a, NodeId b) {
      return g.existsEdge(a, b) || g.existsArc(a, b) || g.existsArc(b, a);
    }

    /// the nodes adjacent to node (neighbours, parents and children)
    Size adjacencySize(const MixedGraph& g, NodeId node) {
      return g.neighbours(node).size() + g.parents(node).size() + g.children(node).size();
    }

    /**
     * Partially directed graph maintaining, for each node v, the counters of Wienöbst et al.
     * (2021, supplementary material):
     * - alpha[v] = number of adjacent pairs {x, y} of undirected neighbours of v;
     * - beta[v]  = number of adjacent pairs {x, y} with x an undirected neighbour and y a parent of
     * v. v is a potential sink iff v has no child, alpha[v] = k(k-1)/2 and beta[v] = k.p, with k
     * the number of its undirected neighbours and p the number of its parents.
     */
    class PotentialSinkGraph {
      public:
      explicit PotentialSinkGraph(const NodeGraphPart& nodes) {
        for (const auto node: nodes.nodes()) {
          graph_.addNodeWithId(node);
          alpha_.insert(node, 0);
          beta_.insert(node, 0);
        }
      }

      const MixedGraph& graph() const { return graph_; }

      void addEdge(NodeId u, NodeId v) {
        graph_.addEdge(u, v);
        updateCounters_(u, v, true);
      }

      void addArc(NodeId tail, NodeId head) {
        graph_.addArc(tail, head);
        updateCounters_(tail, head, true);
      }

      void erase(NodeId u, NodeId v) {
        updateCounters_(u, v, false);
        if (graph_.existsEdge(u, v)) graph_.eraseEdge(Edge(u, v));
        else if (graph_.existsArc(u, v)) graph_.eraseArc(Arc(u, v));
        else graph_.eraseArc(Arc(v, u));
      }

      bool isPotentialSink(NodeId node) const {
        if (!graph_.children(node).empty()) return false;
        const Size k = graph_.neighbours(node).size();
        const Size p = graph_.parents(node).size();
        return alpha_[node] == k * (k - 1) / 2 && beta_[node] == k * p;
      }

      private:
      MixedGraph           graph_;
      NodeProperty< Size > alpha_;
      NodeProperty< Size > beta_;

      /// increases (or decreases) the counters of w for the pair {a, b} of its adjacent nodes
      void updatePair_(NodeId w, NodeId a, NodeId b, bool increase) {
        auto       update = [increase](Size& counter) { increase ? ++counter : --counter; };
        const bool edgeA  = graph_.existsEdge(w, a);
        const bool edgeB  = graph_.existsEdge(w, b);
        if (edgeA && edgeB) update(alpha_[w]);
        if (edgeA && graph_.existsArc(b, w)) update(beta_[w]);
        if (edgeB && graph_.existsArc(a, w)) update(beta_[w]);
      }

      /// the connection u~v belongs to the triangles {u, v, x}: update the pairs of their 3 nodes
      void updateCounters_(NodeId u, NodeId v, bool increase) {
        // iterate over the adjacency of the endpoint with the smallest one
        const NodeId p     = adjacencySize(graph_, u) <= adjacencySize(graph_, v) ? u : v;
        const NodeId q     = (p == u) ? v : u;
        auto         visit = [&](NodeId x) {
          if (x == q || !adjacent(graph_, x, q)) return;
          updatePair_(x, u, v, increase);
          updatePair_(u, v, x, increase);
          updatePair_(v, u, x, increase);
        };
        for (const auto x: graph_.neighbours(p))
          visit(x);
        for (const auto x: graph_.parents(p))
          visit(x);
        for (const auto x: graph_.children(p))
          visit(x);
      }
    };

  }   // namespace

  Sequence< NodeId > degeneracyOrdering(const MixedGraph& g) {
    // smallest-last: repeatedly remove a node of minimal degree; the ordering is the reverse of
    // the removals
    // a mixed graph may connect a pair twice (x->y and y->x, or x->y and x-y): the degree of a node
    // may exceed g.size() - 1
    NodeProperty< Size > degree;
    Size                 maxDegree = 0;
    for (const auto node: g.nodes()) {
      degree.insert(node, adjacencySize(g, node));
      maxDegree = std::max(maxDegree, degree[node]);
    }
    std::vector< std::list< NodeId > >            buckets(maxDegree + 1);
    NodeProperty< std::list< NodeId >::iterator > positionOf;
    for (const auto node: g.nodes()) {
      const Size d = degree[node];
      buckets[d].push_back(node);
      positionOf.insert(node, std::prev(buckets[d].end()));
    }

    std::vector< NodeId > removed;
    removed.reserve(g.size());
    Size smallest = 0;
    while (removed.size() < g.size()) {
      while (buckets[smallest].empty())
        ++smallest;
      const NodeId node = buckets[smallest].front();
      buckets[smallest].pop_front();
      degree.erase(node);
      removed.push_back(node);

      auto decrease = [&](NodeId x) {
        if (!degree.exists(x)) return;
        Size& d = degree[x];
        buckets[d - 1].splice(buckets[d - 1].end(), buckets[d], positionOf[x]);
        --d;
        if (d < smallest) smallest = d;
      };
      for (const auto x: g.neighbours(node))
        decrease(x);
      for (const auto x: g.parents(node))
        decrease(x);
      for (const auto x: g.children(node))
        decrease(x);
    }

    Sequence< NodeId > order;
    for (auto it = removed.rbegin(); it != removed.rend(); ++it)
      order.insert(*it);
    return order;
  }

  namespace {
    /**
     * Removal of the potential sinks (Dor and Tarsi 1992) with the counters of Wienöbst et al.
     * (2021). Without domainSizes, the potential sinks are taken in any order (a stack); with
     * domainSizes, the potential sink with the smallest CPT (the product of the domain sizes of the
     * sink and of its remaining adjacent nodes, which become its parents) is taken first, then the
     * one with the largest domain (so that the small domains remain parents), then the one with
     * the smallest id. If relaxed (with domainSizes only), a node without child is removed when
     * there is no potential sink, as if it were one: the result is then always a DAG, with new
     * v-structures if pdag is not extendable.
     */
    std::optional< DAG > extensionBySinkRemoval(const PDAG&                 pdag,
                                                const NodeProperty< Size >* domainSizes,
                                                bool                        relaxed) {
      // insertion of the edges following a degeneracy ordering: each node is connected to its
      // (at most d) predecessors only
      PotentialSinkGraph working(pdag);
      const auto         order = degeneracyOrdering(pdag);
      for (const auto node: order) {
        const Idx pos = order.pos(node);
        for (const auto x: pdag.neighbours(node))
          if (order.pos(x) < pos) working.addEdge(x, node);
        for (const auto x: pdag.parents(node))
          if (order.pos(x) < pos) working.addArc(x, node);
        for (const auto x: pdag.children(node))
          if (order.pos(x) < pos) working.addArc(node, x);
      }
      const MixedGraph& g = working.graph();

      // the candidates: a stack, or a priority queue (cost, -domain size, id) with lazy deletion
      using Candidate = std::tuple< double, double, NodeId >;
      std::vector< NodeId >                                                      stack;
      NodeSet                                                                    inStack;
      std::priority_queue< Candidate, std::vector< Candidate >, std::greater<> > queue;
      const auto cost = [&g, domainSizes](NodeId node) {
        auto c = double((*domainSizes)[node]);
        for (const auto x: g.parents(node))
          c *= double((*domainSizes)[x]);
        for (const auto x: g.neighbours(node))
          c *= double((*domainSizes)[x]);
        return c;
      };
      const auto push = [&](NodeId node) {
        if (domainSizes == nullptr) {
          if (inStack.contains(node)) return;
          stack.push_back(node);
          inStack.insert(node);
        } else {
          queue.emplace(cost(node), -double((*domainSizes)[node]), node);
        }
      };
      // the removed nodes; parentsOf[s] are the parents of s in the extension
      std::vector< NodeId >                 removed;
      NodeProperty< std::vector< NodeId > > parentsOf;

      // the next potential sink, or std::nullopt
      const auto pop = [&]() -> std::optional< NodeId > {
        if (domainSizes == nullptr) {
          while (!stack.empty()) {
            const NodeId node = stack.back();
            stack.pop_back();
            inStack.erase(node);
            if (working.isPotentialSink(node)) return node;   // else a neighbour has been removed
          }
        } else {
          while (!queue.empty()) {
            const NodeId node = std::get< 2 >(queue.top());
            queue.pop();
            // a node is pushed again each time its adjacency changes, and its cost only decreases:
            // its current entry is popped before its stale ones, which are then skipped since the
            // node has been removed (comparing the costs would depend on the rounding of products)
            if (!parentsOf.exists(node) && working.isPotentialSink(node)) return node;
          }
        }
        return std::nullopt;
      };

      for (const auto node: g.nodes())
        if (working.isPotentialSink(node)) push(node);

      // removal of the potential sinks
      Size nbConnections = g.sizeArcs() + g.sizeEdges();
      while (nbConnections > 0) {
        auto next = pop();
        if (!next.has_value() && relaxed) {
          // no potential sink: the connected node without child with the smallest CPT (there is
          // one, since pdag has no directed cycle)
          std::optional< Candidate > best;
          for (const auto node: g.nodes())
            if (g.children(node).empty() && adjacencySize(g, node) > 0) {
              const Candidate candidate{cost(node), -double((*domainSizes)[node]), node};
              if (!best.has_value() || candidate < *best) best = candidate;
            }
          if (best.has_value()) next = std::get< 2 >(*best);
        }
        if (!next.has_value()) break;
        const NodeId sink = *next;

        std::vector< NodeId > parents(g.parents(sink).begin(), g.parents(sink).end());
        parents.insert(parents.end(), g.neighbours(sink).begin(), g.neighbours(sink).end());
        for (const auto x: parents)
          working.erase(x, sink);
        nbConnections -= parents.size();
        removed.push_back(sink);
        parentsOf.insert(sink, std::move(parents));

        for (const auto x: parentsOf[sink])
          if (working.isPotentialSink(x)) push(x);
      }
      if (nbConnections > 0) return std::nullopt;

      // the reverse of the removals is a topological order of the extension: adding the arcs
      // into a node before any arc out of it keeps the acyclicity checks of DAG::addArc trivial
      DAG extension;
      for (const auto node: pdag.nodes())
        extension.addNodeWithId(node);
      copyNames(pdag, extension);
      for (auto it = removed.rbegin(); it != removed.rend(); ++it)
        for (const auto parent: parentsOf[*it])
          extension.addArc(parent, *it);
      return extension;
    }

    /// true if, in the PDAG g, all the nodes of an undirected component have the same parents:
    /// for every arc a->b and every edge b-c, a->c
    bool hasCommonParentsInComponents(const PDAG& g) {
      for (const auto& arc: g.arcs())
        for (const auto c: g.neighbours(arc.head()))
          if (!g.existsArc(arc.tail(), c)) return false;
      return true;
    }

    /// the connected components of the undirected edges of g restricted to the nodes of `nodes`
    std::vector< std::vector< NodeId > > undirectedComponents(const PDAG&                  g,
                                                              const std::vector< NodeId >& nodes) {
      NodeSet todo;
      for (const auto node: nodes)
        todo.insert(node);
      std::vector< std::vector< NodeId > > components;
      for (const auto start: nodes) {
        if (!todo.contains(start)) continue;
        std::vector< NodeId > component{start};
        todo.erase(start);
        for (Idx i = 0; i < component.size(); ++i)
          for (const auto x: g.neighbours(component[i]))
            if (todo.contains(x)) {
              todo.erase(x);
              component.push_back(x);
            }
        std::sort(component.begin(), component.end());
        components.push_back(std::move(component));
      }
      return components;
    }

    /// orientations without v-structure of minimal total CPT size of the connected chordal
    /// undirected components of a PDAG: the recursion of Clique-Picking described with
    /// graph::orientAsDAG (orientation.h)
    class MinimalCPTOrientation {
      public:
      MinimalCPTOrientation(const PDAG& g, const NodeProperty< Size >& domainSizes) :
          g_(g), domainSizes_(domainSizes) {}

      /// adds to arcs the orientation of the undirected component `component` (sorted node ids)
      void orient(const std::vector< NodeId >& component, std::vector< Arc >& arcs) {
        const auto& solution = solve_(component);
        arcs.insert(arcs.end(), solution.arcs.begin(), solution.arcs.end());
        for (const auto& sub: solution.components)
          orient(sub, arcs);
      }

      private:
      struct Solution {
        double                               cost = std::numeric_limits< double >::infinity();
        std::vector< Arc >                   arcs;         // the arcs of the closure with K first
        std::vector< std::vector< NodeId > > components;   // its remaining undirected components
      };

      const PDAG&                                 g_;
      const NodeProperty< Size >&                 domainSizes_;
      std::map< std::vector< NodeId >, Solution > memo_;

      double size_(NodeId node) const { return double(domainSizes_[node]); }

      /// the maximal cliques of the connected chordal graph h: the families {v} and its neighbours
      /// visited before v by a LexBFS (cliques, the reverse being a perfect elimination ordering)
      /// that are not included in another family, which then is the family of a later neighbour
      static std::vector< std::vector< NodeId > > maximalCliques_(const UndiGraph& h) {
        const auto              order = lexBFS(h);
        NodeProperty< NodeSet > family;
        for (const auto v: order) {
          NodeSet f{v};
          for (const auto u: h.neighbours(v))
            if (order.pos(u) < order.pos(v)) f.insert(u);
          family.insert(v, std::move(f));
        }
        std::vector< std::vector< NodeId > > cliques;
        for (const auto v: order) {
          bool maximal = true;
          for (const auto w: h.neighbours(v))
            if (order.pos(w) > order.pos(v) && family[w].size() > family[v].size()
                && family[v].isSubsetOrEqual(family[w])) {
              maximal = false;
              break;
            }
          if (maximal) cliques.emplace_back(family[v].begin(), family[v].end());
        }
        return cliques;
      }

      const Solution& solve_(const std::vector< NodeId >& component) {
        if (const auto it = memo_.find(component); it != memo_.end()) return it->second;

        Solution best;
        if (component.size() == 1) {
          best.cost = size_(component.front());
          return memo_.emplace(component, std::move(best)).first->second;
        }

        UndiGraph h;
        for (const auto node: component)
          h.addNodeWithId(node);
        for (const auto node: component)
          for (const auto x: g_.neighbours(node))
            if (node < x && h.existsNode(x)) h.addEdge(node, x);

        for (auto clique: maximalCliques_(h)) {
          // increasing domain sizes, then decreasing ids (the smallest id is the last one)
          std::sort(clique.begin(), clique.end(), [this](NodeId x, NodeId y) {
            return size_(x) < size_(y) || (size_(x) == size_(y) && x > y);
          });
          double cost = 0.0, prefix = 1.0;
          for (const auto node: clique) {
            prefix *= size_(node);
            cost += prefix;
          }

          // h with the clique first: ordered inside, oriented towards the rest
          NodeProperty< Idx > posInClique;
          for (Idx i = 0; i < clique.size(); ++i)
            posInClique.insert(clique[i], i);
          // built as a mixed graph then checked once by the PDAG constructor (PDAG::addArc would
          // look for a directed path at each arc)
          MixedGraph first;
          for (const auto node: component)
            first.addNodeWithId(node);
          for (const auto& edge: h.edges()) {
            const NodeId x = edge.first(), y = edge.second();
            const bool   inX = posInClique.exists(x), inY = posInClique.exists(y);
            if (inX && inY) {
              if (posInClique[x] < posInClique[y]) first.addArc(x, y);
              else first.addArc(y, x);
            } else if (inX) first.addArc(x, y);
            else if (inY) first.addArc(y, x);
            else first.addEdge(x, y);
          }
          // extendable: h is chordal, K is a clique
          const auto closure = meekClosure(PDAG(first));

          std::vector< NodeId > rest;
          for (const auto node: component)
            if (!posInClique.exists(node)) rest.push_back(node);
          auto components = undirectedComponents(*closure, rest);
          for (const auto& sub: components) {
            double parentFactor = 1.0;   // the parents are common to all the nodes of sub
            for (const auto p: closure->parents(sub.front()))
              parentFactor *= size_(p);
            cost += parentFactor * solve_(sub).cost;
            if (cost >= best.cost) break;
          }
          // the first clique is always kept, even if all the costs overflow to infinity
          if (best.arcs.empty() || cost < best.cost) {
            best.cost = cost;
            best.arcs.assign(closure->arcs().begin(), closure->arcs().end());
            best.components = std::move(components);
          }
        }
        return memo_.emplace(component, std::move(best)).first->second;
      }
    };

    /// a consistent extension of the PDAG closure (a Meek closure whose undirected components have
    /// common parents) with a minimal total size of the CPTs for its undirected components of at
    /// most maxExactSize nodes, the larger ones being oriented by the greedy removal of the
    /// potential sinks (the common parents of a component do not change its choices)
    DAG minimalCPTExtension(const PDAG&                 closure,
                            const NodeProperty< Size >& domainSizes,
                            Size                        maxExactSize) {
      std::vector< Arc >    arcs(closure.arcs().begin(), closure.arcs().end());
      std::vector< NodeId > nodes(closure.nodes().begin(), closure.nodes().end());
      std::sort(nodes.begin(), nodes.end());
      MinimalCPTOrientation orientation(closure, domainSizes);
      for (const auto& component: undirectedComponents(closure, nodes)) {
        if (component.size() <= 1) continue;
        if (component.size() <= maxExactSize) {
          orientation.orient(component, arcs);
          continue;
        }
        PDAG undirected;
        for (const auto node: component)
          undirected.addNodeWithId(node);
        for (const auto node: component)
          for (const auto x: closure.neighbours(node))
            if (node < x) undirected.addEdge(node, x);
        // a component of a Meek closure is chordal: it always has a consistent extension
        const auto greedy = extensionBySinkRemoval(undirected, &domainSizes, false);
        GUM_ASSERT(greedy.has_value());
        arcs.insert(arcs.end(), greedy->arcs().begin(), greedy->arcs().end());
      }

      // the arcs into a node are added before any arc out of it (topological order of the heads):
      // the acyclicity checks of DAG::addArc remain trivial
      DiGraph digraph;
      for (const auto node: nodes)
        digraph.addNodeWithId(node);
      for (const auto& arc: arcs)
        digraph.addArc(arc.tail(), arc.head());
      DAG extension;
      for (const auto node: nodes)
        extension.addNodeWithId(node);
      copyNames(closure, extension);
      for (const auto node: digraph.topologicalOrder())
        for (const auto parent: digraph.parents(node))
          extension.addArc(parent, node);
      return extension;
    }
  }   // namespace

  std::optional< DAG > consistentExtension(const PDAG& pdag) {
    return extensionBySinkRemoval(pdag, nullptr, false);
  }

  namespace {
    void checkDomainSizes(const PDAG& pdag, const NodeProperty< Size >& domainSizes) {
      for (const auto node: pdag.nodes())
        if (!domainSizes.exists(node))
          GUM_ERROR(InvalidArgument, "No domain size for the node " << node)
        else if (domainSizes[node] == 0)
          GUM_ERROR(InvalidArgument, "The domain size of the node " << node << " is 0")
    }

    /// orientAsDAG, given the Meek closure of pdag (nullptr if pdag is not extendable): exact for
    /// a CPDAG, and for any extendable PDAG whose Meek closure has components with common parents;
    /// the greedy removal of the potential sinks otherwise
    DAG orientWithClosure(const PDAG&                 pdag,
                          const PDAG*                 closure,
                          const NodeProperty< Size >& domainSizes) {
      if (closure != nullptr && hasCommonParentsInComponents(*closure)) {
        const auto maxExactSize
            = gum::config().typed< std::int64_t >("graph", "orientAsDAGExactMaxSize");
        return minimalCPTExtension(*closure, domainSizes, static_cast< Size >(maxExactSize));
      }
      return *extensionBySinkRemoval(pdag, &domainSizes, true);
    }
  }   // namespace

  DAG orientAsDAG(const PDAG& pdag, const NodeProperty< Size >& domainSizes) {
    checkDomainSizes(pdag, domainSizes);
    const auto closure = meekClosure(pdag);
    return orientWithClosure(pdag, closure.has_value() ? &*closure : nullptr, domainSizes);
  }

  namespace detail {
    DAG orientClosureAsDAG(const PDAG& closure, const NodeProperty< Size >& domainSizes) {
      checkDomainSizes(closure, domainSizes);
      return orientWithClosure(closure, &closure, domainSizes);
    }
  }   // namespace detail

  ChainGraph cpdag(const DAG& dag) {
    // Order-Edges: heads by increasing topological rank, then tails by decreasing rank
    const auto topologicalRank = dag.topologicalOrder();

    // Find-Compelled: when the first unknown arc x->y is handled, all the arcs into y are unknown
    // (Lemma 5) and they are all labelled at the end of the step
    ArcProperty< bool > compelled;
    // the compelled arcs into each visited node: scanning only them keeps the time linear (each one
    // either ends the scan or labels a distinct arc into y)
    NodeProperty< std::vector< NodeId > > compelledParents;
    for (const auto y: topologicalRank) {
      const auto& parents        = dag.parents(y);
      auto&       compelledIntoY = compelledParents.insert(y, {}).second;
      if (parents.empty()) continue;

      NodeId x = *parents.begin();
      for (const auto p: parents)
        if (topologicalRank.pos(p) > topologicalRank.pos(x)) x = p;

      bool allCompelled = false;
      for (const auto w: compelledParents[x]) {
        if (!dag.existsArc(w, y)) {
          allCompelled = true;
          break;
        }
        compelled.insert(Arc(w, y), true);
      }

      if (!allCompelled)
        for (const auto z: parents)
          if (z != x && !dag.existsArc(z, x)) {
            allCompelled = true;
            break;
          }

      // the arcs into y not already labelled get the label of x->y
      for (const auto p: parents)
        if (!compelled.exists(Arc(p, y))) compelled.insert(Arc(p, y), allCompelled);
      if (allCompelled)
        for (const auto p: parents)
          compelled[Arc(p, y)] = true;
      for (const auto p: parents)
        if (compelled[Arc(p, y)]) compelledIntoY.push_back(p);
    }

    MixedGraph result;
    for (const auto node: dag.nodes())
      result.addNodeWithId(node);
    copyNames(dag, result);
    for (const auto& arc: dag.arcs()) {
      if (compelled[arc]) result.addArc(arc.tail(), arc.head());
      else result.addEdge(arc.tail(), arc.head());
    }
    return ChainGraph(result);
  }

  std::optional< PDAG > meekClosure(const PDAG& pdag) {
    if (pdag.sizeEdges() == 0) return pdag;   // a PDAG without edge is a DAG

    const auto extension = consistentExtension(pdag);
    if (!extension.has_value()) return std::nullopt;
    return meekClosure(pdag, *extension);
  }

  PDAG meekClosure(const PDAG& pdag, const DAG& extension) {
    const auto order = extension.topologicalOrder();

    // the arcs of the CPDAG are in every extension: only its chain components remain to orient.
    // Built from pdag (same skeleton as its extension), whose node names are kept
    MixedGraph       closure(pdag);
    const ChainGraph compelled = cpdag(extension);
    for (const auto& arc: compelled.arcs())
      if (closure.existsEdge(arc.tail(), arc.head())) {
        closure.eraseEdge(Edge(arc.tail(), arc.head()));
        closure.addArc(arc.tail(), arc.head());
      }

    const auto orient = [&closure](NodeId tail, NodeId head) {
      closure.eraseEdge(Edge(tail, head));
      closure.addArc(tail, head);
    };

    // every edge into an already visited node has its final orientation: a rule can only orient
    // an edge p-v into p->v, p before v in the topological order. These nodes p form a clique (the
    // extension has no v-structure inside a chain component of its CPDAG): at most d of them
    for (const auto v: order) {
      std::vector< NodeId > before;
      for (const auto p: closure.neighbours(v))
        if (order.pos(p) < order.pos(v)) before.push_back(p);
      if (before.empty()) continue;

      // R1: a->p-v, a and v not adjacent. A scanned a adjacent to v is charged to the adjacency
      // a-v, at most d times: O(dm) in all
      for (const auto p: before)
        for (const auto a: closure.parents(p))
          if (!adjacent(closure, a, v)) {
            orient(p, v);
            break;
          }

      // R4: a-d, d->p->v, a-v, a and p adjacent, d and v not adjacent (p->v follows from R1). The
      // candidates a are the remaining undirected neighbours of v before it (at most d)
      std::vector< NodeId > r4;
      for (const auto a: before) {
        if (!closure.existsEdge(a, v)) continue;
        bool found = false;
        for (const auto p: closure.parents(v)) {
          if (!adjacent(closure, a, p)) continue;
          for (const auto d: closure.parents(p))
            if (closure.existsEdge(a, d) && !adjacent(closure, d, v)) {
              found = true;
              break;
            }
          if (found) break;
        }
        if (found) r4.push_back(a);
      }
      for (const auto a: r4)
        orient(a, v);

      // R2: p->b->v, p-v; the closest p first, so that b->v is already oriented if it has to be.
      // b is searched among the parents of v (at most d scans of them: O(dm) in all), not among the
      // children of p, which are not bounded
      std::sort(before.begin(), before.end(), [&order](NodeId x, NodeId y) {
        return order.pos(x) > order.pos(y);
      });
      for (const auto p: before) {
        if (!closure.existsEdge(p, v)) continue;
        for (const auto b: closure.parents(v))
          if (closure.existsArc(p, b)) {
            orient(p, v);
            break;
          }
      }
    }
    return PDAG(closure);
  }

}   // namespace gum::graph
