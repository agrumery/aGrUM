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
#ifndef GUM_ARC_GRAPH_PART_H
#define GUM_ARC_GRAPH_PART_H

#include <algorithm>
#include <utility>
#include <agrum/core/utils.h>
#include <agrum/graphs/graphElements.h>
#include <agrum/core/signal/signaler.h>


namespace gum {


  /** @class ArcGraphPart
   * @brief Classes for directed edge sets
   *
   * \ingroup graph_group
   *
   * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
   *
   * @par Usage example:
   * @code
   * ArcGraphPart arcs1,arcs2,arcs3;
   *
   * // insert elements into arcs1
   * arcs1.insertArc( Arc( 2,3 ) );
   * Arc arc( 5,3 );
   * arcs1.insertArc( arc );
   *
   * // copy arcs1 into arcs2
   * arcs2=arcs1;
   *
   * // remove some elements from arcs1
   * arcs1.eraseArc( 2,3 );
   * arcs1.eraseArc( arc );
   *
   * if ( arcs1.empty() ) std::cerr<<" arcs1 is empty"<<std::endl;
   *
   * // checks whether a given arc exists
   * if ( arcs2.existArc( arc ) )
   *   cerr << "set contains " << arc << endl;
   *
   * if ( arcs2.existArc( 5,3 ) )
   *   cerr << "set contains " << arc << endl;
   *
   * std::cerr<<arcs2.toString()<<std::endl;
   *
   * std::cerr<<arcs2.parents( 3 )<<std::endl;
   *
   * std::cerr<<arcs2.children( 2 )<<std::endl;
   *
   * std::cerr<<std::endl<<std::endl;
   *
   * std::cerr<<std::endl<<std::endl;
   * @endcode
   */

  class ArcGraphPart {
    private:
      static ArcSet __empty_arc_set;
    public:
      Signaler2<NodeId,NodeId> onArcAdded;  // onArcAdded(tail,head)
      Signaler2<NodeId,NodeId> onArcDeleted;  // onArcAdded(tail,head)

      /**
       * @param arcs_size the size of the hash table used to store all the edges
       * @param arcs_resize_policy the resizing policy of this hash table*/
      explicit ArcGraphPart( Size arcs_size = GUM_HASHTABLE_DEFAULT_SIZE,
                             bool arcs_resize_policy    = true );
      ArcGraphPart( const ArcGraphPart& s );
      ArcGraphPart& operator=( const ArcGraphPart& s );
      virtual ~ArcGraphPart();
      virtual void insertArc( const Arc arc );
      virtual void insertArc( const NodeId tail,const NodeId head );
      virtual void eraseArc( const Arc arc );
      virtual void eraseArc( NodeId tail,NodeId head );
      bool existsArc( const Arc arc ) const;
      bool existsArc( const NodeId tail,const NodeId head ) const;
      bool emptyArcs() const;
      void clearArcs();
      Size sizeArcs() const;
      const ArcSet& arcs() const;
      const ArcSet& parents( const NodeId id ) const;
      const ArcSet& children( const NodeId id ) const;
      void eraseParents( const NodeId id );
      void eraseChildren( const NodeId id );
      const ArcSetIterator beginArcs() const;
      const ArcSetIterator& endArcs() const;
      bool operator==( const ArcGraphPart& p ) const;
      bool operator!=( const ArcGraphPart& p ) const;
      const std::string toString() const;

      /** @brief a method to create a hashMap of Assent from a node list
       * (using for all node x, the ASSENT f(x)) */
      template <typename ASSENT> typename Property< ASSENT >::onArcs arcsProperty( ASSENT( *f )( const Arc& ), Size size = 0 ) const;

      /** @brief a method to create a hashMap of Assent from a node list
       * (using for all node x, the ASSENT a) */
      template <typename ASSENT> typename Property< ASSENT >::onArcs arcsProperty( const ASSENT& a, Size size = 0 ) const;

      template <typename ASSENT> List<ASSENT> listMapArcs( ASSENT( *f )( const Arc& ) ) const;


      // ============================================================================
      /// returns a directed path from node1 to node2 in the edge set
      /** @param node1 the id from which the path begins
       * @param node2 the id to which the path ends
       * @throw NotFound
       */
      // ============================================================================
      const std::vector<NodeId>
      directedPath( const NodeId node1, const NodeId node2 ) const;

      // ============================================================================
      /// returns an unoriented (directed) path from node1 to node2 in the edge set
      /** @param node1 the id from which the path begins
       * @param node2 the id to which the path ends
       * @throw NotFound
       */
      // ============================================================================
      const std::vector<NodeId>
      directedUnorientedPath( const NodeId node1, const NodeId node2 ) const;


    protected:
      void _eraseSetOfArcs( const ArcSet& set );


    private:
      Set<Arc> __arcs;
      mutable Property<ArcSet>::onNodes __parents;
      mutable Property<ArcSet>::onNodes __children;
      void __checkParents( const NodeId id ) const;
      void __checkChildren( const NodeId id ) const;
  };


  /// for friendly displaying the content of arc set
  std::ostream& operator<< ( std::ostream&, const ArcGraphPart& );


} /* namespace gum */



#ifndef GUM_NO_INLINE
#include <agrum/graphs/arcGraphPart.inl>
#endif //GUM_NOINLINE


#include <agrum/graphs/arcGraphPart.tcc>


#endif // GUM_ARC_GRAPH_PART_H
