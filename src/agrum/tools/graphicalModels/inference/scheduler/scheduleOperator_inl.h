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
 * @brief the base class for "low-level" operators used to schedule inferences
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>
#  include <agrum/tools/graphicalModels/inference/scheduler/scheduleOperator.h>

namespace gum {

  /// operator ==
  INLINE bool ScheduleOperator::operator==(const ScheduleOperator& op) const {
    return (_op_type_ == op._op_type_);
  }


  /// operator !=
  INLINE bool ScheduleOperator::operator!=(const ScheduleOperator& op) const {
    return !ScheduleOperator::operator==(op);
  }


  /// checks whether two ScheduleOperator are similar
  INLINE bool ScheduleOperator::isSameOperator(const ScheduleOperator& op) const {
    return (_op_type_ == op._op_type_);
  }


  /// returns the name of the operator
  INLINE ScheduleOperatorType ScheduleOperator::type() const { return _op_type_; }


  /// indicates whether the operator deletes some of its arguments
  INLINE bool ScheduleOperator::implyDeletion() const { return _imply_deletion_; }


  /// makes the results of the operator persistent or not
  INLINE void ScheduleOperator::makeResultsPersistent(const bool is_persistent) {
    _result_persistent_ = is_persistent;
  }


  /// shows whether the operator has persistent results
  INLINE bool ScheduleOperator::hasPersistentResults() const { return _result_persistent_; }


} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
