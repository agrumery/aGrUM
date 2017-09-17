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
 * @brief Implementation of Monte Carlo Sampling for inference in Bayesian Networks.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN
 */


#include <agrum/BN/inference/MonteCarloSampling.h>


namespace gum {

	/// Default constructor
  template <typename GUM_SCALAR>
  MonteCarloSampling<GUM_SCALAR>::MonteCarloSampling(const IBayesNet<GUM_SCALAR>* BN)
  :  ApproximateInference<GUM_SCALAR>(BN) {

  	 this->setBurnIn(0);
    GUM_CONSTRUCTOR(MonteCarloSampling);

  }

  /// Destructor
  template <typename GUM_SCALAR>
  MonteCarloSampling<GUM_SCALAR>::~MonteCarloSampling() {

     GUM_DESTRUCTOR(MonteCarloSampling);

  }

  /// no burn in needed for Monte Carlo sampling
  template <typename GUM_SCALAR>
  Instantiation MonteCarloSampling<GUM_SCALAR>::_burnIn(){
  		gum::Instantiation I;
  	   return I;
  }




  template <typename GUM_SCALAR>
  Instantiation MonteCarloSampling<GUM_SCALAR>::_draw(float* w, Instantiation prev, const IBayesNet<GUM_SCALAR>& bn, const NodeSet& hardEvNodes, const NodeProperty<Idx>& hardEv){

	*w = 1.;
	bool wrong_value = false;

	do{

		wrong_value = false; prev.clear();

		for (auto nod: this->BN().topologicalOrder()){

			  	this->_addVarSample(nod, &prev, this->BN());

				if (this->hardEvidenceNodes().contains(nod) and prev.val(this->BN().variable(nod)) != this->hardEvidence()[nod]) {
						wrong_value = true;
						break;
				}
		}

	} while (wrong_value);

	return prev;

  	}


  template <typename GUM_SCALAR>
    void MonteCarloSampling<GUM_SCALAR>::_onContextualize(BayesNetFragment<GUM_SCALAR>* bn, const NodeSet& targets, const NodeSet& hardEvNodes, const NodeProperty<Idx>& hardEv) {

    	for (auto targ = targets.begin(); targ != targets.end(); ++targ)
			this->addTarget(*targ);

	  	for (auto ev = hardEvNodes.begin(); ev != hardEvNodes.end(); ++ev)
	  		this->addEvidence(*ev, hardEv[*ev]);

    }

}

