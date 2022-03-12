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
 * @brief a parallel scheduler that executes available operations
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/tools/graphicalModels/inference/scheduler/schedulerParallel.h>

namespace gum {

  /// sets an upper bound on the memory consumption admissible
  INLINE void SchedulerParallel::setMaxMemory(double megabytes) {
    Scheduler::setMaxMemory(megabytes);
    _sequential_scheduler_.setMaxMemory(megabytes);
  }


  /// sets the number max of threads that can be used
  INLINE void SchedulerParallel::setNumberOfThreads(Size nb) {
    Scheduler::setNumberOfThreads(nb);
    _sequential_scheduler_.setNumberOfThreads(nb);
  }


  /** @bried returns an estimation of the number of elementary operations needed
   * to perform a given schedule */
  INLINE double SchedulerParallel::nbOperations(const Schedule& schedule) {
    return _sequential_scheduler_.nbOperations(schedule);
  }


  /// returns the memory consumption used during the execution of a schedule
  INLINE std::pair< double, double > SchedulerParallel::memoryUsage(const Schedule& schedule) {
    return _sequential_scheduler_.memoryUsage(schedule);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
