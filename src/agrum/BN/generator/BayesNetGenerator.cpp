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
 * @brief Source implementation of BayesNetGenerator
 *
 * @author Pierre-Henri WUILLEMIN and Lionel TORTI
 *
 */
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <iostream>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/generator/BayesNetGenerator.h>
#include <agrum/multidim/labelizedVariable.h>


namespace gum {


  // Default constructor.
  // Use the SimpleCPTGenerator for generating the BNs CPT.
  BayesNetGenerator::BayesNetGenerator() {
    GUM_CONSTRUCTOR ( BayesNetGenerator );
    __cptGenerator = new SimpleCPTGenerator();
  }

  // Use this constructor if you want to use a different policy for generating
  // CPT than the default one.
  // The cptGenerator will be erased when the destructor is called.
  // @param cptGenerator The policy used to generate CPT.
  BayesNetGenerator::BayesNetGenerator ( CPTGenerator* cptGenerator ) {
    GUM_CONSTRUCTOR ( BayesNetGenerator );
    __cptGenerator = cptGenerator;
  }

  // Destructor.
  BayesNetGenerator::~BayesNetGenerator() {
    GUM_DESTRUCTOR ( BayesNetGenerator );
    delete __cptGenerator;
  }

  // Generates a bayesian network using floats.
  // @param nbrNodes The number of nodes in the generated BN.
  // @param density The probability of adding an arc between two nodes.
  // @return A BNs randomly generated.
  BayesNet<float>*
  BayesNetGenerator::generateBNF ( Size nbrNodes, float density, int max_modality ) {
    BayesNet<float>* bayesNet = new BayesNet<float>();
    // First we add nodes
    HashTable<Size, NodeId> map;
    std::stringstream strBuff;
    int nb_mod;
    srand ( time ( NULL ) );

    for ( Size i = 0; i < nbrNodes; ++i ) {
      strBuff << "n" << i;
      nb_mod = ( max_modality == 2 ) ? 2 : 2 + rand() % ( max_modality - 1 );
      map.insert ( i, bayesNet->addVariable ( LabelizedVariable ( strBuff.str(), "" , nb_mod ) ) );
      strBuff.str ( "" );
    }

    // We add arcs
    float p = density * ( float ) RAND_MAX;

    for ( Size i = 0; i < nbrNodes; ++i ) {
      for ( Size j = i + 1; j < nbrNodes; ++j ) {
        if ( ( ( float ) rand() ) < p ) {
          bayesNet->insertArc ( map[i], map[j] );
        }
      }
    }

    // And fill the CPTs
    for ( HashTable<Size, NodeId>::const_iterator iter = map.begin();
          iter != map.end();
          ++iter ) {
      __cptGenerator->generateCPT ( bayesNet->cpt ( *iter ).pos ( bayesNet->variable ( *iter ) ), bayesNet->cpt ( *iter ) );
    }

    return bayesNet;
  }

  // Generates a bayesian network using double.
  // @param nbrNodes The number of nodes in the generated BN.
  // @param density The probability of adding an arc between two nodes.
  // @return A BNs randomly generated.
  BayesNet<double>*
  BayesNetGenerator::generateBND ( Size nbrNodes, float density, int max_modality ) {
    BayesNet<double>* bayesNet = new BayesNet<double>();
    // First we add nodes
    HashTable<Size, NodeId> map;
    std::stringstream strBuff;
    int nb_mod;
    srand ( time ( NULL ) );

    for ( Size i = 0; i < nbrNodes; ++i ) {
      strBuff << "n" << i;
      nb_mod = ( max_modality == 2 ) ? 2 : 2 + rand() % ( max_modality - 1 );
      map.insert ( i, bayesNet->addVariable ( LabelizedVariable ( strBuff.str(),
                                              "" , nb_mod ) ) );
      strBuff.str ( "" );
    }

    // We add arcs
    float p = density * ( float ) RAND_MAX;

    for ( Size i = 0; i < nbrNodes; ++i ) {
      for ( Size j = i + 1; j < nbrNodes; ++j ) {
        if ( ( ( float ) rand() ) < p ) {
          bayesNet->insertArc ( map[i], map[j] );
        }
      }
    }

    // And fill the CPTs
    for ( HashTable<Size, NodeId>::const_iterator iter = map.begin();
          iter != map.end();
          ++iter ) {
      __cptGenerator->generateCPT ( bayesNet->cpt ( *iter ).pos
                                    ( bayesNet->variable ( *iter ) ),
                                    bayesNet->cpt ( *iter ) );
    }

    return bayesNet;
  }


} /* namespace gum */
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
