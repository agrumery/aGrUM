
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief A DBRowGenerator class that returns incomplete rows as EM would do
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <agrum/learning/database/DBRowGeneratorIdentity.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// returns the allocator used
    template < typename GUM_SCALAR, template < typename > class ALLOC >
    INLINE typename DBRowGeneratorEM< GUM_SCALAR, ALLOC >::allocator_type
       DBRowGeneratorEM< GUM_SCALAR, ALLOC >::getAllocator() const {
      return DBRowGenerator< ALLOC >::getAllocator();
    }


    /// default constructor
    template < typename GUM_SCALAR, template < typename > class ALLOC >
    DBRowGeneratorEM< GUM_SCALAR, ALLOC >::DBRowGeneratorEM(
       const std::vector< DBTranslatedValueType, ALLOC< DBTranslatedValueType > >
                                     column_types,
       const BayesNet< GUM_SCALAR >& bn,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
          nodeId2columns,
       const typename DBRowGeneratorEM< GUM_SCALAR, ALLOC >::allocator_type&
          alloc) :
        DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >(
           column_types,
           bn,
           DBRowGeneratorGoal::ONLY_REMOVE_MISSING_VALUES,
           nodeId2columns,
           alloc),
        __filled_row1(bn.size(), 1.0, alloc),
        __filled_row2(bn.size(), 1.0, alloc) {
      setBayesNet(bn);

      GUM_CONSTRUCTOR(DBRowGeneratorEM);
    }


    /// copy constructor with a given allocator
    template < typename GUM_SCALAR, template < typename > class ALLOC >
    DBRowGeneratorEM< GUM_SCALAR, ALLOC >::DBRowGeneratorEM(
       const DBRowGeneratorEM< GUM_SCALAR, ALLOC >& from,
       const typename DBRowGeneratorEM< GUM_SCALAR, ALLOC >::allocator_type&
          alloc) :
        DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >(from, alloc),
        __input_row(from.__input_row), __missing_cols(from.__missing_cols),
        __nb_miss(from.__nb_miss), __joint_proba(from.__joint_proba),
        __filled_row1(from.__filled_row1), __filled_row2(from.__filled_row2),
        __use_filled_row1(from.__use_filled_row1),
        __original_weight(from.__original_weight) {
      if (from.__joint_inst != nullptr) {
        __joint_inst = new Instantiation(__joint_proba);
        const auto&       var_seq = __joint_inst->variablesSequence();
        const std::size_t size = var_seq.size();
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          __joint_inst->chgVal(Idx(i), from.__joint_inst->val(i));
        }
      }

      GUM_CONS_CPY(DBRowGeneratorEM);
    }


    /// copy constructor
    template < typename GUM_SCALAR, template < typename > class ALLOC >
    DBRowGeneratorEM< GUM_SCALAR, ALLOC >::DBRowGeneratorEM(
       const DBRowGeneratorEM< GUM_SCALAR, ALLOC >& from) :
        DBRowGeneratorEM< GUM_SCALAR, ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < typename GUM_SCALAR, template < typename > class ALLOC >
    DBRowGeneratorEM< GUM_SCALAR, ALLOC >::DBRowGeneratorEM(
       DBRowGeneratorEM< GUM_SCALAR, ALLOC >&& from,
       const typename DBRowGeneratorEM< GUM_SCALAR, ALLOC >::allocator_type&
          alloc) :
        DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >(std::move(from), alloc),
        __input_row(from.__input_row),
        __missing_cols(std::move(from.__missing_cols)), __nb_miss(from.__nb_miss),
        __joint_proba(std::move(from.__joint_proba)),
        __filled_row1(std::move(from.__filled_row1)),
        __filled_row2(std::move(from.__filled_row2)),
        __use_filled_row1(from.__use_filled_row1),
        __original_weight(from.__original_weight) {
      if (from.__joint_inst != nullptr) {
        __joint_inst = new Instantiation(__joint_proba);
        const auto&       var_seq = __joint_inst->variablesSequence();
        const std::size_t size = var_seq.size();
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          __joint_inst->chgVal(Idx(i), from.__joint_inst->val(i));
        }
      }

      GUM_CONS_MOV(DBRowGeneratorEM);
    }


    /// move constructor
    template < typename GUM_SCALAR, template < typename > class ALLOC >
    DBRowGeneratorEM< GUM_SCALAR, ALLOC >::DBRowGeneratorEM(
       DBRowGeneratorEM< GUM_SCALAR, ALLOC >&& from) :
        DBRowGeneratorEM< GUM_SCALAR, ALLOC >(std::move(from),
                                              from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < typename GUM_SCALAR, template < typename > class ALLOC >
    DBRowGeneratorEM< GUM_SCALAR, ALLOC >*
       DBRowGeneratorEM< GUM_SCALAR, ALLOC >::clone(
          const typename DBRowGeneratorEM< GUM_SCALAR, ALLOC >::allocator_type&
             alloc) const {
      ALLOC< DBRowGeneratorEM< GUM_SCALAR, ALLOC > > allocator(alloc);
      DBRowGeneratorEM< GUM_SCALAR, ALLOC >* generator = allocator.allocate(1);
      try {
        allocator.construct(generator, *this, alloc);
      } catch (...) {
        allocator.deallocate(generator, 1);
        throw;
      }
      return generator;
    }


    /// virtual copy constructor
    template < typename GUM_SCALAR, template < typename > class ALLOC >
    DBRowGeneratorEM< GUM_SCALAR, ALLOC >*
       DBRowGeneratorEM< GUM_SCALAR, ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < typename GUM_SCALAR, template < typename > class ALLOC >
    DBRowGeneratorEM< GUM_SCALAR, ALLOC >::~DBRowGeneratorEM() {
      if (__joint_inst != nullptr) delete __joint_inst;
      GUM_DESTRUCTOR(DBRowGeneratorEM);
    }


    /// copy operator
    template < typename GUM_SCALAR, template < typename > class ALLOC >
    DBRowGeneratorEM< GUM_SCALAR, ALLOC >& DBRowGeneratorEM< GUM_SCALAR, ALLOC >::
                                           operator=(const DBRowGeneratorEM< GUM_SCALAR, ALLOC >& from) {
      if (this != &from) {
        DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >::operator=(from);
        __input_row = from.__input_row;
        __missing_cols = from.__missing_cols;
        __nb_miss = from.__nb_miss;
        __joint_proba = from.__joint_proba;
        __filled_row1 = from.__filled_row1;
        __filled_row2 = from.__filled_row2;
        __use_filled_row1 = from.__use_filled_row1;
        __original_weight = from.__original_weight;

        if (__joint_inst != nullptr) {
          delete __joint_inst;
          __joint_inst = nullptr;
        }

        if (from.__joint_inst != nullptr) {
          __joint_inst = new Instantiation(__joint_proba);
          const auto&       var_seq = __joint_inst->variablesSequence();
          const std::size_t size = var_seq.size();
          for (std::size_t i = std::size_t(0); i < size; ++i) {
            __joint_inst->chgVal(Idx(i), from.__joint_inst->val(i));
          }
        }
      }

      return *this;
    }


    /// move operator
    template < typename GUM_SCALAR, template < typename > class ALLOC >
    DBRowGeneratorEM< GUM_SCALAR, ALLOC >& DBRowGeneratorEM< GUM_SCALAR, ALLOC >::
                                           operator=(DBRowGeneratorEM< GUM_SCALAR, ALLOC >&& from) {
      if (this != &from) {
        DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >::operator=(std::move(from));
        __input_row = from.__input_row;
        __missing_cols = std::move(from.__missing_cols);
        __nb_miss = from.__nb_miss;
        __joint_proba = std::move(from.__joint_proba);
        __filled_row1 = std::move(from.__filled_row1);
        __filled_row2 = std::move(from.__filled_row2);
        __use_filled_row1 = from.__use_filled_row1;
        __original_weight = from.__original_weight;

        if (__joint_inst != nullptr) {
          delete __joint_inst;
          __joint_inst = nullptr;
        }

        if (from.__joint_inst != nullptr) {
          __joint_inst = new Instantiation(__joint_proba);
          const auto&       var_seq = __joint_inst->variablesSequence();
          const std::size_t size = var_seq.size();
          for (std::size_t i = std::size_t(0); i < size; ++i) {
            __joint_inst->chgVal(Idx(i), from.__joint_inst->val(i));
          }
        }
      }

      return *this;
    }


    /// generates new lines from those the generator gets in input
    template < typename GUM_SCALAR, template < typename > class ALLOC >
    INLINE const DBRow< DBTranslatedValue, ALLOC >&
                 DBRowGeneratorEM< GUM_SCALAR, ALLOC >::generate() {
      this->decreaseRemainingRows();

      // if everything is observed, return the input row
      if (__input_row != nullptr) return *__input_row;

      if (__use_filled_row1) {
        // get the weight of the row from the joint probability
        __filled_row1.setWeight(__joint_proba.get(*__joint_inst)
                                * __original_weight);

        // fill the values of the row
        for (std::size_t i = std::size_t(0); i < __nb_miss; ++i)
          __filled_row1[__missing_cols[i]].discr_val = __joint_inst->val(i);

        __joint_inst->inc();
        __use_filled_row1 = false;

        return __filled_row1;
      } else {
        // get the weight of the row from the joint probability
        __filled_row2.setWeight(__joint_proba.get(*__joint_inst)
                                * __original_weight);

        // fill the values of the row
        for (std::size_t i = std::size_t(0); i < __nb_miss; ++i)
          __filled_row2[__missing_cols[i]].discr_val = __joint_inst->val(i);

        __joint_inst->inc();
        __use_filled_row1 = true;

        return __filled_row2;
      }
    }


    /// computes the rows it will provide in output
    template < typename GUM_SCALAR, template < typename > class ALLOC >
    INLINE std::size_t DBRowGeneratorEM< GUM_SCALAR, ALLOC >::_computeRows(
       const DBRow< DBTranslatedValue, ALLOC >& row) {
      // check if there are unobserved values among the columns of interest.
      // If this is the case, set them as targets
      bool        found_unobserved = false;
      const auto& xrow = row.row();
      for (const auto col : this->_columns_of_interest) {
        switch (this->_column_types[col]) {
          case DBTranslatedValueType::DISCRETE:
            if (xrow[col].discr_val == std::numeric_limits< std::size_t >::max()) {
              if (!found_unobserved) {
                __missing_cols.clear();
                found_unobserved = true;
              }
              __missing_cols.push_back(col);
            }
            break;

          case DBTranslatedValueType::CONTINUOUS:
            GUM_ERROR(NotImplementedYet,
                      "The BDRowGeneratorEM does not handle yet continuous "
                         << "variables. But the variable in column" << col
                         << " is continuous.");
            break;

          default:
            GUM_ERROR(NotImplementedYet,
                      "DBTranslatedValueType " << int(this->_column_types[col])
                                               << " is not supported yet");
        }
      }

      // if there is no unobserved value, make the __input_row point to the row
      if (!found_unobserved) {
        __input_row = &row;
        return std::size_t(1);
      }

      __input_row = nullptr;
      __nb_miss = __missing_cols.size();
      __original_weight = row.weight();

      // here, there are missing symbols, so we should compute the distribution
      // of the missing values. For this purpose, we use Variable Elimination
      VariableElimination< GUM_SCALAR > ve(this->_bn);

      // add the targets and fill the output row with the observed values
      NodeSet target_set(__nb_miss);
      if (this->_nodeId2columns.empty()) {
        std::size_t i = std::size_t(0);
        bool        end_miss = false;
        for (const auto col : this->_columns_of_interest) {
          if (!end_miss && (col == __missing_cols[i])) {
            target_set.insert(NodeId(col));
            ++i;
            if (i == __nb_miss) end_miss = true;
          } else {
            __filled_row1[col].discr_val = xrow[col].discr_val;
            __filled_row2[col].discr_val = xrow[col].discr_val;
          }
        }
      } else {
        std::size_t i = std::size_t(0);
        bool        end_miss = false;
        for (const auto col : this->_columns_of_interest) {
          if (!end_miss && (col == __missing_cols[i])) {
            target_set.insert(this->_nodeId2columns.first(col));
            ++i;
            if (i == __nb_miss) end_miss = true;
          } else {
            __filled_row1[col].discr_val = xrow[col].discr_val;
            __filled_row2[col].discr_val = xrow[col].discr_val;
          }
        }
      }

      ve.addJointTarget(target_set);

      // add the evidence and the target
      const std::size_t row_size = xrow.size();
      if (this->_nodeId2columns.empty()) {
        for (std::size_t col = std::size_t(0); col < row_size; ++col) {
          switch (this->_column_types[col]) {
            case DBTranslatedValueType::DISCRETE:
              // only observed values are evidence
              if (xrow[col].discr_val
                  != std::numeric_limits< std::size_t >::max()) {
                ve.addEvidence(NodeId(col), xrow[col].discr_val);
              }
              break;

            case DBTranslatedValueType::CONTINUOUS:
              GUM_ERROR(NotImplementedYet,
                        "The BDRowGeneratorEM does not handle yet continuous "
                           << "variables. But the variable in column" << col
                           << " is continuous.");
              break;

            default:
              GUM_ERROR(NotImplementedYet,
                        "DBTranslatedValueType " << int(this->_column_types[col])
                                                 << " is not supported yet");
          }
        }
      } else {
        for (std::size_t col = std::size_t(0); col < row_size; ++col) {
          switch (this->_column_types[col]) {
            case DBTranslatedValueType::DISCRETE:
              // only observed values are evidence
              if (xrow[col].discr_val
                  != std::numeric_limits< std::size_t >::max()) {
                ve.addEvidence(this->_nodeId2columns.first(col),
                               xrow[col].discr_val);
              }
              break;

            case DBTranslatedValueType::CONTINUOUS:
              GUM_ERROR(NotImplementedYet,
                        "The BDRowGeneratorEM does not handle yet continuous "
                           << "variables. But the variable in column" << col
                           << " is continuous.");
              break;

            default:
              GUM_ERROR(NotImplementedYet,
                        "DBTranslatedValueType " << int(this->_column_types[col])
                                                 << " is not supported yet");
          }
        }
      }

      // get the potential of the target set
      Potential< GUM_SCALAR >& pot =
         const_cast< Potential< GUM_SCALAR >& >(ve.jointPosterior(target_set));
      __joint_proba = std::move(pot);
      if (__joint_inst != nullptr) delete __joint_inst;
      __joint_inst = new Instantiation(__joint_proba);

      // get the mapping between variables of the joint proba and the
      // columns in the database
      const auto& var_sequence = __joint_proba.variablesSequence();
      if (this->_nodeId2columns.empty()) {
        for (std::size_t i = std::size_t(0); i < __nb_miss; ++i) {
          __missing_cols[i] = std::size_t(this->_bn->nodeId(*(var_sequence[i])));
        }
      } else {
        for (std::size_t i = std::size_t(0); i < __nb_miss; ++i) {
          __missing_cols[i] =
             this->_nodeId2columns.second(this->_bn->nodeId(*(var_sequence[i])));
        }
      }

      return std::size_t(__joint_proba.domainSize());
    }


    /// assign a new Bayes net to the generator
    template < typename GUM_SCALAR, template < typename > class ALLOC >
    void DBRowGeneratorEM< GUM_SCALAR, ALLOC >::setBayesNet(
       const BayesNet< GUM_SCALAR >& new_bn) {
      // check that if nodeId2columns is not empty, then all the columns
      // correspond to nodes of the BN
      if (!this->_nodeId2columns.empty()) {
        const DAG& dag = new_bn.dag();
        for (auto iter = this->_nodeId2columns.begin();
             iter != this->_nodeId2columns.end();
             ++iter) {
          if (!dag.existsNode(iter.first())) {
            GUM_ERROR(IdError,
                      "Column "
                         << iter.second()
                         << " of the database is associated to Node ID "
                         << iter.first()
                         << ", which does not belong to the Bayesian network");
          }
        }
      }

      DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >::setBayesNet(new_bn);

      // we determine the size of the filled rows
      std::size_t size = std::size_t(0);
      if (this->_nodeId2columns.empty()) {
        for (auto node : new_bn.dag())
          if (std::size_t(node) > size) size = std::size_t(node);
      } else {
        for (auto iter = this->_nodeId2columns.begin();
             iter != this->_nodeId2columns.end();
             ++iter) {
          if (iter.second() > size) size = iter.second();
        }
      }
      __filled_row1.resize(size + 1);
      __filled_row2.resize(size + 1);
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
