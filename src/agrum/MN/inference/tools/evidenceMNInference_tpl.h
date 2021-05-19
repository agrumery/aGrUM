/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * JointTargetedMNInference.
 */


namespace gum {


  // Default Constructor
  template < typename GUM_SCALAR >
  EvidenceMNInference< GUM_SCALAR >::EvidenceMNInference(
     const IMarkovNet< GUM_SCALAR >* mn) :
      MarkovNetInference< GUM_SCALAR >(mn) {
    // assign a MN if this has not been done before (due to virtual inheritance)
    if (this->hasNoModel_()) {
      MarkovNetInference< GUM_SCALAR >::setMarkovNetDuringConstruction__(mn);
    }

    GUM_CONSTRUCTOR(EvidenceMNInference);
  }


  // Destructor
  template < typename GUM_SCALAR >
  EvidenceMNInference< GUM_SCALAR >::~EvidenceMNInference() {
    GUM_DESTRUCTOR(EvidenceMNInference);
  }


} /* namespace gum */
