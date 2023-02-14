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


/** @file
 * @brief A DBRowGenerator class that returns incomplete rows as EM would do
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/tools/database/DBRowGeneratorIdentity.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template < typename GUM_SCALAR >
    DBRowGeneratorEM< GUM_SCALAR >::DBRowGeneratorEM(
       const std::vector< DBTranslatedValueType >& column_types,
       const BayesNet< GUM_SCALAR >&               bn,
       const Bijection< NodeId, std::size_t >&     nodeId2columns) :
        DBRowGeneratorWithBN< GUM_SCALAR >(column_types,
                                           bn,
                                           DBRowGeneratorGoal::ONLY_REMOVE_MISSING_VALUES,
                                           nodeId2columns),
        _filled_row1_(bn.size(), 1.0), _filled_row2_(bn.size(), 1.0) {
      setBayesNet(bn);

      GUM_CONSTRUCTOR(DBRowGeneratorEM);
    }


    /// copy constructor
    template < typename GUM_SCALAR >
    DBRowGeneratorEM< GUM_SCALAR >::DBRowGeneratorEM(const DBRowGeneratorEM< GUM_SCALAR >& from) :
        DBRowGeneratorWithBN< GUM_SCALAR >(from), _input_row_(from._input_row_),
        _missing_cols_(from._missing_cols_), _nb_miss_(from._nb_miss_),
        _joint_proba_(from._joint_proba_), _filled_row1_(from._filled_row1_),
        _filled_row2_(from._filled_row2_), _use_filled_row1_(from._use_filled_row1_),
        _original_weight_(from._original_weight_) {
      if (from._joint_inst_ != nullptr) {
        _joint_inst_              = new Instantiation(_joint_proba_);
        const auto&       var_seq = _joint_inst_->variablesSequence();
        const std::size_t size    = var_seq.size();
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          _joint_inst_->chgVal(Idx(i), from._joint_inst_->val(i));
        }
      }

      GUM_CONS_CPY(DBRowGeneratorEM);
    }


    /// move constructor
    template < typename GUM_SCALAR >
    DBRowGeneratorEM< GUM_SCALAR >::DBRowGeneratorEM(DBRowGeneratorEM< GUM_SCALAR >&& from) :
        DBRowGeneratorWithBN< GUM_SCALAR >(std::move(from)), _input_row_(from._input_row_),
        _missing_cols_(std::move(from._missing_cols_)), _nb_miss_(from._nb_miss_),
        _joint_proba_(std::move(from._joint_proba_)), _filled_row1_(std::move(from._filled_row1_)),
        _filled_row2_(std::move(from._filled_row2_)), _use_filled_row1_(from._use_filled_row1_),
        _original_weight_(from._original_weight_) {
      if (from._joint_inst_ != nullptr) {
        _joint_inst_              = new Instantiation(_joint_proba_);
        const auto&       var_seq = _joint_inst_->variablesSequence();
        const std::size_t size    = var_seq.size();
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          _joint_inst_->chgVal(Idx(i), from._joint_inst_->val(i));
        }
      }

      GUM_CONS_MOV(DBRowGeneratorEM);
    }


    /// virtual copy constructor
    template < typename GUM_SCALAR >
    DBRowGeneratorEM< GUM_SCALAR >* DBRowGeneratorEM< GUM_SCALAR >::clone() const {
      return new DBRowGeneratorEM< GUM_SCALAR >(*this);
    }


    /// destructor
    template < typename GUM_SCALAR >
    DBRowGeneratorEM< GUM_SCALAR >::~DBRowGeneratorEM() {
      if (_joint_inst_ != nullptr) delete _joint_inst_;
      GUM_DESTRUCTOR(DBRowGeneratorEM);
    }


    /// copy operator
    template < typename GUM_SCALAR >
    DBRowGeneratorEM< GUM_SCALAR >&
       DBRowGeneratorEM< GUM_SCALAR >::operator=(const DBRowGeneratorEM< GUM_SCALAR >& from) {
      if (this != &from) {
        DBRowGeneratorWithBN< GUM_SCALAR >::operator=(from);
        _input_row_       = from._input_row_;
        _missing_cols_    = from._missing_cols_;
        _nb_miss_         = from._nb_miss_;
        _joint_proba_     = from._joint_proba_;
        _filled_row1_     = from._filled_row1_;
        _filled_row2_     = from._filled_row2_;
        _use_filled_row1_ = from._use_filled_row1_;
        _original_weight_ = from._original_weight_;

        if (_joint_inst_ != nullptr) {
          delete _joint_inst_;
          _joint_inst_ = nullptr;
        }

        if (from._joint_inst_ != nullptr) {
          _joint_inst_              = new Instantiation(_joint_proba_);
          const auto&       var_seq = _joint_inst_->variablesSequence();
          const std::size_t size    = var_seq.size();
          for (std::size_t i = std::size_t(0); i < size; ++i) {
            _joint_inst_->chgVal(Idx(i), from._joint_inst_->val(i));
          }
        }
      }

      return *this;
    }


    /// move operator
    template < typename GUM_SCALAR >
    DBRowGeneratorEM< GUM_SCALAR >&
       DBRowGeneratorEM< GUM_SCALAR >::operator=(DBRowGeneratorEM< GUM_SCALAR >&& from) {
      if (this != &from) {
        DBRowGeneratorWithBN< GUM_SCALAR >::operator=(std::move(from));
        _input_row_       = from._input_row_;
        _missing_cols_    = std::move(from._missing_cols_);
        _nb_miss_         = from._nb_miss_;
        _joint_proba_     = std::move(from._joint_proba_);
        _filled_row1_     = std::move(from._filled_row1_);
        _filled_row2_     = std::move(from._filled_row2_);
        _use_filled_row1_ = from._use_filled_row1_;
        _original_weight_ = from._original_weight_;

        if (_joint_inst_ != nullptr) {
          delete _joint_inst_;
          _joint_inst_ = nullptr;
        }

        if (from._joint_inst_ != nullptr) {
          _joint_inst_              = new Instantiation(_joint_proba_);
          const auto&       var_seq = _joint_inst_->variablesSequence();
          const std::size_t size    = var_seq.size();
          for (std::size_t i = std::size_t(0); i < size; ++i) {
            _joint_inst_->chgVal(Idx(i), from._joint_inst_->val(i));
          }
        }
      }

      return *this;
    }


    /// generates new lines from those the generator gets in input
    template < typename GUM_SCALAR >
    INLINE const DBRow< DBTranslatedValue >& DBRowGeneratorEM< GUM_SCALAR >::generate() {
      this->decreaseRemainingRows();

      // if everything is observed, return the input row
      if (_input_row_ != nullptr) return *_input_row_;

      if (_use_filled_row1_) {
        // get the weight of the row from the joint probability
        _filled_row1_.setWeight(_joint_proba_.get(*_joint_inst_) * _original_weight_);

        // fill the values of the row
        for (std::size_t i = std::size_t(0); i < _nb_miss_; ++i)
          _filled_row1_[_missing_cols_[i]].discr_val = _joint_inst_->val(i);

        _joint_inst_->inc();
        _use_filled_row1_ = false;

        return _filled_row1_;
      } else {
        // get the weight of the row from the joint probability
        _filled_row2_.setWeight(_joint_proba_.get(*_joint_inst_) * _original_weight_);

        // fill the values of the row
        for (std::size_t i = std::size_t(0); i < _nb_miss_; ++i)
          _filled_row2_[_missing_cols_[i]].discr_val = _joint_inst_->val(i);

        _joint_inst_->inc();
        _use_filled_row1_ = true;

        return _filled_row2_;
      }
    }


    /// computes the rows it will provide in output
    template < typename GUM_SCALAR >
    INLINE std::size_t
           DBRowGeneratorEM< GUM_SCALAR >::computeRows_(const DBRow< DBTranslatedValue >& row) {
      // check if there are unobserved values among the columns of interest.
      // If this is the case, set them as targets
      bool        found_unobserved = false;
      const auto& xrow             = row.row();
      for (const auto col: this->columns_of_interest_) {
        switch (this->column_types_[col]) {
          case DBTranslatedValueType::DISCRETE:
            if (xrow[col].discr_val == std::numeric_limits< std::size_t >::max()) {
              if (!found_unobserved) {
                _missing_cols_.clear();
                found_unobserved = true;
              }
              _missing_cols_.push_back(col);
            }
            break;

          case DBTranslatedValueType::CONTINUOUS:
            GUM_ERROR(NotImplementedYet,
                      "The BDRowGeneratorEM does not handle yet continuous "
                         << "variables. But the variable in column" << col << " is continuous.");
            break;

          default:
            GUM_ERROR(NotImplementedYet,
                      "DBTranslatedValueType " << int(this->column_types_[col])
                                               << " is not supported yet");
        }
      }

      // if there is no unobserved value, make the  _input_row_ point to the row
      if (!found_unobserved) {
        _input_row_ = &row;
        return std::size_t(1);
      }

      _input_row_       = nullptr;
      _nb_miss_         = _missing_cols_.size();
      _original_weight_ = row.weight();

      // here, there are missing symbols, so we should compute the distribution
      // of the missing values. For this purpose, we use Variable Elimination
      VariableElimination< GUM_SCALAR > ve(this->bn_);

      // add the targets and fill the output row with the observed values
      NodeSet target_set(_nb_miss_);
      if (this->nodeId2columns_.empty()) {
        std::size_t i        = std::size_t(0);
        bool        end_miss = false;
        for (const auto col: this->columns_of_interest_) {
          if (!end_miss && (col == _missing_cols_[i])) {
            target_set.insert(NodeId(col));
            ++i;
            if (i == _nb_miss_) end_miss = true;
          } else {
            _filled_row1_[col].discr_val = xrow[col].discr_val;
            _filled_row2_[col].discr_val = xrow[col].discr_val;
          }
        }
      } else {
        std::size_t i        = std::size_t(0);
        bool        end_miss = false;
        for (const auto col: this->columns_of_interest_) {
          if (!end_miss && (col == _missing_cols_[i])) {
            target_set.insert(this->nodeId2columns_.first(col));
            ++i;
            if (i == _nb_miss_) end_miss = true;
          } else {
            _filled_row1_[col].discr_val = xrow[col].discr_val;
            _filled_row2_[col].discr_val = xrow[col].discr_val;
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
              if (xrow[col].discr_val != std::numeric_limits< std::size_t >::max()) {
                ve.addEvidence(NodeId(col), xrow[col].discr_val);
              }
              break;

            case DBTranslatedValueType::CONTINUOUS:
              GUM_ERROR(NotImplementedYet,
                        "The BDRowGeneratorEM does not handle yet continuous "
                           << "variables. But the variable in column" << col << " is continuous.");
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
              if (xrow[col].discr_val != std::numeric_limits< std::size_t >::max()) {
                ve.addEvidence(this->nodeId2columns_.first(col), xrow[col].discr_val);
              }
              break;

            case DBTranslatedValueType::CONTINUOUS:
              GUM_ERROR(NotImplementedYet,
                        "The BDRowGeneratorEM does not handle yet continuous "
                           << "variables. But the variable in column" << col << " is continuous.");
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
      _joint_proba_ = std::move(pot);
      if (_joint_inst_ != nullptr) delete _joint_inst_;
      _joint_inst_ = new Instantiation(_joint_proba_);

      // get the mapping between variables of the joint proba and the
      // columns in the database
      const auto& var_sequence = _joint_proba_.variablesSequence();
      if (this->nodeId2columns_.empty()) {
        for (std::size_t i = std::size_t(0); i < _nb_miss_; ++i) {
          _missing_cols_[i] = std::size_t(this->bn_->nodeId(*(var_sequence[i])));
        }
      } else {
        for (std::size_t i = std::size_t(0); i < _nb_miss_; ++i) {
          _missing_cols_[i] = this->nodeId2columns_.second(this->bn_->nodeId(*(var_sequence[i])));
        }
      }

      return std::size_t(_joint_proba_.domainSize());
    }


    /// assign a new Bayes net to the generator
    template < typename GUM_SCALAR >
    void DBRowGeneratorEM< GUM_SCALAR >::setBayesNet(const BayesNet< GUM_SCALAR >& new_bn) {
      // check that if nodeId2columns is not empty, then all the columns
      // correspond to nodes of the BN
      if (!this->nodeId2columns_.empty()) {
        const DAG& dag = new_bn.dag();
        for (auto iter = this->nodeId2columns_.begin(); iter != this->nodeId2columns_.end();
             ++iter) {
          if (!dag.existsNode(iter.first())) {
            GUM_ERROR(InvalidNode,
                      "Column " << iter.second() << " of the database is associated to Node ID "
                                << iter.first()
                                << ", which does not belong to the Bayesian network");
          }
        }
      }

      DBRowGeneratorWithBN< GUM_SCALAR >::setBayesNet(new_bn);

      // we determine the size of the filled rows
      std::size_t size = std::size_t(0);
      if (this->nodeId2columns_.empty()) {
        for (auto node: new_bn.dag())
          if (std::size_t(node) > size) size = std::size_t(node);
      } else {
        for (auto iter = this->nodeId2columns_.begin(); iter != this->nodeId2columns_.end();
             ++iter) {
          if (iter.second() > size) size = iter.second();
        }
      }
      _filled_row1_.resize(size + 1);
      _filled_row2_.resize(size + 1);
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
