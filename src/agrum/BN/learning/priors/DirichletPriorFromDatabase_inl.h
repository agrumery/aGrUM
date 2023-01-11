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

#include <agrum/BN/learning/priors/DirichletPriorFromDatabase.h>
/** @file
 * @brief A dirichlet priori: computes its N'_ijk from a database
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// copy constructor
    INLINE DirichletPriorFromDatabase::DirichletPriorFromDatabase(
       const DirichletPriorFromDatabase& from) :
        Prior(from),
        _counter_(from._counter_), _internal_weight_(from._internal_weight_) {
      GUM_CONS_CPY(DirichletPriorFromDatabase);
    }


    /// move constructor
    INLINE
    DirichletPriorFromDatabase::DirichletPriorFromDatabase(
       DirichletPriorFromDatabase&& from) noexcept :
        Prior(std::move(from)),
        _counter_(std::move(from._counter_)), _internal_weight_(from._internal_weight_) {
      GUM_CONS_MOV(DirichletPriorFromDatabase);
    }


    /// virtual copy constructor
    INLINE DirichletPriorFromDatabase* DirichletPriorFromDatabase::clone() const {
      return new DirichletPriorFromDatabase(*this);
    }


    /// destructor
    INLINE DirichletPriorFromDatabase::~DirichletPriorFromDatabase() {
      GUM_DESTRUCTOR(DirichletPriorFromDatabase);
    }


    /// copy operator
    INLINE DirichletPriorFromDatabase&
       DirichletPriorFromDatabase::operator=(const DirichletPriorFromDatabase& from) {
      if (this != &from) {
        Prior::operator=(from);
        _counter_         = from._counter_;
        _internal_weight_ = from._internal_weight_;
      }
      return *this;
    }


    /// move operator
    INLINE DirichletPriorFromDatabase&
       DirichletPriorFromDatabase::operator=(DirichletPriorFromDatabase&& from) {
      if (this != &from) {
        Prior::operator=(std::move(from));
        _counter_         = std::move(from._counter_);
        _internal_weight_ = from._internal_weight_;
      }
      return *this;
    }


    /// returns the type of the prior
    INLINE PriorType DirichletPriorFromDatabase::getType() const {
      return PriorType::DirichletPriorType;
    }


    /// indicates whether the prior is potentially informative
    INLINE bool DirichletPriorFromDatabase::isInformative() const { return (this->weight_ != 0.0); }


    /// sets the weight of the a prior(kind of effective sample size)
    INLINE void DirichletPriorFromDatabase::setWeight(const double weight) {
      Prior::setWeight(weight);
      if (_counter_.database().nbRows() == 0) _internal_weight_ = 0.0;
      else _internal_weight_ = this->weight_ / double(_counter_.database().nbRows());
    }


    /// returns the prior vector all the variables in the idset
    INLINE void DirichletPriorFromDatabase::addJointPseudoCount(const IdCondSet&       idset,
                                                                std::vector< double >& counts) {
      if (this->weight_ == 0.0) return;

      const auto&       prior = _counter_.counts(idset);
      const std::size_t size  = prior.size();
      if (_internal_weight_ != 1.0) {
        for (auto i = std::size_t(0); i < size; ++i) {
          counts[i] += prior[i] * _internal_weight_;
        }
      } else {
        for (auto i = std::size_t(0); i < size; ++i) {
          counts[i] += prior[i];
        }
      }
    }


    /// returns the prior vector over only the conditioning set of an idset
    INLINE void
       DirichletPriorFromDatabase::addConditioningPseudoCount(const IdCondSet&       idset,
                                                              std::vector< double >& counts) {
      if (_internal_weight_ == 0.0) return;

      const auto&       prior = _counter_.counts(idset.conditionalIdCondSet());
      const std::size_t size  = prior.size();
      if (_internal_weight_ != 1.0) {
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          counts[i] += prior[i] * _internal_weight_;
        }
      } else {
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          counts[i] += prior[i];
        }
      }
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
