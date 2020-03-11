
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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


/** @file
 * @brief Outlined implementation of Sets
 *
 * @author Christophe GONZALES (@AMU) and Pierre-Henri WUILLEMIN (@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/tools/core/set.h>

namespace gum {

  // creates (if needed) and returns the iterator __SetIterEndSafe
  const SetIteratorSafe< int >* SetIteratorStaticEnd::endSafe4Statics() {
    static bool first_time = true;

    if (first_time) {
      first_time = false;
      __SetIterEndSafe = new SetIteratorSafe< int >;
#  ifdef GUM_DEBUG_MODE
      __debug__::__dec_creation("SetIteratorSafe",
                                "__set_static_end",
                                0,
                                "static variable correction",
                                0);
      __debug__::__dec_creation("HashTableConstIteratorSafe",
                                "__set_static_end",
                                0,
                                "static variable correction",
                                0);
#  endif
    }

    return __SetIterEndSafe;
  }

  // creates (if needed) and returns the iterator __SetIterEndSafe
  const SetIteratorSafe< int >* SetIteratorStaticEnd::constEndSafe4Statics() {
    return endSafe4Statics();
  }

  // creates (if needed) and returns the iterator __SetIterEnd
  const SetIterator< int >* SetIteratorStaticEnd::end4Statics() {
    static bool first_time = true;

    if (first_time) {
      first_time = false;
      __SetIterEnd = new SetIterator< int >;
#  ifdef GUM_DEBUG_MODE
      __debug__::__dec_creation(
         "SetIterator", "__set_static_end", 0, "static variable correction", 0);
      __debug__::__dec_creation("HashTableConstIterator",
                                "__set_static_end",
                                0,
                                "static variable correction",
                                0);
#  endif
    }

    return __SetIterEnd;
  }

  // creates (if needed) and returns the iterator __SetIterEnd
  const SetIterator< int >* SetIteratorStaticEnd::constEnd4Statics() {
    return end4Statics();
  }

  // create the end iterator for all Sets
  const SetIteratorSafe< int >* SetIteratorStaticEnd::__SetIterEndSafe =
     SetIteratorStaticEnd::endSafe4Statics();

  // create the end iterator for all Sets
  const SetIterator< int >* SetIteratorStaticEnd::__SetIterEnd =
     SetIteratorStaticEnd::end4Statics();

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#      ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#        ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#          ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
template class gum::Set< int >;
#          endif
#        endif
#      endif
#    endif
#  endif
#endif
#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#      ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#        ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#          ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
template class gum::Set< long >;
#          endif
#        endif
#      endif
#    endif
#  endif
#endif
#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#      ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#        ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#          ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
template class gum::Set< unsigned int >;
#          endif
#        endif
#      endif
#    endif
#  endif
#endif
#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#      ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#        ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#          ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
template class gum::Set< unsigned long >;
#          endif
#        endif
#      endif
#    endif
#  endif
#endif

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#      ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#        ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#          ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
template class gum::Set< double >;
#          endif
#        endif
#      endif
#    endif
#  endif
#endif
#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#      ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#        ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#          ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
template class gum::Set< std::string >;
#          endif
#        endif
#      endif
#    endif
#  endif
#endif
