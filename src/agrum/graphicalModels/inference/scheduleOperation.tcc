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

#include <agrum/config.h>

namespace gum {

  /// returns a new distinct ID for each ScheduleOperation
  template <typename GUM_SCALAR> Id ScheduleOperation<GUM_SCALAR>::__newId() {
    static Id id = 0;
    return ++id;
  }

  /// default constructor
  template <typename GUM_SCALAR>
  ScheduleOperation<GUM_SCALAR>::ScheduleOperation(Type t)
      : __type(t), __id(__newId()) {
    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleOperation);
  }

  /// copy constructor
  template <typename GUM_SCALAR>
  ScheduleOperation<GUM_SCALAR>::ScheduleOperation(
      const ScheduleOperation<GUM_SCALAR> &from)
      : __type(from.__type), __id(from.__id) {
    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleOperation);
  }

  /// destructor
  template <typename GUM_SCALAR>
  ScheduleOperation<GUM_SCALAR>::~ScheduleOperation() {
    // for debugging purposes
    GUM_DESTRUCTOR(ScheduleOperation);
  }

  /// copy operator
  template <typename GUM_SCALAR>
  INLINE ScheduleOperation<GUM_SCALAR> &ScheduleOperation<GUM_SCALAR>::
  operator=(const ScheduleOperation<GUM_SCALAR> &from) {
    __type = from.__type;
    __id = from.__id;
    return *this;
  }

  /// returns the name of the operation
  template <typename GUM_SCALAR>
  INLINE typename ScheduleOperation<GUM_SCALAR>::Type
  ScheduleOperation<GUM_SCALAR>::type() const {
    return __type;
  }

  /// returns the id of the operation
  template <typename GUM_SCALAR>
  INLINE Id ScheduleOperation<GUM_SCALAR>::id() const {
    return __id;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
