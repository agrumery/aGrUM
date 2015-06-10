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
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <agrum/config.h>

namespace gum {

  /// default constructor (construct an empty sequence)
  template <typename GUM_SCALAR> Schedule<GUM_SCALAR>::Schedule() {
    // for debugging purposes
    GUM_CONSTRUCTOR(Schedule);
  }

  /// copy constructor
  template <typename GUM_SCALAR>
  Schedule<GUM_SCALAR>::Schedule(const Schedule<GUM_SCALAR> &from)
      : __dag(from.__dag), __operation2node(from.__operation2node),
        __created_multidims(from.__created_multidims),
        __operations_with_wrong_parents(from.__operations_with_wrong_parents),
        __operations_available(from.__operations_available) {
    // for debugging purposes
    GUM_CONS_CPY(Schedule);

    // we must now copy the operations of "from" into "this"
    for (const auto &elt : from.__node2operation)
      __node2operation.insert(elt.first, elt.second->newFactory());

    // update the set of operations involved with each multidim table
    for (const auto &elt : from.__multidim2operations)
      __multidim2operations.insert(elt.first, new NodeSet(*elt.second));
  }

  /// destructor
  template <typename GUM_SCALAR> Schedule<GUM_SCALAR>::~Schedule() {
    // for debugging purposes
    GUM_DESTRUCTOR(Schedule);

    // remove all the operations that were stored

    for (const auto &elt : __node2operation)
      delete elt.second;

    // remove the sets of operations involved with each multidim table
    for (const auto &elt : __multidim2operations)
      delete elt.second;
  }

  /// copy operator
  template <typename GUM_SCALAR>
  Schedule<GUM_SCALAR> &Schedule<GUM_SCALAR>::operator=(const Schedule &from) {
    // avoid self assignment
    if (this != &from) {
      // remove all the operations that were stored
      for (const auto &elt : __node2operation)
        delete elt.second;

      // remove the sets of operations involved with each multidim table
      for (const auto &elt : __multidim2operations)
        delete elt.second;

      // fill all the data structures with the elements of from
      __dag = from.__dag;
      __operation2node = from.__operation2node;
      __created_multidims = from.__created_multidims;
      __operations_with_wrong_parents = from.__operations_with_wrong_parents;
      __operations_available = from.__operations_available;

      __node2operation.clear();

      for (const auto &elt : from.__node2operation)
        __node2operation.insert(elt.first, elt.second->newFactory());

      // update the set of operations involved with each multidim table
      __multidim2operations.clear();

      for (const auto &elt : from.__multidim2operations)
        __multidim2operations.insert(elt.first, new NodeSet(*elt.second));
    }

    return *this;
  }

  /// inserts an operation to be scheduled
  template <typename GUM_SCALAR>
  NodeId Schedule<GUM_SCALAR>::insert(const ScheduleOperation<GUM_SCALAR> &op) {
    // create a copy of the operation
    ScheduleOperation<GUM_SCALAR> *operation = op.newFactory();

    // create a new node for the operation in the DAG
    NodeId node_id = __dag.addNode();

    // assign the operation to the new node
    __node2operation.insert(node_id, operation);
    __operation2node.insert(operation->id(), node_id);

    // get the list of multidims passed in arguments and determine which ones
    // are abstract. If there are some abstract multidims, then the node we
    // just created must have parents. Try to add these arcs and, if some
    // parents have not been created yet, indicate it in the
    // __operations_with_wrong_parents list
    bool operation_available = true;

    for (const auto par : operation->multiDimArgs()) {
      if (par->isAbstract()) {
        // here we shall have a parent in the graph
        operation_available = false;
        MultiDimId multidim_id = par->id();

        if (__created_multidims.exists(multidim_id)) {
          __dag.addArc(__created_multidims[multidim_id], node_id);
        } else {
          __operations_with_wrong_parents.insert(node_id);
          break;
        }
      }
    }

    // if the operation is available to be processed, mark it as such
    if (operation_available)
      __operations_available.insert(node_id);

    // now we shall find whether, upon executing the operation, new multidim
    // tables are created
    NodeSet *involved_ops;

    for (const auto tab : operation->multiDimResults()) {
      MultiDimId table_id = tab->id();

      if (tab->isAbstract())
        __created_multidims.insert(table_id, node_id);

      if (!__multidim2operations.exists(table_id)) {
        involved_ops = __multidim2operations.insert(table_id, new NodeSet).second;
      } else {
        involved_ops = __multidim2operations[table_id];
      }

      involved_ops->insert(node_id);
    }

    // update __multidim2operations with the arguments passed to the newly
    // added operation
    for (const auto &par : operation->multiDimArgs()) {
      MultiDimId table_id = par->id();

      if (!__multidim2operations.exists(table_id)) {
        involved_ops = __multidim2operations.insert(table_id, new NodeSet).second;
      } else {
        involved_ops = __multidim2operations[table_id];
      }

      involved_ops->insert(node_id);
    }

    return node_id;
  }

  /// updates the set of parents for the nodes whoses parents are not correct yet
  template <typename GUM_SCALAR>
  void Schedule<GUM_SCALAR>::__updateWrongParents() const {
    // parse all the nodes whose parents sets are incorrect

    auto localWrongs = __operations_with_wrong_parents; // complete copy of NodeSet

    for (const auto wrong : localWrongs) {
      // get the arguments passed to wrong and check that those that are abstract
      // multidims belong to the schedule
      const Sequence<const ScheduleMultiDim<GUM_SCALAR> *> &args =
          __node2operation[wrong]->multiDimArgs();
      bool still_wrong = false;

      for (const auto arg : args) {
        if (arg->isAbstract() && !__created_multidims.exists(arg->id())) {
          still_wrong = true;
          break;
        }
      }

      // if the operation is not "still_wrong" anymore, then we should remove
      // it from __operations_with_wrong_parents and update its parents set
      // appropriately. In addition, if there is no parent, then we should
      // indicate that the operation is now available
      if (!still_wrong) {
        unsigned int nb_parents = 0;

        for (const auto arg : args)
          if (arg->isAbstract()) {
            __dag.addArc(__created_multidims[arg->id()], wrong);
            ++nb_parents;
          }

        // check that there is no parent
        if (!nb_parents) {
          __operations_available.insert(wrong);
        }

        __operations_with_wrong_parents.erase(wrong);
      }
    }
  }

  /** @brief adds a constraint indicating that an operation cannot be performed
   * before another one */
  template <typename GUM_SCALAR>
  INLINE void Schedule<GUM_SCALAR>::forceAfter(NodeId op_to_force,
                                               NodeId op_before) {
    // first, add the constraint into the graph
    __dag.addArc(op_before, op_to_force);

    // if op_to_force was available, it is not anymore
    __operations_available.erase(op_to_force);
  }

  /** @brief adds a constraint indicating that an operation cannot be performed
   * before another one */
  template <typename GUM_SCALAR>
  INLINE void
  Schedule<GUM_SCALAR>::forceAfter(const ScheduleOperation<GUM_SCALAR> &op_to_force,
                                   const ScheduleOperation<GUM_SCALAR> &op_before) {
    forceAfter(__operation2node[op_to_force.id()], __operation2node[op_before.id()]);
  }

  /** @brief adds a constraint indicating that an operation cannot be performed
   * before a set of operations */
  template <typename GUM_SCALAR>
  void Schedule<GUM_SCALAR>::forceAfter(NodeId op_to_force,
                                        const NodeSet &ops_before) {
    for (const auto op : ops_before)
      if (op != op_to_force)
        forceAfter(op_to_force, op);
  }

  /** @brief adds a constraint indicating that an operation cannot be performed
   * before a set of operations */
  template <typename GUM_SCALAR>
  void Schedule<GUM_SCALAR>::forceAfter(
      const ScheduleOperation<GUM_SCALAR> &op_to_force,
      const Set<const ScheduleOperation<GUM_SCALAR> *> &ops_before) {
    for (const auto op : ops_before)
      if (*op != op_to_force)
        forceAfter(op_to_force, *op);
  }

  /** @brief adds a constraint indicating that an operation must be performed
   * before another one */
  template <typename GUM_SCALAR>
  INLINE void Schedule<GUM_SCALAR>::forceBefore(NodeId op_to_force,
                                                NodeId op_after) {
    // first, add the constraint into the graph
    __dag.addArc(op_to_force, op_after);

    // if op_to_force was available, it is not anymore
    __operations_available.erase(op_after);
  }

  /** @brief adds a constraint indicating that an operation must be performed
   * before another one */
  template <typename GUM_SCALAR>
  INLINE void
  Schedule<GUM_SCALAR>::forceBefore(const ScheduleOperation<GUM_SCALAR> &op_to_force,
                                    const ScheduleOperation<GUM_SCALAR> &op_after) {
    forceBefore(__operation2node[op_to_force.id()], __operation2node[op_after.id()]);
  }

  /** @brief adds a constraint indicating that an operation must be performed
   * before a set of operations */
  template <typename GUM_SCALAR>
  void Schedule<GUM_SCALAR>::forceBefore(NodeId op_to_force,
                                         const NodeSet &ops_after) {
    for (const auto op : ops_after)
      if (op != op_to_force)
        forceBefore(op_to_force, op);
  }

  /** @brief adds a constraint indicating that an operation must be performed
   * before a set of operations */
  template <typename GUM_SCALAR>
  void Schedule<GUM_SCALAR>::forceBefore(
      const ScheduleOperation<GUM_SCALAR> &op_to_force,
      const Set<const ScheduleOperation<GUM_SCALAR> *> &ops_after) {
    for (typename Set<const ScheduleOperation<GUM_SCALAR> *>::const_iterator iter =
             ops_after.begin();
         iter != ops_after.end(); ++iter) {
      if (**iter != op_to_force) {
        forceBefore(op_to_force, **iter);
      }
    }
  }

  /// returns the set of operations involving a given multidim table
  template <typename GUM_SCALAR>
  INLINE const NodeSet &Schedule<GUM_SCALAR>::operationsInvolving(
      const ScheduleMultiDim<GUM_SCALAR> &table) const {
    return *(__multidim2operations[table.id()]);
  }

  /// returns the set of operations involving a given multidim table
  template <typename GUM_SCALAR>
  INLINE const NodeSet &
  Schedule<GUM_SCALAR>::operationsInvolving(MultiDimId table_id) const {
    return *(__multidim2operations[table_id]);
  }

  /// returns a DAG indicating in which order the operations can be performed
  template <typename GUM_SCALAR>
  INLINE const DAG &Schedule<GUM_SCALAR>::dag() const {
    // first update the set of parents of the nodes of the graph whose parents
    // were not set correctly
    __updateWrongParents();
    return __dag;
  }

  /// returns the scheduleOperation corresponding to an id in the DAG
  template <typename GUM_SCALAR>
  INLINE const ScheduleOperation<GUM_SCALAR> &
  Schedule<GUM_SCALAR>::operation(NodeId node_id) const {
    return *(__node2operation[node_id]);
  }

  /// returns the id associated to a given operation
  template <typename GUM_SCALAR>
  INLINE NodeId
      Schedule<GUM_SCALAR>::nodeId(const ScheduleOperation<GUM_SCALAR> &op) const {
    return __operation2node[op.id()];
  }

  /// returns the association between operations anf nodeIds
  template <typename GUM_SCALAR>
  INLINE const NodeProperty<const ScheduleOperation<GUM_SCALAR> *> &
  Schedule<GUM_SCALAR>::operations() const {
    return reinterpret_cast<NodeProperty<const ScheduleOperation<GUM_SCALAR> *> &>(
        __node2operation);
  }

  /// returns the set of ScheduleOperations that can be executed at once
  template <typename GUM_SCALAR>
  INLINE const NodeSet &Schedule<GUM_SCALAR>::availableOperations() const {
    // update the set of parents
    __updateWrongParents();
    return __operations_available;
  }

  /// executes a given operation
  template <typename GUM_SCALAR> void Schedule<GUM_SCALAR>::execute(NodeId id) {
    // update the parents of the sets of nodes which were not correct
    // !!! it is important to execute the following method before the execution
    // of operation id: this guarantees that the children of id with exactly
    // one parent (i.e., id) are now available to be processed
    __updateWrongParents();

    // before executing an operation, check that the operation is available

    if (__dag.parents(id).size() != 0) {
      GUM_ERROR(OperationNotAllowed, "the operation cannot be executed yet");
    }

    // actually execute the operation
    __node2operation[id]->execute();

    // now update the availability of the children of id: a child is available
    // if and only if it has only one parent
    const NodeSet &children = __dag.children(id);

    for (const auto child : children)
      if (__dag.parents(child).size() == 1)
        __operations_available.insert(child);

    // remove the operation's node and its adjacent arcs
    __dag.eraseChildren(id);

    __dag.eraseNode(id);

    delete __node2operation[id];

    __operation2node.erase(__node2operation[id]->id());

    __node2operation.erase(id);

    __operations_available.erase(id);
  }

  /// executes a given operation
  template <typename GUM_SCALAR>
  INLINE void
  Schedule<GUM_SCALAR>::execute(const ScheduleOperation<GUM_SCALAR> &op) {
    execute(__operation2node[op.id()]);
  }

  /** @bried returns an estimation of the number of elementary operations needed
   * to perform a given ScheduleOperation */
  template <typename GUM_SCALAR>
  INLINE float Schedule<GUM_SCALAR>::nbOperations(NodeId id) const {
    return __node2operation[id]->nbOperations();
  }

  /** @bried returns an estimation of the number of elementary operations needed
   * to perform a given ScheduleOperation */
  template <typename GUM_SCALAR>
  INLINE float
  Schedule<GUM_SCALAR>::nbOperations(ScheduleOperation<GUM_SCALAR> &op) const {
    return op.nbOperations();
  }

  /// returns the memory consumption used during the execution of an operation
  template <typename GUM_SCALAR>
  INLINE std::pair<long, long> Schedule<GUM_SCALAR>::memoryUsage(NodeId id) const {
    return __node2operation[id]->memoryUsage();
  }

  /// returns the memory consumption used during the execution of an operation
  template <typename GUM_SCALAR>
  INLINE std::pair<long, long>
  Schedule<GUM_SCALAR>::memoryUsage(ScheduleOperation<GUM_SCALAR> &op) const {
    return op.memoryUsage();
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
