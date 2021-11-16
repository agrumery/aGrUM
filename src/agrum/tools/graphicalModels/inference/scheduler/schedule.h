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
 * @brief Class containing a schedule of operations to perform on multidims
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#ifndef GUM_SCHEDULE_H
#define GUM_SCHEDULE_H

#include <string>
#include <sstream>
#include <utility>
#include <memory>

#include <agrum/agrum.h>

#include <agrum/tools/core/bijection.h>
#include <agrum/tools/core/hashTable.h>
#include <agrum/tools/core/list.h>
#include <agrum/tools/core/set.h>
#include <agrum/tools/graphs/DAG.h>

#include <agrum/tools/graphicalModels/inference/scheduler/scheduleBinaryCombination.h>
#include <agrum/tools/graphicalModels/inference/scheduler/scheduleDeletion.h>
#include <agrum/tools/graphicalModels/inference/scheduler/scheduleProjection.h>
#include <agrum/tools/graphicalModels/inference/scheduler/scheduleStorage.h>

namespace gum {

  /**
   * @class Schedule
   * @brief Class containing a schedule of operations to perform on multidims
   * @headerfile schedule.h <agrum/tools/graphicalModels/inference/scheduler/schedule.h>
   *
   * @ingroup inference_schedule
   *
   * A Schedule class contains a set of operations to be scheduled. It is able
   * to indicate which operations can currently be performed (because all their
   * arguments have already been computed). In addition, it is possible to
   * insert new operations into the schedule (at a specific location) and to
   * remove some operations.
   */
  template < template < typename > class ALLOC = std::allocator  >
  class Schedule : private ALLOC< Idx > {
    public:
    using allocator_type = ALLOC< Idx >;

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor (construct an empty sequence)
    explicit Schedule(const allocator_type& alloc = allocator_type());

    /// copy constructor
    Schedule(const Schedule< ALLOC >& from);

    /// copy constructor with a given allocator
    Schedule(const Schedule< ALLOC >& from,
             const allocator_type&    alloc);

    /// move constructor
    Schedule(Schedule< ALLOC >&& from);

    /// move constructor with a given allocator
    Schedule(Schedule< ALLOC >&&   from,
             const allocator_type& alloc);

    /// virtual copy constructor
    virtual Schedule< ALLOC >* clone() const;

    /// virtual copy constructor with a given allocator
    virtual Schedule< ALLOC >* clone(const allocator_type& alloc) const;
    
    /// destructor
    virtual ~Schedule();

    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################

    /// @{

    /// copy operator
    Schedule< ALLOC >& operator=(const Schedule< ALLOC >&);

    /// move operator
    Schedule< ALLOC >& operator=(Schedule< ALLOC >&&);

    /// operator ==
    bool operator==(const Schedule< ALLOC >&) const;

    /// operator !=
    bool operator!=(const Schedule< ALLOC >&) const;
    
    /// @}
    

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /** @brief inserts a new table as a source operation's input, i.e., this
     * table is not computed by any ScheduleOperation
     *
     * ScheduleOperations produce new tables that can be passed as arguments to
     * other ScheduleOperations. However source operations, i.e., those that
     * are the sources of the DAG (they are the first to be computed), need
     * arguments/tables that have been created before the Schedule. These tables
     * should be inserted into the schedule using this insert method
     * @param table the table to be inserted into the schedule. Note that this
     * creates a ScheduleMultiDim that references this table (the latter is
     * not copied).
     * @throws DuplicateScheduleMultiDim is thrown if the schedule already
     * contains a table with the same id
     */
    template < typename TABLE >
    const IScheduleMultiDim< ALLOC >& insertTable(const TABLE& table,
                                                  const bool   copy,
                                                  const Idx    id = 0);

    /** @brief inserts a new table as a source operation's input, i.e., this
     * table is not computed by any ScheduleOperation
     *
     * ScheduleOperations produce new tables that can be passed as arguments to
     * other ScheduleOperations. However source operations, i.e., those that
     * are the sources of the DAG (they are the first to be computed), need
     * arguments/tables that have been created before the Schedule. These tables
     * can be inserted into the schedule using this insert method
     * @param multidim the ScheduleMultiDim to be inserted into the schedule. Note
     * that this clones the ScheduleMultiDim passed in argument if it does not
     * already belong to the schedule, or it returns it otherwise.
     * @throws AbstractScheduleMultiDim is thrown if the ScheduleMultiDim is
     * abstract, i.e., it does not contain a true table. The tables we insert
     * with method insertTable are not computed by any schedule operations,
     * hence they should never be abstract, else the schedule may not be
     * executable.
      * @throws DuplicateScheduleMultiDim is thrown if the schedule already
     * contains a table with the same id
     */
    const IScheduleMultiDim< ALLOC >&
       insertScheduleMultiDim(const IScheduleMultiDim< ALLOC >& multidim);

    /// inserts an operation into the schedule
    /** The Schedule class is able to determine by itself when the operation
     * can be performed.
     * @throws UnknownScheduleMultiDim if some of the arguments of op do not
     * already belong to the schedule. Such arguments would indeed prevent the
     * schedule to be performed.
     * @throws OperationNotAllowed if the operation deletes its arguments and
     * either another operation also deletes some of them or the operation has
     * already been performed but other operations use some of these arguments
     * and they have not been executed yet.
     * @warning operations are inserted by cloning */
    const ScheduleOperation< ALLOC >&
       insertOperation(const ScheduleOperation< ALLOC >& op);

    /// emplace a new schedule binary combination operation
    /** @throws UnknownScheduleMultiDim if some of the arguments of op do not
     * already belong to the schedule. Such arguments would indeed prevent the
     * schedule to be performed.
     * @throws OperationNotAllowed if the operation deletes its arguments and
     * either another operation also deletes some of them or the operation has
     * already been performed but other operations use some of these arguments
     * and they have not been executed yet.
     */
    template < typename TABLE1, typename TABLE2, typename TABLE_RES >
    const ScheduleOperation< ALLOC >&
    emplaceBinaryCombination(const ScheduleMultiDim< TABLE1, ALLOC >& table1,
                             const ScheduleMultiDim< TABLE2, ALLOC >& table2,
                             TABLE_RES (*combine)( const TABLE1&, const TABLE2&));

    /// emplace a new schedule projection operation
    /** @throws UnknownScheduleMultiDim if some of the arguments of op do not
     * already belong to the schedule. Such arguments would indeed prevent the
     * schedule to be performed.
     * @throws OperationNotAllowed if the operation deletes its arguments and
     * either another operation also deletes some of them or the operation has
     * already been performed but other operations use some of these arguments
     * and they have not been executed yet.
     */
    template < typename TABLE >
    const ScheduleOperation< ALLOC >&
    emplaceProjection(
          const ScheduleMultiDim< TABLE, ALLOC >& table,
          const Set< const DiscreteVariable* >&   del_vars,
          TABLE (*project)(const TABLE&, const Set< const DiscreteVariable* >&));

    /// emplace a new schedule deletion operation
    /** @throws UnknownScheduleMultiDim if some of the arguments of op do not
     * already belong to the schedule. Such arguments would indeed prevent the
     * schedule to be performed.
     * @throws OperationNotAllowed if the operation deletes its arguments and
     * either another operation also deletes some of them or the operation has
     * already been performed but other operations use some of these arguments
     * and they have not been executed yet.
     */
    template < typename TABLE >
    const ScheduleOperation< ALLOC >&
    emplaceDeletion(const ScheduleMultiDim< TABLE, ALLOC >& table);

     /// emplace a new schedule storage operation
     /** @throws UnknownScheduleMultiDim if some of the arguments of op do not
      * already belong to the schedule. Such arguments would indeed prevent the
      * schedule to be performed.
      * @throws OperationNotAllowed if the operation deletes its arguments and
      * either another operation also deletes some of them or the operation has
      * already been performed but other operations use some of these arguments
      * and they have not been executed yet.
      */
    template < typename TABLE,
               template < typename, typename... > class CONTAINER,
               typename... CONTAINER_PARAMS >
     const ScheduleOperation< ALLOC >&
     emplaceStorage(const IScheduleMultiDim< ALLOC >& table,
                    CONTAINER<TABLE, CONTAINER_PARAMS...>& container);

    /// returns a DAG indicating in which order the operations can be performed
    /** In this DAG, each node corresponds to an operation and an operation
     * can be performed only if its ancestors have all been performed. */
    const DAG& dag() const;

    /// returns the scheduleOperation corresponding to an id in the DAG
    /** @throws NotFound exception is raised if the DAG does not contain the id
     */
    const ScheduleOperation< ALLOC >& operation(const NodeId id) const;

    /// returns the id of the node corresponding to a given ScheduleOperation
    /** @throws NotFound exception is raised the operation does not belong to
     * the Schedule */
    NodeId nodeId(const ScheduleOperation< ALLOC >&) const;

    /** @brief returns the set of ScheduleOperations that are ready to be executed
     *
     * The operations that can be executed at once are those that
     * have no parent or whose parents have already been executed. */
    NodeSet availableOperations() const;

    /// updates the schedule after a given operation has been executed
    /** @warning Parallel schedulers should use this method only in critical
     * section because it updates the schedule's DAG.
     * @param exec_node The node id of the operation that has just been executed
     * @param new_available_nodes a vector that will be filled with all the new
     * operations becoming available due to exec_node being executed.
     * @param check if true, we check that the operation could be executed and
     * has actually been executed.
     * @throws UnavailableScheduleOperation exception is thrown in check mode if
     * the operation was not available and, hence, should not have been executed.
     * This should never happen.
     * @throws UnknownScheduleOperation exception is thrown in check mode if the
     * DAG does not contain the node of the executed operation. This should never
     * happen.
     * @throws UnexecutedScheduleOperation exception is thrown in check mode if
     * the operation has not yet been executed. This should never happen.
     */
    void updateAfterExecution(const NodeId exec_node,
                              std::vector<NodeId>& new_available_nodes,
                              const bool check = false);

    /// updates the schedule after executing a given operation
    /** @warning Parallel schedulers should use this method only in critical
     * section because it updates the schedule's DAG.
     * @param exec_op The operation that has just been executed
     * @param new_available_nodes a vector that will be filled with all the new
     * operations becoming available due to exec_op being executed.
     * @param check if true, we check that the operation could be executed and
     * has actually been executed.
     * @throws UnavailableScheduleOperation exception is thrown in check mode if
     * the operation was not available and, hence, should not have been executed.
     * This should never happen.
     * @throws UnknownScheduleOperation exception is thrown in check mode if the
     * DAG does not contain the node of the executed operation. This should never
     * happen.
     * @throws UnexecutedScheduleOperation exception is thrown in check mode if
     * the operation has not yet been executed. This should never happen.
     */
    void updateAfterExecution(const ScheduleOperation< ALLOC >& exec_op,
                              std::vector<NodeId>& new_available_nodes,
                              const bool check = false);

    /// returns the ScheduleMultiDim corresponding to a given id
    const IScheduleMultiDim< ALLOC >* scheduleMultiDim(const NodeId id) const;

    /// returns the id of a given IScheduleMultiDim
    NodeId scheduleMultiDimId(const IScheduleMultiDim< ALLOC >* multidim) const;

    /// empty the schedule, i.e., remove its content
    void clear();

    /// returns the allocator used by the operation
    allocator_type get_allocator() const;

    /// @}


  private:
    /// the DAG of the operations to perform
    /** Operations can be scheduled as a DAG: nodes without parents can be
     * executed directly. The other nodes need their parents to be executed to
     * get all their arguments constructed. */
    DAG _dag_;

    /// the highest node id in the graph
    NodeId _newId_ {NodeId(0)};

    /// a mapping between the ids of the operations and their pointer
    Bijection< NodeId, ScheduleOperation< ALLOC >* > _node2op_;

    /// a structure to indicate precisely where a ScheduleMultiDim comes from
    /** ScheduleMultiDims can either result from the execution of some
     * operation or reference a MultiDim created outside of the schedule. This
     * structure enables to discriminate these situations:
     * - if the first element of the std::pair is nullptr, this means that the
     * ScheduleMultiDim has been created outside of the schedule, i.e., that
     * it references an outside MultiDim.
     * - if the first element of the std::pair is different from nullptr, then
     * the ScheduleMultiDim results from a ScheduleOperation and the pointer
     * of this operation is this first argument. The second argument of the
     * std::pair indicates the index of the ScheduleMultiDim in the sequence
     * of results of the ScheduleOperation.
     */
    HashTable< const IScheduleMultiDim< ALLOC >*,
               std::pair< ScheduleOperation< ALLOC >*, Idx > >
       _multidim_location_;

    /// a bijection between pointers to IScheduleMultiDim and their Ids
    Bijection< const IScheduleMultiDim< ALLOC >*, Idx > _multidim2id_;

    /// maps the multidims to the set of operations that use them
    HashTable< const IScheduleMultiDim< ALLOC >*, NodeSet > _multidim2nodes_;

    /** @brief A bijection mapping ScheduleMultiDims to be deleted with the
     * operations that will delete them
     *
     * This information is of interest because, to limit the memory consumption,
     * a scheduler should consider executing the operations stored into this
     * bijection as soon as possible
     * @warning only the ScheduleMultiDim that will be eventually deleted are
     * included into this hashtable
     * @warning at most one operation is allowed to delete a ScheduleMultiDim
     * (else, we would free it several times from memory, which is bad, very bad)
     */
    Bijection< const IScheduleMultiDim< ALLOC >*, NodeId > _deleted_multidim2node_;



    /// a function to copy the content of a schedule into another one
    /** @warning this method assumes that "this" is an empty schedule. If this
     * is not the case, then we should first remove everything from "this" */
    void _copy_(const Schedule< ALLOC >& from, const allocator_type& alloc);

    /// a function to delete from memory the content of a schedule
    void _destroy_();

    /// returns the graph that would obtain when no operation is performed
    DAG _fullDAG_() const;

    /// returns the adjective corresponding to a parameter index (1st, 2nd, etc.)
    std::string _paramString_(Idx i) const;

  };

} /* namespace gum */

// always include the template implementation
#include <agrum/tools/graphicalModels/inference/scheduler/schedule_tpl.h>

#endif /* GUM_SCHEDULE_H */
