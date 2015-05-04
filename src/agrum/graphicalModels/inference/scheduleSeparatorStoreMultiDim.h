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
 * @brief an operator used by scheduled inferences to store tables into separators
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_SCHEDULE_SEPARATOR_STORE_MULTI_DIM_H
#define GUM_SCHEDULE_SEPARATOR_STORE_MULTI_DIM_H

#include <string>

#include <agrum/config.h>

#include <agrum/core/sequence.h>
#include <agrum/graphs/graphElements.h>
#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/graphicalModels/inference/scheduleMultiDim.h>
#include <agrum/graphicalModels/inference/scheduleOperation.h>

namespace gum {

  template <typename GUM_SCALAR>

  class ScheduleSeparatorStoreMultiDim : public ScheduleOperation<GUM_SCALAR> {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    ScheduleSeparatorStoreMultiDim(
        const ScheduleMultiDim<GUM_SCALAR> &table,
        ArcProperty<Set<const MultiDimImplementation<GUM_SCALAR> *>> &
            separator_tables,
        Arc separator);

    /// copy constructor
    ScheduleSeparatorStoreMultiDim(
        const ScheduleSeparatorStoreMultiDim<GUM_SCALAR> &);

    /// virtual copy constructor: creates a clone of the operation
    virtual ScheduleSeparatorStoreMultiDim<GUM_SCALAR> *newFactory() const;

    /// destructor
    virtual ~ScheduleSeparatorStoreMultiDim();

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    ScheduleSeparatorStoreMultiDim<GUM_SCALAR> &
    operator=(const ScheduleSeparatorStoreMultiDim<GUM_SCALAR> &);

    /// operator ==
    /** Two operations are identical if and only if they have the same
     * arguments and their types are identical (combine, project, etc) */
    bool operator==(const ScheduleOperation<GUM_SCALAR> &) const;

    /// operator !=
    /** Two operations are identical if and only if they have the same
     * arguments and their types are identical (combine, project, etc) */
    bool operator!=(const ScheduleOperation<GUM_SCALAR> &) const;

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
     * amount of memory still used at the end of the function ( the memory used by
     * the resulting table ) */
    std::pair<long, long> memoryUsage() const;

    /// returns the set of multidims passed in argument to the operation
    const Sequence<const ScheduleMultiDim<GUM_SCALAR> *> &multiDimArgs() const;

    /// returns the set of multidims that should be the result of the operation
    const Sequence<const ScheduleMultiDim<GUM_SCALAR> *> &multiDimResults() const;

    /// displays the content of the operation
    std::string toString() const;

    /// @}

    private:
    // the table to store into the clique
    ScheduleMultiDim<GUM_SCALAR> __table;

    // a mapping assigning to each clique a set of tables
    ArcProperty<Set<const MultiDimImplementation<GUM_SCALAR> *>> *__tableSet;

    // the separator into which the table will be stored
    Arc __separator;

    /// the set of ScheduleMultidims passed in arguments
    mutable Sequence<const ScheduleMultiDim<GUM_SCALAR> *> *__args;
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/graphicalModels/inference/scheduleSeparatorStoreMultiDim.tcc>

#endif /* GUM_SCHEDULE_SEPARATOR_STORE_MULTI_DIM_H */
