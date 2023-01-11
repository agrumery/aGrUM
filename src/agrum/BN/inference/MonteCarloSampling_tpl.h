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
 * @brief Implementation of Monte Carlo Sampling for inference in Bayesian
 * Networks.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN(_at_LIP6)
 */


#include <agrum/BN/inference/MonteCarloSampling.h>


namespace gum {

  /// Default constructor
  template < typename GUM_SCALAR >
  MonteCarloSampling< GUM_SCALAR >::MonteCarloSampling(const IBayesNet< GUM_SCALAR >* bn) :
      SamplingInference< GUM_SCALAR >(bn) {
    GUM_CONSTRUCTOR(MonteCarloSampling);
  }

  /// Destructor
  template < typename GUM_SCALAR >
  MonteCarloSampling< GUM_SCALAR >::~MonteCarloSampling() {
    GUM_DESTRUCTOR(MonteCarloSampling);
  }

  /// no burn in needed for Monte Carlo sampling
  template < typename GUM_SCALAR >
  Instantiation MonteCarloSampling< GUM_SCALAR >::burnIn_() {
    gum::Instantiation I;
    return I;
  }


  template < typename GUM_SCALAR >
  Instantiation MonteCarloSampling< GUM_SCALAR >::draw_(GUM_SCALAR* w, Instantiation prev) {
    *w               = 1.0f;
    bool wrong_value = false;
    do {
      wrong_value = false;
      prev.clear();
      for (const auto nod: this->BN().topologicalOrder()) {
        this->addVarSample_(nod, &prev);
        if (this->hardEvidenceNodes().contains(nod)
            && prev.val(this->BN().variable(nod)) != this->hardEvidence()[nod]) {
          wrong_value = true;
          break;
        }
      }
    } while (wrong_value);
    return prev;
  }
}   // namespace gum
