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
#ifndef GUM_EDGE_GRAPH_PART_H
#define GUM_EDGE_GRAPH_PART_H

#include <algorithm>
#include <utility>

#include <agrum/core/utils.h>
#include <agrum/core/signal/signaler.h>

#include <agrum/graphs/graphElements.h>


namespace gum {


  /** @class EdgeGraphPart
   * @brief Classes for undirected edge sets
   *
   * \ingroup graph_group
   *
   * @author Pierre-Henri WUILLEMIN and Christophe GONZALES*
   *
   * @par Usage example:
   * @code
   * EdgeGraphPart edges1,edges2;
   *
   * // insert elements into edges1
   * edges1.insertEdge( Edge( 2,3 ) );
   *
   * Edge edge( 5,3 );
   * edges1.insertEdge( edge );
   *
   * // copy edges1 into edges2
   * edges2=edges1;
   * std::cerr<<"edges2:"<<edges2.toString()<<std::endl;
   *
   * // remove some elements from edges1
   * edges1.eraseEdge( 2,3 );
   * edges1.eraseEdge( edge );
   *
   * if ( edges1.empty() ) std::cerr<<" edges1 is empty"<<std::endl;
   *
   * // checks whether a given edge exists
   * if ( edges2.exists( edge ) )
   *   std::cerr << "set contains " << edge << endl;
   *
   * if ( edges2.exists( 5,3 ) )
   *   std::cerr << "set contains " << edge << endl;
   *
   * std::cerr<<edges2.toString()<<std::endl;
   * std::cerr<<edges2.neighbours( 5 )<<std::endl;
   * @endcode
   */

  class EdgeGraphPart {
    private:
      static EdgeSet __empty_edge_set;
    public:
      Signaler2<NodeId,NodeId> onEdgeAdded;
      Signaler2<NodeId,NodeId> onEdgeDeleted;

      /** @param edges_size the size of the hash table used to store all the edges
       * @param edges_resize_policy the resizing policy of this hash table */
      explicit EdgeGraphPart( Size edges_size = GUM_HASHTABLE_DEFAULT_SIZE,
                              bool edges_resize_policy    = true );
      EdgeGraphPart( const EdgeGraphPart& s );
      virtual ~EdgeGraphPart();
      virtual void insertEdge( const Edge edge );
      virtual void insertEdge( const NodeId n1,const NodeId n2 );
      virtual void eraseEdge( const Edge edge );
      virtual void eraseEdge( const NodeId n1,const NodeId n2 );
      bool existsEdge( const Edge edge ) const;
      bool existsEdge( const NodeId n1,const NodeId n2 ) const;
      bool emptyEdges() const;
      virtual void clearEdges();
      Size sizeEdges() const;
      const EdgeSet& edges() const;
      const EdgeSet& neighbours( const NodeId id ) const;
      void eraseNeighbours( const NodeId id );
      const EdgeSetIterator beginEdges() const;
      const EdgeSetIterator& endEdges() const;
      bool operator==( const EdgeGraphPart& p ) const;
      bool operator!=( const EdgeGraphPart& p ) const;
      const std::string toString() const;
      template <typename ASSENT> typename Property< ASSENT >::onEdges edgesProperty( ASSENT( *f )( const Edge& ), Size size = 0 ) const;
      template <typename ASSENT> typename Property< ASSENT >::onEdges edgesProperty( const ASSENT& a, Size size = 0 ) const;
      template <typename ASSENT> List<ASSENT> listMapEdges( ASSENT( *f )( const Edge& ) ) const;

      // ============================================================================
      /// returns a possible path from node1 to node2 in the edge set
      /** @param node1 the id from which the path begins
       * @param node2 the id to which the path ends
       * @throw NotFound
       */
      // ============================================================================
      const std::vector<NodeId>
      undirectedPath( const NodeId node1, const NodeId node2 ) const;


    private:
      EdgeSet __edges;
      mutable Property<EdgeSet>::onNodes __neighbours;
      void __checkNeighbours( const NodeId id )const;
  };


  /// for friendly displaying the content of edge set
  std::ostream& operator<< ( std::ostream&, const EdgeGraphPart& );


} /* namespace gum */



#ifndef GUM_NO_INLINE
#include <agrum/graphs/edgeGraphPart.inl>
#endif //GUM_NOINLINE


#include <agrum/graphs/edgeGraphPart.tcc>


#endif // GUM_EDGEGRAPHPART_H
