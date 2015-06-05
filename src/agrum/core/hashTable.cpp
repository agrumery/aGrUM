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
 * @brief Outlined implementation of the hash tables
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <agrum/core/hashTable.h>

#ifndef NDEBUG
#include <agrum/core/debug.h>
#endif

namespace gum {

  // creates (if needed) and returns the iterator __HashTableIterEnd
  const HashTableIterator<int, int> *HashTableIteratorStaticEnd::end4Statics() {
    static bool first_time = true;

    if (first_time) {
      first_time = false;
      __HashTableIterEnd = new HashTableIterator<int, int>;
#ifndef NDEBUG
      __debug__::__dec_creation("HashTableIterator", "__hash_static_end", 0,
          "static variable correction", 0);
      __debug__::__dec_creation("HashTableConstIterator", "__hash_static_end", 0,
          "static variable correction", 0);
#endif
    }

    return __HashTableIterEnd;
  }

  // creates (if needed) and returns the iterator __HashTableIterEnd
  const HashTableConstIterator<int, int> *
  HashTableIteratorStaticEnd::constEnd4Statics() {
    return reinterpret_cast<const HashTableConstIterator<int, int> *>(end4Statics());
  }

  // creates (if needed) and returns the iterator __HashTableIterEndSafe
  const HashTableIteratorSafe<int, int> *
  HashTableIteratorStaticEnd::endSafe4Statics() {
    static bool first_time = true;

    if (first_time) {
      first_time = false;
      __HashTableIterEndSafe = new HashTableIteratorSafe<int, int>;
#ifndef NDEBUG
      __debug__::__dec_creation("HashTableIteratorSafe", "__hash_static_end", 0,
          "static variable correction", 0);
      __debug__::__dec_creation("HashTableConstIteratorSafe", "__hash_static_end", 0,
                     "static variable correction", 0);
#endif
    }

    return __HashTableIterEndSafe;
  }

  // creates (if needed) and returns the iterator __HashTableIterEndSafe
  const HashTableConstIteratorSafe<int, int> *
  HashTableIteratorStaticEnd::constEndSafe4Statics() {
    return reinterpret_cast<const HashTableConstIteratorSafe<int, int> *>(
        endSafe4Statics());
  }

  /// create the end iterator for all hash tables
  const HashTableIterator<int, int> *HashTableIteratorStaticEnd::__HashTableIterEnd =
      HashTableIteratorStaticEnd::end4Statics();

  /// create the end iterator for all hash tables
  const HashTableIteratorSafe<int, int> *
      HashTableIteratorStaticEnd::__HashTableIterEndSafe =
          HashTableIteratorStaticEnd::endSafe4Statics();

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
