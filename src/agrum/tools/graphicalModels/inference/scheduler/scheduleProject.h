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
 * @brief a Projection operator class used for scheduling inferences
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef GUM_SCHEDULE_PROJECT_H
#define GUM_SCHEDULE_PROJECT_H

#include <string>

#include <agrum/agrum.h>

#include <agrum/tools/core/sequence.h>
#include <agrum/tools/core/set.h>
#include <agrum/tools/graphicalModels/inference/scheduler/scheduleMultiDim.h>
#include <agrum/tools/graphicalModels/inference/scheduler/scheduleOperation.h>
#include <agrum/tools/multidim/implementations/multiDimImplementation.h>
#include <agrum/tools/variables/discreteVariable.h>

namespace gum {

  template < typename GUM_SCALAR >

  class ScheduleProject: public ScheduleOperation< GUM_SCALAR > {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @warning table and del_vars are passed by copy */
    ScheduleProject(const ScheduleMultiDim< GUM_SCALAR >& table,
                    const Set< const DiscreteVariable* >& del_vars,
                    MultiDimImplementation< GUM_SCALAR >* (*project)(
                       const MultiDimImplementation< GUM_SCALAR >&,
                       const Set< const DiscreteVariable* >&));

    /// copy constructor
    ScheduleProject(const ScheduleProject< GUM_SCALAR >&);

    /// virtual copy constructor: creates a clone of the operation
    virtual ScheduleProject< GUM_SCALAR >* newFactory() const;

    /// destructor
    virtual ~ScheduleProject();

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    ScheduleProject< GUM_SCALAR >& operator=(const ScheduleProject< GUM_SCALAR >&);

    /// operator ==
    /** Two operations are identical if and only if they have the same
     * arguments and their types are identical (combine, project, etc) */
    bool operator==(const ScheduleOperation< GUM_SCALAR >&) const;

    /// operator !=
    /** Two operations are identical if and only if they have the same
     * arguments and their types are identical (combine, project, etc) */
    bool operator!=(const ScheduleOperation< GUM_SCALAR >&) const;

    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// executes the operation
    void execute();

    /** @brief returns an estimation of the number of elementary operations
     * needed to perform the ScheduleOperation */
    float nbOperations() const;

    /// returns the memory consumption used during the operation
    /** Actually, this function does not return a precise account of the memory
     * used by the ScheduleOperation but a rough estimate based on the sizes
     * of the tables involved in the operation.
     * @return a pair of memory consumption: the first one is the maximum
     * amount of memory used during the operation and the second one is the
     * amount of memory still used at the end of the function ( the memory used
     * by
     * the resulting table ) */
    std::pair< long, long > memoryUsage() const;

    /// returns the scheduleMultidim resulting from the execution of the
    /// operation
    const ScheduleMultiDim< GUM_SCALAR >& result() const;

    /// returns the set of multidims passed in argument to the operation
    const Sequence< const ScheduleMultiDim< GUM_SCALAR >* >& multiDimArgs() const;

    /// returns the set of multidims that should be the result of the operation
    const Sequence< const ScheduleMultiDim< GUM_SCALAR >* >&
       multiDimResults() const;

    /// displays the content of the operation
    std::string toString() const;

    /// @}

    private:
    // the table to project
    ScheduleMultiDim< GUM_SCALAR > table__;

    // the set of variables that should be removed from the table
    Set< const DiscreteVariable* > del_vars__;

    /// the result of the operation
    ScheduleMultiDim< GUM_SCALAR >* result__;

    /// the set of ScheduleMultidims passed in arguments
    mutable Sequence< const ScheduleMultiDim< GUM_SCALAR >* >* args__;

    /// the set of ScheduleMultidims resulting from the operation
    mutable Sequence< const ScheduleMultiDim< GUM_SCALAR >* >* results__;

    /// the projection operator
    MultiDimImplementation< GUM_SCALAR >* (*project__)(
       const MultiDimImplementation< GUM_SCALAR >&,
       const Set< const DiscreteVariable* >&);
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/tools/graphicalModels/inference/scheduler/scheduleProject_tpl.h>

#endif /* GUM_SCHEDULE_PROJECTION_H */