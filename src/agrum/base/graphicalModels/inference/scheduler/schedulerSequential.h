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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief a scheduler that executes any available operation (chosen arbitrarily)
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef GUM_SCHEDULER_SEQUENTIAL_H
#define GUM_SCHEDULER_SEQUENTIAL_H

#include <algorithm>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/graphicalModels/inference/scheduler/scheduler.h>

namespace gum {

  class SchedulerSequential: public Scheduler {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    explicit SchedulerSequential(Size max_nb_threads = 0, double max_megabyte_memory = 0.0);

    /// copy constructor
    SchedulerSequential(const SchedulerSequential& from);

    /// move constructor
    SchedulerSequential(SchedulerSequential&& from);

    /// virtual copy constructor
    SchedulerSequential* clone() const final;

    /// destructor
    virtual ~SchedulerSequential();

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
    /// the schedule we wish to execute
    Schedule* _schedule_{nullptr};

    /// the sequence of operations to perform
    std::vector< NodeId > _operations_;

    /// the memory usage for the sequence of operations;
    std::pair< double, double > _memory_usage_{0.0, 0.0};

    /// is the set of operations up to date
    bool _operations_up_to_date_{false};

    /** @brief a structure to keep informations about operations that could not be
     * executed due to memory usage limitations */
    struct UnexecutedOperation {
      double max_memory_usage;
      double end_memory_usage;
      NodeId node;
    };

    /// sets a schedule and updates the set of operations if necessary
    void _setSchedule_(const Schedule& schedule);

    /** @brief add the currently executable deletions into the set of
     * available operations
     */
    Size _addExecutableOps_(std::vector< UnexecutedOperation >& unexecuted_deletions,
                            std::vector< UnexecutedOperation >& unexecuted_operations,
                            bool&                               unexecuted_deletions_sorted,
                            bool&                               unexecuted_operations_sorted,
                            double                              memory_used,
                            double                              max_memory,
                            List< NodeId >&                     available_nodes) const;

    /** @brief simulate the update of the schedule's DAG resulting from the
     * execution of an operation
     */
    void _simulateDAGUpdate_(DAG&                   dag,
                             const NodeId           node,
                             std::vector< NodeId >& new_available_nodes) const;

    /// simulate the execution of one operation
    void _simulateExecuteOneOperation_(const NodeId           node,
                                       ScheduleOperator&      op,
                                       DAG&                   dag,
                                       List< NodeId >&        available_nodes,
                                       std::vector< NodeId >& new_available_nodes);

    /// simulate the execution of the whole schedule
    void _simulateExecution_();

    /// the comparison function used to sort unexecutable operations
    static bool _cmp_(const UnexecutedOperation& a, const UnexecutedOperation& b);

    /// friendship for allowing std::sort to use _cmp_
    // template<typename ITER, typename CMP>
    // friend void std::sort(ITER, ITER, CMP);
  };

} /* namespace gum */

// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/base/graphicalModels/inference/scheduler/schedulerSequential_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_SCHEDULER_SEQUENTIAL_H */
