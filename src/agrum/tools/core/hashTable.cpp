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
 * @brief Outlined implementation of hash tables.
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
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
      first_time         = false;
      HashTableIterEnd__ = new HashTableIterator< int, int >;
#  ifdef GUM_DEBUG_MODE
      __debug__::dec_creation__("HashTableIterator",
                                "__hash_static_end",
                                0,
                                "static variable correction",
                                0);
      __debug__::dec_creation__("HashTableConstIterator",
                                "__hash_static_end",
                                0,
                                "static variable correction",
                                0);
#  endif
    }

    return HashTableIterEnd__;
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
      first_time             = false;
      HashTableIterEndSafe__ = new HashTableIteratorSafe< int, int >;
#  ifdef GUM_DEBUG_MODE
      __debug__::dec_creation__("HashTableIteratorSafe",
                                "__hash_static_end",
                                0,
                                "static variable correction",
                                0);
      __debug__::dec_creation__("HashTableConstIteratorSafe",
                                "__hash_static_end",
                                0,
                                "static variable correction",
                                0);
#  endif
    }

    return HashTableIterEndSafe__;
  }

  const HashTableConstIteratorSafe< int, int >*
     HashTableIteratorStaticEnd::constEndSafe4Statics() {
    return reinterpret_cast< const HashTableConstIteratorSafe< int, int >* >(
       endSafe4Statics());
  }

  const HashTableIterator< int, int >*
     HashTableIteratorStaticEnd::HashTableIterEnd__
     = HashTableIteratorStaticEnd::end4Statics();

  const HashTableIteratorSafe< int, int >*
     HashTableIteratorStaticEnd::HashTableIterEndSafe__
     = HashTableIteratorStaticEnd::endSafe4Statics();

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
