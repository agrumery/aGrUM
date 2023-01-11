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

#include <agrum/tools/graphicalModels/inference/scheduler/scheduledInference.h>

namespace gum {

  // returns the current scheduler
  INLINE Scheduler& ScheduledInference::scheduler() const {
    if (ThreadNumberManager::getNumberOfThreads() != 1)
      return const_cast< SchedulerParallel& >(_scheduler_parallel_);
    else return const_cast< SchedulerSequential& >(_scheduler_sequential_);
  }


  // sets the number max of threads that can be used
  INLINE void ScheduledInference::setNumberOfThreads(Size nb) {
    ThreadNumberManager::setNumberOfThreads(nb);
    _scheduler_parallel_.setNumberOfThreads(nb);
  }


  // sets an upper bound on the memory consumption admissible
  INLINE void ScheduledInference::setMaxMemory(double megabytes) {
    if (megabytes < 0.0) megabytes = 0.0;
    _scheduler_parallel_.setMaxMemory(megabytes);
    _scheduler_sequential_.setMaxMemory(megabytes);
  }


  // returns the Max memory (in megabytes) available for executing schedules
  INLINE double ScheduledInference::maxMemory() const { return _scheduler_parallel_.maxMemory(); }

} /* namespace gum */
