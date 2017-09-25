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
 * @brief Implementation of Weighted Sampling for inference in Bayesian Networks.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN
 */


#include <agrum/BN/inference/weightedSampling.h>


namespace gum {


	/// Default constructor
  template <typename GUM_SCALAR>
  WeightedSampling<GUM_SCALAR>::WeightedSampling(const IBayesNet<GUM_SCALAR>* BN)
  :  ApproximateInference<GUM_SCALAR>(BN) {

  	  this->setBurnIn(0);
     GUM_CONSTRUCTOR(WeightedSampling);

  }


	/// Destructor
  template <typename GUM_SCALAR>
  WeightedSampling<GUM_SCALAR>::~WeightedSampling() {

     GUM_DESTRUCTOR(WeightedSampling);

  }


	/// No burn in needed for Weighted sampling
  template <typename GUM_SCALAR>
  Instantiation WeightedSampling<GUM_SCALAR>::_burnIn(){
   	gum::Instantiation I;
   	return I;
  }



  template <typename GUM_SCALAR>
  Instantiation WeightedSampling<GUM_SCALAR>::_draw(float* w, Instantiation prev, const IBayesNet<GUM_SCALAR>& bn, const NodeSet& hardEvNodes, const NodeProperty<Idx>& hardEv){

	*w = 1.;
	bool wrongValue = false;

	do {

		prev.clear(); wrongValue = false; *w = 1.;

		for (auto nod: this->BN().topologicalOrder()){

				if (this->hardEvidenceNodes().contains(nod)) {

					prev.add(this->BN().variable(nod));
					prev.chgVal(this->BN().variable(nod), this->hardEvidence()[nod] );
					auto localp = this->BN().cpt(nod).get(prev);

					if (localp == 0) {

						wrongValue = true;
						break;

					}

					*w *=  localp;
					continue;

				}

				this->_addVarSample(nod, &prev);


		}

	} while (wrongValue);

	return prev;

  	}
}

