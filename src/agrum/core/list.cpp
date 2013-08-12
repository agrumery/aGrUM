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

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#include <agrum/core/list.inl>
#endif /* GUM_NO_INLINE */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {

#define GCC_STR(s) #s
#define GCC_JOINSTR(x,y) GCC_STR(x ## y)

#if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 405
# define GCC_DIAG_DO_PRAGMA(x) _Pragma (#x)
# define GCC_DIAG_PRAGMA(x) GCC_DIAG_DO_PRAGMA(GCC diagnostic x)
# if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 406
#  define GCC_DIAG_OFF(x) GCC_DIAG_PRAGMA(push) GCC_DIAG_PRAGMA(ignored GCC_JOINSTR(-W,x))
#  define GCC_DIAG_ON(x)  GCC_DIAG_PRAGMA(pop)
# else
#  define GCC_DIAG_OFF(x) GCC_DIAG_PRAGMA(ignored GCC_JOINSTR(-W,x))
#  define GCC_DIAG_ON(x)  GCC_DIAG_PRAGMA(warning GCC_JOINSTR(-W,x))
# endif
#else
# define GCC_DIAG_OFF(x)
# define GCC_DIAG_ON(x)
#endif

//GCC_DIAG_OFF(strict-aliasing)
//GCC_DIAG_ON(strict-aliasing)


  /// Destructor for end/rend

  template <> ListConstIterator<Debug>::~ListConstIterator()  {
  }


  /// constructor for end/rend

  template <> ListConstIterator<Debug>::ListConstIterator()  :
    __list( 0 ), __bucket( 0 ),
    __next_current_bucket( 0 ), __prev_current_bucket( 0 ),
    __null_pointing( false ), __container( 0 ) {
  }

  // an iterator that represents both end and rend for all the Lists
  // (whatever their type). This is mainly what stroustrup suggests
  // in his C++ programming language, third edition, page 854
  const ListConstIterator<Debug> __list_end;


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

