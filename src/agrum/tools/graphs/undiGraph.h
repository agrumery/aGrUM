/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Base classes for undirected graphs
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_UNDIGRAPH_H
#define GUM_UNDIGRAPH_H

#include <iostream>
#include <utility>

#include <agrum/agrum.h>

#include <agrum/tools/graphs/parts/edgeGraphPart.h>
#include <agrum/tools/graphs/parts/nodeGraphPart.h>

namespace gum {

  /* ===========================================================================
   */
  /* ===          BASE CLASS FOR MANIPULATING ALL UNDIRECTED GRAPHS          ===
   */
  /* ===========================================================================
   */
  /** @class UndiGraph
   * @brief Base class for undirected graphs
   *
   * \ingroup graph_group
   *
   *
   * This is the base class for graphs containing undirected edges
   * (so-called edges).
   *
   * @par Usage example:
   * @code
   * // creating empty graphs
   * UndiGraph g1,g2;
   *
   * // adding nodes and edges to g1
   * NodeId i1=g1.addNode();
   * NodeId i2=g1.addNode();
   * NodeId i3=g1.addNode();
   * g1.addEdge( i1,i2 );
   * g1.addEdge( i1,i3 );
   * g1.addEdge( i2,i3 );
   *
   * //throw an InvalidNode
   * // g1.addEdge( i1+i2+i3,i1 );
   *
   * // copying graphs
   * UndiGraph g3 = g1;
   * g2 = g1;
   * UndiGraph g4=g1;
   *
   * // check if a graph has no node
   * if ( g1.empty() ) cerr << "graph g1 is empty" << endl;
   *
   * // remove all the nodes (as well as their adjacent edges)
   * g1.clear();
   *
   * // remove some edge
   * g4.eraseEdge( Edge ( i1,i3 ) );
   *
   * // remove node
   * g2.eraseNode( i2 );
   *
   * // parse a graph
   * for ( const auto node : g3.nodes() )
   *   cerr << node << endl;
   *
   * for ( const auto& edge : g3.edges())
   *   cerr << edge << endl;
   *
   * const EdgeSet& a=g3.neighbours( 3 );
   * for ( const auto& edge : a)
   *   cerr << "  -  "<<edge;
   *
   * cerr<<endl;
   *
   * // remove all the edges that are adjacent to a given node
   * g3.eraseNeighbours( 2 );
   * @endcode
   */
  /* ===========================================================================
   */

  class UndiGraph: public virtual NodeGraphPart, public EdgeGraphPart {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @param nodes_size the size of the hash table used to store all the nodes
     * @param nodes_resize_policy the resizing policy of this hash table
     * @param edges_size the size of the hash table used to store all the edges
     * @param edges_resize_policy the resizing policy of this hash table */
    explicit UndiGraph(Size nodes_size          = HashTableConst::default_size,
                       bool nodes_resize_policy = true,
                       Size edges_size          = HashTableConst::default_size,
                       bool edges_resize_policy = true);

    /// copy constructor
    /** @param g the UndiGraph to copy */
    UndiGraph(const UndiGraph& g);

    /// destructor
    virtual ~UndiGraph();

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    /** @param g the DiGraph to copy */
    UndiGraph& operator=(const UndiGraph& g);

    /// tests whether two UndiGraphs are identical (same nodes, same edges)
    /** @param g the UndiGraph with which "this" is compared */
    // not virtual : it is a feature !!! :)
    bool operator==(const UndiGraph& g) const;

    /// tests whether two UndiGraphs are different
    /** @param g the UndiGraph with which "this" is compared */
    // not virtual : it is a feature !!! :)
    bool operator!=(const UndiGraph& g) const;

    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// insert a new edge into the undirected graph
    /** The order in which the extremal nodes are specified is not important.
     * @param first the id of one extremal node of the new inserted edge
     * @param second the id of the other extremal node of the new inserted edge
     * @warning if the edge already exists, nothing is done. In particular, no
     * exception is raised.
     * @throw InvalidNode if first and/or second do not belong to the
     * graph nodes */
    void addEdge(NodeId first, NodeId second) override;

    /// remove a node and its adjacent edges from the graph
    /** @param id the id of the node to be removed
     * @warning if the node does not exist, nothing is done. In particular, no
     * exception is raised.*/
    void eraseNode(NodeId id) override;

    /// removes all the nodes and edges from the graph
    void clear() override;

    /// to friendly display the content of the graph
    std::string toString() const override;

    /// to friendly display graph in DOT format
    virtual std::string toDot() const;

    /// checks whether the graph contains cycles
    bool hasUndirectedCycle() const;

    /// returns the partial graph formed by the nodes given in parameter
    virtual UndiGraph partialUndiGraph(NodeSet nodes);

    /// returns a property {node:id of connected component}
    NodeProperty< NodeId > nodes2ConnectedComponent() const;

    /// @}
  };

  /// for friendly displaying the content of undirected graphs
  std::ostream& operator<<(std::ostream&, const UndiGraph&);

} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/tools/graphs/undiGraph_inl.h>
#endif   // GUM_NOINLINE

#endif /* GUM_UNDIGRAPH_H */
