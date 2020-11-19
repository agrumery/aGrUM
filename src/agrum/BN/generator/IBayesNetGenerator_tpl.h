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
 * @brief Source implementation of IBayesNetGenerator
 *
 * @author Christophe GONZALES(@AMU), Pierre-Henri WUILLEMIN(@LIP6), Lionel TORTI
 *and Ariele-Paolo MAESANO
 *
 */

#include <agrum/BN/generator/IBayesNetGenerator.h>

namespace gum {

  // Default constructor.
  // Use the SimpleCPTGenerator for generating the BNs CPT.
  template < typename GUM_SCALAR, template < typename > class ICPTGenerator >
  INLINE IBayesNetGenerator< GUM_SCALAR, ICPTGenerator >::IBayesNetGenerator(
     Size nbrNodes, Size maxArcs, Size maxModality) :
      bayesNet_() {
    GUM_CONSTRUCTOR(IBayesNetGenerator);
    nbrNodes_ = nbrNodes;

    if (maxArcs < nbrNodes - 1 || maxArcs > (nbrNodes * (nbrNodes - 1)) / 2)
      GUM_ERROR(OperationNotAllowed, " maxArcs value not possible ");

    if (maxModality < 2)
      GUM_ERROR(OperationNotAllowed,
                " maxModality must be at least equal to two ");

    maxArcs_ = maxArcs;
    maxModality_ = maxModality;
  }

  // Destructor.
  template < typename GUM_SCALAR, template < typename > class ICPTGenerator >
  INLINE IBayesNetGenerator< GUM_SCALAR, ICPTGenerator >::~IBayesNetGenerator() {
    GUM_DESTRUCTOR(IBayesNetGenerator);
    //    delete cptGenerator_;
  }

  template < typename GUM_SCALAR, template < typename > class ICPTGenerator >
  void IBayesNetGenerator< GUM_SCALAR, ICPTGenerator >::fillCPT() {
    for (auto node: bayesNet_.nodes())
      this->generateCPT(bayesNet_.cpt(node).pos(bayesNet_.variable(node)),
                        bayesNet_.cpt(node));
  }

  template < typename GUM_SCALAR, template < typename > class ICPTGenerator >
  INLINE Size
     IBayesNetGenerator< GUM_SCALAR, ICPTGenerator >::maxModality() const {
    return maxModality_;
  }

  template < typename GUM_SCALAR, template < typename > class ICPTGenerator >
  INLINE Size IBayesNetGenerator< GUM_SCALAR, ICPTGenerator >::nbrNodes() const {
    return nbrNodes_;
  }

  template < typename GUM_SCALAR, template < typename > class ICPTGenerator >
  INLINE Size IBayesNetGenerator< GUM_SCALAR, ICPTGenerator >::maxArcs() const {
    return maxArcs_;
  }

  template < typename GUM_SCALAR, template < typename > class ICPTGenerator >
  INLINE void IBayesNetGenerator< GUM_SCALAR, ICPTGenerator >::setMaxModality(
     Size maxModality) {
    if (maxModality < 2)
      GUM_ERROR(OperationNotAllowed,
                " maxModality must be at least equal to two ");

    maxModality_ = maxModality;
  }
  template < typename GUM_SCALAR, template < typename > class ICPTGenerator >
  INLINE void
     IBayesNetGenerator< GUM_SCALAR, ICPTGenerator >::setNbrNodes(Size nbrNodes) {
    if ((maxArcs_ < nbrNodes - 1) || (maxArcs_ > (nbrNodes * (nbrNodes - 1)) / 2))
      GUM_ERROR(OperationNotAllowed, " nbrNodes value not possible ");

    nbrNodes_ = nbrNodes;
  }

  template < typename GUM_SCALAR, template < typename > class ICPTGenerator >
  INLINE void
     IBayesNetGenerator< GUM_SCALAR, ICPTGenerator >::setMaxArcs(Size maxArcs) {
    if (maxArcs < nbrNodes_ - 1 || maxArcs > (nbrNodes_ * (nbrNodes_ - 1)) / 2)
      GUM_ERROR(OperationNotAllowed, " maxArcs value not possible ");

    maxArcs_ = maxArcs;
  }

  // Generates a bayesian network using floats.
  // @param nbrNodes The number of nodes in the generated BN.
  // @param density The probability of adding an arc between two nodes.
  // @return A BNs randomly generated.

} /* namespace gum */
