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


/** @file
 * @brief Class for Meek Rules
 *
 * @author Mahdi Hadj Ali and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/graphs/algorithms/generic/cycleDetection.h>
#include <agrum/base/graphs/algorithms/MeekRules.h>
#include <agrum/base/graphs/algorithms/orientation.h>

#ifdef GUM_NO_INLINE
#  include <agrum/base/graphs/algorithms/MeekRules_inl.h>
#endif   // GUM_NO_INLINE

namespace gum {

  namespace {
    /// the closure of mg under the Meek rules if mg is an extendable PDAG, std::nullopt otherwise
    std::optional< PDAG > extendableClosure(const MixedGraph& mg) {
      for (const auto& edge: mg.edges())
        if (mg.existsArc(edge.first(), edge.second()) || mg.existsArc(edge.second(), edge.first()))
          return std::nullopt;
      if (graph::hasDirectedCycle(mg)) return std::nullopt;   // including double-headed arcs
      return graph::meekClosure(PDAG(mg));
    }
  }   // namespace

  /// default constructor
  MeekRules::MeekRules() { GUM_CONSTRUCTOR(MeekRules); }

  /// destructor
  MeekRules::~MeekRules() { GUM_DESTRUCTOR(MeekRules); }

  /// Propagates MeekRules in a MixedGraph
  MixedGraph MeekRules::propagate(const MixedGraph& graph) {
    _choices_.clear();
    if (auto closure = extendableClosure(graph)) return std::move(*closure);
    return _propagates_(graph);
  }

  /// Propagates the orientation of a MixedGraph (no double-headed arcs) and return a PDAG.
  PDAG MeekRules::propagateToCPDAG(const MixedGraph& mg) {
    _choices_.clear();
    if (auto closure = extendableClosure(mg)) return std::move(*closure);
    return _propagatesToPDAG_(mg);
  }

  PDAG MeekRules::_propagatesToPDAG_(const MixedGraph& mg) {
    MixedGraph graph = _propagates_(mg);

    // Resolve double-headed arc while avoiding cycle creation.
    _orientDoubleHeadedArcs_(graph);

    // a pair connected by an arc and an edge keeps only the arc
    std::vector< Edge > doubled;
    for (const Edge& edge: graph.edges())
      if (graph.existsArc(edge.first(), edge.second())
          || graph.existsArc(edge.second(), edge.first()))
        doubled.push_back(edge);
    for (const Edge& edge: doubled)
      graph.eraseEdge(edge);
    // only the 2-cycles have been resolved: a longer directed cycle of mg remains, rejected by the
    // PDAG (checked once, names kept)
    return PDAG(graph);
  }

  /// Propagates the orientation of a MixedGraph and return a DAG.
  DAG MeekRules::propagateToDAG(const gum::MixedGraph& mg) {
    // no domain size: all the domains are considered equal
    NodeProperty< Size > domainSizes;
    for (const auto node: mg.nodes())
      domainSizes.insert(node, 2);
    return propagateToDAG(mg, domainSizes);
  }

  DAG MeekRules::propagateToDAG(const MixedGraph& mg, const NodeProperty< Size >& domainSizes) {
    // the propagated graph: the closure of an extendable PDAG (not computed again to orient it), or
    // the result of the former propagation (its arcs are kept in the DAG in both cases)
    _choices_.clear();
    std::optional< PDAG > pdag = extendableClosure(mg);
    DAG                   dag;
    if (pdag.has_value()) {
      dag = graph::detail::orientClosureAsDAG(*pdag, domainSizes);
    } else {
      pdag = _propagatesToPDAG_(mg);
      dag  = graph::orientAsDAG(*pdag, domainSizes);
    }

    // the arbitrary choices: the orientations of the edges left undirected
    for (const auto& edge: pdag->edges())
      if (dag.existsArc(edge.first(), edge.second()))
        _choices_.emplace_back(edge.first(), edge.second());
      else _choices_.emplace_back(edge.second(), edge.first());
    return dag;
  }

  /// Orient double-headed arcs while avoiding cycles
  void MeekRules::_orientDoubleHeadedArcs_(MixedGraph& mg) {
    gum::ArcSet L;   // create a set of all double-headed arcs
    for (gum::NodeId x: mg.nodes()) {
      for (NodeId y: mg.parents(x)) {
        // If there is a mutual parent-child relationship, add the arc to the set
        if (mg.parents(y).contains(x)) {
          if (x > y) {
            continue;   // Avoid duplicate arcs by considering only one direction
          } else {
            L.insert(gum::Arc(x, y));
          }
        }
      }
    }

    // If there are double-headed arcs
    if (!L.empty()) {
      while (true) {
        bool withdrawFlag_L = false;
        for (auto& arc: ArcSet(L)) {
          const bool tail_head        = _existsDirectedPath_(mg, arc.tail(), arc.head());
          const bool head_tail        = _existsDirectedPath_(mg, arc.head(), arc.tail());
          bool       withdrawFlag_arc = false;

          // Case 1: There is already a path from tail to head and no path from head to tail
          if (tail_head && !head_tail) {
            // Erase the arc from head to tail to avoid cycles
            mg.eraseArc(Arc(arc.head(), arc.tail()));
            withdrawFlag_arc = true;

            // Case 2: There is already a path from head to tail and no path from tail to head
          } else if (!tail_head && head_tail) {
            // Erase the arc from tail to head to avoid cycles
            mg.eraseArc(Arc(arc.tail(), arc.head()));
            withdrawFlag_arc = true;

            // Case 3: There is no path between tail and head
          } else if (!tail_head && !head_tail) {
            // Choose an arbitrary orientation and erase the corresponding arc
            // mg.eraseArc(Arc(arc.head(), arc.tail()));
            mg.eraseArc(_selectArcToEraseByMinParents_(mg, arc.tail(), arc.head()));
            withdrawFlag_arc = true;
          }

          // Remove the arc from the set if it was processed
          if (withdrawFlag_arc) {
            L.erase(arc);
            withdrawFlag_L = true;
          }
        }
        // If all double-headed arcs are processed, exit the loop
        if (L.empty()) { break; }

        // If no arcs were withdrawn, erase an arbitrary double arc in the graph (the first one in
        // L). Hoping the situation will improve. ┐(￣ヘ￣)┌ If we arrive here, it's because the
        // double-headed arc creates cycles in both directions
        if (!withdrawFlag_L) {
          auto it  = L.begin();
          auto arc = *it;
          mg.eraseArc(Arc(arc.head(), arc.tail()));
          mg.eraseArc(Arc(arc.tail(), arc.head()));
          L.erase(arc);
        }
      }
    }
  }

  MixedGraph MeekRules::_propagates_(const MixedGraph& mg) {
    MixedGraph graph(mg);
    // Propagates existing orientations thanks to Meek rules
    bool newOrientation = true;
    while (newOrientation) {
      newOrientation = false;
      for (NodeId x: graph.nodes()) {
        if (!graph.parents(x).empty()) { newOrientation |= _applyMeekRules_(graph, x); }
      }
    }
    return graph;
  }

  /// Propagates the orientation from a node to its neighbours
  bool MeekRules::_applyMeekRules_(MixedGraph& graph, NodeId xj) {
    bool       res        = false;
    const auto neighbours = graph.neighbours(xj);
    for (auto& xi: neighbours) {
      bool i_j = _isOrientable_(graph, xi, xj);
      bool j_i = _isOrientable_(graph, xj, xi);
      if (i_j || j_i) {
        // GUM_SL_EMIT(xi, xj, "Removing Edge", "line 660")
        graph.eraseEdge(Edge(xi, xj));
        res = true;
      }
      if (i_j) {
        graph.addArc(xi, xj);
        _applyMeekRules_(graph, xj);
      }
      if (j_i) {
        graph.addArc(xj, xi);
        _applyMeekRules_(graph, xi);
      }
      if (i_j && j_i) {
        // GUM_TRACE(" + add arc (" << xi << "," << xj << ")")
        _choices_.emplace_back(xi, xj);
      }
    }
    return res;
  }

  gum::Arc MeekRules::_selectArcToEraseByMinParents_(const gum::MixedGraph& graph,
                                                     gum::NodeId            x,
                                                     gum::NodeId            y) {
    // If the number of parents of x is less than the number of parents of y.
    if (graph.parents(x).size() < graph.parents(y).size()) {
      // If the number of parents of x is less than the number of parents of y.
      // We want to keep y->x, so we return x->y for erasure.
      return {x, y};
    } else if (graph.parents(x).size() > graph.parents(y).size()) {
      return {y, x};
    } else {   // If they have the same number of parents, we choose the one with less neighbours.
      if (graph.neighbours(x).size() < graph.neighbours(y).size()) {
        return {x, y};
      } else {
        return {y, x};
      }
    }
  }

  bool MeekRules::_isOrientable_(const MixedGraph& graph, NodeId xi, NodeId xj) {
    // no cycle
    if (_existsDirectedPath_(graph, xj, xi)) {
      // GUM_TRACE("cycle(" << xi << "-" << xj << ")")
      return false;
    }
    // R1
    if (!(graph.parents(xi) - graph.boundary(xj)).empty()) {
      // GUM_TRACE("R1(" << xi << "-" << xj << ")")
      return true;
    }
    // R2
    if (_existsDirectedPath_(graph, xi, xj)) {
      // GUM_TRACE("R2(" << xi << "-" << xj << ")")
      return true;
    }
    // R3: orient xi-xj if two non-adjacent parents of xj are both direct undirected
    //     neighbors of xi (classical Meek R3: chains xi-k->xj and xi-l->xj, k not adj l)
    {
      std::vector< NodeId > qualifying;
      for (const auto p: graph.parents(xj)) {
        if (graph.neighbours(xi).contains(p)) { qualifying.push_back(p); }
      }
      for (std::size_t i = 0; i < qualifying.size(); ++i)
        for (std::size_t j = i + 1; j < qualifying.size(); ++j)
          if (!graph.boundary(qualifying[i]).contains(qualifying[j])) {
            // GUM_TRACE("R3(" << xi << "-" << xj << ")")
            return true;
          }
    }
    // R4: orient xi-xj if there are xk, xl such that xi-xk, xk->xl, xl->xj, xi is adjacent
    //     to xl and xk is not adjacent to xj (classical Meek R4)
    {
      const auto bi = graph.boundary(xi);
      const auto bj = graph.boundary(xj);
      for (const auto xl: graph.parents(xj)) {
        if (!bi.contains(xl)) continue;
        for (const auto xk: graph.parents(xl)) {
          if (graph.existsEdge(xi, xk) && !bj.contains(xk)) {
            // GUM_TRACE("R4(" << xi << "-" << xj << ")")
            return true;
          }
        }
      }
    }
    return false;
  }

  bool MeekRules::_existsDirectedPath_(const MixedGraph& graph, const NodeId n1, const NodeId n2) {
    // not recursive version => use a FIFO for simulating the recursion
    List< NodeId > nodeFIFO;
    // mark[node] = successor if visited, else mark[node] does not exist
    Set< NodeId > mark;

    mark.insert(n2);
    nodeFIFO.pushBack(n2);

    NodeId current;

    while (!nodeFIFO.empty()) {
      current = nodeFIFO.front();
      nodeFIFO.popFront();

      // check the parents
      for (const auto new_one: graph.parents(current)) {
        if (graph.existsArc(current,
                            new_one))   // if there is a double arc, pass
          continue;

        if (new_one == n1) { return true; }

        if (mark.exists(new_one))   // if this node is already marked, do not
          continue;                 // check it again

        mark.insert(new_one);
        nodeFIFO.pushBack(new_one);
      }
    }

    return false;
  }

}   // namespace gum
