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
 * @brief A DBRowGenerator class that returns incomplete rows as EM would do
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#include <agrum/tools/database/DBRowGeneratorIdentity.h>

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
        filled_row1__(bn.size(), 1.0, alloc),
        filled_row2__(bn.size(), 1.0, alloc) {
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
        input_row__(from.input_row__), missing_cols__(from.missing_cols__),
        nb_miss__(from.nb_miss__), joint_proba__(from.joint_proba__),
        filled_row1__(from.filled_row1__), filled_row2__(from.filled_row2__),
        use_filled_row1__(from.use_filled_row1__),
        original_weight__(from.original_weight__) {
      if (from.joint_inst__ != nullptr) {
        joint_inst__              = new Instantiation(joint_proba__);
        const auto&       var_seq = joint_inst__->variablesSequence();
        const std::size_t size    = var_seq.size();
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          joint_inst__->chgVal(Idx(i), from.joint_inst__->val(i));
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
        input_row__(from.input_row__),
        missing_cols__(std::move(from.missing_cols__)), nb_miss__(from.nb_miss__),
        joint_proba__(std::move(from.joint_proba__)),
        filled_row1__(std::move(from.filled_row1__)),
        filled_row2__(std::move(from.filled_row2__)),
        use_filled_row1__(from.use_filled_row1__),
        original_weight__(from.original_weight__) {
      if (from.joint_inst__ != nullptr) {
        joint_inst__              = new Instantiation(joint_proba__);
        const auto&       var_seq = joint_inst__->variablesSequence();
        const std::size_t size    = var_seq.size();
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          joint_inst__->chgVal(Idx(i), from.joint_inst__->val(i));
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
      if (joint_inst__ != nullptr) delete joint_inst__;
      GUM_DESTRUCTOR(DBRowGeneratorEM);
    }


    /// copy operator
    template < typename GUM_SCALAR, template < typename > class ALLOC >
    DBRowGeneratorEM< GUM_SCALAR, ALLOC >&
       DBRowGeneratorEM< GUM_SCALAR, ALLOC >::operator=(
          const DBRowGeneratorEM< GUM_SCALAR, ALLOC >& from) {
      if (this != &from) {
        DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >::operator=(from);
        input_row__                                        = from.input_row__;
        missing_cols__                                     = from.missing_cols__;
        nb_miss__                                          = from.nb_miss__;
        joint_proba__                                      = from.joint_proba__;
        filled_row1__                                      = from.filled_row1__;
        filled_row2__                                      = from.filled_row2__;
        use_filled_row1__ = from.use_filled_row1__;
        original_weight__ = from.original_weight__;

        if (joint_inst__ != nullptr) {
          delete joint_inst__;
          joint_inst__ = nullptr;
        }

        if (from.joint_inst__ != nullptr) {
          joint_inst__              = new Instantiation(joint_proba__);
          const auto&       var_seq = joint_inst__->variablesSequence();
          const std::size_t size    = var_seq.size();
          for (std::size_t i = std::size_t(0); i < size; ++i) {
            joint_inst__->chgVal(Idx(i), from.joint_inst__->val(i));
          }
        }
      }

      return *this;
    }


    /// move operator
    template < typename GUM_SCALAR, template < typename > class ALLOC >
    DBRowGeneratorEM< GUM_SCALAR, ALLOC >&
       DBRowGeneratorEM< GUM_SCALAR, ALLOC >::operator=(
          DBRowGeneratorEM< GUM_SCALAR, ALLOC >&& from) {
      if (this != &from) {
        DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >::operator=(std::move(from));
        input_row__                                        = from.input_row__;
        missing_cols__    = std::move(from.missing_cols__);
        nb_miss__         = from.nb_miss__;
        joint_proba__     = std::move(from.joint_proba__);
        filled_row1__     = std::move(from.filled_row1__);
        filled_row2__     = std::move(from.filled_row2__);
        use_filled_row1__ = from.use_filled_row1__;
        original_weight__ = from.original_weight__;

        if (joint_inst__ != nullptr) {
          delete joint_inst__;
          joint_inst__ = nullptr;
        }

        if (from.joint_inst__ != nullptr) {
          joint_inst__              = new Instantiation(joint_proba__);
          const auto&       var_seq = joint_inst__->variablesSequence();
          const std::size_t size    = var_seq.size();
          for (std::size_t i = std::size_t(0); i < size; ++i) {
            joint_inst__->chgVal(Idx(i), from.joint_inst__->val(i));
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
      if (input_row__ != nullptr) return *input_row__;

      if (use_filled_row1__) {
        // get the weight of the row from the joint probability
        filled_row1__.setWeight(joint_proba__.get(*joint_inst__)
                                * original_weight__);

        // fill the values of the row
        for (std::size_t i = std::size_t(0); i < nb_miss__; ++i)
          filled_row1__[missing_cols__[i]].discr_val = joint_inst__->val(i);

        joint_inst__->inc();
        use_filled_row1__ = false;

        return filled_row1__;
      } else {
        // get the weight of the row from the joint probability
        filled_row2__.setWeight(joint_proba__.get(*joint_inst__)
                                * original_weight__);

        // fill the values of the row
        for (std::size_t i = std::size_t(0); i < nb_miss__; ++i)
          filled_row2__[missing_cols__[i]].discr_val = joint_inst__->val(i);

        joint_inst__->inc();
        use_filled_row1__ = true;

        return filled_row2__;
      }
    }


    /// computes the rows it will provide in output
    template < typename GUM_SCALAR, template < typename > class ALLOC >
    INLINE std::size_t DBRowGeneratorEM< GUM_SCALAR, ALLOC >::computeRows_(
       const DBRow< DBTranslatedValue, ALLOC >& row) {
      // check if there are unobserved values among the columns of interest.
      // If this is the case, set them as targets
      bool        found_unobserved = false;
      const auto& xrow             = row.row();
      for (const auto col: this->columns_of_interest_) {
        switch (this->column_types_[col]) {
          case DBTranslatedValueType::DISCRETE:
            if (xrow[col].discr_val == std::numeric_limits< std::size_t >::max()) {
              if (!found_unobserved) {
                missing_cols__.clear();
                found_unobserved = true;
              }
              missing_cols__.push_back(col);
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
                      "DBTranslatedValueType " << int(this->column_types_[col])
                                               << " is not supported yet");
        }
      }

      // if there is no unobserved value, make the input_row__ point to the row
      if (!found_unobserved) {
        input_row__ = &row;
        return std::size_t(1);
      }

      input_row__       = nullptr;
      nb_miss__         = missing_cols__.size();
      original_weight__ = row.weight();

      // here, there are missing symbols, so we should compute the distribution
      // of the missing values. For this purpose, we use Variable Elimination
      VariableElimination< GUM_SCALAR > ve(this->bn_);

      // add the targets and fill the output row with the observed values
      NodeSet target_set(nb_miss__);
      if (this->nodeId2columns_.empty()) {
        std::size_t i        = std::size_t(0);
        bool        end_miss = false;
        for (const auto col: this->columns_of_interest_) {
          if (!end_miss && (col == missing_cols__[i])) {
            target_set.insert(NodeId(col));
            ++i;
            if (i == nb_miss__) end_miss = true;
          } else {
            filled_row1__[col].discr_val = xrow[col].discr_val;
            filled_row2__[col].discr_val = xrow[col].discr_val;
          }
        }
      } else {
        std::size_t i        = std::size_t(0);
        bool        end_miss = false;
        for (const auto col: this->columns_of_interest_) {
          if (!end_miss && (col == missing_cols__[i])) {
            target_set.insert(this->nodeId2columns_.first(col));
            ++i;
            if (i == nb_miss__) end_miss = true;
          } else {
            filled_row1__[col].discr_val = xrow[col].discr_val;
            filled_row2__[col].discr_val = xrow[col].discr_val;
          }
        }
      }

      ve.addJointTarget(target_set);

      // add the evidence and the target
      const std::size_t row_size = xrow.size();
      if (this->nodeId2columns_.empty()) {
        for (std::size_t col = std::size_t(0); col < row_size; ++col) {
          switch (this->column_types_[col]) {
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
                        "DBTranslatedValueType " << int(this->column_types_[col])
                                                 << " is not supported yet");
          }
        }
      } else {
        for (std::size_t col = std::size_t(0); col < row_size; ++col) {
          switch (this->column_types_[col]) {
            case DBTranslatedValueType::DISCRETE:
              // only observed values are evidence
              if (xrow[col].discr_val
                  != std::numeric_limits< std::size_t >::max()) {
                ve.addEvidence(this->nodeId2columns_.first(col),
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
                        "DBTranslatedValueType " << int(this->column_types_[col])
                                                 << " is not supported yet");
          }
        }
      }

      // get the potential of the target set
      Potential< GUM_SCALAR >& pot
         = const_cast< Potential< GUM_SCALAR >& >(ve.jointPosterior(target_set));
      joint_proba__ = std::move(pot);
      if (joint_inst__ != nullptr) delete joint_inst__;
      joint_inst__ = new Instantiation(joint_proba__);

      // get the mapping between variables of the joint proba and the
      // columns in the database
      const auto& var_sequence = joint_proba__.variablesSequence();
      if (this->nodeId2columns_.empty()) {
        for (std::size_t i = std::size_t(0); i < nb_miss__; ++i) {
          missing_cols__[i] = std::size_t(this->bn_->nodeId(*(var_sequence[i])));
        }
      } else {
        for (std::size_t i = std::size_t(0); i < nb_miss__; ++i) {
          missing_cols__[i]
             = this->nodeId2columns_.second(this->bn_->nodeId(*(var_sequence[i])));
        }
      }

      return std::size_t(joint_proba__.domainSize());
    }


    /// assign a new Bayes net to the generator
    template < typename GUM_SCALAR, template < typename > class ALLOC >
    void DBRowGeneratorEM< GUM_SCALAR, ALLOC >::setBayesNet(
       const BayesNet< GUM_SCALAR >& new_bn) {
      // check that if nodeId2columns is not empty, then all the columns
      // correspond to nodes of the BN
      if (!this->nodeId2columns_.empty()) {
        const DAG& dag = new_bn.dag();
        for (auto iter = this->nodeId2columns_.begin();
             iter != this->nodeId2columns_.end();
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
      if (this->nodeId2columns_.empty()) {
        for (auto node: new_bn.dag())
          if (std::size_t(node) > size) size = std::size_t(node);
      } else {
        for (auto iter = this->nodeId2columns_.begin();
             iter != this->nodeId2columns_.end();
             ++iter) {
          if (iter.second() > size) size = iter.second();
        }
      }
      filled_row1__.resize(size + 1);
      filled_row2__.resize(size + 1);
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
