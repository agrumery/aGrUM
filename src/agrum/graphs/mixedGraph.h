/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief Base classes for mixed directed/undirected graphs
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 */
#ifndef GUM_MIXED_GRAPH_H
#define GUM_MIXED_GRAPH_H

#include <iostream>
#include <utility>

#include <agrum/config.h>

#include <agrum/graphs/diGraph.h>
#include <agrum/graphs/undiGraph.h>

namespace gum {

  /* =========================================================================== */
  /* ===      BASE CLASS FOR MANIPULATING GRAPHS WITH BOTH EDGES AND ARCS    === */
  /* =========================================================================== */
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
  /* =========================================================================== */

  class MixedGraph : public virtual UndiGraph, public virtual DiGraph {
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
    explicit MixedGraph(Size nodes_size = HashTableConst::default_size,
                        bool nodes_resize_policy = true,
                        Size arcs_size = HashTableConst::default_size,
                        bool arcs_resize_policy = true,
                        Size edges_size = HashTableConst::default_size,
                        bool edges_resize_policy = true);

    /// copy constructor
    /** @param g the MixedGraph to copy */
    MixedGraph(const MixedGraph &g);

    /// destructor
    virtual ~MixedGraph();

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    /** @param g the MixedGraph to copy */
    MixedGraph &operator=(const MixedGraph &g);

    /// tests whether two MixedGraphs are identical (same nodes, arcs and edges)
    /** @param g the MixedGraph with which "this" is compared */
    // not virtual : it is a feature !!! :)
    bool operator==(const MixedGraph &g) const;

    /// tests whether two MixedGraphs are different
    /** @param g the MixedGraph with which "this" is compared */
    // not virtual : it is a feature !!! :)
    bool operator!=(const MixedGraph &g) const;

    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// remove a node as well as its adjacent arcs and edges from the graph
    /** @param id the id of the node to be removed
     * @warning if the node does not exist, nothing is done. In particular, no
     * exception is raised.*/
    virtual void eraseNode(const NodeId id);

    /// removes all the nodes, arcs and edges from the graph
    virtual void clear();

    /** @brief returns a mixed edge/directed arc path from node1 to node2 in the
     * arc/edge set
     *
     * This function returns, if any, a path from node1 to node2, using edges
     * and/or arcs (following the direction of th arcs)
     * @param node1 the id from which the path begins
     * @param node2 the id to which the path ends
     * @throw NotFound exception is raised if no path can be found between the
     * two nodes */
    const std::vector<NodeId> mixedOrientedPath(const NodeId node1,
                                                const NodeId node2) const;

    /// returns a mixed/directed path from node1 to node2 in the arc/edge set
    /** This function returns, if any, a path from node1 to node2, using edges
     * and/or arcs (not necessarily following the direction of th arcs)
     * @param node1 the id from which the path begins
     * @param node2 the id to which the path ends
     * @throw NotFound exception is raised if no path can be found between the
     * two nodes */
    const std::vector<NodeId> mixedUnorientedPath(const NodeId node1,
                                                  const NodeId node2) const;

    /// to friendly display the content of the MixedGraph
    virtual const std::string toString() const;

    /// @}
  };

  /// for friendly displaying the content of directed graphs
  std::ostream &operator<<(std::ostream &, const MixedGraph &);

} /* namespace gum */

#ifndef GUM_NO_INLINE
#include <agrum/graphs/mixedGraph.inl>
#endif // GUM_NOINLINE

#endif /* GUM_MIXEDGRAPH_H */
