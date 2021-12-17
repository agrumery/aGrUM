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

  /// operator ==
  INLINE bool ScheduleOperation::operator==(const ScheduleOperation& op) const {
    return (_op_type_ == op._op_type_);
  }


  /// operator !=
  INLINE bool ScheduleOperation::operator!=(const ScheduleOperation& op) const {
    return !ScheduleOperation::operator==(op);
  }


  /// checks whether two ScheduleOperation are similar
  INLINE bool ScheduleOperation::isSameOperation(const ScheduleOperation& op) const {
    return (_op_type_ == op._op_type_);
  }


  /// returns the name of the operation
  INLINE ScheduleOperationType ScheduleOperation::type() const { return _op_type_; }


  /// indicates wether the operation deletes some of its arguments
  INLINE bool ScheduleOperation::implyDeletion() const { return _imply_deletion_; }


  /// makes the results of the operation persistent or not
  INLINE void ScheduleOperation::makeResultsPersistent(const bool is_persistent) {
    _result_persistent_ = is_persistent;
  }


  /// shows whether the operation has persistent results
  INLINE bool ScheduleOperation::hasPersistentResults() const { return _result_persistent_; }


} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
