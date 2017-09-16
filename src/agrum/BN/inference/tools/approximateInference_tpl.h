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
 * @brief Implementation of the non pure virtual methods of class
 * ApproximateInference.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN
 */

#include <agrum/BN/inference/tools/approximateInference.h>
#include <agrum/BN/algorithms/barrenNodesFinder.h>
#include <agrum/BN/BayesNetFragment.h>
#include <agrum/BN/algorithms/dSeparation.h>


#define DEFAULT_MAXITER 10000000
#define DEFAULT_MIN_EPSILON_RATE 1e-2
#define DEFAULT_PERIOD_SIZE 500
#define DEFAULT_VERBOSITY false
#define DEFAULT_BURNIN 2000
#define DEFAULT_TIMEOUT 6000
#define DEFAULT_EPSILON 1e-2


namespace gum {

	template <typename GUM_SCALAR>
	ApproximateInference<GUM_SCALAR>::ApproximateInference (const IBayesNet<GUM_SCALAR>* bn) :
	MarginalTargetedInference<GUM_SCALAR>(bn), ApproximationScheme(), __estimator() {

	 setEpsilon(DEFAULT_EPSILON);
	 setMinEpsilonRate( DEFAULT_MIN_EPSILON_RATE );
	 setMaxIter( DEFAULT_MAXITER );
	 setVerbosity( DEFAULT_VERBOSITY );
	 setPeriodSize( DEFAULT_PERIOD_SIZE );
	 setMaxTime( DEFAULT_TIMEOUT);
	 setBurnIn(DEFAULT_BURNIN);
	 GUM_CONSTRUCTOR(ApproximateInference);

	}


	template <typename GUM_SCALAR>
	ApproximateInference<GUM_SCALAR>::~ApproximateInference() {

		GUM_DESTRUCTOR( ApproximateInference );

  	}


  template <typename GUM_SCALAR>
  	void ApproximateInference<GUM_SCALAR>::_setEstimatorFromBN(const NodeSet& hardEvidence){

  		__estimator.setFromBN(&this->BN(), hardEvidence);
  		this->isSetEstimator = true;

  }

   template <typename GUM_SCALAR>
  	void ApproximateInference<GUM_SCALAR>::_setEstimatorFromLBP(LoopyBeliefPropagation<GUM_SCALAR>* lbp, const NodeSet& hardEvidence){

  		__estimator.setFromLBP(lbp, hardEvidence);
  		this->isSetEstimator = true;

  }


   template <typename GUM_SCALAR>
   const Potential<GUM_SCALAR>& ApproximateInference<GUM_SCALAR>::_posterior(const DiscreteVariable &var){

		return __estimator.posterior(var);

	}

	template <typename GUM_SCALAR>
   const Potential<GUM_SCALAR>& ApproximateInference<GUM_SCALAR>::_posterior(const NodeId id){

   	return __estimator.posterior(this->BN().variable(id));

   }

	template <typename GUM_SCALAR>
   const Potential<GUM_SCALAR>& ApproximateInference<GUM_SCALAR>::_posterior(const std::string &name ){

   	return __estimator.posterior(this->BN().variableFromName(name));

   }

  	template <typename GUM_SCALAR>
   void ApproximateInference<GUM_SCALAR>::contextualize (){

  		// Finding Barren nodes

  		BarrenNodesFinder barr_nodes = BarrenNodesFinder(&this->BN().dag());
		barr_nodes.setTargets (&this->targets()); barr_nodes.setEvidence(&this->hardEvidenceNodes());
		NodeSet barren = barr_nodes.barrenNodes();

		//creating BN fragment

	   BayesNetFragment<GUM_SCALAR>* frag = new BayesNetFragment<GUM_SCALAR>(this->BN());
		for (auto elmt: this->BN().dag().asNodeSet()-barren )
			frag->installNode(elmt);

		// D-separated nodes

  		dSeparation dsep = gum::dSeparation();
		NodeSet requisite;

		dsep.requisiteNodes(frag->dag(),  this->targets(), this->hardEvidenceNodes(), this->softEvidenceNodes(), requisite);
		requisite += this->hardEvidenceNodes() + this->targets();

		auto nonRequisite = frag->dag().asNodeSet()-requisite;

		for (auto elmt: nonRequisite)
			frag->uninstallNode(elmt);

  		for (auto hard: this->hardEvidenceNodes()) {

  			const auto& children = frag->dag().children(hard);

  			gum::Sequence<gum::NodeId> sid;
  			for (gum::NodeSet::iterator child = children.begin(); child != children.end(); ++child)
	  			sid << *child;

		  	gum::Instantiation I;
			I.add(this->BN().variable(hard));
			I.chgVal(this->BN().variable(hard), this->hardEvidence()[hard]);

  			for (Size i = 0; i < sid.size(); i++) {

				gum::Potential<GUM_SCALAR>* p =  new gum::Potential<GUM_SCALAR>();
				*p = this->BN().cpt(sid[i]).extract(I);
				frag->installCPT(sid[i], p);

			}

  		}

  		auto hardEvNodes = this->hardEvidenceNodes();
  		auto hardEv = this->hardEvidence();
  		auto targets = this->targets();

  		this->setBN(frag);
  		this->isContextualized = true;
		this->_onContextualize(frag, targets, hardEvNodes, hardEv);

  }


  template <typename GUM_SCALAR>
  void ApproximateInference<GUM_SCALAR>::_makeInference(){

  		const auto &bn = this->BN();
  		auto hardEv = this->hardEvidence();
  		auto hardEvNodes = this->hardEvidenceNodes();

  		if (!isContextualized)
  			this->contextualize();

  		if (!isSetEstimator)
  	   	this->_setEstimatorFromBN(hardEvNodes);

  		initApproximationScheme();
		gum::Instantiation Ip;
		float w = .0;//

		// Burn in
		Ip = this->_burnIn();

		do {

			Ip = this->_draw(&w, Ip, bn, hardEvNodes, hardEv);
  			__estimator.update(Ip,w);
			updateApproximationScheme();

		} while(continueApproximationScheme(__estimator.confidence()));

		this->isSetEstimator = false;

	}


  template <typename GUM_SCALAR>
  void ApproximateInference<GUM_SCALAR>::_addVarSample(NodeId nod, Instantiation* I, const IBayesNet<GUM_SCALAR>& bn){

		int val = 0;

		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

		gum::Instantiation Itop = gum::Instantiation(bn.cpt(nod));
		gum::Potential<GUM_SCALAR> p = bn.cpt(nod).extract(*I);


	  	while (!Itop.end()){

			r -= p.get(Itop);

		 	if (r <= 0){
				val = Itop.val(0);
				break;
			}

			Itop.inc();
		}

		I->add(bn.variable(nod));
		I->chgVal(bn.variable(nod), val);

  }


  /* draws a sample consistent with the evidence */

  template <typename GUM_SCALAR>
  Instantiation ApproximateInference<GUM_SCALAR>::_monteCarloSample(const IBayesNet<GUM_SCALAR>& bn){

      gum::Instantiation I; //= new Instantiation();

      for (auto nod: bn.topologicalOrder()){

		  	if (this->hardEvidenceNodes().contains(nod)) {

			  	I.add(bn.variable(nod));
				I.chgVal(bn.variable(nod), this->hardEvidence()[nod] );
		  		continue;

		  	}

		  	ApproximateInference::_addVarSample(nod, &I, bn);

	  	}

	  return I;

  }

}

