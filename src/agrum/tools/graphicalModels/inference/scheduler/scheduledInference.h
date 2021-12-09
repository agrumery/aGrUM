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
 * @brief The class enabling flexible inferences using schedules
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef GUM_SCHEDULED_INFERENCE_H
#define GUM_SCHEDULED_INFERENCE_H

#include <agrum/agrum.h>
#include <agrum/tools/graphicalModels/inference/scheduler/schedulerSequential.h>
#include <agrum/tools/graphicalModels/inference/scheduler/schedulerParallel.h>

namespace gum {

  /**
   * @class ScheduledInference
   * <agrum/tools/graphicalModels/inference/scheduler/scheduledInference.h>
   * @brief the class containing the scheduler used by schedule-based inferences
   * @ingroup gm_group
   */
  class ScheduledInference {
    public:

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    ScheduledInference(const Scheduler<>& scheduler = SchedulerParallel<>(),
                       Size   max_nb_threads = 0,
                       double max_megabyte_memory = 0.0);

    /// copy constructor
    ScheduledInference(const ScheduledInference& from);

    /// move constructor
    ScheduledInference(ScheduledInference&& from);

    /// virtual copy constructor
    virtual ScheduledInference* clone() const;

    ///destructor
    virtual ~ScheduledInference();

    /// @}


    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// returns the current scheduler
    Scheduler<>& scheduler() const;

    /// sets a new scheduler
    void setScheduler(const Scheduler<>& scheduler);

    /// sets the number max of threads that can be used
    /**
     * @param nb the number max of threads used for executing schedules. If this
     * number is set to 0, then it is defaulted to aGrUM's max number
     * of threads
     */
    void setMaxNbThreads(Size nb);

    /// returns the current max number of threads of the scheduler
    Size maxNbThreads() const;

    /// sets an upper bound on the memory consumption admissible
    /**
     * @param megabytes the number of megabytes that can be used for the
     * execution of the schedule. If this number is equal to 0, then no
     * memory limit is applied.
     */
    void setMaxMemory(double megabytes);

    /// returns the Max memory (in megabytes) available for executing schedules
    /**
     * @return the Max memory (in megabytes) available for executing schedules.
     * 0 means an unlimited amount of memory
     */
    virtual double maxMemory() const;


    /// @}

    private:
    // the scheduler to be used by inferences
    Scheduler<>* _scheduler_{nullptr};

    // the max number to be used by _scheduler_
    Size _max_nb_threads_;

    // the max ammount of memory to be used during inferences (in megabytes)
    double _max_megabyte_memory_;
  };

} // namespace gum

#ifndef GUM_NO_INLINE
#include <agrum/tools/graphicalModels/inference/scheduler/scheduledInference_inl.h>
#endif   // GUM_NO_INLINE

#endif   // GUM_SCHEDULED_INFERENCE_H
