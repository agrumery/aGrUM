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
 * @author Quentin FALCAND and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <tuple>

namespace gum {

  namespace learning {

    /// default constructor
    INLINE Cache43ptInfo::Cache43ptInfo() { GUM_CONSTRUCTOR(Cache43ptInfo); }

    /// copy constructor
    INLINE Cache43ptInfo::Cache43ptInfo(const Cache43ptInfo& from)
        : __scores(from.__scores) {
      GUM_CONS_CPY(Cache43ptInfo);
    }

    /// move constructor
    INLINE Cache43ptInfo::Cache43ptInfo(Cache43ptInfo&& from)
        : __scores(std::move(from.__scores)) {
      GUM_CONS_MOV(Cache43ptInfo);
    }

    /// destructor
    INLINE Cache43ptInfo::~Cache43ptInfo() { GUM_DESTRUCTOR(Cache43ptInfo); }

    /// copy operator
    INLINE Cache43ptInfo& Cache43ptInfo::operator=(const Cache43ptInfo& from) {
      if (&from != this) {
        __scores = from.__scores;
      }
      return *this;
    }

    /// move operator
    INLINE Cache43ptInfo& Cache43ptInfo::operator=(Cache43ptInfo&& from) {
      if (&from != this) {
        __scores = std::move(from.__scores);
      }
      return *this;
    }

    /// insert a new score into the cache
    INLINE void Cache43ptInfo::insert(Idx                       var1,
                                      Idx                       var2,
                                      Idx                       var3,
                                      const std::vector< Idx >& conditioning_set,
                                      double                    score) {
      // if ( var1 > var2 ) std::replace( var1, var2 );
      __scores.insert(std::tuple< IdSet<>, Idx, Idx, Idx >(
                        IdSet<>(conditioning_set, 0), var1, var2, var3),
                      std::move(score));
    }

    /// insert a new score into the cache
    template < typename Alloc >
    INLINE void Cache43ptInfo::insert(Idx             var1,
                                      Idx             var2,
                                      Idx             var3,
                                      IdSet< Alloc >& conditioning_set,
                                      double          score) {
      // if ( var1 > var2 ) std::replace( var1, var2 );
      __scores.insert(
        std::tuple< IdSet<>, Idx, Idx, Idx >(conditioning_set, var1, var2, var3),
        std::move(score));
    }

    /// removes a score (if it exists)
    INLINE void Cache43ptInfo::erase(Idx                       var1,
                                     Idx                       var2,
                                     Idx                       var3,
                                     const std::vector< Idx >& conditioning_set) {
      // if ( var1 > var2 ) std::replace( var1, var2 );
      __scores.erase(std::tuple< IdSet<>, Idx, Idx, Idx >(
        IdSet<>(conditioning_set, 0), var1, var2, var3));
    }

    /// removes a score (if it exists)
    template < typename Alloc >
    INLINE void Cache43ptInfo::erase(Idx                   var1,
                                     Idx                   var2,
                                     Idx                   var3,
                                     const IdSet< Alloc >& conditioning_set) {
      // if ( var1 > var2 ) std::replace( var1, var2 );
      __scores.erase(
        std::tuple< IdSet<>, Idx, Idx, Idx >(conditioning_set, var1, var2, var3));
    }

    /// indicates whether a given score exists
    INLINE bool Cache43ptInfo::exists(Idx                       var1,
                                      Idx                       var2,
                                      Idx                       var3,
                                      const std::vector< Idx >& conditioning_set) {
      // if ( var1 > var2 ) std::replace( var1, var2 );
      return __scores.exists(std::tuple< IdSet<>, Idx, Idx, Idx >(
        IdSet<>(conditioning_set, 0), var1, var2, var3));
    }

    /// returns a given score
    INLINE double Cache43ptInfo::score(
      Idx var1, Idx var2, Idx var3, const std::vector< Idx >& conditioning_set) {
      // if ( var1 > var2 ) std::replace( var1, var2 );
      return __scores[std::tuple< IdSet<>, Idx, Idx, Idx >(
        IdSet<>(conditioning_set, 0), var1, var2, var3)];
    }

    /// removes all the stored scores
    INLINE void Cache43ptInfo::clear() { __scores.clear(); }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
