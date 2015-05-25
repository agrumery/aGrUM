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
 * @brief An efficient class for combining and projecting MultiDim tables
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <limits>

#include <agrum/config.h>

#include <agrum/multidim/operators/multiDimCombineAndProjectDefault.h>

namespace gum {

  /// default constructor
  template <typename GUM_SCALAR, template <typename> class TABLE>
  MultiDimCombineAndProjectDefault<GUM_SCALAR, TABLE>::
      MultiDimCombineAndProjectDefault(
          TABLE<GUM_SCALAR> *(*combine)(const TABLE<GUM_SCALAR> &,
                                        const TABLE<GUM_SCALAR> &),
          TABLE<GUM_SCALAR> *(*project)(const TABLE<GUM_SCALAR> &,
                                        const Set<const DiscreteVariable *> &))
      : MultiDimCombineAndProject<GUM_SCALAR, TABLE>(),
        __combination(new MultiDimCombinationDefault<GUM_SCALAR, TABLE>(combine)),
        __projection(new MultiDimProjection<GUM_SCALAR, TABLE>(project)) {
    // for debugging purposes
    GUM_CONSTRUCTOR(MultiDimCombineAndProjectDefault);
  }

  /// copy constructor
  template <typename GUM_SCALAR, template <typename> class TABLE>
  MultiDimCombineAndProjectDefault<GUM_SCALAR, TABLE>::
      MultiDimCombineAndProjectDefault(
          const MultiDimCombineAndProjectDefault<GUM_SCALAR, TABLE> &from)
      : MultiDimCombineAndProject<GUM_SCALAR, TABLE>(),
        __combination(from.__combination->newFactory()),
        __projection(from.__projection->newFactory()) {
    // for debugging purposes
    GUM_CONS_CPY(MultiDimCombineAndProjectDefault);
  }

  /// destructor
  template <typename GUM_SCALAR, template <typename> class TABLE>
  MultiDimCombineAndProjectDefault<GUM_SCALAR,
                                   TABLE>::~MultiDimCombineAndProjectDefault() {
    // for debugging purposes
    GUM_DESTRUCTOR(MultiDimCombineAndProjectDefault);
    delete __combination;
    delete __projection;
  }

  /// virtual constructor
  template <typename GUM_SCALAR, template <typename> class TABLE>
  MultiDimCombineAndProjectDefault<GUM_SCALAR, TABLE> *
  MultiDimCombineAndProjectDefault<GUM_SCALAR, TABLE>::newFactory() const {
    return new MultiDimCombineAndProjectDefault<GUM_SCALAR, TABLE>(*this);
  }

  /// combine and project
  template <typename GUM_SCALAR, template <typename> class TABLE>
  Set<const TABLE<GUM_SCALAR> *>
  MultiDimCombineAndProjectDefault<GUM_SCALAR, TABLE>::combineAndProject(
      Set<const TABLE<GUM_SCALAR> *> table_set,
      Set<const DiscreteVariable *> del_vars) {
    // when we remove a variable, we need to combine all the tables containing this
    // variable in order to produce a new unique table containing this variable.
    // removing the variable is then performed by marginalizing it out of the
    // table. In the combineAndProject algorithm, we wish to remove first
    // variables that produce small tables. This should speed up the marginalizing
    // process

    unsigned int nb_vars;
    {
      // determine the set of all the variables involved in the tables.
      // this should help sizing correctly the hashtables
      Set<const DiscreteVariable *> all_vars;

      for (typename Set<const TABLE<GUM_SCALAR> *>::const_iterator_safe iter =
               table_set.beginSafe();
           iter != table_set.endSafe(); ++iter) {
        const Sequence<const DiscreteVariable *> &iter_vars =
            (*iter)->variablesSequence();

        for (typename Sequence<const DiscreteVariable *>::const_iterator_safe it =
                 iter_vars.beginSafe();
             it != iter_vars.endSafe(); ++it) {
          all_vars.insert(*it);
        }
      }

      nb_vars = all_vars.size();
    }

    // the tables containing a given variable
    HashTable<const DiscreteVariable *, Set<const TABLE<GUM_SCALAR> *>>
        tables_per_var(nb_vars);
    // for a given variable X to be deleted, the list of all the variables of
    // the tables containing X (actually, we count the number of tables
    // containing the variable. This is more efficient for computing and updating
    // the product_size priority queue (see below) when some tables are removed)
    HashTable<const DiscreteVariable *,
              HashTable<const DiscreteVariable *, unsigned int>>
        tables_vars_per_var(nb_vars);

    // initialize tables_vars_per_var and tables_per_var
    {
      Set<const TABLE<GUM_SCALAR> *> empty_set(table_set.size());
      HashTable<const DiscreteVariable *, unsigned int> empty_hash(nb_vars);

      for (typename Set<const DiscreteVariable *>::const_iterator_safe iter =
               del_vars.beginSafe();
           iter != del_vars.endSafe(); ++iter) {
        tables_per_var.insert(*iter, empty_set);
        tables_vars_per_var.insert(*iter, empty_hash);
      }

      // update properly tables_per_var and tables_vars_per_var
      for (typename Set<const TABLE<GUM_SCALAR> *>::const_iterator_safe iter =
               table_set.beginSafe();
           iter != table_set.endSafe(); ++iter) {
        const Sequence<const DiscreteVariable *> &vars =
            (*iter)->variablesSequence();

        for (unsigned int i = 0; i < vars.size(); ++i) {
          if (del_vars.contains(vars[i])) {
            // add the table to the set of tables related to vars[i]
            tables_per_var[vars[i]].insert(*iter);
            // add the variables of the table to tables_vars_per_var[vars[i]]
            HashTable<const DiscreteVariable *, unsigned int> &iter_vars =
                tables_vars_per_var[vars[i]];

            for (unsigned int j = 0; j < vars.size(); ++j) {
              try {
                ++iter_vars[vars[j]];
              } catch (const NotFound &) {
                iter_vars.insert(vars[j], 1);
              }
            }
          }
        }
      }
    }

    // the sizes of the tables produced when removing a given discrete variable
    PriorityQueue<const DiscreteVariable *, float> product_size;

    // initialize properly product_size

    for (typename HashTable<const DiscreteVariable *,
                            HashTable<const DiscreteVariable *,
                                      unsigned int>>::const_iterator_safe iter =
             tables_vars_per_var.beginSafe();
         iter != tables_vars_per_var.endSafe(); ++iter) {
      float size = 1.0f;
      const HashTable<const DiscreteVariable *, unsigned int> &vars = iter.val();

      if (vars.size()) {
        for (typename HashTable<const DiscreteVariable *,
                                unsigned int>::const_iterator_safe iter2 =
                 vars.beginSafe();
             iter2 != vars.endSafe(); ++iter2) {
          size *= iter2.key()->domainSize();
        }

        product_size.insert(iter.key(), size);
      }
    }

    // create a set of the temporary tables created during the
    // marginalization process (useful for deallocating temporary tables)
    Set<const TABLE<GUM_SCALAR> *> tmp_marginals(table_set.size());

    // now, remove all the variables in del_vars, starting from those that produce
    // the smallest tables
    while (!product_size.empty()) {
      // get the best variable to remove
      const DiscreteVariable *del_var = product_size.pop();
      del_vars.erase(del_var);

      // get the set of tables to combine
      Set<const TABLE<GUM_SCALAR> *> &tables_to_combine = tables_per_var[del_var];

      // if there is no tables to combine, do nothing

      if (tables_to_combine.size() == 0)
        continue;

      // compute the combination of all the tables: if there is only one table,
      // there is nothing to do, else we shall use the MultiDimCombination
      // to perform the combination
      TABLE<GUM_SCALAR> *joint;

      bool joint_to_delete = false;

      if (tables_to_combine.size() == 1) {
        joint = const_cast<TABLE<GUM_SCALAR> *>(*(tables_to_combine.beginSafe()));
        joint_to_delete = false;
      } else {
        joint = __combination->combine(tables_to_combine);
        joint_to_delete = true;
      }

      // compute the table resulting from marginalizing out del_var from joint
      Set<const DiscreteVariable *> del_one_var;

      del_one_var << del_var;

      TABLE<GUM_SCALAR> *marginal = __projection->project(*joint, del_one_var);

      // remove the temporary joint if needed
      if (joint_to_delete)
        delete joint;

      // update tables_vars_per_var : remove the variables of the TABLEs we
      // combined from this hashtable
      // update accordingly tables_per_vars : remove these TABLEs
      // update accordingly product_size : when a variable is no more used by
      // any TABLE, divide product_size by its domain size

      for (typename Set<const TABLE<GUM_SCALAR> *>::const_iterator_safe iter =
               tables_to_combine.beginSafe();
           iter != tables_to_combine.endSafe(); ++iter) {
        const Sequence<const DiscreteVariable *> &table_vars =
            (*iter)->variablesSequence();

        for (unsigned int i = 0; i < table_vars.size(); ++i) {
          if (del_vars.contains(table_vars[i])) {
            // ok, here we have a variable that needed to be removed => update
            // product_size, tables_per_var and tables_vars_per_var
            HashTable<const DiscreteVariable *, unsigned int> &table_vars_of_var_i =
                tables_vars_per_var[table_vars[i]];
            float div_size = 1.0f;

            for (unsigned int j = 0; j < table_vars.size(); ++j) {
              unsigned int k = --table_vars_of_var_i[table_vars[j]];

              if (k == 0) {
                div_size *= table_vars[j]->domainSize();
                table_vars_of_var_i.erase(table_vars[j]);
              }
            }

            tables_per_var[table_vars[i]].erase(*iter);

            if (div_size != 1) {
              product_size.setPriority(
                  table_vars[i], product_size.priority(table_vars[i]) / div_size);
            }
          }
        }

        if (tmp_marginals.contains(*iter)) {
          delete *iter;
          tmp_marginals.erase(*iter);
        }

        table_set.erase(*iter);
      }

      tables_per_var.erase(del_var);

      // add the new projected marginal to the list of TABLES
      const Sequence<const DiscreteVariable *> &marginal_vars =
          marginal->variablesSequence();

      for (unsigned int i = 0; i < marginal_vars.size(); ++i) {
        if (del_vars.contains(marginal_vars[i])) {
          // add the new marginal table to the set of tables of var i
          tables_per_var[marginal_vars[i]].insert(marginal);

          // add the variables of the table to tables_vars_per_var[vars[i]]
          HashTable<const DiscreteVariable *, unsigned int> &iter_vars =
              tables_vars_per_var[marginal_vars[i]];
          float mult_size = 1.0f;

          for (unsigned int j = 0; j < marginal_vars.size(); ++j) {
            try {
              ++iter_vars[marginal_vars[j]];
            } catch (const NotFound &) {
              iter_vars.insert(marginal_vars[j], 1);
              mult_size *= marginal_vars[j]->domainSize();
            }
          }

          if (mult_size != 1) {
            product_size.setPriority(marginal_vars[i],
                                     product_size.priority(marginal_vars[i]) *
                                         mult_size);
          }
        }
      }

      table_set.insert(marginal);

      tmp_marginals.insert(marginal);
    }

    // here, tmp_marginals contains all the newly created tables and
    // table_set contains the list of the tables resulting from the
    // marginalizing out of del_vars of the combination of the tables
    // of table_set

    return table_set;
  }

  /// changes the function used for combining two TABLES
  template <typename GUM_SCALAR, template <typename> class TABLE>
  INLINE void
  MultiDimCombineAndProjectDefault<GUM_SCALAR, TABLE>::setCombineFunction(
      TABLE<GUM_SCALAR> *(*combine)(const TABLE<GUM_SCALAR> &,
                                    const TABLE<GUM_SCALAR> &)) {
    __combination->setCombineFunction(combine);
  }

  /// returns the current combination function
  template <typename GUM_SCALAR, template <typename> class TABLE>
  INLINE TABLE<GUM_SCALAR> *(
      *MultiDimCombineAndProjectDefault<GUM_SCALAR, TABLE>::combineFunction())(
      const TABLE<GUM_SCALAR> &, const TABLE<GUM_SCALAR> &) {
    return __combination->combineFunction();
  }

  /// changes the class that performs the combinations
  template <typename GUM_SCALAR, template <typename> class TABLE>
  INLINE void
  MultiDimCombineAndProjectDefault<GUM_SCALAR, TABLE>::setCombinationClass(
      const MultiDimCombination<GUM_SCALAR, TABLE> &comb_class) {
    delete __combination;
    __combination = comb_class.newFactory();
  }

  /// changes the function used for projecting TABLES
  template <typename GUM_SCALAR, template <typename> class TABLE>
  INLINE void
  MultiDimCombineAndProjectDefault<GUM_SCALAR, TABLE>::setProjectFunction(
      TABLE<GUM_SCALAR> *(*proj)(const TABLE<GUM_SCALAR> &,
                                 const Set<const DiscreteVariable *> &)) {
    __projection->setProjectFunction(proj);
  }

  /// returns the current projection function
  template <typename GUM_SCALAR, template <typename> class TABLE>
  INLINE TABLE<GUM_SCALAR> *(
      *MultiDimCombineAndProjectDefault<GUM_SCALAR, TABLE>::projectFunction())(
      const TABLE<GUM_SCALAR> &, const Set<const DiscreteVariable *> &) {
    return __projection->projectFunction();
  }

  /// changes the class that performs the projections
  template <typename GUM_SCALAR, template <typename> class TABLE>
  INLINE void
  MultiDimCombineAndProjectDefault<GUM_SCALAR, TABLE>::setProjectionClass(
      const MultiDimProjection<GUM_SCALAR, TABLE> &proj_class) {
    delete __projection;
    __projection = proj_class.newFactory();
  }

  /** @brief returns a rough estimate of the number of operations that will be
   * performed to compute the combination */
  template <typename GUM_SCALAR, template <typename> class TABLE>
  float MultiDimCombineAndProjectDefault<GUM_SCALAR, TABLE>::nbOperations(
      const Set<const Sequence<const DiscreteVariable *> *> &table_set,
      Set<const DiscreteVariable *> del_vars) const {
    // when we remove a variable, we need to combine all the tables containing this
    // variable in order to produce a new unique table containing this variable.
    // Here, we do not have the tables but only their variables (dimensions), but
    // the principle is identical. Removing a variable is then performed by
    // marginalizing it out of the table or, equivalently, to remove it from the
    // table's list of variables. In the combineAndProjectDefault algorithm, we
    // wish to remove first variables that would produce small tables. This
    // should speed up the whole marginalizing process.

    unsigned int nb_vars;
    {
      // determine the set of all the variables involved in the tables.
      // this should help sizing correctly the hashtables
      Set<const DiscreteVariable *> all_vars;

      for (
          typename Set<
              const Sequence<const DiscreteVariable *> *>::const_iterator_safe iter =
              table_set.beginSafe();
          iter != table_set.endSafe(); ++iter) {
        const Sequence<const DiscreteVariable *> &iter_vars = **iter;

        for (typename Sequence<const DiscreteVariable *>::const_iterator_safe it =
                 iter_vars.beginSafe();
             it != iter_vars.endSafe(); ++it) {
          all_vars.insert(*it);
        }
      }

      nb_vars = all_vars.size();
    }

    // the tables (actually their variables) containing a given variable
    HashTable<const DiscreteVariable *,
              Set<const Sequence<const DiscreteVariable *> *>>
        tables_per_var(nb_vars);
    // for a given variable X to be deleted, the list of all the variables of
    // the tables containing X (actually, we count the number of tables
    // containing the variable. This is more efficient for computing and updating
    // the product_size priority queue (see below) when some tables are removed)
    HashTable<const DiscreteVariable *,
              HashTable<const DiscreteVariable *, unsigned int>>
        tables_vars_per_var(nb_vars);

    // initialize tables_vars_per_var and tables_per_var
    {
      Set<const Sequence<const DiscreteVariable *> *> empty_set(table_set.size());
      HashTable<const DiscreteVariable *, unsigned int> empty_hash(nb_vars);

      for (typename Set<const DiscreteVariable *>::const_iterator_safe iter =
               del_vars.beginSafe();
           iter != del_vars.endSafe(); ++iter) {
        tables_per_var.insert(*iter, empty_set);
        tables_vars_per_var.insert(*iter, empty_hash);
      }

      // update properly tables_per_var and tables_vars_per_var
      for (
          typename Set<
              const Sequence<const DiscreteVariable *> *>::const_iterator_safe iter =
              table_set.beginSafe();
          iter != table_set.endSafe(); ++iter) {
        const Sequence<const DiscreteVariable *> &vars = **iter;

        for (unsigned int i = 0; i < vars.size(); ++i) {
          if (del_vars.contains(vars[i])) {
            // add the table's variables to the set of those related to vars[i]
            tables_per_var[vars[i]].insert(*iter);
            // add the variables of the table to tables_vars_per_var[vars[i]]
            HashTable<const DiscreteVariable *, unsigned int> &iter_vars =
                tables_vars_per_var[vars[i]];

            for (unsigned int j = 0; j < vars.size(); ++j) {
              try {
                ++iter_vars[vars[j]];
              } catch (const NotFound &) {
                iter_vars.insert(vars[j], 1);
              }
            }
          }
        }
      }
    }

    // the sizes of the tables produced when removing a given discrete variable
    PriorityQueue<const DiscreteVariable *, float> product_size;

    // initialize properly product_size

    for (typename HashTable<const DiscreteVariable *,
                            HashTable<const DiscreteVariable *,
                                      unsigned int>>::const_iterator_safe iter =
             tables_vars_per_var.beginSafe();
         iter != tables_vars_per_var.endSafe(); ++iter) {
      float size = 1.0f;
      const HashTable<const DiscreteVariable *, unsigned int> &vars = iter.val();

      if (vars.size()) {
        for (typename HashTable<const DiscreteVariable *,
                                unsigned int>::const_iterator_safe iter2 =
                 vars.beginSafe();
             iter2 != vars.endSafe(); ++iter2) {
          size *= iter2.key()->domainSize();
        }

        product_size.insert(iter.key(), size);
      }
    }

    // the resulting number of operations
    float nb_operations = 0;

    // create a set of the temporary table's variables created during the
    // marginalization process (useful for deallocating temporary tables)
    Set<const Sequence<const DiscreteVariable *> *> tmp_marginals(table_set.size());

    // now, remove all the variables in del_vars, starting from those that produce
    // the smallest tables
    while (!product_size.empty()) {
      // get the best variable to remove
      const DiscreteVariable *del_var = product_size.pop();
      del_vars.erase(del_var);

      // get the set of tables to combine
      Set<const Sequence<const DiscreteVariable *> *> &tables_to_combine =
          tables_per_var[del_var];

      // if there is no tables to combine, do nothing

      if (tables_to_combine.size() == 0)
        continue;

      // compute the combination of all the tables: if there is only one table,
      // there is nothing to do, else we shall use the MultiDimCombination
      // to perform the combination
      Sequence<const DiscreteVariable *> *joint;

      bool joint_to_delete = false;

      if (tables_to_combine.size() == 1) {
        joint = const_cast<Sequence<const DiscreteVariable *> *>(
            *(tables_to_combine.beginSafe()));
        joint_to_delete = false;
      } else {
        // here, compute the union of all the variables of the tables to combine
        joint = new Sequence<const DiscreteVariable *>;

        for (typename Set<
                 const Sequence<const DiscreteVariable *> *>::const_iterator_safe
                 iter = tables_to_combine.beginSafe();
             iter != tables_to_combine.endSafe(); ++iter) {
          const Sequence<const DiscreteVariable *> &vars = **iter;

          for (typename Sequence<const DiscreteVariable *>::const_iterator_safe
                   iter2 = vars.beginSafe();
               iter2 != vars.endSafe(); ++iter2) {
            if (!joint->exists(*iter2)) {
              joint->insert(*iter2);
            }
          }
        }

        joint_to_delete = true;

        // update the number of operations performed
        nb_operations += __combination->nbOperations(tables_to_combine);
      }

      // update the number of operations performed by marginalizing out del_var
      Set<const DiscreteVariable *> del_one_var;

      del_one_var << del_var;

      nb_operations += __projection->nbOperations(*joint, del_one_var);

      // compute the table resulting from marginalizing out del_var from joint
      Sequence<const DiscreteVariable *> *marginal;

      if (joint_to_delete) {
        marginal = joint;
      } else {
        marginal = new Sequence<const DiscreteVariable *>(*joint);
      }

      marginal->erase(del_var);

      // update tables_vars_per_var : remove the variables of the TABLEs we
      // combined from this hashtable
      // update accordingly tables_per_vars : remove these TABLEs
      // update accordingly product_size : when a variable is no more used by
      // any TABLE, divide product_size by its domain size

      for (
          typename Set<
              const Sequence<const DiscreteVariable *> *>::const_iterator_safe iter =
              tables_to_combine.beginSafe();
          iter != tables_to_combine.endSafe(); ++iter) {
        const Sequence<const DiscreteVariable *> &table_vars = **iter;

        for (unsigned int i = 0; i < table_vars.size(); ++i) {
          if (del_vars.contains(table_vars[i])) {
            // ok, here we have a variable that needed to be removed => update
            // product_size, tables_per_var and tables_vars_per_var
            HashTable<const DiscreteVariable *, unsigned int> &table_vars_of_var_i =
                tables_vars_per_var[table_vars[i]];
            float div_size = 1.0f;

            for (unsigned int j = 0; j < table_vars.size(); ++j) {
              unsigned int k = --table_vars_of_var_i[table_vars[j]];

              if (k == 0) {
                div_size *= table_vars[j]->domainSize();
                table_vars_of_var_i.erase(table_vars[j]);
              }
            }

            tables_per_var[table_vars[i]].erase(*iter);

            if (div_size != 1) {
              product_size.setPriority(
                  table_vars[i], product_size.priority(table_vars[i]) / div_size);
            }
          }
        }

        if (tmp_marginals.contains(*iter)) {
          delete *iter;
          tmp_marginals.erase(*iter);
        }
      }

      tables_per_var.erase(del_var);

      // add the new projected marginal to the list of TABLES

      for (unsigned int i = 0; i < marginal->size(); ++i) {
        const DiscreteVariable *var_i = marginal->atPos(i);

        if (del_vars.contains(var_i)) {
          // add the new marginal table to the set of tables of var i
          tables_per_var[var_i].insert(marginal);

          // add the variables of the table to tables_vars_per_var[vars[i]]
          HashTable<const DiscreteVariable *, unsigned int> &iter_vars =
              tables_vars_per_var[var_i];
          float mult_size = 1.0f;

          for (unsigned int j = 0; j < marginal->size(); ++j) {
            try {
              ++iter_vars[marginal->atPos(j)];
            } catch (const NotFound &) {
              iter_vars.insert(marginal->atPos(j), 1);
              mult_size *= marginal->atPos(j)->domainSize();
            }
          }

          if (mult_size != 1) {
            product_size.setPriority(var_i,
                                     product_size.priority(var_i) * mult_size);
          }
        }
      }

      tmp_marginals.insert(marginal);
    }

    // here, tmp_marginals contains all the newly created tables

    for (
        typename Set<const Sequence<const DiscreteVariable *> *>::const_iterator_safe
            iter = tmp_marginals.beginSafe();
        iter != tmp_marginals.endSafe(); ++iter) {
      delete *iter;
    }

    return nb_operations;
  }

  /** @brief returns a rough estimate of the number of operations that will be
   * performed to compute the combination */
  template <typename GUM_SCALAR, template <typename> class TABLE>
  float MultiDimCombineAndProjectDefault<GUM_SCALAR, TABLE>::nbOperations(
      const Set<const TABLE<GUM_SCALAR> *> &set,
      const Set<const DiscreteVariable *> &del_vars) const {
    // create the set of sets of discrete variables involved in the tables
    Set<const Sequence<const DiscreteVariable *> *> var_set(set.size());

    for (typename Set<const TABLE<GUM_SCALAR> *>::const_iterator_safe iter =
             set.beginSafe();
         iter != set.endSafe(); ++iter) {
      var_set << &((*iter)->variablesSequence());
    }

    return nbOperations(var_set, del_vars);
  }

  /// returns the memory consumption used during the combinations and projections
  template <typename GUM_SCALAR, template <typename> class TABLE>
  std::pair<long, long>
  MultiDimCombineAndProjectDefault<GUM_SCALAR, TABLE>::memoryUsage(
      const Set<const Sequence<const DiscreteVariable *> *> &table_set,
      Set<const DiscreteVariable *> del_vars) const {
    // when we remove a variable, we need to combine all the tables containing this
    // variable in order to produce a new unique table containing this variable.
    // Here, we do not have the tables but only their variables (dimensions), but
    // the principle is identical. Removing a variable is then performed by
    // marginalizing it out of the table or, equivalently, to remove it from the
    // table's list of variables. In the combineAndProjectDefault algorithm, we
    // wish to remove first variables that would produce small tables. This
    // should speed up the whole marginalizing process.

    unsigned int nb_vars;
    {
      // determine the set of all the variables involved in the tables.
      // this should help sizing correctly the hashtables
      Set<const DiscreteVariable *> all_vars;

      for (
          typename Set<
              const Sequence<const DiscreteVariable *> *>::const_iterator_safe iter =
              table_set.beginSafe();
          iter != table_set.endSafe(); ++iter) {
        const Sequence<const DiscreteVariable *> &iter_vars = **iter;

        for (typename Sequence<const DiscreteVariable *>::const_iterator_safe it =
                 iter_vars.beginSafe();
             it != iter_vars.endSafe(); ++it) {
          all_vars.insert(*it);
        }
      }

      nb_vars = all_vars.size();
    }

    // the tables (actually their variables) containing a given variable
    HashTable<const DiscreteVariable *,
              Set<const Sequence<const DiscreteVariable *> *>>
        tables_per_var(nb_vars);
    // for a given variable X to be deleted, the list of all the variables of
    // the tables containing X (actually, we count the number of tables
    // containing the variable. This is more efficient for computing and updating
    // the product_size priority queue (see below) when some tables are removed)
    HashTable<const DiscreteVariable *,
              HashTable<const DiscreteVariable *, unsigned int>>
        tables_vars_per_var(nb_vars);

    // initialize tables_vars_per_var and tables_per_var
    {
      Set<const Sequence<const DiscreteVariable *> *> empty_set(table_set.size());
      HashTable<const DiscreteVariable *, unsigned int> empty_hash(nb_vars);

      for (typename Set<const DiscreteVariable *>::const_iterator_safe iter =
               del_vars.beginSafe();
           iter != del_vars.endSafe(); ++iter) {
        tables_per_var.insert(*iter, empty_set);
        tables_vars_per_var.insert(*iter, empty_hash);
      }

      // update properly tables_per_var and tables_vars_per_var
      for (
          typename Set<
              const Sequence<const DiscreteVariable *> *>::const_iterator_safe iter =
              table_set.beginSafe();
          iter != table_set.endSafe(); ++iter) {
        const Sequence<const DiscreteVariable *> &vars = **iter;

        for (unsigned int i = 0; i < vars.size(); ++i) {
          if (del_vars.contains(vars[i])) {
            // add the table's variables to the set of those related to vars[i]
            tables_per_var[vars[i]].insert(*iter);
            // add the variables of the table to tables_vars_per_var[vars[i]]
            HashTable<const DiscreteVariable *, unsigned int> &iter_vars =
                tables_vars_per_var[vars[i]];

            for (unsigned int j = 0; j < vars.size(); ++j) {
              try {
                ++iter_vars[vars[j]];
              } catch (const NotFound &) {
                iter_vars.insert(vars[j], 1);
              }
            }
          }
        }
      }
    }

    // the sizes of the tables produced when removing a given discrete variable
    PriorityQueue<const DiscreteVariable *, float> product_size;

    // initialize properly product_size

    for (typename HashTable<const DiscreteVariable *,
                            HashTable<const DiscreteVariable *,
                                      unsigned int>>::const_iterator_safe iter =
             tables_vars_per_var.beginSafe();
         iter != tables_vars_per_var.endSafe(); ++iter) {
      float size = 1.0f;
      const HashTable<const DiscreteVariable *, unsigned int> &vars = iter.val();

      if (vars.size()) {
        for (typename HashTable<const DiscreteVariable *,
                                unsigned int>::const_iterator_safe iter2 =
                 vars.beginSafe();
             iter2 != vars.endSafe(); ++iter2) {
          size *= iter2.key()->domainSize();
        }

        product_size.insert(iter.key(), size);
      }
    }

    // the resulting memory consumtions
    long max_memory = 0;

    long current_memory = 0;

    // create a set of the temporary table's variables created during the
    // marginalization process (useful for deallocating temporary tables)
    Set<const Sequence<const DiscreteVariable *> *> tmp_marginals(table_set.size());

    // now, remove all the variables in del_vars, starting from those that produce
    // the smallest tables
    while (!product_size.empty()) {
      // get the best variable to remove
      const DiscreteVariable *del_var = product_size.pop();
      del_vars.erase(del_var);

      // get the set of tables to combine
      Set<const Sequence<const DiscreteVariable *> *> &tables_to_combine =
          tables_per_var[del_var];

      // if there is no tables to combine, do nothing

      if (tables_to_combine.size() == 0)
        continue;

      // compute the combination of all the tables: if there is only one table,
      // there is nothing to do, else we shall use the MultiDimCombination
      // to perform the combination
      Sequence<const DiscreteVariable *> *joint;

      bool joint_to_delete = false;

      if (tables_to_combine.size() == 1) {
        joint = const_cast<Sequence<const DiscreteVariable *> *>(
            *(tables_to_combine.beginSafe()));
        joint_to_delete = false;
      } else {
        // here, compute the union of all the variables of the tables to combine
        joint = new Sequence<const DiscreteVariable *>;

        for (typename Set<
                 const Sequence<const DiscreteVariable *> *>::const_iterator_safe
                 iter = tables_to_combine.beginSafe();
             iter != tables_to_combine.endSafe(); ++iter) {
          const Sequence<const DiscreteVariable *> &vars = **iter;

          for (typename Sequence<const DiscreteVariable *>::const_iterator_safe
                   iter2 = vars.beginSafe();
               iter2 != vars.endSafe(); ++iter2) {
            if (!joint->exists(*iter2)) {
              joint->insert(*iter2);
            }
          }
        }

        joint_to_delete = true;

        // update the number of operations performed
        std::pair<long, long> comb_memory =
            __combination->memoryUsage(tables_to_combine);

        if ((std::numeric_limits<long>::max() - current_memory <
             comb_memory.first) ||
            (std::numeric_limits<long>::max() - current_memory <
             comb_memory.second)) {
          GUM_ERROR(OutOfBounds, "memory usage out of long int range");
        }

        if (current_memory + comb_memory.first > max_memory) {
          max_memory = current_memory + comb_memory.first;
        }

        current_memory += comb_memory.second;
      }

      // update the number of operations performed by marginalizing out del_var
      Set<const DiscreteVariable *> del_one_var;

      del_one_var << del_var;

      std::pair<long, long> comb_memory =
          __projection->memoryUsage(*joint, del_one_var);

      if ((std::numeric_limits<long>::max() - current_memory < comb_memory.first) ||
          (std::numeric_limits<long>::max() - current_memory < comb_memory.second)) {
        GUM_ERROR(OutOfBounds, "memory usage out of long int range");
      }

      if (current_memory + comb_memory.first > max_memory) {
        max_memory = current_memory + comb_memory.first;
      }

      current_memory += comb_memory.second;

      // compute the table resulting from marginalizing out del_var from joint
      Sequence<const DiscreteVariable *> *marginal;

      if (joint_to_delete) {
        marginal = joint;
      } else {
        marginal = new Sequence<const DiscreteVariable *>(*joint);
      }

      marginal->erase(del_var);

      // update tables_vars_per_var : remove the variables of the TABLEs we
      // combined from this hashtable
      // update accordingly tables_per_vars : remove these TABLEs
      // update accordingly product_size : when a variable is no more used by
      // any TABLE, divide product_size by its domain size

      for (
          typename Set<
              const Sequence<const DiscreteVariable *> *>::const_iterator_safe iter =
              tables_to_combine.beginSafe();
          iter != tables_to_combine.endSafe(); ++iter) {
        const Sequence<const DiscreteVariable *> &table_vars = **iter;

        for (unsigned int i = 0; i < table_vars.size(); ++i) {
          if (del_vars.contains(table_vars[i])) {
            // ok, here we have a variable that needed to be removed => update
            // product_size, tables_per_var and tables_vars_per_var
            HashTable<const DiscreteVariable *, unsigned int> &table_vars_of_var_i =
                tables_vars_per_var[table_vars[i]];
            float div_size = 1.0f;

            for (unsigned int j = 0; j < table_vars.size(); ++j) {
              unsigned int k = --table_vars_of_var_i[table_vars[j]];

              if (k == 0) {
                div_size *= table_vars[j]->domainSize();
                table_vars_of_var_i.erase(table_vars[j]);
              }
            }

            tables_per_var[table_vars[i]].erase(*iter);

            if (div_size != 1) {
              product_size.setPriority(
                  table_vars[i], product_size.priority(table_vars[i]) / div_size);
            }
          }
        }

        if (tmp_marginals.contains(*iter)) {
          long del_size = 1;
          const Sequence<const DiscreteVariable *> &del = **iter;

          for (typename Sequence<const DiscreteVariable *>::const_iterator_safe
                   iter_del = del.beginSafe();
               iter_del != del.endSafe(); ++iter_del) {
            del_size *= (*iter_del)->domainSize();
          }

          current_memory -= del_size;

          delete *iter;
          tmp_marginals.erase(*iter);
        }
      }

      tables_per_var.erase(del_var);

      // add the new projected marginal to the list of TABLES

      for (unsigned int i = 0; i < marginal->size(); ++i) {
        const DiscreteVariable *var_i = marginal->atPos(i);

        if (del_vars.contains(var_i)) {
          // add the new marginal table to the set of tables of var i
          tables_per_var[var_i].insert(marginal);

          // add the variables of the table to tables_vars_per_var[vars[i]]
          HashTable<const DiscreteVariable *, unsigned int> &iter_vars =
              tables_vars_per_var[var_i];
          float mult_size = 1.0f;

          for (unsigned int j = 0; j < marginal->size(); ++j) {
            try {
              ++iter_vars[marginal->atPos(j)];
            } catch (const NotFound &) {
              iter_vars.insert(marginal->atPos(j), 1);
              mult_size *= marginal->atPos(j)->domainSize();
            }
          }

          if (mult_size != 1) {
            product_size.setPriority(var_i,
                                     product_size.priority(var_i) * mult_size);
          }
        }
      }

      tmp_marginals.insert(marginal);
    }

    // here, tmp_marginals contains all the newly created tables
    for (
        typename Set<const Sequence<const DiscreteVariable *> *>::const_iterator_safe
            iter = tmp_marginals.beginSafe();
        iter != tmp_marginals.endSafe(); ++iter) {
      delete *iter;
    }

    return std::pair<long, long>(max_memory, current_memory);
  }

  /// returns the memory consumption used during the combinations and projections
  template <typename GUM_SCALAR, template <typename> class TABLE>
  std::pair<long, long>
  MultiDimCombineAndProjectDefault<GUM_SCALAR, TABLE>::memoryUsage(
      const Set<const TABLE<GUM_SCALAR> *> &set,
      const Set<const DiscreteVariable *> &del_vars) const {
    // create the set of sets of discrete variables involved in the tables
    Set<const Sequence<const DiscreteVariable *> *> var_set(set.size());

    for (typename Set<const TABLE<GUM_SCALAR> *>::const_iterator_safe iter =
             set.beginSafe();
         iter != set.endSafe(); ++iter) {
      var_set << &((*iter)->variablesSequence());
    }

    return memoryUsage(var_set, del_vars);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
