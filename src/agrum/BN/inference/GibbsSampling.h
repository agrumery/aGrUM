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

#include <agrum/BN/inference/tools/approximateInference.h>
#include <agrum/BN/inference/tools/gibbsOperator.h>


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

  template <typename GUM_SCALAR>
  class GibbsSampling : public ApproximateInference<GUM_SCALAR>,
                               public GibbsOperator<GUM_SCALAR> {

    public:
    /**
     * Default constructor
     */
    GibbsSampling( const IBayesNet<GUM_SCALAR>* BN );

    /**
  * Destructor
  */
    virtual ~GibbsSampling();

    protected:
    /// draws a defined number of samples without updating the estimators
    virtual Instantiation _burnIn();

    /// draws a sample given previous one according to Gibbs sampling
    /**
    * @param w the weight of sample being generated
    * @param prev the previous sample generated
    * @param bn the bayesian network containing the evidence
    * @param hardEvNodes hard evidence nodes
    * @param hardEv hard evidences values
    *
    * Uses the Gibbs sampling method to generate a new sample given the previous
* one.
    * The method is implemented in the inherited class GibbsOperator. This function
* only makes the
    * call to it.
*It consists of choosing one node x to sample, given the instantiation of all other
* nodes.
*It requires computing  of P( x \given instantiation_markovblanket(x)).
    */
    virtual Instantiation
    _draw( float*                       w,
           Instantiation                prev ,
           const IBayesNet<GUM_SCALAR>& bn = BayesNet<GUM_SCALAR>(),
           const NodeSet&               hardEvNodes = NodeSet(),
           const NodeProperty<Idx>&     hardEv = NodeProperty<Idx>() );

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
    virtual Instantiation _monteCarloSample( const IBayesNet<GUM_SCALAR>& bn );

    /// fired when Bayesian network is contextualized
    /**
    * @param bn the contextualized BayesNetFragment
    * @param targets inference target variables
    * @param hardEvNodes hard evidence nodes
    * @param hardEv hard evidences values
    *
    * Adds the evidence and target variables.
    *
    */
    virtual void _onContextualize( BayesNetFragment<GUM_SCALAR>* bn,
                                   const NodeSet&                targets,
                                   const NodeSet&                hardEvNodes,
                                   const NodeProperty<Idx>&      hardEv );

    virtual void _onEvidenceAdded( const NodeId id, bool isHardEvidence );
    virtual void _onEvidenceErased( const NodeId id, bool isHardEvidence );
    virtual void _onAllEvidenceErased( bool contains_hard_evidence );
    virtual void _onEvidenceChanged( const NodeId id, bool hasChangedSoftHard );
  };

  extern template class GibbsSampling<float>;
  extern template class GibbsSampling<double>;
}

#include <agrum/BN/inference/GibbsSampling_tpl.h>
#endif
