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
 * @brief Implementation of Gibbs inference methods in Bayesian Networks.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN
 */


#include <agrum/BN/inference/tools/gibbsOperator.h>

namespace gum {

  template <typename GUM_SCALAR>
  int GibbsOperator<GUM_SCALAR>::counting = 0;

  template <typename GUM_SCALAR>
  GibbsOperator<GUM_SCALAR>::GibbsOperator(const IBayesNet<GUM_SCALAR>& BN): __bayesNet(BN) {

     GUM_CONSTRUCTOR(GibbsOperator);

  }

  template <typename GUM_SCALAR>
  GibbsOperator<GUM_SCALAR>::~GibbsOperator() {

     GUM_DESTRUCTOR(GibbsOperator);

      for ( auto& elt : __sampling_idx )
        delete ( elt.second );

      for ( auto& elt : __sampling_posterior )
        delete ( elt.second );

      for ( auto& elt : __node_children )
        delete ( elt.second );

      for ( auto& elt : __cpt_idx )
        delete ( elt.second );

  }


   template <typename GUM_SCALAR>
   void GibbsOperator<GUM_SCALAR>::initAttributes(const IBayesNet<GUM_SCALAR>* bn){


		for ( auto node : bn->dag().nodes() ) {

			  const DiscreteVariable& var = bn->variable( node );

			  Potential<GUM_SCALAR>* newPot = new Potential<GUM_SCALAR>();
			  this->__sampling_posterior.insert( node, newPot );
			  ( *newPot ) << var;


			  Instantiation* new_idx = new Instantiation( *newPot );
			  this->__sampling_idx.insert( node, new_idx );


			  std::vector<NodeId>* newChildren = new std::vector<NodeId>();

		     for ( auto chil : bn->dag().children( node ) )
		     	   newChildren->push_back( chil );

		     this->__node_children.insert( node, newChildren );

			  Instantiation* newInst = new Instantiation( bn->cpt( node ) );
			  this->__cpt_idx.insert( node, newInst );


		 	  if (!__hard_evidences.exists(node))
		 	  		sample_nodes.insert(node);

		 }


   }


    /// returns a MC sample
    /// This is not a really sample since we take into account evidence without
    /// care about parent of evidence, etc.
    /// This is just a not-so-bad first sample for GibbsSampler

  template <typename GUM_SCALAR>
  Instantiation GibbsOperator<GUM_SCALAR>::_monteCarloSample(const IBayesNet<GUM_SCALAR>& bn) {

  		this->initAttributes(&bn);

  		gum::Instantiation I;

  		for (auto nod: bn.topologicalOrder()){

		   if (__hard_evidences.exists(nod)) {
		  		I.add(bn.variable(nod));
		  	   this->__setValVar(nod, __hard_evidences[nod], &I, bn); //hardEv[nod]
		  	   continue;

		  	}

		  	this->_addVarSample(nod, &I, bn);
		  	auto choice = I.val(bn.variable(nod));
		  	this->__setValVar(nod, choice, &I, bn);

	  	}

	  return I;

	}


  /// change in Instantiation I a new drawn value for id

  template <typename GUM_SCALAR>
  void GibbsOperator<GUM_SCALAR>::_GibbsSample(NodeId id, Instantiation* I, const IBayesNet<GUM_SCALAR>& bn) {

      const DiscreteVariable& v = bn.variable( id );
      // we have to build P(x \given instantiation_markovblanket(x)) in
      // posterior
      // see Pearl'88, 4.71 p218
      Potential<GUM_SCALAR>& posterior = *__sampling_posterior[id];
      Instantiation&         posterior_idx = *__sampling_idx[id];

      Instantiation* tmp;
      GUM_SCALAR     value;

      for ( posterior_idx.setFirst(); !posterior_idx.end(); posterior_idx.inc() ) {

        Idx current_mod_id = posterior_idx.val( v );
        tmp = __cpt_idx[id];
        tmp->chgVal( v, current_mod_id );
        posterior.set( posterior_idx, value = bn.cpt( id )[*tmp] );

        if ( value == (GUM_SCALAR)0 )
        	  continue;

        if ( __evidences.exists( id ) ) {

          posterior.set( posterior_idx, posterior[posterior_idx] * ( value = ( *__evidences[id] )[posterior_idx] ) );

          if ( value == (GUM_SCALAR)0 )
	            continue;

        }


			for ( std::vector<NodeId>::iterator iter = __node_children[id]->begin();
              iter != __node_children[id]->end();
              iter++ ) {
          tmp = __cpt_idx[*iter];
          tmp->chgVal( v, current_mod_id );

          posterior.set( posterior_idx,
                         posterior[posterior_idx] *
                             ( value = bn.cpt( *iter )[*tmp] ) );

          if ( value == (GUM_SCALAR)0 ) {
            continue;
          }
        }

       }

	    __drawVar( id, I, bn);

  }


   template <typename GUM_SCALAR>
    void GibbsOperator<GUM_SCALAR>::__drawVar( NodeId id, Instantiation* prev, const IBayesNet<GUM_SCALAR>& bn) {


      const DiscreteVariable& v = bn.variable( id );
      Potential<GUM_SCALAR>&  proba = *__sampling_posterior[id];
      GUM_SCALAR              p = (GUM_SCALAR)randomProba();

      // use of __sampling_idx for shrink the number of temporary Instantiation
      Instantiation& I = *__sampling_idx[id];
      Idx            choice = 0;
      // normalisation
      GUM_SCALAR s = (GUM_SCALAR)0;

      for ( I.setFirst(); !I.end(); I.inc() )
     	  s += proba[I];

      if ( s == (GUM_SCALAR)0 ) {
        GUM_ERROR( FatalError,
                   "A normalisation factor is 0 in node " << id << " (" << v
                                                          << ") : "
                                                          << proba );
      }

      // draw value
      for ( I.setFirst(); !I.end(); I.inc() ) {

        if ( proba[I] == (GUM_SCALAR)0 )
        		continue;

        choice = I.val( v );
        GUM_SCALAR q = proba[I] / s;

        if ( p <= q )
          break;

        p -= q;
      }

		__setValVar( id, choice, prev, bn);

    }


    template <typename GUM_SCALAR>
    INLINE void GibbsOperator<GUM_SCALAR>::__setValVar( NodeId id, Idx choice, Instantiation* prev, const IBayesNet<GUM_SCALAR>& bn) {

      const DiscreteVariable& v = bn.variable( id );

      prev->chgVal( v, choice );
      __cpt_idx[id]->chgVal( v, choice );


		for ( std::vector<NodeId>::iterator iter = __node_children[id]->begin();
            iter != __node_children[id]->end();
            iter++ ) {
        __cpt_idx[*iter]->chgVal( v, choice );
      }

    }

	  template <typename GUM_SCALAR>
	  Instantiation GibbsOperator<GUM_SCALAR>::drawNextInstance (float* w , Instantiation prev, const IBayesNet<GUM_SCALAR>& bn){

	 		*w = 1.;

			this->_GibbsSample(*(sample_nodes.begin()+(counting % sample_nodes.size())), &prev, bn);//, hardEvNodes, hardEv, evidence);
	 		counting++;

			return prev;

	  }


   template <typename GUM_SCALAR>
   void GibbsOperator<GUM_SCALAR>::_addVarSample(NodeId nod, Instantiation* I, const IBayesNet<GUM_SCALAR>& bn){

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

   template <typename GUM_SCALAR>
   void GibbsOperator<GUM_SCALAR>::addHardEvidence( NodeId id, Idx pos ) {
   	if (!__hard_evidences.exists(id))	 __hard_evidences.insert( id, pos );

   }

   template <typename GUM_SCALAR>
   INLINE void GibbsOperator<GUM_SCALAR>::eraseHardEvidence( NodeId id ) {

      if ( __hard_evidences.exists( id ) ) __hard_evidences.erase( id );

    }

    template <typename GUM_SCALAR>
    void GibbsOperator<GUM_SCALAR>::addSoftEvidence(
        const Potential<GUM_SCALAR>& pot ) {
      const Sequence<const DiscreteVariable*>& vars = pot.variablesSequence();

      if ( vars.size() != 1 ) {
        GUM_ERROR( SizeError, "The evidence should be one-dimensionnal" );
      }
      __evidences.insert(__bayesNet.nodeId( pot.variable( 0 ) ), &pot );
    }

     template <typename GUM_SCALAR>
    INLINE void GibbsOperator<GUM_SCALAR>::eraseSoftEvidence( NodeId id ) {
      if ( __evidences.exists( id ) ) __evidences.erase( id );
    }

    /// remove all evidence from the graph
    template <typename GUM_SCALAR>
    INLINE void GibbsOperator<GUM_SCALAR>::eraseAllEvidenceOperator() {
      __evidences.clear();
      __hard_evidences.clear();
    }


}
