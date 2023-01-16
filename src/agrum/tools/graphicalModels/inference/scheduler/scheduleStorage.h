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
 * @brief Class for storing multidimensional tables in scheduling inferences
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef GUM_SCHEDULE_STORAGE_H
#define GUM_SCHEDULE_STORAGE_H

#include <type_traits>
#include <string>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/tools/core/exceptions.h>
#include <agrum/tools/graphicalModels/inference/scheduler/scheduleMultiDim.h>
#include <agrum/tools/graphicalModels/inference/scheduler/scheduleOperator.h>

namespace gum {

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  namespace ScheduleStorageMethod {

    /// A general structure to contain the method actually used to store
    /// the multidimensional table into a data structure (container)
    /** @warning we assume that the container will take the ownership of the
     * table after the table has been inserted into it. Namely, if the
     * container contains pointers to tables, then, upon destruction, it will
     * take care of deallocating the pointers it contains. If the container
     * directly contains tables, then those are supposed to be inserted into
     * the container by moves. */
    template < typename TABLE, typename CONTAINER_TABLE, template < typename... > class CONTAINER >
    struct Execution {
      void execute(TABLE& table, CONTAINER< CONTAINER_TABLE >& container);
    };

    /// Specialized structure to store a Table into a Set<Table*>
    /** @warning we assume that the container will take the ownership of the
     * tables inserted into it, i.e., upon destruction, it will take care of
     * deallocating the pointers it contains. */
    template < typename TABLE >
    struct Execution< TABLE, TABLE*, Set > {
      void execute(TABLE& table, Set< TABLE* >& container);
    };

    /// a Specialized structure to store a Table into a vector<Table>
    /** @warning we assume that the container will take the ownership of the
     * tables inserted into it, i.e., the tables are supposed to be inserted
     * into the container by moves. */
    template < typename TABLE >
    struct Execution< TABLE, TABLE, std::vector > {
      void execute(TABLE& table, std::vector< TABLE >& container);
    };

  }   // namespace ScheduleStorageMethod

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  /**
   * @class ScheduleStorage
   * @brief Class for storing multidimensional tables into containers (sets, etc.)
   * @headerfile scheduleStore.h <agrum/tools/graphicalModels/inference/scheduler/scheduleStore.h>
   * @ingroup inference_schedule
   */
  template < typename TABLE, template < typename... > class CONTAINER >
  class ScheduleStorage:
      public ScheduleOperator,
      private ScheduleStorageMethod::
         Execution< typename std::remove_pointer< TABLE >::type, TABLE, CONTAINER > {
    public:
    using SCHED_TABLE = typename std::remove_pointer< TABLE >::type;


    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @param table the ScheduleMultiDim containing the table that will
     * eventually be stored.
     * @warning table is stored only by reference within the ScheduleStorage.
     */
    explicit ScheduleStorage(const IScheduleMultiDim& table, CONTAINER< TABLE >& container);

    /// copy constructor
    ScheduleStorage(const ScheduleStorage< TABLE, CONTAINER >& from);

    /// move constructor
    ScheduleStorage(ScheduleStorage< TABLE, CONTAINER >&& from);

    /// virtual copy constructor
    ScheduleStorage< TABLE, CONTAINER >* clone() const final;

    /// destructor
    virtual ~ScheduleStorage();

    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    ScheduleStorage< TABLE, CONTAINER >& operator=(const ScheduleStorage< TABLE, CONTAINER >&);

    /// move operator
    ScheduleStorage< TABLE, CONTAINER >& operator=(ScheduleStorage< TABLE, CONTAINER >&&);

    /// operator ==
    /** Two ScheduleStorage are identical if and only if they have equal (==)
     * ScheduleMultiDim arguments and the same containers to store their tables
     * into. By Equal arguments, we stress that we mean that these
     * ScheduleMultiDims have the same IDs*/
    bool operator==(const ScheduleOperator&) const final;

    /// operator !=
    /** Two ScheduleStorage are identical if and only if they have equal (==)
     * ScheduleMultiDim arguments and the same containers to store their tables
     * into. By Equal arguments, we stress that we mean that these
     * ScheduleMultiDims have the same IDs*/
    bool operator!=(const ScheduleOperator&) const final;

    /// operator ==
    /** Two ScheduleStorage are identical if and only if they have equal (==)
     * ScheduleMultiDim arguments and the same containers to store their tables
     * into. By Equal arguments, we stress that we mean that these
     * ScheduleMultiDims have the same IDs*/
    virtual bool operator==(const ScheduleStorage< TABLE, CONTAINER >&) const;

    /// operator !=
    /** Two ScheduleStorage are identical if and only if they have equal (==)
     * ScheduleMultiDim arguments and the same containers to store their tables
     * into. By Equal arguments, we stress that we mean that these
     * ScheduleMultiDims have the same IDs*/
    virtual bool operator!=(const ScheduleStorage< TABLE, CONTAINER >&) const;

    /// @}


    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /** @brief checks whether two ScheduleStorage have the same parameters
     * (same variables and same content, and same container)
     *
     * ScheduleMultiDim Parameters having the same variables and the same content
     * are essentially identical but they may have different Ids (so that they
     * may not be ==).
     */
    bool hasSameArguments(const ScheduleOperator&) const final;

    /** @brief checks whether two ScheduleDeletion have the same parameters
     * (same variables and same content, and same container)
     *
     * ScheduleMultiDim Parameters having the same variables and the same content
     * are essentially identical but they may have different Ids (so that they
     * may not be ==).
     */
    bool hasSameArguments(const ScheduleStorage< TABLE, CONTAINER >&) const;

    /** @brief checks whether two ScheduleProjection have similar parameters
     * (same variables but not necessarily the same content)
     */
    bool hasSimilarArguments(const ScheduleOperator&) const final;

    /** @brief checks whether two ScheduleProjection have similar parameters
     * (same variables but not necessarily the same content)
     */
    bool hasSimilarArguments(const ScheduleStorage< TABLE, CONTAINER >&) const;

    /// checks whether two ScheduleOperator perform the same operation
    bool isSameOperator(const ScheduleOperator&) const final;

    /// checks whether two ScheduleOperator perform the same operation
    bool isSameOperator(const ScheduleStorage< TABLE, CONTAINER >&) const;

    /// returns the argument of the storing function
    const ScheduleMultiDim< SCHED_TABLE >& arg() const;

    /// returns the sequence of arguments passed to the operator
    const Sequence< const IScheduleMultiDim* >& args() const final;

    /// returns the sequence of ScheduleMultidim output by the operator
    const Sequence< const IScheduleMultiDim* >& results() const final;

    /// really executes the operator
    void execute() final;

    /// indicates whether the operator has been executed
    bool isExecuted() const final;

    /// undo a previous execution, if any
    /** throws OperationNotAllowed is raised if the operator is unable to
     * perform undo */
    void undo() final;

    /// modifies the arguments of the operator
    /** @throws SizeError is raised if the number of elements in new_args
     * does not correspond to the number of arguments expected by the
     * ScheduleOperator.
     * @throws TypeError is raised if at least one element of new_args does
     * not have a type compatible with what the ScheduleOperator expects.
     */
    void updateArgs(const Sequence< const IScheduleMultiDim* >& new_args) final;

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
     * deleted is abstract or not: it assumes that it is not when the Store
     * is performed. This enables to take into account the memory usage of a
     * whole Schedule tree before performing any operation within it. */
    std::pair< double, double > memoryUsage() const final;

    /// displays the content of the operator
    std::string toString() const final;

    /// @}


    private:
    /// the table to store
    ScheduleMultiDim< SCHED_TABLE >* _arg_;

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

    /// the container into which the table is stored
    CONTAINER< TABLE >* _container_;

    /// indicates whether the operator has been performed or not
    bool _is_executed_{false};
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/tools/graphicalModels/inference/scheduler/scheduleStorage_tpl.h>

#endif /* GUM_SCHEDULE_STORAGE_H */
