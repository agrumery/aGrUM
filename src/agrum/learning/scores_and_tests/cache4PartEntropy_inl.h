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
 * @brief a cache for caching partial entropy in PartEntropy class
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    INLINE Cache4PartEntropy::Cache4PartEntropy() { GUM_CONSTRUCTOR( Cache4PartEntropy ); }

    /// copy constructor
    INLINE Cache4PartEntropy::Cache4PartEntropy( const Cache4PartEntropy& from )
        : __scores( from.__scores ) {
      GUM_CONS_CPY( Cache4PartEntropy );
    }

    /// move constructor
    INLINE Cache4PartEntropy::Cache4PartEntropy( Cache4PartEntropy&& from )
        : __scores( std::move( from.__scores ) ) {
      GUM_CONS_MOV( Cache4PartEntropy );
    }

    /// destructor
    INLINE Cache4PartEntropy::~Cache4PartEntropy() { GUM_DESTRUCTOR( Cache4PartEntropy ); }

    /// copy operator
    INLINE Cache4PartEntropy& Cache4PartEntropy::operator=( const Cache4PartEntropy& from ) {
      if ( &from != this ) {
        __scores = from.__scores;
      }
      return *this;
    }

    /// move operator
    INLINE Cache4PartEntropy& Cache4PartEntropy::operator=( Cache4PartEntropy&& from ) {
      if ( &from != this ) {
        __scores = std::move( from.__scores );
      }
      return *this;
    }

    /// insert a new score into the cache
    INLINE void Cache4PartEntropy::insert( const std::vector<Idx>& var_set,
                                     double                  score ) {
      std::vector<Idx> var_set2 = var_set;
      std::sort( var_set2.begin(), var_set2.end() );
      __scores.insert( var_set2, std::move( score ) );
    }

    /// insert a new score into the cache
    template <typename Alloc>
    INLINE void
    Cache4PartEntropy::insert( const std::vector<Idx>& var_set, double score ) {
        std::vector<Idx> var_set2 = var_set;
        std::sort( var_set2.begin(), var_set2.end() );
      __scores.insert( var_set2, std::move( score ) );
    }

    /// removes a score (if it exists)
    INLINE void Cache4PartEntropy::erase( const std::vector<Idx>& var_set ) {
        std::vector<Idx> var_set2 = var_set;
        std::sort( var_set2.begin(), var_set2.end() );
      __scores.erase( var_set2 );
    }

    /// removes a score (if it exists)
    template <typename Alloc>
    INLINE void Cache4PartEntropy::erase( const std::vector<Idx>& var_set ) {
        std::vector<Idx> var_set2 = var_set;
        std::sort( var_set2.begin(), var_set2.end() );
      __scores.erase( var_set2 );
    }

    /// indicates whether a given score exists
    INLINE bool Cache4PartEntropy::exists( const std::vector<Idx>& var_set ) {
        std::vector<Idx> var_set2 = var_set;
        std::sort( var_set2.begin(), var_set2.end() );
      return __scores.exists( var_set2 );
    }

    /// returns a given score
    INLINE double Cache4PartEntropy::score( const std::vector<Idx>& var_set ) {
        std::vector<Idx> var_set2 = var_set;
        std::sort( var_set2.begin(), var_set2.end() );
      return __scores[var_set2];
    }

    /// removes all the stored scores
    INLINE void Cache4PartEntropy::clear() { __scores.clear(); }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
