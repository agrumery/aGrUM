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


#include <agrum/tools/graphicalModels/inference/scheduler/scheduledInference.h>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/graphicalModels/inference/scheduler/scheduledInference_inl.h>
#endif   // GUM_NO_INLINE

namespace gum {

  // default constructor
  ScheduledInference::ScheduledInference(const Scheduler& scheduler,
                                         Size             max_nb_threads,
                                         double           max_megabyte_memory) :
      ThreadNumberManager(max_nb_threads),
      _scheduler_(scheduler.clone()),
      _sequential_scheduler_(1, max_megabyte_memory) {
    this->setMaxNumberOfThreads(max_nb_threads);
    this->setMaxMemory(max_megabyte_memory);

    // for debugging purposes
    GUM_CONSTRUCTOR(ScheduledInference);
  }


  // copy constructor
  ScheduledInference::ScheduledInference(const ScheduledInference& from) :
      ThreadNumberManager(from), _scheduler_(from._scheduler_->clone()),
      _sequential_scheduler_(from._sequential_scheduler_) {
    // for debugging purposes
    GUM_CONS_CPY(ScheduledInference);
  }


  // move constructor
  ScheduledInference::ScheduledInference(ScheduledInference&& from) :
      ThreadNumberManager(std::move(from)),
      _scheduler_(from._scheduler_),
      _sequential_scheduler_(from._sequential_scheduler_) {
    from._scheduler_ = nullptr;

    // for debugging purposes
    GUM_CONS_MOV(ScheduledInference);
  }


  // copy operator
  ScheduledInference& ScheduledInference::operator=(const ScheduledInference& from) {
    if (this != &from) {
      ThreadNumberManager::operator=(std::move(from));
      delete _scheduler_;
      _scheduler_ = from._scheduler_->clone();
      _sequential_scheduler_.setMaxMemory(from._sequential_scheduler_.maxMemory());
    }
    return *this;
  }

  // move operator
  ScheduledInference& ScheduledInference::operator=(ScheduledInference&& from) {
    if (this != &from) {
      ThreadNumberManager::operator=(from);
      delete _scheduler_;
      _scheduler_ = from._scheduler_->clone();
      _sequential_scheduler_.setMaxMemory(from._sequential_scheduler_.maxMemory());
    }
    return *this;
  }


  // virtual copy constructor
  ScheduledInference* ScheduledInference::clone() const { return new ScheduledInference(*this); }


  // destructor
  ScheduledInference::~ScheduledInference() {
    if (_scheduler_ != nullptr) delete _scheduler_;

    // for debugging purposes
    GUM_DESTRUCTOR(ScheduledInference);
  }


  // sets a new scheduler
  void ScheduledInference::setScheduler(const Scheduler& scheduler) {
    const auto max_nb_threads =
       this->isNbThreadsUserDefined() ? this->getMaxNumberOfThreads() : 0;
    const auto max_memory = _scheduler_->maxMemory();

    delete _scheduler_;

    _scheduler_ = scheduler.clone();
    _scheduler_->setMaxNumberOfThreads(max_nb_threads);
    _scheduler_->setMaxMemory(max_memory);
  }

}   // namespace gum
