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
 * @brief Outlined implementation of Bijections
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifndef NDEBUG
#include <agrum/core/debug.h>
#endif

#include <agrum/core/bijection.h>

namespace gum {

  // creates (if needed) and returns the safe iterator __BijectionIterEndSafe
  const BijectionIteratorSafe<int, int> *
  BijectionIteratorStaticEnd::endSafe4Statics() {
    static bool first_time = true;

    if (first_time) {
      first_time = false;
      __BijectionIterEndSafe = new BijectionIteratorSafe<int, int>;
#ifndef NDEBUG
      __debug__::__dec_creation("BijectionIteratorSafe", "__bijection_static_end", 0,
                                "static variable correction", 0);
      __debug__::__dec_creation("HashTableConstIteratorSafe", "__bijection_static_end", 0,
                                "static variable correction", 0);
#endif
    }

    return __BijectionIterEndSafe;
  }

  // creates (if needed) and returns the unsafe iterator __BijectionIterEnd
  const BijectionIterator<int, int> *BijectionIteratorStaticEnd::end4Statics() {
    static bool first_time = true;

    if (first_time) {
      first_time = false;
      __BijectionIterEnd = new BijectionIterator<int, int>;
#ifndef NDEBUG
      __debug__::__dec_creation("BijectionIterator", "__bijection_static_end", 0,
                                "static variable correction", 0);
      __debug__::__dec_creation("HashTableConstIterator", "__bijection_static_end", 0,
                                "static variable correction", 0);
#endif
    }

    return __BijectionIterEnd;
  }

  // create the end iterator for all Bijections
  const BijectionIteratorSafe<int, int> *
      BijectionIteratorStaticEnd::__BijectionIterEndSafe =
          BijectionIteratorStaticEnd::endSafe4Statics();

  // create the end iterator for all Bijections
  const BijectionIterator<int, int> *BijectionIteratorStaticEnd::__BijectionIterEnd =
      BijectionIteratorStaticEnd::end4Statics();

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
