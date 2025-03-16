/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/





/** @file
 * @brief Source implementation of SimpleBayesNetGenerator
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Lionel TORTI and Ariele-Paolo MAESANO
 *
 */
#include <agrum/BN/generator/simpleBayesNetGenerator.h>

#include <agrum/base/core/utils_random.h>

namespace gum {

  // Use the SimpleCPTGenerator for generating the BNs CPT.
  template < typename GUM_SCALAR, template < typename > class ICPTGenerator >
  INLINE SimpleBayesNetGenerator< GUM_SCALAR, ICPTGenerator >::SimpleBayesNetGenerator(
      Size nbrNodes,
      Size maxArcs,
      Size maxModality) :
      IBayesNetGenerator< GUM_SCALAR, ICPTGenerator >(nbrNodes, maxArcs, maxModality) {
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
  INLINE SimpleBayesNetGenerator< GUM_SCALAR, ICPTGenerator >::~SimpleBayesNetGenerator() {
    GUM_DESTRUCTOR(SimpleBayesNetGenerator);
  }

  // Generates a Bayesian network using floats.
  // @param nbrNodes The number of nodes in the generated BN.
  // @param density The probability of adding an arc between two nodes.
  // @return A BNs randomly generated.

  template < typename GUM_SCALAR, template < typename > class ICPTGenerator >
  void SimpleBayesNetGenerator< GUM_SCALAR, ICPTGenerator >::generateBN(
      BayesNet< GUM_SCALAR >& bayesNet) {
    this->dag_.clear();
    this->dag_.addNodes(this->nbrNodes_);

    // We add arcs
    float density = (float)(this->maxArcs_ * 2) / (float)(this->nbrNodes_ * (this->nbrNodes_ - 1));
    for (Size i = 0; i < this->nbrNodes_; ++i)
      for (Size j = i + 1; j < this->nbrNodes_; ++j)
        if (randomProba() < density) this->dag_.addArc(i, j);

    // Adding arcs until we reach the maxArcs_ number
    while (this->dag_.sizeArcs() < this->maxArcs_) {
      Size i = randomValue(this->nbrNodes_);
      Size j = randomValue(this->nbrNodes_);
      if (i != j) {
        if (i > j) std::swap(i, j);

        if (!this->dag_.existsArc(i, j)) this->dag_.addArc(i, j);
      }
    }

    this->fromDAG(bayesNet);
    this->fillCPT(bayesNet);
  }
} /* namespace gum */
