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
 * @brief Source implementation of DefaultBayesNetGenerator
 *
 * @author Pierre-Henri WUILLEMIN and Lionel TORTI and Ariele-Paolo MAESANO
 *
 */
// ============================================================================
#include <agrum/BN/generator/defaultBayesNetGenerator.h>

// ============================================================================
namespace gum {


// Use the DefaultCPTGenerator for generating the BNs CPT.
  template <typename T_DATA, template<class> class ICPTGenerator> INLINE
  DefaultBayesNetGenerator<T_DATA, ICPTGenerator>::DefaultBayesNetGenerator ( Size nbrNodes,  Size maxArcs, Size maxModality ) :
    AbstractBayesNetGenerator<T_DATA, ICPTGenerator> ( nbrNodes, maxArcs, maxModality ) {
    GUM_CONSTRUCTOR ( DefaultBayesNetGenerator );
  }

// Use this constructor if you want to use a different policy for generating
// CPT than the default one.
// The cptGenerator will be erased when the destructor is called.
// @param cptGenerator The policy used to generate CPT.
  /*template <typename T_DATA, template<class> class ICPTGenerator>
  DefaultBayesNetGenerator<T_DATA,ICPTGenerator>::DefaultBayesNetGenerator(CPTGenerator* cptGenerator ,Size nbrNodes, float density, Size maxModality):
    AbstractBayesNetGenerator<T_DATA,ICPTGenerator>(cptGenerator ,nbrNodes,density,maxModality) {
    GUM_CONSTRUCTOR ( DefaultBayesNetGenerator );
  }*/

// Destructor.
  template <typename T_DATA, template<class> class ICPTGenerator> INLINE
  DefaultBayesNetGenerator<T_DATA, ICPTGenerator>::~DefaultBayesNetGenerator() {
    GUM_DESTRUCTOR ( DefaultBayesNetGenerator );

  }

// Generates a bayesian network using floats.
// @param nbrNodes The number of nodes in the generated BN.
// @param density The probability of adding an arc between two nodes.
// @return A BNs randomly generated.

  template <typename T_DATA, template<class> class ICPTGenerator>
  void DefaultBayesNetGenerator<T_DATA, ICPTGenerator>::generateBN ( BayesNet<T_DATA> & bayesNet ) {
    AbstractBayesNetGenerator<T_DATA, ICPTGenerator>::_bayesNet = bayesNet;
    HashTable<Size, NodeId> map;
    std::stringstream strBuff;
    int nb_mod;
    static int c = 0;
    srand ( time ( NULL ) + c++ );

    for ( Size i = 0; i < AbstractBayesNetGenerator<T_DATA, ICPTGenerator>::_nbrNodes; ++i ) {
      strBuff << "n" << i;
      nb_mod = ( AbstractBayesNetGenerator<T_DATA, ICPTGenerator>::_maxModality == 2 ) ? 2 : 2 + rand() % ( AbstractBayesNetGenerator<T_DATA, ICPTGenerator>::_maxModality - 1 );
      map.insert ( i, AbstractBayesNetGenerator<T_DATA, ICPTGenerator>::_bayesNet.addVariable ( LabelizedVariable ( strBuff.str(), "" , nb_mod ) ) );
      strBuff.str ( "" );
    }

    // We add arcs


    float density = ( float ) ( AbstractBayesNetGenerator<T_DATA, ICPTGenerator>::_maxArcs * 2 ) / ( float ) ( AbstractBayesNetGenerator<T_DATA, ICPTGenerator>::_nbrNodes * ( AbstractBayesNetGenerator<T_DATA, ICPTGenerator>::_nbrNodes - 1 ) );
    float p = density * ( float ) RAND_MAX;

    for ( Size i = 0; i < AbstractBayesNetGenerator<T_DATA, ICPTGenerator>::_nbrNodes; ++i )
      for ( Size j = i + 1; j < AbstractBayesNetGenerator<T_DATA, ICPTGenerator>::_nbrNodes; ++j )
        if ( ( ( float ) rand() ) < p )
          AbstractBayesNetGenerator<T_DATA, ICPTGenerator>::_bayesNet.insertArc ( map[i], map[j] );

    AbstractBayesNetGenerator<T_DATA, ICPTGenerator>::fillCPT();


    bayesNet = AbstractBayesNetGenerator<T_DATA, ICPTGenerator>::_bayesNet;
  }
} /* namespace gum */
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 
