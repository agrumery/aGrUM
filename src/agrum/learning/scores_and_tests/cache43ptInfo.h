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
 * @brief a cache for caching 3 points mutual information in 3off2
 *
 * Caching previously computed scores is very important for learning algorithms
 * because computing independence tests requires parsing the database and this
 * is the most time consuming operation in learning. This class provides an
 * efficient cache that can significantly alleviate the scoring burden.
 *
 * @author Quentin FALCAND and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_CACHE_4_3_PT_INFO_H
#define GUM_LEARNING_CACHE_4_3_PT_INFO_H

#include <utility>

#include <agrum/config.h>
#include <agrum/core/hashTable.h>
#include <agrum/learning/scores_and_tests/idSet.h>

namespace gum {

  namespace learning {

    /* =========================================================================
     */
    /* ===                     CACHE 4 3-PT INFO CLASS                       ===
     */
    /* =========================================================================
     */
    /** @class Cache4Score
     * @brief a cache for caching 3 points mutual information in 3off2
     * @ingroup learning_group
     *
     * Caching previously computed scores is very important for learning
     *algorithms
     * because computing independence tests requires parsing the database and
     *this
     * is the most time consuming operation in learning. This class provides an
     * efficient cache that can significantly alleviate the scoring burden.
     */
    class Cache43ptInfo {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      Cache43ptInfo();

      /// copy constructor
      Cache43ptInfo(const Cache43ptInfo& from);

      /// move constructor
      Cache43ptInfo(Cache43ptInfo&& from);

      /// destructor
      ~Cache43ptInfo();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      Cache43ptInfo& operator=(const Cache43ptInfo& from);

      /// move operator
      Cache43ptInfo& operator=(Cache43ptInfo&& from);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// insert a new score into the cache
      /** @throws DuplicateElement exception is raised if a score for the same
       * variables already exists */
      void insert(Idx                       var1,
                  Idx                       var2,
                  Idx                       var3,
                  const std::vector< Idx >& conditioning_set,
                  double                    score);

      /// insert a new score into the cache
      /** @throws DuplicateElement exception is raised if a score for the same
       * variables already exists */
      template < typename Alloc >
      void insert(Idx             var1,
                  Idx             var2,
                  Idx             var3,
                  IdSet< Alloc >& conditioning_set,
                  double          score);

      /// removes a score (if it exists)
      /** If the score does not exist, nothing is done. In particular, no
       * exception is raised */
      void erase(Idx                       var1,
                 Idx                       var2,
                 Idx                       var3,
                 const std::vector< Idx >& conditioning_set);

      /// removes a score (if it exists)
      /** If the score does not exist, nothing is done. In particular, no
       * exception is raised */
      template < typename Alloc >
      void
      erase(Idx var1, Idx var2, Idx var3, const IdSet< Alloc >& conditioning_set);

      /// indicates whether a given score exists
      bool exists(Idx                       var1,
                  Idx                       var2,
                  Idx                       var3,
                  const std::vector< Idx >& conditioning_set);

      /// returns a given score
      /** @throws NotFound is raised if the score is not cached */
      double score(Idx                       var1,
                   Idx                       var2,
                   Idx                       var3,
                   const std::vector< Idx >& conditioning_set);

      /// removes all the stored scores
      void clear();

      /// @}

      private:
      /// the scores stored into the cache
      HashTable< std::tuple< IdSet<>, Idx, Idx, Idx >, double > __scores;
    };

  } /* namespace learning */

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/scores_and_tests/cache43ptInfo_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_CACHE_4_3_PT_INFO_H */