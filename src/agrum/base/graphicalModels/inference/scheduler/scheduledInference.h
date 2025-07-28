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
 * @brief The class enabling flexible inferences using schedules
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef GUM_SCHEDULED_INFERENCE_H
#define GUM_SCHEDULED_INFERENCE_H

#include <agrum/agrum.h>

#include <agrum/base/graphicalModels/inference/scheduler/schedulerParallel.h>

namespace gum {

  /**
   * @class ScheduledInference
   * @brief the class containing the scheduler used by schedule-based inferences
   * @headerfile scheduledInference.h <agrum/base/graphicalModels/inference/scheduler/scheduledInference.h>
   * @ingroup gm_group
   */
  class ScheduledInference: public ThreadNumberManager {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    ScheduledInference(Size max_nb_threads = 0, double max_megabyte_memory = 0.0);

    /// copy constructor
    ScheduledInference(const ScheduledInference& from);

    /// move constructor
    ScheduledInference(ScheduledInference&& from);

    /// virtual copy constructor
    virtual ScheduledInference* clone() const;

    /// destructor
    virtual ~ScheduledInference();

    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    ScheduledInference& operator=(const ScheduledInference& from);

    /// move operator
    ScheduledInference& operator=(ScheduledInference&& from);

    /// @}


    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// returns the current scheduler
    Scheduler& scheduler() const;

    /// sets the number max of threads that can be used
    /**
     * @param nb the number max of threads used for executing schedules. If this
     * number is set to 0, then it is defaulted to aGrUM's number of threads
     */
    void setNumberOfThreads(Size nb) final;

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
    // the scheduler to be used when several threads are used
    SchedulerParallel _scheduler_parallel_;

    // a sequential scheduler: to be used if only one thread is used
    SchedulerSequential _scheduler_sequential_;
  };

}   // namespace gum

#ifndef GUM_NO_INLINE
#  include <agrum/base/graphicalModels/inference/scheduler/scheduledInference_inl.h>
#endif   // GUM_NO_INLINE

#endif   // GUM_SCHEDULED_INFERENCE_H
