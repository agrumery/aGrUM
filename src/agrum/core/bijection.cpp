/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *   test $Id: $                                                           *
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


#include <agrum/core/bijection.h>


namespace gum {

  // creates (if needed) and returns the iterator __BijectionIterEnd
  const BijectionIterator<int,int>* BijectionIteratorStaticEnd::end4Statics () {
    static bool first_time = true;
    if ( first_time ) {
      first_time = false;
      __BijectionIterEnd = new BijectionIterator<int,int>;
    }

    return __BijectionIterEnd; 
  }


  // create the end iterator for all Bijections
  const BijectionIterator<int,int>*
  BijectionIteratorStaticEnd::__BijectionIterEnd =
    BijectionIteratorStaticEnd::end4Statics ();
  

  
  // creates (if needed) and returns the iterator __BijectionStarIterEnd
  const BijectionIterator<int*,int*>*
  BijectionStarIteratorStaticEnd::end4Statics () {
    static bool first_time = true;
    if ( first_time ) {
      first_time = false;
      __BijectionStarIterEnd = new BijectionIterator<int*,int*>;
    }

    return __BijectionStarIterEnd;
  }


  // create the end iterator for all BijectionStars
  const BijectionIterator<int*,int*>*
  BijectionStarIteratorStaticEnd::__BijectionStarIterEnd =
    BijectionStarIteratorStaticEnd::end4Statics ();


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
