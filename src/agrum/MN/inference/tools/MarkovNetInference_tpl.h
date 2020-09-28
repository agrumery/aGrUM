
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * MarkovNetInference.
 */

#include <agrum/MN/inference/tools/MarkovNetInference.h>

namespace gum {


  // Default Constructor
  template < typename GUM_SCALAR >
  MarkovNetInference< GUM_SCALAR >::MarkovNetInference(
     const IMarkovNet< GUM_SCALAR >* mn) :
     GraphicalModelInference< GUM_SCALAR >(mn) {
    GUM_CONSTRUCTOR(MarkovNetInference);
  }


  // Default Constructor
  template < typename GUM_SCALAR >
  MarkovNetInference< GUM_SCALAR >::MarkovNetInference() {
    GUM_CONSTRUCTOR(MarkovNetInference);
  }


  // Destructor
  template < typename GUM_SCALAR >
  MarkovNetInference< GUM_SCALAR >::~MarkovNetInference() {
    GUM_DESTRUCTOR(MarkovNetInference);
  }



  // Returns a constant reference over the IMarkovNet referenced by this class
  template < typename GUM_SCALAR >
  INLINE const IMarkovNet< GUM_SCALAR >&
               MarkovNetInference< GUM_SCALAR >::MN() const {
    return static_cast< const IMarkovNet< GUM_SCALAR >& >(this->model());
  }


  // assigns a new MN to the inference engine
  template < typename GUM_SCALAR >
  void
     MarkovNetInference< GUM_SCALAR >::setMN(const IMarkovNet< GUM_SCALAR >* mn) {
    this->setModel_(mn);
  }


  // assigns a MN to a newly constructed inference engine
  template < typename GUM_SCALAR >
  void MarkovNetInference< GUM_SCALAR >::setMarkovNetDuringConstruction__(
     const IMarkovNet< GUM_SCALAR >* mn) {
    this->setModelDuringConstruction_(mn);
  }
} /* namespace gum */
