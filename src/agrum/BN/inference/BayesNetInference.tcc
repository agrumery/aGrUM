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
  template <typename T_DATA>
  BayesNetInference<T_DATA>::BayesNetInference( const AbstractBayesNet<T_DATA>& bn ):
      __bayesNet( bn ) {
    GUM_CONSTRUCTOR( BayesNetInference );
  }

  // Destructor
  template <typename T_DATA>
  BayesNetInference<T_DATA>::~BayesNetInference() {
    GUM_DESTRUCTOR( BayesNetInference );
    _invalidateMarginals();
  }

  template <typename T_DATA>
  void BayesNetInference<T_DATA>::_invalidateMarginals() {
    for ( typename Property< Potential<T_DATA> * >::onNodes::iterator it =
            _marginals.begin();it != _marginals.end();++it ) {
      if ( *it )delete( *it );
    }

    _marginals.clear();
  }

  template <typename T_DATA>
  const Potential<T_DATA>& BayesNetInference<T_DATA>::marginal( NodeId id ) {
    if ( ! _marginals.exists( id ) ) {
      _marginals.insert( id, new Potential<T_DATA>() );
      _fillMarginal( id, *_marginals[id] );
    }

    return *_marginals[id];
  }

  template <typename T_DATA>
  const AbstractBayesNet<T_DATA>& BayesNetInference<T_DATA>::bn() const {return __bayesNet;}


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;
