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

#include <agrum/BN/learning/priors/bdeuPrior.h>
/** @file
 * @brief the internal prior for the BDeu score (N' / (r_i * q_i)
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum::learning {

  /// default constructor
  INLINE BDeuPrior::BDeuPrior(const DatabaseTable&                    database,
                              const Bijection< NodeId, std::size_t >& nodeId2columns) :
      Prior(database, nodeId2columns){GUM_CONSTRUCTOR(BDeuPrior)}


      /// copy constructor
      INLINE BDeuPrior::BDeuPrior(const BDeuPrior& from) :
      Prior(from){GUM_CONS_CPY(BDeuPrior)}


      /// move constructor
      INLINE BDeuPrior::BDeuPrior(BDeuPrior && from) noexcept :
      Prior(std::move(from)){GUM_CONS_MOV(BDeuPrior)}


      /// virtual copy constructor
      INLINE BDeuPrior
      * BDeuPrior::clone() const {
    return new BDeuPrior(*this);
  }


  /// destructor
  INLINE BDeuPrior::~BDeuPrior(){GUM_DESTRUCTOR(BDeuPrior)}


  /// copy operator
  INLINE BDeuPrior& BDeuPrior::operator=(const BDeuPrior& from) {
    Prior::operator=(from);
    return *this;
  }


  /// move operator
  INLINE BDeuPrior& BDeuPrior::operator=(BDeuPrior&& from) noexcept {
    Prior::operator=(std::move(from));
    return *this;
  }


  /// sets the effective sample size N' (alias of setEffectiveSampleSize ())
  INLINE void BDeuPrior::setWeight(const double weight) {
    if (weight < 0.0) {
      GUM_ERROR(OutOfBounds, "A negative weight (" << weight << ") is forbidden for the BDeu prior")
    }
    this->weight_ = weight;
  }


  /// sets the effective sample size N'
  INLINE void BDeuPrior::setEffectiveSampleSize(const double weight) { setWeight(weight); }


  /// returns the type of the prior
  INLINE PriorType BDeuPrior::getType() const { return PriorType::BDeuPriorType; }


  /// indicates whether the prior is potentially informative
  INLINE bool BDeuPrior::isInformative() const { return this->weight_ != 0.0; }


  /// returns the prior vector all the variables in the idset
  INLINE void BDeuPrior::addJointPseudoCount(const IdCondSet&       idset,
                                             std::vector< double >& counts) {
    // if the idset is empty or the weight is zero, the prior is also empty
    if (idset.empty() || (this->weight_ == 0.0)) return;

    // otherwise, add the weight to all the cells in the counting vector
    const double weight = this->weight_ / double(counts.size());
    for (auto& count: counts)
      count += weight;
  }


}   // namespace gum::learning

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
