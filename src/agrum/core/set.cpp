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


#include <agrum/core/set.h>


namespace gum {


  // creates (if needed) and returns the iterator __SetIterEnd
  const SetIteratorSafe<int>* SetIteratorStaticEnd::endSafe4Statics() {
    static bool first_time = true;

    if ( first_time ) {
      first_time = false;
      __SetIterEndSafe = new SetIteratorSafe<int>;
    }

    return __SetIterEndSafe;
  }


  // creates (if needed) and returns the iterator __SetIterEnd
  const SetIteratorSafe<int>* SetIteratorStaticEnd::constEndSafe4Statics() {
    return endSafe4Statics();
  }


  // create the end iterator for all Sets
  const SetIteratorSafe<int>* SetIteratorStaticEnd::__SetIterEndSafe =
    SetIteratorStaticEnd::endSafe4Statics();


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
