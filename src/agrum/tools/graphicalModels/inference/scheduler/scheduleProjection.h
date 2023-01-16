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
 * @brief a Projection operator class used for scheduling inferences
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef GUM_SCHEDULE_PROJECTION_H
#define GUM_SCHEDULE_PROJECTION_H

#include <string>
#include <memory>

#include <agrum/agrum.h>

#include <agrum/tools/graphicalModels/inference/scheduler/scheduleMultiDim.h>
#include <agrum/tools/graphicalModels/inference/scheduler/scheduleOperator.h>

namespace gum {


  /**
   * @class ScheduleProjection
   * @brief a Projection operator class used for scheduling inferences
   * @headerfile scheduleProjection.h <agrum/graphicalModels/inference/scheduleProjection.h>
   * @ingroup inference_schedule
   *
   * ScheduleProjection is a generic wrapper designed to project efficiently a
   * ScheduleMultiDim objects over a subset of its variables.
   *
   * To be quite generic, the ScheduleProjection takes in argument the function
   * that produces the result of the projection of the object contained inside
   * the ScheduleMultiDim. Note that executing a ScheduleProjection will always
   * produce a freshly allocated table.
   * The following code gives an example of the usage of ScheduleProjection:
   * @code
   * // two functions used to project a Potential<float>:
   * Potential<float> MinPot ( const Potential<float>& table,
   *                           const Set<const DiscreteVariable*>& del_vars ) {
   *   return Potential<float> (...);
   * }
   *
   * Potential<float> MaxPot ( const Potential<float>& table,
   *                           const Set<const DiscreteVariable*>& del_vars ) {
   *   return Potential<float> (...);
   * }
   *
   *
   * ScheduleMultiDim< Potential< float > > t1(...), t2(...);
   * Set<const DiscreteVariable*> set1, set2;
   * ScheduleProjection< Potential<float> > proj1 ( t1, set1, MinPot );
   * proj1.execute();
   * ScheduleProjection< Potential<float> > proj2 ( proj1.result(), set1, MaxPot );
   *
   * // change the operator to apply
   * proj1.setProjectionFunction ( MaxPot );
   * SchedulePotential< Potential<float> > proj_table2 = proj1.result();
   *
   * @endcode
   *
   */
  template < typename TABLE >
  class ScheduleProjection: public ScheduleOperator {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @param table the ScheduleMultiDim containing the table that will
     * be projected
     * @param del_vars the set of variables that will be removed from table.
     * Note that variables in del_vars that do not belong to table are simply
     * not taken into account. They do not raise any exception.
     * @param project the function that will actually perform the projection
     * @param is_result_persistent this boolean indicates whether the result of
     * the projection is persistent, i.e., whether it should be kept in
     * memory when the operator itself is deleted from memory.
     * @warning table is stored only by reference within the ScheduleProjection.
     * But the set of variables to remove (del_vars) is stored by copy
     * within the ScheduleProjection */
    explicit ScheduleProjection(const ScheduleMultiDim< TABLE >&      table,
                                const Set< const DiscreteVariable* >& del_vars,
                                TABLE (*project)(const TABLE&,
                                                 const Set< const DiscreteVariable* >&),
                                const bool is_result_persistent = false);

    /// copy constructor
    ScheduleProjection(const ScheduleProjection< TABLE >& from);

    /// move constructor
    ScheduleProjection(ScheduleProjection< TABLE >&& from);

    /// virtual copy constructor
    ScheduleProjection< TABLE >* clone() const final;

    /// destructor
    /** @warning If the ScheduleProjection has created some output
     * SchedulePotential, upon destruction, it is removed from memory */
    virtual ~ScheduleProjection();

    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    ScheduleProjection< TABLE >& operator=(const ScheduleProjection< TABLE >&);

    /// move operator
    ScheduleProjection< TABLE >& operator=(ScheduleProjection< TABLE >&&);

    /// operator ==
    /** Two operators are identical if and only if they have equal (==)
     * ScheduleMultiDim arguments, the same set of variables to project on, and
     * they perform exactly the same set of operations (e.g., both perform a max
     * projection). By Equal arguments, we stress that we mean that these
     * ScheduleMultiDims have the same IDs*/
    bool operator==(const ScheduleOperator&) const final;

    /// operator !=
    /** Two operators are different if and only if they either have different
     * (!=) ScheduleMultiDim arguments or they have different sets of variables
     * to project on, or they perform different operations (e.g., one performs a
     * max projection and the other a min projection). Different
     * ScheduleMultiDim arguments means that the latter differ by their Ids. */
    bool operator!=(const ScheduleOperator&) const final;

    /// operator ==
    /** Two operators are identical if and only if they have equal (==)
     * ScheduleMultiDim arguments, the same set of variables to project on, and
     * they perform exactly the same set of operations (e.g., both perform a max
     * projection). By Equal arguments, we mean that these ScheduleMultiDims have
     * the same IDs */
    virtual bool operator==(const ScheduleProjection< TABLE >&) const;

    /// operator !=
    /** Two operators are different if and only if they either have different
     * (!=) ScheduleMultiDim arguments or they have different sets of variables
     * to project on, or they perform different operations (e.g., one performs a
     * max projection and the other a min projection). Different
     * ScheduleMultiDim arguments means that the latter differ by their Ids. */
    virtual bool operator!=(const ScheduleProjection< TABLE >&) const;

    /// @}


    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /** @brief checks whether two ScheduleProjection have the same parameters
     * (same variables, including the set of variables to delete, and same content)
     *
     * ScheduleMultiDim Parameters having the same variables and the same content
     * are essentially identical but they may have different Ids (so that they
     * may not be ==).
     */
    bool hasSameArguments(const ScheduleOperator&) const final;

    /** @brief checks whether two ScheduleProjection have the same parameters
     * (same variables, including the set of variables to delete, and same content)
     *
     * ScheduleMultiDim Parameters having the same variables and the same content
     * are essentially identical but they may have different Ids (so that they
     * may not be ==).
     */
    bool hasSameArguments(const ScheduleProjection< TABLE >&) const;

    /** @brief checks whether two ScheduleProjection have similar parameters
     * (same variables, including the set of variables to delete)
     *
     * ScheduleMultiDim Parameters having the same variables and the same content
     * are essentially identical but they may have different Ids (so that they
     * may not be ==).
     */
    bool hasSimilarArguments(const ScheduleOperator&) const final;

    /** @brief checks whether two ScheduleProjection have similar parameters
     * (same variables, including the set of variables to delete)
     *
     * ScheduleMultiDim Parameters having the same variables and the same content
     * are essentially identical but they may have different Ids (so that they
     * may not be ==).
     */
    bool hasSimilarArguments(const ScheduleProjection< TABLE >&) const;

    /// checks whether two ScheduleOperator perform the same projection
    bool isSameOperator(const ScheduleOperator&) const final;

    /// checks whether two ScheduleOperator perform the same projection
    bool isSameOperator(const ScheduleProjection< TABLE >&) const;

    /// returns the argument of the projection
    const ScheduleMultiDim< TABLE >& arg() const;

    /// returns the sequence of arguments passed to the operator
    const Sequence< const IScheduleMultiDim* >& args() const final;

    /// returns the result of the projection
    const ScheduleMultiDim< TABLE >& result() const;

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
     * memory used by the resulting table ) */
    std::pair< double, double > memoryUsage() const final;

    /// displays the content of the operator
    std::string toString() const final;

    /// use a new projection function
    void setProjectionFunction(TABLE (*project)(const TABLE&,
                                                const Set< const DiscreteVariable* >&));

    /// @}

    private:
    /// the argument to be projected
    const ScheduleMultiDim< TABLE >* _arg_{nullptr};

    /// the sequence of arguments passed to the operator
    /** This method is convenient when using ScheduleOperator rather than
     * directly using ScheduleBinaryCombination */
    Sequence< const IScheduleMultiDim* > _args_;

    /// the result of the projection
    ScheduleMultiDim< TABLE >* _result_{nullptr};

    /// the sequence of ScheduleMultidim output by the operator
    /** @warning Note that the Operator has always some output, even if
     * it has not been executed. In this case, the outputs are abstract
     * ScheduleMultiDim.
     */
    Sequence< const IScheduleMultiDim* > _results_;

    /// the set of variables that should be removed from the _arg_ table
    Set< const DiscreteVariable* > _del_vars_;

    /// the projection operator
    TABLE (*_project_)(const TABLE&, const Set< const DiscreteVariable* >&);
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/tools/graphicalModels/inference/scheduler/scheduleProjection_tpl.h>

#endif /* GUM_SCHEDULE_PROJECTION_H */
