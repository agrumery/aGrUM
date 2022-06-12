/**
 *
 *   Copyright (c) 2005-2022 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief A dirichlet priori: computes its N'_ijk from a database
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// copy constructor
    INLINE AprioriDirichletFromDatabase::AprioriDirichletFromDatabase(
       const AprioriDirichletFromDatabase& from) :
        Prior(from),
        _counter_(from._counter_), _internal_weight_(from._internal_weight_) {
      GUM_CONS_CPY(AprioriDirichletFromDatabase);
    }


    /// move constructor
    INLINE AprioriDirichletFromDatabase::AprioriDirichletFromDatabase(
       AprioriDirichletFromDatabase&& from) :
        Prior(std::move(from)),
        _counter_(std::move(from._counter_)), _internal_weight_(from._internal_weight_) {
      GUM_CONS_MOV(AprioriDirichletFromDatabase);
    }


    /// virtual copy constructor
    INLINE AprioriDirichletFromDatabase* AprioriDirichletFromDatabase::clone() const {
      return new AprioriDirichletFromDatabase(*this);
    }


    /// destructor
    INLINE AprioriDirichletFromDatabase::~AprioriDirichletFromDatabase() {
      GUM_DESTRUCTOR(AprioriDirichletFromDatabase);
    }


    /// copy operator
    INLINE AprioriDirichletFromDatabase&
       AprioriDirichletFromDatabase::operator=(const AprioriDirichletFromDatabase& from) {
      if (this != &from) {
        Prior::operator =(from);
        _counter_         = from._counter_;
        _internal_weight_ = from._internal_weight_;
      }
      return *this;
    }


    /// move operator
    INLINE AprioriDirichletFromDatabase&
       AprioriDirichletFromDatabase::operator=(AprioriDirichletFromDatabase&& from) {
      if (this != &from) {
        Prior::operator =(std::move(from));
        _counter_         = std::move(from._counter_);
        _internal_weight_ = from._internal_weight_;
      }
      return *this;
    }


    /// returns the type of the apriori
    INLINE PriorType AprioriDirichletFromDatabase::getType() const {
      return PriorType::DirichletPriorType;
    }


    /// indicates whether the apriori is potentially informative
    INLINE bool AprioriDirichletFromDatabase::isInformative() const {
      return (this->weight_ != 0.0);
    }


    /// sets the weight of the a priori (kind of effective sample size)
    INLINE void AprioriDirichletFromDatabase::setWeight(const double weight) {
      Prior::setWeight(weight);
      if (_counter_.database().nbRows() == 0.0)
        _internal_weight_ = 0.0;
      else
        _internal_weight_ = this->weight_ / _counter_.database().nbRows();
    }


    /// returns the apriori vector all the variables in the idset
    INLINE void AprioriDirichletFromDatabase::addAllApriori(const IdCondSet&       idset,
                                                            std::vector< double >& counts) {
      if (this->weight_ == 0.0) return;

      const auto&       apriori = _counter_.counts(idset);
      const std::size_t size    = apriori.size();
      if (_internal_weight_ != 1.0) {
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          counts[i] += apriori[i] * _internal_weight_;
        }
      } else {
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          counts[i] += apriori[i];
        }
      }
    }


    /// returns the apriori vector over only the conditioning set of an idset
    INLINE void
       AprioriDirichletFromDatabase::addConditioningApriori(const IdCondSet&       idset,
                                                            std::vector< double >& counts) {
      if (_internal_weight_ == 0.0) return;

      const auto&       apriori = _counter_.counts(idset.conditionalIdCondSet());
      const std::size_t size    = apriori.size();
      if (_internal_weight_ != 1.0) {
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          counts[i] += apriori[i] * _internal_weight_;
        }
      } else {
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          counts[i] += apriori[i];
        }
      }
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
