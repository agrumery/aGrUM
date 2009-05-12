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
 * @brief Base classes for mixed graphs
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 */
#ifndef GUM_MIXED_GRAPH_H
#define GUM_MIXED_GRAPH_H

#include <iostream>
#include <utility>
#include <agrum/core/utils.h>
#include <agrum/graphs/diGraph.h>
#include <agrum/graphs/undiGraph.h>


namespace gum {


  /* ============================================================================ */
  /* ===      BASE CLASS FOR MANIPULATING GRAPHS WITH BOTH EDGES AND ARCS     === */
  /* ============================================================================ */
  /** @class MixedGraph
   * @brief Base class for mixed graphs
   *
   * \ingroup graph_group
   *
   *
   * @par Usage example:
   * @code
   * @endcode
   */
  /* ============================================================================ */
  class MixedGraph : public virtual UndiGraph,public virtual DiGraph {
  public:
    /** @param nodes_size the size of the hash table used to store all the nodes
     * @param nodes_resize_policy the resizing policy of this hash table
     * @param arcs_size the size of the hash table used to store all the arcs
     * @param arcs_resize_policy the resizing policy of this hash table
     * @param edges_size the size of the hash table used to store all the edges
     * @param edges_resize_policy the resizing policy of this hash table */
    explicit MixedGraph( Size nodes_size = GUM_HASHTABLE_DEFAULT_SIZE,
                         bool nodes_resize_policy    = true,
                         Size arcs_size = GUM_HASHTABLE_DEFAULT_SIZE,
                         bool arcs_resize_policy    = true,
                         Size edges_size = GUM_HASHTABLE_DEFAULT_SIZE,
                         bool edges_resize_policy    = true );
    MixedGraph( const MixedGraph& g );
    virtual ~MixedGraph();

    virtual void eraseNode( const NodeId id );
    virtual void clear();
    
    // not virtual : it is a feature !!! :)
    bool operator==( const MixedGraph& g ) const;
    bool operator!=( const MixedGraph& g ) const;
    
    // ============================================================================
    /// returns a oriented mixed path from node1 to node2 in the edge set
    /** @param node1 the id from which the path begins
     * @param node2 the id to which the path ends
     * @throw NotFound
     */
    // ============================================================================
    const std::vector<NodeId> mixedOrientedPath
    ( const NodeId node1, const NodeId node2 ) const;
    
    // ============================================================================
    /// returns aa unoriented directed path from node1 to node2 in the edge set
    /** @param node1 the id from which the path begins
     * @param node2 the id to which the path ends
     * @throw NotFound
     */
    // ============================================================================
    const std::vector<NodeId> mixedUnorientedPath
    ( const NodeId node1, const NodeId node2 ) const;

    virtual const std::string toString() const;
  };


  /// for friendly displaying the content of directed graphs
  std::ostream& operator<< ( std::ostream&, const MixedGraph& );

  
} /* namespace gum */



#ifndef GUM_NO_INLINE
#include <agrum/graphs/mixedGraph.inl>
#endif //GUM_NOINLINE

#endif /* GUM_MIXEDGRAPH_H */
