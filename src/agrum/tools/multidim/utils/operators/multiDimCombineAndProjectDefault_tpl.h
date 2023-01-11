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
 * @brief An efficient class for combining and projecting MultiDim tables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <limits>

#  include <agrum/agrum.h>

#  include <agrum/tools/multidim/utils/operators/multiDimCombineAndProjectDefault.h>

namespace gum {

  // default constructor
  template < class TABLE >
  MultiDimCombineAndProjectDefault< TABLE >::MultiDimCombineAndProjectDefault(
     TABLE (*combine)(const TABLE&, const TABLE&),
     TABLE (*project)(const TABLE&, const Set< const DiscreteVariable* >&)) :
      MultiDimCombineAndProject< TABLE >(),
      _combination_(new MultiDimCombinationDefault< TABLE >(combine)),
      _projection_(new MultiDimProjection< TABLE >(project)) {
    // for debugging purposes
    GUM_CONSTRUCTOR(MultiDimCombineAndProjectDefault);
  }

  // copy constructor
  template < class TABLE >
  MultiDimCombineAndProjectDefault< TABLE >::MultiDimCombineAndProjectDefault(
     const MultiDimCombineAndProjectDefault< TABLE >& from) :
      MultiDimCombineAndProject< TABLE >(),
      _combination_(from._combination_->clone()), _projection_(from._projection_->clone()) {
    // for debugging purposes
    GUM_CONS_CPY(MultiDimCombineAndProjectDefault);
  }

  // destructor
  template < class TABLE >
  MultiDimCombineAndProjectDefault< TABLE >::~MultiDimCombineAndProjectDefault() {
    // for debugging purposes
    GUM_DESTRUCTOR(MultiDimCombineAndProjectDefault);
    delete _combination_;
    delete _projection_;
  }

  // virtual constructor
  template < class TABLE >
  MultiDimCombineAndProjectDefault< TABLE >*
     MultiDimCombineAndProjectDefault< TABLE >::clone() const {
    return new MultiDimCombineAndProjectDefault< TABLE >(*this);
  }

  // combine and project
  template < class TABLE >
  Set< const TABLE* > MultiDimCombineAndProjectDefault< TABLE >::execute(
     const Set< const TABLE* >&            table_set,
     const Set< const DiscreteVariable* >& del_vars) {
    // create a vector with all the tables stored as multidims
    std::vector< const IScheduleMultiDim* > tables;
    tables.reserve(table_set.size());
    for (const auto table: table_set) {
      tables.push_back(new ScheduleMultiDim< TABLE >(*table, false));
    }

    // get the set of operations to perform and execute them
    auto ops_plus_res = operations(tables, del_vars, false);
    for (auto op: ops_plus_res.first) {
      op->execute();
    }

    // get the schedule multidims resulting from the computations and save them
    Set< const TABLE* > result(ops_plus_res.second.size());
    for (const auto pot: ops_plus_res.second) {
      auto& schedule_result = const_cast< ScheduleMultiDim< TABLE >& >(
         static_cast< const ScheduleMultiDim< TABLE >& >(*pot));
      auto potres = new TABLE(std::move(schedule_result.multiDim()));
      result.insert(potres);
    }

    // delete all the operations created as well as all the schedule tables
    _freeData_(tables, ops_plus_res.first);

    return result;
  }


  // changes the function used for combining two TABLES
  template < class TABLE >
  INLINE void MultiDimCombineAndProjectDefault< TABLE >::setCombinationFunction(
     TABLE (*combine)(const TABLE&, const TABLE&)) {
    _combination_->setCombinationFunction(combine);
  }

  // returns the current combination function
  template < class TABLE >
  INLINE TABLE (*MultiDimCombineAndProjectDefault< TABLE >::combinationFunction())(const TABLE&,
                                                                                   const TABLE&) {
    return _combination_->combinationFunction();
  }

  // changes the class that performs the combinations
  template < class TABLE >
  INLINE void MultiDimCombineAndProjectDefault< TABLE >::setCombinationClass(
     const MultiDimCombination< TABLE >& comb_class) {
    delete _combination_;
    _combination_ = comb_class.clone();
  }

  // changes the function used for projecting TABLES
  template < class TABLE >
  INLINE void MultiDimCombineAndProjectDefault< TABLE >::setProjectionFunction(
     TABLE (*proj)(const TABLE&, const Set< const DiscreteVariable* >&)) {
    _projection_->setProjectionFunction(proj);
  }

  // returns the current projection function
  template < class TABLE >
  INLINE TABLE (*MultiDimCombineAndProjectDefault< TABLE >::projectionFunction())(
     const TABLE&,
     const Set< const DiscreteVariable* >&) {
    return _projection_->projectionFunction();
  }

  // changes the class that performs the projections
  template < class TABLE >
  INLINE void MultiDimCombineAndProjectDefault< TABLE >::setProjectionClass(
     const MultiDimProjection< TABLE >& proj_class) {
    delete _projection_;
    _projection_ = proj_class.clone();
  }

  /** @brief returns a rough estimate of the number of operations that will be
   * performed to compute the combination */
  template < class TABLE >
  double MultiDimCombineAndProjectDefault< TABLE >::nbOperations(
     const Set< const Sequence< const DiscreteVariable* >* >& table_set,
     const Set< const DiscreteVariable* >&                    del_vars) const {
    // create a vector with all the tables stored as multidims
    std::vector< const IScheduleMultiDim* > tables;
    tables.reserve(table_set.size());
    for (const auto vars: table_set) {
      tables.push_back(new ScheduleMultiDim< TABLE >(*vars, false));
    }

    // get the set of operations to perform and compute their number of operations
    auto   ops_plus_res  = operations(tables, del_vars, false);
    double nb_operations = 0.0;
    for (auto op: ops_plus_res.first) {
      nb_operations += op->nbOperations();
    }

    // delete all the operations created as well as all the schedule tables
    _freeData_(tables, ops_plus_res.first);

    return nb_operations;
  }

  /** @brief returns a rough estimate of the number of operations that will be
   * performed to compute the combination */
  template < class TABLE >
  double MultiDimCombineAndProjectDefault< TABLE >::nbOperations(
     const Set< const TABLE* >&            set,
     const Set< const DiscreteVariable* >& del_vars) const {
    // create the set of sets of discrete variables involved in the tables
    Set< const Sequence< const DiscreteVariable* >* > var_set(set.size());

    for (const auto ptrTab: set) {
      var_set << &(ptrTab->variablesSequence());
    }

    return nbOperations(var_set, del_vars);
  }

  // returns the memory consumption used during the combinations and
  // projections
  template < class TABLE >
  std::pair< double, double > MultiDimCombineAndProjectDefault< TABLE >::memoryUsage(
     const Set< const Sequence< const DiscreteVariable* >* >& table_set,
     const Set< const DiscreteVariable* >&                    del_vars) const {
    // create a vector with all the tables stored as multidims
    std::vector< const IScheduleMultiDim* > tables;
    tables.reserve(table_set.size());
    for (const auto vars: table_set) {
      tables.push_back(new ScheduleMultiDim< TABLE >(*vars, false));
    }

    // get the set of operations to perform and compute their number of operations
    auto ops_plus_res = operations(tables, del_vars, false);

    // the resulting memory consumtions
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

  // returns the memory consumption used during the combinations and
  // projections
  template < class TABLE >
  std::pair< double, double > MultiDimCombineAndProjectDefault< TABLE >::memoryUsage(
     const Set< const TABLE* >&            set,
     const Set< const DiscreteVariable* >& del_vars) const {
    // create the set of sets of discrete variables involved in the tables
    Set< const Sequence< const DiscreteVariable* >* > var_set(set.size());

    for (const auto ptrTab: set) {
      var_set << &(ptrTab->variablesSequence());
    }

    return memoryUsage(var_set, del_vars);
  }


  /// returns the set of operations to perform to make all the combinations
  /// and projections
  template < class TABLE >
  std::pair< std::vector< ScheduleOperator* >, Set< const IScheduleMultiDim* > >
     MultiDimCombineAndProjectDefault< TABLE >::operations(
        const std::vector< const IScheduleMultiDim* >& original_tables,
        const Set< const DiscreteVariable* >&          del_vars,
        const bool                                     is_result_persistent) const {
    Set< const IScheduleMultiDim* > tables_set(original_tables.size());
    for (const auto table: original_tables) {
      tables_set.insert(table);
    }
    return operations(tables_set, del_vars, is_result_persistent);
  }


  /// returns the set of operations to perform to make all the combinations
  /// and projections
  template < class TABLE >
  std::pair< std::vector< ScheduleOperator* >, Set< const IScheduleMultiDim* > >
     MultiDimCombineAndProjectDefault< TABLE >::operations(
        const Set< const IScheduleMultiDim* >& original_tables,
        const Set< const DiscreteVariable* >&  original_del_vars,
        const bool                             is_result_persistent) const {
    // check if we need to combine and/or project something
    const Size tabsize = original_tables.size();
    if (tabsize < 2) {
      if (tabsize == 1) {
        auto res = _projection_->operations(*original_tables.begin(), original_del_vars);
        return std::pair< std::vector< ScheduleOperator* >, Set< const IScheduleMultiDim* > >(
           {res.first},
           {res.second});
      } else {
        GUM_ERROR(OperationNotAllowed,
                  "MultiDimCombineAndProject need at least one table to "
                  "have some work to do.");
      }
    }

    // we copy the set of tables to be combined and the set of variables to
    // delete because we will modify them during the combination/projection process
    Set< const IScheduleMultiDim* > tables   = original_tables;
    Set< const DiscreteVariable* >  del_vars = original_del_vars;

    // when we remove a variable, we need to combine all the tables containing
    // this variable in order to produce a new unique table containing this
    // variable. Removing a variable is then performed by marginalizing it out of
    // the table. In the combineAndProjectDefault algorithm, we wish to remove
    // first variables that would produce small tables. This should speed up the
    // whole marginalizing process.

    Size nb_vars;
    {
      // determine the set of all the variables involved in the tables.
      // this should help sizing correctly the hashtables used hereafter
      Set< const DiscreteVariable* > all_vars;

      for (const auto table: tables) {
        for (const auto ptrVar: table->variablesSequence()) {
          all_vars.insert(ptrVar);
        }
      }

      nb_vars = all_vars.size();
    }

    // the tables containing a given variable
    HashTable< const DiscreteVariable*, Set< const IScheduleMultiDim* > > tables_per_var(nb_vars);

    // for a given variable X to be deleted, the list of all the variables of
    // the tables containing X (actually, we also count the number of tables
    // containing the variable. This is more efficient for computing and
    // updating the product_size priority queue (see below) when some tables
    // are removed)
    HashTable< const DiscreteVariable*, HashTable< const DiscreteVariable*, unsigned int > >
       clique_vars_per_var(nb_vars);

    // initialize clique_vars_per_var and tables_per_var
    {
      Set< const IScheduleMultiDim* >                    empty_set(tables.size());
      HashTable< const DiscreteVariable*, unsigned int > empty_hash(nb_vars);

      for (const auto ptrVar: del_vars) {
        tables_per_var.insert(ptrVar, empty_set);
        clique_vars_per_var.insert(ptrVar, empty_hash);
      }

      // update properly tables_per_var and clique_vars_per_var
      for (const auto ptrTab: tables) {
        const auto& vars = ptrTab->variablesSequence();

        for (const auto ptrVar: vars) {
          if (del_vars.contains(ptrVar)) {
            // add the table to the set of tables related to vars[i]
            tables_per_var[ptrVar].insert(ptrTab);

            // add the variables of the table to clique_vars_per_var[vars[i]]
            auto& comb_vars = clique_vars_per_var[ptrVar];
            for (const auto xptrVar: vars) {
              try {
                ++comb_vars[xptrVar];
              } catch (const NotFound&) { comb_vars.insert(xptrVar, 1); }
            }
          }
        }
      }
    }

    // create the set of operations to execute to perform the combinations and
    // projections
    std::vector< ScheduleOperator* > ops;
    ops.reserve(2 * tables.size() + del_vars.size());

    // keep track of the operations that created new tables. This is useful
    // when requiring that results are persistent
    HashTable< const IScheduleMultiDim*, ScheduleOperator* > multidim2op(tables.size());

    // the sizes of the tables produced when removing a given discrete variable
    PriorityQueue< const DiscreteVariable*, double > product_size;

    // initialize properly product_size
    for (const auto& elt: clique_vars_per_var) {
      double      size     = 1.0;
      const auto  ptrVar   = elt.first;
      const auto& hashvars = elt.second;   // HashTable<DiscreteVariable*, int>

      if (!hashvars.empty()) {
        for (const auto& xelt: hashvars) {
          size *= (double)xelt.first->domainSize();
        }

        product_size.insert(ptrVar, size);
      }
    }

    // now, remove all the variables in del_vars, starting from those that
    // produce the smallest tables
    while (!product_size.empty()) {
      // get the best variable to remove
      const DiscreteVariable* del_var = product_size.pop();
      del_vars.erase(del_var);

      // get the set of tables to combine
      auto& tables_to_combine = tables_per_var[del_var];

      // if there is no tables to combine, do nothing
      if (tables_to_combine.empty()) continue;

      // compute the combination of all the tables: if there is only one table,
      // there is nothing to do, else we shall use the MultiDimCombination
      // to perform the combination
      const IScheduleMultiDim* joint = nullptr;
      bool                     joint_to_delete;
      if (tables_to_combine.size() == 1) {
        joint           = *(tables_to_combine.begin());
        joint_to_delete = false;
      } else {
        // get the operations to perform to make the combination as well as
        // the result of the combination
        auto comb_ops = _combination_->operations(tables_to_combine);
        ops.insert(ops.cend(), comb_ops.first.begin(), comb_ops.first.end());
        joint           = comb_ops.second;
        joint_to_delete = true;
      }

      // compute the table resulting from marginalizing out del_var from joint
      // and add the projection to the set of operations. Here, we know that the
      // joint contains del_var, hence there is a nonempty projection to perform
      Set< const DiscreteVariable* > del_one_var;
      del_one_var << del_var;
      auto proj_ops = _projection_->operations(joint, del_one_var);
      ops.push_back(proj_ops.first);
      const IScheduleMultiDim* marginal = proj_ops.second;
      if (is_result_persistent) multidim2op.insert(marginal, proj_ops.first);

      // remove the temporary joint if needed
      if (joint_to_delete) {
        auto deletion
           = new ScheduleDeletion< TABLE >(static_cast< const ScheduleMultiDim< TABLE >& >(*joint));
        ops.push_back(deletion);
      }

      // update clique_vars_per_var : remove the variables of the tables we
      // combined from this hashtable
      // update accordingly tables_per_vars : remove these tables
      // update accordingly product_size : when a variable is no more used by
      // any table, divide product_size by its domain size
      for (const auto ptrTab: tables_to_combine) {
        const auto& table_vars    = ptrTab->variablesSequence();
        const Size  tab_vars_size = table_vars.size();

        for (Size i = 0; i < tab_vars_size; ++i) {
          if (del_vars.contains(table_vars[i])) {
            // here we have a variable that needed to be removed => update
            // product_size, tables_per_var and clique_vars_per_var: here,
            // the update corresponds to removing table PtrTab
            auto&  table_vars_of_var_i = clique_vars_per_var[table_vars[i]];
            double div_size            = 1.0;

            for (Size j = 0; j < tab_vars_size; ++j) {
              unsigned int k = --table_vars_of_var_i[table_vars[j]];

              if (k == 0) {
                div_size *= table_vars[j]->domainSize();
                table_vars_of_var_i.erase(table_vars[j]);
              }
            }

            tables_per_var[table_vars[i]].erase(ptrTab);

            if (div_size != 1.0) {
              product_size.setPriority(table_vars[i],
                                       product_size.priority(table_vars[i]) / div_size);
            }
          }
        }

        // if ptrTab is a table resulting from preceding combinations/projections,
        // it is temporary and, therefore, it should be deleted
        if (!original_tables.contains(ptrTab)) {
          auto deletion = new ScheduleDeletion< TABLE >(
             static_cast< const ScheduleMultiDim< TABLE >& >(*ptrTab));
          ops.push_back(deletion);
        }

        tables.erase(ptrTab);
      }

      tables_per_var.erase(del_var);

      // add the new projected marginal to the list of tables
      const auto& marginal_vars = marginal->variablesSequence();
      for (const auto mvar: marginal_vars) {
        if (del_vars.contains(mvar)) {
          // add the new marginal table to the set of tables of mvar
          tables_per_var[mvar].insert(marginal);

          // add the variables of the table to clique_vars_per_var[mvar]
          auto&  iter_vars = clique_vars_per_var[mvar];
          double mult_size = 1.0;
          for (const auto var: marginal_vars) {
            try {
              ++iter_vars[var];
            } catch (const NotFound&) {
              iter_vars.insert(var, 1);
              mult_size *= (double)var->domainSize();
            }
          }

          if (mult_size != 1.0) {
            product_size.setPriority(mvar, product_size.priority(mvar) * mult_size);
          }
        }
      }

      tables.insert(marginal);
    }

    // here, Set "tables" contains the list of the tables resulting from
    // marginalizing out of del_vars of the combination of the tables
    // of original_tables. Note in particular that it will contain all the
    // potentials with no dimension (constants)

    // if we require persistent results, update the operations that produced some
    // of the tables in Set "tables"
    if (is_result_persistent) {
      for (const auto table: tables) {
        if (multidim2op.exists(table)) multidim2op[table]->makeResultsPersistent(true);
      }
    }

    return {ops, tables};
  }


  /// free scheduing memory
  template < class TABLE >
  INLINE void MultiDimCombineAndProjectDefault< TABLE >::_freeData_(
     std::vector< const IScheduleMultiDim* >& tables,
     std::vector< ScheduleOperator* >&        operations) const {
    for (auto op: operations)
      delete op;

    for (auto table: tables)
      delete table;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
