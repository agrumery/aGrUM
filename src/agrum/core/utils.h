/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it udnder the terms of the GNU General Public License as published by *
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
#ifndef GUM_UTILS_H
#define GUM_UTILS_H


#include <iostream>
#include <utility>
#include <vector>
#include <cstdlib>

// FLAG : DEBUG mode
//#undef  NDEBUG
//#define NDEBUG

#ifdef NDEBUG
#define GUM_CAST dynamic_cast
#else
#define GUM_CAST static_cast
#endif

#include <agrum/core/utils_dir.h>
#include <agrum/core/utils_string.h>

namespace std {

  // used ,e.g., for hashtables
  template <typename T1, typename T2>
  ostream& operator<< ( ostream& stream, const pair<T1, T2>& val );

  template <typename T>
  ostream& operator<< ( ostream& stream, const vector<T>& val );

} /* namespace std */


namespace gum {
  /// Implements a stream with the same behaviour as /dev/null
  struct NullStream : std::ostream {
    NullStream() : std::ios( 0 ), std::ostream( 0 ) {}
  };

  /// cross-platform replacement for memcmp. returns true if OK
  bool Memcmp( const void* const _in,
               const void* const _out,
               unsigned long size );

  /// for debug purpose
  void __atexit( void );

  /// returns a random number between 0 and 1 included (i.e. a proba)
  double randomProba();

  /// return a random discrete distribution
  /// @param n is the number of modalities for the ditribution
  template<typename GUM_SCALAR>
  std::vector<GUM_SCALAR> randomDistribution(Size n);

  /// initialize random generator seed
  void initRandom(unsigned int init=0);

  /// indicate whether two elements are (almost) different or not
  template <typename T>
  struct AlmostDifferent {
    bool operator()( const T& t1, const T& t2 ) {
      return ( ( t1 > t2 + ( T ) 0.000001 ) ||
               ( t2 > t1 + ( T ) 0.000001 ) );
    }
  };

  template <typename T>
  struct AlmostDifferent<T*> {
    bool operator()( const T* t1, const T* t2 ) {
      return ( t1 != t2 );
    }
  };


} /* namespace gum */


#ifndef GUM_NO_INLINE
#include <agrum/core/utils.inl>
#endif // GUM_NO_INLINE


#include <agrum/core/utils.tcc>


#endif /* GUM_UTILS_H */
