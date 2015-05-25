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
 * @brief A class to combine efficiently several ScheduleMultiDims
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <limits>
#include <agrum/config.h>

#include <agrum/graphicalModels/inference/scheduleCombinationBasic.h>

namespace gum {

  /// constructor
  template <typename GUM_SCALAR>
  ScheduleCombinationBasic<GUM_SCALAR>::ScheduleCombinationBasic(
      MultiDimImplementation<GUM_SCALAR> *(*combine)(
          const MultiDimImplementation<GUM_SCALAR> &,
          const MultiDimImplementation<GUM_SCALAR> &))
      : ScheduleCombination<GUM_SCALAR>(), _combine(combine) {
    /// for debugging purposes
    GUM_CONSTRUCTOR(ScheduleCombinationBasic);
  }

  /// copy constructor
  template <typename GUM_SCALAR>
  ScheduleCombinationBasic<GUM_SCALAR>::ScheduleCombinationBasic(
      const ScheduleCombinationBasic<GUM_SCALAR> &from)
      : ScheduleCombination<GUM_SCALAR>(from), _combine(from._combine) {
    /// for debugging purposes
    GUM_CONS_CPY(ScheduleCombinationBasic);
  }

  /// destructor
  template <typename GUM_SCALAR>
  ScheduleCombinationBasic<GUM_SCALAR>::~ScheduleCombinationBasic() {
    /// for debugging purposes
    GUM_DESTRUCTOR(ScheduleCombinationBasic);
  }

  /// virtual constructor
  template <typename GUM_SCALAR>
  ScheduleCombinationBasic<GUM_SCALAR> *
  ScheduleCombinationBasic<GUM_SCALAR>::newFactory() const {
    return new ScheduleCombinationBasic<GUM_SCALAR>(*this);
  }

  /// changes the function used for combining two MultiDimImplementations
  template <typename GUM_SCALAR>
  void ScheduleCombinationBasic<GUM_SCALAR>::setCombineFunction(
      MultiDimImplementation<GUM_SCALAR> *(*combine)(
          const MultiDimImplementation<GUM_SCALAR> &,
          const MultiDimImplementation<GUM_SCALAR> &)) {
    _combine = combine;
  }

  /// returns the combination function currently used by the combinator
  template <typename GUM_SCALAR>
  MultiDimImplementation<GUM_SCALAR> *(*ScheduleCombinationBasic<
      GUM_SCALAR>::combineFunction())(const MultiDimImplementation<GUM_SCALAR> &,
                                      const MultiDimImplementation<GUM_SCALAR> &) {
    return _combine;
  }

  /// returns the domain size of the Cartesian product of the union of all the
  /// variables in seq1 and seq2
  template <typename GUM_SCALAR>
  Size ScheduleCombinationBasic<GUM_SCALAR>::_combinedSize(
      const Sequence<const DiscreteVariable *> &seq1,
      const Sequence<const DiscreteVariable *> &seq2) const {
    if (seq1.empty() && seq2.empty())
      return 0;

    Size size = 1;

    for (const auto var : seq1)
      size *= var->domainSize();

    for (const auto var : seq2)
      if (!seq1.exists(var))
        size *= var->domainSize();

    return size;
  }

  // adds operations to an already created schedule
  template <typename GUM_SCALAR>
  ScheduleMultiDim<GUM_SCALAR> ScheduleCombinationBasic<GUM_SCALAR>::combine(
      const Set<const ScheduleMultiDim<GUM_SCALAR> *> &set,
      Schedule<GUM_SCALAR> &schedule) {
    // check if the set passed in argument is empty. If so, raise an exception
    if (set.size() < 2) {
      GUM_ERROR(InvalidArgumentsNumber,
                "the set passed to a ScheduleCombinationBasic"
                " should at least contain two elements");
    }

    // create a vector with all the tables to combine
    std::vector<const ScheduleMultiDim<GUM_SCALAR> *> tables(set.size());

    {
      unsigned int i = 0;

      for (const auto sched : set) {
        tables[i] = sched;
        i += 1;
      }
    }

    // create a vector indicating wether the elements in tables are freshly
    // created ScheduleMultiDim<GUM_SCALAR>* due to the combination of some
    // ScheduleMultiDims or if they were added by the user into the
    // combination container
    std::vector<bool> is_t_new(tables.size(), false);

    // for each pair of tables (i,j), compute the size of the table that would
    // result from the addition of tables i and j and store the result into a
    // priorityQueue
    std::pair<unsigned int, unsigned int> pair;

    PriorityQueue<std::pair<unsigned int, unsigned int>, Size> queue;

    for (unsigned int i = 0; i < tables.size(); ++i) {
      pair.first = i;
      const Sequence<const DiscreteVariable *> &seq1 =
          tables[i]->variablesSequence();

      for (unsigned int j = i + 1; j < tables.size(); ++j) {
        pair.second = j;
        queue.insert(pair, _combinedSize(seq1, tables[j]->variablesSequence()));
      }
    }

    // now parse the priority queue: the top element (i,j) gives the combination
    // to perform. When the result R has been computed, substitute i by R, remove
    // table j and recompute all the priorities of all the pairs (R,k) still
    // available.
    // Timer timer;
    for (unsigned int k = 1; k < tables.size(); ++k) {
      // get the combination to perform and do it
      pair = queue.pop();
      unsigned int ti = pair.first;
      unsigned int tj = pair.second;

      // create the combination that will be performed later on and put it into
      // the schedule
      ScheduleCombine<GUM_SCALAR> comb(*(tables[ti]), *(tables[tj]), _combine);
      NodeId comb_id = schedule.insert(comb);

      // substitute tables[pair.first] by the result and delete the temporary
      // multidim tables

      if (tables[ti] && is_t_new[ti]) {
        ScheduleDeleteMultiDim<GUM_SCALAR> del(*(tables[ti]));
        NodeId del_id = schedule.insert(del);
        const NodeSet &set_i = schedule.operationsInvolving(*(tables[ti]));
        schedule.forceAfter(del_id, set_i);
      }

      if (tables[tj] && is_t_new[tj]) {
        ScheduleDeleteMultiDim<GUM_SCALAR> del(*(tables[tj]));
        NodeId del_id = schedule.insert(del);
        const NodeSet &set_j = schedule.operationsInvolving(*(tables[tj]));
        schedule.forceAfter(del_id, set_j);
      }

      tables[ti] = &(static_cast<const ScheduleCombine<GUM_SCALAR> &>

                     (schedule.operation(comb_id)).result());
      is_t_new[ti] = true;
      tables[tj] = 0;

      // remove all the pairs involving tj in the priority queue

      for (unsigned int ind = 0; ind < tj; ++ind) {
        if (tables[ind]) {
          pair.first = ind;
          queue.erase(pair);
        }
      }

      pair.first = tj;

      for (unsigned int ind = tj + 1; ind < tables.size(); ++ind) {
        if (tables[ind]) {
          pair.second = ind;
          queue.erase(pair);
        }
      }

      // update the "combinated" size of all the pairs involving "result"
      {
        const Sequence<const DiscreteVariable *> &seq1 =
            tables[ti]->variablesSequence();
        pair.second = ti;
        Size newsize;

        for (unsigned int ind = 0; ind < ti; ++ind) {
          if (tables[ind]) {
            pair.first = ind;
            newsize = _combinedSize(seq1, tables[ind]->variablesSequence());
            queue.setPriority(pair, newsize);
          }
        }

        pair.first = ti;

        for (unsigned int ind = ti + 1; ind < tables.size(); ++ind) {
          if (tables[ind]) {
            pair.second = ind;
            newsize = _combinedSize(seq1, tables[ind]->variablesSequence());
            queue.setPriority(pair, newsize);
          }
        }
      }
    }

    // here, there remains only one nonzero pointer in tables:
    // the result of our combination
    unsigned int k = 0;

    while (!tables[k])
      ++k;

    return *(tables[k]);
  }

  // adds operations to an already created schedule
  template <typename GUM_SCALAR>
  INLINE ScheduleMultiDim<GUM_SCALAR> ScheduleCombinationBasic<GUM_SCALAR>::combine(
      const Set<const MultiDimImplementation<GUM_SCALAR> *> &set,
      Schedule<GUM_SCALAR> &schedule) {
    return ScheduleCombination<GUM_SCALAR>::combine(set, schedule);
  }

  // adds operations to an already created schedule
  template <typename GUM_SCALAR>
  template <template <typename> class TABLE>
  INLINE ScheduleMultiDim<GUM_SCALAR> ScheduleCombinationBasic<GUM_SCALAR>::combine(
      const Set<const TABLE<GUM_SCALAR> *> &set, Schedule<GUM_SCALAR> &schedule) {
    return ScheduleCombination<GUM_SCALAR>::combine(set, schedule);
  }

  /// returns the result of the combination
  template <typename GUM_SCALAR>
  float ScheduleCombinationBasic<GUM_SCALAR>::nbOperations(
      const Set<const ScheduleMultiDim<GUM_SCALAR> *> &set,
      const Schedule<GUM_SCALAR> &schedule) {
    // check if the set passed in argument is empty.
    if (set.size() < 2)
      return 0.0f;

    float result = 0.0f;

    // create a vector with all the tables to combine
    std::vector<const Sequence<const DiscreteVariable *> *> tables(set.size());

    {
      unsigned int i = 0;

      for (const auto sched : set) {
        tables[i] = &(sched->variablesSequence());
        i += 1;
      }
    }

    // create a vector indicating wether the elements in tables are freshly
    // created Sequence<const DiscreteVariable *>* due to the combination of some
    // ScheduleMultiDims or if they were added by the user into the combination
    // container
    std::vector<bool> is_t_new(tables.size(), false);

    // for each pair of tables (i,j), compute the size of the table that would
    // result from the addition of tables i and j and store the result into a
    // priorityQueue
    std::pair<unsigned int, unsigned int> pair;
    PriorityQueue<std::pair<unsigned int, unsigned int>, Size> queue;

    for (unsigned int i = 0; i < tables.size(); ++i) {
      pair.first = i;

      for (unsigned int j = i + 1; j < tables.size(); ++j) {
        pair.second = j;
        queue.insert(pair, _combinedSize(*(tables[i]), *(tables[j])));
      }
    }

    // now parse the priority queue: the top element (i,j) gives the combination
    // to perform. When the result R has been computed, substitute i by R, remove
    // table j and recompute all the priorities of all the pairs (R,k) still
    // available.
    for (unsigned int k = 1; k < tables.size(); ++k) {
      // get the combination to perform and do it
      pair = queue.pop();
      unsigned int ti = pair.first;
      unsigned int tj = pair.second;

      // compute the result
      Sequence<const DiscreteVariable *> *new_seq =
          new Sequence<const DiscreteVariable *>;
      const Sequence<const DiscreteVariable *> &seq1 = *(tables[ti]);
      const Sequence<const DiscreteVariable *> &seq2 = *(tables[tj]);

      Size new_size = 1;

      for (const auto var : seq1) {
        new_size *= var->domainSize();
        new_seq->insert(var);
      }

      for (const auto var : seq2)
        if (!seq1.exists(var)) {
          new_size *= var->domainSize();
          new_seq->insert(var);
        }

      result += new_size;

      // substitute tables[pair.first] by the result

      if (tables[ti] && is_t_new[ti])
        delete tables[ti];

      if (tables[tj] && is_t_new[tj])
        delete tables[tj];

      tables[ti] = new_seq;

      is_t_new[ti] = true;

      tables[tj] = 0;

      // remove all the pairs involving tj in the priority queue
      for (unsigned int ind = 0; ind < tj; ++ind) {
        if (tables[ind]) {
          pair.first = ind;
          queue.erase(pair);
        }
      }

      pair.first = tj;

      for (unsigned int ind = tj + 1; ind < tables.size(); ++ind) {
        if (tables[ind]) {
          pair.second = ind;
          queue.erase(pair);
        }
      }

      // update the "combined" size of all the pairs involving "new_seq"
      {
        pair.second = ti;
        Size newsize;

        for (unsigned int ind = 0; ind < ti; ++ind) {
          if (tables[ind]) {
            pair.first = ind;
            newsize = _combinedSize(*new_seq, *(tables[ind]));
            queue.setPriority(pair, newsize);
          }
        }

        pair.first = ti;

        for (unsigned int ind = ti + 1; ind < tables.size(); ++ind) {
          if (tables[ind]) {
            pair.second = ind;
            newsize = _combinedSize(*new_seq, *(tables[ind]));
            queue.setPriority(pair, newsize);
          }
        }
      }
    }

    // here, there remains only one nonzero pointer in tables:
    // the result of our combination
    unsigned int k = 0;

    while (!tables[k])
      ++k;

    delete tables[k];

    return result;
  }

  /// returns the result of the combination
  template <typename GUM_SCALAR>
  INLINE float ScheduleCombinationBasic<GUM_SCALAR>::nbOperations(
      const Set<const MultiDimImplementation<GUM_SCALAR> *> &set,
      const Schedule<GUM_SCALAR> &schedule) {
    return ScheduleCombination<GUM_SCALAR>::nbOperations(set, schedule);
  }

  /// returns the result of the combination
  template <typename GUM_SCALAR>
  template <template <typename> class TABLE>
  INLINE float ScheduleCombinationBasic<GUM_SCALAR>::nbOperations(
      const Set<const TABLE<GUM_SCALAR> *> &set,
      const Schedule<GUM_SCALAR> &schedule) {
    return ScheduleCombination<GUM_SCALAR>::nbOperations(set, schedule);
  }

  /// returns the result of the combination
  template <typename GUM_SCALAR>
  std::pair<long, long> ScheduleCombinationBasic<GUM_SCALAR>::memoryUsage(
      const Set<const ScheduleMultiDim<GUM_SCALAR> *> &set,
      const Schedule<GUM_SCALAR> &schedule) {
    // check if the set passed in argument is empty.
    if (set.size() < 2)
      return std::pair<long, long>(0, 0);

    long max_memory = 0;
    long current_memory = 0;

    // create a vector with all the tables to combine
    std::vector<const Sequence<const DiscreteVariable *> *> tables(set.size());
    std::vector<Size> table_size(set.size());

    {
      unsigned int i = 0;

      for (const auto tab : set) {
        tables[i] = &(tab->variablesSequence());
        Size size = 0;

        for (const auto var : tab->variablesSequence())
          size *= var->domainSize();

        table_size[i] = size;
        i += 1;
      }
    }

    // create a vector indicating wether the elements in tables are freshly
    // created Sequence<const DiscreteVariable *>* due to the combination of some
    // ScheduleMultiDims or if they were added by the user into the combination
    // container
    std::vector<bool> is_t_new(tables.size(), false);

    // for each pair of tables (i,j), compute the size of the table that would
    // result from the addition of tables i and j and store the result into a
    // priorityQueue
    std::pair<unsigned int, unsigned int> pair;

    PriorityQueue<std::pair<unsigned int, unsigned int>, Size> queue;

    for (unsigned int i = 0; i < tables.size(); ++i) {
      pair.first = i;

      for (unsigned int j = i + 1; j < tables.size(); ++j) {
        pair.second = j;
        queue.insert(pair, _combinedSize(*(tables[i]), *(tables[j])));
      }
    }

    // now parse the priority queue: the top element (i,j) gives the combination
    // to perform. When the result R has been computed, substitute i by R, remove
    // table j and recompute all the priorities of all the pairs (R,k) still
    // available.
    for (unsigned int k = 1; k < tables.size(); ++k) {
      // get the combination to perform and do it
      pair = queue.pop();
      unsigned int ti = pair.first;
      unsigned int tj = pair.second;

      // compute the result
      Sequence<const DiscreteVariable *> *new_seq =
          new Sequence<const DiscreteVariable *>;
      const Sequence<const DiscreteVariable *> &seq1 = *(tables[ti]);
      const Sequence<const DiscreteVariable *> &seq2 = *(tables[tj]);

      long new_size = 1;

      for (const auto var : seq1) {
        if (std::numeric_limits<long>::max() / (long)var->domainSize() < new_size)
          GUM_ERROR(OutOfBounds, "memory usage out of long int range");

        new_size *= var->domainSize();
        new_seq->insert(var);
      }

      for (const auto var : seq2) {
        if (!seq1.exists(var)) {
          if (std::numeric_limits<long>::max() / (long)var->domainSize() < new_size)
            GUM_ERROR(OutOfBounds, "memory usage out of long int range");

          new_size *= var->domainSize();
          new_seq->insert(var);
        }
      }

      if (std::numeric_limits<long>::max() - current_memory < new_size) {
        GUM_ERROR(OutOfBounds, "memory usage out of long int range");
      }

      current_memory += new_size;

      if (current_memory > max_memory) {
        max_memory = current_memory;
      }

      // substitute tables[pair.first] by the result
      if (tables[ti] && is_t_new[ti]) {
        delete tables[ti];
        current_memory -= table_size[ti];
      }

      if (tables[tj] && is_t_new[tj]) {
        delete tables[tj];
        current_memory -= table_size[tj];
      }

      tables[ti] = new_seq;

      table_size[ti] = new_size;
      is_t_new[ti] = true;
      tables[tj] = 0;

      // remove all the pairs involving tj in the priority queue

      for (unsigned int ind = 0; ind < tj; ++ind) {
        if (tables[ind]) {
          pair.first = ind;
          queue.erase(pair);
        }
      }

      pair.first = tj;

      for (unsigned int ind = tj + 1; ind < tables.size(); ++ind) {
        if (tables[ind]) {
          pair.second = ind;
          queue.erase(pair);
        }
      }

      // update the "combined" size of all the pairs involving "new_seq"
      {
        pair.second = ti;
        Size newsize;

        for (unsigned int ind = 0; ind < ti; ++ind) {
          if (tables[ind]) {
            pair.first = ind;
            newsize = _combinedSize(*new_seq, *(tables[ind]));
            queue.setPriority(pair, newsize);
          }
        }

        pair.first = ti;

        for (unsigned int ind = ti + 1; ind < tables.size(); ++ind) {
          if (tables[ind]) {
            pair.second = ind;
            newsize = _combinedSize(*new_seq, *(tables[ind]));
            queue.setPriority(pair, newsize);
          }
        }
      }
    }

    // here, there remains only one nonzero pointer in tables:
    // the result of our combination
    unsigned int k = 0;

    while (!tables[k])
      ++k;

    delete tables[k];

    return std::pair<long, long>(max_memory, current_memory);
  }

  /// returns the memory consumption used during the combination
  template <typename GUM_SCALAR>
  INLINE std::pair<long, long> ScheduleCombinationBasic<GUM_SCALAR>::memoryUsage(
      const Set<const MultiDimImplementation<GUM_SCALAR> *> &set,
      const Schedule<GUM_SCALAR> &schedule) {
    return ScheduleCombination<GUM_SCALAR>::memoryUsage(set, schedule);
  }

  /// returns the memory consumption used during the combination
  template <typename GUM_SCALAR>
  template <template <typename> class TABLE>
  INLINE std::pair<long, long> ScheduleCombinationBasic<GUM_SCALAR>::memoryUsage(
      const Set<const TABLE<GUM_SCALAR> *> &set,
      const Schedule<GUM_SCALAR> &schedule) {
    return ScheduleCombination<GUM_SCALAR>::memoryUsage(set, schedule);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
