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
 * @brief An efficient class for combining and projecting MultiDim tables
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <limits>

#  include <agrum/agrum.h>

#  include <agrum/tools/multidim/utils/operators/multiDimCombineAndProjectDefault.h>

namespace gum {

  // default constructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimCombineAndProjectDefault< GUM_SCALAR, TABLE >::
     MultiDimCombineAndProjectDefault(
        TABLE< GUM_SCALAR >* (*combine)(const TABLE< GUM_SCALAR >&,
                                        const TABLE< GUM_SCALAR >&),
        TABLE< GUM_SCALAR >* (*project)(const TABLE< GUM_SCALAR >&,
                                        const Set< const DiscreteVariable* >&)) :
      MultiDimCombineAndProject< GUM_SCALAR, TABLE >(),
      combination__(new MultiDimCombinationDefault< GUM_SCALAR, TABLE >(combine)),
      projection__(new MultiDimProjection< GUM_SCALAR, TABLE >(project)) {
    // for debugging purposes
    GUM_CONSTRUCTOR(MultiDimCombineAndProjectDefault);
  }

  // copy constructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimCombineAndProjectDefault< GUM_SCALAR, TABLE >::
     MultiDimCombineAndProjectDefault(
        const MultiDimCombineAndProjectDefault< GUM_SCALAR, TABLE >& from) :
      MultiDimCombineAndProject< GUM_SCALAR, TABLE >(),
      combination__(from.combination__->newFactory()),
      projection__(from.projection__->newFactory()) {
    // for debugging purposes
    GUM_CONS_CPY(MultiDimCombineAndProjectDefault);
  }

  // destructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimCombineAndProjectDefault< GUM_SCALAR,
                                    TABLE >::~MultiDimCombineAndProjectDefault() {
    // for debugging purposes
    GUM_DESTRUCTOR(MultiDimCombineAndProjectDefault);
    delete combination__;
    delete projection__;
  }

  // virtual constructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimCombineAndProjectDefault< GUM_SCALAR, TABLE >*
     MultiDimCombineAndProjectDefault< GUM_SCALAR, TABLE >::newFactory() const {
    return new MultiDimCombineAndProjectDefault< GUM_SCALAR, TABLE >(*this);
  }

  // combine and project
  template < typename GUM_SCALAR, template < typename > class TABLE >
  Set< const TABLE< GUM_SCALAR >* >
     MultiDimCombineAndProjectDefault< GUM_SCALAR, TABLE >::combineAndProject(
        Set< const TABLE< GUM_SCALAR >* > table_set,
        Set< const DiscreteVariable* >    del_vars) {
    // when we remove a variable, we need to combine all the tables containing
    // this variable in order to produce a new unique table containing this
    // variable. Removing the variable is then performed by marginalizing it
    // out of the table. In the combineAndProject algorithm, we wish to remove
    // first variables that produce small tables. This should speed up the
    // marginalizing process
    Size nb_vars;
    {
      // determine the set of all the variables involved in the tables.
      // this should help sizing correctly the hashtables
      Set< const DiscreteVariable* > all_vars;

      for (const auto ptrTab: table_set) {
        for (const auto ptrVar: ptrTab->variablesSequence()) {
          all_vars.insert(ptrVar);
        }
      }

      nb_vars = all_vars.size();
    }

    // the tables containing a given variable to be deleted
    HashTable< const DiscreteVariable*, Set< const TABLE< GUM_SCALAR >* > >
       tables_per_var(nb_vars);

    // for a given variable X to be deleted, the list of all the variables of
    // the tables containing X (actually, we also count the number of tables
    // containing the variable. This is more efficient for computing and
    // updating the product_size priority queue (see below) when some tables
    // are removed)
    HashTable< const DiscreteVariable*,
               HashTable< const DiscreteVariable*, unsigned int > >
       tables_vars_per_var(nb_vars);

    // initialize tables_vars_per_var and tables_per_var
    {
      Set< const TABLE< GUM_SCALAR >* > empty_set(table_set.size());
      HashTable< const DiscreteVariable*, unsigned int > empty_hash(nb_vars);

      for (const auto ptrVar: del_vars) {
        tables_per_var.insert(ptrVar, empty_set);
        tables_vars_per_var.insert(ptrVar, empty_hash);
      }

      // update properly tables_per_var and tables_vars_per_var
      for (const auto ptrTab: table_set) {
        const Sequence< const DiscreteVariable* >& vars
           = ptrTab->variablesSequence();

        for (const auto ptrVar: vars) {
          if (del_vars.contains(ptrVar)) {
            // add the table to the set of tables related to vars[i]
            tables_per_var[ptrVar].insert(ptrTab);

            // add the variables of the table to tables_vars_per_var[vars[i]]
            HashTable< const DiscreteVariable*, unsigned int >& iter_vars
               = tables_vars_per_var[ptrVar];

            for (const auto xptrVar: vars) {
              try {
                ++iter_vars[xptrVar];
              } catch (const NotFound&) { iter_vars.insert(xptrVar, 1); }
            }
          }
        }
      }
    }

    // the sizes of the tables produced when removing a given discrete variable
    PriorityQueue< const DiscreteVariable*, double > product_size;

    // initialize properly product_size
    for (const auto& elt: tables_vars_per_var) {
      double      size     = 1.0;
      const auto  ptrVar   = elt.first;
      const auto& hashvars = elt.second;   // HashTable<DiscreteVariable*, int>

      if (hashvars.size()) {
        for (const auto& xelt: hashvars) {
          size *= (double)xelt.first->domainSize();
        }

        product_size.insert(ptrVar, size);
      }
    }

    // create a set of the temporary tables created during the
    // marginalization process (useful for deallocating temporary tables)
    Set< const TABLE< GUM_SCALAR >* > tmp_marginals(table_set.size());

    // now, remove all the variables in del_vars, starting from those that
    // produce the smallest tables
    while (!product_size.empty()) {
      // get the best variable to remove
      const DiscreteVariable* del_var = product_size.pop();
      del_vars.erase(del_var);

      // get the set of tables to combine
      Set< const TABLE< GUM_SCALAR >* >& tables_to_combine
         = tables_per_var[del_var];

      // if there is no tables to combine, do nothing
      if (tables_to_combine.size() == 0) continue;

      // compute the combination of all the tables: if there is only one table,
      // there is nothing to do, else we shall use the MultiDimCombination
      // to perform the combination
      TABLE< GUM_SCALAR >* joint;

      bool joint_to_delete = false;

      if (tables_to_combine.size() == 1) {
        joint = const_cast< TABLE< GUM_SCALAR >* >(*(tables_to_combine.begin()));
        joint_to_delete = false;
      } else {
        joint           = combination__->combine(tables_to_combine);
        joint_to_delete = true;
      }

      // compute the table resulting from marginalizing out del_var from joint
      Set< const DiscreteVariable* > del_one_var;
      del_one_var << del_var;

      TABLE< GUM_SCALAR >* marginal = projection__->project(*joint, del_one_var);

      // remove the temporary joint if needed
      if (joint_to_delete) delete joint;

      // update tables_vars_per_var : remove the variables of the TABLEs we
      // combined from this hashtable
      // update accordingly tables_per_vars : remove these TABLEs
      // update accordingly product_size : when a variable is no more used by
      // any TABLE, divide product_size by its domain size

      for (const auto ptrTab: tables_to_combine) {
        const Sequence< const DiscreteVariable* >& table_vars
           = ptrTab->variablesSequence();
        const Size tab_vars_size = table_vars.size();

        for (Size i = 0; i < tab_vars_size; ++i) {
          if (del_vars.contains(table_vars[i])) {
            // ok, here we have a variable that needed to be removed => update
            // product_size, tables_per_var and tables_vars_per_var: here,
            // the update corresponds to removing table PtrTab
            HashTable< const DiscreteVariable*, unsigned int >& table_vars_of_var_i
               = tables_vars_per_var[table_vars[i]];
            double div_size = 1.0;

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
                                       product_size.priority(table_vars[i])
                                          / div_size);
            }
          }
        }

        if (tmp_marginals.contains(ptrTab)) {
          delete ptrTab;
          tmp_marginals.erase(ptrTab);
        }

        table_set.erase(ptrTab);
      }

      tables_per_var.erase(del_var);

      // add the new projected marginal to the list of TABLES
      const Sequence< const DiscreteVariable* >& marginal_vars
         = marginal->variablesSequence();

      for (const auto mvar: marginal_vars) {
        if (del_vars.contains(mvar)) {
          // add the new marginal table to the set of tables of mvar
          tables_per_var[mvar].insert(marginal);

          // add the variables of the table to tables_vars_per_var[mvar]
          HashTable< const DiscreteVariable*, unsigned int >& iter_vars
             = tables_vars_per_var[mvar];
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
            product_size.setPriority(mvar,
                                     product_size.priority(mvar) * mult_size);
          }
        }
      }

      table_set.insert(marginal);
      tmp_marginals.insert(marginal);
    }

    // here, tmp_marginals contains all the newly created tables and
    // table_set contains the list of the tables resulting from the
    // marginalizing out of del_vars of the combination of the tables
    // of table_set. Note in particular that it will contain all the
    // potentials with no dimension (constants)
    return table_set;
  }

  // changes the function used for combining two TABLES
  template < typename GUM_SCALAR, template < typename > class TABLE >
  INLINE void
     MultiDimCombineAndProjectDefault< GUM_SCALAR, TABLE >::setCombineFunction(
        TABLE< GUM_SCALAR >* (*combine)(const TABLE< GUM_SCALAR >&,
                                        const TABLE< GUM_SCALAR >&)) {
    combination__->setCombineFunction(combine);
  }

  // returns the current combination function
  template < typename GUM_SCALAR, template < typename > class TABLE >
  INLINE TABLE< GUM_SCALAR >* (
     *MultiDimCombineAndProjectDefault< GUM_SCALAR, TABLE >::combineFunction())(
     const TABLE< GUM_SCALAR >&,
     const TABLE< GUM_SCALAR >&) {
    return combination__->combineFunction();
  }

  // changes the class that performs the combinations
  template < typename GUM_SCALAR, template < typename > class TABLE >
  INLINE void
     MultiDimCombineAndProjectDefault< GUM_SCALAR, TABLE >::setCombinationClass(
        const MultiDimCombination< GUM_SCALAR, TABLE >& comb_class) {
    delete combination__;
    combination__ = comb_class.newFactory();
  }

  // changes the function used for projecting TABLES
  template < typename GUM_SCALAR, template < typename > class TABLE >
  INLINE void
     MultiDimCombineAndProjectDefault< GUM_SCALAR, TABLE >::setProjectFunction(
        TABLE< GUM_SCALAR >* (*proj)(const TABLE< GUM_SCALAR >&,
                                     const Set< const DiscreteVariable* >&)) {
    projection__->setProjectFunction(proj);
  }

  // returns the current projection function
  template < typename GUM_SCALAR, template < typename > class TABLE >
  INLINE TABLE< GUM_SCALAR >* (
     *MultiDimCombineAndProjectDefault< GUM_SCALAR, TABLE >::projectFunction())(
     const TABLE< GUM_SCALAR >&,
     const Set< const DiscreteVariable* >&) {
    return projection__->projectFunction();
  }

  // changes the class that performs the projections
  template < typename GUM_SCALAR, template < typename > class TABLE >
  INLINE void
     MultiDimCombineAndProjectDefault< GUM_SCALAR, TABLE >::setProjectionClass(
        const MultiDimProjection< GUM_SCALAR, TABLE >& proj_class) {
    delete projection__;
    projection__ = proj_class.newFactory();
  }

  /** @brief returns a rough estimate of the number of operations that will be
   * performed to compute the combination */
  template < typename GUM_SCALAR, template < typename > class TABLE >
  float MultiDimCombineAndProjectDefault< GUM_SCALAR, TABLE >::nbOperations(
     const Set< const Sequence< const DiscreteVariable* >* >& table_set,
     Set< const DiscreteVariable* >                           del_vars) const {
    // when we remove a variable, we need to combine all the tables containing
    // this variable in order to produce a new unique table containing this
    // variable. Here, we do not have the tables but only their variables
    // (dimensions), but the principle is identical. Removing a variable is then
    // performed by marginalizing it out of the table or, equivalently, to
    // remove it from the table's list of variables. In the
    // combineAndProjectDefault algorithm, we wish to remove first variables
    // that would produce small tables. This should speed up the whole
    // marginalizing process.

    Size nb_vars;
    {
      // determine the set of all the variables involved in the tables.
      // this should help sizing correctly the hashtables
      Set< const DiscreteVariable* > all_vars;

      for (const auto ptrSeq: table_set) {
        for (const auto ptrVar: *ptrSeq) {
          all_vars.insert(ptrVar);
        }
      }

      nb_vars = all_vars.size();
    }

    // the tables (actually their variables) containing a given variable
    // to be deleted
    HashTable< const DiscreteVariable*,
               Set< const Sequence< const DiscreteVariable* >* > >
       tables_per_var(nb_vars);

    // for a given variable X to be deleted, the list of all the variables of
    // the tables containing X (actually, we count the number of tables
    // containing the variable. This is more efficient for computing and
    // updating the product_size priority queue (see below) when some tables
    // are removed)
    HashTable< const DiscreteVariable*,
               HashTable< const DiscreteVariable*, unsigned int > >
       tables_vars_per_var(nb_vars);

    // initialize tables_vars_per_var and tables_per_var
    {
      Set< const Sequence< const DiscreteVariable* >* > empty_set(
         table_set.size());
      HashTable< const DiscreteVariable*, unsigned int > empty_hash(nb_vars);

      for (const auto ptrVar: del_vars) {
        tables_per_var.insert(ptrVar, empty_set);
        tables_vars_per_var.insert(ptrVar, empty_hash);
      }

      // update properly tables_per_var and tables_vars_per_var
      for (const auto ptrSeq: table_set) {
        const Sequence< const DiscreteVariable* >& vars = *ptrSeq;

        for (const auto ptrVar: vars) {
          if (del_vars.contains(ptrVar)) {
            // add the table's variables to the set of those related to ptrVar
            tables_per_var[ptrVar].insert(ptrSeq);

            // add the variables of the table to tables_vars_per_var[ptrVar]
            HashTable< const DiscreteVariable*, unsigned int >& iter_vars
               = tables_vars_per_var[ptrVar];

            for (const auto xptrVar: vars) {
              try {
                ++iter_vars[xptrVar];
              } catch (const NotFound&) { iter_vars.insert(xptrVar, 1); }
            }
          }
        }
      }
    }

    // the sizes of the tables produced when removing a given discrete variable
    PriorityQueue< const DiscreteVariable*, double > product_size;

    // initialize properly product_size
    for (const auto& elt: tables_vars_per_var) {
      double     size     = 1.0;
      const auto ptrVar   = elt.first;
      const auto hashvars = elt.second;   // HashTable<DiscreteVariable*, int>

      if (hashvars.size()) {
        for (const auto& xelt: hashvars) {
          size *= (double)xelt.first->domainSize();
        }

        product_size.insert(ptrVar, size);
      }
    }

    // the resulting number of operations
    float nb_operations = 0;

    // create a set of the temporary table's variables created during the
    // marginalization process (useful for deallocating temporary tables)
    Set< const Sequence< const DiscreteVariable* >* > tmp_marginals(
       table_set.size());

    // now, remove all the variables in del_vars, starting from those that
    // produce the smallest tables
    while (!product_size.empty()) {
      // get the best variable to remove
      const DiscreteVariable* del_var = product_size.pop();
      del_vars.erase(del_var);

      // get the set of tables to combine
      Set< const Sequence< const DiscreteVariable* >* >& tables_to_combine
         = tables_per_var[del_var];

      // if there is no tables to combine, do nothing
      if (tables_to_combine.size() == 0) continue;

      // compute the combination of all the tables: if there is only one table,
      // there is nothing to do, else we shall use the MultiDimCombination
      // to perform the combination
      Sequence< const DiscreteVariable* >* joint;

      bool joint_to_delete = false;

      if (tables_to_combine.size() == 1) {
        joint = const_cast< Sequence< const DiscreteVariable* >* >(
           *(tables_to_combine.beginSafe()));
        joint_to_delete = false;
      } else {
        // here, compute the union of all the variables of the tables to combine
        joint = new Sequence< const DiscreteVariable* >;

        for (const auto ptrSeq: tables_to_combine) {
          for (const auto ptrVar: *ptrSeq) {
            if (!joint->exists(ptrVar)) { joint->insert(ptrVar); }
          }
        }

        joint_to_delete = true;

        // update the number of operations performed
        nb_operations += combination__->nbOperations(tables_to_combine);
      }

      // update the number of operations performed by marginalizing out del_var
      Set< const DiscreteVariable* > del_one_var;
      del_one_var << del_var;

      nb_operations += projection__->nbOperations(*joint, del_one_var);

      // compute the table resulting from marginalizing out del_var from joint
      Sequence< const DiscreteVariable* >* marginal;

      if (joint_to_delete) {
        marginal = joint;
      } else {
        marginal = new Sequence< const DiscreteVariable* >(*joint);
      }

      marginal->erase(del_var);

      // update tables_vars_per_var : remove the variables of the TABLEs we
      // combined from this hashtable
      // update accordingly tables_per_vars : remove these TABLEs
      // update accordingly product_size : when a variable is no more used by
      // any TABLE, divide product_size by its domain size

      for (const auto ptrSeq: tables_to_combine) {
        const Sequence< const DiscreteVariable* >& table_vars = *ptrSeq;
        const Size tab_vars_size                              = table_vars.size();

        for (Size i = 0; i < tab_vars_size; ++i) {
          if (del_vars.contains(table_vars[i])) {
            // ok, here we have a variable that needed to be removed => update
            // product_size, tables_per_var and tables_vars_per_var
            HashTable< const DiscreteVariable*, unsigned int >& table_vars_of_var_i
               = tables_vars_per_var[table_vars[i]];
            double div_size = 1.0;

            for (Size j = 0; j < tab_vars_size; ++j) {
              unsigned int k = --table_vars_of_var_i[table_vars[j]];

              if (k == 0) {
                div_size *= table_vars[j]->domainSize();
                table_vars_of_var_i.erase(table_vars[j]);
              }
            }

            tables_per_var[table_vars[i]].erase(ptrSeq);

            if (div_size != 1.0) {
              product_size.setPriority(table_vars[i],
                                       product_size.priority(table_vars[i])
                                          / div_size);
            }
          }
        }

        if (tmp_marginals.contains(ptrSeq)) {
          delete ptrSeq;
          tmp_marginals.erase(ptrSeq);
        }
      }

      tables_per_var.erase(del_var);

      // add the new projected marginal to the list of TABLES
      for (const auto mvar: *marginal) {
        if (del_vars.contains(mvar)) {
          // add the new marginal table to the set of tables of var i
          tables_per_var[mvar].insert(marginal);

          // add the variables of the table to tables_vars_per_var[vars[i]]
          HashTable< const DiscreteVariable*, unsigned int >& iter_vars
             = tables_vars_per_var[mvar];
          double mult_size = 1.0;

          for (const auto var: *marginal) {
            try {
              ++iter_vars[var];
            } catch (const NotFound&) {
              iter_vars.insert(var, 1);
              mult_size *= (double)var->domainSize();
            }
          }

          if (mult_size != 1.0) {
            product_size.setPriority(mvar,
                                     product_size.priority(mvar) * mult_size);
          }
        }
      }

      tmp_marginals.insert(marginal);
    }

    // here, tmp_marginals contains all the newly created tables
    for (auto iter = tmp_marginals.beginSafe(); iter != tmp_marginals.endSafe();
         ++iter) {
      delete *iter;
    }

    return nb_operations;
  }

  /** @brief returns a rough estimate of the number of operations that will be
   * performed to compute the combination */
  template < typename GUM_SCALAR, template < typename > class TABLE >
  float MultiDimCombineAndProjectDefault< GUM_SCALAR, TABLE >::nbOperations(
     const Set< const TABLE< GUM_SCALAR >* >& set,
     const Set< const DiscreteVariable* >&    del_vars) const {
    // create the set of sets of discrete variables involved in the tables
    Set< const Sequence< const DiscreteVariable* >* > var_set(set.size());

    for (const auto ptrTab: set) {
      var_set << &(ptrTab->variablesSequence());
    }

    return nbOperations(var_set, del_vars);
  }

  // returns the memory consumption used during the combinations and
  // projections
  template < typename GUM_SCALAR, template < typename > class TABLE >
  std::pair< long, long >
     MultiDimCombineAndProjectDefault< GUM_SCALAR, TABLE >::memoryUsage(
        const Set< const Sequence< const DiscreteVariable* >* >& table_set,
        Set< const DiscreteVariable* >                           del_vars) const {
    // when we remove a variable, we need to combine all the tables containing
    // this variable in order to produce a new unique table containing this
    // variable. Here, we do not have the tables but only their variables
    // (dimensions), but the principle is identical. Removing a variable is then
    // performed by marginalizing it out of the table or, equivalently, to
    // remove it from the table's list of variables. In the
    // combineAndProjectDefault algorithm, we wish to remove first variables
    // that would produce small tables. This should speed up the whole
    // marginalizing process.

    Size nb_vars;
    {
      // determine the set of all the variables involved in the tables.
      // this should help sizing correctly the hashtables
      Set< const DiscreteVariable* > all_vars;

      for (const auto ptrSeq: table_set) {
        for (const auto ptrVar: *ptrSeq) {
          all_vars.insert(ptrVar);
        }
      }

      nb_vars = all_vars.size();
    }

    // the tables (actually their variables) containing a given variable
    HashTable< const DiscreteVariable*,
               Set< const Sequence< const DiscreteVariable* >* > >
       tables_per_var(nb_vars);
    // for a given variable X to be deleted, the list of all the variables of
    // the tables containing X (actually, we count the number of tables
    // containing the variable. This is more efficient for computing and
    // updating the product_size priority queue (see below) when some tables
    // are removed)
    HashTable< const DiscreteVariable*,
               HashTable< const DiscreteVariable*, unsigned int > >
       tables_vars_per_var(nb_vars);

    // initialize tables_vars_per_var and tables_per_var
    {
      Set< const Sequence< const DiscreteVariable* >* > empty_set(
         table_set.size());
      HashTable< const DiscreteVariable*, unsigned int > empty_hash(nb_vars);

      for (const auto ptrVar: del_vars) {
        tables_per_var.insert(ptrVar, empty_set);
        tables_vars_per_var.insert(ptrVar, empty_hash);
      }

      // update properly tables_per_var and tables_vars_per_var
      for (const auto ptrSeq: table_set) {
        const Sequence< const DiscreteVariable* >& vars = *ptrSeq;

        for (const auto ptrVar: vars) {
          if (del_vars.contains(ptrVar)) {
            // add the table's variables to the set of those related to ptrVar
            tables_per_var[ptrVar].insert(ptrSeq);

            // add the variables of the table to tables_vars_per_var[ptrVar]
            HashTable< const DiscreteVariable*, unsigned int >& iter_vars
               = tables_vars_per_var[ptrVar];

            for (const auto xptrVar: vars) {
              try {
                ++iter_vars[xptrVar];
              } catch (const NotFound&) { iter_vars.insert(xptrVar, 1); }
            }
          }
        }
      }
    }

    // the sizes of the tables produced when removing a given discrete variable
    PriorityQueue< const DiscreteVariable*, double > product_size;

    // initialize properly product_size
    for (const auto& elt: tables_vars_per_var) {
      double     size     = 1.0;
      const auto ptrVar   = elt.first;
      const auto hashvars = elt.second;   // HashTable<DiscreteVariable*, int>

      if (hashvars.size()) {
        for (const auto& xelt: hashvars) {
          size *= (double)xelt.first->domainSize();
        }

        product_size.insert(ptrVar, size);
      }
    }

    // the resulting memory consumtions
    long max_memory     = 0;
    long current_memory = 0;

    // create a set of the temporary table's variables created during the
    // marginalization process (useful for deallocating temporary tables)
    Set< const Sequence< const DiscreteVariable* >* > tmp_marginals(
       table_set.size());

    // now, remove all the variables in del_vars, starting from those that
    // produce
    // the smallest tables
    while (!product_size.empty()) {
      // get the best variable to remove
      const DiscreteVariable* del_var = product_size.pop();
      del_vars.erase(del_var);

      // get the set of tables to combine
      Set< const Sequence< const DiscreteVariable* >* >& tables_to_combine
         = tables_per_var[del_var];

      // if there is no tables to combine, do nothing
      if (tables_to_combine.size() == 0) continue;

      // compute the combination of all the tables: if there is only one table,
      // there is nothing to do, else we shall use the MultiDimCombination
      // to perform the combination
      Sequence< const DiscreteVariable* >* joint;

      bool joint_to_delete = false;

      if (tables_to_combine.size() == 1) {
        joint = const_cast< Sequence< const DiscreteVariable* >* >(
           *(tables_to_combine.beginSafe()));
        joint_to_delete = false;
      } else {
        // here, compute the union of all the variables of the tables to combine
        joint = new Sequence< const DiscreteVariable* >;

        for (const auto ptrSeq: tables_to_combine) {
          for (const auto ptrVar: *ptrSeq) {
            if (!joint->exists(ptrVar)) { joint->insert(ptrVar); }
          }
        }

        joint_to_delete = true;

        // update the number of operations performed
        std::pair< long, long > comb_memory
           = combination__->memoryUsage(tables_to_combine);

        if ((std::numeric_limits< long >::max() - current_memory
             < comb_memory.first)
            || (std::numeric_limits< long >::max() - current_memory
                < comb_memory.second)) {
          GUM_ERROR(OutOfBounds, "memory usage out of long int range");
        }

        if (current_memory + comb_memory.first > max_memory) {
          max_memory = current_memory + comb_memory.first;
        }

        current_memory += comb_memory.second;
      }

      // update the number of operations performed by marginalizing out del_var
      Set< const DiscreteVariable* > del_one_var;
      del_one_var << del_var;

      std::pair< long, long > comb_memory
         = projection__->memoryUsage(*joint, del_one_var);

      if ((std::numeric_limits< long >::max() - current_memory < comb_memory.first)
          || (std::numeric_limits< long >::max() - current_memory
              < comb_memory.second)) {
        GUM_ERROR(OutOfBounds, "memory usage out of long int range");
      }

      if (current_memory + comb_memory.first > max_memory) {
        max_memory = current_memory + comb_memory.first;
      }

      current_memory += comb_memory.second;

      // compute the table resulting from marginalizing out del_var from joint
      Sequence< const DiscreteVariable* >* marginal;

      if (joint_to_delete) {
        marginal = joint;
      } else {
        marginal = new Sequence< const DiscreteVariable* >(*joint);
      }

      marginal->erase(del_var);

      // update tables_vars_per_var : remove the variables of the TABLEs we
      // combined from this hashtable
      // update accordingly tables_per_vars : remove these TABLEs
      // update accordingly product_size : when a variable is no more used by
      // any TABLE, divide product_size by its domain size

      for (const auto ptrSeq: tables_to_combine) {
        const Sequence< const DiscreteVariable* >& table_vars = *ptrSeq;
        const Size tab_vars_size                              = table_vars.size();

        for (Size i = 0; i < tab_vars_size; ++i) {
          if (del_vars.contains(table_vars[i])) {
            // ok, here we have a variable that needed to be removed => update
            // product_size, tables_per_var and tables_vars_per_var
            HashTable< const DiscreteVariable*, unsigned int >& table_vars_of_var_i
               = tables_vars_per_var[table_vars[i]];
            double div_size = 1.0;

            for (Size j = 0; j < tab_vars_size; ++j) {
              Size k = --table_vars_of_var_i[table_vars[j]];

              if (k == 0) {
                div_size *= table_vars[j]->domainSize();
                table_vars_of_var_i.erase(table_vars[j]);
              }
            }

            tables_per_var[table_vars[i]].erase(ptrSeq);

            if (div_size != 1) {
              product_size.setPriority(table_vars[i],
                                       product_size.priority(table_vars[i])
                                          / div_size);
            }
          }
        }

        if (tmp_marginals.contains(ptrSeq)) {
          Size del_size = 1;

          for (const auto ptrVar: *ptrSeq) {
            del_size *= ptrVar->domainSize();
          }

          current_memory -= long(del_size);

          delete ptrSeq;
          tmp_marginals.erase(ptrSeq);
        }
      }

      tables_per_var.erase(del_var);

      // add the new projected marginal to the list of TABLES
      for (const auto mvar: *marginal) {
        if (del_vars.contains(mvar)) {
          // add the new marginal table to the set of tables of var i
          tables_per_var[mvar].insert(marginal);

          // add the variables of the table to tables_vars_per_var[vars[i]]
          HashTable< const DiscreteVariable*, unsigned int >& iter_vars
             = tables_vars_per_var[mvar];
          double mult_size = 1.0;

          for (const auto var: *marginal) {
            try {
              ++iter_vars[var];
            } catch (const NotFound&) {
              iter_vars.insert(var, 1);
              mult_size *= (double)var->domainSize();
            }
          }

          if (mult_size != 1) {
            product_size.setPriority(mvar,
                                     product_size.priority(mvar) * mult_size);
          }
        }
      }

      tmp_marginals.insert(marginal);
    }

    // here, tmp_marginals contains all the newly created tables
    for (auto iter = tmp_marginals.beginSafe(); iter != tmp_marginals.endSafe();
         ++iter) {
      delete *iter;
    }

    return std::pair< long, long >(max_memory, current_memory);
  }

  // returns the memory consumption used during the combinations and
  // projections
  template < typename GUM_SCALAR, template < typename > class TABLE >
  std::pair< long, long >
     MultiDimCombineAndProjectDefault< GUM_SCALAR, TABLE >::memoryUsage(
        const Set< const TABLE< GUM_SCALAR >* >& set,
        const Set< const DiscreteVariable* >&    del_vars) const {
    // create the set of sets of discrete variables involved in the tables
    Set< const Sequence< const DiscreteVariable* >* > var_set(set.size());

    for (const auto ptrTab: set) {
      var_set << &(ptrTab->variablesSequence());
    }

    return memoryUsage(var_set, del_vars);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
