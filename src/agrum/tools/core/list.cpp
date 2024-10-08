/**
 *
 *   Copyright (c) 2005-2024 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief outlined implementations for the chained lists
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/tools/core/list.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

#  define GCC_STR(s)        #s
#  define GCC_JOINSTR(x, y) GCC_STR(x##y)

#  if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 405
#    define GCC_DIAG_DO_PRAGMA(x) Pragma_(#x)
#    define GCC_DIAG_PRAGMA(x)    GCC_DIAG_DO_PRAGMA(GCC diagnostic x)
#    if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 406
#      define GCC_DIAG_OFF(x) GCC_DIAG_PRAGMA(push) GCC_DIAG_PRAGMA(ignored GCC_JOINSTR(-W, x))
#      define GCC_DIAG_ON(x)  GCC_DIAG_PRAGMA(pop)
#    else
#      define GCC_DIAG_OFF(x) GCC_DIAG_PRAGMA(ignored GCC_JOINSTR(-W, x))
#      define GCC_DIAG_ON(x)  GCC_DIAG_PRAGMA(warning GCC_JOINSTR(-W, x))
#    endif
#  else
#    define GCC_DIAG_OFF(x)
#    define GCC_DIAG_ON(x)
#  endif

  // an iterator that represents both end and rend for all the Lists (whatever
  // their type). Here, we use the constinit keyword rather than constexpr because,
  // in debugging mode, the iterators' destructors cannot be declared as constepxr.
  constinit const ListConstIteratorSafe< Debug >
                                             _static_list_end_safe_(StaticInitializer::CONSTINIT);
  constinit const ListConstIterator< Debug > _static_list_end_(StaticInitializer::CONSTINIT);

} /* namespace gum */


#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
template class gum::List< bool >;
template class gum::List< int >;
template class gum::List< unsigned int >;
#  endif


#endif   // DOXYGEN_SHOULD_SKIP_THIS
