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
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>

namespace gum {

  /// default constructor (construct an empty sequence)
  template < typename GUM_SCALAR >
  Schedule< GUM_SCALAR >::Schedule() {
    // for debugging purposes
    GUM_CONSTRUCTOR(Schedule);
  }

  /// copy constructor
  template < typename GUM_SCALAR >
  Schedule< GUM_SCALAR >::Schedule(const Schedule< GUM_SCALAR >& from) :
      dag__(from.dag__), operation2node__(from.operation2node__),
      created_multidims__(from.created_multidims__),
      operations_with_wrong_parents__(from.operations_with_wrong_parents__),
      operations_available__(from.operations_available__) {
    // for debugging purposes
    GUM_CONS_CPY(Schedule);

    // we must now copy the operations of "from" into "this"
    for (const auto& elt: from.node2operation__)
      node2operation__.insert(elt.first, elt.second->newFactory());

    // update the set of operations involved with each multidim table
    for (const auto& elt: from.multidim2operations__)
      multidim2operations__.insert(elt.first, new NodeSet(*elt.second));
  }

  /// destructor
  template < typename GUM_SCALAR >
  Schedule< GUM_SCALAR >::~Schedule() {
    // for debugging purposes
    GUM_DESTRUCTOR(Schedule);

    // remove all the operations that were stored

    for (const auto& elt: node2operation__)
      delete elt.second;

    // remove the sets of operations involved with each multidim table
    for (const auto& elt: multidim2operations__)
      delete elt.second;
  }

  /// copy operator
  template < typename GUM_SCALAR >
  Schedule< GUM_SCALAR >& Schedule< GUM_SCALAR >::operator=(const Schedule& from) {
    // avoid self assignment
    if (this != &from) {
      // remove all the operations that were stored
      for (const auto& elt: node2operation__)
        delete elt.second;

      // remove the sets of operations involved with each multidim table
      for (const auto& elt: multidim2operations__)
        delete elt.second;

      // fill all the data structures with the elements of from
      dag__ = from.dag__;
      operation2node__ = from.operation2node__;
      created_multidims__ = from.created_multidims__;
      operations_with_wrong_parents__ = from.operations_with_wrong_parents__;
      operations_available__ = from.operations_available__;

      node2operation__.clear();

      for (const auto& elt: from.node2operation__)
        node2operation__.insert(elt.first, elt.second->newFactory());

      // update the set of operations involved with each multidim table
      multidim2operations__.clear();

      for (const auto& elt: from.multidim2operations__)
        multidim2operations__.insert(elt.first, new NodeSet(*elt.second));
    }

    return *this;
  }

  /// inserts an operation to be scheduled
  template < typename GUM_SCALAR >
  NodeId
     Schedule< GUM_SCALAR >::insert(const ScheduleOperation< GUM_SCALAR >& op) {
    // create a copy of the operation
    ScheduleOperation< GUM_SCALAR >* operation = op.newFactory();

    // create a new node for the operation in the DAG
    NodeId node_id = dag__.addNode();

    // assign the operation to the new node
    node2operation__.insert(node_id, operation);
    operation2node__.insert(operation->id(), node_id);

    // get the list of multidims passed in arguments and determine which ones
    // are abstract. If there are some abstract multidims, then the node we
    // just created must have parents. Try to add these arcs and, if some
    // parents have not been created yet, indicate it in the
    // operations_with_wrong_parents__ list
    bool operation_available = true;

    for (const auto par: operation->multiDimArgs()) {
      if (par->isAbstract()) {
        // here we shall have a parent in the graph
        operation_available = false;
        MultiDimId multidim_id = par->id();

        if (created_multidims__.exists(multidim_id)) {
          dag__.addArc(created_multidims__[multidim_id], node_id);
        } else {
          operations_with_wrong_parents__.insert(node_id);
          break;
        }
      }
    }

    // if the operation is available to be processed, mark it as such
    if (operation_available) operations_available__.insert(node_id);

    // now we shall find whether, upon executing the operation, new multidim
    // tables are created
    NodeSet* involved_ops;

    for (const auto tab: operation->multiDimResults()) {
      MultiDimId table_id = tab->id();

      if (tab->isAbstract()) created_multidims__.insert(table_id, node_id);

      if (!multidim2operations__.exists(table_id)) {
        involved_ops = multidim2operations__.insert(table_id, new NodeSet).second;
      } else {
        involved_ops = multidim2operations__[table_id];
      }

      involved_ops->insert(node_id);
    }

    // update multidim2operations__ with the arguments passed to the newly
    // added operation
    for (const auto& par: operation->multiDimArgs()) {
      MultiDimId table_id = par->id();

      if (!multidim2operations__.exists(table_id)) {
        involved_ops = multidim2operations__.insert(table_id, new NodeSet).second;
      } else {
        involved_ops = multidim2operations__[table_id];
      }

      involved_ops->insert(node_id);
    }

    return node_id;
  }

  /// updates the set of parents for the nodes whoses parents are not correct
  /// yet
  template < typename GUM_SCALAR >
  void Schedule< GUM_SCALAR >::updateWrongParents__() const {
    // parse all the nodes whose parents sets are incorrect

    auto localWrongs =
       operations_with_wrong_parents__;   // complete copy of NodeSet

    for (const auto wrong: localWrongs) {
      // get the arguments passed to wrong and check that those that are
      // abstract
      // multidims belong to the schedule
      const Sequence< const ScheduleMultiDim< GUM_SCALAR >* >& args =
         node2operation__[wrong]->multiDimArgs();
      bool still_wrong = false;

      for (const auto arg: args) {
        if (arg->isAbstract() && !created_multidims__.exists(arg->id())) {
          still_wrong = true;
          break;
        }
      }

      // if the operation is not "still_wrong" anymore, then we should remove
      // it from operations_with_wrong_parents__ and update its parents set
      // appropriately. In addition, if there is no parent, then we should
      // indicate that the operation is now available
      if (!still_wrong) {
        Size nb_parents = 0;

        for (const auto arg: args)
          if (arg->isAbstract()) {
            dag__.addArc(created_multidims__[arg->id()], wrong);
            ++nb_parents;
          }

        // check that there is no parent
        if (!nb_parents) { operations_available__.insert(wrong); }

        operations_with_wrong_parents__.erase(wrong);
      }
    }
  }

  /** @brief adds a constraint indicating that an operation cannot be performed
   * before another one */
  template < typename GUM_SCALAR >
  INLINE void Schedule< GUM_SCALAR >::forceAfter(NodeId op_to_force,
                                                 NodeId op_before) {
    // first, add the constraint into the graph
    dag__.addArc(op_before, op_to_force);

    // if op_to_force was available, it is not anymore
    operations_available__.erase(op_to_force);
  }

  /** @brief adds a constraint indicating that an operation cannot be performed
   * before another one */
  template < typename GUM_SCALAR >
  INLINE void Schedule< GUM_SCALAR >::forceAfter(
     const ScheduleOperation< GUM_SCALAR >& op_to_force,
     const ScheduleOperation< GUM_SCALAR >& op_before) {
    forceAfter(operation2node__[op_to_force.id()],
               operation2node__[op_before.id()]);
  }

  /** @brief adds a constraint indicating that an operation cannot be performed
   * before a set of operations */
  template < typename GUM_SCALAR >
  void Schedule< GUM_SCALAR >::forceAfter(NodeId         op_to_force,
                                          const NodeSet& ops_before) {
    for (const auto op: ops_before)
      if (op != op_to_force) forceAfter(op_to_force, op);
  }

  /** @brief adds a constraint indicating that an operation cannot be performed
   * before a set of operations */
  template < typename GUM_SCALAR >
  void Schedule< GUM_SCALAR >::forceAfter(
     const ScheduleOperation< GUM_SCALAR >&               op_to_force,
     const Set< const ScheduleOperation< GUM_SCALAR >* >& ops_before) {
    for (const auto op: ops_before)
      if (*op != op_to_force) forceAfter(op_to_force, *op);
  }

  /** @brief adds a constraint indicating that an operation must be performed
   * before another one */
  template < typename GUM_SCALAR >
  INLINE void Schedule< GUM_SCALAR >::forceBefore(NodeId op_to_force,
                                                  NodeId op_after) {
    // first, add the constraint into the graph
    dag__.addArc(op_to_force, op_after);

    // if op_to_force was available, it is not anymore
    operations_available__.erase(op_after);
  }

  /** @brief adds a constraint indicating that an operation must be performed
   * before another one */
  template < typename GUM_SCALAR >
  INLINE void Schedule< GUM_SCALAR >::forceBefore(
     const ScheduleOperation< GUM_SCALAR >& op_to_force,
     const ScheduleOperation< GUM_SCALAR >& op_after) {
    forceBefore(operation2node__[op_to_force.id()],
                operation2node__[op_after.id()]);
  }

  /** @brief adds a constraint indicating that an operation must be performed
   * before a set of operations */
  template < typename GUM_SCALAR >
  void Schedule< GUM_SCALAR >::forceBefore(NodeId         op_to_force,
                                           const NodeSet& ops_after) {
    for (const auto op: ops_after)
      if (op != op_to_force) forceBefore(op_to_force, op);
  }

  /** @brief adds a constraint indicating that an operation must be performed
   * before a set of operations */
  template < typename GUM_SCALAR >
  void Schedule< GUM_SCALAR >::forceBefore(
     const ScheduleOperation< GUM_SCALAR >&               op_to_force,
     const Set< const ScheduleOperation< GUM_SCALAR >* >& ops_after) {
    for (typename Set< const ScheduleOperation< GUM_SCALAR >* >::const_iterator
            iter = ops_after.begin();
         iter != ops_after.end();
         ++iter) {
      if (**iter != op_to_force) { forceBefore(op_to_force, **iter); }
    }
  }

  /// returns the set of operations involving a given multidim table
  template < typename GUM_SCALAR >
  INLINE const NodeSet& Schedule< GUM_SCALAR >::operationsInvolving(
     const ScheduleMultiDim< GUM_SCALAR >& table) const {
    return *(multidim2operations__[table.id()]);
  }

  /// returns the set of operations involving a given multidim table
  template < typename GUM_SCALAR >
  INLINE const NodeSet&
               Schedule< GUM_SCALAR >::operationsInvolving(MultiDimId table_id) const {
    return *(multidim2operations__[table_id]);
  }

  /// returns a DAG indicating in which order the operations can be performed
  template < typename GUM_SCALAR >
  INLINE const DAG& Schedule< GUM_SCALAR >::scheduling_dag() const {
    // first update the set of parents of the nodes of the graph whose parents
    // were not set correctly
    updateWrongParents__();
    return dag__;
  }

  /// returns the scheduleOperation corresponding to an id in the DAG
  template < typename GUM_SCALAR >
  INLINE const ScheduleOperation< GUM_SCALAR >&
               Schedule< GUM_SCALAR >::operation(NodeId node_id) const {
    return *(node2operation__[node_id]);
  }

  /// returns the id associated to a given operation
  template < typename GUM_SCALAR >
  INLINE NodeId Schedule< GUM_SCALAR >::nodeId(
     const ScheduleOperation< GUM_SCALAR >& op) const {
    return operation2node__[op.id()];
  }

  /// returns the association between operations anf nodeIds
  template < typename GUM_SCALAR >
  INLINE const NodeProperty< const ScheduleOperation< GUM_SCALAR >* >&
               Schedule< GUM_SCALAR >::operations() const {
    return reinterpret_cast<
       NodeProperty< const ScheduleOperation< GUM_SCALAR >* >& >(node2operation__);
  }

  /// returns the set of ScheduleOperations that can be executed at once
  template < typename GUM_SCALAR >
  INLINE const NodeSet& Schedule< GUM_SCALAR >::availableOperations() const {
    // update the set of parents
    updateWrongParents__();
    return operations_available__;
  }

  /// executes a given operation
  template < typename GUM_SCALAR >
  void Schedule< GUM_SCALAR >::execute(NodeId id) {
    // update the parents of the sets of nodes which were not correct
    // !!! it is important to execute the following method before the execution
    // of operation id: this guarantees that the children of id with exactly
    // one parent (i.e., id) are now available to be processed
    updateWrongParents__();

    // before executing an operation, check that the operation is available

    if (dag__.parents(id).size() != 0) {
      GUM_ERROR(OperationNotAllowed, "the operation cannot be executed yet");
    }

    // actually execute the operation
    node2operation__[id]->execute();

    // now update the availability of the children of id: a child is available
    // if and only if it has only one parent
    const NodeSet& children = dag__.children(id);

    for (const auto child: children)
      if (dag__.parents(child).size() == 1) operations_available__.insert(child);

    // remove the operation's node and its adjacent arcs
    dag__.eraseChildren(id);

    dag__.eraseNode(id);

    delete node2operation__[id];

    operation2node__.erase(node2operation__[id]->id());

    node2operation__.erase(id);

    operations_available__.erase(id);
  }

  /// executes a given operation
  template < typename GUM_SCALAR >
  INLINE void
     Schedule< GUM_SCALAR >::execute(const ScheduleOperation< GUM_SCALAR >& op) {
    execute(operation2node__[op.id()]);
  }

  /** @bried returns an estimation of the number of elementary operations needed
   * to perform a given ScheduleOperation */
  template < typename GUM_SCALAR >
  INLINE float Schedule< GUM_SCALAR >::nbOperations(NodeId id) const {
    return node2operation__[id]->nbOperations();
  }

  /** @bried returns an estimation of the number of elementary operations needed
   * to perform a given ScheduleOperation */
  template < typename GUM_SCALAR >
  INLINE float Schedule< GUM_SCALAR >::nbOperations(
     ScheduleOperation< GUM_SCALAR >& op) const {
    return op.nbOperations();
  }

  /// returns the memory consumption used during the execution of an operation
  template < typename GUM_SCALAR >
  INLINE std::pair< long, long >
         Schedule< GUM_SCALAR >::memoryUsage(NodeId id) const {
    return node2operation__[id]->memoryUsage();
  }

  /// returns the memory consumption used during the execution of an operation
  template < typename GUM_SCALAR >
  INLINE std::pair< long, long > Schedule< GUM_SCALAR >::memoryUsage(
     ScheduleOperation< GUM_SCALAR >& op) const {
    return op.memoryUsage();
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
