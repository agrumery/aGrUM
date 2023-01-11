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

/** @file
 * @brief a scheduler that executes any available operation (chosen aribtrarily)
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/tools/graphicalModels/inference/scheduler/schedulerSequential.h>

namespace gum {

  /// sets an upper bound on the memory consumption admissible
  INLINE void SchedulerSequential::setMaxMemory(double megabytes) {
    if (megabytes != this->maxMemory()) {
      Scheduler::setMaxMemory(megabytes);
      _operations_up_to_date_ = false;
    }
  }


  /// sets a new schedule
  INLINE void SchedulerSequential::_setSchedule_(const Schedule& schedule) {
    if (&schedule != _schedule_) { _schedule_ = const_cast< Schedule* >(&schedule); }
    _simulateExecution_();
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
