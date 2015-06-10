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
 * @brief a scheduler that executes any available operation (chosen aribtrarily)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <limits>
#include <agrum/config.h>

namespace gum {

  /// default constructor
  template <typename GUM_SCALAR>
  SchedulerBasic<GUM_SCALAR>::SchedulerBasic()
      : Scheduler<GUM_SCALAR>() {
    // for debugging purposes
    GUM_CONSTRUCTOR(SchedulerBasic);
  }

  /// copy constructor
  template <typename GUM_SCALAR>
  SchedulerBasic<GUM_SCALAR>::SchedulerBasic(const SchedulerBasic<GUM_SCALAR> &from)
      : Scheduler<GUM_SCALAR>(from) {
    // for debugging purposes
    GUM_CONS_CPY(SchedulerBasic);
  }

  /// destructor
  template <typename GUM_SCALAR> SchedulerBasic<GUM_SCALAR>::~SchedulerBasic() {
    // for debugging purposes
    GUM_DESTRUCTOR(SchedulerBasic);
  }

  /// virtual constructor
  template <typename GUM_SCALAR>
  SchedulerBasic<GUM_SCALAR> *SchedulerBasic<GUM_SCALAR>::newFactory() const {
    return new SchedulerBasic<GUM_SCALAR>(*this);
  }

  /// execute all the operations of a given schedule
  template <typename GUM_SCALAR>
  bool SchedulerBasic<GUM_SCALAR>::execute(Schedule<GUM_SCALAR> &schedule) {
    const NodeSet &available = schedule.availableOperations();

    while (!available.empty()) {
      for (typename NodeSet::const_iterator_safe iter = available.beginSafe();
           iter != available.endSafe(); ++iter) {
        schedule.execute(*iter);
      }
    }

    return (schedule.dag().size() == 0);
  }

  /// execute only k operations of a given schedule (default k = 1)
  template <typename GUM_SCALAR>
  bool SchedulerBasic<GUM_SCALAR>::execute(Schedule<GUM_SCALAR> &schedule,
                                           unsigned int k) {
    const NodeSet &available = schedule.availableOperations();

    while (!available.empty() && k) {
      for (typename NodeSet::const_iterator_safe iter = available.beginSafe();
           iter != available.endSafe() && k; ++iter, --k) {
        schedule.execute(*iter);
      }
    }

    return !k || (schedule.dag().size() == 0);
  }

  /** @bried returns an estimation of the number of elementary operations needed
   * to perform a given schedule */
  template <typename GUM_SCALAR>
  float SchedulerBasic<GUM_SCALAR>::nbOperations(
      const Schedule<GUM_SCALAR> &schedule) const {
    NodeSet available = schedule.availableOperations();
    DAG dag = schedule.dag();
    float nb_operations = 0;

    while (!available.empty()) {
      for (typename NodeSet::const_iterator_safe iter = available.beginSafe();
           iter != available.endSafe(); ++iter) {
        NodeId id = *iter;
        nb_operations += schedule.nbOperations(id);
        const NodeSet &children = dag.children(id);

        for (typename NodeSet::const_iterator_safe iter_children =
                 children.beginSafe();
             iter_children != children.endSafe(); ++iter_children) {
          if (dag.parents(*iter_children).size() == 1) {
            available.insert(*iter_children);
          }
        }

        dag.eraseNode(id);
        available.erase(iter);
      }
    }

    return nb_operations;
  }

  /** @bried returns an estimation of the number of elementary operations needed
   * to perform the k first ScheduleOperations of a given schedule */
  template <typename GUM_SCALAR>
  float
  SchedulerBasic<GUM_SCALAR>::nbOperations(const Schedule<GUM_SCALAR> &schedule,
                                           unsigned int k) const {
    NodeSet available = schedule.availableOperations();
    DAG dag = schedule.dag();
    float nb_operations = 0;

    while (!available.empty() && k) {
      for (typename NodeSet::const_iterator_safe iter = available.beginSafe();
           iter != available.endSafe() && k; ++iter, --k) {
        NodeId id = *iter;
        nb_operations += schedule.nbOperations(id);
        const NodeSet &children = dag.children(id);

        for (typename NodeSet::const_iterator_safe iter_children =
                 children.beginSafe();
             iter_children != children.endSafe(); ++iter_children) {
          if (dag.parents(*iter_children).size() == 1) {
            available.insert(*iter_children);
          }
        }

        dag.eraseNode(id);
        available.erase(iter);
      }
    }

    return nb_operations;
  }

  /// returns the memory consumption used during the execution of a schedule
  template <typename GUM_SCALAR>
  std::pair<long, long> SchedulerBasic<GUM_SCALAR>::memoryUsage(
      const Schedule<GUM_SCALAR> &schedule) const {
    NodeSet available = schedule.availableOperations();
    DAG dag = schedule.dag();
    long max_memory = 0;
    long current_memory = 0;

    while (!available.empty()) {
      for (typename NodeSet::const_iterator_safe iter = available.beginSafe();
           iter != available.endSafe(); ++iter) {
        NodeId id = *iter;

        std::pair<long, long> mem_op = schedule.memoryUsage(id);

        if ((std::numeric_limits<long>::max() - current_memory < mem_op.first) ||
            (std::numeric_limits<long>::max() - current_memory < mem_op.second)) {
          GUM_ERROR(OutOfBounds, "memory usage out of long int range");
        }

        if (current_memory + mem_op.first > max_memory)
          max_memory = current_memory + mem_op.first;

        current_memory += mem_op.second;

        const NodeSet &children = dag.children(id);

        for (typename NodeSet::const_iterator_safe iter_children =
                 children.beginSafe();
             iter_children != children.endSafe(); ++iter_children) {
          if (dag.parents(*iter_children).size() == 1) {
            available.insert(*iter_children);
          }
        }

        dag.eraseNode(id);
        available.erase(iter);
      }
    }

    return std::pair<long, long>(max_memory, current_memory);
  }

  /** @brief returns the memory consumption used during the execution of the
   * k first ScheduleOperations of a given schedule */
  template <typename GUM_SCALAR>
  std::pair<long, long>
  SchedulerBasic<GUM_SCALAR>::memoryUsage(const Schedule<GUM_SCALAR> &schedule,
                                          unsigned int k) const {
    NodeSet available = schedule.availableOperations();
    DAG dag = schedule.dag();
    long max_memory = 0;
    long current_memory = 0;

    while (!available.empty() && k) {
      for (typename NodeSet::const_iterator_safe iter = available.beginSafe();
           iter != available.endSafe() && k; ++iter, --k) {
        NodeId id = *iter;

        std::pair<long, long> mem_op = schedule.memoryUsage(id);

        if ((std::numeric_limits<long>::max() - current_memory < mem_op.first) ||
            (std::numeric_limits<long>::max() - current_memory < mem_op.second)) {
          GUM_ERROR(OutOfBounds, "memory usage out of long int range");
        }

        if (current_memory + mem_op.first > max_memory)
          max_memory = current_memory + mem_op.first;

        current_memory += mem_op.second;

        const NodeSet &children = dag.children(id);

        for (typename NodeSet::const_iterator_safe iter_children =
                 children.beginSafe();
             iter_children != children.endSafe(); ++iter_children) {
          if (dag.parents(*iter_children).size() == 1) {
            available.insert(*iter_children);
          }
        }

        dag.eraseNode(id);
        available.erase(iter);
      }
    }

    return std::pair<long, long>(max_memory, current_memory);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
