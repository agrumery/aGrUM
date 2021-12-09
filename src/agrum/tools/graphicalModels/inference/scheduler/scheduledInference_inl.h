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

#include<agrum/tools/graphicalModels/inference/scheduler/scheduledInference.h>

namespace gum {

  // returns the current scheduler
  INLINE Scheduler<>& ScheduledInference::scheduler() const {
    return *_scheduler_;
  }


  // sets the number max of threads that can be used
  INLINE void ScheduledInference::setMaxNbThreads(Size nb) {
    if (nb == Size(0)) nb = gum::getMaxNumberOfThreads();
    _max_nb_threads_ = nb;
    _scheduler_->setMaxNbThreads(nb);
  }


  // returns the current max number of threads of the scheduler
  INLINE Size ScheduledInference::maxNbThreads() const {
    return _max_nb_threads_;
  }


  // sets an upper bound on the memory consumption admissible
  INLINE void ScheduledInference::setMaxMemory(double megabytes) {
    if (megabytes < 0.0) megabytes = 0.0;
    _max_megabyte_memory_ = megabytes;
    _scheduler_->setMaxMemory(megabytes);
  }


  // returns the Max memory (in megabytes) available for executing schedules
  INLINE double ScheduledInference::maxMemory() const {
    return _max_megabyte_memory_;
  }

} /* namespace gum */
