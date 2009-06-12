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
 * @brief Base classes for oriented graphs
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 */
#ifndef GUM_DIGRAPH_H
#define GUM_DIGRAPH_H

#include <iostream>
#include <utility>
#include <sstream>
#include <agrum/core/utils.h>
#include <agrum/graphs/nodeGraphPart.h>
#include <agrum/graphs/arcGraphPart.h>


namespace gum {


  /* =========================================================================== */
  /* ===     BASE CLASS FOR MANIPULATING GRAPHS WITH BOTH EDGES AND ARCS     === */
  /* =========================================================================== */
  /** @class DiGraph
   * @brief Base class for all graphs
   *
   * \ingroup graph_group
   *
   *
   * This is the base class for graphs containing directed edges (so-called arcs)
   * *
   * @par Usage example:
   * @code
   *  // creating empty graphs
   * DiGraph g1,g2;
   *
   * // adding nodes, edges and arcs to g1
   * NodeId i1=g1.insertNode();
   * NodeId i2=g1.insertNode();
   * NodeId i3=g1.insertNode();
   * g1.insertArc( i1,i2 );
   * g1.insertArc( i1,i3 );
   * g1.insertArc( i2,i3 );
   *
   * //throw a InvalidNode
   * // g1.insertArc( i1+i2+i3,i1 );
   *
   * // copying graphs
   * DiGraph g3 = g1;
   * g2 = g1;
   * DiGraph g4=g1;
   *
   * // check if a graph has no node
   * if ( g1.empty() ) cerr << "graph g1 is empty" << endl;
   *
   *
   * // remove all the nodes (as well as their adjacent arcs)
   * g1.clear();
   *
   * // remove some arc
   * g4.eraseArc( i1,i3 );
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
   * const ArcSet& a=g3.parents( 3 );
   *
   * for ( ArcSetIterator iter = a.begin( ); iter != a.end(); ++iter )
   *   cerr << "  -  "<<*iter;
   *
   * cerr<<endl;
   *
   * // remove all the arcs parent to a given node
   *  g3.eraseParents( 2
   * @endcode
   */
  /* =========================================================================== */
  class DiGraph : public ArcGraphPart,public virtual NodeGraphPart {
  public:
    /** @param nodes_size the size of the hash table used to store all the nodes
     * @param nodes_resize_policy the resizing policy of this hash table
     * @param arcs_size the size of the hash table used to store all the arcs
     * @param arcs_resize_policy the resizing policy of this hash table */
    explicit DiGraph( Size nodes_size = GUM_HASHTABLE_DEFAULT_SIZE,
                      bool nodes_resize_policy    = true,
                      Size arcs_size = GUM_HASHTABLE_DEFAULT_SIZE,
                      bool arcs_resize_policy    = true );
    DiGraph( const DiGraph& g );
    virtual ~DiGraph();

    /// @throw InvalidNode if head or tail does not belong to the graph nodes
    virtual void insertArc( const Arc& arc );

    /// @throw InvalidNode if head or tail does not belong to the graph nodes
    virtual void insertArc( const NodeId& tail,const NodeId& head );
    virtual void eraseNode( const NodeId id );
    virtual void clear();

    // not virtual : it is a feature !!! :)
    bool operator==( const DiGraph& g ) const;
    bool operator!=( const DiGraph& g ) const;
    virtual const std::string toString() const;

    /// @param name The graph name in the dot syntax. Default is G.
    /// @return Returns a string describing the graph in the dot syntax
    virtual const std::string toDot(std::string name="G") const;
  };


  /// for friendly displaying the content of directed graphs
  std::ostream& operator<< ( std::ostream&, const DiGraph& );

} /* namespace gum */

  

#ifndef GUM_NO_INLINE
#include <agrum/graphs/diGraph.inl>
#endif //GUM_NOINLINE

#endif /* GUM_DIGRAPH_H */
