/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES et Pierre-Henri WUILLEMIN   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 * @file
 * @brief Implementation of Gibbs Sampling for inference in Bayesian Networks.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN
 */

#include <agrum/BN/inference/GibbsSampling.h>

#define GIBBS_SAMPLING_DEFAULT_EPSILON std::exp(-1.6)
#define GIBBS_SAMPLING_DEFAULT_MIN_EPSILON_RATE std::exp(-5)
#define GIBBS_SAMPLING_DEFAULT_BURNIN 300

#define GIBBS_SAMPLING_POURCENT_DRAWN_SAMPLE 50   // percent drawn
#define GIBBS_SAMPLING_DRAWN_AT_RANDOM true

namespace gum {

  ///  default constructor
  template < typename GUM_SCALAR >
  GibbsSampling< GUM_SCALAR >::GibbsSampling(const IBayesNet< GUM_SCALAR >* bn) :
      SamplingInference< GUM_SCALAR >(bn),
      GibbsOperator< GUM_SCALAR >(
         *bn,
         &this->hardEvidence(),
         1 + (bn->size() * GIBBS_SAMPLING_POURCENT_DRAWN_SAMPLE / 100),
         GIBBS_SAMPLING_DRAWN_AT_RANDOM) {
    GUM_CONSTRUCTOR(GibbsSampling);

    this->setEpsilon(GIBBS_SAMPLING_DEFAULT_EPSILON);
    this->setMinEpsilonRate(GIBBS_SAMPLING_DEFAULT_MIN_EPSILON_RATE);
    this->setBurnIn(GIBBS_SAMPLING_DEFAULT_BURNIN);
  }

  /// destructor
  template < typename GUM_SCALAR >
  GibbsSampling< GUM_SCALAR >::~GibbsSampling() {
    GUM_DESTRUCTOR(GibbsSampling);
  }


  template < typename GUM_SCALAR >
  Instantiation GibbsSampling< GUM_SCALAR >::_monteCarloSample() {
    return GibbsOperator< GUM_SCALAR >::monteCarloSample();
  }


  template < typename GUM_SCALAR >
  Instantiation GibbsSampling< GUM_SCALAR >::_burnIn() {
    gum::Instantiation Ip;
    if (this->burnIn() == 0) return Ip;

    GUM_SCALAR w = 1.0f;
    Ip = _monteCarloSample();
    for (Size i = 1; i < this->burnIn(); i++)
      Ip = this->_draw(&w, Ip);

    return Ip;
  }

  /// draws next sample for gibbs sampling

  template < typename GUM_SCALAR >
  Instantiation GibbsSampling< GUM_SCALAR >::_draw(GUM_SCALAR*   w,
                                                   Instantiation prev) {
    *w = 1.0;
    return GibbsOperator< GUM_SCALAR >::nextSample(prev);
  }
}   // namespace gum
