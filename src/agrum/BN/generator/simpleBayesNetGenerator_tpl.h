/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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

/** @file
 * @brief Source implementation of SimpleBayesNetGenerator
 *
 * @author Pierre-Henri WUILLEMIN and Lionel TORTI and Ariele-Paolo MAESANO
 *
 */

#include <agrum/BN/generator/simpleBayesNetGenerator.h>

namespace gum {

#ifdef _MSC_VER
#define IBNG IBayesNetGenerator
#else
#define IBNG IBayesNetGenerator<GUM_SCALAR, ICPTGenerator>
#endif

  // Use the SimpleCPTGenerator for generating the BNs CPT.
  template <typename GUM_SCALAR, template <typename> class ICPTGenerator>
  INLINE
  SimpleBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::SimpleBayesNetGenerator(
      Size nbrNodes, Size maxArcs, Size maxModality )
      : IBNG( nbrNodes, maxArcs, maxModality ) {
    GUM_CONSTRUCTOR( SimpleBayesNetGenerator );
  }

  // Use this constructor if you want to use a different policy for generating
  // CPT than the default one.
  // The cptGenerator will be erased when the destructor is called.
  // @param cptGenerator The policy used to generate CPT.
  /*template <typename GUM_SCALAR, template<class> class ICPTGenerator>
  SimpleBayesNetGenerator<GUM_SCALAR,ICPTGenerator>::SimpleBayesNetGenerator(CPTGenerator*
  cptGenerator ,Size nbrNodes, float density, Size maxModality):
    IBayesNetGenerator<GUM_SCALAR,ICPTGenerator>(cptGenerator
  ,nbrNodes,density,maxModality) {
    GUM_CONSTRUCTOR ( SimpleBayesNetGenerator );
  }*/

  // Destructor.
  template <typename GUM_SCALAR, template <typename> class ICPTGenerator>
  INLINE SimpleBayesNetGenerator<GUM_SCALAR,
                                 ICPTGenerator>::~SimpleBayesNetGenerator() {
    GUM_DESTRUCTOR( SimpleBayesNetGenerator );
  }

  // Generates a bayesian network using floats.
  // @param nbrNodes The number of nodes in the generated BN.
  // @param density The probability of adding an arc between two nodes.
  // @return A BNs randomly generated.

  template <typename GUM_SCALAR, template <typename> class ICPTGenerator>
  void SimpleBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::generateBN(
      BayesNet<GUM_SCALAR>& bayesNet ) {
    IBNG::_bayesNet = bayesNet;
    HashTable<Size, NodeId> map;
    std::stringstream strBuff;
    int               nb_mod;

    for ( Size i = 0; i < IBNG::_nbrNodes; ++i ) {
      strBuff << "n" << i;
      nb_mod = ( IBNG::_maxModality == 2 )
                   ? 2
                   : 2 + rand() % ( IBNG::_maxModality - 1 );
      map.insert(
          i,
          IBNG::_bayesNet.add( LabelizedVariable( strBuff.str(), "", nb_mod ) ) );
      strBuff.str( "" );
    }

    // We add arcs
    float density = (float)( IBNG::_maxArcs * 2 ) /
                    (float)( IBNG::_nbrNodes * ( IBNG::_nbrNodes - 1 ) );
    float p = density * (float)RAND_MAX;

    for ( Size i = 0; i < IBNG::_nbrNodes; ++i )
      for ( Size j = i + 1; j < IBNG::_nbrNodes; ++j )
        if ( ( (float)rand() ) < p ) IBNG::_bayesNet.addArc( map[i], map[j] );

    IBNG::fillCPT();

    bayesNet = IBNG::_bayesNet;
  }
} /* namespace gum */
#undef IBNG
