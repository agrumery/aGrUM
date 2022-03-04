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
 * @brief a binary Combination operator class used for scheduling inferences
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#ifndef GUM_SCHEDULE_BINARY_COMBINATION_H
#define GUM_SCHEDULE_BINARY_COMBINATION_H

#include <string>
#include <memory>

#include <agrum/agrum.h>

#include <agrum/tools/graphicalModels/inference/scheduler/scheduleMultiDim.h>
#include <agrum/tools/graphicalModels/inference/scheduler/scheduleOperator.h>

namespace gum {

  /**
   * @class ScheduleBinaryCombination
   * @brief a binary Combination operator class used for scheduling inferences
   * @headerfile scheduleBinaryCombination.h <agrum/graphicalModels/inference/scheduleBinaryCombination.h>
   * @ingroup inference_schedule
   *
   * To be quite generic, the ScheduleBinaryCombination takes in argument the
   * function that produces the result of the combination of the "real" tables
   * contained within the ScheduleMultiDims to be combined. Note that executing
   * a ScheduleBinaryCombination will always produce a freshly allocated table.
   * The following code gives an example of the usage of ScheduleBinaryCombination:
   * @code
   * // the combination function over "true" Potentials
   * Potential< double > myadd(const gum::Potential< double >& f1,
   *                           const gum::Potential< double >& f2) {
   *   return f1 + f2;
   * }
   *
   * // define the potentials to be combined and wrap them into ScheduleMultiDim
   * Potential< double > pot1, pot2;
   * .......
   * ScheduleMultiDim< Potential< double > > f1(pot1);
   * ScheduleMultiDim< Potential< double > > f2(pot2);
   *
   * // define the ScheduleBinaryCombination
   * ScheduleBinaryCombination< Potential< double >,
   *                            Potential< double >,
   *                            Potential< double > > comb(f1, f2, myadd);
   *
   * // before executing the combination, check whether we have sufficient
   * // memory and time to do it
   * const std::pair<double,double> memory = comb.memoryUsage();
   * const nb_operations = comb.nbOperations();
   *
   * // execute the combination and get the result
   * comb.execute();
   * const ScheduleMultiDim< Potential< double > >& result = comb.result();
   * const Potential< double >& pot_result = result.multiDim();
   *
   * @endcode
   */
  template < typename TABLE1, typename TABLE2, typename TABLE_RES >
  class ScheduleBinaryCombination: public ScheduleOperator {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @param table1 the first ScheduleMultiDim to combine with the other table
     * @param table2 the second table involved in the combination
     * @param combine a function taking two (real) tables in argument and
     * returning the result of their combination
     * @param is_result_persistent this boolean indicates whether the result of
     * the binary combination is persistent, i.e., whether it should be kept in
     * memory when the operator itself is deleted from memory.
     * @warning tables 1 and 2 are stored only by reference within the
     * ScheduleBinaryCombination. */
    ScheduleBinaryCombination(const ScheduleMultiDim< TABLE1 >& table1,
                              const ScheduleMultiDim< TABLE2 >& table2,
                              TABLE_RES (*combine)(const TABLE1&, const TABLE2&),
                              const bool is_result_persistent = false);

    /// copy constructor
    ScheduleBinaryCombination(const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >& from);

    /// move constructor
    ScheduleBinaryCombination(ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >&& from);

    /// virtual copy constructor
    ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >* clone() const final;

    /// destructor
    /** @warning If the ScheduleOperator has created some output
     * ScheduleMultiDim, upon destruction, it is removed from memory */
    virtual ~ScheduleBinaryCombination();

    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >&
       operator=(const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >&);

    /// move operator
    ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >&
       operator=(ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >&&);

    /// operator ==
    /** Two operators are identical if and only if they have equal (==)
     * arguments and they perform the same operations (e.g., both perform
     * additions). By Equal arguments, we mean that these ScheduleMultiDims have
     * the same IDs */
    bool operator==(const ScheduleOperator&) const final;

    /// operator !=
    /** Two operators are different if and only if they either have different
     * ScheduleMultiDim arguments or they perform different operations
     * (e.g., one performs an addition and another one a subtraction). different
     * ScheduleMultiDim arguments means that the latter differ by their Ids. */
    bool operator!=(const ScheduleOperator&) const final;

    /// operator ==
    /** Two operators are identical if and only if they have equal
     * arguments and they perform the same operation (e.g., both perform
     * additions). By Equal arguments, we mean that these ScheduleMultiDims have
     * the same IDs */
    bool operator==(const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >&) const;

    /// operator !=
    /** Two operators are different if and only if they either have different
     * ScheduleMultiDim arguments or they perform different operations
     * (e.g., one performs an addition and another one a subtraction). Different
     * ScheduleMultiDim arguments means that the latter differ by their Ids. */
    bool operator!=(const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >&) const;

    /// @}


    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /** @brief checks whether two ScheduleCombination have the same parameters
     * (same variables and same content)
     *
     * Parameters having the same variables and the same content are essentially
     * identical but they may have different Ids (so that they may not be ==).
     */
    bool hasSameArguments(const ScheduleOperator&) const final;

    /** @brief checks whether two ScheduleCombination have the same parameters
     * (same variables and same content)
     *
     * Parameters having the same variables and the same content are essentially
     * identical but they may have different Ids (so that they may not be ==).
     */
    bool hasSameArguments(const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >&) const;

    /** @brief checks whether two ScheduleCombination have similar parameters
     * (same variables) */
    bool hasSimilarArguments(const ScheduleOperator&) const final;

    /** @brief checks whether two ScheduleCombination have similar parameters
     * (same variables) */
    bool hasSimilarArguments(const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >&) const;

    /// checks whether two ScheduleOperator perform the same set of operations
    bool isSameOperator(const ScheduleOperator&) const final;

    /// checks whether two ScheduleOperator perform the same set of operations
    bool isSameOperator(const ScheduleBinaryCombination< TABLE1, TABLE2, TABLE_RES >&) const;

    /// returns the first argument of the combination
    const ScheduleMultiDim< TABLE1 >& arg1() const;

    /// returns the first argument of the combination
    const ScheduleMultiDim< TABLE2 >& arg2() const;

    /// returns the sequence of arguments passed to the operator
    const Sequence< const IScheduleMultiDim* >& args() const final;

    /// returns the result of the combination
    /** @return the ScheduleMultiDim resulting from the combination. It is
     * abstract if the combination has not been performed yet.
     */
    const ScheduleMultiDim< TABLE_RES >& result() const;

    /// returns the ScheduleMultidim resulting from the operator
    /** @warning Note that the Operator always returns its outputs, even if
     * it has not been executed. In this case, the outputs are abstract
     * ScheduleMultiDim.
     * @return the sequence of ScheduleMultiDim resulting from the operator.
     * Those can be abstract if the operator has not been performed yet.
     */
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
    /** This method guarantees that the result of the combination is a stored
     * into a newly allocated table */
    void execute() final;

    /// undo a previous execution, if any
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
     * memory used by the resulting table) */
    std::pair< double, double > memoryUsage() const final;

    /// displays the content of the operator
    std::string toString() const final;

    /// use a new combination function
    void setCombinationFunction(TABLE_RES (*combine)(const TABLE1&, const TABLE2&));

    /// @}


    private:
    /// the first argument of the combination
    const ScheduleMultiDim< TABLE1 >* _arg1_{nullptr};

    /// the second argument of the combination
    const ScheduleMultiDim< TABLE2 >* _arg2_{nullptr};

    /// the sequence of arguments passed to the operator
    /** This method is convenient when using ScheduleOperator rather than
     * directly using ScheduleBinaryCombination */
    Sequence< const IScheduleMultiDim* > _args_;

    /// the result of the combination
    ScheduleMultiDim< TABLE_RES >* _result_{nullptr};

    /// the sequence of ScheduleMultidim output by the operator
    /** @warning Note that the Operator has always some output, even if
     * it has not been executed. In this case, the outputs are abstract
     * ScheduleMultiDim.
     */
    Sequence< const IScheduleMultiDim* > _results_;

    /// the function actually used to perform the combination
    TABLE_RES (*_combine_)(const TABLE1&, const TABLE2&);
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/tools/graphicalModels/inference/scheduler/scheduleBinaryCombination_tpl.h>

#endif /* GUM_SCHEDULE_BINARY_COMBINATION_H */
