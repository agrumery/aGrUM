/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/** @file
 * @brief a cache for caching scores and independence tests results
 *
 * Caching previously computed scores or the results of conditional independence
 * tests is very important for learning algorithms because computing a score or
 * an independence test requires parsing the database and this is the most
 * time consuming operation in learning. This class provides an efficient cache
 * that can significantly alleviate the learning computational burden.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_SCORING_CACHE_H
#define GUM_LEARNING_SCORING_CACHE_H

#include <utility>

#include <agrum/agrum.h>
#include <agrum/tools/stattests/idCondSet.h>

namespace gum {

  namespace learning {


    /** @class ScoringCache
     * @brief a cache for caching scores and independence tests results
     * @headerfile scoringCache.h <agrum/tools/database/scoringCache.h>
     * @ingroup learning_scores
     *
     * Caching previously computed scores or the results of conditional
     * independence tests is very important for learning algorithms because
     * computing a score or an independence test requires parsing the database
     * and this is the most time consuming operation in learning. This class
     * provides an efficient cache that can significantly alleviate the
     * learning computational burden.
     */
    class ScoringCache {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      ScoringCache();

      /// copy constructor
      ScoringCache(const ScoringCache& from);

      /// move constructor
      ScoringCache(ScoringCache&& from);

      /// virtual copy constructor
      virtual ScoringCache* clone() const;

      /// destructor
      virtual ~ScoringCache();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      ScoringCache& operator=(const ScoringCache& from);

      /// move operator
      ScoringCache& operator=(ScoringCache&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// insert a new score into the cache
      /** @param idset the IdCondSet storing the sets of variables
       * @param score the score assigned to the IdCondSet
       * @throws DuplicateElement exception is raised if a score for the same
       * variables already exists */
      void insert(const IdCondSet& idset, double score);

      /// insert a new score into the cache
      /** @param idset the IdCondSet storing the sets of variables
       * @param score the score assigned to the IdCondSet
       * @throws DuplicateElement exception is raised if a score for the same
       * variables already exists */
      void insert(IdCondSet&& idset, double score);

      /// removes a score (if it exists)
      /** @param idset the IdCondSet storing the sets of variables
       * @warning If the score does not exist, nothing is done. In particular,
       * no exception is raised */
      void erase(const IdCondSet& idset);

      /// indicates whether a given score exists
      /** @param idset the IdCondSet storing the sets of variables */
      bool exists(const IdCondSet& idset);

      /// returns a given score
      /** @param idset the IdCondSet storing the sets of variables
       * @throws NotFound is raised if the score is not cached */
      double score(const IdCondSet& idset);

      /// removes all the stored scores
      void clear();

      /// returns the number of scores saved in the cache
      std::size_t size() const;

      /// @}


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      /// the scores stored into the cache
      HashTable< IdCondSet, double > _scores_;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */

// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/BN/learning/scores_and_tests/scoringCache_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_SCORING_CACHE_H */
