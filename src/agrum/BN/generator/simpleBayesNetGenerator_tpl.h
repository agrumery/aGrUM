/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
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
 * @author Pierre-Henri WUILLEMIN(@LIP6) and Lionel TORTI and Ariele-Paolo MAESANO
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
        Size nbrNodes,
        Size maxArcs,
        Size maxModality) :
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
    GUM_CONSTRUCTOR ( SimpleBayesNetGenerator )
  }*/

  // Destructor.
  template < typename GUM_SCALAR, template < typename > class ICPTGenerator >
  INLINE SimpleBayesNetGenerator< GUM_SCALAR,
                                  ICPTGenerator >::~SimpleBayesNetGenerator() {
    GUM_DESTRUCTOR(SimpleBayesNetGenerator);
  }

  // Generates a Bayesian network using floats.
  // @param nbrNodes The number of nodes in the generated BN.
  // @param density The probability of adding an arc between two nodes.
  // @return A BNs randomly generated.

  template < typename GUM_SCALAR, template < typename > class ICPTGenerator >
  void SimpleBayesNetGenerator< GUM_SCALAR, ICPTGenerator >::generateBN(
     BayesNet< GUM_SCALAR >& bayesNet) {
    this->bayesNet_ = bayesNet;
    HashTable< Size, NodeId > map;
    std::stringstream         strBuff;

    for (Size i = 0; this->nbrNodes_ > i; ++i) {
      strBuff << "n" << i;
      Size nb_mod
         = (this->maxModality_ == 2) ? 2 : 2 + randomValue(this->maxModality_ - 1);
      map.insert(
         i,
         this->bayesNet_.add(LabelizedVariable(strBuff.str(), "", nb_mod)));
      strBuff.str("");
    }

    // We add arcs
    float density = (float)(this->maxArcs_ * 2)
                  / (float)(this->nbrNodes_ * (this->nbrNodes_ - 1));

    for (Size i = 0; i < this->nbrNodes_; ++i)
      for (Size j = i + 1; j < this->nbrNodes_; ++j)
        if (randomProba() < density) this->bayesNet_.addArc(map[i], map[j]);

    this->fillCPT();

    bayesNet = this->bayesNet_;
  }
} /* namespace gum */
