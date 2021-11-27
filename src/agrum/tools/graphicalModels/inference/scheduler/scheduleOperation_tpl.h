/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) et Christophe GONZALES(@AMU)
 *  info_at_agrum_dot_org
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

/** @file
 * @brief the base class for "low-level" operations used to schedule inferences
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>
#  include <agrum/tools/graphicalModels/inference/scheduler/scheduleOperation.h>

namespace gum {

  /// returns the allocator used by the operation
  template < template < typename > class ALLOC >
  INLINE typename ScheduleOperation< ALLOC >::allocator_type
     ScheduleOperation< ALLOC >::get_allocator() const {
    return *this;
  }


  /// default constructor
  template < template < typename > class ALLOC >
  ScheduleOperation< ALLOC >::ScheduleOperation(
     const ScheduleOperationType                                type,
     const bool                                                 imply_deletion,
     const bool                                                 are_results_persistent,
     const typename ScheduleOperation< ALLOC >::allocator_type& alloc) :
      ALLOC< Idx >(alloc),
      _op_type_(type), _imply_deletion_(imply_deletion),
      _result_persistent_(are_results_persistent) {
    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleOperation);
  }


  /// copy constructor with  a given allocator
  template < template < typename > class ALLOC >
  ScheduleOperation< ALLOC >::ScheduleOperation(
     const ScheduleOperation< ALLOC >&                          from,
     const typename ScheduleOperation< ALLOC >::allocator_type& alloc) :
      ALLOC< Idx >(alloc),
      _op_type_(from._op_type_), _imply_deletion_(from._imply_deletion_),
      _result_persistent_(from._result_persistent_) {
    // for debugging purposes
    GUM_CONS_CPY(ScheduleOperation);
  }


  /// copy constructor
  template < template < typename > class ALLOC >
  INLINE ScheduleOperation< ALLOC >::ScheduleOperation(const ScheduleOperation< ALLOC >& from) :
      ScheduleOperation< ALLOC >(from, from.get_allocator()) {}


  /// move constructor with a given allocator
  template < template < typename > class ALLOC >
  ScheduleOperation< ALLOC >::ScheduleOperation(
     ScheduleOperation< ALLOC >&&                               from,
     const typename ScheduleOperation< ALLOC >::allocator_type& alloc) :
      ALLOC< Idx >(alloc),
      _op_type_(from._op_type_), _imply_deletion_(from._imply_deletion_),
      _result_persistent_(from._result_persistent_) {
    // for debugging purposes
    GUM_CONS_MOV(ScheduleOperation);
  }


  /// move constructor
  template < template < typename > class ALLOC >
  INLINE ScheduleOperation< ALLOC >::ScheduleOperation(ScheduleOperation< ALLOC >&& from) :
      ScheduleOperation< ALLOC >(std::move(from), from.get_allocator()) {}


  /// destructor
  template < template < typename > class ALLOC >
  ScheduleOperation< ALLOC >::~ScheduleOperation() {
    // for debugging purposes
    GUM_DESTRUCTOR(ScheduleOperation);
  }


  /// copy operator
  template < template < typename > class ALLOC >
  ScheduleOperation< ALLOC >&
     ScheduleOperation< ALLOC >::operator=(const ScheduleOperation< ALLOC >& from) {
    _op_type_           = from._op_type_;
    _imply_deletion_    = from._imply_deletion_;
    _result_persistent_ = from._result_persistent_;
    return *this;
  }


  /// move operator
  template < template < typename > class ALLOC >
  ScheduleOperation< ALLOC >&
     ScheduleOperation< ALLOC >::operator=(ScheduleOperation< ALLOC >&& from) {
    _op_type_           = from._op_type_;
    _imply_deletion_    = from._imply_deletion_;
    _result_persistent_ = from._result_persistent_;
    return *this;
  }


  /// operator ==
  template < template < typename > class ALLOC >
  INLINE bool ScheduleOperation< ALLOC >::operator==(const ScheduleOperation< ALLOC >& op) const {
    return (_op_type_ == op._op_type_);
  }


  /// operator !=
  template < template < typename > class ALLOC >
  INLINE bool ScheduleOperation< ALLOC >::operator!=(const ScheduleOperation< ALLOC >& op) const {
    return !ScheduleOperation< ALLOC >::operator==(op);
  }


  /// checks whether two ScheduleOperation are similar
  template < template < typename > class ALLOC >
  INLINE bool
     ScheduleOperation< ALLOC >::isSameOperation(const ScheduleOperation< ALLOC >& op) const {
    return (_op_type_ == op._op_type_);
  }


  /// returns the name of the operation
  template < template < typename > class ALLOC >
  INLINE ScheduleOperationType ScheduleOperation< ALLOC >::type() const {
    return _op_type_;
  }


  /// indicates wether the operation deletes some of its arguments
  template < template < typename > class ALLOC >
  INLINE bool ScheduleOperation< ALLOC >::implyDeletion() const {
    return _imply_deletion_;
  }


  /// makes the results of the operation persistent or not
  template < template < typename > class ALLOC >
  INLINE void ScheduleOperation< ALLOC >::makeResultsPersistent(const bool is_persistent) {
    _result_persistent_ = is_persistent;
  }


  /// shows whether the operation has persistent results
  template < template < typename > class ALLOC >
  INLINE bool ScheduleOperation< ALLOC >::hasPersistentResults() const {
    return _result_persistent_;
  }


} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
