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
 * @brief Base classes for dag
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 * @par exemple de code
 * @code
 * DAG g1;
 * // adding nodes, edges and arcs to g1
 *
 * NodeId i1=g1.insertNode();
 * NodeId i2=g1.insertNode();
 * NodeId i3=g1.insertNode();
 *
 * g1.insertArc( i1,i2 );
 * g1.insertArc( i1,i3 );
 * g1.insertArc( i2,i3 );
 *
 * @endcode
 */
#ifndef GUM_DAG_H
#define GUM_DAG_H

#include <agrum/graphs/diGraph.h>


namespace gum {


  /* =========================================================================== */
  /* ===      BASE CLASS FOR MANIPULATING GRAPHS WITH BOTH EDGES AND ARCS    === */
  /* =========================================================================== */
  /** @class DAG
   * @brief Base class for dag
   *
   * \ingroup graph_group
   *
   *
   * This is the base class for Directed Acyclic Graph : insertArc may throw a
   * Circuit if any (directed) cycle is created by this arc.
   */
  /* =========================================================================== */
  class DAG : public DiGraph {
  public:
    /** @param nodes_size the size of the hash table used to store all the nodes
     * @param nodes_resize_policy the resizing policy of this hash table
     * @param arcs_size the size of the hash table used to store all the arcs
     * @param arcs_resize_policy the resizing policy of this hash table */
    explicit DAG( Size nodes_size = GUM_HASHTABLE_DEFAULT_SIZE,
                  bool nodes_resize_policy    = true,
                  Size arcs_size = GUM_HASHTABLE_DEFAULT_SIZE,
                  bool arcs_resize_policy    = true );
    DAG( const DAG& g );
    DAG& operator=( const DAG& g );

    virtual ~DAG();
    
    /// @throw Circuit if any (directed) cycle is created by this arc.
    /// @warning Unfortunately, this means that insertNode is not in constant
    /// time anymore.
    virtual void insertArc( const NodeId& tail,const NodeId& head );


  private:
    bool hasDirectedPath( const NodeId& from, const NodeId& to );
  };


} /* namespace gum */


#ifndef GUM_NO_INLINE
#include <agrum/graphs/DAG.inl>
#endif //GUM_NOINLINE


#endif /* GUM_DAG_H */
