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

#include <agrum/tools/graphicalModels/inference/scheduler/schedulerParallel.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/tools/graphicalModels/inference/scheduler/schedulerParallel_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  /// default constructor
  SchedulerParallel::SchedulerParallel(Size max_nb_threads, double max_megabyte_memory) :
      Scheduler(max_nb_threads, max_megabyte_memory),
      _sequential_scheduler_(max_nb_threads, max_megabyte_memory) {
    // for debugging purposes
    GUM_CONSTRUCTOR(SchedulerParallel);
  }


  /// copy constructor
  SchedulerParallel::SchedulerParallel(const SchedulerParallel& from) :
      Scheduler(from), _sequential_scheduler_(from._sequential_scheduler_) {
    // for debugging purposes
    GUM_CONS_CPY(SchedulerParallel);
  }


  /// move constructor
  SchedulerParallel::SchedulerParallel(SchedulerParallel&& from) :
      Scheduler(std::move(from)), _sequential_scheduler_(std::move(from._sequential_scheduler_)) {
    // for debugging purposes
    GUM_CONS_MOV(SchedulerParallel);
  }


  /// virtual copy constructor
  SchedulerParallel* SchedulerParallel::clone() const { return new SchedulerParallel(*this); }


  /// destructor
  SchedulerParallel::~SchedulerParallel() {
    // for debugging purposes
    GUM_DESTRUCTOR(SchedulerParallel);
  }


  /// execute all the operations of a given schedule
  void SchedulerParallel::execute(Schedule& schedule) {
    // check that, a priori, we have sufficient memory to execute everything.
    // If this is not the case, then we should raise an exception before even
    // trying to execute any operation of the schedule
    if (this->_max_memory != 0.0) {
      SchedulerSequential seq_scheduler(this->maxNbThreads(), this->maxMemory());
      auto                memory_usage = seq_scheduler.memoryUsage(schedule);
      if (memory_usage.first > this->_max_memory) { throw std::bad_alloc(); }
    }

    // compute the set of operations to perform
    List< NodeId > available_nodes;
    const DAG&     dag = schedule.dag();
    NodeSet        nodes_to_execute(dag.sizeNodes());
    {
      for (const auto node: schedule.availableOperations()) {
        available_nodes.insert(node);
      }

      List< NodeId > nodes = available_nodes;
      while (!nodes.empty()) {
        const NodeId node = nodes.front();
        nodes.popFront();
        if (!nodes_to_execute.exists(node)) {
          nodes_to_execute.insert(node);
          for (const auto child: dag.children(node))
            nodes.insert(child);
        }
      }
    }
    if (nodes_to_execute.empty()) return;

    // indicate for each node, the number of parents remaining to execute
    std::atomic< Size >                 nb_remaining_operations(nodes_to_execute.size());
    NodeProperty< std::atomic< Size > > nb_parents_to_execute(nb_remaining_operations);
    for (const auto node: nodes_to_execute) {
      Size nb_unexecuted_parents = dag.parents(node).size();
      for (const auto parent: dag.parents(node))
        if (schedule.operation(parent).isExecuted()) --nb_unexecuted_parents;
      nb_parents_to_execute.emplace(node, nb_unexecuted_parents);
    }
    nodes_to_execute.clear();

    // compute the number of threads to execute
    const Size nb_threads = nb_remaining_operations.load() < this->maxNbThreads()
                             ? nb_remaining_operations.load()
                             : this->maxNbThreads();

    // indicate which threads are active
    std::vector< std::atomic< bool > > active_threads(nb_remaining_operations.load());
    for (auto& thread: active_threads)
      thread = false;

    // assign the available nodes to the threads
    std::vector< std::atomic< NodeId > > thread2node(nb_remaining_operations.load());
    for (auto& node: thread2node)
      node = 0;
    {
      Idx thread_index = Idx(0);
      while (!available_nodes.empty()) {
        const NodeId node = available_nodes.front();
        available_nodes.popFront();
        thread2node[thread_index]    = node;
        active_threads[thread_index] = true;
        ++thread_index;
        if (thread_index >= nb_threads) break;
      }
    }

    // prepare keeping information about memory usage. This is useful if the user
    // added constraints on memory usage. When operations cannot be performed
    // due to memory shortage, they will be temporarily stored into ordered vectors
    double overall_memory_used = 0.0;   // the current memory used by all the threads

    // create the mutexes needed for threads synchronization
    std::vector< std::mutex >              thread2mutex(nb_threads);
    std::vector< std::condition_variable > thread2not_empty(nb_threads);
    std::mutex                             overall_mutex;

    // here, we create a lambda that will be executed by all the threads
    // to execute the operations in a parallel manner
    auto opExecute = [this,
                      &schedule,
                      &nb_parents_to_execute,
                      &overall_mutex,
                      &thread2mutex,
                      &thread2not_empty,
                      &active_threads,
                      &thread2node,
                      &available_nodes,
                      &nb_remaining_operations,
                      &overall_memory_used](const std::size_t this_thread,
                                            const std::size_t nb_threads) -> void {
      const DAG& dag = schedule.dag();

      // get the synchronization objects
      auto& this_mutex         = thread2mutex[this_thread];
      auto& this_not_empty     = thread2not_empty[this_thread];
      auto& this_active_thread = active_threads[this_thread];

      // get the operation to execute
      auto& node_to_execute = thread2node[this_thread];
      Size  nb_remaining;

      // sets the condition to wait for new nodes
      const auto duration = std::chrono::milliseconds(10);
      auto has_node_to_process =
         [&node_to_execute, &nb_remaining_operations, &nb_remaining, this_thread] {
           nb_remaining = nb_remaining_operations.load();
           return (node_to_execute != NodeId(0)) || (nb_remaining == Size(0));
         };

      while (true) {
        {   // use brace for unique_lock's scope

          // wait until some operation is available or all the operations have
          // been executed
          std::unique_lock< std::mutex > lock(this_mutex);
          do {}
          while(!this_not_empty.wait_for(lock, duration, has_node_to_process));

          // if all the operations have been executed, stop the thread
          if (nb_remaining == Size(0)) {
            thread2not_empty[(this_thread + 1) % nb_threads].notify_one();
            return;
          }

          this_active_thread = true;
        }

        // now, actually execute the operation
        const_cast< ScheduleOperation& >(schedule.operation(node_to_execute)).execute();
        nb_remaining_operations.fetch_sub(1);

        // compute the next operations available to be executed
        std::vector< NodeId > new_available_nodes;
        new_available_nodes.reserve(nb_remaining);
        for (const auto child_node: dag.children(node_to_execute)) {
          // indicate that node has been executed and get the set of nodes
          // without parents yet to execute. Note that nb_parents_to_execute[child_node]
          // is an atomic<Size>. Hence, the --operation is atomic and only one thread
          // can pass the if statement below
          if (--nb_parents_to_execute[child_node] == Size(0)) {
            new_available_nodes.push_back(child_node);
          }
        }

        // if there are new nodes to process. Assign one node to the current thread
        bool this_thread_is_inactive = false;
        if (!new_available_nodes.empty()) {
          // no need to lock with a mutex here since, the thread being still active,
          // no other thread can assign it another node
          node_to_execute = new_available_nodes.back();
          new_available_nodes.pop_back();
        }
        else {
          // here, no new node was assigned, so the thread becomes inactive
          this_thread_is_inactive = true;
        }

        // There may still exist nodes to process. We will try to assign them to the
        // set of inactive threads. So, get a list of threads that seem inactive. As we
        // do not guard the reads of these inactive_threads, some threads might seem to
        // be currently inactive but can become active before we try to assign them some
        // operations to perform
        std::vector< Idx > inactive_threads;
        inactive_threads.reserve(nb_threads);
        for (Idx i = 0; i < nb_threads; ++i) {
          if (!active_threads[i]) inactive_threads.push_back(i);
        }

        // If we did not assign a new node to the current thread, consider it now
        // as inactive. Below, we may be able to assign it a node that was stored
        // into the overall set of available nodes
        if (this_thread_is_inactive) {
          inactive_threads.push_back(this_thread);
          this_mutex.lock();
          this_active_thread = false;
          node_to_execute = 0;
          this_mutex.unlock();
        }

        // if no inactive thread was identified, we store the newly available nodes
        // into the overall set of available nodes and we go back to the waiting step
        if (inactive_threads.empty()) {
          if (!new_available_nodes.empty()) {
            // lock the next shared memory operations
            std::lock_guard< std::mutex > overall_lock(overall_mutex);
            for (const auto node: new_available_nodes)
              available_nodes.insert(node);
          }

          // go to the waiting step
          continue;
        }

        // here we know that there are some threads that are still inactive.

        // try to assign the nodes in new_available_nodes to these inactive threads.
        // Here, we guard what we do with mutexes in order to get really inactive
        // threads performing operations that are guaranteed to be different.
        bool no_more_inactive_threads = false;
        {
          Idx i = inactive_threads.size() - 1;
          Idx thread;
          while (!new_available_nodes.empty()) {
            while (!no_more_inactive_threads) {
              thread = inactive_threads[i];
              {   // use brace for lock_guard's scope
                std::lock_guard< std::mutex > lock(thread2mutex[thread]);
                if (!active_threads[thread]) {
                  active_threads[thread] = true;
                  thread2node[thread]    = new_available_nodes.back();
                  new_available_nodes.pop_back();
                  thread2not_empty[thread].notify_one();

                  if (i != Idx(0))
                    --i;
                  else
                    no_more_inactive_threads = true;
                  break;
                }
              }
              if (i != Idx(0))
                --i;
              else
                no_more_inactive_threads = true;
            }

            if (no_more_inactive_threads) break;
          }

          // do not take anymore into account the threads we have assigned
          // operations to
          if (no_more_inactive_threads) {
            inactive_threads.clear();
          }
          else {
            inactive_threads.resize(i + 1);
          }
        }

        // add the remaining elements of new_available_nodes to available_nodes and
        // try to assign the nodes in available_nodes to the remaining inactive threads.
        // Here, we guard what we do with mutexes in order to get really inactive
        // threads performing operations that are guaranteed to be different.
        {
          // lock the next shared memory operations
          std::lock_guard< std::mutex > overall_lock(overall_mutex);

          for (const auto node: new_available_nodes)
            available_nodes.insert(node);

          Idx i = inactive_threads.size() - 1;
          Idx thread;
          while (!available_nodes.empty()) {
            while (!no_more_inactive_threads) {
              thread = inactive_threads[i];
              {
                std::lock_guard< std::mutex > lock(thread2mutex[thread]);
                if (!active_threads[thread]) {
                  active_threads[thread] = true;
                  thread2node[thread]    = available_nodes.front();
                  available_nodes.popFront();
                  thread2not_empty[thread].notify_one();
                  if (i != Idx(0))
                    --i;
                  else
                    no_more_inactive_threads = true;
                  break;
                }
              }

              if (i != Idx(0))
                --i;
              else
                no_more_inactive_threads = true;

            }
            if (no_more_inactive_threads) break;
          }
        }
      }
    };

    // launch the threads
    ThreadExecutor::execute(nb_threads, opExecute);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
