/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Implementation of Gibbs inference methods in Bayesian networks.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN(@LIP6)
 */


#include <agrum/BN/inference/tools/gibbsOperator.h>
#include <agrum/tools/core/utils_random.h>

namespace gum {

  template < typename GUM_SCALAR >
  GibbsOperator< GUM_SCALAR >::GibbsOperator(const IBayesNet< GUM_SCALAR >& BN,
                                             const NodeProperty< Idx >*     hardEv,
                                             Size                           nbr,
                                             bool atRandom) :
      counting_(0),
      sampling_bn_(BN), hardEv_(hardEv), nbr_(nbr), atRandom_(atRandom) {
    updateSamplingNodes__();
    GUM_CONSTRUCTOR(GibbsOperator);
  }

  template < typename GUM_SCALAR >
  GibbsOperator< GUM_SCALAR >::~GibbsOperator() {
    GUM_DESTRUCTOR(GibbsOperator);
  }

  template < typename GUM_SCALAR >
  void GibbsOperator< GUM_SCALAR >::updateSamplingNodes__() {
    samplingNodes_.clear();
    for (const auto node: sampling_bn_.nodes())
      if (hardEv_ == nullptr || !hardEv_->exists(node))
        samplingNodes_.insert(node);
    if (samplingNodes_.size() == 0) {
      GUM_ERROR(InvalidArgument,
                "No node to sample (too many nodes or too much evidence)!")
    }
    if (nbr_ > samplingNodes_.size()) nbr_ = samplingNodes_.size();
  }

  /// returns a MC sample
  /// This is not a really sample since we take into account evidence without
  /// care about parent of evidence, etc.
  /// This is just a not-so-bad first sample for GibbsSampler
  template < typename GUM_SCALAR >
  Instantiation GibbsOperator< GUM_SCALAR >::monteCarloSample() {
    gum::Instantiation I;

    for (const auto nod: sampling_bn_.topologicalOrder()) {
      I.add(sampling_bn_.variable(nod));
      if (hardEv_ != nullptr && hardEv_->exists(nod)) {
        I.chgVal(sampling_bn_.variable(nod), (*hardEv_)[nod]);
      } else {
        drawVarMonteCarlo__(nod, &I);
      }
    }
    return I;
  }

  template < typename GUM_SCALAR >
  void GibbsOperator< GUM_SCALAR >::drawVarMonteCarlo__(NodeId         nod,
                                                        Instantiation* I) {
    gum::Instantiation Itop(*I);
    Itop.erase(sampling_bn_.variable(nod));
    I->chgVal(sampling_bn_.variable(nod),
              sampling_bn_.cpt(nod).extract(Itop).draw());
  }


  template < typename GUM_SCALAR >
  Instantiation GibbsOperator< GUM_SCALAR >::nextSample(Instantiation prev) {
    for (Idx i = 0; i < nbr_; i++) {
      auto pos = atRandom_ ? randomValue(samplingNodes_.size())
                           : (counting_ % samplingNodes_.size());
      this->GibbsSample__(samplingNodes_[pos], &prev);
      counting_++;
    }
    return prev;
  }
  /// change in Instantiation I a new drawn value for id

  template < typename GUM_SCALAR >
  void GibbsOperator< GUM_SCALAR >::GibbsSample__(NodeId id, Instantiation* I) {
    gum::Instantiation Itop(*I);
    Itop.erase(sampling_bn_.variable(id));
    gum::Potential< GUM_SCALAR > p = sampling_bn_.cpt(id).extract(Itop);
    for (const auto nod: sampling_bn_.children(id))
      p *= sampling_bn_.cpt(nod).extract(Itop);
    GUM_ASSERT(p.nbrDim() == 1);
    if (p.sum() != 0) {
      p.normalize();
      I->chgVal(sampling_bn_.variable(id), p.draw());
    }
  }
}   // namespace gum
