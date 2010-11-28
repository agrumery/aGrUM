/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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


#ifndef DOXYGEN_SHOULD_SKIP_THIS


#include <agrum/core/debug.h>


namespace gum {


  /// default constructor
  template <typename T_DATA>
  ScheduleOperation<T_DATA>::ScheduleOperation ( Type t) :
    __type ( t ) {
    // for debugging purposes
    GUM_CONSTRUCTOR ( ScheduleOperation );
  }
  

  /// destructor
  template <typename T_DATA>
  ScheduleOperation<T_DATA>::~ScheduleOperation () {
    // for debugging purposes
    GUM_DESTRUCTOR ( ScheduleOperation );
  }


  /// returns the name of the operation
  template <typename T_DATA>
  INLINE typename ScheduleOperation<T_DATA>::Type
  ScheduleOperation<T_DATA>::type () const {
    return __type;
  }  


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
