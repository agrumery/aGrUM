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
 * @brief a cache for caching scores in Indep Test classes (chi2, G2, etc.)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <tuple>

namespace gum {

  namespace learning {

    /// default constructor
    INLINE Cache4IndepTest::Cache4IndepTest() { GUM_CONSTRUCTOR(Cache4IndepTest); }

    /// copy constructor
    INLINE Cache4IndepTest::Cache4IndepTest(const Cache4IndepTest &from)
        : __scores(from.__scores) {
      GUM_CONS_CPY(Cache4IndepTest);
    }

    /// move constructor
    INLINE Cache4IndepTest::Cache4IndepTest(Cache4IndepTest &&from)
        : __scores(std::move(from.__scores)) {
      GUM_CONS_MOV(Cache4IndepTest);
    }

    /// destructor
    INLINE Cache4IndepTest::~Cache4IndepTest() { GUM_DESTRUCTOR(Cache4IndepTest); }

    /// copy operator
    INLINE Cache4IndepTest &Cache4IndepTest::operator=(const Cache4IndepTest &from) {
      if (&from != this) {
        __scores = from.__scores;
      }
      return *this;
    }

    /// move operator
    INLINE Cache4IndepTest &Cache4IndepTest::operator=(Cache4IndepTest &&from) {
      if (&from != this) {
        __scores = std::move(from.__scores);
      }
      return *this;
    }

    /// insert a new score into the cache
    INLINE void
    Cache4IndepTest::insert(unsigned int var1, unsigned int var2,
                            const std::vector<unsigned int> &conditioning_set,
                            float score) {
      if (var1 > var2)
        std::swap(var1, var2);
      __scores.insert(std::tuple<IdSet<>, unsigned int, unsigned int>(
                          IdSet<>(conditioning_set, 0), var1, var2),
                      std::move(score));
    }

    /// insert a new score into the cache
    template <typename Alloc>
    INLINE void Cache4IndepTest::insert(unsigned int var1, unsigned int var2,
                                        IdSet<Alloc> &conditioning_set,
                                        float score) {
      if (var1 > var2)
        std::swap(var1, var2);
      __scores.insert(std::tuple<IdSet<>, unsigned int, unsigned int>(
                          IdSet<>(conditioning_set), var1, var2),
                      std::move(score));
    }

    /// removes a score (if it exists)
    INLINE void
    Cache4IndepTest::erase(unsigned int var1, unsigned int var2,
                           const std::vector<unsigned int> &conditioning_set) {
      if (var1 > var2)
        std::swap(var1, var2);
      __scores.erase(std::tuple<IdSet<>, unsigned int, unsigned int>(
          IdSet<>(conditioning_set, 0), var1, var2));
    }

    /// removes a score (if it exists)
    template <typename Alloc>
    INLINE void Cache4IndepTest::erase(unsigned int var1, unsigned int var2,
                                       const IdSet<Alloc> &conditioning_set) {
      if (var1 > var2)
        std::swap(var1, var2);
      __scores.erase(std::tuple<IdSet<>, unsigned int, unsigned int>(
          conditioning_set, var1, var2));
    }

    /// indicates whether a given score exists
    INLINE bool
    Cache4IndepTest::exists(unsigned int var1, unsigned int var2,
                            const std::vector<unsigned int> &conditioning_set) {
      if (var1 > var2)
        std::swap(var1, var2);
      return __scores.exists(std::tuple<IdSet<>, unsigned int, unsigned int>(
          IdSet<>(conditioning_set, 0), var1, var2));
    }

    /// returns a given score
    INLINE float
    Cache4IndepTest::score(unsigned int var1, unsigned int var2,
                           const std::vector<unsigned int> &conditioning_set) {
      if (var1 > var2)
        std::swap(var1, var2);
      return __scores[std::tuple<IdSet<>, unsigned int, unsigned int>(
          IdSet<>(conditioning_set, 0), var1, var2)];
    }

    /// removes all the stored scores
    INLINE void Cache4IndepTest::clear() { __scores.clear(); }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
