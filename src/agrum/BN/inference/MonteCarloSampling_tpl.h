/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/
#pragma once


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
