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
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    INLINE Cache4Score::Cache4Score() { GUM_CONSTRUCTOR(Cache4Score); }

    /// copy constructor
    INLINE Cache4Score::Cache4Score(const Cache4Score &from)
        : __scores(from.__scores) {
      GUM_CONS_CPY(Cache4Score);
    }

    /// move constructor
    INLINE Cache4Score::Cache4Score(Cache4Score &&from)
        : __scores(std::move(from.__scores)) {
      GUM_CONS_MOV(Cache4Score);
    }

    /// destructor
    INLINE Cache4Score::~Cache4Score() { GUM_DESTRUCTOR(Cache4Score); }

    /// copy operator
    INLINE Cache4Score &Cache4Score::operator=(const Cache4Score &from) {
      if (&from != this) {
        __scores = from.__scores;
      }
      return *this;
    }

    /// move operator
    INLINE Cache4Score &Cache4Score::operator=(Cache4Score &&from) {
      if (&from != this) {
        __scores = std::move(from.__scores);
      }
      return *this;
    }

    /// insert a new score into the cache
    INLINE void
    Cache4Score::insert(unsigned int var,
                        const std::vector<unsigned int> &conditioning_set,
                        float score) {
      __scores.insert(
          std::pair<IdSet<>, unsigned int>(IdSet<>(conditioning_set, 0), var),
          std::move(score));
    }

    /// insert a new score into the cache
    template <typename Alloc>
    INLINE void Cache4Score::insert(unsigned int var, IdSet<Alloc> &conditioning_set,
                                    float score) {
      __scores.insert(
          std::pair<IdSet<>, unsigned int>(IdSet<>(conditioning_set), var),
          std::move(score));
    }

    /// removes a score (if it exists)
    INLINE void
    Cache4Score::erase(unsigned int var,
                       const std::vector<unsigned int> &conditioning_set) {
      __scores.erase(
          std::pair<IdSet<>, unsigned int>(IdSet<>(conditioning_set, 0), var));
    }

    /// removes a score (if it exists)
    template <typename Alloc>
    INLINE void Cache4Score::erase(unsigned int var,
                                   const IdSet<Alloc> &conditioning_set) {
      __scores.erase(std::pair<IdSet<>, unsigned int>(conditioning_set, var));
    }

    /// indicates whether a given score exists
    INLINE bool
    Cache4Score::exists(unsigned int var,
                        const std::vector<unsigned int> &conditioning_set) {
      return __scores.exists(
          std::pair<IdSet<>, unsigned int>(IdSet<>(conditioning_set, 0), var));
    }

    /// returns a given score
    INLINE float
    Cache4Score::score(unsigned int var,
                       const std::vector<unsigned int> &conditioning_set) {
      return __scores[std::pair<IdSet<>, unsigned int>(IdSet<>(conditioning_set, 0),
                                                       var)];
    }

    /// removes all the stored scores
    INLINE void Cache4Score::clear() { __scores.clear(); }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
