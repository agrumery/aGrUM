/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief the common interface of all the schedulers
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_SCHEDULER_H
#define GUM_SCHEDULER_H

#include <utility>
#include <agrum/graphicalModels/inference/schedule.h>

namespace gum {

  template <typename GUM_SCALAR> class Scheduler {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    Scheduler();

    /// copy constructor
    Scheduler(const Scheduler<GUM_SCALAR> &);

    /// destructor
    virtual ~Scheduler();

    /// virtual constructor
    /** @return a new fresh Scheduler */
    virtual Scheduler<GUM_SCALAR> *newFactory() const = 0;

    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// execute all the operations of a given schedule
    /** @return a Boolean indicating whether all the schedule was performed */
    virtual bool execute(Schedule<GUM_SCALAR> &) = 0;

    /// execute only k operations of a given schedule (default k = 1)
    /** If there are fewer than k operations in the schedule, then all those
     * operations are performed
     * @return a Boolean indicating whether the k operations (or all the
     * operations of the schedule) were performed */
    virtual bool execute(Schedule<GUM_SCALAR> &, unsigned int k) = 0;

    /** @brief returns an estimation of the number of elementary operations needed
     * to perform a given schedule */
    virtual float nbOperations(const Schedule<GUM_SCALAR> &) const = 0;

    /** @brief returns an estimation of the number of elementary operations needed
     * to perform the k first ScheduleOperations of a given schedule
     *
     * If there are fewer than k operations in the schedule, then all those
     * operations are taken into account */
    virtual float nbOperations(const Schedule<GUM_SCALAR> &,
                               unsigned int k) const = 0;

    /// returns the memory consumption used during the execution of a schedule
    /** Actually, this function does not return a precise account of the memory
     * used to perform the schedule but a rough estimate based on the sizes
     * of the tables involved in the schedule.
     * @return a pair of memory consumption: the first one is the maximum
     * amount of memory used during the execution of the Schedule and the second
     * one is the amount of memory still used at the end of the execution of
     * the schedule */
    virtual std::pair<long, long>
    memoryUsage(const Schedule<GUM_SCALAR> &) const = 0;

    /** @brief returns the memory consumption used during the execution of the
     * k first ScheduleOperations of a given schedule
     *
     * Actually, this function does not return a precise account of the memory
     * used to perform the schedule but a rough estimate based on the sizes
     * of the tables involved in the schedule.
     * If there are fewer than k operations in the schedule, then all those
     * operations are taken into account.
     * @return a pair of memory consumption: the first one is the maximum
     * amount of memory used during the execution of the Schedule and the second
     * one is the amount of memory still used at the end of the execution of
     * k first operations of the schedule */
    virtual std::pair<long, long> memoryUsage(const Schedule<GUM_SCALAR> &,
                                              unsigned int k) const = 0;

    /// @}
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/graphicalModels/inference/scheduler.tcc>

#endif /* GUM_SCHEDULER_H */
