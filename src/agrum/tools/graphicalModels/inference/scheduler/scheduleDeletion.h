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
 * @brief an operator to force a ScheduleMultiDim to be abstract
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef GUM_SCHEDULE_DELETION_H
#define GUM_SCHEDULE_DELETION_H

#include <string>

#include <agrum/agrum.h>

#include <agrum/tools/graphicalModels/inference/scheduler/scheduleMultiDim.h>
#include <agrum/tools/graphicalModels/inference/scheduler/scheduleOperator.h>

namespace gum {

  /**
   * @class ScheduleDeletion
   * @brief an operator to make a ScheduleMultiDim abstract and free its memory
   * @headerfile scheduleDeletion.h <agrum/tools/graphicalModels/inference/scheduler/scheduleDeletion.h>
   *
   * @ingroup inference_schedule
   * The following code gives an example of the usage of ScheduleDeletion:
   * @code
   * // create the multidim
   * Potential< float > p1 (....), p2(....);
   * ScheduleMultiDim< Potential< float > > t1(p1, true); // contains a copy of pot
   * ScheduleMultiDim< Potential< float > > t2(p2, false); // contains a ref on pot
   *
   * // create the ScheduleDeletions
   * ScheduleDeletion< Potential<float> > del1(t1), del2(t2);
   *
   * // applying del1 consists of releasing the memory of the potential it
   * // contains and, subsequently, of making the ScheduleMultiDim abstract
   * del1.execute();
   *
   * // applying del2 consists of making ScheduleMultiDim t2 abstract. As t2
   * // contained only a reference on p2, the latter remains unchanged.
   * @encode
   */
  template < typename TABLE >
  class ScheduleDeletion: public ScheduleOperator {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @param table the ScheduleMultiDim containing the table that will
     * be deleted.
     * @warning table is stored only by reference within the ScheduleDeletion.
     */
    explicit ScheduleDeletion(const ScheduleMultiDim< TABLE >& table);

    /// copy constructor
    ScheduleDeletion(const ScheduleDeletion< TABLE >& from);

    /// move constructor
    ScheduleDeletion(ScheduleDeletion< TABLE >&& from);

    /// virtual copy constructor
    ScheduleDeletion< TABLE >* clone() const final;

    /// destructor
    virtual ~ScheduleDeletion();

    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    ScheduleDeletion< TABLE >& operator=(const ScheduleDeletion< TABLE >&);

    /// move operator
    ScheduleDeletion< TABLE >& operator=(ScheduleDeletion< TABLE >&&);

    /// operator ==
    /** Two operators are identical if and only if they have equal (==)
     * ScheduleMultiDim arguments. By Equal arguments, we stress that we mean
     * that these ScheduleMultiDims have the same IDs*/
    bool operator==(const ScheduleOperator& m) const final;

    /// operator !=
    /** Two operators are identical if and only if they have equal (==)
     * ScheduleMultiDim arguments. By Equal arguments, we stress that we mean
     * that these ScheduleMultiDims have the same IDs*/
    bool operator!=(const ScheduleOperator& m) const final;

    /// operator ==
    /** Two operators are identical if and only if they have equal (==)
     * ScheduleMultiDim arguments. By Equal arguments, we stress that we mean
     * that these ScheduleMultiDims have the same IDs*/
    virtual bool operator==(const ScheduleDeletion< TABLE >&) const;

    /// operator !=
    /** Two operators are identical if and only if they have equal (==)
     * ScheduleMultiDim arguments. By Equal arguments, we stress that we mean
     * that these ScheduleMultiDims have the same IDs*/
    virtual bool operator!=(const ScheduleDeletion< TABLE >&) const;

    /// @}


    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /** @brief checks whether two ScheduleDeletion have the same parameters
     * (same variables and same content)
     *
     * ScheduleMultiDim Parameters having the same variables and the same content
     * are essentially identical but they may have different Ids (so that they
     * may not be ==).
     */
    bool hasSameArguments(const ScheduleOperator&) const final;

    /** @brief checks whether two ScheduleDeletion have the same parameters
     * (same variables and same content)
     *
     * ScheduleMultiDim Parameters having the same variables and the same content
     * are essentially identical but they may have different Ids (so that they
     * may not be ==).
     */
    bool hasSameArguments(const ScheduleDeletion< TABLE >&) const;

    /** @brief checks whether two ScheduleProjection have similar parameters
     * (same variables but not necessarily the same content)
     */
    bool hasSimilarArguments(const ScheduleOperator&) const final;

    /** @brief checks whether two ScheduleProjection have similar parameters
     * (same variables but not necessarily the same content)
     */
    bool hasSimilarArguments(const ScheduleDeletion< TABLE >&) const;

    /// checks whether two ScheduleOperator perform the same set of operations
    bool isSameOperator(const ScheduleOperator&) const final;

    /// checks whether two ScheduleOperator perform the same set of operations
    bool isSameOperator(const ScheduleDeletion< TABLE >&) const;

    /// returns the argument of the projection
    const ScheduleMultiDim< TABLE >& arg() const;

    /// returns the sequence of arguments passed to the operator
    const Sequence< const IScheduleMultiDim* >& args() const final;

    /// returns the sequence of ScheduleMultidim output by the operator
    const Sequence< const IScheduleMultiDim* >& results() const final;

    /// modifies the arguments of the operator
    /** @throws SizeError is raised if the number of elements in new_args
     * does not correspond to the number of arguments expected by the
     * ScheduleOperator.
     * @throws TypeError is raised if at least one element of new_args does
     * not have a type compatible with what the ScheduleOperator expects.
     */
    void updateArgs(const Sequence< const IScheduleMultiDim* >& new_args) final;

    /// indicates whether the operator has been executed
    bool isExecuted() const final;

    /// executes the operator
    void execute() final;

    /// undo a previous execution, if any
    /** @throws OperationNotAllowed is raised if we wish to undo a deletion because
     * no information is stored about the original table when this one
     * is deleted. */
    void undo() final;

    /** @brief returns an estimation of the number of elementary operations
     * needed to perform the ScheduleOperator */
    double nbOperations() const final;

    /// returns the memory consumption used during the execution of the operator
    /** Actually, this function does not return a precise account of the memory
     * used by the ScheduleOperator but a rough estimate based on the sizes
     * of the tables involved in the operator.
     * @return a pair of memory consumption: the first one is the maximum
     * amount of memory used during the execution of the operator and the second
     * one is the amount of memory still used at the end of the function (the
     * memory used by the resulting table)
     * @warning This method does not checks whether the ScheduleMultiDim to be
     * deleted is abstract or not: it assumes that it is not when the deletion
     * is performed. This enables to take into account the memory usage of a
     * whole Schedule tree before performing any operation within it. */
    std::pair< double, double > memoryUsage() const final;

    /// displays the content of the operator
    std::string toString() const final;

    /// @}

    private:
    /// the table to delete, i.e., to make abstract
    ScheduleMultiDim< TABLE >* _arg_{nullptr};

    /// the sequence of arguments passed to the operator
    /** This method is convenient when using ScheduleOperator rather than
     * directly using ScheduleBinaryCombination */
    Sequence< const IScheduleMultiDim* > _args_;

    /// the sequence of ScheduleMultidim output by the operator
    /** @warning Note that the Operator has always some output, even if
     * it has not been executed. In this case, the outputs are abstract
     * ScheduleMultiDim.
     */
    Sequence< const IScheduleMultiDim* > _results_;

    /// indicates whether the operator has been performed or not
    bool _is_executed_{false};
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/tools/graphicalModels/inference/scheduler/scheduleDeletion_tpl.h>

#endif /* GUM_SCHEDULE_DELETION_H */
