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
 * @brief Base classes for undirected graphs
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 */
#ifndef GUM_UNDIGRAPH_H
#define GUM_UNDIGRAPH_H

#include <iostream>
#include <utility>
#include <agrum/core/utils.h>
#include <agrum/graphs/nodeGraphPart.h>
#include <agrum/graphs/edgeGraphPart.h>


namespace gum {


  /* =========================================================================== */
  /* ===     BASE CLASS FOR MANIPULATING GRAPHS WITH BOTH EDGES AND ARCS     === */
  /* =========================================================================== */
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
   *
   * @endcode
   */
  /* =========================================================================== */
  class UndiGraph : public virtual NodeGraphPart,public EdgeGraphPart {
  public:
    /** @param nodes_size the size of the hash table used to store all the nodes
     * @param nodes_resize_policy the resizing policy of this hash table
     * @param edges_size the size of the hash table used to store all the edges
     * @param edges_resize_policy the resizing policy of this hash table */
    explicit UndiGraph( Size nodes_size = GUM_HASHTABLE_DEFAULT_SIZE,
                        bool nodes_resize_policy    = true,
                        Size edges_size = GUM_HASHTABLE_DEFAULT_SIZE,
                        bool edges_resize_policy    = true );
    UndiGraph( const UndiGraph& g );
	UndiGraph& operator=(const UndiGraph& g );
    virtual ~UndiGraph();
    
    /// @throw InvalidNode if head or tail does not belong to the graph nodes
    virtual void insertEdge( const Edge& edge );

    /// @throw InvalidNode if head or tail does not belong to the graph nodes
    virtual void insertEdge( const NodeId& first,const NodeId& second );
    virtual void eraseNode( const NodeId id );
    virtual void clear();

    // not virtual : it is a feature !!! :)
    bool operator==( const UndiGraph& g ) const;
    bool operator!=( const UndiGraph& g ) const;
    bool hasUndirectedCycle() const;

    virtual const std::string toString() const;
  };


  /// for friendly displaying the content of undirected graphs
  std::ostream& operator<< ( std::ostream&, const UndiGraph& );

  
} /* namespace gum */


#ifndef GUM_NO_INLINE
#include <agrum/graphs/undiGraph.inl>
#endif //GUM_NOINLINE

#endif /* GUM_UNDIGRAPH_H */
