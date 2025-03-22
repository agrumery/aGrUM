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
