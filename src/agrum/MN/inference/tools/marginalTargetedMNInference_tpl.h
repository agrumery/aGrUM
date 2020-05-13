
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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
 * @brief Implementation of the generic class for the computation of
 * (possibly incrementally) marginal posteriors
 */
#include <iterator>

namespace gum {


  // Default Constructor
  template < typename GUM_SCALAR >
  MarginalTargetedMNInference< GUM_SCALAR >::MarginalTargetedMNInference(
     const IMarkovNet< GUM_SCALAR >* mn) :
      MarkovNetInference< GUM_SCALAR >(mn) {
    // assign a MN if this has not been done before (due to virtual inheritance)
    if (this->__mn == nullptr) {
      MarkovNetInference< GUM_SCALAR >::__setMarkovNetDuringConstruction(mn);
    }

    // sets all the nodes as targets
    if (mn != nullptr) {
      __targeted_mode = false;
      __targets = mn->graph().asNodeSet();
    }

    GUM_CONSTRUCTOR(MarginalTargetedMNInference);
  }


  // Destructor
  template < typename GUM_SCALAR >
  MarginalTargetedMNInference< GUM_SCALAR >::~MarginalTargetedMNInference() {
    GUM_DESTRUCTOR(MarginalTargetedMNInference);
  }


  // fired when a new MN is assigned to the inference engine
  template < typename GUM_SCALAR >
  void MarginalTargetedMNInference< GUM_SCALAR >::_onMarkovNetChanged(
     const IMarkovNet< GUM_SCALAR >* mn) {
    __targeted_mode = true;
    __setAllMarginalTargets();
  }


  // ##############################################################################
  // Targets
  // ##############################################################################

  // return true if variable is a target
  template < typename GUM_SCALAR >
  INLINE bool
     MarginalTargetedMNInference< GUM_SCALAR >::isTarget(NodeId node) const {
    // check that the variable belongs to the mn
    if (this->__mn == nullptr)
      GUM_ERROR(NullElement,
                "No Markov net has been assigned to the "
                "inference algorithm");
    if (!this->__mn->graph().exists(node)) {
      GUM_ERROR(UndefinedElement, node << " is not a NodeId in the Markov network");
    }

    return __targets.contains(node);
  }

  // Add a single target to the list of targets
  template < typename GUM_SCALAR >
  INLINE bool MarginalTargetedMNInference< GUM_SCALAR >::isTarget(
     const std::string& nodeName) const {
    return isTarget(this->__mn->idFromName(nodeName));
  }


  // Clear all previously defined targets (single targets and sets of targets)
  template < typename GUM_SCALAR >
  INLINE void MarginalTargetedMNInference< GUM_SCALAR >::eraseAllTargets() {
    _onAllMarginalTargetsErased();

    __targets.clear();
    _setTargetedMode();   // does nothing if already in targeted mode

    this->__setState(
       MarkovNetInference< GUM_SCALAR >::StateOfMNInference::OutdatedMNStructure);
  }


  // Add a single target to the list of targets
  template < typename GUM_SCALAR >
  void MarginalTargetedMNInference< GUM_SCALAR >::addTarget(NodeId target) {
    // check if the node belongs to the Markov network
    if (this->__mn == nullptr)
      GUM_ERROR(NullElement,
                "No Markov net has been assigned to the "
                "inference algorithm");

    if (!this->__mn->graph().exists(target)) {
      GUM_ERROR(UndefinedElement, target << " is not a NodeId in the Markov network");
    }

    _setTargetedMode();   // does nothing if already in targeted mode
    // add the new target
    if (!__targets.contains(target)) {
      __targets.insert(target);
      _onMarginalTargetAdded(target);
      this->__setState(
         MarkovNetInference< GUM_SCALAR >::StateOfMNInference::OutdatedMNStructure);
    }
  }


  // Add all nodes as targets
  template < typename GUM_SCALAR >
  void MarginalTargetedMNInference< GUM_SCALAR >::addAllTargets() {
    // check if the node belongs to the Markov network
    if (this->__mn == nullptr)
      GUM_ERROR(NullElement,
                "No Markov net has been assigned to the "
                "inference algorithm");


    _setTargetedMode();   // does nothing if already in targeted mode
    for (const auto target: this->__mn->graph()) {
      if (!__targets.contains(target)) {
        __targets.insert(target);
        _onMarginalTargetAdded(target);
        this->__setState(
           MarkovNetInference< GUM_SCALAR >::StateOfMNInference::OutdatedMNStructure);
      }
    }
  }


  // Add a single target to the list of targets
  template < typename GUM_SCALAR >
  void MarginalTargetedMNInference< GUM_SCALAR >::addTarget(
     const std::string& nodeName) {
    // check if the node belongs to the Markov network
    if (this->__mn == nullptr)
      GUM_ERROR(NullElement,
                "No Markov net has been assigned to the "
                "inference algorithm");

    addTarget(this->__mn->idFromName(nodeName));
  }


  // removes an existing target
  template < typename GUM_SCALAR >
  void MarginalTargetedMNInference< GUM_SCALAR >::eraseTarget(NodeId target) {
    // check if the node belongs to the Markov network
    if (this->__mn == nullptr)
      GUM_ERROR(NullElement,
                "No Markov net has been assigned to the "
                "inference algorithm");

    if (!this->__mn->graph().exists(target)) {
      GUM_ERROR(UndefinedElement, target << " is not a NodeId in the Markov network");
    }


    if (__targets.contains(target)) {
      __targeted_mode = true;   // we do not use _setTargetedMode because we do not
                                // want to clear the targets
      _onMarginalTargetErased(target);
      __targets.erase(target);
      this->__setState(
         MarkovNetInference< GUM_SCALAR >::StateOfMNInference::OutdatedMNStructure);
    }
  }


  // Add a single target to the list of targets
  template < typename GUM_SCALAR >
  void MarginalTargetedMNInference< GUM_SCALAR >::eraseTarget(
     const std::string& nodeName) {
    // check if the node belongs to the Markov network
    if (this->__mn == nullptr)
      GUM_ERROR(NullElement,
                "No Markov net has been assigned to the "
                "inference algorithm");

    eraseTarget(this->__mn->idFromName(nodeName));
  }


  // returns the list of single targets
  template < typename GUM_SCALAR >
  INLINE const NodeSet& MarginalTargetedMNInference< GUM_SCALAR >::targets() const
     noexcept {
    return __targets;
  }

  // returns the list of single targets
  template < typename GUM_SCALAR >
  INLINE const Size MarginalTargetedMNInference< GUM_SCALAR >::nbrTargets() const
     noexcept {
    return __targets.size();
  }


  /// sets all the nodes of the Markov net as targets
  template < typename GUM_SCALAR >
  void MarginalTargetedMNInference< GUM_SCALAR >::__setAllMarginalTargets() {
    __targets.clear();
    if (this->__mn != nullptr) {
      __targets = this->__mn->graph().asNodeSet();
      _onAllMarginalTargetsAdded();
    }
  }


  // ##############################################################################
  // Inference
  // ##############################################################################

  // Compute the posterior of a node.
  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
     MarginalTargetedMNInference< GUM_SCALAR >::posterior(NodeId node) {
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
     MarginalTargetedMNInference< GUM_SCALAR >::posterior(
        const std::string& nodeName) {
    return posterior(this->MN().idFromName(nodeName));
  }

  /* Entropy
   * Compute Shanon's entropy of a node given the observation
   */
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR MarginalTargetedMNInference< GUM_SCALAR >::H(NodeId X) {
    return posterior(X).entropy();
  }

  /* Entropy
   * Compute Shanon's entropy of a node given the observation
   */
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR
     MarginalTargetedMNInference< GUM_SCALAR >::H(const std::string& nodeName) {
    return H(this->MN().idFromName(nodeName));
  }


  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >
     MarginalTargetedMNInference< GUM_SCALAR >::evidenceImpact(NodeId         target,
                                                             const NodeSet& evs) {
    const auto& vtarget = this->MN().variable(target);

    if (evs.contains(target)) {
      GUM_ERROR(InvalidArgument,
                "Target <" << vtarget.name() << "> (" << target
                           << ") can not be in evs (" << evs << ").");
    }
    auto condset = this->MN().minimalCondSet(target, evs);

    Potential< GUM_SCALAR > res;
    this->eraseAllTargets();
    this->eraseAllEvidence();
    res.add(this->MN().variable(target));
    this->addTarget(target);
    for (const auto& n: condset) {
      res.add(this->MN().variable(n));
      this->addEvidence(n, 0);
    }

    Instantiation inst(res);
    for (inst.setFirst(); !inst.end(); inst.incNotVar(vtarget)) {
      // inferring
      for (const auto& n: condset)
        this->chgEvidence(n, inst.val(this->MN().variable(n)));
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
  Potential< GUM_SCALAR > MarginalTargetedMNInference< GUM_SCALAR >::evidenceImpact(
     const std::string& target, const std::vector< std::string >& evs) {
    const auto& mn = this->MN();

    gum::NodeSet evsId;
    for (const auto& evname: evs) {
      evsId.insert(mn.idFromName(evname));
    }

    return evidenceImpact(mn.idFromName(target), evsId);
  }


  template < typename GUM_SCALAR >
  INLINE bool MarginalTargetedMNInference< GUM_SCALAR >::_isTargetedMode() const {
    return __targeted_mode;
  }
  template < typename GUM_SCALAR >
  INLINE void MarginalTargetedMNInference< GUM_SCALAR >::_setTargetedMode() {
    if (!__targeted_mode) {
      __targets.clear();
      __targeted_mode = true;
    }
  }
} /* namespace gum */
