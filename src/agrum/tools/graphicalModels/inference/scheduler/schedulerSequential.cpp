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

/**
 * @file
 * @brief Class to include at least once this header
 *
 * @author Christophe GONZALES(@AMU) & Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <agrum/tools/graphicalModels/inference/scheduler/schedulerSequential.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/tools/graphicalModels/inference/scheduler/schedulerSequential_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  /// default constructor
  SchedulerSequential::SchedulerSequential(Size max_nb_threads, double max_megabyte_memory) :
      Scheduler(max_nb_threads, max_megabyte_memory) {
    // for debugging purposes
    GUM_CONSTRUCTOR(SchedulerSequential);
  }


  /// copy constructor
  SchedulerSequential::SchedulerSequential(const SchedulerSequential& from) : Scheduler(from) {
    // for debugging purposes
    GUM_CONS_CPY(SchedulerSequential);
  }


  /// move constructor
  SchedulerSequential::SchedulerSequential(SchedulerSequential&& from) :
      Scheduler(std::move(from)) {
    // for debugging purposes
    GUM_CONS_MOV(SchedulerSequential);
  }


  /// virtual copy constructor
  SchedulerSequential* SchedulerSequential::clone() const { return new SchedulerSequential(*this); }


  /// destructor
  SchedulerSequential::~SchedulerSequential() {
    // for debugging purposes
    GUM_DESTRUCTOR(SchedulerSequential);
  }


  /// execute all the operations of a given schedule
  void SchedulerSequential::execute(Schedule& schedule) {
    // if we do not have the right set of operations stored into operations,
    // then recompute it
    _setSchedule_(schedule);

    // check that, a priori, we have sufficient memory to execute everything.
    // If this is not the case, then we should raise an exception before even
    // trying to execute any operation of the schedule
    if ((this->_max_memory != 0.0) && _memory_usage_.first > this->_max_memory) {
      throw std::bad_alloc();
    }

    // execute the schedule
    for (const auto node: _operations_) {
      // try to execute the operation. If this raises an exception, then
      // we do not try to catch it
      auto& op = const_cast< ScheduleOperation& >(schedule.operation(node));
      op.execute();

      // if we succeeded to execute the operation, indicate that the set of all
      // the operations of the schedule is no more up to date
      _operations_up_to_date_ = false;
    }
  }


  /** @bried returns an estimation of the number of elementary operations needed
   * to perform a given schedule */
  double SchedulerSequential::nbOperations(const Schedule& schedule) {
    double nb_ops = 0.0;

    for (const auto node: schedule.dag()) {
      nb_ops += schedule.operation(node).nbOperations();
    }

    return nb_ops;
  }


  /// the comparison function used to sort unexecutable operations
  bool SchedulerSequential::_cmp_(const UnexecutedOperation& a, const UnexecutedOperation& b) {
    if (a.max_memory_usage < b.max_memory_usage) return true;
    if (b.max_memory_usage < a.max_memory_usage) return false;
    return (a.end_memory_usage < b.end_memory_usage);
  }


  /// returns the memory consumption used during the execution of a schedule
  std::pair< double, double > SchedulerSequential::memoryUsage(const Schedule& schedule) {
    // if we do not have the right memory usage stored into _memory_usage_,
    // then recompute it
    _setSchedule_(schedule);

    return _memory_usage_;
  }


  /// add the currently executable operations into the set of available operations
  Size SchedulerSequential::_addExecutableOps_(
     std::vector< UnexecutedOperation >& unexecuted_deletions,
     std::vector< UnexecutedOperation >& unexecuted_operations,
     bool&                               unexecuted_deletions_sorted,
     bool&                               unexecuted_operations_sorted,
     double                              memory_used,
     double                              max_memory,
     List< NodeId >&                     available_nodes) const {
    double added_mem = 0.0;

    // sort the vectors by increasing memory usage
    if (!unexecuted_deletions_sorted) {
      std::sort(unexecuted_deletions.begin(), unexecuted_deletions.end(), _cmp_);
      unexecuted_deletions_sorted = true;
    }
    if (!unexecuted_operations_sorted) {
      std::sort(unexecuted_operations.begin(), unexecuted_operations.end(), _cmp_);
      unexecuted_operations_sorted = true;
    }


    // get the index of the last executable deletion
    std::size_t i_del = 0;
    for (std::size_t end = unexecuted_deletions.size(); i_del < end; ++i_del) {
      if (memory_used + added_mem + unexecuted_deletions[i_del].max_memory_usage > max_memory)
        break;
      added_mem += unexecuted_deletions[i_del].end_memory_usage;
    }

    // get the index of the last executable operation (that does not imply
    // any deletion
    std::size_t i_op = 0;
    for (std::size_t end = unexecuted_operations.size(); i_op < end; ++i_op) {
      if (memory_used + added_mem + unexecuted_operations[i_op].max_memory_usage > max_memory)
        break;
      added_mem += unexecuted_operations[i_op].end_memory_usage;
    }


    // insert the executable operations (not deletions) into available_nodes
    if (i_op != 0) {
      for (std::size_t j = i_op - 1, i = i_op; i > 0; --j, --i)
        available_nodes.pushFront(unexecuted_operations[j].node);

      unexecuted_operations.erase(unexecuted_operations.begin(),
                                  unexecuted_operations.begin() + i_op);
    }

    // insert the executable deletions into available_nodes
    if (i_del != 0) {
      for (std::size_t j = i_del - 1, i = 0; i > 0; --j, --i)
        available_nodes.pushFront(unexecuted_deletions[j].node);

      unexecuted_deletions.erase(unexecuted_deletions.begin(),
                                 unexecuted_deletions.begin() + i_del);
    }

    return i_op + i_del;
  }


  /// simulate the update of the schedule's DAG resulting from the execution
  /// of an operation
  void SchedulerSequential::_simulateDAGUpdate_(DAG&                   dag,
                                                const NodeId           node,
                                                std::vector< NodeId >& new_available_nodes) const {
    new_available_nodes.clear();

    for (const auto child_node: dag.children(node)) {
      if (dag.parents(child_node).size() == 1) {   // only node is a parent
        new_available_nodes.push_back(child_node);
      }
    }

    // remove node_exec
    dag.eraseNode(node);
  }


  /// execute one operation
  void SchedulerSequential::_simulateExecuteOneOperation_(
     const NodeId           node,
     ScheduleOperation&     op,
     DAG&                   dag,
     List< NodeId >&        available_nodes,
     std::vector< NodeId >& new_available_nodes) {
    // save the fact that operation node/op is to be executed
    _operations_.push_back(node);

    // whenever an operation has been executed, update the schedule
    _simulateDAGUpdate_(dag, node, new_available_nodes);

    // add the new available_nodes nodes to List available_nodes: make it so
    // that the operations that imply deletions are first (hence, they should
    // decrease the stress on memory usage)
    for (const auto new_node: new_available_nodes) {
      if (!_schedule_->operation(new_node).implyDeletion()) available_nodes.pushFront(new_node);
    }
    for (const auto new_node: new_available_nodes) {
      if (_schedule_->operation(new_node).implyDeletion()) available_nodes.pushFront(new_node);
    }
  }


  /// returns the memory consumption used during the execution of a schedule
  void SchedulerSequential::_simulateExecution_() {
    // get the DAG of the operations to perform
    DAG dag = _schedule_->dag();

    _operations_.clear();
    _operations_.reserve(dag.sizeNodes());

    // the list of available_nodes nodes to perform
    List< NodeId >        available_nodes;
    std::vector< NodeId > new_available_nodes;   // nodes available after an execution

    // fill available_nodes by inserting at the front the deletions and at the
    // back the other nodes. This will enable to free memory as soon as possible
    for (const auto node: _schedule_->availableOperations()) {
      if (_schedule_->operation(node).implyDeletion())
        available_nodes.pushFront(node);
      else
        available_nodes.pushBack(node);
    }

    // if there is nothing to do, don't do it
    if (available_nodes.empty()) {
      _memory_usage_          = {0.0, 0.0};
      _operations_up_to_date_ = true;
      return;
    }


    // prepare keeping information about memory usage. This is useful if the user
    // added constraints on memory usage. When operations cannot be performed
    // due to memory shortage, they will be temporarily stored into ordered vectors
    double                             memory_used     = 0;   // the current memory used
    double                             max_memory_used = 0;   // max memory used so far
    double                             max_memory      = this->_max_memory;   // max memory allowed
    std::vector< UnexecutedOperation > unexecuted_deletions;
    std::vector< UnexecutedOperation > unexecuted_operations;
    bool                               unexecuted_deletions_sorted  = true;
    bool                               unexecuted_operations_sorted = true;

    // perform the schedule
    bool schedule_fully_performed = false;
    do {
      while (!available_nodes.empty()) {
        // get the first operation available_nodes
        const NodeId node = available_nodes.front();
        available_nodes.popFront();
        auto& op = const_cast< ScheduleOperation& >(_schedule_->operation(node));

        // if scheduling under memory budget, check that we do not consume too
        // much memory.
        std::pair< double, double > op_mem = op.memoryUsage();
        if (max_memory != 0.0) {
          if (memory_used + op_mem.first > max_memory) {
            // here, we cannot execute the operation due to memory shortage
            // so we should put the node to the unexecuted operations vectors.
            // Hopefully, we will be able to execute them later on.
            if (op_mem.second > 0) {
              unexecuted_operations.push_back({op_mem.first, op_mem.second, node});
              unexecuted_operations_sorted = false;
            } else {
              unexecuted_deletions.push_back({op_mem.first, op_mem.second, node});
              unexecuted_deletions_sorted = false;
            }

            continue;
          }
        }

        // here, we remain under the memory limit, if any, so we should try to
        // execute the operation
        _simulateExecuteOneOperation_(node, op, dag, available_nodes, new_available_nodes);
        if (memory_used + op_mem.first > max_memory_used)
          max_memory_used = memory_used + op_mem.first;
        memory_used += op_mem.second;
      }

      // if scheduling under memory limitations, try to see if we can execute
      // some operations that were previously unexecutable
      if (!unexecuted_operations.empty() || !unexecuted_deletions.empty()) {
        const Size nb_new_ops = const_cast< SchedulerSequential* >(this)->_addExecutableOps_(
           unexecuted_deletions,
           unexecuted_operations,
           unexecuted_deletions_sorted,
           unexecuted_operations_sorted,
           memory_used,
           max_memory,
           available_nodes);

        if (nb_new_ops == 0) {
          // here, there exists no more operation that can be executed given the
          // current memory constraints. But there may remain some unexecuted
          // operations. In this case, try to increment the memory budget.
          if (!unexecuted_deletions.empty())
            max_memory = memory_used + unexecuted_deletions[0].max_memory_usage;
          else if (!unexecuted_operations.empty())
            max_memory = memory_used + unexecuted_operations[0].max_memory_usage;

          const_cast< SchedulerSequential* >(this)->_addExecutableOps_(unexecuted_deletions,
                                                                       unexecuted_operations,
                                                                       unexecuted_deletions_sorted,
                                                                       unexecuted_operations_sorted,
                                                                       memory_used,
                                                                       max_memory,
                                                                       available_nodes);
        }
      } else {
        schedule_fully_performed = true;
      }
    } while (!schedule_fully_performed);

    _memory_usage_          = {max_memory_used, memory_used};
    _operations_up_to_date_ = true;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
