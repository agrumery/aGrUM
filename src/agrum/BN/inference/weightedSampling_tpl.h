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
 * @brief Implementation of Weighted Sampling for inference in Bayesian networks.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN(_at_LIP6)
 */


#include <agrum/BN/inference/weightedSampling.h>


namespace gum {


  /// Default constructor
  template < typename GUM_SCALAR >
  WeightedSampling< GUM_SCALAR >::WeightedSampling(const IBayesNet< GUM_SCALAR >* bn) :
      SamplingInference< GUM_SCALAR >(bn) {
    GUM_CONSTRUCTOR(WeightedSampling)
  }


  /// Destructor
  template < typename GUM_SCALAR >
  WeightedSampling< GUM_SCALAR >::~WeightedSampling() {
    GUM_DESTRUCTOR(WeightedSampling)
  }


  /// No burn in needed for Weighted sampling
  template < typename GUM_SCALAR >
  Instantiation WeightedSampling< GUM_SCALAR >::burnIn_() {
    gum::Instantiation I;
    return I;
  }


  template < typename GUM_SCALAR >
  Instantiation WeightedSampling< GUM_SCALAR >::draw_(GUM_SCALAR* w, Instantiation prev) {
    *w              = 1.0f;
    bool wrongValue = false;
    do {
      prev.clear();
      wrongValue = false;
      *w         = 1.0f;

      for (const auto nod: this->BN().topologicalOrder()) {
        if (this->hardEvidenceNodes().contains(nod)) {
          prev.add(this->BN().variable(nod));
          prev.chgVal(this->BN().variable(nod), this->hardEvidence()[nod]);
          auto localp = this->BN().cpt(nod).get(prev);

          if (localp == 0) {
            wrongValue = true;
            break;
          }

          *w *= localp;
        } else {
          this->addVarSample_(nod, &prev);
        }
      }
    } while (wrongValue);
    return prev;
  }
}   // namespace gum
