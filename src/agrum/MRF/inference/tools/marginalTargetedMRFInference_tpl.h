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
 * @brief Implementation of the generic class for the computation of
 * (possibly incrementally) marginal posteriors
 */
#include <iterator>

namespace gum {


  // Default Constructor
  template < typename GUM_SCALAR >
  MarginalTargetedMRFInference< GUM_SCALAR >::MarginalTargetedMRFInference(
     const IMarkovRandomField< GUM_SCALAR >* mn) :
      MRFInference< GUM_SCALAR >(mn) {
    // assign a MRF if this has not been done before (due to virtual inheritance)
    if (this->hasNoModel_()) { MRFInference< GUM_SCALAR >::_setMRFDuringConstruction_(mn); }

    // sets all the nodes as targets
    if (mn != nullptr) {
      _targeted_mode_ = false;
      _targets_       = mn->graph().asNodeSet();
    }

    GUM_CONSTRUCTOR(MarginalTargetedMRFInference);
  }


  // Destructor
  template < typename GUM_SCALAR >
  MarginalTargetedMRFInference< GUM_SCALAR >::~MarginalTargetedMRFInference() {
    GUM_DESTRUCTOR(MarginalTargetedMRFInference);
  }


  // fired when a new MRF is assigned to the inference engine
  template < typename GUM_SCALAR >
  void MarginalTargetedMRFInference< GUM_SCALAR >::onModelChanged_(const GraphicalModel* mn) {
    _targeted_mode_ = true;
    _setAllMarginalTargets_();
  }


  // ##############################################################################
  // Targets
  // ##############################################################################

  // return true if variable is a target
  template < typename GUM_SCALAR >
  INLINE bool MarginalTargetedMRFInference< GUM_SCALAR >::isTarget(NodeId node) const {
    // check that the variable belongs to the mn
    if (this->hasNoModel_())
      GUM_ERROR(NullElement,
                "No Markov net has been assigned to the "
                "inference algorithm");
    if (!this->MRF().graph().exists(node)) {
      GUM_ERROR(UndefinedElement, node << " is not a NodeId in the Markov random field")
    }

    return _targets_.contains(node);
  }

  // Add a single target to the list of targets
  template < typename GUM_SCALAR >
  INLINE bool
     MarginalTargetedMRFInference< GUM_SCALAR >::isTarget(const std::string& nodeName) const {
    return isTarget(this->MRF().idFromName(nodeName));
  }


  // Clear all previously defined targets (single targets and sets of targets)
  template < typename GUM_SCALAR >
  INLINE void MarginalTargetedMRFInference< GUM_SCALAR >::eraseAllTargets() {
    onAllMarginalTargetsErased_();

    _targets_.clear();
    setTargetedMode_();   // does nothing if already in targeted mode

    this->setState_(MRFInference< GUM_SCALAR >::StateOfInference::OutdatedStructure);
  }


  // Add a single target to the list of targets
  template < typename GUM_SCALAR >
  void MarginalTargetedMRFInference< GUM_SCALAR >::addTarget(NodeId target) {
    // check if the node belongs to the Markov random field
    if (this->hasNoModel_())
      GUM_ERROR(NullElement,
                "No Markov net has been assigned to the "
                "inference algorithm");

    if (!this->MRF().graph().exists(target)) {
      GUM_ERROR(UndefinedElement, target << " is not a NodeId in the Markov random field")
    }

    setTargetedMode_();   // does nothing if already in targeted mode
    // add the new target
    if (!_targets_.contains(target)) {
      _targets_.insert(target);
      onMarginalTargetAdded_(target);
      this->setState_(MRFInference< GUM_SCALAR >::StateOfInference::OutdatedStructure);
    }
  }


  // Add all nodes as targets
  template < typename GUM_SCALAR >
  void MarginalTargetedMRFInference< GUM_SCALAR >::addAllTargets() {
    // check if the node belongs to the Markov random field
    if (this->hasNoModel_())
      GUM_ERROR(NullElement,
                "No Markov net has been assigned to the "
                "inference algorithm");


    setTargetedMode_();   // does nothing if already in targeted mode
    for (const auto target: this->MRF().graph()) {
      if (!_targets_.contains(target)) {
        _targets_.insert(target);
        onMarginalTargetAdded_(target);
        this->setState_(MRFInference< GUM_SCALAR >::StateOfInference::OutdatedStructure);
      }
    }
  }


  // Add a single target to the list of targets
  template < typename GUM_SCALAR >
  void MarginalTargetedMRFInference< GUM_SCALAR >::addTarget(const std::string& nodeName) {
    // check if the node belongs to the Markov random field
    if (this->hasNoModel_())
      GUM_ERROR(NullElement,
                "No Markov net has been assigned to the "
                "inference algorithm");

    addTarget(this->MRF().idFromName(nodeName));
  }


  // removes an existing target
  template < typename GUM_SCALAR >
  void MarginalTargetedMRFInference< GUM_SCALAR >::eraseTarget(NodeId target) {
    // check if the node belongs to the Markov random field
    if (this->hasNoModel_())
      GUM_ERROR(NullElement,
                "No Markov net has been assigned to the "
                "inference algorithm");

    if (!this->MRF().graph().exists(target)) {
      GUM_ERROR(UndefinedElement, target << " is not a NodeId in the Markov random field")
    }


    if (_targets_.contains(target)) {
      _targeted_mode_ = true;   // we do not use setTargetedMode_ because we do not
                                // want to clear the targets
      onMarginalTargetErased_(target);
      _targets_.erase(target);
      this->setState_(MRFInference< GUM_SCALAR >::StateOfInference::OutdatedStructure);
    }
  }


  // Add a single target to the list of targets
  template < typename GUM_SCALAR >
  void MarginalTargetedMRFInference< GUM_SCALAR >::eraseTarget(const std::string& nodeName) {
    // check if the node belongs to the Markov random field
    if (this->hasNoModel_())
      GUM_ERROR(NullElement,
                "No Markov net has been assigned to the "
                "inference algorithm");

    eraseTarget(this->MRF().idFromName(nodeName));
  }


  // returns the list of single targets
  template < typename GUM_SCALAR >
  INLINE const NodeSet& MarginalTargetedMRFInference< GUM_SCALAR >::targets() const noexcept {
    return _targets_;
  }

  // returns the list of single targets
  template < typename GUM_SCALAR >
  INLINE Size MarginalTargetedMRFInference< GUM_SCALAR >::nbrTargets() const noexcept {
    return _targets_.size();
  }


  /// sets all the nodes of the Markov net as targets
  template < typename GUM_SCALAR >
  void MarginalTargetedMRFInference< GUM_SCALAR >::_setAllMarginalTargets_() {
    _targets_.clear();
    if (!this->hasNoModel_()) {
      _targets_ = this->MRF().graph().asNodeSet();
      onAllMarginalTargetsAdded_();
    }
  }


  // ##############################################################################
  // Inference
  // ##############################################################################

  // Compute the posterior of a node.
  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
     MarginalTargetedMRFInference< GUM_SCALAR >::posterior(NodeId node) {
    if (this->hardEvidenceNodes().contains(node)) { return *(this->evidence()[node]); }

    if (!isTarget(node)) {
      // throws UndefinedElement if var is not a target
      GUM_ERROR(UndefinedElement, node << " is not a target node")
    }

    if (!this->isInferenceDone()) { this->makeInference(); }

    return posterior_(node);
  }

  // Compute the posterior of a node.
  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
     MarginalTargetedMRFInference< GUM_SCALAR >::posterior(const std::string& nodeName) {
    return posterior(this->MRF().idFromName(nodeName));
  }

  /* Entropy
   * Compute Shanon's entropy of a node given the observation
   */
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR MarginalTargetedMRFInference< GUM_SCALAR >::H(NodeId X) {
    return posterior(X).entropy();
  }

  /* Entropy
   * Compute Shanon's entropy of a node given the observation
   */
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR MarginalTargetedMRFInference< GUM_SCALAR >::H(const std::string& nodeName) {
    return H(this->MRF().idFromName(nodeName));
  }


  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >
     MarginalTargetedMRFInference< GUM_SCALAR >::evidenceImpact(NodeId target, const NodeSet& evs) {
    const auto& vtarget = this->MRF().variable(target);

    if (evs.contains(target)) {
      GUM_ERROR(InvalidArgument,
                "Target <" << vtarget.name() << "> (" << target << ") can not be in evs (" << evs
                           << ").");
    }
    auto condset = this->MRF().minimalCondSet(target, evs);

    Potential< GUM_SCALAR > res;
    this->eraseAllTargets();
    this->eraseAllEvidence();
    res.add(this->MRF().variable(target));
    this->addTarget(target);
    for (const auto& n: condset) {
      res.add(this->MRF().variable(n));
      this->addEvidence(n, 0);
    }

    Instantiation inst(res);
    for (inst.setFirst(); !inst.end(); inst.incNotVar(vtarget)) {
      // inferring
      for (const auto& n: condset)
        this->chgEvidence(n, inst.val(this->MRF().variable(n)));
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
  Potential< GUM_SCALAR > MarginalTargetedMRFInference< GUM_SCALAR >::evidenceImpact(
     const std::string&                target,
     const std::vector< std::string >& evs) {
    const auto& mn = this->MRF();
    return evidenceImpact(mn.idFromName(target), mn.nodeset(evs));
  }


  template < typename GUM_SCALAR >
  INLINE bool MarginalTargetedMRFInference< GUM_SCALAR >::isTargetedMode_() const {
    return _targeted_mode_;
  }
  template < typename GUM_SCALAR >
  INLINE void MarginalTargetedMRFInference< GUM_SCALAR >::setTargetedMode_() {
    if (!_targeted_mode_) {
      _targets_.clear();
      _targeted_mode_ = true;
    }
  }
} /* namespace gum */
