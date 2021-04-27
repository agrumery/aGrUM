/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Outlined implementation of Bijections
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <string>

#include <agrum/tools/core/bijection.h>

#ifdef GUM_DEBUG_MODE
#  include <agrum/tools/core/debug.h>
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  // creates (if needed) and returns the safe iterator BijectionIterEndSafe__
  const BijectionIteratorSafe< int, int >*
     BijectionIteratorStaticEnd::endSafe4Statics() {
    static bool first_time = true;

    if (first_time) {
      first_time             = false;
      BijectionIterEndSafe__ = new BijectionIteratorSafe< int, int >;
#  ifdef GUM_DEBUG_MODE
      __debug__::dec_creation__("BijectionIteratorSafe",
                                "__bijection_static_end",
                                0,
                                "static variable correction",
                                0);
      __debug__::dec_creation__("HashTableConstIteratorSafe",
                                "__bijection_static_end",
                                0,
                                "static variable correction",
                                0);
#  endif
    }

    return BijectionIterEndSafe__;
  }

  // creates (if needed) and returns the unsafe iterator BijectionIterEnd__
  const BijectionIterator< int, int >* BijectionIteratorStaticEnd::end4Statics() {
    static bool first_time = true;

    if (first_time) {
      first_time         = false;
      BijectionIterEnd__ = new BijectionIterator< int, int >;
#  ifdef GUM_DEBUG_MODE
      __debug__::dec_creation__("BijectionIterator",
                                "__bijection_static_end",
                                0,
                                "static variable correction",
                                0);
      __debug__::dec_creation__("HashTableConstIterator",
                                "__bijection_static_end",
                                0,
                                "static variable correction",
                                0);
#  endif
    }

    return BijectionIterEnd__;
  }

  // create the end iterator for all Bijections
  const BijectionIteratorSafe< int, int >*
     BijectionIteratorStaticEnd::BijectionIterEndSafe__
     = BijectionIteratorStaticEnd::endSafe4Statics();

  // create the end iterator for all Bijections
  const BijectionIterator< int, int >*
     BijectionIteratorStaticEnd::BijectionIterEnd__
     = BijectionIteratorStaticEnd::end4Statics();

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
template class gum::Bijection< int, int >;
#  endif
#endif
#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
template class gum::Bijection< std::string, std::string >;
#  endif
#endif
