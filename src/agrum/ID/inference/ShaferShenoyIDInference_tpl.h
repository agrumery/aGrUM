
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
 * @brief Implementations of the classes defined
 * in InfluenceDiagram/inference/ShaferShenoyIDInference.h.
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

    createReduced_();
  }

  template < typename GUM_SCALAR >
  ShaferShenoyIDInference< GUM_SCALAR >::~ShaferShenoyIDInference() {
    GUM_DESTRUCTOR(ShaferShenoyIDInference);
  }
  template < typename GUM_SCALAR >
  void ShaferShenoyIDInference< GUM_SCALAR >::clear() {
    GraphicalModelInference< GUM_SCALAR >::clear();
    reduce_.clear();
    decisionConstraints_.clear();
    partialOrder().clear();
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
  void ShaferShenoyIDInference< GUM_SCALAR >::updateOutdatedStructure_() {
    createReduced_();
    /*if (NoForgetting_) { addNoForgetting_(); }
    reduce();*/
  }
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
  Idx ShaferShenoyIDInference< GUM_SCALAR >::optimalDecision(NodeId decisionId) {
    GUM_ERROR(NotImplementedYet, "tbd asap")
  }

  template < typename GUM_SCALAR >
  std::vector< std::pair< NodeId, Idx > >
     ShaferShenoyIDInference< GUM_SCALAR >::optimalDecisions() {
    GUM_ERROR(NotImplementedYet, "tbd asap")
  }

  template < typename GUM_SCALAR >
  void ShaferShenoyIDInference< GUM_SCALAR >::createReduced_() {
    reduce_.clear();
    const InfluenceDiagram< GUM_SCALAR >& infdiag = this->influenceDiagram();
    if (infdiag.decisionNodeSize() == 0U) return;

    gum::NodeProperty< gum::Size > level;

    // build reduce_
    for (auto node: infdiag.nodes())
      if (!infdiag.isUtilityNode(node))
        reduce_.addNodeWithId(node);
      else
        level.insert(node, 0);   // utility node is level 0

    for (const auto& arc: infdiag.arcs())
      if (reduce_.exists(arc.tail()) && reduce_.exists(arc.head()))
        reduce_.addArc(arc.tail(), arc.head());

    // build partialOrder_
    gum::Size max_level = 0;

    partialOrder_.clear();
    partialOrder_.resize(infdiag.size());
    gum::NodeSet currents;
    for (auto node: infdiag.nodes()) {
      if (infdiag.isUtilityNode(node)) continue;

      if (reduce_.children(node).empty()) {
        currents.clear();
        currents.insert(node);
        level.insert(node, 0);
        while (!currents.empty()) {
          gum::NodeId elt = *(currents.begin());
          currents.erase(elt);

          if (infdiag.isDecisionNode(elt)) partialOrder_[level[elt]].insert(elt);

          for (auto parent: reduce_.parents(elt)) {
            gum::Size lev = 0;
            gum::Size newl;
            bool      ok_to_add = true;
            for (auto child: infdiag.children(parent)) {
              if (!level.exists(child)) {
                ok_to_add = false;
                break;
              }
              newl = level[child];
              if (infdiag.isDecisionNode(child)) newl += 1;
              if (lev < newl) lev = newl;
            }
            if (ok_to_add) {
              currents.insert(parent);
              level.insert(parent, lev);
              if (max_level < lev) max_level = lev;
            }
          }
        }
      }
    }
    partialOrder_.resize(max_level + 1);
  }

  template < typename GUM_SCALAR >
  std::vector< NodeSet > ShaferShenoyIDInference< GUM_SCALAR >::partialOrder() {
    return partialOrder_;
  }
  template < typename GUM_SCALAR >
  void ShaferShenoyIDInference< GUM_SCALAR >::forceNoForgettingAssumption(
     const std::vector< std::string >& seq) {}

  template < typename GUM_SCALAR >
  bool ShaferShenoyIDInference< GUM_SCALAR >::isNoForgettingAssumption() const {
    return NoForgetting_;
  }
  template < typename GUM_SCALAR >
  void ShaferShenoyIDInference< GUM_SCALAR >::releaseNoForgettingAssumption() {}
} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
