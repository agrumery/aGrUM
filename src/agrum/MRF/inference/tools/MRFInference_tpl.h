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
 * MRFInference.
 */

#include <agrum/MRF/inference/tools/MRFInference.h>

namespace gum {


  // Default Constructor
  template < typename GUM_SCALAR >
  MRFInference< GUM_SCALAR >::MRFInference(const IMarkovRandomField< GUM_SCALAR >* mn) :
      GraphicalModelInference< GUM_SCALAR >(mn) {
    GUM_CONSTRUCTOR(MRFInference);
  }


  // Default Constructor
  template < typename GUM_SCALAR >
  MRFInference< GUM_SCALAR >::MRFInference() {
    GUM_CONSTRUCTOR(MRFInference);
  }


  // Destructor
  template < typename GUM_SCALAR >
  MRFInference< GUM_SCALAR >::~MRFInference() {
    GUM_DESTRUCTOR(MRFInference);
  }


  // Returns a constant reference over the IMarkovRandomField referenced by this class
  template < typename GUM_SCALAR >
  INLINE const IMarkovRandomField< GUM_SCALAR >& MRFInference< GUM_SCALAR >::MRF() const {
    return static_cast< const IMarkovRandomField< GUM_SCALAR >& >(this->model());
  }


  // assigns a new MRF to the inference engine
  template < typename GUM_SCALAR >
  void MRFInference< GUM_SCALAR >::setMRF(const IMarkovRandomField< GUM_SCALAR >* mrf) {
    this->setModel_(mrf);
  }


  // assigns a MRF to a newly constructed inference engine
  template < typename GUM_SCALAR >
  void MRFInference< GUM_SCALAR >::_setMRFDuringConstruction_(
     const IMarkovRandomField< GUM_SCALAR >* mn) {
    this->setModelDuringConstruction_(mn);
  }
} /* namespace gum */
