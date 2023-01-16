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
 * @brief the base class for "low-level" operators used to schedule inferences
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#ifndef GUM_SCHEDULE_OPERATOR_H
#define GUM_SCHEDULE_OPERATOR_H

#include <string>
#include <utility>
#include <memory>

#include <agrum/agrum.h>

#include <agrum/tools/graphicalModels/inference/scheduler/IScheduleMultiDim.h>

namespace gum {


  /// the currently supported types of "low-level" operators
  /** @enum ScheduleOperatorType
   * @brief the types of "low-level" operators used for scheduling inferences
   * @headerfile scheduleOperator.h <agrum/tools/graphicalModels/inference/scheduler/scheduleOperator.h>
   * @ingroup inference_schedule
   */
  enum class ScheduleOperatorType : char {
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
   * @class ScheduleOperator
   * @brief the base class for "low-level" operators used to schedule inferences
   * @headerfile scheduleOperator.h <agrum/tools/graphicalModels/inference/scheduler/scheduleOperator.h>
   * @ingroup inference_schedule
   */
  class ScheduleOperator {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /**
     * @param type The type of operator to be scheduled (projection, etc)
     * @param imply_deletion A Boolean indicating whether the operator, when
     * performed, will delete some ScheduleMultiDims passed as its arguments
     * @param are_results_persistent if the operator produces some
     * ScheduleMultiDims, this boolean indicates whether these results are
     * persistent, i.e., whether they should be kept in memory when the operator
     * itself is deleted from memory.
     */
    explicit ScheduleOperator(const ScheduleOperatorType type,
                              const bool                 imply_deletion,
                              const bool                 are_results_persistent);

    /// virtual copy constructor
    virtual ScheduleOperator* clone() const = 0;

    /// destructor
    /** @warning If the ScheduleOperator has created an output ScheduleMultiDim,
     * upon destruction, this one is removed from memory */
    virtual ~ScheduleOperator();

    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// operator ==
    /** Two operators are identical if and only if they have equal
     * arguments and they perform the same operations. By Equal arguments,
     * we mean that these ScheduleMultiDims have the same IDs */
    virtual bool operator==(const ScheduleOperator&) const;

    /// operator !=
    /** Two operators are identical if and only if they have the same
     * arguments and they perform the same operations. By Equal arguments,
     * we mean that these ScheduleMultiDims have the same IDs */
    virtual bool operator!=(const ScheduleOperator&) const;

    /// @}


    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /** @brief checks whether two ScheduleOperator have similar parameters
     * (same variables and same content)
     *
     * Parameters having the same variables and the same content are essentially
     * identical but they may have different Ids (so that they may not be ==).
     */
    virtual bool hasSameArguments(const ScheduleOperator&) const = 0;

    /** @brief checks whether two ScheduleOperator have similar parameters
     * (same variables) */
    virtual bool hasSimilarArguments(const ScheduleOperator&) const = 0;

    /// checks whether two ScheduleOperator perform the same operator
    virtual bool isSameOperator(const ScheduleOperator&) const;

    /// returns the type of the operator
    ScheduleOperatorType type() const;

    /// indicates whether the operator deletes some of its arguments
    bool implyDeletion() const;

    /// really executes the operator
    virtual void execute() = 0;

    /// indicates whether the operator has been executed
    virtual bool isExecuted() const = 0;

    /// undo a previous execution, if any
    /** throws OperationNotAllowed is raised if the operator is unable to
     * perform undo */
    virtual void undo() = 0;

    /// modifies the arguments of the operator
    /** @throws SizeError is raised if the number of elements in new_args
     * does not correspond to the number of arguments expected by the
     * ScheduleOperator.
     * @throws TypeError is raised if at least one element of new_args does
     * not have a type compatible with what the ScheduleOperator expects.
     */
    virtual void updateArgs(const Sequence< const IScheduleMultiDim* >& new_args) = 0;

    /// returns the sequence of arguments passed to the operator
    virtual const Sequence< const IScheduleMultiDim* >& args() const = 0;

    /// returns the sequence of ScheduleMultidim output by the operator
    /** @warning Note that the Operator always returns its outputs, even if
     * it has not been executed. In this case, the outputs are abstract
     * ScheduleMultiDim.
     * @return the sequence of ScheduleMultiDim resulting from the operator.
     * Those can be abstract if the operator has not been performed yet.
     */
    virtual const Sequence< const IScheduleMultiDim* >& results() const = 0;

    /// makes the results of the operator persistent or not
    /** Unlike non-persistent results, a persistent one is not destroyed when the
     * operator itself is destroyed
     */
    void makeResultsPersistent(const bool is_persistent);

    /// shows whether the operator has persistent results
    bool hasPersistentResults() const;

    /** @brief returns an estimation of the number of elementary operations
     * needed to perform the ScheduleOperator */
    virtual double nbOperations() const = 0;

    /// returns the memory consumption used by the operator
    /** Actually, this function does not return a precise account of the memory
     * used by the ScheduleOperator but a rough estimate based on the sizes
     * of the tables involved in the execution of the operator.
     * @return a pair of memory consumption: the first one is the maximum
     * amount of memory used during the execution of the operator and the second
     * one is the amount of memory still used at the end of the function
     * (the memory used by the resulting table) */
    virtual std::pair< double, double > memoryUsage() const = 0;

    /// displays the content of the operator
    virtual std::string toString() const = 0;

    /// @}

    protected:
    /// copy constructor
    ScheduleOperator(const ScheduleOperator& from);

    /// move constructor
    ScheduleOperator(ScheduleOperator&& from);

    /// copy operator
    /** @warning if the operator contained some output ScheduleMultiDims,
     * then those are removed from memory before assigning the content of from */
    ScheduleOperator& operator=(const ScheduleOperator& from);

    /// move operator
    /** @warning if the operator contained some output ScheduleMultiDims,
     * then those are removed from memory before assigning the content of from */
    ScheduleOperator& operator=(ScheduleOperator&& from);


    private:
    /// the name of the operator to perform
    ScheduleOperatorType _op_type_;

    /** @brief indicates whether the operator will delete some of the
     * ScheduleMultiDim passed as its arguments */
    bool _imply_deletion_;

    /// is the result persistent
    bool _result_persistent_;
  };

} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/tools/graphicalModels/inference/scheduler/scheduleOperator_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_SCHEDULE_OPERATOR_H */
