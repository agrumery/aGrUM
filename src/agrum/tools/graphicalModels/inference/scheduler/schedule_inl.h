/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/tools/graphicalModels/inference/scheduler/schedule.h>

namespace gum {

  /// returns a new distinct Schedule version
  INLINE Idx Schedule::_newVersionNumber_() { return ++_overall_version_number_; }

  /// virtual copy constructor
  INLINE Schedule* Schedule::clone() const { return new Schedule(*this); }

  /// operator !=
  INLINE bool Schedule::operator!=(const Schedule& from) const { return !operator==(from); }

  /// returns the version number of the schedule
  INLINE Idx Schedule::versionNumber() const { return _version_number_; }

  /// returns a DAG indicating in which order the operations can be performed
  INLINE const DAG& Schedule::dag() const { return _dag_; }

  /// returns the scheduleOperation corresponding to an id in the DAG
  INLINE const ScheduleOperator& Schedule::operation(const NodeId node_id) const {
    return *(_node2op_.second(node_id));
  }

  /// returns the id associated to a given operation
  INLINE NodeId Schedule::nodeId(const ScheduleOperator& op) const {
    return _node2op_.first(const_cast< ScheduleOperator* >(&op));
  }

  /// updates the DAG after a given operation has been executed
  INLINE void Schedule::updateAfterExecution(const ScheduleOperator& op,
                                             std::vector< NodeId >&  new_available_nodes,
                                             const bool              check) {
    /// check that the node belongs to the schedule
    if (check) {
      if (!_node2op_.existsSecond(const_cast< ScheduleOperator* >(&op))) {
        GUM_ERROR(UnknownScheduleOperation,
                  "the schedule cannot be updated because Operation "
                     << op.toString() << " that has been executed does not belong to its DAG.");
      }
    }

    updateAfterExecution(_node2op_.first(const_cast< ScheduleOperator* >(&op)),
                         new_available_nodes,
                         check);
  }

  /// returns the ScheduleMultiDim corresponding to a given id
  INLINE const IScheduleMultiDim* Schedule::scheduleMultiDim(const NodeId id) const {
    return _multidim2id_.first(id);
  }

  /// indicates whether the schedule contains a given ScheduleMultiDim
  INLINE bool Schedule::existsScheduleMultiDim(const NodeId id) const {
    return _multidim2id_.existsSecond(id);
  }

  /// returns the id of a given IScheduleMultiDim
  INLINE NodeId Schedule::scheduleMultiDimId(const IScheduleMultiDim* multidim) const {
    return _multidim2id_.second(multidim);
  }

  /// returns the operation, if any, that created a given scheduleMultiDim
  INLINE const ScheduleOperator*
     Schedule::scheduleMultiDimCreator(const IScheduleMultiDim* multidim) const {
    return _multidim_location_[multidim].first;
  }

  /// returns the operation, if any, that created a given scheduleMultiDim
  INLINE const ScheduleOperator* Schedule::scheduleMultiDimCreator(const NodeId id) const {
    return scheduleMultiDimCreator(_multidim2id_.first(id));
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
