
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


/**
 * @file
 * @brief Implementation of the non pure virtual methods of class
 * JointTargetedInference.
 */


namespace gum {


  // Default Constructor
  template < typename GUM_SCALAR >
  EvidenceInference< GUM_SCALAR >::EvidenceInference(
     const IBayesNet< GUM_SCALAR >* bn) :
      BayesNetInference< GUM_SCALAR >(bn) {
    // assign a BN if this has not been done before (due to virtual inheritance)
    if (this->__bn == nullptr) {
      BayesNetInference< GUM_SCALAR >::__setBayesNetDuringConstruction(bn);
    }

    GUM_CONSTRUCTOR(EvidenceInference);
  }


  // Destructor
  template < typename GUM_SCALAR >
  EvidenceInference< GUM_SCALAR >::~EvidenceInference() {
    GUM_DESTRUCTOR(EvidenceInference);
  }


} /* namespace gum */
