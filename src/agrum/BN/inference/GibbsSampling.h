/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
 * @brief This file contains Gibbs sampling class definition.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN
 */


#ifndef GUM_GIBBS_SAMPLING_H
#define GUM_GIBBS_SAMPLING_H

#include <agrum/BN/inference/tools/gibbsOperator.h>
#include <agrum/BN/inference/tools/samplingInference.h>


namespace gum {

  /**
  * @class GibbsSampling gibbsSampling.h
  *<agrum/BN/inference/gibbsSampling.h>
  * @brief class for making Gibbs sampling inference in bayesian networks.
  * @ingroup bn_approximation
  *
  * This class overrides pure function declared in the inherited class
  *ApproximateInference.
  * It defines the way Gibbs sampling draws a sample. It also inherits
  *GibbsOperator
  * which contains Gibbs sampling methods.
  *
  */

  template < typename GUM_SCALAR >
  class GibbsSampling : public SamplingInference< GUM_SCALAR >,
                        public GibbsOperator< GUM_SCALAR > {

    public:
    /**
     * Default constructor
     */
    explicit GibbsSampling(const IBayesNet< GUM_SCALAR >* bn);

    /**
  * Destructor
  */
    ~GibbsSampling() override;

    /**
     * @brief Number of burn in for one iteration.
     * @param b The number of burn in.
     * @throw OutOfLowerBound Raised if b < 1.
     */
    void setBurnIn(Size b) { this->_burn_in = b; };

    /**
     * @brief Returns the number of burn in.
     * @return Returns the number of burn in.
     */
    Size burnIn() const { return this->_burn_in; };

    protected:
    /// draws a defined number of samples without updating the estimators
    Instantiation _burnIn() override;

    /// draws a sample given previous one according to Gibbs sampling
    /**
    * @param w the weight of sample being generated
    * @param prev the previous sample generated
    * @param bn the bayesian network containing the evidence
    * @param hardEvNodes hard evidence nodes
    * @param hardEv hard evidences values
    *
    * Uses the Gibbs sampling method to generate a new sample given the previous
    * one. The method is implemented in the inherited class GibbsOperator. This
    * function only makes the call to it. It consists of choosing one node x to
    * sample, given the instantiation of all other nodes. It requires computing  of
    * P( x \given instantiation_markovblanket(x)).
    */
    Instantiation _draw(float* w, Instantiation prev) override;

    /// draws a Monte Carlo sample
    /**
    * @param bn the reference bayesian network
    *
    * This Monte Carlo sample generates a good starting point for Gibbs sampling,
    * because it returns
    * a sample consistent with the evidence, but it differs from the one
    * implemented in the inherited
    * class Approximate Inference because it also initializes attributes needed for
    * Gibbs sampling.
    */
    Instantiation _monteCarloSample();
  };

  extern template class GibbsSampling< float >;
  extern template class GibbsSampling< double >;
}

#include <agrum/BN/inference/GibbsSampling_tpl.h>
#endif