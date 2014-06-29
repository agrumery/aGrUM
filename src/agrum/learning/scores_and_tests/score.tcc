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
    template <typename IdSetAlloc, typename CountAlloc>
    template <typename RowFilter> INLINE
    Score<IdSetAlloc,CountAlloc>::Score
    ( const RowFilter& filter,
      const std::vector<unsigned int>& var_modalities ) :
      Counter<IdSetAlloc,CountAlloc> ( filter, var_modalities ) {
      GUM_CONSTRUCTOR ( Score );
    }

    
    /// destructor
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    Score<IdSetAlloc,CountAlloc>::~Score () {
      GUM_DESTRUCTOR ( Score );
    }


    /// add a new single variable to be counted
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Score<IdSetAlloc,CountAlloc>::addNodeSet
    ( unsigned int var ) {
      if ( __use_cache ) {
        try {
          float score = __cache.score ( var, __empty_conditioning_set );
          __is_cached_score.push_back ( true );
          __cached_score.push_back ( score );
          return Counter<IdSetAlloc,CountAlloc>::addEmptyNodeSet ();
        }
        catch ( const NotFound& ) {}
      }

      __is_cached_score.push_back ( false );
      __cached_score.push_back ( 0 );
      return Counter<IdSetAlloc,CountAlloc>::addNodeSet ( var );
    }


    /// add a new target variable plus some conditioning vars
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int
    Score<IdSetAlloc,CountAlloc>::addNodeSet
    ( unsigned int var,
      const std::vector<unsigned int>& conditioning_ids ) {
      if ( __use_cache ) {
        try {
          float score = __cache.score ( var, conditioning_ids );
          __is_cached_score.push_back ( true );
          __cached_score.push_back ( score );
          return Counter<IdSetAlloc,CountAlloc>::addEmptyNodeSet ();
        }
        catch ( const NotFound& ) {}
      }

      __is_cached_score.push_back ( false );
      __cached_score.push_back ( 0 );
      return Counter<IdSetAlloc,CountAlloc>::addNodeSet ( var,  conditioning_ids );
    }


    /// clears all the data structures from memory
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    void Score<IdSetAlloc,CountAlloc>::clear () {
      Counter<IdSetAlloc,CountAlloc>::clear ();
      __is_cached_score.clear ();
      __cached_score.clear ();
    }


    /// indicates whether a score belongs to the cache
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    bool Score<IdSetAlloc,CountAlloc>::_isInCache
    ( unsigned int nodeset_index ) const noexcept {
      return ( ( nodeset_index < __is_cached_score.size () ) &&
               __is_cached_score[ nodeset_index ] );
    }


    /// inserts a new score into the cache    
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    void Score<IdSetAlloc,CountAlloc>::_insertIntoCache
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
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    float Score<IdSetAlloc,CountAlloc>::_cachedScore
    ( unsigned int nodeset_index ) const noexcept {
      return __cached_score[nodeset_index];
    }


    /// indicates whether we use the cache or not
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    bool Score<IdSetAlloc,CountAlloc>::_isUsingCache () const noexcept {
      return __use_cache;
    }

    
    /// turn on/off the use of a cache of the previously computed score
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    void
    Score<IdSetAlloc,CountAlloc>::useCache ( bool on_off ) noexcept {
      if ( ! on_off ) clear ();
      __use_cache = on_off;
    }

    
    /// clears the current cache (clear nodesets as well)
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    void Score<IdSetAlloc,CountAlloc>::clearCache () {
      clear ();
      __cache.clear ();
    }

 
  } /* namespace learning */
  
  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

