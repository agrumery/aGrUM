/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * in InfluenceDiagram/inference/ShaferShenoyLIMIDInference.h.
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease parsing by IDE
#  include <agrum/ID/inference/ShaferShenoyLIMIDInference.h>
#  include <agrum/ID/inference/tools/decisionPotential.h>
#  include <agrum/tools/multidim/potential.h>

namespace gum {

  template < typename GUM_SCALAR >
  ShaferShenoyLIMIDInference< GUM_SCALAR >::ShaferShenoyLIMIDInference(
     const InfluenceDiagram< GUM_SCALAR >* infDiag) :
      InfluenceDiagramInference< GUM_SCALAR >(infDiag) {
    GUM_CONSTRUCTOR(ShaferShenoyLIMIDInference);
    createReduced_();
  }

  template < typename GUM_SCALAR >
  ShaferShenoyLIMIDInference< GUM_SCALAR >::~ShaferShenoyLIMIDInference() {
    GUM_DESTRUCTOR(ShaferShenoyLIMIDInference);
  }
  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDInference< GUM_SCALAR >::clear() {
    GraphicalModelInference< GUM_SCALAR >::clear();
    noForgettingOrder_.clear();
    reduced_.clear();
    reducedJunctionTree_.clear();
    solvabiltyOrder_.clear();
    posteriors_.clear();
    unconditionalDecisions_.clear();
    strategies_.clear();
    reversePartialOrder_.clear();
  }
  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDInference< GUM_SCALAR >::onStateChanged_() {}
  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDInference< GUM_SCALAR >::onEvidenceAdded_(
     const NodeId id, bool isHardEvidence) {
    const InfluenceDiagram< GUM_SCALAR >& infdiag = this->influenceDiagram();
    if (infdiag.isUtilityNode(id)) {
      GUM_ERROR(InvalidNode, "No evidence on a utility node.");
    }
    if (infdiag.isDecisionNode(id)) {
      if (!isHardEvidence)
        GUM_ERROR(InvalidNode, "No soft evidence on a decision node.");
    }
  }
  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDInference< GUM_SCALAR >::onEvidenceErased_(
     const NodeId id, bool isHardEvidence) {}
  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDInference< GUM_SCALAR >::onAllEvidenceErased_(
     bool contains_hard_evidence) {}
  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDInference< GUM_SCALAR >::onEvidenceChanged_(
     const NodeId id, bool hasChangedSoftHard) {}
  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDInference< GUM_SCALAR >::onModelChanged_(
     const GraphicalModel* model) {
    createReduced_();
  }

  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDInference< GUM_SCALAR >::updateOutdatedStructure_() {
    createReduced_();
  }
  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDInference< GUM_SCALAR >::updateOutdatedPotentials_() {}

  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDInference< GUM_SCALAR >::makeInference_() {
    if (!isSolvable()) {
      GUM_ERROR(FatalError, "This LIMID/Influence Diagram is not solvable.")
    }

    PhiNodeProperty phi;
    PsiArcProperty  psi;

    initializingInference_(phi, psi);

    // message passing (using reverse order of solvabilityOrder)
    // first collect of phis into root
    const auto firstRootIndice = 0;
    collectingMessage_(
       phi, psi, node_to_clique_[solvabiltyOrder_[firstRootIndice]]);
    deciding_(phi, psi, solvabiltyOrder_[firstRootIndice]);

    for (Idx nextRootIndice = 1; nextRootIndice < solvabiltyOrder_.size();
         nextRootIndice++) {
      collectingToFollowingRoot_(
         phi,
         psi,
         node_to_clique_[solvabiltyOrder_[nextRootIndice - 1]],
         node_to_clique_[solvabiltyOrder_[nextRootIndice]]);
      deciding_(phi, psi, solvabiltyOrder_[nextRootIndice]);
    }

    // last distribution
    distributingMessage_(phi, psi, node_to_clique_[solvabiltyOrder_[0]]);
    computingPosteriors_(phi, psi);
  }

  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDInference< GUM_SCALAR >::initializingInference_(
     PhiNodeProperty& phi, PsiArcProperty& psi) {
    const auto& jt = *junctionTree();
    const auto& infdiag = this->influenceDiagram();
    // init JT potentials and separators

    for (const auto node: jt.nodes()) {
      phi.insert(node, DecisionPotential< GUM_SCALAR >());
      for (const auto nei: jt.neighbours(node)) {
        psi.insert(Arc(node, nei), DecisionPotential< GUM_SCALAR >());
        if (node < nei) {   // to do it only once by edge
          // we create the set of vars in node and nei  (cached in varsSeparators_)
          for (const auto n: jt.clique(node) * jt.clique(nei))
            varsSeparator_.getWithDefault(Edge(node, nei), SetOfVars())
               .insert(&(infdiag.variable(n)));
        }
      }
    }
    for (const auto node: infdiag.nodes()) {
      const auto clik = node_to_clique_[node];
      if (this->hasEvidence(node)) {
        phi[clik].insertProba(*(this->evidence()[node]));
      }

      if (infdiag.isDecisionNode(node)) {
        if (!this->hasEvidence(node)) {
          auto p =
             (Potential< GUM_SCALAR >() << infdiag.variable(node)).fillWith(1);
          phi[clik].insertProba(p);   // WITHOUT NORMALIZATION !!!
        }
      } else if (infdiag.isChanceNode(node))
        phi[clik].insertProba(infdiag.cpt(node));
      else if (infdiag.isUtilityNode(node))
        phi[clik].insertUtility(infdiag.utility(node));
      else
        GUM_ERROR(FatalError, "Node " << node << " has no type.");
    }
  }

  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDInference< GUM_SCALAR >::creatingJunctionTree__() {
    const auto& infdiag = this->influenceDiagram();
    auto        moral = reduced_.moralGraph();

    // once the moral graph is completed, we remove the utility nodes before
    // triangulation
    NodeProperty< Size > modalities;
    for (const auto node: infdiag.nodes())
      if (infdiag.isUtilityNode(node)) {
        moral.eraseNode(node);
      } else {
        modalities.insert(node, infdiag.variable(node).domainSize());
      }
    DefaultTriangulation triangulation(&moral, &modalities);
    reducedJunctionTree_ = triangulation.junctionTree();
    findingCliqueForEachNode__(triangulation);
  }

  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDInference< GUM_SCALAR >::findingCliqueForEachNode__(
     DefaultTriangulation& triangulation) {
    // indicate, for each node of the moral graph a clique in JT__ that can
    // contain its conditional probability table
    const auto& infdiag = this->influenceDiagram();
    NodeId      first_eliminated_node;
    Idx         elim_number;
    node_to_clique_.clear();
    const std::vector< NodeId >& JT_elim_order = triangulation.eliminationOrder();
    NodeProperty< Idx >          elim_order(Size(JT_elim_order.size()));
    for (Idx i = Idx(0), size = JT_elim_order.size(); i < size; ++i)
      elim_order.insert(JT_elim_order[i], (int)i);
    for (const auto node: reduced_.nodes()) {
      if (infdiag.isUtilityNode(node)) {
        // utility nodes are not in the junction tree but we want to associate a
        // clique as well
        first_eliminated_node = node;
        elim_number = infdiag.size() + 1;   // an impossible elim_number;
      } else {
        // get the variables in the potential of node (and its parents)
        first_eliminated_node = node;
        elim_number = elim_order[first_eliminated_node];
      }

      for (const auto parent: reduced_.parents(node)) {
        if (elim_order[parent] < elim_number) {
          elim_number = elim_order[parent];
          first_eliminated_node = parent;
        }
      }

      // first_eliminated_node contains the first var (node or one of its
      // parents) eliminated => the clique created during its elimination
      // contains node and all of its parents => it can contain the potential
      // assigned to the node in the BN
      node_to_clique_.insert(
         node, triangulation.createdJunctionTreeClique(first_eliminated_node));
    }
  }

  template < typename GUM_SCALAR >
  std::pair< GUM_SCALAR, GUM_SCALAR >
     ShaferShenoyLIMIDInference< GUM_SCALAR >::MEU() {
    if (!this->isInferenceDone())
      GUM_ERROR(OperationNotAllowed, "Call MakeInference first")

    const InfluenceDiagram< GUM_SCALAR >& infdiag = this->influenceDiagram();

    GUM_SCALAR resmean = 0;
    GUM_SCALAR resvar = 0;
    for (auto node: infdiag.nodes()) {
      if (infdiag.isUtilityNode(node)) {
        auto p = meanVar(node);
        resmean += p.first;
        resvar += p.second;
      }
    }
    return std::pair< GUM_SCALAR, GUM_SCALAR >(resmean, resvar);
  }

  template < typename GUM_SCALAR >
  gum::Potential< GUM_SCALAR >
     ShaferShenoyLIMIDInference< GUM_SCALAR >::optimalDecision(NodeId decisionId) {
    if (!this->isInferenceDone())
      GUM_ERROR(OperationNotAllowed, "Call MakeInference first")

    const InfluenceDiagram< GUM_SCALAR >& infdiag = this->influenceDiagram();
    if (!infdiag.isDecisionNode(decisionId))
      GUM_ERROR(InvalidNode,
                infdiag.variable(decisionId).name()
                   << "(" << decisionId << ") is not a decision node.")

    return strategies_[decisionId];
  }


  template < typename GUM_SCALAR >
  bool ShaferShenoyLIMIDInference< GUM_SCALAR >::isSolvable() const {
    return (!solvabiltyOrder_.empty());
  }

  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDInference< GUM_SCALAR >::createReduced_() {
    // from LIMIDS of decision Problems, Lauritzen et Nilsson, 1999
    reduced_.clear();
    reducedJunctionTree_.clear();
    solvabiltyOrder_.clear();
    reversePartialOrder_.clear();
    posteriors_.clear();
    unconditionalDecisions_.clear();
    strategies_.clear();
    const InfluenceDiagram< GUM_SCALAR >& infdiag = this->influenceDiagram();

    NodeSet utilities;

    // build reduced_
    for (auto node: infdiag.nodes()) {
      reduced_.addNodeWithId(node);
      if (infdiag.isUtilityNode(node)) { utilities.insert(node); }
    }

    for (const auto& arc: infdiag.arcs())
      reduced_.addArc(arc.tail(), arc.head());

    completingNoForgettingAssumption__();
    creatingPartialOrder__(utilities);
    checkingSolvability__(utilities);
    if (isSolvable()) {
      reducingLIMID__();
      creatingJunctionTree__();
    }

    this->setState_(
       GraphicalModelInference< GUM_SCALAR >::StateOfInference::OutdatedStructure);
  }

  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDInference<
     GUM_SCALAR >::completingNoForgettingAssumption__() {
    // force no forgetting if necessary
    if (hasNoForgettingAssumption()) {
      auto last = *(noForgettingOrder_.begin());
      for (auto node: noForgettingOrder_)
        if (node == last)   // first one
          continue;
        else {   // we deal with last->node
                 // adding the whole family of last as parents of node
          if (!reduced_.existsArc(last, node)) { reduced_.addArc(last, node); }
          for (auto par: reduced_.parents(last)) {
            if (!reduced_.existsArc(par, node)) reduced_.addArc(par, node);
          }
          last = node;
        }
    }
  }

  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDInference< GUM_SCALAR >::checkingSolvability__(
     const NodeSet& utilities) {
    if (hasNoForgettingAssumption()) {
      solvabiltyOrder_ = noForgettingOrder_;
      std::reverse(solvabiltyOrder_.begin(), solvabiltyOrder_.end());
      return;
    }

    solvabiltyOrder_.clear();
    for (const auto& sen: reversePartialOrder()) {
      NodeSet tobetested = sen;
      while (!tobetested.empty()) {
        bool foundOne = false;
        for (const auto& node: tobetested) {
          const auto us = utilities * reduced_.descendants(node);
          NodeSet    decs;
          for (const auto dec: tobetested)
            if (dec != node) decs += reduced_.family(dec);
          if (reduced_.isIndependent(decs, us, reduced_.family(node))) {
            solvabiltyOrder_.push_back(node);
            foundOne = true;
            tobetested.erase(node);
            break;
          }
        }
        if (!foundOne) {   // no solvability
          solvabiltyOrder_.clear();
          return;
        }
      }
    }
  }

  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDInference< GUM_SCALAR >::reducingLIMID__() {
    for (const auto& sen: reversePartialOrder_) {
      for (auto n: sen) {
        for (auto p: nonRequisiteNodes_(n))
          reduced_.eraseArc(Arc(p, n));
      }
    }
  }

  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDInference< GUM_SCALAR >::creatingPartialOrder__(
     const NodeSet& utilities) {
    const InfluenceDiagram< GUM_SCALAR >& infdiag = this->influenceDiagram();
    NodeProperty< Size >                  level;

    for (const auto& node: utilities)
      level.insert(node, 0);   // utility node is level 0

    // creating the partial order
    Size max_level = 0;
    reversePartialOrder_.clear();
    reversePartialOrder_.resize(infdiag.size());
    NodeSet currents;
    for (auto node: infdiag.nodes()) {
      if (infdiag.isUtilityNode(node)) continue;
      if (reduced_.children(node).isSubsetOrEqual(utilities)) {
        currents.clear();
        currents.insert(node);
        level.insert(node, 0);
        while (!currents.empty()) {
          NodeId elt = *(currents.begin());
          currents.erase(elt);

          if (infdiag.isDecisionNode(elt))
            reversePartialOrder_[level[elt]].insert(elt);

          for (auto parent: reduced_.parents(elt)) {
            Size lev = 0;
            Size newl;
            bool ok_to_add = true;
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
                  GUM_ERROR(FatalError,
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
  }

  template < typename GUM_SCALAR >
  std::vector< NodeSet >
     ShaferShenoyLIMIDInference< GUM_SCALAR >::reversePartialOrder() const {
    return reversePartialOrder_;
  }
  template < typename GUM_SCALAR >
  bool
     ShaferShenoyLIMIDInference< GUM_SCALAR >::hasNoForgettingAssumption() const {
    return !noForgettingOrder_.empty();
  }

  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDInference< GUM_SCALAR >::addNoForgettingAssumption(
     const std::vector< std::string >& names) {
    addNoForgettingAssumption(this->influenceDiagram().ids(names));
  }

  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDInference< GUM_SCALAR >::addNoForgettingAssumption(
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
     ShaferShenoyLIMIDInference< GUM_SCALAR >::nonRequisiteNodes_(NodeId d) const {
    const InfluenceDiagram< GUM_SCALAR >& infdiag = this->influenceDiagram();

    if (!infdiag.isDecisionNode(d))
      GUM_ERROR(TypeError, d << " is not a decision node");

    NodeSet res;
    if (reduced_.parents(d).empty()) return res;

    NodeSet descUs;
    for (const auto n: reduced_.descendants(d))
      if (infdiag.isUtilityNode(n)) descUs.insert(n);

    NodeSet cumul{descUs};
    cumul << d;
    auto g = reduced_.moralizedAncestralGraph(cumul);

    NodeSet family{reduced_.parents(d)};
    family << d;
    bool notReq;
    for (const auto p: reduced_.parents(d)) {
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
  InfluenceDiagram< GUM_SCALAR >
     ShaferShenoyLIMIDInference< GUM_SCALAR >::reducedLIMID() const {
    const auto&                    infdiag = this->influenceDiagram();
    InfluenceDiagram< GUM_SCALAR > res;
    for (auto node: infdiag.nodes()) {
      if (infdiag.isChanceNode(node))
        res.addChanceNode(infdiag.variable(node), node);
      else if (infdiag.isDecisionNode(node))
        res.addDecisionNode(infdiag.variable(node), node);
      else if (infdiag.isUtilityNode(node))
        res.addUtilityNode(infdiag.variable(node), node);
      else
        GUM_ERROR(FatalError, "Type of node " << node << "is unknown.")
    }

    for (const auto& arc: reduced_.arcs()) {
      res.addArc(arc.tail(), arc.head());
    }

    // Potentials !!!
    return res;
  }


  template < typename GUM_SCALAR >
  const JunctionTree*
     ShaferShenoyLIMIDInference< GUM_SCALAR >::junctionTree() const {
    if (!isSolvable()) {
      GUM_ERROR(FatalError, "This LIMID/Influence Diagram is not solvable.")
    }
    return &reducedJunctionTree_;
  }

  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDInference< GUM_SCALAR >::collectingMessage_(
     PhiNodeProperty& phi, PsiArcProperty& psi, const NodeId rootClique) {
    const auto& jt = *junctionTree();

    std::function< void(NodeId, NodeId) > parcours = [&](NodeId node,
                                                         NodeId from) {
      for (const auto nei: jt.neighbours(node)) {
        if (nei != from) parcours(nei, node);
      }
      transmittingMessage_(phi, psi, node, from);
    };

    for (const auto nei: jt.neighbours(rootClique)) {
      parcours(nei, rootClique);
    }
  }
  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDInference< GUM_SCALAR >::collectingToFollowingRoot_(
     PhiNodeProperty& phi,
     PsiArcProperty&  psi,
     NodeId           fromClique,
     NodeId           toClique) {
    const auto& jt = *junctionTree();

    std::function< bool(NodeId, NodeId, NodeId) > revparcours =
       [&](NodeId node, NodeId from, NodeId target) {
         if (node == target) return true;

         bool   found = false;
         NodeId prec;
         for (const auto nei: jt.neighbours(node)) {
           if (nei != from)
             if (revparcours(nei, node, target)) {
               prec = nei;
               found = true;
               break;
             }
         }
         if (found) { transmittingMessage_(phi, psi, prec, node); }
         return found;
       };

    revparcours(toClique, jt.size() + 1, fromClique);
  }

  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDInference< GUM_SCALAR >::deciding_(PhiNodeProperty& phi,
                                                           PsiArcProperty&  psi,
                                                           NodeId decisionNode) {
    const auto& infdiag = this->influenceDiagram();
    const auto& jt = *junctionTree();

    /*GUM_TRACE("=> integration in "
              << node_to_clique_[decisionNode] << "("
              << infdiag.names(jt.clique(node_to_clique_[decisionNode]))
              << ") for " << infdiag.variable(decisionNode).name())*/
    auto& decision =
       strategies_.getWithDefault(decisionNode, Potential< GUM_SCALAR >());

    if (this->hasHardEvidence(decisionNode)) {
      decision = *(this->evidence()[decisionNode]);
    } else {
      auto dp = integrating_(phi, psi, node_to_clique_[decisionNode]);

      SetOfVars sev;
      sev.insert(&infdiag.variable(decisionNode));
      for (const auto parent: reduced_.parents(decisionNode)) {
        // GUM_TRACE("    parent : " << infdiag.variable(parent).name())
        sev.insert(&infdiag.variable(parent));
      }
      dp = dp ^ sev;

      if (sev.size() == 1) {   // unconditionnal => we keep the utility before
                               // inserting hard decision
        unconditionalDecisions_.set(decisionNode, dp);
      }
      decision =
         (dp.utilPot * dp.probPot).putFirst(&infdiag.variable(decisionNode));

      binarizingMax_(decision);
    }
    phi[node_to_clique_[decisionNode]].insertProba(decision);
  }

  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDInference< GUM_SCALAR >::binarizingMax_(
     Potential< GUM_SCALAR >& decision)
     const {   // compute the decisions (as maxEU)
    Instantiation I(decision);
    const auto&   firstvar = decision.variable(0);
    for (I.setFirst(); !I.end(); I.incNotVar(firstvar)) {
      I.setFirstVar(firstvar);
      Idx        argm = I.val(firstvar);
      GUM_SCALAR umax = decision[I];
      for (I.incVar(firstvar); !I.end(); I.incVar(firstvar)) {
        if (umax < decision[I]) {
          umax = decision[I];
          argm = I.val(firstvar);
        }
      }
      for (I.setFirstVar(firstvar); !I.end(); I.incVar(firstvar))
        decision.set(I, 0);
      I.chgVal(firstvar, argm);
      decision.set(I, 1);
    }
  }

  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDInference< GUM_SCALAR >::distributingMessage_(
     PhiNodeProperty& phi, PsiArcProperty& psi, NodeId rootClique) {
    const auto& jt = *junctionTree();

    std::function< void(NodeId, NodeId) > parcours = [&](NodeId node,
                                                         NodeId from) {
      // todo : not twice if already integrated in deciding_ !
      phi.set(from, integrating_(phi, psi, from));
      transmittingMessage_(phi, psi, from, node);
      for (const auto nei: jt.neighbours(node)) {
        if (nei != from) parcours(nei, node);
      }
    };
    for (const auto nei: jt.neighbours(rootClique)) {
      parcours(nei, rootClique);
    }
  }

  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDInference< GUM_SCALAR >::transmittingMessage_(
     PhiNodeProperty& phi,
     PsiArcProperty&  psi,
     NodeId           fromClique,
     NodeId           toClique) {
    const auto& jt = *junctionTree();
    psi.set(Arc(fromClique, toClique),
            integrating_(phi, psi, fromClique, toClique)
               ^ varsSeparator_[Edge(fromClique, toClique)]);
    /*
    const auto& infdiag = this->influenceDiagram();
    GUM_TRACE("Computing message : " << infdiag.names(jt.clique(fromClique))
                                     << "->" << infdiag.names(jt.clique(toClique)))
    GUM_TRACE_VAR(psi[Arc(fromClique, toClique)])*/
  }

  template < typename GUM_SCALAR >
  DecisionPotential< double >
     ShaferShenoyLIMIDInference< GUM_SCALAR >::integrating_(
        const PhiNodeProperty& phi,
        const PsiArcProperty&  psi,
        NodeId                 clique,
        NodeId                 except) const {
    const auto&                 jt = *junctionTree();
    DecisionPotential< double > res = phi[clique];
    for (const auto nei: jt.neighbours(clique))
      if (nei != except) res *= psi[Arc(nei, clique)];

    return res;
  }

  template < typename GUM_SCALAR >
  DecisionPotential< double >
     ShaferShenoyLIMIDInference< GUM_SCALAR >::integrating_(
        const PhiNodeProperty& phi,
        const PsiArcProperty&  psi,
        NodeId                 clique) const {
    return integrating_(phi, psi, clique, junctionTree()->size() + 1);
  }

  template < typename GUM_SCALAR >
  void ShaferShenoyLIMIDInference< GUM_SCALAR >::computingPosteriors_(
     const PhiNodeProperty& phi, const PsiArcProperty& psi) {
    const auto& infdiag = this->influenceDiagram();
    posteriors_.clear();
    strategies_.clear();
    for (const auto node: infdiag.nodes()) {
      auto& res =
         posteriors_.getWithDefault(node, DecisionPotential< GUM_SCALAR >());
      if (infdiag.isChanceNode(node)) {
        SetOfVars sev;
        sev.insert(&infdiag.variable(node));
        res = phi[node_to_clique_[node]] ^ sev;
        res.probPot.normalize();
      } else if (infdiag.isDecisionNode(node)) {
        SetOfVars sev;
        sev.insert(&infdiag.variable(node));
        SetOfVars family = sev;
        for (const auto par: reduced_.parents(node)) {
          if (infdiag.isChanceNode(par)) family.insert(&infdiag.variable(par));
        }
        const auto               dp = phi[node_to_clique_[node]] ^ family;
        gum::Potential< double > decision = dp.utilPot.putFirst(&infdiag.variable(node));
        binarizingMax_(decision);
        strategies_.insert(node, decision);
        res = dp ^ sev;
        res.probPot.normalize();
        if (reduced_.parents(node).size() == 0) {   // unconditional decision
          if (!this->hasHardEvidence(node)) {
            if (!unconditionalDecisions_.exists(node))
              GUM_ERROR(InvalidNode,
                        "Node " << infdiag.variable(node).name()
                                << " should have an unconditionaldecision")
            res.utilPot = unconditionalDecisions_[node].utilPot;
          }
        }
      } else {
        SetOfVars family;
        family.insert(&infdiag.variable(node));
        for (const auto par: reduced_.parents(node)) {
          family.insert(&infdiag.variable(par));
        }
        res = phi[node_to_clique_[node]] ^ family;
        res.utilPot = infdiag.utility(node);
      }
    }
  }

  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
     ShaferShenoyLIMIDInference< GUM_SCALAR >::posterior(NodeId node) {
    return posteriors_[node].probPot;
  }

  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
     ShaferShenoyLIMIDInference< GUM_SCALAR >::posteriorUtility(NodeId node) {
    return posteriors_[node].utilPot;
  }


  template < typename GUM_SCALAR >
  std::pair< GUM_SCALAR, GUM_SCALAR >
     ShaferShenoyLIMIDInference< GUM_SCALAR >::meanVar(NodeId node) {
    return posteriors_[node].meanVar();
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
