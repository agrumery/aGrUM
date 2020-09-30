
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
 * @brief Implementations of the classes defined in
 * InfluenceDiagram/inference/influenceDiagramInference.h.
 *  @note For deep understanding of the inherent algorithms implemented here,
 *  a strong look at "From Influence Diagrams To Junction Trees, Frank Jensen,
 * Finn V.
 * Jensen, Soren L; Dittmer, 1994" is
 *  highly recommended.
 *  @note triangulation__->eliminationOrder() gives nodes in order in which they
 * disappear, meaning that the first one
 *  is the first one to be eliminated.
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease parsing by IDE
#  include <agrum/ID/inference/ShaferShenoyIDInference.h>

namespace gum {

  template < typename GUM_SCALAR >
  ShaferShenoyIDInference< GUM_SCALAR >::ShaferShenoyIDInference(
     const InfluenceDiagram< GUM_SCALAR >* infDiag) :
      InfluenceDiagramInference< GUM_SCALAR >(infDiag) {
    GUM_CONSTRUCTOR(ShaferShenoyIDInference);
  }

  template < typename GUM_SCALAR >
  ShaferShenoyIDInference< GUM_SCALAR >::~ShaferShenoyIDInference() {
    GUM_DESTRUCTOR(ShaferShenoyIDInference);
  }
  template < typename GUM_SCALAR >
  void ShaferShenoyIDInference< GUM_SCALAR >::clear() {
    GraphicalModelInference< GUM_SCALAR >::clear();
  }
  template < typename GUM_SCALAR >
  void ShaferShenoyIDInference< GUM_SCALAR >::onStateChanged_() {}
  template < typename GUM_SCALAR >
  void
     ShaferShenoyIDInference< GUM_SCALAR >::onEvidenceAdded_(const NodeId id,
                                                             bool isHardEvidence) {

  }
  template < typename GUM_SCALAR >
  void ShaferShenoyIDInference< GUM_SCALAR >::onEvidenceErased_(
     const NodeId id, bool isHardEvidence) {}
  template < typename GUM_SCALAR >
  void ShaferShenoyIDInference< GUM_SCALAR >::onAllEvidenceErased_(
     bool contains_hard_evidence) {}
  template < typename GUM_SCALAR >
  void ShaferShenoyIDInference< GUM_SCALAR >::onEvidenceChanged_(
     const NodeId id, bool hasChangedSoftHard) {}
  template < typename GUM_SCALAR >
  void ShaferShenoyIDInference< GUM_SCALAR >::onModelChanged_(
     const GraphicalModel* model) {}
  template < typename GUM_SCALAR >
  void ShaferShenoyIDInference< GUM_SCALAR >::updateOutdatedStructure_() {}
  template < typename GUM_SCALAR >
  void ShaferShenoyIDInference< GUM_SCALAR >::updateOutdatedPotentials_() {}

  template < typename GUM_SCALAR >
  void ShaferShenoyIDInference< GUM_SCALAR >::makeInference_() {
    GUM_ERROR(NotImplementedYet, "tbd asap")
  }

  template < typename GUM_SCALAR >
  GUM_SCALAR ShaferShenoyIDInference< GUM_SCALAR >::MEU() {
    GUM_ERROR(NotImplementedYet, "tbd asap")
  }

  template < typename GUM_SCALAR >
  Idx ShaferShenoyIDInference< GUM_SCALAR >::optimalDecision(
     NodeId decisionId) {
    GUM_ERROR(NotImplementedYet, "tbd asap")
  }

  template < typename GUM_SCALAR >
  std::vector< std::pair< NodeId, Idx > >
     ShaferShenoyIDInference< GUM_SCALAR >::optimalDecisions() {
    GUM_ERROR(NotImplementedYet, "tbd asap")
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
