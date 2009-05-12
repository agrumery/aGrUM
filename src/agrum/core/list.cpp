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
#include <agrum/core/inline.h>
#include <agrum/core/list.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {

  
  // ==============================================================================
  /// Destructor for end/rend
  // ==============================================================================
  template <> ListIterator<Debug>::~ListIterator()  {
  }

  // ==============================================================================
  /// constructor for end/rend
  // ==============================================================================
  template <> ListIterator<Debug>::ListIterator()  :
    list( 0 ), bucket( 0 ), next_current_bucket( 0 ), prev_current_bucket( 0 ),
    null_pointing( false ), container( 0 ) {
  }

  // an iterator that represents both end and rend for all the Lists
  // (whatever their type). This is mainly what stroustrup suggests
  // in his C++ programming language, third edition, page 854
  const ListIterator<Debug> __list_end;


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

