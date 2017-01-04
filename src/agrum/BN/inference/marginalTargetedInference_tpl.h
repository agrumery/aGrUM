/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 * @file
 * @brief Implementation of the generic class for the computation of
 * (possibly incrementally) marginal posteriors
 */
#include <iterator>

namespace gum {


  // Default Constructor
  template<typename GUM_SCALAR>
  MarginalTargetedInference<GUM_SCALAR>::MarginalTargetedInference(
          const IBayesNet <GUM_SCALAR> *bn)
          : BayesNetInference<GUM_SCALAR>(bn) {
    // assign a BN if this has not been done before (due to virtual inheritance)
    if (this->__bn == nullptr) {
      BayesNetInference<GUM_SCALAR>::__setBayesNetDuringConstruction(bn);
    }

    // sets all the nodes as targets
    if (bn != nullptr) {
      __targets = bn->dag().asNodeSet();
    }

    GUM_CONSTRUCTOR(MarginalTargetedInference);
  }


  // Destructor
  template<typename GUM_SCALAR>
  MarginalTargetedInference<GUM_SCALAR>::~MarginalTargetedInference() {
    GUM_DESTRUCTOR(MarginalTargetedInference);
  }


  // fired when a new BN is assigned to the inference engine
  template<typename GUM_SCALAR>
  void MarginalTargetedInference<GUM_SCALAR>::_onBayesNetChanged(
          const IBayesNet <GUM_SCALAR> *bn) {
    __setAllMarginalTargets();
  }


  // ##############################################################################
  // Targets
  // ##############################################################################

  // return true if variable is a target
  template<typename GUM_SCALAR>
  INLINE bool
  MarginalTargetedInference<GUM_SCALAR>::isTarget(const NodeId var) const {
    // check that the variable belongs to the bn
    if (this->__bn == nullptr) GUM_ERROR(NullElement,
                                         "No Bayes net has been assigned to the "
                                                 "inference algorithm");
    if (!this->__bn->dag().exists(var)) {
      GUM_ERROR(UndefinedElement, var << " is not a NodeId in the bn");
    }

    return __targets.contains(var);
  }

  // Add a single target to the list of targets
  template<typename GUM_SCALAR>
  INLINE bool MarginalTargetedInference<GUM_SCALAR>::isTarget(
          const std::string &nodeName) const {
    return isTarget(this->__bn->idFromName(nodeName));
  }


  // Clear all previously defined targets (single targets and sets of targets)
  template<typename GUM_SCALAR>
  INLINE void MarginalTargetedInference<GUM_SCALAR>::eraseAllTargets() {
    _onAllMarginalTargetsErased();
    __targets.clear();
    this->__state =
            BayesNetInference<GUM_SCALAR>::StateOfInference::OutdatedBNStructure;
  }


  // Add a single target to the list of targets
  template<typename GUM_SCALAR>
  void MarginalTargetedInference<GUM_SCALAR>::addTarget(NodeId target) {
    // check if the node belongs to the Bayesian network
    if (this->__bn == nullptr) GUM_ERROR(NullElement,
                                         "No Bayes net has been assigned to the "
                                                 "inference algorithm");

    if (!this->__bn->dag().exists(target)) {
      GUM_ERROR(UndefinedElement, target << " is not a NodeId in the bn");
    }

    // add the new target
    if (!__targets.contains(target)) {
      __targets.insert(target);
      _onMarginalTargetAdded(target);
      this->__state =
              BayesNetInference<GUM_SCALAR>::StateOfInference::OutdatedBNStructure;
    }
  }


  // Add all nodes as targets
  template<typename GUM_SCALAR>
  void MarginalTargetedInference<GUM_SCALAR>::addAllTargets() {
    // check if the node belongs to the Bayesian network
    if (this->__bn == nullptr) GUM_ERROR(NullElement,
                                         "No Bayes net has been assigned to the "
                                                 "inference algorithm");

    for (auto target : this->__bn->dag()) {
      if (!__targets.contains(target)) {
        __targets.insert(target);
        _onMarginalTargetAdded(target);
        this->__state =
                BayesNetInference<GUM_SCALAR>::StateOfInference::OutdatedBNStructure;
      }
    }
  }


  // Add a single target to the list of targets
  template<typename GUM_SCALAR>
  void
  MarginalTargetedInference<GUM_SCALAR>::addTarget(const std::string &nodeName) {
    // check if the node belongs to the Bayesian network
    if (this->__bn == nullptr) GUM_ERROR(NullElement,
                                         "No Bayes net has been assigned to the "
                                                 "inference algorithm");

    addTarget(this->__bn->idFromName(nodeName));
  }


  // removes an existing target
  template<typename GUM_SCALAR>
  void MarginalTargetedInference<GUM_SCALAR>::eraseTarget(const NodeId target) {
    // check if the node belongs to the Bayesian network
    if (this->__bn == nullptr) GUM_ERROR(NullElement,
                                         "No Bayes net has been assigned to the "
                                                 "inference algorithm");

    if (!this->__bn->dag().exists(target)) {
      GUM_ERROR(UndefinedElement, target << " is not a NodeId in the bn");
    }

    if (__targets.contains(target)) {
      _onMarginalTargetErased(target);
      __targets.erase(target);
      this->__state =
              BayesNetInference<GUM_SCALAR>::StateOfInference::OutdatedBNStructure;
    }
  }


  // Add a single target to the list of targets
  template<typename GUM_SCALAR>
  void MarginalTargetedInference<GUM_SCALAR>::eraseTarget(
          const std::string &nodeName) {
    // check if the node belongs to the Bayesian network
    if (this->__bn == nullptr) GUM_ERROR(NullElement,
                                         "No Bayes net has been assigned to the "
                                                 "inference algorithm");

    eraseTarget(this->__bn->idFromName(nodeName));
  }


  // returns the list of single targets
  template<typename GUM_SCALAR>
  INLINE const NodeSet &MarginalTargetedInference<GUM_SCALAR>::targets() const
  noexcept {
    return __targets;
  }

  // returns the list of single targets
  template<typename GUM_SCALAR>
  INLINE const Size MarginalTargetedInference<GUM_SCALAR>::nbrTargets() const
  noexcept {
    return __targets.size();
  }


  /// sets all the nodes of the Bayes net as targets
  template<typename GUM_SCALAR>
  void MarginalTargetedInference<GUM_SCALAR>::__setAllMarginalTargets() {
    __targets.clear();
    if (this->__bn != nullptr) {
      __targets = this->__bn->dag().asNodeSet();
      _onAllMarginalTargetsAdded();
    }
  }


  // ##############################################################################
  // Inference
  // ##############################################################################

  // Compute the posterior of a node.
  template<typename GUM_SCALAR>
  const Potential <GUM_SCALAR> &
  MarginalTargetedInference<GUM_SCALAR>::posterior(const NodeId var) {
    if (!isTarget(var)) {
      // throws UndefinedElement if var is not a target
      GUM_ERROR(UndefinedElement, var << " is not a target node");
    }

    if (!this->isDone()) {
      this->makeInference();
    }

    return _posterior(var);
  }

  // Compute the posterior of a node.
  template<typename GUM_SCALAR>
  const Potential <GUM_SCALAR> &
  MarginalTargetedInference<GUM_SCALAR>::posterior(const std::string &nodeName) {
    return posterior(this->BN().idFromName(nodeName));
  }

  /* Entropy
   * Compute Shanon's entropy of a node given the observation
   */
  template<typename GUM_SCALAR>
  INLINE GUM_SCALAR MarginalTargetedInference<GUM_SCALAR>::H(const NodeId X) {
    return posterior(X).entropy();
  }

  /* Entropy
   * Compute Shanon's entropy of a node given the observation
   */
  template<typename GUM_SCALAR>
  INLINE GUM_SCALAR
  MarginalTargetedInference<GUM_SCALAR>::H(const std::string &nodeName) {
    return H(this->BN().idFromName(nodeName));
  }


  template<typename GUM_SCALAR>
  Potential <GUM_SCALAR>
  MarginalTargetedInference<GUM_SCALAR>::evidenceImpact(NodeId target,
                                                        const std::vector<NodeId> evs) {
    const auto &vtarget = this->BN().variable(target);

    NodeSet soids(evs.size());
    for (const auto &e : evs)
      soids << e;
    if (soids.contains(target)) {
      GUM_ERROR(InvalidArgument,
                "Target <" << vtarget.name() << "> (" << target
                           << ") can not be in evs ("
                           << evs
                           << ").");
    }
    auto condset = this->BN().minimalCondSet(target, soids);

    Potential<GUM_SCALAR> res;
    this->eraseAllTargets();
    this->eraseAllEvidence();
    res.add(this->BN().variable(target));
    this->addTarget(target);
    for (const auto &n : condset) {
      res.add(this->BN().variable(n));
      this->addEvidence(n, 0);
    }

    Instantiation inst(res);
    for (inst.setFirst(); !inst.end(); inst.incNotVar(vtarget)) {
      // inferring
      for (const auto &n : condset)
        this->chgEvidence(n, inst.val(this->BN().variable(n)));
      this->makeInference();
      // populate res
      for (inst.setFirstVar(vtarget); !inst.end(); inst.incVar(vtarget)) {
        res.set(inst, this->posterior(target)[inst]);
      }
      inst.setFirstVar(vtarget);  // remove inst.end() flag
    }

    return res;
  }


  template<typename GUM_SCALAR>
  Potential <GUM_SCALAR>
  MarginalTargetedInference<GUM_SCALAR>::evidenceImpact(std::string target,
                                                        const std::vector<std::string> evs) {
    const auto &bn = this->BN();

    std::vector<NodeId> evsId;
    evsId.reserve(evs.size());
    std::transform(std::begin(evs),
                   std::end(evs),
                   std::back_inserter(evsId),
                   [&bn](std::string s) { return bn.idFromName(s); });

    return evidenceImpact(bn.idFromName(target), evsId);
  }
} /* namespace gum */
