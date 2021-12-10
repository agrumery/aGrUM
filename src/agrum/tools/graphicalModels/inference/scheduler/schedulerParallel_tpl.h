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
 * @brief a parallel scheduler that executes available operations
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/tools/graphicalModels/inference/scheduler/schedulerParallel.h>

namespace gum {

  /// default constructor
  template < template < typename > class ALLOC >
  SchedulerParallel< ALLOC >::SchedulerParallel(
     Size                                                       max_nb_threads,
     double                                                     max_megabyte_memory,
     const typename SchedulerParallel< ALLOC >::allocator_type& alloc) :
      Scheduler< ALLOC >(max_nb_threads, max_megabyte_memory, alloc),
      _sequential_scheduler_(max_nb_threads, max_megabyte_memory, alloc) {
    // for debugging purposes
    GUM_CONSTRUCTOR(SchedulerParallel);
  }


  /// copy constructor with a given allocator
  template < template < typename > class ALLOC >
  SchedulerParallel< ALLOC >::SchedulerParallel(
     const SchedulerParallel< ALLOC >&                          from,
     const typename SchedulerParallel< ALLOC >::allocator_type& alloc) :
      Scheduler< ALLOC >(from, alloc),
      _sequential_scheduler_(from._sequential_scheduler_, alloc) {
    // for debugging purposes
    GUM_CONS_CPY(SchedulerParallel);
  }


  /// copy constructor
  template < template < typename > class ALLOC >
  SchedulerParallel< ALLOC >::SchedulerParallel(const SchedulerParallel< ALLOC >& from) :
      SchedulerParallel< ALLOC >(from, from.get_allocator()) {
    // for debugging purposes
    GUM_CONS_CPY(SchedulerParallel);
  }


  /// move constructor with a given allocator
  template < template < typename > class ALLOC >
  SchedulerParallel< ALLOC >::SchedulerParallel(
     SchedulerParallel< ALLOC >&&                               from,
     const typename SchedulerParallel< ALLOC >::allocator_type& alloc) :
      Scheduler< ALLOC >(std::move(from), alloc),
      _sequential_scheduler_(std::move(from._sequential_scheduler_, alloc)) {
    // for debugging purposes
    GUM_CONS_MOV(SchedulerParallel);
  }


  /// move constructor
  template < template < typename > class ALLOC >
  SchedulerParallel< ALLOC >::SchedulerParallel(SchedulerParallel< ALLOC >&& from) :
      SchedulerParallel< ALLOC >(std::move(from), from.get_allocator()) {
    // for debugging purposes
    GUM_CONS_MOV(SchedulerParallel);
  }


  /// virtual copy constructor with a given allocator
  template < template < typename > class ALLOC >
  SchedulerParallel< ALLOC >* SchedulerParallel< ALLOC >::clone(
     const typename SchedulerParallel< ALLOC >::allocator_type& alloc) const {
    ALLOC< SchedulerParallel< ALLOC > > allocator(alloc);
    SchedulerParallel< ALLOC >*         new_sched = allocator.allocate(1);
    try {
      new ((void*)new_sched) SchedulerParallel< ALLOC >(*this, alloc);
    } catch (...) {
      allocator.deallocate(new_sched, 1);
      throw;
    }

    return new_sched;
  }


  /// virtual copy constructor with a given allocator
  template < template < typename > class ALLOC >
  INLINE SchedulerParallel< ALLOC >* SchedulerParallel< ALLOC >::clone() const {
    return clone(this->get_allocator());
  }


  /// destructor
  template < template < typename > class ALLOC >
  SchedulerParallel< ALLOC >::~SchedulerParallel() {
    // for debugging purposes
    GUM_DESTRUCTOR(SchedulerParallel);
  }


  /// sets an upper bound on the memory consumption admissible
  template < template < typename > class ALLOC >
  INLINE void SchedulerParallel< ALLOC >::setMaxMemory(double megabytes) {
    Scheduler< ALLOC >::setMaxMemory(megabytes);
    _sequential_scheduler_.setMaxMemory(megabytes);
  }


  /// sets the number max of threads that can be used
  template < template < typename > class ALLOC >
  INLINE void SchedulerParallel< ALLOC >::setMaxNbThreads(Size nb) {
    if (nb == Size(0)) nb = gum::getMaxNumberOfThreads();

    Scheduler< ALLOC >::setMaxNbThreads(nb);
    _sequential_scheduler_.setMaxNbThreads(nb);
  }


  /** @bried returns an estimation of the number of elementary operations needed
   * to perform a given schedule */
  template < template < typename > class ALLOC >
  INLINE double SchedulerParallel< ALLOC >::nbOperations(const Schedule< ALLOC >& schedule) {
    return _sequential_scheduler_.nbOperations(schedule);
  }


  /// returns the memory consumption used during the execution of a schedule
  template < template < typename > class ALLOC >
  INLINE std::pair< double, double >
         SchedulerParallel< ALLOC >::memoryUsage(const Schedule< ALLOC >& schedule) {
    return _sequential_scheduler_.memoryUsage(schedule);
  }


  /// execute all the operations of a given schedule
  template < template < typename > class ALLOC >
  void SchedulerParallel< ALLOC >::execute(Schedule< ALLOC >& schedule) {
    // check that, a priori, we have sufficient memory to execute everything.
    // If this is not the case, then we should raise an exception before even
    // trying to execute any operation of the schedule
    if (this->_max_memory != 0.0) {
      SchedulerSequential< ALLOC > seq_scheduler(this->maxNbThreads(), this->maxMemory());
      auto                         memory_usage = seq_scheduler.memoryUsage(schedule);
      if (memory_usage.first > this->_max_memory) {
        throw std::bad_alloc();
      }
    }

    // compute the set of operations to perform
    List< NodeId> available_nodes;
    const DAG& dag = schedule.dag();
    NodeSet nodes_to_execute (dag.sizeNodes());
    {
      for (const auto node: schedule.availableOperations()) {
        available_nodes.insert(node);
      }

      List< NodeId > nodes = available_nodes;
      while (!nodes.empty()) {
        const NodeId node = nodes.front();
        nodes.popFront();
        nodes_to_execute.insert(node);
        for (const auto child: dag.children(node))
          nodes.insert(child);
      }
    }
    if (nodes_to_execute.empty()) return;

    std::cout << "nb operations to execute = " << nodes_to_execute.size() << std::endl;

    // indicate for each node, the number of parents remaining to execute
    std::atomic< Size > nb_remaining_operations(nodes_to_execute.size());
    NodeProperty< std::atomic< Size > > nb_parents_to_execute(nb_remaining_operations);
    for (const auto node: nodes_to_execute) {
      Size nb_unexecuted_parents = dag.parents(node).size();
      for (const auto parent: dag.parents(node))
        if (schedule.operation(parent).isExecuted())
          --nb_unexecuted_parents;
      nb_parents_to_execute.emplace(node, nb_unexecuted_parents);
    }

    // compute the number of threads to execute
    const Size nb_threads = nb_remaining_operations.load() < this->maxNbThreads()
                             ? nb_remaining_operations.load()
                             : this->maxNbThreads();
    std::cout << "nb threads = " << nb_threads << std::endl;

    // indicate which threads are active
    std::vector< std::atomic< bool > > active_threads(nb_remaining_operations.load());
    for (auto& thread: active_threads) thread = false;

    // assign the available nodes to the threads
    std::vector< NodeId > thread2node(nb_remaining_operations.load(), NodeId(0));
    {
      Idx thread_index = Idx(0);
      while (!available_nodes.empty()) {
        const NodeId node = available_nodes.front();
        available_nodes.popFront();
        thread2node[thread_index]    = node;
        active_threads[thread_index] = true;
        if (++thread_index >= nb_threads) break;
      }
    }


    // prepare keeping information about memory usage. This is useful if the user
    // added constraints on memory usage. When operations cannot be performed
    // due to memory shortage, they will be temporarily stored into ordered vectors
    double overall_memory_used = 0.0;   // the current memory used by all the threads

    // create the mutexes needed for threads synchronization
    std::vector< std::mutex* > thread2mutex;
    thread2mutex.reserve(nb_threads);
    for (Idx i = 0; i < nb_threads; ++i)
      thread2mutex.push_back(new std::mutex());
    std::vector< std::condition_variable* > thread2not_empty;
    thread2not_empty.reserve(nb_threads);
    for (Idx i = 0; i < nb_threads; ++i)
      thread2not_empty.push_back(new std::condition_variable());
    std::mutex overall_mutex;
    std::mutex aff_mutex;
    std::cout << "init remain : " << nb_remaining_operations.load() << std::endl;

    // here, we create a lambda that will be executed by all the threads
    // to execute the operations in a parallel manner
    auto opExecute = [this,
                      &schedule,
                      &nb_parents_to_execute,
                      &overall_mutex,
                      &aff_mutex,
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
      auto& this_mutex     = *thread2mutex[this_thread];
      auto& this_not_empty = *thread2not_empty[this_thread];

      // get the operation to execute
      auto& node_to_execute = thread2node[this_thread];


std::chrono::steady_clock::time_point begin, end1, end2;
size_t nb1 = 0, nb2 = 0;
int nb_exec = 0;

      while (true) {
        {
/*
          aff_mutex.lock();
          std::cout << "thread " << this_thread << " enter wait" << std::endl;
          aff_mutex.unlock();

*/

          // wait until some operation is available or all the operations have
          // been executed
          Size nb_remaining = Size(0);
          std::unique_lock< std::mutex > lock(this_mutex);
          this_not_empty.wait(lock, [this_thread, &aff_mutex, &node_to_execute, &nb_remaining_operations, &nb_remaining] {
            nb_remaining = nb_remaining_operations.load();
            return !node_to_execute.empty() || (nb_remaining == Size(0));
          });

          // if all the operations have been executed, stop the thread
          if (nb_remaining == Size(0)) {
            thread2not_empty[(this_thread+1) % nb_threads]->notify_one();
            std::cout << "timing : " << nb1 << " vs " << nb2 << " ex " << nb_exec
                      << " = " << schedule.dag().sizeNodes() << std::endl;
            return;
          }


/*
          aff_mutex.lock();
          std::cout << "thread " << this_thread << " passed wait2" << std::endl;
          aff_mutex.unlock();
*/
        }
begin = std::chrono::steady_clock::now();
        // now, actually execute the operations
        for (const auto node: nodes_to_execute) {
          const_cast< ScheduleOperation< ALLOC >& >(schedule.operation(node)).execute();
        }
        end1 = std::chrono::steady_clock::now();
          nb1 += std::chrono::duration_cast<std::chrono::microseconds>(end1 - begin).count();

begin = std::chrono::steady_clock::now();
        //std::cout << "has been executed" << std::endl;

        // compute the next operations to execute
        std::vector< std::pair< NodeId, double > > new_available_nodes;
        new_available_nodes.reserve(dag.sizeNodes());

        {
          // lock the next shared memory operations
          std::lock_guard< std::mutex > overall_lock(overall_mutex);

          //std::cout << schedule.dag() << std::endl;

          // update the set of available operations
          for (const auto node: nodes_to_execute) {
/*
            aff_mutex.lock();
            std::cout << "executed " << node << std::endl;
            aff_mutex.unlock();
*/
            for (const auto child_node: dag.children(node)) {
              // indicate that node has been executed
              --nb_parents_to_execute[child_node];
              const Size nb_pars_to_execute = nb_parents_to_execute[child_node].load();
/*
              aff_mutex.lock();
              std::cout << " child: " << child_node << " : " << nb_pars_executed << std::endl;
              aff_mutex.unlock();
*/

              // check if all parents have been executed
              if (nb_pars_to_execute == Size(0)) {
                new_available_nodes.push_back(std::pair< NodeId, double >(child_node, 0.0));
              }
            }
          }

          // update the number of instruction of the current thread in thread2nb_instructions
          for (auto& elt: thread2nb_instructions) {
            if (elt->first == this_thread) {
              elt->second = 0.0;
              break;
            }
          }

          // sort the threads by increasing number of operations to perform
          std::sort(thread2nb_instructions.begin(), thread2nb_instructions.end(),
                    [](const std::unique_ptr< std::pair< Idx, double > >& a,
                       const std::unique_ptr< std::pair< Idx, double > >& b) -> bool {
                      return a->second < b->second;
                    });

          // store the order of the threads in thread_order
          {
            Idx i = Idx(0);
            for(const auto& elt: thread2nb_instructions) {
              thread_order[i] = elt->first;
              ++i;
            }
          }
        }

        // now the operations have been performed
        nb_exec += nodes_to_execute.size();
        nb_remaining_operations.fetch_sub(nodes_to_execute.size());

        this_mutex.lock();
        nodes_to_execute.clear();
        this_mutex.unlock();


        // sort the number of instructions of the new_available_nodes by
        // decreasing order
        for (auto& avail: new_available_nodes)
          avail.second = schedule.operation(avail.first).nbOperations();
        std::sort(new_available_nodes.begin(), new_available_nodes.end(),
                  [](const std::pair< NodeId, double >& a,
                     const std::pair< NodeId, double >& b) -> bool {
                    return b.second < a.second;
                  });


        // assign the new available operations to the thread
        Idx  thread_index = 0;
        bool had_no_ops;
        std::vector<Idx> to_wake_up;
        to_wake_up.reserve(new_available_nodes.size() < nb_threads ? new_available_nodes.size() : nb_threads);
        for (const auto& avail: new_available_nodes) {
          const Idx thread = thread_order[thread_index];
          thread_index = (thread_index + 1) % nb_threads;
          thread2mutex[thread]->lock();
          had_no_ops = thread2available_nodes[thread].empty();
          thread2available_nodes[thread].insert(avail.first);
          thread2mutex[thread]->unlock();

          if (had_no_ops) to_wake_up.push_back(thread);
        }

        for (auto thread: to_wake_up)
          thread2not_empty[thread]->notify_all();

        // std::cout << "fin du loop" << std::endl;
end2 = std::chrono::steady_clock::now();
          nb2 += std::chrono::duration_cast<std::chrono::microseconds>(end2 - begin).count();
/*
        aff_mutex.lock();
        std::cout << "thread " << this_thread << " remaining ops: "
                  << nb_remaining_operations.load() << std::endl;
        std::cout << "thread " << this_thread << " ended loop" << std::endl;
        aff_mutex.unlock();
*/
      }
    };

    std::cout << "real nb: " << nb_threads << std::endl;

    // launch the threads
    ThreadExecutor::execute(nb_threads, opExecute);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
