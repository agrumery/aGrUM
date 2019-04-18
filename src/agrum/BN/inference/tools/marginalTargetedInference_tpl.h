
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief Implementation of the generic class for the computation of
 * (possibly incrementally) marginal posteriors
 */
#include <iterator>

namespace gum {


  // Default Constructor
  template < typename GUM_SCALAR >
  MarginalTargetedInference< GUM_SCALAR >::MarginalTargetedInference(
     const IBayesNet< GUM_SCALAR >* bn) :
      BayesNetInference< GUM_SCALAR >(bn) {
    // assign a BN if this has not been done before (due to virtual inheritance)
    if (this->__bn == nullptr) {
      BayesNetInference< GUM_SCALAR >::__setBayesNetDuringConstruction(bn);
    }

    // sets all the nodes as targets
    if (bn != nullptr) {
      __targeted_mode = false;
      __targets = bn->dag().asNodeSet();
    }

    GUM_CONSTRUCTOR(MarginalTargetedInference);
  }


  // Destructor
  template < typename GUM_SCALAR >
  MarginalTargetedInference< GUM_SCALAR >::~MarginalTargetedInference() {
    GUM_DESTRUCTOR(MarginalTargetedInference);
  }


  // fired when a new BN is assigned to the inference engine
  template < typename GUM_SCALAR >
  void MarginalTargetedInference< GUM_SCALAR >::_onBayesNetChanged(
     const IBayesNet< GUM_SCALAR >* bn) {
    __targeted_mode = true;
    __setAllMarginalTargets();
  }


  // ##############################################################################
  // Targets
  // ##############################################################################

  // return true if variable is a target
  template < typename GUM_SCALAR >
  INLINE bool
     MarginalTargetedInference< GUM_SCALAR >::isTarget(NodeId node) const {
    // check that the variable belongs to the bn
    if (this->__bn == nullptr)
      GUM_ERROR(NullElement,
                "No Bayes net has been assigned to the "
                "inference algorithm");
    if (!this->__bn->dag().exists(node)) {
      GUM_ERROR(UndefinedElement, node << " is not a NodeId in the bn");
    }

    return __targets.contains(node);
  }

  // Add a single target to the list of targets
  template < typename GUM_SCALAR >
  INLINE bool MarginalTargetedInference< GUM_SCALAR >::isTarget(
     const std::string& nodeName) const {
    return isTarget(this->__bn->idFromName(nodeName));
  }


  // Clear all previously defined targets (single targets and sets of targets)
  template < typename GUM_SCALAR >
  INLINE void MarginalTargetedInference< GUM_SCALAR >::eraseAllTargets() {
    _onAllMarginalTargetsErased();

    __targets.clear();
    _setTargetedMode();   // does nothing if already in targeted mode

    this->__setState(
       BayesNetInference< GUM_SCALAR >::StateOfInference::OutdatedBNStructure);
  }


  // Add a single target to the list of targets
  template < typename GUM_SCALAR >
  void MarginalTargetedInference< GUM_SCALAR >::addTarget(NodeId target) {
    // check if the node belongs to the Bayesian network
    if (this->__bn == nullptr)
      GUM_ERROR(NullElement,
                "No Bayes net has been assigned to the "
                "inference algorithm");

    if (!this->__bn->dag().exists(target)) {
      GUM_ERROR(UndefinedElement, target << " is not a NodeId in the bn");
    }

    _setTargetedMode();   // does nothing if already in targeted mode
    // add the new target
    if (!__targets.contains(target)) {
      __targets.insert(target);
      _onMarginalTargetAdded(target);
      this->__setState(
         BayesNetInference< GUM_SCALAR >::StateOfInference::OutdatedBNStructure);
    }
  }


  // Add all nodes as targets
  template < typename GUM_SCALAR >
  void MarginalTargetedInference< GUM_SCALAR >::addAllTargets() {
    // check if the node belongs to the Bayesian network
    if (this->__bn == nullptr)
      GUM_ERROR(NullElement,
                "No Bayes net has been assigned to the "
                "inference algorithm");


    _setTargetedMode();   // does nothing if already in targeted mode
    for (const auto target : this->__bn->dag()) {
      if (!__targets.contains(target)) {
        __targets.insert(target);
        _onMarginalTargetAdded(target);
        this->__setState(
           BayesNetInference< GUM_SCALAR >::StateOfInference::OutdatedBNStructure);
      }
    }
  }


  // Add a single target to the list of targets
  template < typename GUM_SCALAR >
  void MarginalTargetedInference< GUM_SCALAR >::addTarget(
     const std::string& nodeName) {
    // check if the node belongs to the Bayesian network
    if (this->__bn == nullptr)
      GUM_ERROR(NullElement,
                "No Bayes net has been assigned to the "
                "inference algorithm");

    addTarget(this->__bn->idFromName(nodeName));
  }


  // removes an existing target
  template < typename GUM_SCALAR >
  void MarginalTargetedInference< GUM_SCALAR >::eraseTarget(NodeId target) {
    // check if the node belongs to the Bayesian network
    if (this->__bn == nullptr)
      GUM_ERROR(NullElement,
                "No Bayes net has been assigned to the "
                "inference algorithm");

    if (!this->__bn->dag().exists(target)) {
      GUM_ERROR(UndefinedElement, target << " is not a NodeId in the bn");
    }


    if (__targets.contains(target)) {
      __targeted_mode = true;   // we do not use _setTargetedMode because we do not
                                // want to clear the targets
      _onMarginalTargetErased(target);
      __targets.erase(target);
      this->__setState(
         BayesNetInference< GUM_SCALAR >::StateOfInference::OutdatedBNStructure);
    }
  }


  // Add a single target to the list of targets
  template < typename GUM_SCALAR >
  void MarginalTargetedInference< GUM_SCALAR >::eraseTarget(
     const std::string& nodeName) {
    // check if the node belongs to the Bayesian network
    if (this->__bn == nullptr)
      GUM_ERROR(NullElement,
                "No Bayes net has been assigned to the "
                "inference algorithm");

    eraseTarget(this->__bn->idFromName(nodeName));
  }


  // returns the list of single targets
  template < typename GUM_SCALAR >
  INLINE const NodeSet& MarginalTargetedInference< GUM_SCALAR >::targets() const
     noexcept {
    return __targets;
  }

  // returns the list of single targets
  template < typename GUM_SCALAR >
  INLINE const Size MarginalTargetedInference< GUM_SCALAR >::nbrTargets() const
     noexcept {
    return __targets.size();
  }


  /// sets all the nodes of the Bayes net as targets
  template < typename GUM_SCALAR >
  void MarginalTargetedInference< GUM_SCALAR >::__setAllMarginalTargets() {
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
  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
     MarginalTargetedInference< GUM_SCALAR >::posterior(NodeId node) {
    if (this->hardEvidenceNodes().contains(node)) {
      return *(this->evidence()[node]);
    }

    if (!isTarget(node)) {
      // throws UndefinedElement if var is not a target
      GUM_ERROR(UndefinedElement, node << " is not a target node");
    }

    if (!this->isDone()) { this->makeInference(); }

    return _posterior(node);
  }

  // Compute the posterior of a node.
  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
     MarginalTargetedInference< GUM_SCALAR >::posterior(
        const std::string& nodeName) {
    return posterior(this->BN().idFromName(nodeName));
  }

  /* Entropy
   * Compute Shanon's entropy of a node given the observation
   */
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR MarginalTargetedInference< GUM_SCALAR >::H(NodeId X) {
    return posterior(X).entropy();
  }

  /* Entropy
   * Compute Shanon's entropy of a node given the observation
   */
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR
         MarginalTargetedInference< GUM_SCALAR >::H(const std::string& nodeName) {
    return H(this->BN().idFromName(nodeName));
  }


  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >
     MarginalTargetedInference< GUM_SCALAR >::evidenceImpact(NodeId         target,
                                                             const NodeSet& evs) {
    const auto& vtarget = this->BN().variable(target);

    if (evs.contains(target)) {
      GUM_ERROR(InvalidArgument,
                "Target <" << vtarget.name() << "> (" << target
                           << ") can not be in evs (" << evs << ").");
    }
    auto condset = this->BN().minimalCondSet(target, evs);

    Potential< GUM_SCALAR > res;
    this->eraseAllTargets();
    this->eraseAllEvidence();
    res.add(this->BN().variable(target));
    this->addTarget(target);
    for (const auto& n : condset) {
      res.add(this->BN().variable(n));
      this->addEvidence(n, 0);
    }

    Instantiation inst(res);
    for (inst.setFirst(); !inst.end(); inst.incNotVar(vtarget)) {
      // inferring
      for (const auto& n : condset)
        this->chgEvidence(n, inst.val(this->BN().variable(n)));
      this->makeInference();
      // populate res
      for (inst.setFirstVar(vtarget); !inst.end(); inst.incVar(vtarget)) {
        res.set(inst, this->posterior(target)[inst]);
      }
      inst.setFirstVar(vtarget);   // remove inst.end() flag
    }

    return res;
  }


  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR > MarginalTargetedInference< GUM_SCALAR >::evidenceImpact(
     const std::string& target, const std::vector< std::string >& evs) {
    const auto& bn = this->BN();

    gum::NodeSet evsId;
    for (const auto& evname : evs) {
      evsId.insert(bn.idFromName(evname));
    }

    return evidenceImpact(bn.idFromName(target), evsId);
  }


  template < typename GUM_SCALAR >
  INLINE bool MarginalTargetedInference< GUM_SCALAR >::_isTargetedMode() const {
    return __targeted_mode;
  }
  template < typename GUM_SCALAR >
  INLINE void MarginalTargetedInference< GUM_SCALAR >::_setTargetedMode() {
    if (!__targeted_mode) {
      __targets.clear();
      __targeted_mode = true;
    }
  }
} /* namespace gum */
