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
 * @brief the base class for all the scores used for learning (BIC, BDeu, etc)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

 

namespace gum {

  
  namespace learning {


    /// default constructor
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    Score<RowFilter,IdSetAlloc,CountAlloc>::Score
    ( const RowFilter& filter,
      const std::vector<unsigned int>& var_modalities ) :
      Counter<RowFilter,IdSetAlloc,CountAlloc> ( filter, var_modalities ) {
      GUM_CONSTRUCTOR ( Score );
    }

    
    /// destructor
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    Score<RowFilter,IdSetAlloc,CountAlloc>::~Score () {
      GUM_DESTRUCTOR ( Score );
    }


    /// add a new single variable to be counted
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Score<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
    ( unsigned int var ) {
      if ( __use_cache ) {
        try {
          float score = __cache.score ( var, __empty_conditioning_set );
          __is_cached_score.push_back ( true );
          __cached_score.push_back ( score );
          return Counter<RowFilter,IdSetAlloc,CountAlloc>::addEmptyNodeSet ();
        }
        catch ( const NotFound& ) {}
      }

      __is_cached_score.push_back ( false );
      __cached_score.push_back ( 0 );
      return Counter<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet ( var );
    }


    /// add a new target variable plus some conditioning vars
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int
    Score<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
    ( unsigned int var,
      const std::vector<unsigned int>& conditioning_ids ) {
      if ( __use_cache ) {
        try {
          float score = __cache.score ( var, conditioning_ids );
          __is_cached_score.push_back ( true );
          __cached_score.push_back ( score );
          return Counter<RowFilter,IdSetAlloc,CountAlloc>::addEmptyNodeSet ();
        }
        catch ( const NotFound& ) {}
      }

      __is_cached_score.push_back ( false );
      __cached_score.push_back ( 0 );
      return Counter<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
        ( var,  conditioning_ids );
    }


    /// clears all the data structures from memory
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    void Score<RowFilter,IdSetAlloc,CountAlloc>::clear () {
      Counter<RowFilter,IdSetAlloc,CountAlloc>::clear ();
      __is_cached_score.clear ();
      __cached_score.clear ();
    }


    /// indicates whether a score belongs to the cache
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    bool Score<RowFilter,IdSetAlloc,CountAlloc>::_isInCache
    ( unsigned int nodeset_index ) const noexcept {
      return ( ( nodeset_index < __is_cached_score.size () ) &&
               __is_cached_score[ nodeset_index ] );
    }


    /// inserts a new score into the cache    
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    void Score<RowFilter,IdSetAlloc,CountAlloc>::_insertIntoCache
    ( unsigned int nodeset_index, float score ) {
      const std::vector<unsigned int,IdSetAlloc>& all_nodes = 
        _getAllNodes ( nodeset_index );
      const std::vector<unsigned int,IdSetAlloc>* conditioning_nodes = 
        _getConditioningNodes ( nodeset_index );

      if ( conditioning_nodes != nullptr ) {
        try {
          __cache.insert ( all_nodes[all_nodes.size() - 1],
                           *conditioning_nodes, score );
        }
        catch ( const gum::DuplicateElement& ) {}
      }
      else {
        try {
          __cache.insert ( all_nodes[0], __empty_conditioning_set, score );
        }
        catch ( const gum::DuplicateElement& ) {}
      }
    }


    /// returns a cached score
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    float Score<RowFilter,IdSetAlloc,CountAlloc>::_cachedScore
    ( unsigned int nodeset_index ) const noexcept {
      return __cached_score[nodeset_index];
    }


    /// indicates whether we use the cache or not
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    bool Score<RowFilter,IdSetAlloc,CountAlloc>::_isUsingCache () const noexcept {
      return __use_cache;
    }

    
    /// turn on/off the use of a cache of the previously computed score
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    void
    Score<RowFilter,IdSetAlloc,CountAlloc>::useCache ( bool on_off ) noexcept {
      if ( ! on_off ) clear ();
      __use_cache = on_off;
    }

    
    /// clears the current cache (clear nodesets as well)
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    void Score<RowFilter,IdSetAlloc,CountAlloc>::clearCache () {
      clear ();
      __cache.clear ();
    }


    /// assign the DiGraph that is being learnt (useful for a priori settings)
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    void
    Score<RowFilter,IdSetAlloc,CountAlloc>::setGraph ( const DiGraph& graph ) {
      __learnt_graph = graph;
    }


    /// returns a reference on the digraph being learnt
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    const DiGraph&
    Score<RowFilter,IdSetAlloc,CountAlloc>::graph () const noexcept {
      return __learnt_graph;
    }
 
    
    /// inform the score that a new arc has been inserted into the learnt graph
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    void Score<RowFilter,IdSetAlloc,CountAlloc>::modifyGraph
    ( const GraphChange& change ) {
      switch ( change.type () ) {
      case GraphChangeType::ARC_ADDITION:
        __learnt_graph.insertArc ( change.node1(), change.node2 () );
        break;

      case GraphChangeType::ARC_DELETION:
        __learnt_graph.eraseArc ( Arc ( change.node1(), change.node2 () ) );
        break;

      case GraphChangeType::ARC_REVERSAL:
        __learnt_graph.eraseArc ( Arc ( change.node1(), change.node2 () ) );
        __learnt_graph.insertArc ( change.node2(), change.node1 () );
        break;
        
      default:
        GUM_ERROR ( OperationNotAllowed,
                    "edge modifications are not supported by scores" );
      }
    }

 
  } /* namespace learning */
  
  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

