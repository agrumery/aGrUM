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
#include <agrum/tools/core/list.h>
#include <agrum/tools/core/priorityQueue.h>
#include <agrum/tools/core/threads.h>
#include <agrum/tools/core/threadExecutor.h>
#include <agrum/tools/graphicalModels/inference/scheduler/scheduler.h>
#include <agrum/tools/graphicalModels/inference/scheduler/schedulerSequential.h>

namespace gum {

  template < template < typename > class ALLOC = std::allocator >
  class SchedulerParallel: public Scheduler< ALLOC > {
    public:
    using allocator_type = ALLOC< Idx >;

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    explicit SchedulerParallel(Size                  max_nb_threads      = 0,
                               double                max_megabyte_memory = 0.0,
                               const allocator_type& alloc               = allocator_type());

    /// copy constructor
    SchedulerParallel(const SchedulerParallel< ALLOC >& from);

    /// copy constructor with a given allocator
    SchedulerParallel(const SchedulerParallel< ALLOC >& from, const allocator_type& alloc);

    /// move constructor
    SchedulerParallel(SchedulerParallel< ALLOC >&& from);

    /// move constructor with a given allocator
    SchedulerParallel(SchedulerParallel< ALLOC >&& from, const allocator_type& alloc);

    /// virtual copy constructor
    SchedulerParallel< ALLOC >* clone() const final;

    /// virtual copy constructor with a given allocator
    SchedulerParallel< ALLOC >* clone(const allocator_type& alloc) const final;

    /// destructor
    virtual ~SchedulerParallel();

    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// sets the number max of threads that can be used
    /**
     * @param nb the number max of threads used for executing schedules. If this
     * number is set to 0, then it is defaulted to aGrUM's max number
     * of threads
     */
    void setMaxNbThreads(Size nb) final;

    /// sets an upper bound on the memory consumption admissible
    /**
     * @param megabytes the number of megabytes that can be used for the
     * execution of the schedule. If this number is equal to 0, then no
     * memory limit is applied.
     */
    void setMaxMemory(double megabytes) final;

    /// execute all the operations of a given schedule
    void execute(Schedule< ALLOC >&) final;

    /** @brief returns an estimation of the number of elementary operations
     * needed to perform a given schedule */
    double nbOperations(const Schedule< ALLOC >&) final;

    /// returns the memory consumption used during the execution of a schedule
    /** Actually, this function does not return a precise account of the memory
     * used to perform the schedule but a rough estimate based on the sizes
     * of the tables involved in the schedule.
     * @return a pair of memory consumption: the first one is the maximum
     * amount of memory used during the execution of the Schedule and the second
     * one is the amount of memory still used at the end of the execution of
     * the schedule */
    std::pair< double, double > memoryUsage(const Schedule< ALLOC >&) final;

    /// @}


    private:
    /// a scheduler used for computing the number of operations and memory usage
    SchedulerSequential< ALLOC > _sequential_scheduler_;

    /// the max cumulative number of instructions allowed for a given thread
    double _max_instruction_number_ {2000};

    /// friendship for allowing std::sort to use _cmp_
    template < typename ITER, typename CMP >
    friend void std::sort(ITER, ITER, CMP);
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/tools/graphicalModels/inference/scheduler/schedulerParallel_tpl.h>

#endif /* GUM_SCHEDULER_PARALLEL_H */
