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

  // Use the SimpleCPTGenerator for generating the BNs CPT.
  template <typename GUM_SCALAR, template <class> class ICPTGenerator>
  INLINE SimpleBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::SimpleBayesNetGenerator(
      Size nbrNodes, Size maxArcs, Size maxModality)
      : IBayesNetGenerator<GUM_SCALAR, ICPTGenerator>(nbrNodes, maxArcs,
                                                      maxModality) {
    GUM_CONSTRUCTOR(SimpleBayesNetGenerator);
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
  template <typename GUM_SCALAR, template <class> class ICPTGenerator>
  INLINE SimpleBayesNetGenerator<GUM_SCALAR,
                                 ICPTGenerator>::~SimpleBayesNetGenerator() {
    GUM_DESTRUCTOR(SimpleBayesNetGenerator);
  }

  // Generates a bayesian network using floats.
  // @param nbrNodes The number of nodes in the generated BN.
  // @param density The probability of adding an arc between two nodes.
  // @return A BNs randomly generated.

  template <typename GUM_SCALAR, template <class> class ICPTGenerator>
  void SimpleBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::generateBN(
      BayesNet<GUM_SCALAR> &bayesNet) {
    IBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet = bayesNet;
    HashTable<Size, NodeId> map;
    std::stringstream strBuff;
    int nb_mod;

    for (Size i = 0; i < IBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_nbrNodes;
         ++i) {
      strBuff << "n" << i;
      nb_mod = (IBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_maxModality == 2)
                   ? 2
                   : 2 +
                         rand() % (IBayesNetGenerator<GUM_SCALAR,
                                                      ICPTGenerator>::_maxModality -
                                   1);
      map.insert(i, IBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.add(
                        LabelizedVariable(strBuff.str(), "", nb_mod)));
      strBuff.str("");
    }

    // We add arcs
    float density =
        (float)(IBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_maxArcs * 2) /
        (float)(IBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_nbrNodes *
                (IBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_nbrNodes - 1));
    float p = density * (float)RAND_MAX;

    for (Size i = 0; i < IBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_nbrNodes;
         ++i)
      for (Size j = i + 1;
           j < IBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_nbrNodes; ++j)
        if (((float)rand()) < p)
          IBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.addArc(map[i],
                                                                          map[j]);

    IBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::fillCPT();

    bayesNet = IBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet;
  }
} /* namespace gum */
