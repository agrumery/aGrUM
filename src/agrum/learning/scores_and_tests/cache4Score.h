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
 * @brief a cache for caching scores in Score classes (BIC, BDeu, K2, etc.)
 *
 * Caching previously computed scores is very important for learning algorithms
 * because computing a score requires parsing the database and this is the most
 * time consuming operation in learning. This class provides an efficient cache
 * that can significantly alleviate the scoring burden.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_CACHE_4_SCORE_H
#define GUM_LEARNING_CACHE_4_SCORE_H

#include <utility>

#include <agrum/config.h>
#include <agrum/core/hashTable.h>
#include <agrum/learning/scores_and_tests/idSet.h>

namespace gum {

  namespace learning {

    /* ========================================================================= */
    /* ===                        CACHE 4 SCORE CLASS                        === */
    /* ========================================================================= */
    /** @class Cache4Score
     * @brief a cache for caching scores in Score classes (BIC, BDeu, K2, etc.)
     * @ingroup learning_group
     *
     * Caching previously computed scores is very important for learning
     * algorithms because computing a score requires parsing the database and
     * this is the most time consuming operation in learning. This class provides
     * an efficient cache that can significantly alleviate the scoring burden.
     */
    class Cache4Score {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      Cache4Score();

      /// copy constructor
      Cache4Score(const Cache4Score &from);

      /// move constructor
      Cache4Score(Cache4Score &&from);

      /// destructor
      ~Cache4Score();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      Cache4Score &operator=(const Cache4Score &from);

      /// move operator
      Cache4Score &operator=(Cache4Score &&from);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// insert a new score into the cache
      /** @throws DuplicateElement exception is raised if a score for the same
       * variables already exists */
      void insert(unsigned int var,
                  const std::vector<unsigned int> &conditioning_set, float score);

      /// insert a new score into the cache
      /** @throws DuplicateElement exception is raised if a score for the same
       * variables already exists */
      template <typename Alloc>
      void insert(unsigned int var, IdSet<Alloc> &conditioning_set, float score);

      /// removes a score (if it exists)
      /** If the score does not exist, nothing is done. In particular, no
       * exception is raised */
      void erase(unsigned int var,
                 const std::vector<unsigned int> &conditioning_set);

      /// removes a score (if it exists)
      /** If the score does not exist, nothing is done. In particular, no
       * exception is raised */
      template <typename Alloc>
      void erase(unsigned int var, const IdSet<Alloc> &conditioning_set);

      /// indicates whether a given score exists
      bool exists(unsigned int var,
                  const std::vector<unsigned int> &conditioning_set);

      /// returns a given score
      /** @throws NotFound is raised if the score is not cached */
      float score(unsigned int var,
                  const std::vector<unsigned int> &conditioning_set);

      /// removes all the stored scores
      void clear();

      /// @}

      private:
      /// the scores stored into the cache
      HashTable<std::pair<IdSet<>, unsigned int>, float> __scores;
    };

  } /* namespace learning */

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/scores_and_tests/cache4Score.inl>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_CACHE_4_SCORE_H */
