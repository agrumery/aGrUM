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
 * @brief the base operation class used to schedule inferences
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_SCHEDULE_OPERATION_H
#define GUM_SCHEDULE_OPERATION_H

#include <utility>
#include <string>

#include <agrum/config.h>

#include <agrum/core/sequence.h>
#include <agrum/graphicalModels/inference/scheduleMultiDim.h>

namespace gum {

  template <typename GUM_SCALAR> class ScheduleOperation {
    public:
    /// the currently supported types of operations
    enum class Type : char {
      /// combine 2 ScheduleMultiDims
      COMBINE_MULTIDIM,
      /// project a ScheduleMultiDim over a subset of its variables
      PROJECT_MULTIDIM,
      /// delete a MultiDim from memory
      DELETE_MULTIDIM,
      /// store a Multidim into a clique
      CLIQUE_STORE_MULTIDIM,
      /// store a multidim into a separator
      SEPARATOR_STORE_MULTIDIM
    };

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// virtual copy constructor: creates a clone of the operation
    virtual ScheduleOperation<GUM_SCALAR> *newFactory() const = 0;

    /// destructor
    virtual ~ScheduleOperation();

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// operator ==
    /** Two operations are identical if and only if they have the same
     * arguments and their types are identical (combine, project, etc) */
    virtual bool operator==(const ScheduleOperation<GUM_SCALAR> &) const = 0;

    /// operator !=
    /** Two operations are identical if and only if they have the same
     * arguments and their types are identical (combine, project, etc) */
    virtual bool operator!=(const ScheduleOperation<GUM_SCALAR> &) const = 0;

    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// returns the name of the operation
    Type type() const;

    /// returns the id of the operation
    Id id() const;

    /// returns the set of multidims passed in argument to the operation
    virtual const Sequence<const ScheduleMultiDim<GUM_SCALAR> *> &
    multiDimArgs() const = 0;

    /// returns the set of multidims that should be the result of the operation
    virtual const Sequence<const ScheduleMultiDim<GUM_SCALAR> *> &
    multiDimResults() const = 0;

    /// executes the operation
    virtual void execute() = 0;

    /** @brief returns an estimation of the number of elementary operations
     * needed to perform the ScheduleOperation */
    virtual float nbOperations() const = 0;

    /// returns the memory consumption used during the operation
    /** Actually, this function does not return a precise account of the memory
     * used by the ScheduleOperation but a rough estimate based on the sizes
     * of the tables involved in the operation.
     * @return a pair of memory consumption: the first one is the maximum
     * amount of memory used during the operation and the second one is the
     * amount of memory still used at the end of the function ( the memory used by
     * the resulting table ) */
    virtual std::pair<long, long> memoryUsage() const = 0;

    /// displays the content of the operation
    virtual std::string toString() const = 0;

    /// @}

    protected:
    /// default constructor
    ScheduleOperation(Type t);

    /// copy constructor
    ScheduleOperation(const ScheduleOperation<GUM_SCALAR> &from);

    /// copy operator
    ScheduleOperation<GUM_SCALAR> &operator=(const ScheduleOperation<GUM_SCALAR> &);

    private:
    /// the name of the operation to perform
    Type __type;

    /// the id corresponding to the operation
    Id __id;

    /// returns a new distinct ID for each operation
    static Id __newId();
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/graphicalModels/inference/scheduleOperation.tcc>

#endif /* GUM_SCHEDULE_OPERATION_H */
