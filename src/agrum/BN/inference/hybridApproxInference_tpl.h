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
 * @brief Implements approximate inference algorithms from Loopy Belief Propagation.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN
 *
 */


#include <agrum/BN/inference/hybridApproxInference.h>
#define DEFAULT_LBP_MAX_ITER 20

namespace gum {


  template <typename GUM_SCALAR, template <typename> class APPROX>
  HybridApproxInference<GUM_SCALAR, APPROX>::HybridApproxInference(const IBayesNet<GUM_SCALAR>* BN)
  : APPROX<GUM_SCALAR>(BN){

 		GUM_CONSTRUCTOR(HybridApproxInference);

	}


  template <typename GUM_SCALAR, template <typename> class APPROX>
  HybridApproxInference<GUM_SCALAR, APPROX>::~HybridApproxInference() {

     GUM_DESTRUCTOR(HybridApproxInference);

  }


  template <typename GUM_SCALAR, template <typename> class APPROX>
  void HybridApproxInference<GUM_SCALAR, APPROX>::_makeInference(){

		LoopyBeliefPropagation<GUM_SCALAR> lbp (&this->BN());
  	   lbp.setMaxIter(DEFAULT_LBP_MAX_ITER);
  	   lbp.makeInference();

		const auto &bn = this->BN();
  		auto hardEv = this->hardEvidence();
  		auto hardEvNodes = this->hardEvidenceNodes();

  		if (!this->isContextualized)
  			this->contextualize();

  	  	if (!this->isSetEstimator)
  	  		this->_setEstimatorFromLBP(&lbp, this->hardEvidenceNodes() );

  		this->initApproximationScheme();
		gum::Instantiation Ip;
		float w = .0;//

		// Burn in
		Ip = this->_burnIn();

		do {

			Ip = this->_draw(&w, Ip, bn, hardEvNodes, hardEv);
  			this->__estimator.update(Ip,w);
			this->updateApproximationScheme();

		} while(this->continueApproximationScheme(this->__estimator.confidence()));

		this->isSetEstimator = false;

  }


}
