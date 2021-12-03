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
 * @brief the common interface of all the schedulers
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef GUM_SCHEDULER_H
#define GUM_SCHEDULER_H

#include <utility>

#include <agrum/agrum.h>

#include <agrum/tools/core/threads.h>
#include <agrum/tools/graphicalModels/inference/scheduler/schedule.h>

namespace gum {

  template < template < typename > class ALLOC = std::allocator  >
  class Scheduler : private ALLOC< Idx > {
    public:
    using allocator_type = ALLOC< Idx >;

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /**
     * By default, the number of threads used by the scheduler to execute
     * schedules is aGrUM's max number of threads (see file thread.h). The
     * default max memory available is infinite.
     * @param max_megabyte_memory the maximum number of megabytes to use for
     * executing the operations of a schedule. If non strictly positive, this
     * means an infinite memory available (the default).
     * @param max_nb_threads the number max of threads to use to execute in
     * parallel the operations of a schedule. 0 = use aGrUM's default.
     * @param alloc the allocator used to allocate all the structures
     */
    Scheduler(Size   max_nb_threads = 0,
              double max_megabyte_memory = 0.0,
              const  allocator_type& alloc = allocator_type());

    /// virtual copy constructor
    virtual Scheduler< ALLOC >* clone() const = 0;

    /// virtual copy constructor with a given allocator
    virtual Scheduler< ALLOC >* clone(const allocator_type& alloc) const = 0;

    /// destructor
    virtual ~Scheduler();

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
    virtual void setMaxNbThreads(Size nb);

    /// returns the current max number of threads of the scheduler
    virtual Size maxNbThreads() const;

    /// sets an upper bound on the memory consumption admissible
    /**
     * @param megabytes the number of megabytes that can be used for the
     * execution of the schedule. If this number is equal to 0, then no
     * memory limit is applied.
     */
    virtual void setMaxMemory(double megabytes);

    /// returns the Max memory (in megabytes) available for executing schedules
    /**
     * @return the Max memory (in megabytes) available for executing schedules.
     * 0 means an unlimited amount of memory
     */
    virtual double maxMemory() const;

    /// execute all the operations of a given schedule
    virtual void execute(Schedule< ALLOC >&) = 0;

    /** @brief returns an estimation of the number of elementary operations
     * needed to perform a given schedule */
    virtual double nbOperations(const Schedule< ALLOC >&) = 0;

    /// returns the memory consumption used during the execution of a schedule
    /** Actually, this function does not return a precise account of the memory
     * used to perform the schedule but a rough estimate based on the sizes
     * of the tables involved in the schedule.
     * @return a pair of memory consumption: the first one is the maximum
     * amount of memory used during the execution of the Schedule and the second
     * one is the amount of memory still used at the end of the execution of
     * the schedule */
    virtual std::pair< double, double > memoryUsage(const Schedule< ALLOC >&) = 0;

    /// returns the allocator used by the scheduler
    allocator_type get_allocator() const;

    /// @}

    
    protected:
    /// the max number of threads
    Size _max_nb_threads{gum::getMaxNumberOfThreads()};

    /// the max memory usage possible (in bytes)
    double _max_memory {0.0};


    /// copy constructor
    Scheduler(const Scheduler< ALLOC >& from);

    /// copy constructor with a given allocator
    Scheduler(const Scheduler< ALLOC >& from,
              const allocator_type&     alloc);

    /// move constructor
    Scheduler(Scheduler< ALLOC >&& from);

    /// move constructor with a given allocator
    Scheduler(Scheduler< ALLOC >&&  from,
              const allocator_type& alloc);

    /// copy operator
    Scheduler< ALLOC >& operator=(const Scheduler< ALLOC >& from);

    /// move operator
    Scheduler< ALLOC >& operator=(Scheduler< ALLOC >&& from);

  };

} /* namespace gum */

// always include the template implementation
#include <agrum/tools/graphicalModels/inference/scheduler/scheduler_tpl.h>

#endif /* GUM_SCHEDULER_H */
