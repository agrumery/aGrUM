/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief the common interface of all the schedulers
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef GUM_SCHEDULER_H
#define GUM_SCHEDULER_H

#include <utility>

#include <agrum/agrum.h>

#include <agrum/base/core/threads/threadNumberManager.h>
#include <agrum/base/graphicalModels/inference/scheduler/schedule.h>

namespace gum {

  /// the type of scheduler that can be used
  enum SchedulerType {
    SEQUENTIAL,          // fully sequential scheduler
    PARALLEL_OPERATORS   // combinations and projections are dispatched among threads
  };

  /**
   * @class Scheduler
   * @brief The common interface of all the schedulers
   * @headerfile scheduler.h <agrum/base/graphicalModels/inference/scheduler/scheduler.h>
   */
  class Scheduler: public ThreadNumberManager {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /**
     * By default, the number of threads used by the scheduler to execute
     * schedules is aGrUM's number of threads (see file thread.h). The
     * default max memory available is infinite.
     * @param max_megabyte_memory the maximum number of megabytes to use for
     * executing the operations of a schedule. If non strictly positive, this
     * means an infinite memory available (the default).
     * @param nb_threads the number of threads to use to execute in
     * parallel the operations of a schedule. 0 = use aGrUM's default.
     */
    Scheduler(Size nb_threads = 0, double max_megabyte_memory = 0.0);

    /// virtual copy constructor
    virtual Scheduler* clone() const = 0;

    /// destructor
    virtual ~Scheduler();

    /// @}


    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

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
    virtual void execute(Schedule&) = 0;

    /** @brief returns an estimation of the number of elementary operations
     * needed to perform a given schedule */
    virtual double nbOperations(const Schedule&) = 0;

    /// returns the memory consumption used during the execution of a schedule
    /** Actually, this function does not return a precise account of the memory
     * used to perform the schedule but a rough estimate based on the sizes
     * of the tables involved in the schedule.
     * @return a pair of memory consumption: the first one is the maximum
     * amount of memory used during the execution of the Schedule and the second
     * one is the amount of memory still used at the end of the execution of
     * the schedule */
    virtual std::pair< double, double > memoryUsage(const Schedule&) = 0;

    /// @}


    protected:
    /// the max memory usage possible (in bytes)
    double _max_memory{0.0};


    /// copy constructor
    Scheduler(const Scheduler& from);

    /// move constructor
    Scheduler(Scheduler&& from);

    /// copy operator
    Scheduler& operator=(const Scheduler& from);

    /// move operator
    Scheduler& operator=(Scheduler&& from);
  };

} /* namespace gum */

// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/base/graphicalModels/inference/scheduler/scheduler_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_SCHEDULER_H */
