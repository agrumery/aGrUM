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

    // get the DAG of the operations to perform
    std::atomic< Size > nb_remaining_operations(schedule.dag().sizeNodes());
    if (nb_remaining_operations == Size(0)) return;
    NodeProperty< Size > nb_parents_executed(nb_remaining_operations);
    for (const auto node: schedule.dag()) {
      nb_parents_executed.insert(node, Size(0));
    }

    // compute the number of threads to execute
    const Size nb_threads = nb_remaining_operations < this->maxNbThreads()
                             ? nb_remaining_operations.load()
                             : this->maxNbThreads();
    Size nb_active_threads = Size(0); // the number of threads currently active
    std::cout << "nb threads = " << nb_threads << std::endl;

    // the list of operations each thread has to execute, as well as their
    // cumulative number of instructions
    std::vector< List< NodeId > > thread2available_nodes(
       nb_threads, List< NodeId >());
    std::vector< std::unique_ptr< std::pair< Idx, double > > > thread2nb_instructions;
    thread2nb_instructions.reserve(nb_threads);
    for (Idx i = 0; i < nb_threads; ++i)
      thread2nb_instructions.push_back(std::unique_ptr< std::pair< Idx, double > >(
         new std::pair< Idx, double >(i, 0.0)));

    // assign the available nodes to the threads
    {
      // get the available operations and sort them by increasing number of
      // instructions
      const auto available_nodes = schedule.availableOperations();
      if (available_nodes.empty()) return; // if there is nothing to do, don't do it
      const Size                                 nb_available_nodes = available_nodes.size();
      std::vector< std::pair< NodeId, double > > sorted_available_nodes;
      sorted_available_nodes.reserve(nb_available_nodes);
      double nb_all_inst = 0.0; // the number of instructions needed to perform all
                                // the available operations
      for (const auto node: schedule.availableOperations()) {
        const double nb_inst_node = schedule.operation(node).nbOperations();
        nb_all_inst += nb_inst_node;
        sorted_available_nodes.push_back(std::pair< NodeId, double >(node, nb_inst_node));
      }
      std::sort(sorted_available_nodes.begin(),
                sorted_available_nodes.end(),
                [](const std::pair< NodeId, double >& a,
                   const std::pair< NodeId, double >& b) -> bool {
                  return a.second < b.second;
                });

      // assign the operations to the threads
      const double nb_inst_per_thread = nb_all_inst / nb_threads;
      double thread_nb_inst = 0.0;
      Idx thread = Idx(0);
      for(Idx i = 0; i < nb_available_nodes; ++i) {
        const double nb_inst = sorted_available_nodes[i].second;
        if ((nb_inst >= nb_inst_per_thread) ||
            (thread_nb_inst + nb_inst < nb_inst_per_thread)) {
          thread2available_nodes[thread].insert(sorted_available_nodes[i].first);
          thread2nb_instructions[thread]->second += nb_inst;
        }
        else {
          --i;
        }

        thread_nb_inst += nb_inst;
        if (thread_nb_inst >= nb_inst_per_thread) {
          thread_nb_inst = 0.0;
          thread         = (thread + 1) % nb_threads;
        }
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
                      &nb_parents_executed,
                      &overall_mutex,
                      &aff_mutex,
                      &thread2mutex,
                      &thread2not_empty,
                      &thread2available_nodes,
                      &thread2nb_instructions,
                      &nb_remaining_operations,
                      &nb_active_threads,
                      &overall_memory_used](const std::size_t this_thread,
                                            const std::size_t nb_threads) -> void {
      const DAG& dag = schedule.dag();

      // get the synchronization objects
      auto& this_mutex = *thread2mutex[this_thread];
      auto& this_not_empty = *thread2not_empty[this_thread];

      // get the list of operations to execute
      auto& nodes_to_execute = thread2available_nodes[this_thread];

      // to sort the threads by increasing number of instructions
      std::vector< Idx > thread_order(nb_threads, Idx(0));
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

          // wait until some operations are available or all the operations have
          // been executed
          Size nb_remaining = Size(0);
          std::unique_lock< std::mutex > lock(this_mutex);
          this_not_empty.wait(lock, [this_thread,&aff_mutex,&nodes_to_execute, &nb_remaining_operations, &nb_remaining] {
            nb_remaining = nb_remaining_operations.load();
            return !nodes_to_execute.empty() || (nb_remaining == Size(0));
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
              ++nb_parents_executed[child_node];
              const Size nb_pars_executed = nb_parents_executed[child_node];
/*
              aff_mutex.lock();
              std::cout << " child: " << child_node << " : " << nb_pars_executed << std::endl;
              aff_mutex.unlock();
*/

              // check if all parents have been executed
              if (dag.parents(child_node).size() == nb_pars_executed) {
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
