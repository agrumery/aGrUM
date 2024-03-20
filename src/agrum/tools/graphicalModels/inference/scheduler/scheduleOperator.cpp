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


/**
 * @file
 * @brief the base class for "low-level" operators used to schedule inferences
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <agrum/tools/graphicalModels/inference/scheduler/scheduleOperator.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/tools/graphicalModels/inference/scheduler/scheduleOperator_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  /// default constructor
  ScheduleOperator::ScheduleOperator(const ScheduleOperatorType type,
                                     const bool                 imply_deletion,
                                     const bool                 are_results_persistent) :
      _op_type_(type), _imply_deletion_(imply_deletion),
      _result_persistent_(are_results_persistent) {
    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduleOperator);
  }

  /// copy constructor
  ScheduleOperator::ScheduleOperator(const ScheduleOperator& from) :
      _op_type_(from._op_type_), _imply_deletion_(from._imply_deletion_),
      _result_persistent_(from._result_persistent_) {
    // for debugging purposes
    GUM_CONS_CPY(ScheduleOperator);
  }

  /// move constructor
  ScheduleOperator::ScheduleOperator(ScheduleOperator&& from) :
      _op_type_(from._op_type_), _imply_deletion_(from._imply_deletion_),
      _result_persistent_(from._result_persistent_) {
    // for debugging purposes
    GUM_CONS_MOV(ScheduleOperator);
  }

  /// destructor
  ScheduleOperator::~ScheduleOperator() {
    // for debugging purposes
    GUM_DESTRUCTOR(ScheduleOperator);
  }

  /// copy operator
  ScheduleOperator& ScheduleOperator::operator=(const ScheduleOperator& from) {
    _op_type_           = from._op_type_;
    _imply_deletion_    = from._imply_deletion_;
    _result_persistent_ = from._result_persistent_;
    return *this;
  }

  /// move operator
  ScheduleOperator& ScheduleOperator::operator=(ScheduleOperator&& from) {
    _op_type_           = from._op_type_;
    _imply_deletion_    = from._imply_deletion_;
    _result_persistent_ = from._result_persistent_;
    return *this;
  }

}   // namespace gum

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
