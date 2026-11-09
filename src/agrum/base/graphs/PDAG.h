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
 * @brief Base classes for partially directed acyclic graphs
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_PDAG_H
#define GUM_PDAG_H

#include <agrum/base/graphs/DAG.h>
#include <agrum/base/graphs/mixedGraph.h>

namespace gum {

  namespace dot {
    class DotOptions;
  }   // namespace dot

  /* =================================================================== */
  /* ===      BASE CLASS FOR MANIPULATING PARTIALLY DIRECTED ACYCLIC GRAPHS */
  /* =================================================================== */
  /** @class PDAG
   * @brief Base class for partially directed acyclic graphs
   *
   * \ingroup graph_group
   *
   *
   * A PDAG is a mixed graph (arcs and undirected edges) with at most one connection (an arc or an
   * edge) between two nodes and without any directed cycle, i.e. a cycle of at least 3 distinct
   * nodes made of arcs only, followed in their direction. A cycle mixing arcs and edges (a
   * partially directed cycle) is allowed: a PDAG without partially directed cycle is a chain graph
   * (see gum::ChainGraph). This is the definition used for the orientation of partially directed
   * graphs (Meek 1995; Chickering 2002; Wienöbst et al. 2021, Sec. 2): the closure of a PDAG under
   * the Meek rules is a PDAG, not always a chain graph.
   *
   * addArc and addEdge check these properties and throw if they would be violated.
   *
   * @warning The assignment operators are not virtual: a PDAG assigned through a reference to
   * gum::MixedGraph is not checked. The same holds for a gum::ChainGraph assigned through a
   * reference to gum::PDAG or gum::MixedGraph. Use the checked constructors to convert a graph.
   *
   * @par References
   * - C. Meek. Causal Inference and Causal Explanation with Background Knowledge. Proceedings of
   *   the 11th Conference on Uncertainty in Artificial Intelligence (UAI 1995), pp. 403-410.
   *   arXiv:1302.4972.
   * - D. M. Chickering. Optimal Structure Identification With Greedy Search. Journal of Machine
   *   Learning Research 3:507-554, 2002.
   * - M. Wienöbst, M. Bannach, M. Liśkiewicz. Extendability of causal graphical models: Algorithms
   *   and computational complexity. Proceedings of the 37th Conference on Uncertainty in Artificial
   *   Intelligence, PMLR 161:1248-1257, 2021.
   *
   * @par Code example
   * @code
   * // creating empty graphs
   * gum::PDAG g1,g2;
   *
   * // adding nodes and arcs to g1
   * gum::NodeId i1=g1.addNode();
   * gum::NodeId i2=g1.addNode();
   * gum::NodeId i3=g1.addNode();
   * g1.addArc( i1,i2 );
   * g1.addArc( i1,i3 );
   * g1.addEdge( i2,i3 );
   *
   * //throw an InvalidNode
   * // g1.addArc( i1+i2+i3,i1 );
   *
   * // throw an InvalidDirectedCycle
   * // g1.addArc( i3,i1 );
   *
   * // copying graphs
   * gum::PDAG g3 = g1;
   * g2 = g1;
   * gum::PDAG g4=g1;
   *
   * // check if a graph has no node
   * if ( g1.empty() ) cerr << "graph g1 is empty" << endl;
   *
   * // remove all the nodes (as well as their adjacent arcs)
   * g1.clear();
   *
   * // remove some arc
   * g4.eraseArc( Arc ( i1,i3 ) );
   *
   * // remove some edge
   * g4.eraseEdge( Edge ( i2,i3 ) );
   *
   * // remove node
   * g2.eraseNode( i2 );
   *
   * // parse a graph
   * for ( const auto node : g3.nodes() ) // type of node = gum::NodeId
   *   cerr << node << endl;
   *
   * for ( const auto& arc : g3.arcs()) // type of arc : gum::Arc&
   *   cerr << arc << endl;
   *
   * for ( const auto& edge : g3.edges()) // type of edge : gum::Edge&
   *   cerr << edge << endl;
   *
   * for ( const auto node :g3.parents( gum::NodeId(3) ))
   *   cerr << "  -  "<<node;
   *
   * for ( const auto node :g3.neighbours( gum::NodeId(3) ))
   *   cerr << "  -  "<<node;
   *
   * cerr<<endl;
   *
   * // remove all the arcs that are parent of a given node
   * g3.eraseParents( gum::NodeId(2) );
   *
   * @endcode
   */
  /* ====================================================================== */
  class GUM_SHARED_PUBLIC PDAG: public MixedGraph {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /**
     * @param nodes_size the size of the hash table used to store all the nodes
     * @param nodes_resize_policy the resizing policy of this hash table
     * @param arcs_size the size of the hash table used to store all the arcs
     * @param arcs_resize_policy the resizing policy of this hash table.
     */
    PDAG(Size nodes_size          = HashTableConst::default_size,
         bool nodes_resize_policy = true,
         Size arcs_size           = HashTableConst::default_size,
         bool arcs_resize_policy  = true,
         Size edges_size          = HashTableConst::default_size,
         bool edges_resize_policy = true);

    /// copy of an undirected graph
    /** @throw InvalidDirectedCycle if g contains an edge from a node to itself */
    explicit PDAG(const UndiGraph& g);
    explicit PDAG(const DAG& g);
    /// copy of a mixed graph
    /** @throw InvalidDirectedCycle if g contains a directed cycle or an edge from a node to itself
     * @throw InvalidArc if a pair of nodes of g is connected by an arc and an edge */
    explicit PDAG(const MixedGraph& g);

    /// copy constructor
    /** @param g the PDAG to copy */
    PDAG(const PDAG& g);

    /// move constructor
    PDAG(PDAG&& g);

    /// destructor
    ~PDAG() override;

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    /** @param g the PDAG to copy */
    PDAG& operator=(const PDAG& g);

    /// move assignment operator
    PDAG& operator=(PDAG&& g);

    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// insert a new arc into the directed graph
    /**
     * @param tail the id of the tail of the new inserted arc
     * @param head the id of the head of the new inserted arc
     * @warning if the arc already exists, nothing is done. In particular, no
     * exception is raised.
     * @throw InvalidNode if head or tail does not belong to the graph nodes
     * @throw InvalidArc if an edge already connects tail and head
     * @throw InvalidDirectedCycle if the arc creates a directed cycle (head == tail included)
     * @warning addArc searches for a directed path from head to tail: it is not in constant time.
     */
    void addArc(NodeId tail, NodeId head) override;
    /// @}

    /// insert a new edge into the partially directed graph
    /**
     * @param first the id of the first node of the new inserted edge
     * @param second the id of the second node of the new inserted edge
     * @warning if the edge already exists, nothing is done. In particular, no
     * exception is raised.
     * @throw InvalidNode if first or second does not belong to the graph nodes
     * @throw InvalidEdge if an arc already connects first and second
     * @throw InvalidDirectedCycle if first == second
     */
    void addEdge(NodeId first, NodeId second) override;
    /// @}

    /** build a UndiGraph by moralizing the PDAG
     *
     * @return the moralized graph
     * @deprecated deprecated after 3.3.2: the moralization (LWF) is defined for chain graphs, use
     * ChainGraph::moralGraph.
     */
    [[deprecated("PDAG::moralGraph is deprecated after 3.3.2, use ChainGraph::moralGraph")]]
    UndiGraph moralGraph() const;

    /** build a UndiGraph by moralizing the Ancestral Graph of a set of Nodes
     *
     * @param nodes the set of nodeId
     * @return the moralized ancestral graph
     * @deprecated deprecated after 3.3.2: the moralization (LWF) is defined for chain graphs, use
     * ChainGraph::moralizedAncestralGraph.
     */
    [[deprecated("PDAG::moralizedAncestralGraph is deprecated after 3.3.2, use "
                 "ChainGraph::moralizedAncestralGraph")]]
    UndiGraph moralizedAncestralGraph(const NodeSet& nodes) const;

    /** @brief returns true if a walk from n1 to n2 exists, following the arcs in their direction
     * and the edges in either direction, with at least one arc
     *
     * A walk may visit a node several times: with a partially directed cycle, allowed in a PDAG,
     * the result may be true without any path (visiting each node once) having an arc.
     *
     * @param n1 the id from which the walk begins
     * @param n2 the id to which the walk ends
     * @deprecated deprecated after 3.3.2: exact for chain graphs only, use
     * ChainGraph::hasMixedReallyOrientedPath.
     */
    [[deprecated("PDAG::hasMixedReallyOrientedPath is deprecated after 3.3.2, use "
                 "ChainGraph::hasMixedReallyOrientedPath")]]
    bool hasMixedReallyOrientedPath(NodeId n1, NodeId n2) const;

    /** check if node X and node Y are independent given nodes Z (in the sense of
     * c-separation)
     *
     * @deprecated deprecated after 3.3.2: the c-separation is defined for chain graphs, use
     * ChainGraph::cSeparation.
     */
    [[deprecated("PDAG::cSeparation is deprecated after 3.3.2, use ChainGraph::cSeparation")]]
    bool cSeparation(NodeId X, NodeId Y, const NodeSet& Z) const;

    /** check if nodes X and nodes  Y are independent given Z (in the sense of
     * c-separation)
     *
     * @deprecated deprecated after 3.3.2: the c-separation is defined for chain graphs, use
     * ChainGraph::cSeparation.
     */
    [[deprecated("PDAG::cSeparation is deprecated after 3.3.2, use ChainGraph::cSeparation")]]
    bool cSeparation(const NodeSet& X, const NodeSet& Y, const NodeSet& Z) const;
  };

} /* namespace gum */

static_assert(gum::GUM_MixedGraphable< gum::PDAG >);

#ifndef GUM_NO_INLINE
#  include <agrum/base/graphs/PDAG_inl.h>
#endif   // GUM_NO_INLINE

#endif   /* GUM_PDAG_H */
