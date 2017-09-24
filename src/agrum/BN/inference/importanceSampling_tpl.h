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
 * @brief Implementation of Importance Sampling for inference in Bayesian Networks.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN
 */


#include <agrum/BN/inference/importanceSampling.h>


namespace gum {

  ///  default constructor
  template <typename GUM_SCALAR>
  ImportanceSampling<GUM_SCALAR>::ImportanceSampling(
      const IBayesNet<GUM_SCALAR>* BN )
      : ApproximateInference<GUM_SCALAR>( BN ) {

    this->setBurnIn( 0 );
    GUM_CONSTRUCTOR( ImportanceSampling );
  }


  ///  destructor
  template <typename GUM_SCALAR>
  ImportanceSampling<GUM_SCALAR>::~ImportanceSampling() {

    GUM_DESTRUCTOR( ImportanceSampling );
  }

  /// no burn in needed for Importance sampling
  template <typename GUM_SCALAR>
  Instantiation ImportanceSampling<GUM_SCALAR>::_burnIn() {
    Instantiation I;
    return I;
  }

  template <typename GUM_SCALAR>
  Instantiation ImportanceSampling<GUM_SCALAR>::_draw( float*        w,
                                                       Instantiation prev ) {

    GUM_SCALAR pSurQ = 1.;

    do {

      prev.clear();
      pSurQ = 1.;
      for ( auto ev : this->hardEvidenceNodes()) {
        prev.add( BN().variable( ev ) );
        prev.chgVal( BN().variable( ev ), this->evidence()[ev] );
      }

      for ( auto nod : this->BN().topologicalOrder() ) {
        this->_addVarSample( nod, &prev);
        auto probaQ = BN().cpt( nod ).get( prev );
        auto probaP = _frag->cpt( nod ).get( prev );
        if ( ( probaP == 0 ) || ( probaQ == 0 ) ) {
          pSurQ = 0;
          continue;
        }
        pSurQ = probaP / probaQ;
      }
      if ( pSurQ > 0.0 ) {
        for ( auto ev = hardEvNodes.beginSafe(); ev != hardEvNodes.endSafe();
              ++ev ) {
          pSurQ *= bn.cpt( *ev ).get( prev );
        }
      }
    } while ( pSurQ == 0 );

    *w = pSurQ;
    return prev;
  }


  template <typename GUM_SCALAR>
  void
  ImportanceSampling<GUM_SCALAR>::_unsharpenBN( BayesNetFragment<GUM_SCALAR>* bn,
                                                float epsilon ) {
    for ( auto nod : bn->nodes().asNodeSet() ) {
      Potential<GUM_SCALAR>* p = new Potential<GUM_SCALAR>();
      *p = bn->cpt( nod ).isNonZeroMap().scale( epsilon ) + bn->cpt( nod );
      p->normalizeAsCPT();
      bn->installCPT( nod, p );
    }
  }

  template <typename GUM_SCALAR>
  void ImportanceSampling<GUM_SCALAR>::_onContextualize(
      BayesNetFragment<GUM_SCALAR>* bn ) {
    /*
  Sequence<NodeId> sid;
  for ( NodeSet::iterator ev = hardEvNodes.begin(); ev != hardEvNodes.end();
        ++ev )
    sid << *ev;
*/
    auto hardEvNodes = this->hardEvidenceNodes();
    auto hardEv = this->hardEvidence();
    auto targets = this->targets();
    GUM_CHECKPOINT;
    for ( auto ev : hardEvNodes ) {

      GUM_CHECKPOINT;
      bn->uninstallCPT( ev );
      GUM_TRACE_VAR( ev );
      GUM_TRACE_VAR( this->hardEvidenceNodes() );
      bn->installCPT( ev, new Potential<GUM_SCALAR>( *this->evidence()[ev] ) );
      GUM_CHECKPOINT;
      // we keep the variables with hard evidence but alone
      // bn->uninstallNode( sid[i] );
    }

    GUM_CHECKPOINT;
    for ( auto targ = targets.begin(); targ != targets.end(); ++targ ) {
      if ( this->BN().dag().exists( *targ ) ) this->addTarget( *targ );
    }

    GUM_CHECKPOINT;
    auto minParam = bn->minNonZeroParam();
    auto minAccepted = this->epsilon() / bn->maxVarDomainSize();

    GUM_CHECKPOINT;
    if ( minParam < minAccepted ) this->_unsharpenBN( bn, minAccepted );
  }
}
