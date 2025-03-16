/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/





/**
 * @file
 * @brief the base class for "low-level" operators used to schedule inferences
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <agrum/base/graphicalModels/inference/scheduler/scheduleOperator.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/base/graphicalModels/inference/scheduler/scheduleOperator_inl.h>
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
