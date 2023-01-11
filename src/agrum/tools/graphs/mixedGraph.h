/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/** @file
 * @brief Base classes for mixed directed/undirected graphs
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_MIXED_GRAPH_H
#define GUM_MIXED_GRAPH_H

#include <iostream>
#include <utility>

#include <agrum/agrum.h>

#include <agrum/tools/graphs/diGraph.h>
#include <agrum/tools/graphs/undiGraph.h>

namespace gum {

  /* ===========================================================================
   */
  /* ===      BASE CLASS FOR MANIPULATING GRAPHS WITH BOTH EDGES AND ARCS    ===
   */
  /* ===========================================================================
   */
  /** @class MixedGraph
   * @brief Base class for mixed graphs
   *
   * \ingroup graph_group
   *
   *
   * @par Usage example:
   * @code
   * // creating empty graphs
   * MixedGraph g1,g2;
   *
   * // adding nodes, arcs and edges to g1
   * NodeId i1=g1.addNode();
   * NodeId i2=g1.addNode();
   * NodeId i3=g1.addNode();
   * g1.addArc( i1,i2 );
   * g1.addArc( i1,i3 );
   * g1.addArc( i2,i3 );
   * g1.addEdge( i1,i2 );
   * g1.addEdge( i1,i3 );
   * g1.addEdge( i2,i3 );
   *
   * //throw an InvalidNode
   * // g1.addArc( i1+i2+i3,i1 );
   * // g1.addEdge( i1+i2+i3,i1 );
   *
   * // copying graphs
   * MixedGraph g3 = g1;
   * g2 = g1;
   * MixedGraph g4=g1;
   *
   * // check if a graph has no node
   * if ( g1.empty() ) cerr << "graph g1 is empty" << endl;
   *
   * // remove all the nodes (as well as their adjacent arcs and edges)
   * g1.clear();
   *
   * // remove some arc
   * g4.eraseArc( Arc ( i1,i3 ) );
   *
   * // remove some edge
   * g4.eraseEdge( Edge ( i1,i3 ) );
   *
   * // remove node
   * g2.eraseNode( i2 );
   *
   * // parse a graph
   * for ( const auto nod : g3.nodes() )
   *   cerr << nod << endl;
   *
   * for ( const auto arc : g3.arcs() )
   *   cerr << arc << endl;
   *
   * for ( const auto edg : g3.edges()) )
   *   cerr << edg << endl;
   *
   * const NodeSet& a=g3.parents( 3 );
   *
   * for ( const auto iter : a)
   *   cerr << "  -  "<<iter;
   *
   * cerr<<endl;
   *
   * const NodeSet& a=g3.neighbours( 3 );
   *
   * for ( NodeSetIterator iter = a.begin( ); iter != a.end(); ++iter )
   *   cerr << "  -  "<<*iter;
   *
   * cerr<<endl;
   *
   * // remove all the arcs that are parent of a given node
   * g3.eraseParents( 2 );
   *
   * // remove all the edges that are adjacent to a given node
   * g3.eraseNeighbours( 2 );
   * @endcode
   */
  /* ===========================================================================
   */

  class MixedGraph: public UndiGraph, public DiGraph {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @param nodes_size the size of the hash table used to store all the nodes
     * @param nodes_resize_policy the resizing policy of this hash table
     * @param arcs_size the size of the hash table used to store all the arcs
     * @param arcs_resize_policy the resizing policy of this hash table
     * @param edges_size the size of the hash table used to store all the edges
     * @param edges_resize_policy the resizing policy of this hash table */
    explicit MixedGraph(Size nodes_size          = HashTableConst::default_size,
                        bool nodes_resize_policy = true,
                        Size arcs_size           = HashTableConst::default_size,
                        bool arcs_resize_policy  = true,
                        Size edges_size          = HashTableConst::default_size,
                        bool edges_resize_policy = true);

    explicit MixedGraph(const UndiGraph& g);

    explicit MixedGraph(const DiGraph& g);

    /// copy constructor
    /** @param g the MixedGraph to copy */
    MixedGraph(const MixedGraph& g);

    /// destructor
    virtual ~MixedGraph();

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    /** @param g the MixedGraph to copy */
    MixedGraph& operator=(const MixedGraph& g);


    /// tests whether two MixedGraphs are identical (same nodes, arcs and edges)
    /** @param g the MixedGraph with which "this" is compared */
    // not virtual : it is a feature !!! :)
    bool operator==(const MixedGraph& g) const;

    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// remove a node as well as its adjacent arcs and edges from the graph
    /** @param id the id of the node to be removed
     * @warning if the node does not exist, nothing is done. In particular, no
     * exception is raised.*/
    void eraseNode(const NodeId node) override;

    /// removes all the nodes, arcs and edges from the graph
    void clear() override;

    /** @brief returns a mixed edge/directed arc path from node1 to node2 in the
     * arc/edge set
     *
     * This function returns, if any, a path from node1 to node2, using edges
     * and/or arcs (wrt the direction of th arcs)
     * @param node1 the id from which the path begins
     * @param node2 the id to which the path ends
     * if no path can be found between the two nodes, the returned vector is empty*/
    std::vector< NodeId > mixedOrientedPath(NodeId node1, NodeId node2) const;

    /** @brief returns true if a mixed edge/directed arc path from node1 to node2 in the
     * arc/edge set exists.
     *
     * @param node1 the id from which the path begins
     * @param node2 the id to which the path ends
     */
    bool hasMixedOrientedPath(NodeId node1, NodeId node2) const;

    /// returns a mixed/directed path from node1 to node2 in the arc/edge set
    /** This function returns, if any, a path from node1 to node2, using edges
     * and/or arcs (not necessarily following the direction of th arcs)
     * @param node1 the id from which the path begins
     * @param node2 the id to which the path ends
     * if no path can be found between the two nodes, the returned vector is empty. */
    std::vector< NodeId > mixedUnorientedPath(NodeId node1, NodeId node2) const;

    /// to friendly display mixed graph in DOT format
    std::string toDot() const override;

    /// to friendly display the content of the MixedGraph
    std::string toString() const override;

    /// returns the set of node adjacent to a given node
    /** Note that the set of node returned may be empty.
     * @param id the node to which the edges are adjacent */
    NodeSet boundary(NodeId node) const;

    /// returns the set of nodes reachable by undirected path
    /** Note that the set of node returned may be empty.
     * @param id the node to which the edges are adjacent */
    NodeSet chainComponent(NodeId node) const;
    /// @}
  };

  /// for friendly displaying the content of directed graphs
  std::ostream& operator<<(std::ostream&, const MixedGraph&);

} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/tools/graphs/mixedGraph_inl.h>
#endif   // GUM_NOINLINE

#endif /* GUM_MIXEDGRAPH_H */
