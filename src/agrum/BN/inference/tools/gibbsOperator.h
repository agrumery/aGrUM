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
 * @brief This file contains Gibbs sampling (for BNs) class definitions
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN
 */


#ifndef GUM_GIBBS_OPERATOR_H
#define GUM_GIBBS_OPERATOR_H

#include <agrum/multidim/instantiation.h>
#include <agrum/BN/IBayesNet.h>
#include <agrum/multidim/potential.h>

namespace gum {

    /**
     * @class GibbsOperator gibbsOperator.h <agrum/BN/inference/gibbsOperator.h>
  	  * @brief class containing all variables and methods required for Gibbs sampling
  	  *
  	  * @ingroup bn_approximation
     *
     */

    template <typename GUM_SCALAR>
    class GibbsOperator {

    public:

		/**
       * Default constructor
       */
		 GibbsOperator(const IBayesNet<GUM_SCALAR>& BN);

      /**
       * Destructor.
       */
		 virtual ~GibbsOperator();


      /** Evidence management
       * @{
       */

      /**
       * Insert new evidence in GibbsOperator.
       * @warning if an evidence already w.r.t. a given node and a new
       * evidence w.r.t. this node is inserted, the old evidence is removed.
       */
		 virtual void addSoftEvidence (const Potential<GUM_SCALAR>& pot);

      /**
       * Insert new hard evidence in GibbsOperator.
       * @warning if an evidence already w.r.t. a given node and a new
       * evidence w.r.t. this node is inserted, the old evidence is removed.
       */
		 virtual void addHardEvidence( NodeId id, Idx pos );

      /**
       * Remove a given hard evidence from the graph.
       */
		 virtual void eraseHardEvidence( NodeId id);

      /**
       * Remove a given soft evidence from the graph.
       */
		 virtual void eraseSoftEvidence( NodeId id );

      /**
       * Remove all evidence from the graph.
       */
		 virtual void eraseAllEvidenceOperator();
      /** @} */

    protected:

       /// set of nodes that can be sampled
       NodeSet sample_nodes;

       /// number of samples drawn
    	 static int counting;

		 /// draws a Monte Carlo sample
		 /**
		 * @param bn the reference bayesian network
		 *
		 * This Monte Carlo sample generates a good starting point for Gibbs sampling, because it returns
		 * a sample consistent with the evidence, and it also initializes attributes needed for Gibbs sampling.
		 */
		 virtual Instantiation _monteCarloSample(const IBayesNet<GUM_SCALAR>& bn);

		 /// draws next sample of Gibbs sampling
		 virtual Instantiation drawNextInstance (float* w , Instantiation prev, const IBayesNet<GUM_SCALAR>& bn); //, const NodeSet& hardEvNodes);

	private:
       const IBayesNet<GUM_SCALAR>& __bayesNet;

       /// a list of all the evidence stored into the graph
       NodeProperty<const Potential<GUM_SCALAR>*> __evidences;

       /// a list of hard evidence
       NodeProperty<Idx> __hard_evidences;

      /// a table of instantiation for each cpt
       NodeProperty<Instantiation*> __cpt_idx;

       NodeProperty<Instantiation*> __sampling_idx;

       /// a table of children for each node
       NodeProperty<std::vector<NodeId>*> __node_children;

       /// a table of potential for posterior computed in the markov blanket
       NodeProperty<Potential<GUM_SCALAR>*> __sampling_posterior;

       /// intializes all attributes needed for Gibbs sampling
		 virtual void initAttributes(const IBayesNet<GUM_SCALAR>* bn);

		 /// adds a node to current instantiation
		 virtual void _addVarSample(NodeId nod, Instantiation* I, const IBayesNet<GUM_SCALAR>& bn);
		 virtual void _GibbsSample(NodeId id, Instantiation* I, const IBayesNet<GUM_SCALAR>& bn);
		 virtual void __drawVar( NodeId id, Instantiation* prev, const IBayesNet<GUM_SCALAR>& bn);
		 virtual void __setValVar( NodeId id, Idx choice, Instantiation* prev, const IBayesNet<GUM_SCALAR>& bn);


	};

  extern template class GibbsOperator<float>;
  extern template class GibbsOperator<double>;

}

#include <agrum/BN/inference/tools/gibbsOperator_tpl.h>
#endif
