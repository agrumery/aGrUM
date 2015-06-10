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
 * @brief Outlined implementation of Sets
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <agrum/core/debug.h>

#include <agrum/core/set.h>

namespace gum {

  // creates (if needed) and returns the iterator __SetIterEndSafe
  const SetIteratorSafe<int> *SetIteratorStaticEnd::endSafe4Statics() {
    static bool first_time = true;

    if (first_time) {
      first_time = false;
      __SetIterEndSafe = new SetIteratorSafe<int>;
#ifndef NDEBUG
      __debug__::__dec_creation("SetIteratorSafe", "__set_static_end", 0,
                     "static variable correction", 0);
      __debug__::__dec_creation("HashTableConstIteratorSafe", "__set_static_end", 0,
                     "static variable correction", 0);
#endif
    }

    return __SetIterEndSafe;
  }

  // creates (if needed) and returns the iterator __SetIterEndSafe
  const SetIteratorSafe<int> *SetIteratorStaticEnd::constEndSafe4Statics() {
    return endSafe4Statics();
  }

  // creates (if needed) and returns the iterator __SetIterEnd
  const SetIterator<int> *SetIteratorStaticEnd::end4Statics() {
    static bool first_time = true;

    if (first_time) {
      first_time = false;
      __SetIterEnd = new SetIterator<int>;
#ifndef NDEBUG
      __debug__::__dec_creation("SetIterator", "__set_static_end", 0,
                     "static variable correction", 0);
      __debug__::__dec_creation("HashTableConstIterator", "__set_static_end", 0,
                     "static variable correction", 0);
#endif
    }

    return __SetIterEnd;
  }

  // creates (if needed) and returns the iterator __SetIterEnd
  const SetIterator<int> *SetIteratorStaticEnd::constEnd4Statics() {
    return end4Statics();
  }

  // create the end iterator for all Sets
  const SetIteratorSafe<int> *SetIteratorStaticEnd::__SetIterEndSafe =
      SetIteratorStaticEnd::endSafe4Statics();

  // create the end iterator for all Sets
  const SetIterator<int> *SetIteratorStaticEnd::__SetIterEnd =
      SetIteratorStaticEnd::end4Statics();

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
