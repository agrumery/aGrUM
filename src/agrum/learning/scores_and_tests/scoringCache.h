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
 * @brief a cache for caching scores and independence tests results
 *
 * Caching previously computed scores or the results of conditional independence
 * tests is very important for learning algorithms because computing a score or
 * an independence test requires parsing the database and this is the most
 * time consuming operation in learning. This class provides an efficient cache
 * that can significantly alleviate the learning computational burden.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_SCORING_CACHE_H
#define GUM_LEARNING_SCORING_CACHE_H

#include <utility>

#include <agrum/agrum.h>
#include <agrum/learning/scores_and_tests/idSet.h>

namespace gum {

  namespace learning {


    /** @class ScoringCache
     * @brief a cache for caching scores and independence tests results
     * @headerfile scoringCache.h <agrum/learning/database/scoringCache.h>
     * @ingroup learning_scores
     *
     * Caching previously computed scores or the results of conditional
     * independence tests is very important for learning algorithms because
     * computing a score or an independence test requires parsing the database
     * and this is the most time consuming operation in learning. This class
     * provides an efficient cache that can significantly alleviate the
     * learning computational burden.
     */
    template < template < typename > class ALLOC = std::allocator >
    class ScoringCache : private ALLOC< NodeId > {
      public:
      /// type for the allocators passed in arguments of methods
      using allocator_type = ALLOC< NodeId >;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      ScoringCache(const allocator_type& alloc = allocator_type());

      /// copy constructor
      ScoringCache(const ScoringCache< ALLOC >& from);

      /// copy constructor with a given allocator
      ScoringCache(const ScoringCache< ALLOC >& from, const allocator_type& alloc);

      /// move constructor
      ScoringCache(ScoringCache< ALLOC >&& from);

      /// move constructor with a given allocator
      ScoringCache(ScoringCache< ALLOC >&& from, const allocator_type& alloc);

      /// virtual copy constructor
      virtual ScoringCache< ALLOC >* clone() const;

      /// virtual copy constructor with a given allocator
      virtual ScoringCache< ALLOC >* clone(const allocator_type& alloc) const;

      /// destructor
      virtual ~ScoringCache();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      ScoringCache< ALLOC >& operator=(const ScoringCache< ALLOC >& from);

      /// move operator
      ScoringCache< ALLOC >& operator=(ScoringCache< ALLOC >&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// insert a new score into the cache
      /** @param idset the IdSet storing the sets of variables
       * @param score the score assigned to the IdSet
       * @throws DuplicateElement exception is raised if a score for the same
       * variables already exists */
      void insert(const IdSet< ALLOC >& idset, double score);

      /// insert a new score into the cache
      /** @param idset the IdSet storing the sets of variables
       * @param score the score assigned to the IdSet
       * @throws DuplicateElement exception is raised if a score for the same
       * variables already exists */
      void insert(IdSet< ALLOC >&& idset, double score);

      /// removes a score (if it exists)
      /** @param idset the IdSet storing the sets of variables
       * @warning If the score does not exist, nothing is done. In particular,
       * no exception is raised */
      void erase(const IdSet< ALLOC >& idset);

      /// indicates whether a given score exists
      /** @param idset the IdSet storing the sets of variables */
      bool exists(const IdSet< ALLOC >& idset);

      /// returns a given score
      /** @param idset the IdSet storing the sets of variables
       * @throws NotFound is raised if the score is not cached */
      double score(const IdSet< ALLOC >& idset);

      /// removes all the stored scores
      void clear();

      /// returns the number of scores saved in the cache
      std::size_t size() const;

      /// returns the allocator used by the translator
      allocator_type getAllocator() const;

      /// @}


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      /// the scores stored into the cache
      HashTable< IdSet< ALLOC >,
                 double,
                 ALLOC< std::pair< IdSet< ALLOC >, double > > >
        __scores;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */


// always include the template implementation
#include <agrum/learning/scores_and_tests/scoringCache_tpl.h>


#endif /* GUM_LEARNING_SCORING_CACHE_H */
