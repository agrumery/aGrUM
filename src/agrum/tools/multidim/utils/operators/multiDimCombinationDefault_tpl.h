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
 * @brief A generic class to combine efficiently several MultiDim tables
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <limits>

#  include <agrum/agrum.h>

#  include <agrum/tools/core/priorityQueue.h>
#  include <agrum/tools/multidim/utils/operators/multiDimCombinationDefault.h>

namespace gum {

  // constructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimCombinationDefault< GUM_SCALAR, TABLE >::MultiDimCombinationDefault(
     TABLE< GUM_SCALAR >* (*combine)(const TABLE< GUM_SCALAR >&,
                                     const TABLE< GUM_SCALAR >&)) :
      MultiDimCombination< GUM_SCALAR, TABLE >(),
      combine_(combine) {
    GUM_CONSTRUCTOR(MultiDimCombinationDefault);
  }

  // copy constructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimCombinationDefault< GUM_SCALAR, TABLE >::MultiDimCombinationDefault(
     const MultiDimCombinationDefault< GUM_SCALAR, TABLE >& from) :
      MultiDimCombination< GUM_SCALAR, TABLE >(),
      combine_(from.combine_) {
    // for debugging purposes
    GUM_CONS_CPY(MultiDimCombinationDefault);
  }

  // destructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimCombinationDefault< GUM_SCALAR, TABLE >::~MultiDimCombinationDefault() {
    // for debugging purposes
    GUM_DESTRUCTOR(MultiDimCombinationDefault);
  }

  // virtual constructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimCombinationDefault< GUM_SCALAR, TABLE >*
     MultiDimCombinationDefault< GUM_SCALAR, TABLE >::newFactory() const {
    return new MultiDimCombinationDefault< GUM_SCALAR, TABLE >(combine_);
  }

  // changes the function used for combining two TABLES
  template < typename GUM_SCALAR, template < typename > class TABLE >
  void MultiDimCombinationDefault< GUM_SCALAR, TABLE >::setCombineFunction(
     TABLE< GUM_SCALAR >* (*combine)(const TABLE< GUM_SCALAR >&,
                                     const TABLE< GUM_SCALAR >&)) {
    combine_ = combine;
  }

  // returns the combination function currently used by the combinator
  template < typename GUM_SCALAR, template < typename > class TABLE >
  INLINE TABLE< GUM_SCALAR >* (
     *MultiDimCombinationDefault< GUM_SCALAR, TABLE >::combineFunction())(
     const TABLE< GUM_SCALAR >&,
     const TABLE< GUM_SCALAR >&) {
    return combine_;
  }

  // returns the domain size of the Cartesian product of the union of all the
  // variables in seq1 and seq2
  template < typename GUM_SCALAR, template < typename > class TABLE >
  Size MultiDimCombinationDefault< GUM_SCALAR, TABLE >::combinedSize_(
     const Sequence< const DiscreteVariable* >& seq1,
     const Sequence< const DiscreteVariable* >& seq2) const {
    if (seq1.empty() && seq2.empty()) return 0;

    Size size = 1;

    for (const auto ptrVar: seq1)
      size *= ptrVar->domainSize();

    for (const auto ptrVar: seq2)
      if (!seq1.exists(ptrVar)) size *= ptrVar->domainSize();

    return size;
  }

  // returns the result of the combination
  template < typename GUM_SCALAR, template < typename > class TABLE >
  INLINE void MultiDimCombinationDefault< GUM_SCALAR, TABLE >::combine(
     TABLE< GUM_SCALAR >&                     container,
     const Set< const TABLE< GUM_SCALAR >* >& set) {
    TABLE< GUM_SCALAR >* res = combine(set);
    container                = std::move(*res);
    delete (res);
  }

  // returns the result of the combination
  template < typename GUM_SCALAR, template < typename > class TABLE >
  TABLE< GUM_SCALAR >* MultiDimCombinationDefault< GUM_SCALAR, TABLE >::combine(
     const Set< const TABLE< GUM_SCALAR >* >& set) {
    // check if the set passed in argument is empty. If so, raise an exception
    if (set.size() < 2) {
      GUM_ERROR(InvalidArgumentsNumber,
                "the set passed to a MultiDimCombinationDefault"
                " should at least contain two elements");
    }

    // create a vector with all the tables to combine
    std::vector< const TABLE< GUM_SCALAR >* > tables(set.size());
    const Size                                tabsize = tables.size();

    {
      Size i = Size(0);
      for (auto iter = set.cbegin(); iter != set.cend(); ++iter, ++i) {
        tables[i] = *iter;
      }
    }

    // create a vector indicating wether the elements in tables are freshly
    // created TABLE<GUM_SCALAR>* due to the combination of some TABLEs or if
    // they were added by the user into the combination container
    std::vector< bool > is_t_new(tabsize, false);

    // for each pair of tables (i,j), compute the size of the table that would
    // result from the addition of tables i and j and store the result into a
    // priorityQueue
    std::pair< Size, Size >                        pair;
    PriorityQueue< std::pair< Size, Size >, Size > queue;

    for (Size i = Size(0); i < tabsize; ++i) {
      pair.first = i;
      const Sequence< const DiscreteVariable* >& seq1
         = tables[i]->variablesSequence();

      for (Size j = i + 1; j < tabsize; ++j) {
        pair.second = j;
        queue.insert(pair, combinedSize_(seq1, tables[j]->variablesSequence()));
      }
    }

    // now parse the priority queue: the top element (i,j) gives the combination
    // to perform. When the result R has been computed, substitute i by R,
    // remove table j and recompute all the priorities of all the pairs (R,k)
    // still available.
    for (Size k = 1; k < tabsize; ++k) {
      // get the combination to perform and do it
      pair          = queue.pop();
      const Size ti = pair.first;
      const Size tj = pair.second;

      TABLE< GUM_SCALAR >* result = combine_(*(tables[ti]), *(tables[tj]));

      // substitute tables[pair.first] by the result
      if (tables[ti] && is_t_new[ti]) delete tables[ti];
      if (tables[tj] && is_t_new[tj]) delete tables[tj];

      tables[ti]   = result;
      is_t_new[ti] = true;

      tables[tj] = nullptr;

      // remove all the pairs involving tj in the priority queue
      for (Size ind = 0; ind < tj; ++ind) {
        if (tables[ind] != nullptr) {
          pair.first = ind;
          queue.erase(pair);
        }
      }

      pair.first = tj;
      for (Size ind = tj + 1; ind < tabsize; ++ind) {
        if (tables[ind] != nullptr) {
          pair.second = ind;
          queue.erase(pair);
        }
      }

      // update the "combinated" size of all the pairs involving "result"
      {
        const Sequence< const DiscreteVariable* >& seq1
           = tables[ti]->variablesSequence();
        pair.second = ti;
        Size newsize;

        for (Size ind = 0; ind < ti; ++ind) {
          if (tables[ind] != nullptr) {
            pair.first = ind;
            newsize    = combinedSize_(seq1, tables[ind]->variablesSequence());
            queue.setPriority(pair, newsize);
          }
        }

        pair.first = ti;

        for (Size ind = ti + 1; ind < tabsize; ++ind) {
          if (tables[ind] != nullptr) {
            pair.second = ind;
            newsize     = combinedSize_(seq1, tables[ind]->variablesSequence());
            queue.setPriority(pair, newsize);
          }
        }
      }
    }

    // here, there remains only one nonzero pointer in tables:
    // the result of our combination
    Size k = 0;
    while (tables[k] == nullptr)
      ++k;

    return const_cast< TABLE< GUM_SCALAR >* >(tables[k]);
  }

  // returns the result of the combination
  template < typename GUM_SCALAR, template < typename > class TABLE >
  float MultiDimCombinationDefault< GUM_SCALAR, TABLE >::nbOperations(
     const Set< const Sequence< const DiscreteVariable* >* >& set) const {
    // check if the set passed in argument is empty.
    if (set.size() < 2) return 0.0f;

    float result = 0.0f;

    // create a vector with all the tables to combine
    std::vector< const Sequence< const DiscreteVariable* >* > tables(set.size());
    const Size tabsize = tables.size();

    {
      Size i = Size(0);
      for (auto iter = set.cbegin(); iter != set.cend(); ++iter, ++i) {
        tables[i] = *iter;
      }
    }

    // create a vector indicating wether the elements in tables are freshly
    // created Sequence<const DiscreteVariable *>* due to the combination of
    // some TABLEs or if they were added by the user into the combination
    // container
    std::vector< bool > is_t_new(tabsize, false);

    // for each pair of tables (i,j), compute the size of the table that would
    // result from the addition of tables i and j and store the result into a
    // priorityQueue
    std::pair< Size, Size >                        pair;
    PriorityQueue< std::pair< Size, Size >, Size > queue;

    for (Size i = Size(0); i < tabsize; ++i) {
      pair.first = i;

      for (Size j = i + 1; j < tabsize; ++j) {
        pair.second = j;
        queue.insert(pair, combinedSize_(*(tables[i]), *(tables[j])));
      }
    }

    // now parse the priority queue: the top element (i,j) gives the combination
    // to perform. When the result R has been computed, substitute i by R,
    // remove table j and recompute all the priorities of all the pairs (R,k)
    // still available.
    for (Size k = 1; k < tabsize; ++k) {
      // get the combination to perform and do it
      pair          = queue.pop();
      const Size ti = pair.first;
      const Size tj = pair.second;

      // compute the result
      Sequence< const DiscreteVariable* >* new_seq
         = new Sequence< const DiscreteVariable* >;
      const Sequence< const DiscreteVariable* >& seq1 = *(tables[ti]);
      const Sequence< const DiscreteVariable* >& seq2 = *(tables[tj]);

      Size new_size = 1;

      for (const auto ptrVar: seq1) {
        new_size *= ptrVar->domainSize();
        new_seq->insert(ptrVar);
      }

      for (const auto ptrVar: seq2) {
        if (!seq1.exists(ptrVar)) {
          new_size *= ptrVar->domainSize();
          new_seq->insert(ptrVar);
        }
      }

      result += new_size;

      // substitute tables[pair.first] by the result
      if (tables[ti] && is_t_new[ti]) delete tables[ti];
      if (tables[tj] && is_t_new[tj]) delete tables[tj];

      tables[ti]   = new_seq;
      is_t_new[ti] = true;

      tables[tj] = nullptr;

      // remove all the pairs involving tj in the priority queue
      for (Size ind = 0; ind < tj; ++ind) {
        if (tables[ind] != nullptr) {
          pair.first = ind;
          queue.erase(pair);
        }
      }

      pair.first = tj;

      for (Size ind = tj + 1; ind < tabsize; ++ind) {
        if (tables[ind] != nullptr) {
          pair.second = ind;
          queue.erase(pair);
        }
      }

      // update the "combined" size of all the pairs involving "new_seq"
      {
        pair.second = ti;
        Size newsize;

        for (Size ind = 0; ind < ti; ++ind) {
          if (tables[ind] != nullptr) {
            pair.first = ind;
            newsize    = combinedSize_(*new_seq, *(tables[ind]));
            queue.setPriority(pair, newsize);
          }
        }

        pair.first = ti;

        for (Size ind = ti + 1; ind < tabsize; ++ind) {
          if (tables[ind] != nullptr) {
            pair.second = ind;
            newsize     = combinedSize_(*new_seq, *(tables[ind]));
            queue.setPriority(pair, newsize);
          }
        }
      }
    }

    // here, there remains only one nonzero pointer in tables:
    // the result of our combination
    Size k = 0;
    while (tables[k] == nullptr)
      ++k;

    delete tables[k];

    return result;
  }

  // returns the result of the combination
  template < typename GUM_SCALAR, template < typename > class TABLE >
  float MultiDimCombinationDefault< GUM_SCALAR, TABLE >::nbOperations(
     const Set< const TABLE< GUM_SCALAR >* >& set) const {
    // check if the set passed in argument is empty.
    if (set.size() < 2) return 0.0f;

    // create the set of sets of discrete variables involved in the tables
    Set< const Sequence< const DiscreteVariable* >* > var_set(set.size());

    for (const auto ptrTab: set) {
      var_set << &(ptrTab->variablesSequence());
    }

    return nbOperations(var_set);
  }

  // returns the memory consumption used during the combination
  template < typename GUM_SCALAR, template < typename > class TABLE >
  std::pair< long, long >
     MultiDimCombinationDefault< GUM_SCALAR, TABLE >::memoryUsage(
        const Set< const Sequence< const DiscreteVariable* >* >& set) const {
    // check if the set passed in argument is empty.
    if (set.size() < 2) return std::pair< long, long >(0, 0);

    Size max_memory     = 0;
    Size current_memory = 0;

    // create a vector with all the tables to combine
    std::vector< const Sequence< const DiscreteVariable* >* > tables(set.size());
    const Size tabsize = tables.size();

    std::vector< Size > table_size(set.size());

    {
      Size i = Size(0);

      for (auto iter = set.cbegin(); iter != set.cend(); ++iter, ++i) {
        const Sequence< const DiscreteVariable* >* vars = *iter;
        tables[i]                                       = vars;

        Size size = Size(1);
        for (const auto ptrVar: *vars) {
          size *= ptrVar->domainSize();
        }

        table_size[i] = size;
      }
    }

    // create a vector indicating wether the elements in tables are freshly
    // created Sequence<const DiscreteVariable *>* due to the combination of
    // some TABLEs or if they were added by the user into the combination
    // container
    std::vector< bool > is_t_new(tables.size(), false);

    // for each pair of tables (i,j), compute the size of the table that would
    // result from the addition of tables i and j and store the result into a
    // priorityQueue
    std::pair< Size, Size > pair;

    PriorityQueue< std::pair< Size, Size >, Size > queue;

    for (Size i = Size(0); i < tabsize; ++i) {
      pair.first = i;

      for (Size j = i + 1; j < tabsize; ++j) {
        pair.second = j;
        queue.insert(pair, combinedSize_(*(tables[i]), *(tables[j])));
      }
    }

    // now parse the priority queue: the top element (i,j) gives the combination
    // to perform. When the result R has been computed, substitute i by R,
    // remove table j and recompute all the priorities of all the pairs (R,k)
    // still available.
    for (Size k = 1; k < tabsize; ++k) {
      // get the combination to perform and do it
      pair          = queue.pop();
      const Size ti = pair.first;
      const Size tj = pair.second;

      // compute the result
      Sequence< const DiscreteVariable* >* new_seq
         = new Sequence< const DiscreteVariable* >;
      const Sequence< const DiscreteVariable* >& seq1 = *(tables[ti]);
      const Sequence< const DiscreteVariable* >& seq2 = *(tables[tj]);

      Size new_size = Size(1);

      for (const auto ptrVar: seq1) {
        if (std::numeric_limits< Size >::max() / ptrVar->domainSize() < new_size) {
          GUM_ERROR(OutOfBounds, "memory usage out of long int range")
        }

        new_size *= ptrVar->domainSize();

        new_seq->insert(ptrVar);
      }

      for (const auto ptrVar: seq2) {
        if (!seq1.exists(ptrVar)) {
          if (std::numeric_limits< Size >::max() / ptrVar->domainSize()
              < new_size) {
            GUM_ERROR(OutOfBounds, "memory usage out of long int range")
          }

          new_size *= ptrVar->domainSize();

          new_seq->insert(ptrVar);
        }
      }

      if (std::numeric_limits< Size >::max() - current_memory < new_size) {
        GUM_ERROR(OutOfBounds, "memory usage out of long int range")
      }

      current_memory += new_size;

      if (current_memory > max_memory) { max_memory = current_memory; }

      // substitute tables[pair.first] by the result
      if (tables[ti] && is_t_new[ti]) {
        delete tables[ti];
        current_memory -= table_size[ti];
      }

      if (tables[tj] && is_t_new[tj]) {
        delete tables[tj];
        current_memory -= table_size[tj];
      }

      tables[ti]     = new_seq;
      table_size[ti] = new_size;
      is_t_new[ti]   = true;

      tables[tj] = nullptr;

      // remove all the pairs involving tj in the priority queue
      for (Size ind = 0; ind < tj; ++ind) {
        if (tables[ind] != nullptr) {
          pair.first = ind;
          queue.erase(pair);
        }
      }

      pair.first = tj;

      for (Size ind = tj + 1; ind < tabsize; ++ind) {
        if (tables[ind] != nullptr) {
          pair.second = ind;
          queue.erase(pair);
        }
      }

      // update the "combined" size of all the pairs involving "new_seq"
      {
        pair.second = ti;
        Size newsize;

        for (Size ind = Size(0); ind < ti; ++ind) {
          if (tables[ind] != nullptr) {
            pair.first = ind;
            newsize    = combinedSize_(*new_seq, *(tables[ind]));
            queue.setPriority(pair, newsize);
          }
        }

        pair.first = ti;

        for (Size ind = ti + 1; ind < tabsize; ++ind) {
          if (tables[ind] != nullptr) {
            pair.second = ind;
            newsize     = combinedSize_(*new_seq, *(tables[ind]));
            queue.setPriority(pair, newsize);
          }
        }
      }
    }

    // here, there remains only one nonzero pointer in tables:
    // the result of our combination
    Size k = Size(0);
    while (!tables[k])
      ++k;

    delete tables[k];

    return std::pair< long, long >(long(max_memory), long(current_memory));
  }

  // returns the memory consumption used during the combination
  template < typename GUM_SCALAR, template < typename > class TABLE >
  std::pair< long, long >
     MultiDimCombinationDefault< GUM_SCALAR, TABLE >::memoryUsage(
        const Set< const TABLE< GUM_SCALAR >* >& set) const {
    // check if the set passed in argument is empty.
    if (set.size() < 2) return std::pair< long, long >(0, 0);

    // create the set of sets of discrete variables involved in the tables
    Set< const Sequence< const DiscreteVariable* >* > var_set(set.size());

    for (const auto ptrTab: set) {
      var_set << &(ptrTab->variablesSequence());
    }

    return memoryUsage(var_set);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
