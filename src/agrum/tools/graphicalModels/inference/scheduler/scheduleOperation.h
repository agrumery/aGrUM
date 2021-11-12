/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) et Christophe GONZALES(@AMU)
 *  info_at_agrum_dot_org
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
 * @brief the base class for "low-level" operations used to schedule inferences
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#ifndef GUM_SCHEDULE_OPERATION_H
#define GUM_SCHEDULE_OPERATION_H

#include <string>
#include <utility>
#include <memory>

#include <agrum/agrum.h>

#include <agrum/tools/core/sequence.h>
#include <agrum/tools/graphicalModels/inference/scheduler/IScheduleMultiDim.h>

namespace gum {


  /// the currently supported types of "low-level" operations
  /** @enum ScheduleOperationType
   * @brief the types of "low-level" operations used for scheduling inferences
   * @headerfile scheduleOperation.h <agrum/tools/graphicalModels/inference/scheduler/scheduleOperation.h>
   * @ingroup inference_schedule
   */
  enum class ScheduleOperationType : char {
    /// combine 2 ScheduleMultiDims
    COMBINE_MULTIDIM,
    /// project a ScheduleMultiDim over a subset of its variables
    PROJECT_MULTIDIM,
    /// remove from memory a multidimensional table stored in a ScheduleMultiDim
    DELETE_MULTIDIM,
    /// store a multidimensional table outside of the scheduler
    STORE_MULTIDIM
  };


  /**
   * @class ScheduleOperation
   * @brief the base class for "low-level" operations used to schedule inferences
   * @headerfile scheduleOperation.h <agrum/tools/graphicalModels/inference/scheduler/scheduleOperation.h>
   * @ingroup inference_schedule
   */
  template < template < typename > class ALLOC = std::allocator >
  class ScheduleOperation: private ALLOC< Idx > {
    public:
    using allocator_type = ALLOC< Idx >;

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /**
     *
     * @param type The type of operation to be scheduled (projection, etc)
     * @param imply_deletion A Boolean indicating whether the operation, when
     * performed, will delete some ScheduleMultiDims passed as its arguments
     * @param alloc the allocator used to allocate the operation and its
     * ScheduleMultiDims
     */
    explicit ScheduleOperation(const ScheduleOperationType type,
                               const bool                  imply_deletion,
                               const allocator_type&       alloc);

    /// virtual copy constructor
    virtual ScheduleOperation< ALLOC >* clone() const = 0;

    /// virtual copy constructor with a given allocator
    virtual ScheduleOperation< ALLOC >*
       clone(const allocator_type& alloc) const = 0;

    /// destructor
    /** @warning If the ScheduleOperation has created an output ScheduleMultiDim,
     * upon destruction, this one is removed from memory */
    virtual ~ScheduleOperation();

    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// operator ==
    /** Two operations are identical if and only if they have equal
     * arguments and they perform the same operation. By Equal arguments,
     * we mean that these ScheduleMultiDims have the same IDs */
    virtual bool operator==(const ScheduleOperation< ALLOC >&) const;

    /// operator !=
    /** Two operations are identical if and only if they have the same
     * arguments and they perform the same operation. By Equal arguments,
     * we mean that these ScheduleMultiDims have the same IDs */
    virtual bool operator!=(const ScheduleOperation< ALLOC >&) const;

    /// @}


    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /** @brief checks whether two ScheduleOperation have the similar parameters
     * (same variables and same content)
     *
     * Parameters having the same variables and the same content are essentially
     * identical but they may have different Ids (so that they may not be ==).
     */
    virtual bool hasSameArguments(const ScheduleOperation< ALLOC >&) const = 0;

    /** @brief checks whether two ScheduleCombination have similar parameters
     * (same variables) */
    virtual bool hasSimilarArguments(const ScheduleOperation< ALLOC >&) const = 0;

    /// checks whether two ScheduleOperation perform the same operation
    virtual bool isSameOperation(const ScheduleOperation< ALLOC >&) const;

    /// returns the type of the operation
    ScheduleOperationType type() const;

    /// indicates wether the operation deletes some of its arguments
    bool implyDeletion() const;

    /// really executes the operation
    virtual void execute() = 0;

    /// indicates whether the operation has been executed
    virtual bool isExecuted() const = 0;

    /// undo a previous execution, if any
    /** throws OperationNotAllowed is raised if the operation is unable to
     * perform undo */
    virtual void undo() = 0;

    /// modifies the arguments of the operation
    /** @throws SizeError is raised if the number of elements in new_args
     * does not correspond to the number of arguments expected by the
     * ScheduleOperation.
     * @throws TypeError is raised if at least one element of new_args does
     * not have a type compatible with what the ScheduleOperation expects.
     */
    virtual void updateArgs(
       const Sequence< const IScheduleMultiDim< ALLOC >* >& new_args) = 0;

    /// returns the sequence of arguments passed to the operation
    virtual const Sequence< const IScheduleMultiDim< ALLOC >* >& args() const = 0;

    /// returns the sequence of ScheduleMultidim output by the operation
    /** @warning Note that the Operation always returns its outputs, even if
     * it has not been executed. In this case, the outputs are abstract
     * ScheduleMultiDim.
     * @return the sequence of ScheduleMultiDim resulting from the operation.
     * Those can be abstract if the operation has not been performed yet.
     */
    virtual const Sequence< const IScheduleMultiDim< ALLOC >* >&
       results() const = 0;

    /** @brief returns an estimation of the number of elementary operations
     * needed to perform the ScheduleOperation */
    virtual double nbOperations() const = 0;

    /// returns the memory consumption used during the operation
    /** Actually, this function does not return a precise account of the memory
     * used by the ScheduleOperation but a rough estimate based on the sizes
     * of the tables involved in the operation.
     * @return a pair of memory consumption: the first one is the maximum
     * amount of memory used during the operation and the second one is the
     * amount of memory still used at the end of the function ( the memory used
     * by the resulting table ) */
    virtual std::pair< double, double > memoryUsage() const = 0;

    /// displays the content of the operation
    virtual std::string toString() const = 0;

    /// returns the allocator used by the operation
    allocator_type get_allocator() const;

    /// @}

    protected:
    /// copy constructor
    ScheduleOperation(const ScheduleOperation< ALLOC >& from);

    /// copy constructor with a given allocator
    ScheduleOperation(const ScheduleOperation< ALLOC >& from,
                      const allocator_type&             alloc);

    /// move constructor
    ScheduleOperation(ScheduleOperation< ALLOC >&& from);

    /// move constructor with a given allocator
    ScheduleOperation(ScheduleOperation< ALLOC >&& from,
                      const allocator_type&        alloc);

    /// copy operator
    /** @warning if the operation contained some output ScheduleMultiDims,
     * then those are removed from memory before assigning the content of from */
    ScheduleOperation< ALLOC >& operator=(const ScheduleOperation< ALLOC >& from);

    /// move operator
    /** @warning if the operation contained some output ScheduleMultiDims,
     * then those are removed from memory before assigning the content of from */
    ScheduleOperation< ALLOC >&
       operator=(ScheduleOperation< ALLOC >&& from);


    private:
    /// the name of the operation to perform
    ScheduleOperationType _op_type_;

    /** @brief indicates whether the operation will delete some of the
     * ScheduleMultiDim passed as its arguments */
    bool _imply_deletion_;

  };

} /* namespace gum */

// always include the template implementation
#include <agrum/tools/graphicalModels/inference/scheduler/scheduleOperation_tpl.h>

#endif /* GUM_SCHEDULE_OPERATION_H */
