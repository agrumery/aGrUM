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
 * @brief This file contains Importance sampling class definition.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN
 */


#ifndef GUM_IMPORTANCE_INFERENCE_H
#define GUM_IMPORTANCE_INFERENCE_H

#include <agrum/BN/inference/tools/samplingInference.h>

namespace gum {

  /**
   * @class ImportanceInference importanceInference.h
   *<agrum/BN/inference/importanceInference.h>
   * @brief class for making Importance sampling inference in bayesian networks.
   * @ingroup bn_approximation
   *
   * This class overrides pure function declared in the inherited class
   *ApproximateInference.
   * It defines the way Importance sampling draws a sample.
   *
   */


  template < typename GUM_SCALAR >
  class ImportanceSampling : public SamplingInference< GUM_SCALAR > {
    public:
    /**
     * Default constructor
     */
    explicit ImportanceSampling(const IBayesNet< GUM_SCALAR >* bn);


    /**
     * Destructor
     */
    ~ImportanceSampling() override;

    protected:
    /// draws a defined number of samples without updating the estimators
    Instantiation _burnIn() override;


    /// draws a sample according to Importance sampling
    /**
     * @param w the weight of sample being generated
     * @param prev the previous sample generated
     * @param bn the bayesian network containing the evidence
     * @param hardEvNodes hard evidence nodes
     * @param hardEv hard evidences values
     *
     * uses the Importance sampling method to generate a new sample using an
     * evidence-mutilated Bayesian network.
     * Each node added to the sample (in a topological order) has a weight.
     * The sample's weight is the product of all weights.
     */
    Instantiation _draw(GUM_SCALAR* w, Instantiation prev) override;


    /// modifies the cpts of a BN in order to tend to uniform distributions
    /**
     * @param bn a BN fragment on which we wish to modify CPTs
     * @param epsilon a default parameter used to scale the modification of the
     * distributions
     *
     * For every CPT in the BN, epsilon is added to each potential value before
     * normalizing
     *
     */
    void _unsharpenBN(BayesNetFragment< GUM_SCALAR >* bn, float epsilon);

    /// fired when Bayesian network is contextualized
    /**
     * @param bn the contextualized BayesNetFragment
     * @param targets inference target variables
     * @param hardEvNodes hard evidence nodes
     * @param hardEv hard evidences values
     *
     * Adds the target variables, erases the evidence variables and unsharpens the
     * BN.
     *
     */
    void _onContextualize(BayesNetFragment< GUM_SCALAR >* bn) override;
  };


  extern template class ImportanceSampling< double >;
}   // namespace gum

#include <agrum/BN/inference/importanceSampling_tpl.h>

#endif
