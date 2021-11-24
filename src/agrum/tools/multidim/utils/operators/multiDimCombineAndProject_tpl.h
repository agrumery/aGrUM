/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
  Set< const IScheduleMultiDim<>* > MultiDimCombineAndProject< TABLE >::schedule(
     Schedule<>&                              schedule,
     const Set< const IScheduleMultiDim<>* >& original_tables,
     const Set< const DiscreteVariable* >&    del_vars) const {
    // compute the set of operations to perform and insert them into the schedule
    auto ops_res = operations(original_tables, del_vars);
    for (const auto op: ops_res.first) schedule.insertOperation(*op);

    // get the results
    Set< const IScheduleMultiDim<>* > result (ops_res.second.size());
    for (const auto pot: ops_res.second)
      result.insert(schedule.scheduleMultiDim(pot->id()));

    // free memory
    for (const auto op: ops_res.first) delete op;

    return result;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
