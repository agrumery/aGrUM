/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <limits>

#  include <agrum/agrum.h>
#  include <agrum/tools/core/priorityQueue.h>
#  include <agrum/tools/multidim/utils/operators/multiDimCombinationDefault.h>


namespace gum {

  /// constructor
  template < class TABLE >
  MultiDimCombinationDefault< TABLE >::MultiDimCombinationDefault(TABLE (*combine)(const TABLE&,
                                                                                   const TABLE&)) :
      MultiDimCombination< TABLE >(),
      _combine_(combine) {
    GUM_CONSTRUCTOR(MultiDimCombinationDefault);
  }


  /// copy constructor
  template < class TABLE >
  MultiDimCombinationDefault< TABLE >::MultiDimCombinationDefault(
     const MultiDimCombinationDefault< TABLE >& from) :
      MultiDimCombination< TABLE >(),
      _combine_(from._combine_) {
    // for debugging purposes
    GUM_CONS_CPY(MultiDimCombinationDefault);
  }


  /// destructor
  template < class TABLE >
  MultiDimCombinationDefault< TABLE >::~MultiDimCombinationDefault() {
    // for debugging purposes
    GUM_DESTRUCTOR(MultiDimCombinationDefault);
  }


  /// virtual constructor
  template < class TABLE >
  MultiDimCombinationDefault< TABLE >* MultiDimCombinationDefault< TABLE >::clone() const {
    return new MultiDimCombinationDefault< TABLE >(_combine_);
  }


  /// changes the function used for combining two TABLES
  template < class TABLE >
  void MultiDimCombinationDefault< TABLE >::setCombinationFunction(TABLE (*combine)(const TABLE&,
                                                                                    const TABLE&)) {
    _combine_ = combine;
  }


  /// returns the combination function currently used by the combinator
  template < class TABLE >
  INLINE TABLE (*MultiDimCombinationDefault< TABLE >::combinationFunction())(const TABLE&,
                                                                             const TABLE&) {
    return _combine_;
  }


  /// returns the result of the combination
  template < class TABLE >
  INLINE void MultiDimCombinationDefault< TABLE >::execute(TABLE&                     container,
                                                           const Set< const TABLE* >& set) const {
    TABLE* res = execute(set);
    container  = std::move(*res);
    delete (res);
  }


  /// returns the result of the combination
  template < class TABLE >
  TABLE* MultiDimCombinationDefault< TABLE >::execute(const Set< const TABLE* >& set) const {
    // check if the set passed in argument is empty. If so, raise an exception
    if (set.size() < 2) {
      GUM_ERROR(InvalidArgumentsNumber,
                "the set passed to a MultiDimCombinationDefault"
                " should at least contain two elements");
    }

    // create a vector with all the tables stored as multidims
    std::vector< const IScheduleMultiDim* > tables;
    tables.reserve(set.size());
    for (const auto table: set) {
      tables.push_back(new ScheduleMultiDim< TABLE >(*table, false));
    }

    // get the set of operations to perform and execute them
    auto ops_plus_res = operations(tables);
    for (auto op: ops_plus_res.first) {
      op->execute();
    }

    // get the schedule multidim of the last combination and save it
    auto& schedule_result = const_cast< ScheduleMultiDim< TABLE >& >(
       static_cast< const ScheduleMultiDim< TABLE >& >(*ops_plus_res.second));

    // note that, as ScheduleCombinations always produce new freshly allocated
    // tables, we can safely export the multiDims of their results
    auto result = schedule_result.exportMultiDim();

    // delete all the operations created as well as all the schedule tables
    _freeData_(tables, ops_plus_res.first);

    return result;
  }


  // returns the result of the combination
  template < class TABLE >
  double MultiDimCombinationDefault< TABLE >::nbOperations(
     const Set< const Sequence< const DiscreteVariable* >* >& set) const {
    // check if the set passed in argument is empty.
    if (set.size() < 2) return 0.0;

    // create a vector with all the tables stored as multidims
    std::vector< const IScheduleMultiDim* > tables;
    tables.reserve(set.size());
    for (const auto ptrVars: set) {
      tables.push_back(new ScheduleMultiDim< TABLE >(*ptrVars));
    }

    // get the set of operations to perform and compute their number of operations
    auto   ops_plus_res  = operations(tables);
    double nb_operations = 0.0;
    for (const auto op: ops_plus_res.first) {
      nb_operations += op->nbOperations();
    }

    // delete all the operations created as well as all the schedule tables
    _freeData_(tables, ops_plus_res.first);

    return nb_operations;
  }


  // returns the result of the combination
  template < class TABLE >
  double MultiDimCombinationDefault< TABLE >::nbOperations(const Set< const TABLE* >& set) const {
    // check if the set passed in argument is empty.
    if (set.size() < 2) return 0.0;

    // create the set of sets of discrete variables involved in the tables
    Set< const Sequence< const DiscreteVariable* >* > var_set(set.size());

    for (const auto ptrTab: set) {
      var_set << &(ptrTab->variablesSequence());
    }

    return nbOperations(var_set);
  }


  // returns the memory consumption used during the combination
  template < class TABLE >
  std::pair< double, double > MultiDimCombinationDefault< TABLE >::memoryUsage(
     const Set< const Sequence< const DiscreteVariable* >* >& set) const {
    // check if the set passed in argument is empty.
    if (set.size() < 2) return {0.0, 0.0};

    // create a vector with all the tables stored as multidims
    std::vector< const IScheduleMultiDim* > tables;
    tables.reserve(set.size());
    for (const auto ptrVars: set) {
      tables.push_back(new ScheduleMultiDim< TABLE >(*ptrVars));
    }

    // get the set of operations to perform and compute their memory consumption
    auto ops_plus_res = operations(tables);

    double max_memory = 0.0;
    double end_memory = 0.0;

    for (const auto op: ops_plus_res.first) {
      const auto usage = op->memoryUsage();
      if (end_memory + usage.first > max_memory) max_memory = end_memory + usage.first;
      end_memory += usage.second;
    }

    // delete all the operations created as well as all the schedule tables
    _freeData_(tables, ops_plus_res.first);

    return {max_memory, end_memory};
  }


  // returns the memory consumption used during the combination
  template < class TABLE >
  std::pair< double, double >
     MultiDimCombinationDefault< TABLE >::memoryUsage(const Set< const TABLE* >& set) const {
    // check if the set passed in argument is empty.
    if (set.size() < 2) return {0.0, 0.0};

    // create the set of sets of discrete variables involved in the tables
    Set< const Sequence< const DiscreteVariable* >* > var_set(set.size());

    for (const auto ptrTab: set) {
      var_set << &(ptrTab->variablesSequence());
    }

    return memoryUsage(var_set);
  }


  // returns the domain size of the Cartesian product of the union of all the
  // variables in seq1 and seq2
  template < class TABLE >
  INLINE double
     MultiDimCombinationDefault< TABLE >::_combinedSize_(const IScheduleMultiDim& table1,
                                                         const IScheduleMultiDim& table2) const {
    auto        size  = double(table1.domainSize());
    const auto& vars1 = table1.variablesSequence();
    const auto& vars2 = table2.variablesSequence();
    for (const auto ptrVar: vars2)
      if (!vars1.exists(ptrVar)) size *= double(ptrVar->domainSize());

    return size;
  }


  // returns the set of operations to perform to make the combination
  template < class TABLE >
  std::pair< std::vector< ScheduleOperator* >, const IScheduleMultiDim* >
     MultiDimCombinationDefault< TABLE >::operations(
        const std::vector< const IScheduleMultiDim* >& original_tables,
        const bool                                     is_result_persistent) const {
    // check if the set passed in argument is empty.
    const Size tabsize = original_tables.size();
    if (tabsize < 2) return {};

    // we copy the vector of tables to be combined because we will modify
    // it during the combination process
    std::vector< const IScheduleMultiDim* > tables = original_tables;

    // create the resulting set of operations to execute to perform the combination
    std::vector< ScheduleOperator* > operations;
    operations.reserve(2 * tables.size());

    // create a vector indicating whether the elements in Vector tables are
    // freshly created ScheduleMultiDim* resulting from the combination of
    // some tables or if they were added by the user into the set of tables
    // to combine
    std::vector< bool > is_t_new(tabsize, false);

    // for each pair of tables (i,j), compute the size of the table that would
    // operations from the combination of tables i and j and store the operations into a
    // priorityQueue
    std::pair< Size, Size >                          pair;
    PriorityQueue< std::pair< Size, Size >, double > queue;

    for (Size i = Size(0); i < tabsize; ++i) {
      pair.first = i;

      for (Size j = i + 1; j < tabsize; ++j) {
        pair.second = j;
        queue.insert(pair, _combinedSize_(*tables[i], *tables[j]));
      }
    }

    // keep track of the result of the last combination performed as well as of
    // the operation that created it
    const IScheduleMultiDim* resulting_table = nullptr;
    ScheduleOperator*        resulting_op    = nullptr;

    // now parse the priority queue: the top element (i,j) gives the combination
    // to perform. When the operations R has been computed,substitute i by R,
    // remove table j and recompute all the priorities of all the pairs (R,k)
    // still available.
    for (Size k = 1; k < tabsize; ++k) {
      // get the combination to perform and save it
      pair          = queue.pop();
      const Size ti = pair.first;
      const Size tj = pair.second;

      // compute the operations and free the temporary tables
      auto combination = new ScheduleBinaryCombination< TABLE, TABLE, TABLE >(
         static_cast< const ScheduleMultiDim< TABLE >& >(*tables[ti]),
         static_cast< const ScheduleMultiDim< TABLE >& >(*tables[tj]),
         _combine_);
      operations.push_back(combination);
      resulting_table = &combination->result();
      resulting_op    = combination;

      // add operations to remove the temporary tables
      if (is_t_new[ti]) {
        auto deletion = new ScheduleDeletion< TABLE >(
           static_cast< const ScheduleMultiDim< TABLE >& >(*tables[ti]));
        operations.push_back(deletion);
      }
      if (is_t_new[tj]) {
        auto deletion = new ScheduleDeletion< TABLE >(
           static_cast< const ScheduleMultiDim< TABLE >& >(*tables[tj]));
        operations.push_back(deletion);
      }

      // substitute ti by result and remove tj
      tables[ti]   = resulting_table;
      is_t_new[ti] = true;
      tables[tj]   = nullptr;

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

      // update the "combined" size of all the pairs involving ti (i.e., result)
      {
        pair.second = ti;
        for (Size ind = 0; ind < ti; ++ind) {
          if (tables[ind] != nullptr) {
            pair.first = ind;
            queue.setPriority(pair, _combinedSize_(*resulting_table, *(tables[ind])));
          }
        }

        pair.first = ti;
        for (Size ind = ti + 1; ind < tabsize; ++ind) {
          if (tables[ind] != nullptr) {
            pair.second = ind;
            queue.setPriority(pair, _combinedSize_(*resulting_table, *(tables[ind])));
          }
        }
      }
    }

    // if necessary, make the resulting table persistent
    if (is_result_persistent) { resulting_op->makeResultsPersistent(true); }

    return {operations, resulting_table};
  }


  /// returns the set of operations to perform to make the combination
  template < class TABLE >
  std::pair< std::vector< ScheduleOperator* >, const IScheduleMultiDim* >
     MultiDimCombinationDefault< TABLE >::operations(const Set< const IScheduleMultiDim* >& set,
                                                     const bool is_result_persistent) const {
    std::vector< const IScheduleMultiDim* > vect;
    vect.reserve(set.size());
    for (const auto elt: set) {
      vect.push_back(elt);
    }
    return operations(vect, is_result_persistent);
  }


  /// free scheduing memory
  template < class TABLE >
  INLINE void MultiDimCombinationDefault< TABLE >::_freeData_(
     std::vector< const IScheduleMultiDim* >& tables,
     std::vector< ScheduleOperator* >&        operations) const {
    for (auto op: operations)
      delete op;

    for (auto table: tables)
      delete table;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
