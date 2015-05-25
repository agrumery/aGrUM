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
 * @brief A generic class to project efficiently a ScheduleMultiDim over a subset
 * of its variables
 *
 * ScheduleProjection is a generic wrapper designed to project efficiently a
 * multidimensional schedule object over a subset of its variables.
 *
 * To be quite generic, the ScheduleProjection takes in argument the function
 * that produces the result of the projection of the multidimensional object.
 * The following code gives an example of the usage of ScheduleProjection:
 * @code
 * // a function used to project a MultiDimImplementation<float>:
 * MultiDimImplementation<float>*
 * MinPot ( const MultiDimImplementation<float>& table,
 *          const Set<const DiscreteVariable*>& del_vars ) {
 *   return new MultiDimImplementation<float> (...);
 * }
 *
 * // another function used to project a MultiDimImplementation<float>:
 * MultiDimImplementation<float>*
 * MaxPot ( const Potential<float>& table,
 *          const Set<const DiscreteVariable*>& del_vars ) {
 *   return new Potential<float> (...);
 * }
 *
 *
 * Schedule<float> schedule;
 * ScheduleMultiDim<float> t1, t2;
 * Set<const DiscreteVariable*> set1, set2;
 * ScheduleProjectionBasic<float> Proj ( MinPot );
 * ScheduleMultiDim<float> proj_table1 = Proj.project ( t1, set1, schedule );
 *
 * // change the operator to apply
 * Proj.setProjectFunction ( MaxPot );
 * ScheduleMultiDim<float> proj_table2 = Proj.project ( t2, set2, schedule );
 *
 * @endcode
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_SCHEDULE_PROJECTION_H
#define GUM_SCHEDULE_PROJECTION_H

#include <utility>
#include <agrum/core/set.h>
#include <agrum/variables/discreteVariable.h>
#include <agrum/graphicalModels/inference/scheduleMultiDim.h>
#include <agrum/graphicalModels/inference/schedule.h>

namespace gum {

  template <typename GUM_SCALAR> class ScheduleProjection {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    ScheduleProjection();

    /// copy constructor
    ScheduleProjection(const ScheduleProjection<GUM_SCALAR> &);

    /// destructor
    virtual ~ScheduleProjection();

    /// virtual constructor
    /** @return a new fresh ScheduleCombinator with the same projection
     * function. */
    virtual ScheduleProjection<GUM_SCALAR> *newFactory() const = 0;

    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// creates and returns the projection of the table over a subset of its vars
    /** @return a new freshly created ScheduleMultiDim which is the result of the
     * projection of the table passed in argument over the set of variables
     * NOT IN del_vars
     * @warning If del_vars is precisely equal to the variables of table, the
     * result is an empty table. */
    virtual ScheduleMultiDim<GUM_SCALAR>
    project(const ScheduleMultiDim<GUM_SCALAR> &table,
            const Set<const DiscreteVariable *> &del_vars,
            Schedule<GUM_SCALAR> &schedule) = 0;
    ScheduleMultiDim<GUM_SCALAR>
    project(const MultiDimImplementation<GUM_SCALAR> &table,
            const Set<const DiscreteVariable *> &del_vars,
            Schedule<GUM_SCALAR> &schedule);
    template <template <typename> class TABLE>
    ScheduleMultiDim<GUM_SCALAR>
    project(const TABLE<GUM_SCALAR> &table,
            const Set<const DiscreteVariable *> &del_vars,
            Schedule<GUM_SCALAR> &schedule);

    /// changes the function used for projecting tables
    virtual void setProjectFunction(MultiDimImplementation<GUM_SCALAR> *(*proj)(
        const MultiDimImplementation<GUM_SCALAR> &,
        const Set<const DiscreteVariable *> &)) = 0;

    /// returns the projection function currently used by the projector
    virtual MultiDimImplementation<GUM_SCALAR> *(*projectFunction())(
        const MultiDimImplementation<GUM_SCALAR> &,
        const Set<const DiscreteVariable *> &) = 0;

    /** @brief returns a rough estimate of the number of operations that will be
     * performed to compute the projection */
    virtual float nbOperations(const ScheduleMultiDim<GUM_SCALAR> &table,
                               const Set<const DiscreteVariable *> &del_vars,
                               const Schedule<GUM_SCALAR> &schedule) = 0;
    float nbOperations(const MultiDimImplementation<GUM_SCALAR> &table,
                       const Set<const DiscreteVariable *> &del_vars,
                       const Schedule<GUM_SCALAR> &schedule);
    template <template <typename> class TABLE>
    float nbOperations(const TABLE<GUM_SCALAR> &set,
                       const Set<const DiscreteVariable *> &del_vars,
                       const Schedule<GUM_SCALAR> &schedule);

    /// returns the memory consumption used during the projection
    /** Actually, this function does not return a precise account of the memory
     * used by the multidimProjection but a rough estimate based on the size
     * of the table involved in the projection.
     * @return a pair of memory consumption: the first one is the maximum
     * amount of memory used during the combination and the second one is the
     * amount of memory still used at the end of the function ( the memory used by
     * the resulting table ) */
    virtual std::pair<long, long>
    memoryUsage(const ScheduleMultiDim<GUM_SCALAR> &table,
                const Set<const DiscreteVariable *> &del_vars,
                const Schedule<GUM_SCALAR> &schedule) = 0;
    std::pair<long, long>
    memoryUsage(const MultiDimImplementation<GUM_SCALAR> &table,
                const Set<const DiscreteVariable *> &del_vars,
                const Schedule<GUM_SCALAR> &schedule);
    template <template <typename> class TABLE>
    std::pair<long, long> memoryUsage(const TABLE<GUM_SCALAR> &table,
                                      const Set<const DiscreteVariable *> &del_vars,
                                      const Schedule<GUM_SCALAR> &schedule);

    /// @}

    private:
    /// to be coherent with combinations, forbid copy operators
    ScheduleProjection<GUM_SCALAR> &
    operator=(const ScheduleProjection<GUM_SCALAR> &);
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/graphicalModels/inference/scheduleProjection.tcc>

#endif /* GUM_SCHEDULE_PROJECTION_H */
