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
 * @brief Implementation of the non pure virtual methods of
 * BayesNetInference.
 */

// to ease parsers in IDE
#include <agrum/BN/inference/BayesNetInference.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {


  // Default Constructor
  template <typename GUM_SCALAR>
  BayesNetInference<GUM_SCALAR>::BayesNetInference( const IBayesNet<GUM_SCALAR>& bn ):
    __bayesNet( bn ) {
    GUM_CONSTRUCTOR( BayesNetInference );
  }

  // Destructor
  template <typename GUM_SCALAR>
  BayesNetInference<GUM_SCALAR>::~BayesNetInference() {
    GUM_DESTRUCTOR( BayesNetInference );
    _invalidateMarginals();
  }

  template <typename GUM_SCALAR>
  void BayesNetInference<GUM_SCALAR>::_invalidateMarginals() {
    for ( typename Property< Potential<GUM_SCALAR> * >::onNodes::iterator_safe it =
            _marginals.beginSafe(); it != _marginals.endSafe(); ++it ) {
      if ( *it )delete( *it );
    }

    _marginals.clear();
  }

  template <typename GUM_SCALAR>
  const Potential<GUM_SCALAR>& BayesNetInference<GUM_SCALAR>::marginal( NodeId id ) {
    if ( ! _marginals.exists( id ) ) {
      _marginals.insert( id, new Potential<GUM_SCALAR>() );
      _fillMarginal( id, *_marginals[id] );
    }

    return *_marginals[id];
  }

  template <typename GUM_SCALAR>
  const IBayesNet<GUM_SCALAR>& BayesNetInference<GUM_SCALAR>::bn() const {return __bayesNet;}


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
// kate: indent-mode cstyle; indent-width 2; replace-tabs on;
