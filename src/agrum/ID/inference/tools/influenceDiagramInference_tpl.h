/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


/**
 * @file
 * @brief Implementation of the non pure virtual methods of
 * InfluenceDiagramInference.
 */

// to ease parsers in IDE
#include <agrum/base/graphicalModels/inference/graphicalModelInference.h>
#include <agrum/ID/inference/tools/influenceDiagramInference.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  // Default Constructor
  template < typename GUM_SCALAR >
  InfluenceDiagramInference< GUM_SCALAR >::InfluenceDiagramInference(
      const InfluenceDiagram< GUM_SCALAR >* infDiag) :
      GraphicalModelInference< GUM_SCALAR >(infDiag) {
    GUM_CONSTRUCTOR(InfluenceDiagramInference);
  }

  // Destructor
  template < typename GUM_SCALAR >
  InfluenceDiagramInference< GUM_SCALAR >::~InfluenceDiagramInference() {
    GUM_DESTRUCTOR(InfluenceDiagramInference);
  }

  template < typename GUM_SCALAR >
  const InfluenceDiagram< GUM_SCALAR >&
      InfluenceDiagramInference< GUM_SCALAR >::influenceDiagram() const {
    return static_cast< const InfluenceDiagram< GUM_SCALAR >& >(this->model());
  }

  // assigns a new BN to the inference engine
  template < typename GUM_SCALAR >
  void InfluenceDiagramInference< GUM_SCALAR >::setInfluenceDiagram(
      const InfluenceDiagram< GUM_SCALAR >* infdiag) {
    this->setModel_(infdiag);
  }

  // assigns a BN to a newly constructed inference engine
  template < typename GUM_SCALAR >
  void InfluenceDiagramInference< GUM_SCALAR >::_setIDDuringConstruction_(
      const InfluenceDiagram< GUM_SCALAR >* infdiag) {
    this->setModelDuringConstruction_(infdiag);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
