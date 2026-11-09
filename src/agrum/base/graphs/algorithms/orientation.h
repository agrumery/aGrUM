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
 * @brief Orientation algorithms for partially directed graphs: consistent extension of a PDAG,
 * completed PDAG (CPDAG) of a DAG and closure of a PDAG under the Meek rules.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_GRAPH_ORIENTATION_H
#define GUM_GRAPH_ORIENTATION_H

#include <optional>

#include <agrum/agrum.h>

#include <agrum/base/graphs/chainGraph.h>

namespace gum::graph {

  /**
   * @brief Degeneracy (smallest-last) ordering of the skeleton of @p g.
   *
   * Every node has at most d neighbours (arcs and edges, whatever their orientation) before it in
   * the returned sequence, where d is the degeneracy of the skeleton, the smallest such bound
   * (Matula and Beck 1983). O(n + m).
   *
   * @par References
   * - D. W. Matula, L. L. Beck. Smallest-last ordering and clustering and graph coloring
   * algorithms. Journal of the ACM 30(3):417-427, 1983. doi:10.1145/2402.322385.
   */
  GUM_SHARED_PUBLIC Sequence< NodeId > degeneracyOrdering(const MixedGraph& g);

  /**
   * @brief A consistent extension of the PDAG @p pdag, if any.
   *
   * A consistent extension of a PDAG is a DAG with the same skeleton and the same v-structures (an
   * induced x->y<-z, x and z non adjacent) that contains all its arcs: its undirected edges are
   * oriented without creating any directed cycle nor any new v-structure (Dor and Tarsi 1992). The
   * PDAG is extendable iff it has one.
   *
   * Algorithm of Wienöbst, Bannach and Liśkiewicz (2021, Lemma 4.7 and Theorem 4.6): the nodes are
   * removed one by one, each one being a potential sink -- a node without child whose undirected
   * neighbours form a clique and are adjacent to all its parents (Dor and Tarsi 1992) -- and all
   * its edges are oriented towards it. Potential sinks are found in constant time from two counters
   * per node, updated at each insertion or deletion of an edge; the edges are inserted following a
   * degeneracy ordering of the skeleton. With hash tables for the adjacencies (the variant proposed
   * by the authors for linear space), the time is in O(n + dm) expected, d being the degeneracy of
   * the skeleton. Each connection {u, v} is inserted once and deleted at most once, and each of
   * these updates scans the adjacency of the endpoint with the fewest adjacent nodes (to find the
   * triangles containing {u, v}), with constant expected time per scanned node: since the degrees
   * never exceed those of the skeleton, the total cost is O(sum over {u, v} of min(deg(u),
   * deg(v))). Bounding min(deg(u), deg(v)) by the degree of the endpoint that comes later in a
   * degeneracy ordering, each node is counted for at most d connections (its earlier adjacent
   * nodes): the sum is at most d times the sum of the degrees, i.e. 2dm. This is Lemma 2 of Chiba
   * and Nishizeki (1985), stated with the arboricity a(G) <= d and proved with spanning forests
   * instead of a degeneracy ordering. Hence the bound holds without the dedicated pop-ps operation
   * of the authors, whatever the number of parents of the sinks.
   *
   * @return the consistent extension, or std::nullopt if @p pdag is not extendable
   *
   * @par References
   * - D. Dor, M. Tarsi. A simple algorithm to construct a consistent extension of a partially
   *   oriented graph. UCLA Cognitive Systems Laboratory, Technical Report R-185, October 1992.
   * - M. Wienöbst, M. Bannach, M. Liśkiewicz. Extendability of causal graphical models: Algorithms
   *   and computational complexity. Proceedings of the 37th Conference on Uncertainty in Artificial
   *   Intelligence, PMLR 161:1248-1257, 2021.
   * - N. Chiba, T. Nishizeki. Arboricity and subgraph listing algorithms. SIAM Journal on Computing
   *   14(1):210-223, 1985.
   */
  GUM_SHARED_PUBLIC std::optional< DAG > consistentExtension(const PDAG& pdag);

  /**
   * @brief A DAG orienting the PDAG @p pdag with small conditional probability tables: a consistent
   * extension if @p pdag is extendable, with a minimal total size of the CPTs (sum over the nodes
   * of the product of the domain sizes of the node and of its parents) when it is computed exactly.
   *
   * <b>Exact case.</b> If @p pdag is extendable and if, in its Meek closure (meekClosure()), all
   * the nodes of an undirected component have the same parents -- always the case for a CPDAG, a
   * chain graph without induced a->b-c (Andersson, Madigan and Perlman 1997) -- the components are
   * oriented independently, the cost
   * of a component being the product of the domain sizes of its common parents times the total
   * size of its CPTs restricted to its own nodes. Each component is a connected chordal graph whose
   * consistent orientations are its orientations without v-structure, minimized by the recursion
   * of the Clique-Picking algorithm (Wienöbst, Bannach and Liśkiewicz 2021, 2023): every such
   * orientation starts with a maximal clique K; once K is placed first, the remaining undirected
   * components of the closure are chordal, their nodes have common parents, and they are oriented
   * independently. Hence cost(H) = min over K of [cost of K + sum over these components C of
   * (product of the domain sizes of the parents of C in H) * cost(C)], memoized on the node sets
   * (at most 2|Π(H)| - 1 distinct components, |Π(H)| <= |H| being the number of maximal cliques).
   * The overlaps between the orientations reached from several cliques, which the counting
   * algorithm has to correct, do not matter for a minimum; the nodes of K are ordered by increasing
   * domain size (then decreasing id), which minimizes the sum of the prefix products of their
   * domain sizes (exchange argument). For a component H with n_H nodes and m_H edges, each of the
   * at most 2|Π(H)| memoized components tries at most |Π(H)| cliques, each try costing a Meek
   * closure (see meekClosure()) and O(n_H + d m_H) for the rest: polynomial in the size of H.
   * The components with more nodes than the key @c orientAsDAGExactMaxSize of the section
   * @c graph of the configuration (100 by default) are oriented by the greedy heuristic below.
   *
   * <b>Greedy heuristic</b> (larger components, or a Meek closure without common parents in a
   * component, e.g. with background knowledge). Same algorithm as consistentExtension(const PDAG&),
   * but the potential sink removed at each step is the one with the smallest CPT in the resulting
   * DAG: the product of its domain size and of the domain sizes of its remaining adjacent nodes,
   * which all become its parents. Ties are broken in favour of the largest domain (the smallest
   * domains remain parents), then of the smallest id. Removing any potential sink of an extendable
   * PDAG leaves an extendable PDAG (Dor and Tarsi 1992): the result is then a consistent extension,
   * but its total size of the CPTs is not guaranteed to be minimal. O(n + dm + dn log n) expected:
   * a node is queued only while it is a potential sink, and it remains one until its removal; it is
   * queued when it becomes one, then each time one of its undirected neighbours is removed -- they
   * form a clique with it, so there are at most d of them -- hence at most d + 1 entries per node
   * in the priority queue, each one computing the size of its CPT in O(deg).
   *
   * <b>Not extendable.</b> When no potential sink remains, the node without child with the smallest
   * CPT is removed as if it were one: its edges are oriented towards it, which creates new
   * v-structures. The result is always a DAG containing the arcs of @p pdag: the parents of a node
   * are removed after it. Each such step adds O(n + m) (scan of the nodes without child), hence
   * O(n(n + m)) in the worst case.
   *
   * @par References
   * - S. A. Andersson, D. Madigan, M. D. Perlman. A characterization of Markov equivalence classes
   *   for acyclic digraphs. The Annals of Statistics 25(2):505-541, 1997.
   * - D. Dor, M. Tarsi. A simple algorithm to construct a consistent extension of a partially
   *   oriented graph. UCLA Cognitive Systems Laboratory, Technical Report R-185, October 1992.
   * - M. Wienöbst, M. Bannach, M. Liśkiewicz. Polynomial-time algorithms for counting and sampling
   *   Markov equivalent DAGs. Proceedings of the 35th AAAI Conference on Artificial Intelligence
   *   35(13):12198-12206, 2021.
   * - M. Wienöbst, M. Bannach, M. Liśkiewicz. Polynomial-time algorithms for counting and sampling
   *   Markov equivalent DAGs with applications. Journal of Machine Learning Research 24(213):1-45,
   *   2023.
   *
   * @param pdag the PDAG to orient
   * @param domainSizes the domain size of each node of @p pdag
   * @throw InvalidArgument if a node of @p pdag has no domain size, or a domain size 0
   */
  GUM_SHARED_PUBLIC DAG orientAsDAG(const PDAG& pdag, const NodeProperty< Size >& domainSizes);

  namespace detail {
    /**
     * @brief graph::orientAsDAG for a PDAG which already is the Meek closure of an extendable PDAG
     * (as computed by MeekRules::propagateToDAG): the closure is not computed again.
     *
     * @throw InvalidArgument if a node of @p closure has no domain size, or a domain size 0
     */
    GUM_SHARED_PUBLIC DAG orientClosureAsDAG(const PDAG&                 closure,
                                             const NodeProperty< Size >& domainSizes);
  }   // namespace detail

  /**
   * @brief The completed PDAG (CPDAG, or essential graph) of the Markov equivalence class of
   * @p dag.
   *
   * An arc of @p dag is compelled if it has the same orientation in every DAG of its Markov
   * equivalence class, reversible otherwise; the CPDAG keeps the compelled arcs and turns the
   * reversible ones into undirected edges. It is a chain graph whose chain components are chordal
   * (Andersson, Madigan and Perlman 1997).
   *
   * Algorithm Find-Compelled of Chickering (1995, Sec. 4.1): the arcs are labelled in the order
   * given by Order-Edges (the heads by increasing topological rank, then, for a given head, the
   * tails by decreasing topological rank). O(n + m) expected (hash adjacency tests).
   *
   * @par References
   * - D. M. Chickering. A Transformational Characterization of Equivalent Bayesian Network
   *   Structures. Proceedings of the 11th Conference on Uncertainty in Artificial Intelligence
   *   (UAI 1995), pp. 87-98. arXiv:1302.4938.
   * - S. A. Andersson, D. Madigan, M. D. Perlman. A characterization of Markov equivalence classes
   *   for acyclic digraphs. The Annals of Statistics 25(2):505-541, 1997.
   *   doi:10.1214/aos/1031833662.
   */
  GUM_SHARED_PUBLIC ChainGraph cpdag(const DAG& dag);

  /**
   * @brief The closure of the PDAG @p pdag under the orientation rules R1-R4 of Meek, if @p pdag is
   * extendable.
   *
   * The rules orient an undirected edge whenever its other orientation would create a directed
   * cycle or a new v-structure in every extension. The closure of an extendable PDAG is unique and
   * is its maximally oriented PDAG (MPDAG): the arcs shared by all its consistent extensions (Meek
   * 1995). If the arcs of @p pdag are the arcs of its v-structures, the closure is the CPDAG of its
   * extensions.
   *
   * Algorithm 2 of Wienöbst, Bannach and Liśkiewicz (2021, Theorem 7.1): from a consistent
   * extension D, the closure starts from the CPDAG of D, whose undirected edges get the
   * orientation of @p pdag; the nodes are then visited once in a topological order of D, and the
   * undirected edges into the current node are oriented by R1, then R4, then R2 (from the closest
   * tail to the farthest one), R3 never applying inside a chain component of a CPDAG. After the
   * extension, with d the degeneracy of the skeleton (expected times, hash adjacency tests): R1 and
   * R2 cost O(dm) in all (the undirected edges into a node from before it form a clique, so they
   * are at most d), R4 at most O(d * sum over the nodes x of |pa(x)| * |ch(x)|) (parents and
   * children in the closure).
   *
   * @return the closure, or std::nullopt if @p pdag is not extendable (its closure is then not
   * unique)
   *
   * @par References
   * - C. Meek. Causal inference and causal explanation with background knowledge. Proceedings of
   *   the 11th Conference on Uncertainty in Artificial Intelligence (UAI 1995), pp. 403-410.
   *   arXiv:1302.4972.
   * - M. Wienöbst, M. Bannach, M. Liśkiewicz. Extendability of causal graphical models: Algorithms
   *   and computational complexity. Proceedings of the 37th Conference on Uncertainty in Artificial
   *   Intelligence, PMLR 161:1248-1257, 2021.
   */
  GUM_SHARED_PUBLIC std::optional< PDAG > meekClosure(const PDAG& pdag);

  /**
   * @brief The closure of the PDAG @p pdag under the orientation rules of Meek, from one of its
   * consistent extensions.
   *
   * Same closure as meekClosure(const PDAG&), for a caller which already holds a consistent
   * extension of @p pdag (e.g. to check or store it): the extension is not computed twice.
   *
   * @pre @p extension is a consistent extension of @p pdag (see consistentExtension): this is not
   * checked, and the result is meaningless otherwise.
   */
  GUM_SHARED_PUBLIC PDAG meekClosure(const PDAG& pdag, const DAG& extension);

}   // namespace gum::graph

#endif   // GUM_GRAPH_ORIENTATION_H
