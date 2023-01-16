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
 * @brief A scheduler that executes available operators in parallel
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef GUM_SCHEDULER_PARALLEL_H
#define GUM_SCHEDULER_PARALLEL_H

#include <vector>
#include <algorithm>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <chrono>

#include <agrum/agrum.h>
#include <agrum/tools/core/priorityQueue.h>
#include <agrum/tools/core/threadExecutor.h>
#include <agrum/tools/graphicalModels/inference/scheduler/schedulerSequential.h>

namespace gum {

  /**
   * @class SchedulerParallel
   * @brief A scheduler that executes available operators in parallel
   * @headerfile schedulerParallel.h <agrum/tools/graphicalModels/inference/scheduler/schedulerParallel.h>
   */
  class SchedulerParallel: public Scheduler {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    explicit SchedulerParallel(Size max_nb_threads = 0, double max_megabyte_memory = 0.0);

    /// copy constructor
    SchedulerParallel(const SchedulerParallel& from);

    /// move constructor
    SchedulerParallel(SchedulerParallel&& from);

    /// virtual copy constructor
    SchedulerParallel* clone() const final;

    /// destructor
    virtual ~SchedulerParallel();

    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// sets the number of threads that can be used by the scheduler
    /**
     * @param nb the number of threads used for executing schedules. If this
     * number is set to 0, then it is defaulted to aGrUM's number of threads
     */
    void setNumberOfThreads(Size nb) final;

    /// sets an upper bound on the memory consumption admissible
    /**
     * @param megabytes the number of megabytes that can be used for the
     * execution of the schedule. If this number is equal to 0, then no
     * memory limit is applied.
     */
    void setMaxMemory(double megabytes) final;

    /// execute all the operations of a given schedule
    void execute(Schedule&) final;

    /** @brief returns an estimation of the number of elementary operations
     * needed to perform a given schedule */
    double nbOperations(const Schedule&) final;

    /// returns the memory consumption used during the execution of a schedule
    /** Actually, this function does not return a precise account of the memory
     * used to perform the schedule but a rough estimate based on the sizes
     * of the tables involved in the schedule.
     * @return a pair of memory consumption: the first one is the maximum
     * amount of memory used during the execution of the Schedule and the second
     * one is the amount of memory still used at the end of the execution of
     * the schedule */
    std::pair< double, double > memoryUsage(const Schedule&) final;

    /// @}


    private:
    /// a scheduler used for computing the number of operations and memory usage
    SchedulerSequential _sequential_scheduler_;
  };

} /* namespace gum */

// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/tools/graphicalModels/inference/scheduler/schedulerParallel_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_SCHEDULER_PARALLEL_H */
