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
 * @brief outlined implementations for the chained lists
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <agrum/config.h>

#include <agrum/core/list.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

#define GCC_STR(s) #s
#define GCC_JOINSTR(x, y) GCC_STR(x##y)

#if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 405
#define GCC_DIAG_DO_PRAGMA(x) _Pragma(#x)
#define GCC_DIAG_PRAGMA(x) GCC_DIAG_DO_PRAGMA(GCC diagnostic x)
#if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 406
#define GCC_DIAG_OFF(x)                                                             \
  GCC_DIAG_PRAGMA(push) GCC_DIAG_PRAGMA(ignored GCC_JOINSTR(-W, x))
#define GCC_DIAG_ON(x) GCC_DIAG_PRAGMA(pop)
#else
#define GCC_DIAG_OFF(x) GCC_DIAG_PRAGMA(ignored GCC_JOINSTR(-W, x))
#define GCC_DIAG_ON(x) GCC_DIAG_PRAGMA(warning GCC_JOINSTR(-W, x))
#endif
#else
#define GCC_DIAG_OFF(x)
#define GCC_DIAG_ON(x)
#endif

  // GCC_DIAG_OFF(strict-aliasing)
  // GCC_DIAG_ON(strict-aliasing)

  /// Destructor for end/rend
  template <> ListConstIteratorSafe<Debug>::~ListConstIteratorSafe() {}

  /// constructor for end/rend
  template <> ListConstIteratorSafe<Debug>::ListConstIteratorSafe() noexcept {}

  /// Destructor for end/rend
  template <> ListConstIterator<Debug>::~ListConstIterator() noexcept {}

  /// constructor for end/rend
  template <> ListConstIterator<Debug>::ListConstIterator() noexcept {}

  // an iterator that represents both end and rend for all the Lists
  // (whatever their type). This is mainly what stroustrup suggests
  // in his C++ programming language, third edition, page 854
  static const ListConstIteratorSafe<Debug> __static_list_end_safe;
  static const ListConstIterator<Debug> __static_list_end;

  static constexpr const void *__get_list_end_safe() {
    return &__static_list_end_safe;
  }
  static constexpr const void *__get_list_end() { return &__static_list_end; }

  const void *const __list_end_safe = __get_list_end_safe();
  const void *const __list_end = __get_list_end();

  // to optimize compile-link time, provide the usual lists
  template class List<bool>;
  template class List<int>;
  template class List<unsigned int>;

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
