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
  Instantiation
  ImportanceSampling<GUM_SCALAR>::_draw( float*                       w,
                                         Instantiation                prev,
                                         const IBayesNet<GUM_SCALAR>& bn,
                                         const NodeSet&               hardEvNodes,
                                         const NodeProperty<Idx>&     hardEv ) {

    GUM_SCALAR pSurQ = 1.;

    do {

      prev.clear();
      pSurQ = 1.;
      for ( auto ev = hardEvNodes.beginSafe(); ev != hardEvNodes.endSafe();
            ++ev ) {
        prev.add( bn.variable( *ev ) );
        prev.chgVal( bn.variable( *ev ), hardEv[*ev] );
      }

      for ( auto nod : this->BN().topologicalOrder() ) {
        this->_addVarSample( nod, &prev, this->BN() );
        auto probaQ = this->BN().cpt( nod ).get( prev );
        auto probaP = bn.cpt( nod ).get( prev );
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

    GUM_CHECKPOINT;
    for ( auto nod : bn->nodes().asNodeSet() ) {
      Potential<GUM_SCALAR>* p = new Potential<GUM_SCALAR>();
      *p = bn->cpt( nod ).isNonZeroMap().scale( epsilon ) + bn->cpt( nod );
      p->normalizeAsCPT();
      bn->installCPT( nod, p );
    }
  }

  template <typename GUM_SCALAR>
  void ImportanceSampling<GUM_SCALAR>::_onContextualize(
      BayesNetFragment<GUM_SCALAR>* bn,
      const NodeSet&                targets,
      const NodeSet&                hardEvNodes,
      const NodeProperty<Idx>&      hardEv ) {

    GUM_CHECKPOINT;
    Sequence<NodeId> sid;
    for ( NodeSet::iterator ev = hardEvNodes.begin(); ev != hardEvNodes.end();
          ++ev )
      sid << *ev;

    for ( Size i = 0; i < sid.size(); i++ ) {

      bn->uninstallCPT( sid[i] );
      bn->uninstallNode( sid[i] );
    }

    for ( auto targ = targets.begin(); targ != targets.end(); ++targ ) {

      if ( this->BN().dag().exists( *targ ) ) this->addTarget( *targ );
    }

    auto minParam = bn->minNonZeroParam();
    auto minAccepted = this->epsilon() / bn->maxVarDomainSize();

    if ( minParam < minAccepted ) this->_unsharpenBN( bn, minAccepted );
  }
}
