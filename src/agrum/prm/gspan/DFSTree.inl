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
/**
 * @file
 * @brief Inline implementation of the DFSTree class.
 *
 * @author Lionel TORTI
 */
// ============================================================================
namespace gum {
  namespace prm {
    namespace gspan {

      INLINE
      DFSTree::DFSTree( const InterfaceGraph& graph, gspan::SearchStrategy* strategy ):
          __graph( &graph ), __strategy( strategy ) {
        GUM_CONSTRUCTOR( DFSTree );
        if ( not __strategy )
          __strategy = new FrequenceSearch( 2 );
        __strategy->setTree( this );
      }

      INLINE
      std::list<NodeId>&
      DFSTree::roots() { return __roots; }

      INLINE
      const std::list<NodeId>&
      DFSTree::roots() const { return __roots; }

      INLINE
      Pattern&
      DFSTree::parent( const Pattern& p ) {
        try {
          return *( __node_map.second( *( DiGraph::parents( __node_map.first( const_cast<Pattern*>( &p ) ) ).begin() ) ) );
        } catch ( NotFound& ) {
          if ( __node_map.existsSecond( const_cast<Pattern*>( &p ) ) ) {
            GUM_ERROR( NotFound, "the given pattern is a root node" );
          } else {
            GUM_ERROR( NotFound, "pattern not found in this DFSTree" );
          }
        }
      }

      INLINE
      const Pattern&
      DFSTree::parent( const Pattern& p ) const {
        try {
          return *( __node_map.second( *( DiGraph::parents( __node_map.first( const_cast<Pattern*>( &p ) ) ).begin() ) ) );
        } catch ( NotFound& ) {
          if ( __node_map.existsSecond( const_cast<Pattern*>( &p ) ) ) {
            GUM_ERROR( NotFound, "the given pattern is a root node" );
          } else {
            GUM_ERROR( NotFound, "pattern not found in this DFSTree" );
          }
        }
      }

      INLINE
      std::list<NodeId>&
      DFSTree::children( const Pattern& p ) {
        try {
          return __data[const_cast<Pattern*>( &p )]->children;
        } catch ( NotFound& ) {
          GUM_ERROR( NotFound, "pattern not found in this DFSTree" );
        }
      }

      INLINE
      const std::list<NodeId>&
      DFSTree::children( const Pattern& p ) const {
        try {
          return __data[const_cast<Pattern*>( &p )]->children;
        } catch ( NotFound& ) {
          GUM_ERROR( NotFound, "pattern not found in this DFSTree" );
        }
      }

      INLINE
      Pattern&
      DFSTree::pattern( NodeId id ) {
        try {
          return *( __node_map.second( id ) );
        } catch ( NotFound& ) {
          GUM_ERROR( NotFound, "no pattern matching the given id" );
        }
      }

      INLINE
      const Pattern&
      DFSTree::pattern( NodeId id ) const {
        try {
          return *( __node_map.second( id ) );
        } catch ( NotFound& ) {
          GUM_ERROR( NotFound, "no pattern matching the given id" );
        }
      }

      INLINE
      UndiGraph&
      DFSTree::iso_graph( const Pattern& p ) {
        try {
          return __data[const_cast<Pattern*>( &p )]->iso_graph;
        } catch ( NotFound& ) {
          GUM_ERROR( NotFound, "pattern not found in this DFSTree" );
        }
      }

      INLINE
      Sequence<Instance*>&
      DFSTree::iso_map( const Pattern& p, NodeId node ) {
        try {
          return *( __data[const_cast<Pattern*>( &p )]->iso_map[node] );
        } catch ( NotFound& ) {
          if ( __data.exists( const_cast<Pattern*>( &p ) ) ) {
            GUM_ERROR( NotFound, "node not found in Pattern's isomorphism graph" );
          } else {
            GUM_ERROR( NotFound, "pattern not found in this DFSTree" );
          }
        }
      }

      INLINE
      Set<NodeId>&
      DFSTree::max_indep_set( const Pattern& p ) {
        try {
          return __data[const_cast<Pattern*>( &p )]->max_indep_set;
        } catch ( NotFound& ) {
          GUM_ERROR( NotFound, "pattern not found in this DFSTree" );
        }
      }

      INLINE
      const InterfaceGraph&
      DFSTree::graph() const {
        return *__graph;
      }

      INLINE
      std::ostream&
      operator<<( std::ostream& out, const DFSTree::EdgeGrowth &edge ) {
        out << edge.u << ", " << *( edge.edge ) << ", " << *( edge.l_v ) << ", " << edge.v;
        return out;
      }

// INLINE
// Sequence< HashTable<ClassElement*, Size>* >&
// DFSTree::sub_patterns(const Pattern& p) {
//   return __data[const_cast<Pattern*>(&p)]->sub_patterns;
// }
//
// INLINE
// HashTable<NodeId, Idx>&
// DFSTree::sub_patterns_map(const Pattern& p) {
//   return __data[const_cast<Pattern*>(&p)]->sub_patterns_map;
// }

// ============================================================================
// EdgeGrowth
// ============================================================================

      INLINE
      DFSTree::EdgeGrowth::EdgeGrowth( NodeId a_u, LabelData* an_edge,
                                       LabelData* a_l_v, NodeId a_v ):
          u( a_u ), edge( an_edge ), l_v( a_l_v ), v( a_v ),
          degree_list( new std::vector<NodeId>() ) {
        GUM_CONSTRUCTOR( DFSTree::EdgeGrowth );
      }

      INLINE
      DFSTree::EdgeGrowth::EdgeGrowth( const EdgeGrowth& from ):
          u( from.u ), edge( from.edge ), v( from.v ), matches( from.matches ),
          iso_graph( from.iso_graph ), degree_list( 0 ), max_indep_set( from.max_indep_set ) {
        GUM_CONS_CPY( DFSTree::EdgeGrowth );
        if ( from.degree_list != 0 ) {
          degree_list = new std::vector<NodeId>( *( from.degree_list ) );
        }
      }

      INLINE
      DFSTree::EdgeGrowth::~EdgeGrowth() {
        GUM_DESTRUCTOR( DFSTree::EdgeGrowth );
        if ( degree_list != 0 ) {
          delete degree_list;
        }
      }

      INLINE
      std::string
      DFSTree::EdgeGrowth::toString() {
        std::stringstream str;
        str << u << "-" << edge << "-" << l_v << "-" << v;
        return str.str();
      }

//INLINE
//Size
//DFSTree::EdgeGrowth::count() const {
//  return matches.size();
//}

      INLINE
      double
      DFSTree::frequency( const Pattern& p ) const {
        return ( double ) __data[const_cast<Pattern*>( &p )]->max_indep_set.size();
      }

      INLINE
      DFSTree::PatternData&
      DFSTree::data( const Pattern& p ) {
        return *( __data[const_cast<Pattern*>( &p )] );
      }

      INLINE
      const DFSTree::PatternData&
      DFSTree::data( const Pattern& p ) const {
        return *( __data[const_cast<Pattern*>( &p )] );
      }

      INLINE
      SearchStrategy&
      DFSTree::strategy() { return *__strategy; }

      INLINE
      const SearchStrategy&
      DFSTree::strategy() const { return *__strategy; }

// ============================================================================
// NeighborDegreeSort
// ============================================================================

      INLINE
      DFSTree::NeighborDegreeSort::NeighborDegreeSort( UndiGraph& graph ):
          g( graph ) {
        GUM_CONSTRUCTOR( DFSTree::NeighborDegreeSort );
      }

      INLINE
      DFSTree::NeighborDegreeSort::NeighborDegreeSort( const NeighborDegreeSort& source ):
          g( source.g ) {
        GUM_CONS_CPY( DFSTree::NeighborDegreeSort );
      }

      INLINE
      DFSTree::NeighborDegreeSort::~NeighborDegreeSort() {
        GUM_DESTRUCTOR( DFSTree::NeighborDegreeSort );
      }

      INLINE
      bool
      DFSTree::NeighborDegreeSort::operator()( NodeId i, NodeId j ) {
        return g.neighbours( i ).size() < g.neighbours( j ).size();
      }

// ============================================================================
// PatternData
// ============================================================================

      INLINE
      DFSTree::PatternData::PatternData( Pattern* p ):
          pattern( p ), cost( 0 ), gain( 0 ) {
        GUM_CONSTRUCTOR( DFSTree::PatternData );
      }

    } /* namespace gspan */
  } /* namespace prm */
} /* namespace gum */
// ============================================================================
