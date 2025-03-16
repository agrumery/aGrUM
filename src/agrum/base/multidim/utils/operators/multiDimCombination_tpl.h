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
 * @brief A generic class to combine efficiently several MultiDim tables.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>

namespace gum {

  // Constructor
  template < class TABLE >
  MultiDimCombination< TABLE >::MultiDimCombination() {
    GUM_CONSTRUCTOR(MultiDimCombination);
  }

  // Copy constructor
  template < class TABLE >
  MultiDimCombination< TABLE >::MultiDimCombination(const MultiDimCombination< TABLE >& from) {
    GUM_CONS_CPY(MultiDimCombination);
  }

  // Destructor
  template < class TABLE >
  MultiDimCombination< TABLE >::~MultiDimCombination() {
    GUM_DESTRUCTOR(MultiDimCombination);
  }

  // add to a given schedule the set of operations needed to perform the combination
  template < class TABLE >
  const IScheduleMultiDim*
      MultiDimCombination< TABLE >::schedule(Schedule& schedule,
                                             const std::vector< const IScheduleMultiDim* >& set,
                                             const bool is_result_persistent) const {
    // compute the set of operations and store it into the schedule
    auto ops_plus_res = operations(set, false);
    for (const auto op: ops_plus_res.first) {
      schedule.insertOperation(*op);
    }

    // get the result of the schedule and, if required, make it persistent in the
    // operation that created it
    const IScheduleMultiDim* table = schedule.scheduleMultiDim(ops_plus_res.second->id());
    if (is_result_persistent) {
      const auto creating_op = schedule.scheduleMultiDimCreator(table);
      if (creating_op != nullptr) {
        const_cast< ScheduleOperator* >(creating_op)->makeResultsPersistent(true);
      }
    }

    // free the operations: they are no more necessary since we already copied
    // them into the schedule
    for (auto op: ops_plus_res.first)
      delete op;

    return table;
  }

  // add to a given schedule the set of operations needed to perform the combination
  template < class TABLE >
  INLINE const IScheduleMultiDim*
      MultiDimCombination< TABLE >::schedule(Schedule&                              schedule,
                                             const Set< const IScheduleMultiDim* >& set,
                                             const bool is_result_persistent) const {
    std::vector< const IScheduleMultiDim* > vect;
    vect.reserve(set.size());
    for (const auto elt: set) {
      vect.push_back(elt);
    }
    return this->schedule(schedule, vect, is_result_persistent);
  }


} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
