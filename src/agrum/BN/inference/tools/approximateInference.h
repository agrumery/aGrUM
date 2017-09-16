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
 * @brief This file contains general methods for simulation-oriented approximate inference.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN
 */

#ifndef GUM_APPROXIMATE_INFERENCE_H
#define GUM_APPROXIMATE_INFERENCE_H

#include <agrum/multidim/instantiation.h>
#include <agrum/BN/inference/tools/marginalTargetedInference.h>
#include <agrum/core/approximations/approximationScheme.h>
#include <agrum/BN/inference/tools/estimator.h>
#include <agrum/BN/BayesNetFragment.h>
#include <agrum/BN/inference/tools/gibbsOperator.h>

namespace gum {

  /**
   * @class ApproximateInference approximateInference.h
   * <agrum/BN/inference/approximateInference.h>
   * @brief A generic class for making approximate inference in bayesian networks adapted
   *
   * @ingroup bn_approximation
   *
   * The goal of this class is to define the genereal scheme used by all approximate inference algorithms,
   * which are implemented as derived classes of ApproximateInference.
   * This class inherits from MarginalTargetedInference for the handling of marginal targets
   * and from ApproximationScheme.
   */

	template <typename GUM_SCALAR>
  	class ApproximateInference : public MarginalTargetedInference<GUM_SCALAR>, public ApproximationScheme {

   public:
		 // ############################################################################
		 /// @name Constructors / Destructors
		 // ############################################################################
		 /// @{

		 /// default constructor
		 /** @warning By default, all the nodes of the Bayes net are targets.
		  * @warning note that, by aGrUM's rule, the BN is not copied but only
			  * referenced by the inference algorithm. */

		ApproximateInference(const IBayesNet<GUM_SCALAR>* bn);

		/// destructor
		virtual ~ApproximateInference();
		/// @}



    // ############################################################################
    /// @name Probability computations
    // ############################################################################
    /// @{


	 /// Computes and returns the posterior of a variable node.
    /**
     * @returns a const ref to the posterior probability of the variable node.
     * @param var the variable node for which we need a posterior probability
     *
     * @warning for efficiency reasons, the potential is returned by reference.
     * In order to ensure that the potential may still exist even if the Inference object
     * is destroyed, the user has to copy it explicitly.
     *
     *
     * @throw UndefinedElement if variable node is not in the set of targets.
     * @throw NotFound if variable node is not in the BN.
     */
		virtual const Potential<GUM_SCALAR>& _posterior(const DiscreteVariable &var);


    /// Computes and returns the posterior of a node.
    /**
     * @returns a const ref to the posterior probability of the node.
     * @param id the node for which we need a posterior probability
     *
     * @warning for efficiency reasons, the potential is returned by reference.
     * In order to ensure that the potential may still exist even if the Inference object
     * is destroyed, the user has to copy it explicitly.
     *
     *
     * @throw UndefinedElement if node is not in the set of targets.
     * @throw NotFound if node is not in the BN.
     */
		virtual const Potential<GUM_SCALAR>& _posterior(const NodeId id);

    /// Computes and returns the posterior of a node referred by it's name.
    /**
     * @returns a const ref to the posterior probability of the node referred by name.
     * @param name the name of the node for which we need a posterior probability
     *
     * @warning for efficiency reasons, the potential is returned by reference.
     * In order to ensure that the potential may still exist even if the Inference object
     * is destroyed, the user has to copy it explicitly.
     *
     *
     * @throw UndefinedElement if node corresponding to name is not in the set of targets.
     * @throw NotFound if node corresponding to name is not in the BN.
     */
		virtual const Potential<GUM_SCALAR>& _posterior(const std::string &name);
	 /// @}



	 /// Simplifying the bayesian network with relevance reasonning to lighten the computational charge
	 /**
	 * Sets the reference Bayesian Network as a BayesNetFragment after having eliminated nodes
	 * that are idle for simulation and computation, such as barren or d-separated nodes.
	 * Eliminates the arcs from evidence nodes to it's children, after setting new CPT's for them.
	 *
	 */
		virtual void contextualize ();

	 // ############################################################################
    /// @name Estimator objects initializing
    // ############################################################################
    /// @{


	 /// Initializes the estimators object linked to the simulation
    /**
     * @param hardEvidence the set of hard evidence nodes
     *
     * Initializes the estimator object by creating a hashtable between non evidence
     * nodes and a 0-filled potential which will approximate the node's posterior
	  *
     */
		virtual void _setEstimatorFromBN(const NodeSet& hardEvidence);

	 /// Initializes the estimators object linked to the simulation
    /**
     * @param lbp a LoopyBeliefPropagation object
     * @param hardEvidence the set of hard evidence nodes
     *
     * Initializes the estimator object by createing a hashtable between
     * non evidence nodes and the current approximation of the node's posterior
     * obtained by running LoopyBeliefPropagation algorithm
	  *
     */
		virtual void _setEstimatorFromLBP(LoopyBeliefPropagation<GUM_SCALAR>* lbp, const NodeSet& hardEvidence);
	  ///@}

	protected:
	/// Estimator object designed to approximate target posteriors
	Estimator<GUM_SCALAR> __estimator;

	/// whether the Estimator object has been initialized
	bool isSetEstimator = false;

	///whether the referenced Bayesian Network has been "contextualized"
	bool isContextualized = false;

    /// draws samples without updating the estimators
	virtual Instantiation _burnIn () = 0;

	/// draws a sample in the bayesian network given a previous one
	/**
	* @param w the weight of sample being generated
	* @param prev the previous sample generated
	* @param bn the bayesian network containing the evidence
	* @param hardEvNodes hard evidence nodes
	* @param hardEv hard evidences values
	*
	*/
	virtual Instantiation _draw (float* w, Instantiation prev, const IBayesNet<GUM_SCALAR>& bn = IBayesNet<GUM_SCALAR>(), const NodeSet& hardEvNodes = NodeSet(), const NodeProperty<Idx>& hardEv = NodeProperty<Idx>()) = 0;

	/// makes the inference by generating samples
   virtual void _makeInference();

	/// adds a node to current instantiation
	/**
	* @param nod the node to add to the sample
	* @param I the current sample
	* @param bn the bayesian network used for sampling
	*
	* generates random value based on the BN's CPT's and adds the node to the Instantiation with that value
	*/
	virtual void _addVarSample(NodeId nod, Instantiation* I, const IBayesNet<GUM_SCALAR>& bn);

	///draws a sample consistent with the evidence
	/**
	* @param bn the bayesian network used for sampling
	*/
   virtual Instantiation _monteCarloSample(const IBayesNet<GUM_SCALAR>& bn);

   ///fired when Bayesian network is contextualized
   /**
   * @param bn the contextualized BayesNetFragment
   * @param targets inference target variables
   * @param hardEvNodes hard evidence nodes
   * @param hardEv hard evidences values
   *
   */
   virtual void _onContextualize(BayesNetFragment<GUM_SCALAR>* bn, const NodeSet& targets ,const NodeSet& hardEvNodes, const NodeProperty<Idx>& hardEv) = 0;


   virtual void _onEvidenceAdded( const NodeId id, bool isHardEvidence ){};

   virtual void _onEvidenceErased( const NodeId id, bool isHardEvidence ){};

   virtual void _onAllEvidenceErased( bool contains_hard_evidence ){};

   virtual void _onEvidenceChanged( const NodeId id, bool hasChangedSoftHard ){};

   virtual void _onBayesNetChanged( const IBayesNet<GUM_SCALAR>* bn ){};

   virtual void _updateOutdatedBNStructure(){};

   virtual void _updateOutdatedBNPotentials(){};

   virtual void _onMarginalTargetAdded( const NodeId id ){};

   virtual void _onMarginalTargetErased( const NodeId id ) {};

   virtual void _onAllMarginalTargetsAdded(){};

   virtual void _onAllMarginalTargetsErased(){};

	};



  extern template class ApproximateInference<float>;
  extern template class ApproximateInference<double>;
}

#include <agrum/BN/inference/tools/approximateInference_tpl.h>
#endif

