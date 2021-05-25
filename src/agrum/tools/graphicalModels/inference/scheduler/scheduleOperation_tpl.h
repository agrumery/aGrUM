/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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


#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>

namespace gum {

  /// returns a new distinct ID for each ScheduleOperation
  template < typename GUM_SCALAR >
  Idx ScheduleOperation< GUM_SCALAR >:: _newId_() {
    static Idx id = 0;
    return ++id;
  }

  /// default constructor
  template < typename GUM_SCALAR >
  ScheduleOperation< GUM_SCALAR >::ScheduleOperation(Type t) :
       _type_(t),  _id_( _newId_()) {
    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleOperation);
  }

  /// copy constructor
  template < typename GUM_SCALAR >
  ScheduleOperation< GUM_SCALAR >::ScheduleOperation(
     const ScheduleOperation< GUM_SCALAR >& from) :
       _type_(from. _type_),
       _id_(from. _id_) {
    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleOperation);
  }

  /// destructor
  template < typename GUM_SCALAR >
  ScheduleOperation< GUM_SCALAR >::~ScheduleOperation() {
    // for debugging purposes
    GUM_DESTRUCTOR(ScheduleOperation);
  }

  /// copy operator
  template < typename GUM_SCALAR >
  INLINE ScheduleOperation< GUM_SCALAR >&
     ScheduleOperation< GUM_SCALAR >::operator=(
        const ScheduleOperation< GUM_SCALAR >& from) {
     _type_ = from. _type_;
     _id_   = from. _id_;
    return *this;
  }

  /// returns the name of the operation
  template < typename GUM_SCALAR >
  INLINE typename ScheduleOperation< GUM_SCALAR >::Type
     ScheduleOperation< GUM_SCALAR >::type() const {
    return  _type_;
  }

  /// returns the id of the operation
  template < typename GUM_SCALAR >
  INLINE Idx ScheduleOperation< GUM_SCALAR >::id() const {
    return  _id_;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
