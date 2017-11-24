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
#ifndef GUM_LEARNING_CACHE_4_PARTENTROPY_H
#define GUM_LEARNING_CACHE_4_PARTENTROPY_H

#include <utility>

#include <agrum/config.h>
#include <agrum/core/hashTable.h>
#include <agrum/learning/scores_and_tests/idSet.h>

namespace gum {

  namespace learning {

    /* =========================================================================
     */
    /* ===                   CACHE 4 PARTIAL ENTROPY CLASS                   ===
     */
    /* =========================================================================
     */
    /** @class Cache4PartEntropy
     * @brief a cache for caching partial entropy in PartEntropy class
     * @ingroup learning_group
     *
     * Caching previously computed scores is very important for learning
     * algorithms because computing a score requires parsing the database and
     * this is the most time consuming operation in learning. This class
     *provides
     * an efficient cache that can significantly alleviate the scoring burden.
     */
    class Cache4PartEntropy {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      Cache4PartEntropy();

      /// copy constructor
      Cache4PartEntropy(const Cache4PartEntropy& from);

      /// move constructor
      Cache4PartEntropy(Cache4PartEntropy&& from);

      /// destructor
      ~Cache4PartEntropy();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      Cache4PartEntropy& operator=(const Cache4PartEntropy& from);

      /// move operator
      Cache4PartEntropy& operator=(Cache4PartEntropy&& from);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// insert a new score into the cache
      /** @throws DuplicateElement exception is raised if a score for the same
       * variables already exists */
      void insert(const std::vector< Idx >& var_set, double score);

      /// insert a new score into the cache
      /** @throws DuplicateElement exception is raised if a score for the same
       * variables already exists */
      template < typename Alloc >
      void insert(const std::vector< Idx >& var_set, double score);

      /// removes a score (if it exists)
      /** If the score does not exist, nothing is done. In particular, no
       * exception is raised */
      void erase(const std::vector< Idx >& var_set);

      /// removes a score (if it exists)
      /** If the score does not exist, nothing is done. In particular, no
       * exception is raised */
      template < typename Alloc >
      void erase(const std::vector< Idx >& var_set);

      /// indicates whether a given score exists
      bool exists(const std::vector< Idx >& var_set);

      /// returns a given score
      /** @throws NotFound is raised if the score is not cached */
      double score(const std::vector< Idx >& var_set);

      /// removes all the stored scores
      void clear();

      /// @}

      private:
      /// the scores stored into the cache
      HashTable< std::vector< Idx >, double > __scores;
    };

  } /* namespace learning */

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/scores_and_tests/cache4PartEntropy_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_CACHE_4_PARTENTROPY_H */
