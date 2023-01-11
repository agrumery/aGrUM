/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


/**
 * @file
 * @brief Implementation of the non pure virtual methods of class
 * JointTargetedInference.
 */


namespace gum {


  // Default Constructor
  template < typename GUM_SCALAR >
  EvidenceInference< GUM_SCALAR >::EvidenceInference(const IBayesNet< GUM_SCALAR >* bn) :
      BayesNetInference< GUM_SCALAR >(bn) {
    // assign a BN if this has not been done before (due to virtual inheritance)
    if (this->hasNoModel_()) {
      BayesNetInference< GUM_SCALAR >::_setBayesNetDuringConstruction_(bn);
    }

    GUM_CONSTRUCTOR(EvidenceInference);
  }


  // Destructor
  template < typename GUM_SCALAR >
  EvidenceInference< GUM_SCALAR >::~EvidenceInference() {
    GUM_DESTRUCTOR(EvidenceInference);
  }


} /* namespace gum */
