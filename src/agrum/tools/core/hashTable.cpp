
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief Outlined implementation of hash tables.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/tools/core/hashTable.h>

#ifdef GUM_DEBUG_MODE
#  include <agrum/tools/core/debug.h>
#endif

namespace gum {

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  const HashTableIterator< int, int >* HashTableIteratorStaticEnd::end4Statics() {
    static bool first_time = true;

    if (first_time) {
      first_time = false;
      __HashTableIterEnd = new HashTableIterator< int, int >;
#  ifdef GUM_DEBUG_MODE
      __debug__::__dec_creation("HashTableIterator",
                                "__hash_static_end",
                                0,
                                "static variable correction",
                                0);
      __debug__::__dec_creation("HashTableConstIterator",
                                "__hash_static_end",
                                0,
                                "static variable correction",
                                0);
#  endif
    }

    return __HashTableIterEnd;
  }

  const HashTableConstIterator< int, int >*
     HashTableIteratorStaticEnd::constEnd4Statics() {
    return reinterpret_cast< const HashTableConstIterator< int, int >* >(
       end4Statics());
  }

  const HashTableIteratorSafe< int, int >*
     HashTableIteratorStaticEnd::endSafe4Statics() {
    static bool first_time = true;

    if (first_time) {
      first_time = false;
      __HashTableIterEndSafe = new HashTableIteratorSafe< int, int >;
#  ifdef GUM_DEBUG_MODE
      __debug__::__dec_creation("HashTableIteratorSafe",
                                "__hash_static_end",
                                0,
                                "static variable correction",
                                0);
      __debug__::__dec_creation("HashTableConstIteratorSafe",
                                "__hash_static_end",
                                0,
                                "static variable correction",
                                0);
#  endif
    }

    return __HashTableIterEndSafe;
  }

  const HashTableConstIteratorSafe< int, int >*
     HashTableIteratorStaticEnd::constEndSafe4Statics() {
    return reinterpret_cast< const HashTableConstIteratorSafe< int, int >* >(
       endSafe4Statics());
  }

  const HashTableIterator< int, int >*
     HashTableIteratorStaticEnd::__HashTableIterEnd =
        HashTableIteratorStaticEnd::end4Statics();

  const HashTableIteratorSafe< int, int >*
     HashTableIteratorStaticEnd::__HashTableIterEndSafe =
        HashTableIteratorStaticEnd::endSafe4Statics();

#endif   // DOXYGEN_SHOULD_SKIP_THIS

} /* namespace gum */


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#      ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
template class gum::HashTable< int, int >;
#      endif
#    endif
#  endif
#endif
#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#      ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
template class gum::HashTable< int, std::string >;
#      endif
#    endif
#  endif
#endif
#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#      ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
template class gum::HashTable< std::string, std::string >;
#      endif
#    endif
#  endif
#endif
#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#      ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
template class gum::HashTable< std::string, int >;
#      endif
#    endif
#  endif
#endif
