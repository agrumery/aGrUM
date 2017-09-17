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
 * @brief Implementation of Gibbs Sampling for inference in Bayesian Networks.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN
 */

#include <agrum/BN/inference/GibbsSampling.h>


namespace gum {

  ///  default constructor
  template <typename GUM_SCALAR>
  GibbsSampling<GUM_SCALAR>::GibbsSampling(const IBayesNet<GUM_SCALAR>* BN)
  :  ApproximateInference<GUM_SCALAR>(BN), GibbsOperator<GUM_SCALAR>(*BN) {

     GUM_CONSTRUCTOR(GibbsSampling);

  }

	/// destructor
  template <typename GUM_SCALAR>
  GibbsSampling<GUM_SCALAR>::~GibbsSampling() {

     GUM_DESTRUCTOR(GibbsSampling);

  }


  template <typename GUM_SCALAR>
  Instantiation GibbsSampling<GUM_SCALAR>::_monteCarloSample(const IBayesNet<GUM_SCALAR>& bn){

  		return GibbsOperator<GUM_SCALAR>::_monteCarloSample(bn);

  }


  template <typename GUM_SCALAR>
  Instantiation GibbsSampling<GUM_SCALAR>::_burnIn(){

  	  gum::Instantiation Ip;
  	  float w = 1.;

  	  Ip = _monteCarloSample(this->BN());

	  if (this->burnIn() == 0)
	  		return Ip;

	  for (Size i = 1; i < this->burnIn(); i++)
	  		Ip = this->_draw(&w, Ip);

	  	return Ip;

  }

	/// draws next sample for gibbs sampling

  template <typename GUM_SCALAR>
  Instantiation GibbsSampling<GUM_SCALAR>::_draw(float* w, Instantiation prev, const IBayesNet<GUM_SCALAR>& bn, const NodeSet& hardEvNodes, const NodeProperty<Idx>& hardEv){

		return this->drawNextInstance(w, prev, this->BN());
	}


/* initializing node properties after contextualizing the BN in order for the computation to be lighter */

	 template <typename GUM_SCALAR>
    void GibbsSampling<GUM_SCALAR>::_onContextualize(BayesNetFragment<GUM_SCALAR>* bn, const NodeSet& targets, const NodeSet& hardEvNodes, const NodeProperty<Idx>& hardEv) {

  		for (auto targ = targets.begin(); targ != targets.end(); ++targ)
			this->addTarget(*targ);

	  	for (auto ev = hardEvNodes.begin(); ev != hardEvNodes.end(); ++ev)
	  		this->addEvidence(*ev, hardEv[*ev]);


	 }




  template <typename GUM_SCALAR>
  INLINE void GibbsSampling<GUM_SCALAR>::_onEvidenceAdded( const NodeId id,
                                                            bool isHardEvidence ) {
    if ( isHardEvidence )
      this->addHardEvidence( id, this->hardEvidence()[id] );
    else {
      this->addSoftEvidence( *( this->evidence()[id] ) );
    }

  }

  template <typename GUM_SCALAR>
  INLINE void GibbsSampling<GUM_SCALAR>::_onEvidenceErased( const NodeId id,
                                                 bool isHardEvidence ) {
    if ( isHardEvidence )
      this->eraseHardEvidence( id );

  }


  template <typename GUM_SCALAR>
  INLINE void GibbsSampling<GUM_SCALAR>::_onAllEvidenceErased( bool contains_hard_evidence ){
    	this->eraseAllEvidenceOperator();
  }

  template <typename GUM_SCALAR>
  INLINE void
  GibbsSampling<GUM_SCALAR>::_onEvidenceChanged( const NodeId id,
                                                  bool hasChangedSoftHard ) {
    if ( this->hardEvidence().exists( id ) ) {
      // soft evidence has been removed
      	this->eraseSoftEvidence( id );
      	this->addHardEvidence( id, this->hardEvidence()[id] );
    } else {
      // hard evidence has been removed
         this->eraseHardEvidence( id );
      	this->addSoftEvidence( *( this->evidence()[id] ) );
    }
  }



}

