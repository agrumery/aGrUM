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
 * @brief A generic class to combine efficiently several MultiDim tables
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <limits>

#include <agrum/config.h>

#include <agrum/core/priorityQueue.h>
#include <agrum/multidim/operators/multiDimCombinationDefault.h>

namespace gum {

  /// constructor
  template <typename GUM_SCALAR, template <typename> class TABLE>
  MultiDimCombinationDefault<GUM_SCALAR, TABLE>::MultiDimCombinationDefault(
      TABLE<GUM_SCALAR> *(*combine)(const TABLE<GUM_SCALAR> &,
                                    const TABLE<GUM_SCALAR> &))
      : MultiDimCombination<GUM_SCALAR, TABLE>(), _combine(combine) {
    /// for debugging purposes
    GUM_CONSTRUCTOR(MultiDimCombinationDefault);
  }

  /// copy constructor
  template <typename GUM_SCALAR, template <typename> class TABLE>
  MultiDimCombinationDefault<GUM_SCALAR, TABLE>::MultiDimCombinationDefault(
      const MultiDimCombinationDefault<GUM_SCALAR, TABLE> &from)
      : MultiDimCombination<GUM_SCALAR, TABLE>(), _combine(from._combine) {
    /// for debugging purposes
    GUM_CONS_CPY(MultiDimCombinationDefault);
  }

  /// destructor
  template <typename GUM_SCALAR, template <typename> class TABLE>
  MultiDimCombinationDefault<GUM_SCALAR, TABLE>::~MultiDimCombinationDefault() {
    /// for debugging purposes
    GUM_DESTRUCTOR(MultiDimCombinationDefault);
  }

  /// virtual constructor
  template <typename GUM_SCALAR, template <typename> class TABLE>
  MultiDimCombinationDefault<GUM_SCALAR, TABLE> *
  MultiDimCombinationDefault<GUM_SCALAR, TABLE>::newFactory() const {
    return new MultiDimCombinationDefault<GUM_SCALAR, TABLE>(_combine);
  }

  /// changes the function used for combining two TABLES
  template <typename GUM_SCALAR, template <typename> class TABLE>
  void MultiDimCombinationDefault<GUM_SCALAR, TABLE>::setCombineFunction(
      TABLE<GUM_SCALAR> *(*combine)(const TABLE<GUM_SCALAR> &,
                                    const TABLE<GUM_SCALAR> &)) {
    _combine = combine;
  }

  /// returns the combination function currently used by the combinator
  template <typename GUM_SCALAR, template <typename> class TABLE>
  INLINE TABLE<GUM_SCALAR> *(
      *MultiDimCombinationDefault<GUM_SCALAR, TABLE>::combineFunction())(
      const TABLE<GUM_SCALAR> &, const TABLE<GUM_SCALAR> &) {
    return _combine;
  }

  /// returns the domain size of the Cartesian product of the union of all the
  /// variables in seq1 and seq2
  template <typename GUM_SCALAR, template <typename> class TABLE>
  Size MultiDimCombinationDefault<GUM_SCALAR, TABLE>::_combinedSize(
      const Sequence<const DiscreteVariable *> &seq1,
      const Sequence<const DiscreteVariable *> &seq2) const {
    if (seq1.empty() && seq2.empty())
      return 0;

    Size size = 1;

    for (Sequence<const DiscreteVariable *>::const_iterator_safe iter =
             seq1.beginSafe();
         iter != seq1.endSafe(); ++iter) {
      size *= (*iter)->domainSize();
    }

    for (Sequence<const DiscreteVariable *>::const_iterator_safe iter =
             seq2.beginSafe();
         iter != seq2.endSafe(); ++iter) {
      if (!seq1.exists(*iter))
        size *= (*iter)->domainSize();
    }

    return size;
  }

  /// returns the result of the combination
  /// @todo This implementation is very improvable : we need a way for decorators
  /// to swap contents instead of copying it...
  template <typename GUM_SCALAR, template <typename> class TABLE>
  INLINE void MultiDimCombinationDefault<GUM_SCALAR, TABLE>::combine(
      TABLE<GUM_SCALAR> &container, const Set<const TABLE<GUM_SCALAR> *> &set) {
    TABLE<GUM_SCALAR> *res = combine(set);
    container = *res;
    delete (res);
  }

  /// returns the result of the combination
  template <typename GUM_SCALAR, template <typename> class TABLE>
  TABLE<GUM_SCALAR> *MultiDimCombinationDefault<GUM_SCALAR, TABLE>::combine(
      const Set<const TABLE<GUM_SCALAR> *> &set) {
    // check if the set passed in argument is empty. If so, raise an exception
    if (set.size() < 2) {
      GUM_ERROR(InvalidArgumentsNumber,
                "the set passed to a MultiDimCombinationDefault"
                " should at least contain two elements");
    }

    // create a vector with all the tables to combine
    std::vector<const TABLE<GUM_SCALAR> *> tables(set.size());

    {
      unsigned int i = 0;

      for (typename Set<const TABLE<GUM_SCALAR> *>::const_iterator_safe
               iter = set.beginSafe();
           iter != set.endSafe(); ++iter, ++i) {
        tables[i] = *iter;
      }
    }

    // create a vector indicating wether the elements in tables are freshly
    // created TABLE<GUM_SCALAR>* due to the combination of some TABLEs or if they
    // were added by the user into the combination container
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
    for (unsigned int k = 1; k < tables.size(); ++k) {
      // get the combination to perform and do it
      pair = queue.pop();
      unsigned int ti = pair.first;
      unsigned int tj = pair.second;

      TABLE<GUM_SCALAR> *result = _combine(*(tables[ti]), *(tables[tj]));

      // substitute tables[pair.first] by the result

      if (tables[ti] && is_t_new[ti])
        delete tables[ti];

      if (tables[tj] && is_t_new[tj])
        delete tables[tj];

      tables[ti] = result;

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

    return const_cast<TABLE<GUM_SCALAR> *>(tables[k]);
  }

  /// returns the result of the combination
  template <typename GUM_SCALAR, template <typename> class TABLE>
  float MultiDimCombinationDefault<GUM_SCALAR, TABLE>::nbOperations(
      const Set<const Sequence<const DiscreteVariable *> *> &set) const {
    // check if the set passed in argument is empty.
    if (set.size() < 2)
      return 0.0f;

    float result = 0.0f;

    // create a vector with all the tables to combine
    std::vector<const Sequence<const DiscreteVariable *> *> tables(set.size());

    {
      unsigned int i = 0;

      for (typename Set<
               const Sequence<const DiscreteVariable *> *>::const_iterator_safe
               iter = set.beginSafe();
           iter != set.endSafe(); ++iter, ++i) {
        tables[i] = *iter;
      }
    }

    // create a vector indicating wether the elements in tables are freshly
    // created Sequence<const DiscreteVariable *>* due to the combination of some
    // TABLEs or if they were added by the user into the combination container
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

      for (Sequence<const DiscreteVariable *>::const_iterator_safe iter =
               seq1.beginSafe();
           iter != seq1.endSafe(); ++iter) {
        new_size *= (*iter)->domainSize();
        new_seq->insert(*iter);
      }

      for (Sequence<const DiscreteVariable *>::const_iterator_safe iter =
               seq2.beginSafe();
           iter != seq2.endSafe(); ++iter) {
        if (!seq1.exists(*iter)) {
          new_size *= (*iter)->domainSize();
          new_seq->insert(*iter);
        }
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
  template <typename GUM_SCALAR, template <typename> class TABLE>
  float MultiDimCombinationDefault<GUM_SCALAR, TABLE>::nbOperations(
      const Set<const TABLE<GUM_SCALAR> *> &set) const {
    // check if the set passed in argument is empty.
    if (set.size() < 2)
      return 0.0f;

    // create the set of sets of discrete variables involved in the tables
    Set<const Sequence<const DiscreteVariable *> *> var_set(set.size());

    for (typename Set<const TABLE<GUM_SCALAR> *>::const_iterator_safe iter =
             set.beginSafe();
         iter != set.endSafe(); ++iter) {
      var_set << &((*iter)->variablesSequence());
    }

    return nbOperations(var_set);
  }

  /// returns the memory consumption used during the combination
  template <typename GUM_SCALAR, template <typename> class TABLE>
  std::pair<long, long> MultiDimCombinationDefault<GUM_SCALAR, TABLE>::memoryUsage(
      const Set<const Sequence<const DiscreteVariable *> *> &set) const {
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

      for (typename Set<
               const Sequence<const DiscreteVariable *> *>::const_iterator_safe
               iter = set.beginSafe();
           iter != set.endSafe(); ++iter, ++i) {
        const Sequence<const DiscreteVariable *> *vars = *iter;
        tables[i] = vars;

        long size = 0;

        for (typename Sequence<const DiscreteVariable *>::const_iterator_safe iter2 =
                 vars->beginSafe();
             iter2 != vars->endSafe(); ++iter2) {
          size *= (*iter2)->domainSize();
        }

        table_size[i] = size;
      }
    }

    // create a vector indicating wether the elements in tables are freshly
    // created Sequence<const DiscreteVariable *>* due to the combination of some
    // TABLEs or if they were added by the user into the combination container
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

      for (Sequence<const DiscreteVariable *>::const_iterator_safe iter =
               seq1.beginSafe();
           iter != seq1.endSafe(); ++iter) {
        if (std::numeric_limits<long>::max() / (long)(*iter)->domainSize() <
            new_size) {
          GUM_ERROR(OutOfBounds, "memory usage out of long int range");
        }

        new_size *= (*iter)->domainSize();

        new_seq->insert(*iter);
      }

      for (Sequence<const DiscreteVariable *>::const_iterator_safe iter =
               seq2.beginSafe();
           iter != seq2.endSafe(); ++iter) {
        if (!seq1.exists(*iter)) {
          if (std::numeric_limits<long>::max() / (long)(*iter)->domainSize() <
              new_size) {
            GUM_ERROR(OutOfBounds, "memory usage out of long int range");
          }

          new_size *= (*iter)->domainSize();

          new_seq->insert(*iter);
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
  template <typename GUM_SCALAR, template <typename> class TABLE>
  std::pair<long, long> MultiDimCombinationDefault<GUM_SCALAR, TABLE>::memoryUsage(
      const Set<const TABLE<GUM_SCALAR> *> &set) const {
    // check if the set passed in argument is empty.
    if (set.size() < 2)
      return std::pair<long, long>(0, 0);

    // create the set of sets of discrete variables involved in the tables
    Set<const Sequence<const DiscreteVariable *> *> var_set(set.size());

    for (typename Set<const TABLE<GUM_SCALAR> *>::const_iterator_safe iter =
             set.beginSafe();
         iter != set.endSafe(); ++iter) {
      var_set << &((*iter)->variablesSequence());
    }

    return memoryUsage(var_set);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
