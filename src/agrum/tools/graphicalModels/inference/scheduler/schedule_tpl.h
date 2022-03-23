/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Class containing a schedule of operations to perform
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>

namespace gum {

  /// inserts a new table as a source multidim
  template < typename TABLE >
  const IScheduleMultiDim*
     Schedule::insertTable(const TABLE& table, const bool copy, const Idx id) {
    // if the schedule already contains the id, throw an error
    if (_multidim2id_.existsSecond(id)) {
      GUM_ERROR(DuplicateScheduleMultiDim,
                "A table with Id " << id << " already exists in the schedule");
    }

    // allocate the new table within the schedule
    ScheduleMultiDim< TABLE >* new_multidim = new ScheduleMultiDim< TABLE >(table, copy, id);

    // keep track that this is a source multidim
    _multidim2nodes_.insert(new_multidim, NodeSet());
    _multidim_location_.insert(new_multidim, std::pair< ScheduleOperator*, Idx >(nullptr, Idx(0)));
    _multidim2id_.insert(new_multidim, new_multidim->id());

    // indicate that the schedule has been modified
    ++_version_number_;

    return new_multidim;
  }


  /// emplace a new schedule binary combination operator
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  const ScheduleOperator&
     Schedule::emplaceBinaryCombination(const ScheduleMultiDim< TABLE1 >& table1,
                                        const ScheduleMultiDim< TABLE2 >& table2,
                                        TABLE_RES (*combine)(const TABLE1&, const TABLE2&),
                                        const bool is_result_persistent) {
    // note that the insertOperation will check that table1 and table2
    // already belong to the schedule
    return insertOperation(
       ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >(table1, table2, combine, false),
       is_result_persistent);
  }


  /// emplace a new schedule projection operator
  template < typename TABLE >
  const ScheduleOperator& Schedule::emplaceProjection(
     const ScheduleMultiDim< TABLE >&      table,
     const Set< const DiscreteVariable* >& del_vars,
     TABLE (*project)(const TABLE&, const Set< const DiscreteVariable* >&),
     const bool is_result_persistent) {
    // note that the insertOperation will check that table already belongs
    // to the schedule
    return insertOperation(ScheduleProjection< TABLE >(table, del_vars, project, false),
                           is_result_persistent);
  }


  /// emplace a new schedule deletion operation
  template < typename TABLE >
  const ScheduleOperator& Schedule::emplaceDeletion(const ScheduleMultiDim< TABLE >& table) {
    // note that the insertOperation will check that table already belongs
    // to the schedule
    return insertOperation(ScheduleDeletion< TABLE >(table), false);
  }


  /// emplace a new schedule storage operation
  template < typename TABLE, template < typename... > class CONTAINER >
  const ScheduleOperator& Schedule::emplaceStorage(const IScheduleMultiDim& table,
                                                   CONTAINER< TABLE >&      container) {
    // note that the insertOperation will check that table already belongs
    // to the schedule
    return insertOperation(ScheduleStorage< TABLE, CONTAINER >(table, container), false);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
