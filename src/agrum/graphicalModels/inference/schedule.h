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
 * @brief Class containing a schedule of operations to perform on multidims
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_SCHEDULE_H
#define GUM_SCHEDULE_H

#include <utility>

#include <agrum/config.h>

#include <agrum/core/hashTable.h>
#include <agrum/core/set.h>
#include <agrum/graphs/DAG.h>
#include <agrum/graphs/graphElements.h>

#include <agrum/graphicalModels/inference/scheduleOperation.h>
#include <agrum/graphicalModels/inference/scheduleCombine.h>
#include <agrum/graphicalModels/inference/scheduleProject.h>
#include <agrum/graphicalModels/inference/scheduleCliqueStoreMultiDim.h>
#include <agrum/graphicalModels/inference/scheduleSeparatorStoreMultiDim.h>
#include <agrum/graphicalModels/inference/scheduleDeleteMultiDim.h>

namespace gum {

  /**
   * @class Schedule
   * @brief Class containing a schedule of operations to perform on multidims
   *
   * A Schedule class contains a set of operations to be scheduled. It is able to
   * indicate which operations can currently be performed (because all their
   * arguments have already been computed). In addition, it is possible to insert
   * new operations into the schedule (at a specific place) and to remove some
   * operations.
   *
   * @warning In the Schedule class does not guarantee that the schedule can always
   * be performed, i.e., that there always exists a sequence in which all the
   * operations of the schedule can be performed. It is up to the user to check
   * this (function schedulePossible () may help here).
   */
  template <typename GUM_SCALAR> class Schedule {
    public:
    /// to identify correctly the ids that correspond to ScheduleOperation ids
    typedef Id OperationId;

    /// to identify correctly the ids that correspond to ScheduleMultiDim ids
    typedef Id MultiDimId;

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor (construct an empty sequence)
    Schedule();

    /// copy constructor
    Schedule(const Schedule<GUM_SCALAR> &);

    /// destructor
    ~Schedule();

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################

    /// @{

    /// copy operator
    Schedule<GUM_SCALAR> &operator=(const Schedule<GUM_SCALAR> &);

    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// inserts an operation to be scheduled
    /** The Schedule class is able to determined by itself when the operation
     * should be performed.
     * @warning operations are inserted by copy */
    NodeId insert(const ScheduleOperation<GUM_SCALAR> &);

    /** @brief adds a constraint indicating that an operation cannot be performed
     * before another one */
    void forceAfter(const ScheduleOperation<GUM_SCALAR> &op_to_force,
                    const ScheduleOperation<GUM_SCALAR> &op_before);
    void forceAfter(NodeId op_to_force, NodeId op_before);

    /** @brief adds a constraint indicating that an operation cannot be performed
     * before a set of operations */
    void forceAfter(const ScheduleOperation<GUM_SCALAR> &op_to_force,
                    const Set<const ScheduleOperation<GUM_SCALAR> *> &ops_before);
    void forceAfter(NodeId op_to_force, const NodeSet &ops_before);

    /** @brief adds a constraint indicating that an operation must be performed
     * before another one */
    void forceBefore(const ScheduleOperation<GUM_SCALAR> &op_to_force,
                     const ScheduleOperation<GUM_SCALAR> &op_after);
    void forceBefore(NodeId op_to_force, NodeId op_after);

    /** @brief adds a constraint indicating that an operation must be performed
     * before a set of operations */
    void forceBefore(const ScheduleOperation<GUM_SCALAR> &op_to_force,
                     const Set<const ScheduleOperation<GUM_SCALAR> *> &ops_after);
    void forceBefore(NodeId op_to_force, const NodeSet &ops_after);

    /// returns a DAG indicating in which order the operations can be performed
    /** In this DAG, each node corresponds to an operation and an operation
     * can be performed only if its ancestors have all been performed. */
    const DAG &dag() const;

    /// returns the scheduleOperation corresponding to an id in the DAG
    /** @throws NotFound exception is raised if the DAG does not contain the id */
    const ScheduleOperation<GUM_SCALAR> &operation(NodeId) const;

    /// returns the id of the node corresponding to a given ScheduleOperation
    /** @throws NotFound exception is raised the operation does not belong to
     * the Schedule */
    NodeId nodeId(const ScheduleOperation<GUM_SCALAR> &) const;

    /// resturns the association between operations anf nodeIds
    const NodeProperty<const ScheduleOperation<GUM_SCALAR> *> &operations() const;

    /// returns the set of operations involving a given multidim table
    const NodeSet &
    operationsInvolving(const ScheduleMultiDim<GUM_SCALAR> &table) const;
    const NodeSet &operationsInvolving(MultiDimId table_id) const;

    /// returns the set of ScheduleOperations that can be executed at once
    /** The scheduleOperations that can be executed at once are those that
     have no parent or whose parents have already been executed. */
    const NodeSet &availableOperations() const;

    /// executes a given operation (if this one is available)
    /** Note that, whenever an operation is performed, the list of available
     * operations is updated and the operation itslef is removed from the
     * schedule
     * @throws OperationNotAllowed exception is thrown if the operation cannot be
     * executed yet because some of its arguments have not already been computed
     * @throws NotFound exception is thrown if the operation cannot be found */
    void execute(NodeId);
    void execute(const ScheduleOperation<GUM_SCALAR> &);

    /** @brief returns an estimation of the number of elementary operations needed
     * to perform a given ScheduleOperation */
    float nbOperations(NodeId) const;
    float nbOperations(ScheduleOperation<GUM_SCALAR> &) const;

    /// returns the memory consumption used during the execution of an operation
    /** Actually, this function does not return a precise account of the memory
     * used by the scheduleOperation but a rough estimate based on the sizes
     * of the tables involved in the operation.
     * @return a pair of memory consumption: the first one is the maximum
     * amount of memory used during the ScheduleOperation and the second one is the
     * amount of memory still used at the end of the operation ( the memory used by
     * the resulting table ) */
    std::pair<long, long> memoryUsage(NodeId) const;
    std::pair<long, long> memoryUsage(ScheduleOperation<GUM_SCALAR> &) const;

    /// @}

    private:
    /// the DAG of the operations to perform
    /** Operations can be scheduled as a DAG: nodes without parents can be
     * executed directly. The other nodes need their parents to be executed to
     * get all their arguments constructed. */
    mutable DAG __dag;

    /// a hashtable assigning to each node of the DAG an operation
    NodeProperty<ScheduleOperation<GUM_SCALAR> *> __node2operation;

    /// a hashtable assigning to each operation id a node id in the DAG
    HashTable<OperationId, NodeId> __operation2node;

    /** @brief a hashtable assigning to each ScheduleMultiDim resulting from a
     * computation the MultiDimOperation node id that created it */
    HashTable<MultiDimId, NodeId> __created_multidims;

    /// a list of operations whose parents are not properly set
    /** when entering operations to be performed in a "wrong" order, it may
     * happen that the parents of some operations in the DAG are not properly
     * set (the parents being inserted after the child). We keep a list of
     * such nodes and, whenever we wish to perform the schedule or get its DAG,
     * we compute the correct set of parents of the operations of this list
     * and, when this is done, we remove them from the list. As such, when the
     * list is empty, the schedule can be performed. */
    mutable NodeSet __operations_with_wrong_parents;

    /// the set of operations that can be executed at once
    mutable NodeSet __operations_available;

    /// for each multidim, store the set of operations involving it
    HashTable<MultiDimId, NodeSet *> __multidim2operations;

    /** @brief updates the set of parents for the nodes whoses parents are not
     * correct yet and update accordingly the available operations */
    void __updateWrongParents() const;
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/graphicalModels/inference/schedule.tcc>

#endif /* GUM_SCHEDULE_H */
