
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
 * in InfluenceDiagram/inference/ShaferShenoyLIMIDSInference.h.
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease parsing by IDE
#  include <agrum/ID/inference/ShaferShenoyLIMIDSInference.h>

namespace gum {

  template < typename GUM_SCALAR >
  ShaferShenoyLIMIDSInference< GUM_SCALAR >::ShaferShenoyLIMIDSInference(
     const InfluenceDiagram< GUM_SCALAR >* infDiag) :
      InfluenceDiagramInference< GUM_SCALAR >(infDiag) {
    GUM_CONSTRUCTOR(ShaferShenoyLIMIDSInference);

    createReduced_();
  }

  template < typename GUM_SCALAR >
  ShaferShenoyLIMIDSInference< GUM_SCALAR >::~ShaferShenoyLIMIDSInference() {
    GUM_DESTRUCTOR(ShaferShenoyLIMIDSInference);
  }
  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDSInference< GUM_SCALAR >::clear() {
    GraphicalModelInference< GUM_SCALAR >::clear();
    reduced_.clear();
    noForgettingOrder_.clear();
    reversePartialOrder().clear();
  }
  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDSInference< GUM_SCALAR >::onStateChanged_() {}
  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDSInference< GUM_SCALAR >::onEvidenceAdded_(
     const NodeId id, bool isHardEvidence) {}
  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDSInference< GUM_SCALAR >::onEvidenceErased_(
     const NodeId id, bool isHardEvidence) {}
  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDSInference< GUM_SCALAR >::onAllEvidenceErased_(
     bool contains_hard_evidence) {}
  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDSInference< GUM_SCALAR >::onEvidenceChanged_(
     const NodeId id, bool hasChangedSoftHard) {}
  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDSInference< GUM_SCALAR >::onModelChanged_(
     const GraphicalModel* model) {}

  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDSInference< GUM_SCALAR >::updateOutdatedStructure_() {
    createReduced_();
  }
  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDSInference< GUM_SCALAR >::updateOutdatedPotentials_() {}

  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDSInference< GUM_SCALAR >::makeInference_() {
    GUM_ERROR(NotImplementedYet, "tbd asap")
  }

  template < typename GUM_SCALAR >
  GUM_SCALAR ShaferShenoyLIMIDSInference< GUM_SCALAR >::MEU() {
    GUM_ERROR(NotImplementedYet, "tbd asap")
  }

  template < typename GUM_SCALAR >
  Idx ShaferShenoyLIMIDSInference< GUM_SCALAR >::optimalDecision(
     NodeId decisionId) {
    GUM_ERROR(NotImplementedYet, "tbd asap")
  }

  template < typename GUM_SCALAR >
  std::vector< std::pair< NodeId, Idx > >
     ShaferShenoyLIMIDSInference< GUM_SCALAR >::optimalDecisions() {
    GUM_ERROR(NotImplementedYet, "tbd asap")
  }

  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDSInference< GUM_SCALAR >::createReduced_() {
    reduced_.clear();
    const InfluenceDiagram< GUM_SCALAR >& infdiag = this->influenceDiagram();
    if (infdiag.decisionNodeSize() == 0U) return;

    gum::NodeProperty< gum::Size > level;

    // build reduced_
    for (auto node: infdiag.nodes())
      if (!infdiag.isUtilityNode(node))
        reduced_.addNodeWithId(node);
      else
        level.insert(node, 0);   // utility node is level 0

    for (const auto& arc: infdiag.arcs())
      if (reduced_.exists(arc.tail()) && reduced_.exists(arc.head()))
        reduced_.addArc(arc.tail(), arc.head());

    // force no forgetting if necessary
    if (isNoForgettingAssumption()) {
      auto last = *(noForgettingOrder_.begin());
      for (auto node: noForgettingOrder_)
        if (node == last)   // first one
          continue;
        else {   // we dead with last->node
                 // adding the whole family of last as parents of node
          if (!reduced_.existsArc(last, node)) {
            reduced_.addArc(last, node);
          }
          for (auto par: reduced_.parents(last)) {
            if (!reduced_.existsArc(par, node)) reduced_.addArc(par, node);
          }
          last = node;
        }
    }

    // reducing the graph
    gum::Size max_level = 0;
    reversePartialOrder_.clear();
    reversePartialOrder_.resize(infdiag.size());
    gum::NodeSet currents;
    for (auto node: infdiag.nodes()) {
      if (infdiag.isUtilityNode(node)) continue;

      if (reduced_.children(node).empty()) {
        currents.clear();
        currents.insert(node);
        level.insert(node, 0);
        while (!currents.empty()) {
          gum::NodeId elt = *(currents.begin());
          currents.erase(elt);

          if (infdiag.isDecisionNode(elt))
            reversePartialOrder_[level[elt]].insert(elt);

          for (auto parent: reduced_.parents(elt)) {
            gum::Size lev = 0;
            gum::Size newl;
            bool      ok_to_add = true;
            for (auto child: reduced_.children(parent)) {
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
              if (level.exists(parent)) {
                if (level[parent] != lev)
                  GUM_ERROR(InvalidArgument,
                            "Trying to set level["
                               << parent << "] to level=" << lev
                               << " but already is " << level[parent]);
              } else {
                level.insert(parent, lev);
              }

              if (max_level < lev) max_level = lev;
            }
          }
        }
      }
    }
    Size levmax = 0;
    for (const auto& k: reversePartialOrder_) {
      if (k.empty()) break;
      levmax++;
    }
    reversePartialOrder_.resize(levmax);

    // we add utility nodes for finding requisite nodes
    for (auto node: infdiag.nodes()) {
      if (infdiag.isUtilityNode(node)) {
        reduced_.addNodeWithId(node);
        for (auto par: infdiag.parents(node))
          reduced_.addArc(par, node);
      }
    }

    for (const auto& sen: reversePartialOrder_) {
      for (auto n: sen)
        for (auto p: nonRequisiteNodes(n))
          reduced_.eraseArc(Arc(p, n));
    }

    // and then we erase them
    for (auto node: infdiag.nodes()) {
      if (infdiag.isUtilityNode(node)) reduced_.eraseNode(node);
    }

    this->setState_(
       GraphicalModelInference< GUM_SCALAR >::StateOfInference::OutdatedStructure);
  }

  template < typename GUM_SCALAR >
  std::vector< NodeSet >
     ShaferShenoyLIMIDSInference< GUM_SCALAR >::reversePartialOrder() const {
    return reversePartialOrder_;
  }
  template < typename GUM_SCALAR >
  bool
     ShaferShenoyLIMIDSInference< GUM_SCALAR >::isNoForgettingAssumption() const {
    return (!noForgettingOrder_.empty());
  }

  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDSInference< GUM_SCALAR >::addNoForgettingAssumption(
     const std::vector< std::string >& names) {
    addNoForgettingAssumption(this->influenceDiagram().ids(names));
  }

  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDSInference< GUM_SCALAR >::addNoForgettingAssumption(
     const std::vector< NodeId >& ids) {
    const auto& infdiag = this->influenceDiagram();
    for (const auto node: ids) {
      if (!infdiag.exists(node)) GUM_ERROR(NotFound, node << " is not a NodeId");
      if (!infdiag.isDecisionNode(node))
        GUM_ERROR(TypeError,
                  "Node " << node << node << " (" << infdiag.variable(node).name()
                          << ") is not a decision node");
    }
    if (infdiag.decisionNodeSize() != ids.size())
      GUM_ERROR(SizeError,
                "Some decision nodes are missing in the sequence " << ids);

    noForgettingOrder_ = ids;
    createReduced_();
  }
  template < typename GUM_SCALAR >
  NodeSet
     ShaferShenoyLIMIDSInference< GUM_SCALAR >::nonRequisiteNodes(NodeId d) const {
    NodeSet     res;
    const auto& infdiag = this->influenceDiagram();

    if (!infdiag.isDecisionNode(d))
      GUM_ERROR(TypeError, d << " is not a decision node");

    if (infdiag.parents(d).empty()) return res;

    const DAG dag = reducedGraph();
    NodeSet   descUs;
    for (const auto n: dag.descendants(d))
      if (infdiag.isUtilityNode(n)) descUs.insert(n);

    NodeSet cumul{descUs};
    cumul << d;
    auto g = dag.moralizedAncestralGraph(cumul);

    NodeSet family{dag.parents(d)};
    family << d;
    bool notReq;
    for (const auto p: dag.parents(d)) {
      notReq = true;
      for (const auto u: descUs) {
        if (g.hasUndirectedPath(p, u, family)) {
          notReq = false;
          break;
        }
      }
      if (notReq) res << p;
    }

    return res;
  }

  template < typename GUM_SCALAR >
  gum::Idx ShaferShenoyLIMIDSInference< GUM_SCALAR >::optimalDecision(
     std::string decisionName) {
    return optimalDecision(this->influenceDiagram().idFromName(decisionName));
  }

  template < typename GUM_SCALAR >
  NodeSet ShaferShenoyLIMIDSInference< GUM_SCALAR >::nonRequisiteNodes(
     const std::string& dname) const {
    return nonRequisiteNodes(this->influenceDiagram().idFromName(dname));
  }
  template < typename GUM_SCALAR >
  InfluenceDiagram< GUM_SCALAR >
     ShaferShenoyLIMIDSInference< GUM_SCALAR >::reducedLIMID() const {
    const auto&                    infdiag = this->influenceDiagram();
    InfluenceDiagram< GUM_SCALAR > res;
    for (auto node: infdiag.nodes()) {
      if (infdiag.isChanceNode(node))
        res.addChanceNode(infdiag.variable(node), node);
      else if (infdiag.isDecisionNode(node))
        res.addDecisionNode(infdiag.variable(node), node);
    }

    for (const auto& arc: reduced_.arcs()) {
      res.addArc(arc.tail(), arc.head());
    }

    for (auto node: infdiag.nodes()) {
      if (infdiag.isUtilityNode(node)) {
        res.addUtilityNode(infdiag.variable(node), node);
        for (auto par: infdiag.parents(node))
          res.addArc(par, node);
      }
    }

    // Potentials !!!
    return res;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
