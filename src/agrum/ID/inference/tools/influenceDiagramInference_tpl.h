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
 * @brief Implementation of the non pure virtual methods of
 * InfluenceDiagramInference.
 */

// to ease parsers in IDE
#include <agrum/tools/graphicalModels/inference/graphicalModelInference.h>
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
