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
 * @brief This file contains Monte Carlo sampling class definition.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN
 */


#ifndef GUM_MONTE_CARLO_INFERENCE_H
#define GUM_MONTE_CARLO_INFERENCE_H

#include <agrum/BN/inference/tools/samplingInference.h>

namespace gum {

  /**
      * @class MonteCarloInference monteCarloInference.h
      *<agrum/BN/inference/monteCarloInference.h>
      * @brief class for making Monte Carlo sampling inference in bayesian
    *networks.
      * @ingroup bn_approximation
      *
      * This class overrides pure function declared in the inherited class
    *ApproximateInference.
      * It defines the way Monte Carlo sampling draws a sample.
      *
      */


  template < typename GUM_SCALAR >
  class MonteCarloSampling : public SamplingInference< GUM_SCALAR > {

    public:
    /**
     * Default constructor
     */
    explicit MonteCarloSampling(const IBayesNet< GUM_SCALAR >* bn);

    /**
     * Destructor
     */
    ~MonteCarloSampling() override;

    protected:
    /// draws a defined number of samples without updating the estimators
    Instantiation _burnIn() override;

    /// draws a sample according to classic Monte Carlo sampling
    /**
    * @param w the weight of sample being generated
    * @param prev the previous sample generated
    * @param bn the bayesian network containing the evidence
    * @param hardEvNodes hard evidence nodes
    * @param hardEv hard evidences values
    *
    * Generates a new sample using forward sampling, rejecting
    * samples not consistent with evidence
    *
    */
    Instantiation _draw(float* w, Instantiation prev) override;
  };

  extern template class MonteCarloSampling< float >;

  extern template class MonteCarloSampling< double >;
}

#include <agrum/BN/inference/MonteCarloSampling_tpl.h>

#endif