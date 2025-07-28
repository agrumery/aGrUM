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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


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
  const ScheduleOperator& Schedule::emplaceProjection(const ScheduleMultiDim< TABLE >& table,
                                                      const gum::VariableSet&          del_vars,
                                                      TABLE (*project)(const TABLE&,
                                                                       const gum::VariableSet&),
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
