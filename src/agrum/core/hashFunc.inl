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
 * @brief Inlined implementation of the basic hash functions
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <string>
#include <utility>

#include <agrum/config.h>

// to ease parsing in IDE
#include <agrum/core/hashFunc.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /* in aGrUM, the sizes of hash tables (number of slots) are powers of 2. This is
   * not actually compulsory for the hash function we use. However, as it
   * speeds up the computations of hashed values, we chose to impose
   * this restriction. Function __hashTableLog2 thus returns the size in
   * bits - 1 necessary to store the smallest power of 2 greater than or
   * equal nb. */
  INLINE unsigned int __hashTableLog2(const Size &nb) {
    unsigned int i = 0;

    for (Size nbb = nb; nbb > 1UL; ++i, nbb >>= 1) {
    };

    return ((1UL << i) < nb ? i + 1U : i);
  }

  /// returns a hashed key for hash tables the keys of which are represented
  /// by strings
  INLINE Size HashFunc<std::string>::operator()(const std::string &key) const {
    Size h = 0;
    unsigned int size = key.size();
    const char *char_ptr = key.c_str();
    const unsigned long *int_ptr = (const unsigned long *)char_ptr;

    for (; size >= sizeof(unsigned long); size -= sizeof(unsigned long), ++int_ptr) {
      h = h * HashFuncConst::gold + *int_ptr;
    }

    for (char_ptr = (const char *)int_ptr; size != 0; --size, ++char_ptr) {
      h = 19 * h + *char_ptr;
    }

    return (h & _hash_mask);
  }

  /// returns a hashed key for hash tables the keys of which are represented
  /// by strings
  INLINE Size HashFunc<std::pair<std::string, std::string>>::
  operator()(const std::pair<std::string, std::string> &key) const {
    Size h = 0;

    const std::string &s1 = key.first;
    unsigned int size = s1.size();
    const char *char_ptr = s1.c_str();
    const unsigned long *int_ptr = (const unsigned long *)char_ptr;

    for (; size >= sizeof(unsigned long); size -= sizeof(unsigned long), ++int_ptr) {
      h = h * HashFuncConst::gold + *int_ptr;
    }

    for (char_ptr = (const char *)int_ptr; size != 0; --size, ++char_ptr) {
      h = 19 * h + *char_ptr;
    }

    const std::string &s2 = key.second;
    size = s2.size();
    char_ptr = s2.c_str();
    int_ptr = (const unsigned long *)char_ptr;

    for (; size >= sizeof(unsigned long); size -= sizeof(unsigned long), ++int_ptr) {
      h = h * HashFuncConst::gold + *int_ptr;
    }

    for (char_ptr = (const char *)int_ptr; size != 0; --size, ++char_ptr) {
      h = 19 * h + *char_ptr;
    }

    return (h & _hash_mask);
  }

  /// returns a hashed key for hash tables the keys of which are represented
  /// by vectors of Idx

  INLINE Size HashFunc<std::vector<Idx>>::
  operator()(const std::vector<Idx> &key) const {
    Size h = 0;

    for (size_t i = 0; i < key.size(); ++i)
      h += i * key[i];

    return ((h * HashFuncConst::gold) & _hash_mask);
  }

  /// returns a hashed key for hash tables the keys of which are represented
  /// by Debugs
  INLINE Size HashFunc<Debug>::operator()(const Debug &key) const {
    Size h = 0;

    for (size_t i = 0, j = key.size(); i < j; ++i)
      h = 19 * h + key[i];

    return ((h * HashFuncConst::gold) & _hash_mask);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
