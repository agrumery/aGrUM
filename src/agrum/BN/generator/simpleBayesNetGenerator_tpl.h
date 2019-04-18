
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/** @file
 * @brief Source implementation of SimpleBayesNetGenerator
 *
 * @author Pierre-Henri WUILLEMIN and Lionel TORTI and Ariele-Paolo MAESANO
 *
 */

#include <agrum/BN/generator/simpleBayesNetGenerator.h>

namespace gum {

#ifdef _MSC_VER
#  define IBNG IBayesNetGenerator
#else
#  define IBNG IBayesNetGenerator< GUM_SCALAR, ICPTGenerator >
#endif

  // Use the SimpleCPTGenerator for generating the BNs CPT.
  template < typename GUM_SCALAR, template < typename > class ICPTGenerator >
  INLINE
     SimpleBayesNetGenerator< GUM_SCALAR, ICPTGenerator >::SimpleBayesNetGenerator(
        Size nbrNodes, Size maxArcs, Size maxModality) :
      IBNG(nbrNodes, maxArcs, maxModality) {
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
  template < typename GUM_SCALAR, template < typename > class ICPTGenerator >
  INLINE SimpleBayesNetGenerator< GUM_SCALAR,
                                  ICPTGenerator >::~SimpleBayesNetGenerator() {
    GUM_DESTRUCTOR(SimpleBayesNetGenerator);
  }

  // Generates a bayesian network using floats.
  // @param nbrNodes The number of nodes in the generated BN.
  // @param density The probability of adding an arc between two nodes.
  // @return A BNs randomly generated.

  template < typename GUM_SCALAR, template < typename > class ICPTGenerator >
  void SimpleBayesNetGenerator< GUM_SCALAR, ICPTGenerator >::generateBN(
     BayesNet< GUM_SCALAR >& bayesNet) {
    this->_bayesNet = bayesNet;
    HashTable< Size, NodeId > map;
    std::stringstream         strBuff;

    for (Size i = 0; this->_nbrNodes > i; ++i) {
      strBuff << "n" << i;
      Size nb_mod =
         (this->_maxModality == 2) ? 2 : 2 + randomValue(this->_maxModality - 1);
      map.insert(
         i, this->_bayesNet.add(LabelizedVariable(strBuff.str(), "", nb_mod)));
      strBuff.str("");
    }

    // We add arcs
    float density = (float)(this->_maxArcs * 2)
                    / (float)(this->_nbrNodes * (this->_nbrNodes - 1));

    for (Size i = 0; i < this->_nbrNodes; ++i)
      for (Size j = i + 1; j < this->_nbrNodes; ++j)
        if (randomProba() < density) this->_bayesNet.addArc(map[i], map[j]);

    this->fillCPT();

    bayesNet = this->_bayesNet;
  }
} /* namespace gum */
