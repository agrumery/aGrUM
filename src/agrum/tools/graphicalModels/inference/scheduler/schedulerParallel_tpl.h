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
    {
      SchedulerSequential< ALLOC > seq_scheduler(this->maxNbThreads(), this->maxMemory());
      auto                         memory_usage = seq_scheduler.memoryUsage(schedule);
      if ((this->_max_memory != 0.0) && memory_usage.first > this->_max_memory) {
        throw std::bad_alloc();
      }
    }

    // get the DAG of the operations to perform
    DAG  dag                     = schedule.dag();
    Size nb_remaining_operations = dag.sizeNodes();


    // the comparison function used to sort the available operations
    auto cmp
       = [](const std::pair< double, double >& a, const std::pair< double, double >& b) -> bool {
      if (a.second < 0.0) {
        if (b.second < 0.0)
          return a.first < b.first;
        else
          return true;
      } else {
        if (b.second < 0)
          return false;
        else
          return a.second < b.second;
      }
    };

    PriorityQueue< NodeId, std::pair< double, double >, decltype(cmp) > available_nodes(
       cmp,
       nb_remaining_operations);

    // fill available_nodes by inserting at the front the deletions and at the
    // back the other nodes. This will enable to free memory as soon as possible
    for (const auto node: schedule.availableOperations()) {
      available_nodes.insert(node, schedule.operation(node).memoryUsage());
    }

    // if there is nothing to do, don't do it
    if (available_nodes.empty()) return;

    // prepare keeping information about memory usage. This is useful if the user
    // added constraints on memory usage. When operations cannot be performed
    // due to memory shortage, they will be temporarily stored into ordered vectors
    double overall_memory_used = 0;   // the current memory used by all the threads

    // create the mutex needed for threads synchronization
    std::mutex              mutex;
    std::condition_variable is_not_empty;

    // the number of threads currently active
    Size nb_active_threads = Size(0);

    // here, we create a lambda that will be executed by all the threads
    // to execute the operations in a parallel manner
    auto opExecute = [this,
                      &schedule,
                      &dag,
                      &mutex,
                      &is_not_empty,
                      &available_nodes,
                      &nb_remaining_operations,
                      &nb_active_threads,
                      &overall_memory_used](const std::size_t this_thread,
                                            const std::size_t nb_threads) -> void {
      // the next operations/nodes to execute
      std::vector< NodeId > nodes_to_execute;
      double                nb_instructions_to_execute;

      while (true) {
        {
          nodes_to_execute.clear();
          nb_instructions_to_execute = 0.0;

          // wait until some operations are available or all the operations have
          // been executed
          std::unique_lock< std::mutex > lock(mutex);
          is_not_empty.wait(lock, [&available_nodes, &nb_remaining_operations] {
            return !available_nodes.empty() || (nb_remaining_operations == Size(0));
          });

          // if all the operations have been executed, stop the thread
          if (nb_remaining_operations == Size(0)) {
            is_not_empty.notify_all();
            return;
          }

          // here, we found some operations to execute. So, get the first operation
          // in available_nodes
          if (this->_max_memory == 0.0) {
            while ((nb_instructions_to_execute < _max_instruction_number_)
                   && !available_nodes.empty()) {
              // get the next node to execute
              const NodeId node = available_nodes.pop();
              nodes_to_execute.push_back(node);
              nb_instructions_to_execute += schedule.operation(node).nbOperations();
            }
          } else {
            while ((nb_instructions_to_execute < _max_instruction_number_)
                   && !available_nodes.empty()) {
              // get the next node to execute
              const auto  node = available_nodes.top();
              const auto& op   = schedule.operation(node);

              // if scheduling under memory budget, check that we do not consume too
              // much memory.
              std::pair< double, double > op_mem = op.memoryUsage();
              if (overall_memory_used + op_mem.first > this->_max_memory) {
                // here, we cannot execute the operation due to memory shortage
                // so we should put the node back into the set of available_nodes:
                // if other threads are active, they may execute some deletions that
                // will provide sufficient memory to execute op later on.
                if ((nb_active_threads > Size(0)) || !nodes_to_execute.empty()) {
                  break;
                } else {
                  // here, we cannot do anything. Let's abort the inference
                  is_not_empty.notify_all();
                  return;
                }
              } else {
                nodes_to_execute.push_back(node);
                available_nodes.pop();
                overall_memory_used += op_mem.first;
                nb_instructions_to_execute += op.nbOperations();
              }
            }
          }

          // here, we remain under the memory limit, so we execute the operation.
          // so, let's indicate that the the tread is active
          ++nb_active_threads;
        }


        // now, actually execute the operations
        for (const auto node: nodes_to_execute) {
          const_cast< ScheduleOperation< ALLOC >& >(schedule.operation(node)).execute();
        }

        // lock the next shared memory operations
        std::lock_guard< std::mutex > new_lock(mutex);

        // now the thread is no more active and the operation has been performed
        --nb_active_threads;
        nb_remaining_operations -= nodes_to_execute.size();

        // update the set of available operations
        for (const auto node: nodes_to_execute) {
          for (const auto child_node: dag.children(node)) {
            if (dag.parents(child_node).size() == 1) {   // only node is a parent
              available_nodes.insert(child_node, schedule.operation(node).memoryUsage());
            }
          }

          // remove node from the dag since it has been executed
          dag.eraseNode(node);
        }

        is_not_empty.notify_all();
      }
    };


    // launch the threads
    const Size nb_threads = this->maxNbThreads() < nb_remaining_operations
                             ? this->maxNbThreads()
                             : nb_remaining_operations;

    // std::cout << "nb threads = " << nb_threads << std::endl;
    ThreadExecutor::execute(nb_threads, opExecute);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
