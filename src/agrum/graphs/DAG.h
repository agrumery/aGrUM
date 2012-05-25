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
 * @brief Base classes for directed acyclic graphs
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
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
   *
   * @par exemple de code
   * @code
   * // creating empty graphs
   * DAG g1,g2;
   *
   * // adding nodes and arcs to g1
   * NodeId i1=g1.insertNode();
   * NodeId i2=g1.insertNode();
   * NodeId i3=g1.insertNode();
   * g1.insertArc( i1,i2 );
   * g1.insertArc( i1,i3 );
   * g1.insertArc( i2,i3 );
   *
   * //throw an InvalidNode
   * // g1.insertArc( i1+i2+i3,i1 );
   *
   * // throw an InvalidCircuit
   * // g1.insertArc( i3,i1 );
   *
   * // copying graphs
   * DAG g3 = g1;
   * g2 = g1;
   * DAG g4=g1;
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
   * // remove node
   * g2.eraseNode( i2 );
   *
   * // parse a graph
   * for ( NodeGraphPart::iterator iter = g3.beginNodes();
   *       iter != g3.endNodes(); ++iter )
   *   cerr << *iter << endl;
   *
   * for ( ArcGraphPart::iterator iter = g3.beginArcs();
   *       iter != g3.endArcs(); ++iter )
   *   cerr << *iter << endl;
   *
   * const NodeSet& a=g3.parents( 3 );
   *
   * for ( NodeSetIterator iter = a.begin( ); iter != a.end(); ++iter )
   *   cerr << "  -  "<<*iter;
   *
   * cerr<<endl;
   *
   * // remove all the arcs that are parent of a given node
   * g3.eraseParents( 2 );
   *
   * @endcode
   */
  /* =========================================================================== */
  class DAG : public DiGraph {
  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @param nodes_size the size of the hash table used to store all the nodes
     * @param nodes_resize_policy the resizing policy of this hash table
     * @param arcs_size the size of the hash table used to store all the arcs
     * @param arcs_resize_policy the resizing policy of this hash table */
    explicit DAG( Size nodes_size = GUM_HASHTABLE_DEFAULT_SIZE,
                  bool nodes_resize_policy    = true,
                  Size arcs_size = GUM_HASHTABLE_DEFAULT_SIZE,
                  bool arcs_resize_policy    = true );

    /// copy constructor
    /** @param g the DAG to copy */
    DAG( const DAG& g );

    /// destructor
    virtual ~DAG();
    
    /// @}

    
    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    /** @param g the DAG to copy */
    DAG& operator=( const DAG& g );

    /// @}

    
    
    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// insert a new arc into the directed graph
    /** @param tail the id of the tail of the new inserted arc
     * @param head the id of the head of the new inserted arc
     * @warning if the arc already exists, nothing is done. In particular, no
     * exception is raised.
     * @throw InvalidNode if head or tail does not belong to the graph nodes
     * @throw InvalidCircuit if any (directed) cycle is created by this arc.
     *@warning Unfortunately, this means that insertArc is not in constant
     * time anymore. */
    virtual void insertArc( const NodeId tail,const NodeId head );

    /// @}
    

  private:
    /// checks whether there exists a directed path from \e from to \e to
    bool __hasDirectedPath( const NodeId from, const NodeId to );
  };


} /* namespace gum */


#ifndef GUM_NO_INLINE
#include <agrum/graphs/DAG.inl>
#endif //GUM_NOINLINE


#endif /* GUM_DAG_H */
