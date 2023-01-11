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
 * BayesNetInference.
 */

#include <agrum/BN/inference/tools/BayesNetInference.h>

namespace gum {


  // Default Constructor
  template < typename GUM_SCALAR >
  BayesNetInference< GUM_SCALAR >::BayesNetInference(const IBayesNet< GUM_SCALAR >* bn) :
      GraphicalModelInference< GUM_SCALAR >(bn) {
    GUM_CONSTRUCTOR(BayesNetInference);
  }


  // Default Constructor
  template < typename GUM_SCALAR >
  BayesNetInference< GUM_SCALAR >::BayesNetInference() {
    GUM_CONSTRUCTOR(BayesNetInference);
  }


  // Destructor
  template < typename GUM_SCALAR >
  BayesNetInference< GUM_SCALAR >::~BayesNetInference() {
    GUM_DESTRUCTOR(BayesNetInference);
  }


  // Returns a constant reference over the IBayesNet referenced by this class
  template < typename GUM_SCALAR >
  INLINE const IBayesNet< GUM_SCALAR >& BayesNetInference< GUM_SCALAR >::BN() const {
    return static_cast< const IBayesNet< GUM_SCALAR >& >(this->model());
  }


  // assigns a new BN to the inference engine
  template < typename GUM_SCALAR >
  void BayesNetInference< GUM_SCALAR >::setBN(const IBayesNet< GUM_SCALAR >* bn) {
    this->setModel_(bn);
  }


  // assigns a BN to a newly constructed inference engine
  template < typename GUM_SCALAR >
  void BayesNetInference< GUM_SCALAR >::_setBayesNetDuringConstruction_(
     const IBayesNet< GUM_SCALAR >* bn) {
    this->setModelDuringConstruction_(bn);
  }

} /* namespace gum */
