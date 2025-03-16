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





/** @file
 * @brief A generic interface to combine and project efficiently MultiDim tables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>

namespace gum {

  // default constructor
  template < class TABLE >
  MultiDimCombineAndProject< TABLE >::MultiDimCombineAndProject() {
    // for debugging purposes
    GUM_CONSTRUCTOR(MultiDimCombineAndProject);
  }

  // copy constructor
  template < class TABLE >
  MultiDimCombineAndProject< TABLE >::MultiDimCombineAndProject(
      const MultiDimCombineAndProject< TABLE >&) {
    // for debugging purposes
    GUM_CONS_CPY(MultiDimCombineAndProject);
  }

  // destructor
  template < class TABLE >
  MultiDimCombineAndProject< TABLE >::~MultiDimCombineAndProject() {
    // for debugging purposes
    GUM_DESTRUCTOR(MultiDimCombineAndProject);
  }

  // add to a given schedule the set of operations needed
  template < class TABLE >
  Set< const IScheduleMultiDim* > MultiDimCombineAndProject< TABLE >::schedule(
      Schedule&                              schedule,
      const Set< const IScheduleMultiDim* >& original_tables,
      const gum::VariableSet&                del_vars,
      const bool                             is_result_persistent) const {
    // compute the set of operations to perform and insert them into the schedule
    auto ops_res = operations(original_tables, del_vars, false);
    for (const auto op: ops_res.first)
      schedule.insertOperation(*op);

    // get the results actually stored into the schedule
    Set< const IScheduleMultiDim* > result(ops_res.second.size());
    if (!is_result_persistent) {
      for (const auto pot: ops_res.second)
        result.insert(schedule.scheduleMultiDim(pot->id()));
    } else {
      for (const auto pot: ops_res.second) {
        const IScheduleMultiDim* table       = schedule.scheduleMultiDim(pot->id());
        const auto               creating_op = schedule.scheduleMultiDimCreator(table);
        if (creating_op != nullptr) {
          const_cast< ScheduleOperator* >(creating_op)->makeResultsPersistent(true);
        }
        result.insert(table);
      }
    }

    // free memory
    for (const auto op: ops_res.first)
      delete op;

    return result;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
