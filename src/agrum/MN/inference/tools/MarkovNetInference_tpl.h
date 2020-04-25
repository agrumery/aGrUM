
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
 * @brief Implementation of the non pure virtual methods of class
 * MarkovNetInference.
 */

#include <agrum/MN/inference/tools/MarkovNetInference.h>

namespace gum {


  // Default Constructor
  template < typename GUM_SCALAR >
  MarkovNetInference< GUM_SCALAR >::MarkovNetInference(
     const IMarkovNet< GUM_SCALAR >* mn) :
      __mn(mn) {
    __computeDomainSizes();

    __targeted_mode = false;
    // sets all the nodes as targets
    if (mn != nullptr) {
      __targets = mn->dag().asNodeSet();
    } else {
      MarkovNetInference< GUM_SCALAR >::__setMarkovNetDuringConstruction(mn);
    }

    GUM_CONSTRUCTOR(MarkovNetInference);
  }


  // Default Constructor
  template < typename GUM_SCALAR >
  MarkovNetInference< GUM_SCALAR >::MarkovNetInference() {
    GUM_CONSTRUCTOR(MarkovNetInference);
  }


  // Destructor
  template < typename GUM_SCALAR >
  MarkovNetInference< GUM_SCALAR >::~MarkovNetInference() {
    // clear all evidence.
    // Warning: Do not use method eraseAllEvidence () because it contains a call
    // to pure virtual method _onAllEvidenceErased which belongs to an inherited
    // instance and, therefore, does not exist anymore when ~MarkovNetInference ()
    // is called
    for (const auto& pair: __evidence) {
      if (pair.second != nullptr) { delete (pair.second); }
    }

    GUM_DESTRUCTOR(MarkovNetInference);
  }


  // returns whether the inference object is in a ready state
  template < typename GUM_SCALAR >
  INLINE bool MarkovNetInference< GUM_SCALAR >::isInferenceReady() const noexcept {
    return (__state == StateOfMNInference::ReadyForInference);
  }
  // returns whether the inference object is in a OutdatedMNStructure state
  template < typename GUM_SCALAR >
  INLINE bool
     MarkovNetInference< GUM_SCALAR >::isInferenceOutdatedMNStructure() const
     noexcept {
    return (__state == StateOfMNInference::OutdatedMNStructure);
  }
  // returns whether the inference object is in a OutdatedMNPotential state
  template < typename GUM_SCALAR >
  INLINE bool
     MarkovNetInference< GUM_SCALAR >::isInferenceOutdatedMNPotentials() const
     noexcept {
    return (__state == StateOfMNInference::OutdatedMNPotentials);
  }
  // returns whether the inference object is in a InferenceDone state
  template < typename GUM_SCALAR >
  INLINE bool MarkovNetInference< GUM_SCALAR >::isInferenceDone() const noexcept {
    return (__state == StateOfMNInference::Done);
  }


  // returns whether the inference object is in a done state
  template < typename GUM_SCALAR >
  INLINE bool MarkovNetInference< GUM_SCALAR >::isDone() const noexcept {
    return (__state == StateOfMNInference::Done);
  }


  // returns the state of the inference engine
  template < typename GUM_SCALAR >
  INLINE typename MarkovNetInference< GUM_SCALAR >::StateOfMNInference
     MarkovNetInference< GUM_SCALAR >::state() const noexcept {
    return __state;
  }

  // set the state of the inference
  template < typename GUM_SCALAR >
  INLINE void
     MarkovNetInference< GUM_SCALAR >::__setState(const StateOfMNInference state) {
    if (__state != state) {
      __state = state;
      _onStateChanged();
    }
  }

  // Returns a constant reference over the IMarkovNet referenced by this class
  template < typename GUM_SCALAR >
  INLINE const IMarkovNet< GUM_SCALAR >&
               MarkovNetInference< GUM_SCALAR >::MN() const {
    if (__mn == nullptr)
      GUM_ERROR(UndefinedElement,
                "No Markov net has been assigned to "
                "the inference algorithm.");
    return *__mn;
  }


  // assigns a new MN to the inference engine
  template < typename GUM_SCALAR >
  void MarkovNetInference< GUM_SCALAR >::setMN(const IMarkovNet< GUM_SCALAR >* mn) {
    clear();
    __mn = mn;
    __computeDomainSizes();
    _onMarkovNetChanged(mn);
    __setState(StateOfMNInference::OutdatedMNStructure);
  }


  // assigns a MN to a newly constructed inference engine
  template < typename GUM_SCALAR >
  void MarkovNetInference< GUM_SCALAR >::__setMarkovNetDuringConstruction(
     const IMarkovNet< GUM_SCALAR >* mn) {
    __mn = mn;
    __computeDomainSizes();
    __setState(StateOfMNInference::OutdatedMNStructure);
  }


  // clears all the data structures allocated for the last inference
  template < typename GUM_SCALAR >
  INLINE void MarkovNetInference< GUM_SCALAR >::clear() {
    eraseAllEvidence();
    __setState(StateOfMNInference::OutdatedMNStructure);
  }


  /// computes the domain sizes of the random variables
  template < typename GUM_SCALAR >
  void MarkovNetInference< GUM_SCALAR >::__computeDomainSizes() {
    __domain_sizes.clear();
    if (__mn != nullptr) {
      for (const auto node: __mn->dag()) {
        __domain_sizes.insert(node, __mn->variable(node).domainSize());
      }
    }
  }


  // get the domain sizes of the random variables of the MN
  template < typename GUM_SCALAR >
  INLINE const NodeProperty< Size >&
               MarkovNetInference< GUM_SCALAR >::domainSizes() const {
    return __domain_sizes;
  }


  // ##############################################################################
  // Evidence
  // ##############################################################################

  // create the internal structure for a hard evidence
  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >
     MarkovNetInference< GUM_SCALAR >::__createHardEvidence(NodeId    id,
                                                           const Idx val) const {
    // check that it is possible to create the evidence
    if (__mn == nullptr)
      GUM_ERROR(NullElement,
                "No Markov net has been assigned to the "
                "inference algorithm");

    if (!__mn->dag().exists(id)) {
      GUM_ERROR(UndefinedElement, id << " is not a NodeId in the mn");
    }

    if (__mn->variable(id).domainSize() <= val) {
      GUM_ERROR(InvalidArgument,
                "node " << __mn->variable(id) << " has fewer possible values than "
                        << val);
    }

    // create the deterministic potential
    Potential< GUM_SCALAR > pot;
    pot.beginMultipleChanges();
    pot << __mn->variable(id);
    pot.endMultipleChanges(0.0);

    Instantiation I(pot);
    I.chgVal(__mn->variable(id), val);
    pot.set(I, 1.0);

    return pot;
  }


  // checks wether a potential corresponds to a hard evidence
  template < typename GUM_SCALAR >
  bool MarkovNetInference< GUM_SCALAR >::__isHardEvidence(
     const Potential< GUM_SCALAR >& pot, Idx& val) const {
    // checking if pot is determininstic
    bool          notZero = false;
    Instantiation I(pot);

    for (I.setFirst(); !I.end(); I.inc()) {
      if (pot[I] != 0.0) {
        if (notZero) {   // we already met a non-zero value
          return false;
        } else {
          val = I.val(0);
          notZero = true;   // this is the first met non-zero value
        }
      }
    }

    if (!notZero) {   // we met no non-zero value
      GUM_ERROR(FatalError, "Evidence of impossibility (vector of 0s)");
    }

    return true;   // pot is deterministic
  }


  // adds a new hard evidence on node id
  template < typename GUM_SCALAR >
  INLINE void MarkovNetInference< GUM_SCALAR >::addEvidence(NodeId    id,
                                                           const Idx val) {
    addEvidence(__createHardEvidence(id, val));
  }

  // adds a new hard evidence on node id
  template < typename GUM_SCALAR >
  INLINE void
     MarkovNetInference< GUM_SCALAR >::addEvidence(const std::string& nodeName,
                                                  const Idx          val) {
    addEvidence(this->MN().idFromName(nodeName), val);
  }

  // adds a new hard evidence on node id
  template < typename GUM_SCALAR >
  INLINE void
     MarkovNetInference< GUM_SCALAR >::addEvidence(NodeId             id,
                                                  const std::string& label) {
    addEvidence(id, this->MN().variable(id)[label]);
  }

  // adds a new hard evidence on node id
  template < typename GUM_SCALAR >
  INLINE void
     MarkovNetInference< GUM_SCALAR >::addEvidence(const std::string& nodeName,
                                                  const std::string& label) {
    NodeId id = this->MN().idFromName(nodeName);
    addEvidence(id, this->MN().variable(id)[label]);
  }

  // adds a new evidence on node id (might be soft or hard)
  template < typename GUM_SCALAR >
  void MarkovNetInference< GUM_SCALAR >::addEvidence(
     NodeId id, const std::vector< GUM_SCALAR >& vals) {
    // checks that the evidence is meaningful
    if (__mn == nullptr)
      GUM_ERROR(NullElement,
                "No Markov net has been assigned to the "
                "inference algorithm");

    if (!__mn->dag().exists(id)) {
      GUM_ERROR(UndefinedElement, id << " is not a NodeId in the mn");
    }

    if (__mn->variable(id).domainSize() != vals.size()) {
      GUM_ERROR(InvalidArgument,
                "node " << __mn->variable(id)
                        << " and its evidence vector have different sizes.");
    }

    Potential< GUM_SCALAR > pot;
    pot.add(__mn->variable(id));
    pot.fillWith(vals);
    addEvidence(std::move(pot));
  }

  // adds a new evidence on node id (might be soft or hard)
  template < typename GUM_SCALAR >
  void MarkovNetInference< GUM_SCALAR >::addEvidence(
     const std::string& nodeName, const std::vector< GUM_SCALAR >& vals) {
    addEvidence(this->MN().idFromName(nodeName), vals);
  }

  // adds a new evidence on node id (might be soft or hard)
  template < typename GUM_SCALAR >
  void
     MarkovNetInference< GUM_SCALAR >::addEvidence(Potential< GUM_SCALAR >&& pot) {
    // check if the potential corresponds to an evidence
    if (pot.nbrDim() != 1) {
      GUM_ERROR(InvalidArgument, pot << " is not mono-dimensional.");
    }
    if (__mn == nullptr)
      GUM_ERROR(NullElement,
                "No Markov net has been assigned to the "
                "inference algorithm");

    NodeId id = __mn->nodeId(pot.variable(0));

    if (hasEvidence(id)) {
      GUM_ERROR(InvalidArgument,
                " node " << id
                         << " already has an evidence. Please use chgEvidence().");
    }

    // check whether we have a hard evidence (and also check whether the
    // potential only contains 0 (in this case, this will automatically raise
    // an exception) )
    Idx  val;
    bool is_hard_evidence = __isHardEvidence(pot, val);

    // insert the evidence
    __evidence.insert(
       id,
       new Potential< GUM_SCALAR >(std::forward< Potential< GUM_SCALAR > >(pot)));
    if (is_hard_evidence) {   // pot is deterministic
      __hard_evidence.insert(id, val);
      __hard_evidence_nodes.insert(id);
    } else {
      __soft_evidence_nodes.insert(id);
    }
    __setState(StateOfMNInference::OutdatedMNStructure);
    _onEvidenceAdded(id, is_hard_evidence);
  }


  // adds a new evidence on node id (might be soft or hard)
  template < typename GUM_SCALAR >
  INLINE void MarkovNetInference< GUM_SCALAR >::addEvidence(
     const Potential< GUM_SCALAR >& pot) {
    Potential< GUM_SCALAR > new_pot(pot);
    addEvidence(std::move(new_pot));
  }


  /// adds a new list of evidence
  template < typename GUM_SCALAR >
  INLINE void MarkovNetInference< GUM_SCALAR >::addListOfEvidence(
     const List< const Potential< GUM_SCALAR >* >& potlist) {
    for (const auto pot: potlist)
      addEvidence(*pot);
  }


  /// adds a new set of evidence
  template < typename GUM_SCALAR >
  INLINE void MarkovNetInference< GUM_SCALAR >::addSetOfEvidence(
     const Set< const Potential< GUM_SCALAR >* >& potset) {
    for (const auto pot: potset)
      addEvidence(*pot);
  }


  // indicates whether some node(s) have received evidence
  template < typename GUM_SCALAR >
  INLINE bool MarkovNetInference< GUM_SCALAR >::hasEvidence() const {
    return !__evidence.empty();
  }


  // indicates whether node id has received an evidence
  template < typename GUM_SCALAR >
  INLINE bool MarkovNetInference< GUM_SCALAR >::hasEvidence(NodeId id) const {
    return __evidence.exists(id);
  }


  // indicates whether node id has received a hard evidence
  template < typename GUM_SCALAR >
  INLINE bool MarkovNetInference< GUM_SCALAR >::hasHardEvidence(NodeId id) const {
    return __hard_evidence_nodes.exists(id);
  }


  // indicates whether node id has received a soft evidence
  template < typename GUM_SCALAR >
  INLINE bool MarkovNetInference< GUM_SCALAR >::hasSoftEvidence(NodeId id) const {
    return __soft_evidence_nodes.exists(id);
  }


  // indicates whether node id has received an evidence
  template < typename GUM_SCALAR >
  INLINE bool MarkovNetInference< GUM_SCALAR >::hasEvidence(
     const std::string& nodeName) const {
    return hasEvidence(this->MN().idFromName(nodeName));
  }


  // indicates whether node id has received a hard evidence
  template < typename GUM_SCALAR >
  INLINE bool MarkovNetInference< GUM_SCALAR >::hasHardEvidence(
     const std::string& nodeName) const {
    return hasHardEvidence(this->MN().idFromName(nodeName));
  }


  // indicates whether node id has received a soft evidence
  template < typename GUM_SCALAR >
  INLINE bool MarkovNetInference< GUM_SCALAR >::hasSoftEvidence(
     const std::string& nodeName) const {
    return hasSoftEvidence(this->MN().idFromName(nodeName));
  }

  // change the value of an already existing hard evidence
  template < typename GUM_SCALAR >
  INLINE void MarkovNetInference< GUM_SCALAR >::chgEvidence(NodeId    id,
                                                           const Idx val) {
    chgEvidence(__createHardEvidence(id, val));
  }

  // change the value of an already existing hard evidence
  template < typename GUM_SCALAR >
  INLINE void
     MarkovNetInference< GUM_SCALAR >::chgEvidence(const std::string& nodeName,
                                                  const Idx          val) {
    chgEvidence(this->MN().idFromName(nodeName), val);
  }

  // change the value of an already existing hard evidence
  template < typename GUM_SCALAR >
  INLINE void
     MarkovNetInference< GUM_SCALAR >::chgEvidence(NodeId             id,
                                                  const std::string& label) {
    chgEvidence(id, this->MN().variable(id)[label]);
  }

  // change the value of an already existing hard evidence
  template < typename GUM_SCALAR >
  INLINE void
     MarkovNetInference< GUM_SCALAR >::chgEvidence(const std::string& nodeName,
                                                  const std::string& label) {
    NodeId id = this->MN().idFromName(nodeName);
    chgEvidence(id, this->MN().variable(id)[label]);
  }

  // change the value of an already existing evidence (might be soft or hard)
  template < typename GUM_SCALAR >
  INLINE void MarkovNetInference< GUM_SCALAR >::chgEvidence(
     NodeId id, const std::vector< GUM_SCALAR >& vals) {
    // check whether this corresponds to an evidence
    if (__mn == nullptr)
      GUM_ERROR(NullElement,
                "No Markov net has been assigned to the "
                "inference algorithm");

    if (!__mn->dag().exists(id)) {
      GUM_ERROR(UndefinedElement, id << " is not a NodeId in the mn");
    }

    if (__mn->variable(id).domainSize() != vals.size()) {
      GUM_ERROR(InvalidArgument,
                "node " << __mn->variable(id)
                        << " and its evidence have different sizes.");
    }

    // create the potential corresponding to vals
    Potential< GUM_SCALAR > pot;
    pot.add(__mn->variable(id));
    pot.fillWith(vals);
    chgEvidence(pot);
  }

  // change the value of an already existing evidence (might be soft or hard)
  template < typename GUM_SCALAR >
  INLINE void MarkovNetInference< GUM_SCALAR >::chgEvidence(
     const std::string& nodeName, const std::vector< GUM_SCALAR >& vals) {
    chgEvidence(this->MN().idFromName(nodeName), vals);
  }


  // change the value of an already existing evidence (might be soft or hard)
  template < typename GUM_SCALAR >
  void MarkovNetInference< GUM_SCALAR >::chgEvidence(
     const Potential< GUM_SCALAR >& pot) {
    // check if the potential corresponds to an evidence
    if (pot.nbrDim() != 1) {
      GUM_ERROR(InvalidArgument, pot << " is not a mono-dimensional potential.");
    }
    if (__mn == nullptr)
      GUM_ERROR(NullElement,
                "No Markov net has been assigned to the "
                "inference algorithm");

    NodeId id = __mn->nodeId(pot.variable(0));

    if (!hasEvidence(id)) {
      GUM_ERROR(InvalidArgument,
                id << " has no evidence. Please use addEvidence().");
    }

    // check whether we have a hard evidence (and also check whether the
    // potential only contains 0 (in this case, this will automatically raise
    // an exception) )
    Idx  val;
    bool is_hard_evidence = __isHardEvidence(pot, val);

    // modify the evidence already stored
    const Potential< GUM_SCALAR >* localPot = __evidence[id];
    Instantiation                  I(pot);
    for (I.setFirst(); !I.end(); I.inc()) {
      localPot->set(I, pot[I]);
    }

    // the inference state will be different
    // whether evidence change from Hard to Soft or not.
    bool hasChangedSoftHard = false;

    if (is_hard_evidence) {
      if (!hasHardEvidence(id)) {
        hasChangedSoftHard = true;
        __hard_evidence.insert(id, val);
        __hard_evidence_nodes.insert(id);
        __soft_evidence_nodes.erase(id);
      } else {
        __hard_evidence[id] = val;
      }
    } else {
      if (hasHardEvidence(id)) {   // evidence was hard
        __hard_evidence.erase(id);
        __hard_evidence_nodes.erase(id);
        __soft_evidence_nodes.insert(id);
        hasChangedSoftHard = true;
      }
    }

    if (hasChangedSoftHard) {
      __setState(StateOfMNInference::OutdatedMNStructure);
    } else {
      if (!isInferenceOutdatedMNStructure()) {
        __setState(StateOfMNInference::OutdatedMNPotentials);
      }
    }

    _onEvidenceChanged(id, hasChangedSoftHard);
  }


  // removed the evidence, if any, corresponding to node id
  template < typename GUM_SCALAR >
  INLINE void MarkovNetInference< GUM_SCALAR >::eraseEvidence(NodeId id) {
    if (hasEvidence(id)) {
      if (hasHardEvidence(id)) {
        _onEvidenceErased(id, true);
        __hard_evidence.erase(id);
        __hard_evidence_nodes.erase(id);
        __setState(StateOfMNInference::OutdatedMNStructure);
      } else {
        _onEvidenceErased(id, false);
        __soft_evidence_nodes.erase(id);
        if (!isInferenceOutdatedMNStructure()) {
          __setState(StateOfMNInference::OutdatedMNPotentials);
        }
      }

      delete (__evidence[id]);
      __evidence.erase(id);
    }
  }
  // removed the evidence, if any, corresponding to node of name nodeName
  template < typename GUM_SCALAR >
  INLINE void
     MarkovNetInference< GUM_SCALAR >::eraseEvidence(const std::string& nodeName) {
    eraseEvidence(this->MN().idFromName(nodeName));
  }


  // removes all the evidence entered into the network
  template < typename GUM_SCALAR >
  INLINE void MarkovNetInference< GUM_SCALAR >::eraseAllEvidence() {
    bool has_hard_evidence = !__hard_evidence.empty();
    this->_onAllEvidenceErased(has_hard_evidence);

    for (const auto& pair: __evidence) {
      if (pair.second != nullptr) { delete (pair.second); }
    }

    __evidence.clear();
    __hard_evidence.clear();
    __hard_evidence_nodes.clear();
    __soft_evidence_nodes.clear();

    if (has_hard_evidence) {
      __setState(StateOfMNInference::OutdatedMNStructure);
    } else {
      if (!isInferenceOutdatedMNStructure()) {
        __setState(StateOfMNInference::OutdatedMNPotentials);
      }
    }
  }


  // returns the number of evidence entered into the Bayesian network
  template < typename GUM_SCALAR >
  INLINE Size MarkovNetInference< GUM_SCALAR >::nbrEvidence() const {
    return __evidence.size();
  }


  // returns the number of hard evidence entered into the Bayesian network
  template < typename GUM_SCALAR >
  INLINE Size MarkovNetInference< GUM_SCALAR >::nbrHardEvidence() const {
    return __hard_evidence_nodes.size();
  }


  // returns the number of soft evidence entered into the Bayesian network
  template < typename GUM_SCALAR >
  INLINE Size MarkovNetInference< GUM_SCALAR >::nbrSoftEvidence() const {
    return __soft_evidence_nodes.size();
  }


  // indicate for each node with hard evidence which value it took
  template < typename GUM_SCALAR >
  INLINE const NodeProperty< Idx >&
               MarkovNetInference< GUM_SCALAR >::hardEvidence() const {
    return __hard_evidence;
  }


  // the set of evidence entered into the network
  template < typename GUM_SCALAR >
  INLINE const NodeProperty< const Potential< GUM_SCALAR >* >&
               MarkovNetInference< GUM_SCALAR >::evidence() const {
    return __evidence;
  }


  /// the set of nodes that received soft evidence
  template < typename GUM_SCALAR >
  INLINE const NodeSet&
               MarkovNetInference< GUM_SCALAR >::softEvidenceNodes() const {
    return __soft_evidence_nodes;
  }


  /// the set of nodes that received hard evidence
  template < typename GUM_SCALAR >
  INLINE const NodeSet&
               MarkovNetInference< GUM_SCALAR >::hardEvidenceNodes() const {
    return __hard_evidence_nodes;
  }


  // ##############################################################################
  // Inference
  // ##############################################################################

  // put the inference into an unprepared state
  template < typename GUM_SCALAR >
  INLINE void MarkovNetInference< GUM_SCALAR >::_setOutdatedMNStructureState() {
    __setState(StateOfMNInference::OutdatedMNStructure);
  }


  /** puts the inference into an OutdatedMNPotentials state if it is not
   *  already in an OutdatedMNStructure state */
  template < typename GUM_SCALAR >
  INLINE void MarkovNetInference< GUM_SCALAR >::_setOutdatedMNPotentialsState() {
    __setState(StateOfMNInference::OutdatedMNPotentials);
  }


  // prepare the internal inference structures for the next inference
  template < typename GUM_SCALAR >
  INLINE void MarkovNetInference< GUM_SCALAR >::prepareInference() {
    if (isInferenceReady() || isDone()) { return; }

    if (__mn == nullptr)
      GUM_ERROR(NullElement,
                "No Markov net has been assigned to the "
                "inference algorithm");

    if (__state == StateOfMNInference::OutdatedMNStructure)
      _updateOutdatedMNStructure();
    else
      _updateOutdatedMNPotentials();

    __setState(StateOfMNInference::ReadyForInference);
  }


  // perform the heavy computations needed to compute the targets' posteriors
  template < typename GUM_SCALAR >
  INLINE void MarkovNetInference< GUM_SCALAR >::makeInference() {
    if (isDone()) { return; }

    if (!isInferenceReady()) { prepareInference(); }

    _makeInference();

    __setState(StateOfMNInference::Done);
  }

  // assigns a new MN to the inference engine
  template < typename GUM_SCALAR >
  void MarkovNetInference< GUM_SCALAR >::_onMarkovNetChanged(
     const IMarkovNet< GUM_SCALAR >* mn) {
    MarginalTargetedInference< GUM_SCALAR >::_onMarkovNetChanged(mn);
    _onAllJointTargetsErased();
    __joint_targets.clear();
  }


  // ##############################################################################
  // Targets
  // ##############################################################################

  // return true if target is a nodeset target.
  template < typename GUM_SCALAR >
  INLINE bool MarkovNetInference< GUM_SCALAR >::isJointTarget(
     const NodeSet& vars) const {
    if (this->__mn == nullptr)
    GUM_ERROR(NullElement,
              "No Markov net has been assigned to the "
              "inference algorithm");

    const auto& dag = this->__mn->dag();
    for (const auto var: vars) {
      if (!dag.exists(var)) {
        GUM_ERROR(UndefinedElement, var << " is not a NodeId in the mn");
      }
    }

    return __joint_targets.contains(vars);
  }


  // Clear all previously defined single targets
  template < typename GUM_SCALAR >
  INLINE void MarkovNetInference< GUM_SCALAR >::eraseAllMarginalTargets() {
    MarginalTargetedInference< GUM_SCALAR >::eraseAllTargets();
  }


  // Clear all previously defined targets (single targets and sets of targets)
  template < typename GUM_SCALAR >
  INLINE void MarkovNetInference< GUM_SCALAR >::eraseAllJointTargets() {
    if (__joint_targets.size() > 0) {
      // we already are in target mode. So no this->_setTargetedMode();  is needed
      _onAllJointTargetsErased();
      __joint_targets.clear();
      this->__setState(
         MarkovNetInference< GUM_SCALAR >::StateOfMNInference::OutdatedMNStructure);
    }
  }


  // Clear all previously defined targets (single and joint targets)
  template < typename GUM_SCALAR >
  INLINE void MarkovNetInference< GUM_SCALAR >::eraseAllTargets() {
    eraseAllMarginalTargets();
    eraseAllJointTargets();
  }


  // Add a set of nodes as a new target
  template < typename GUM_SCALAR >
  void MarkovNetInference< GUM_SCALAR >::addJointTarget(
     const NodeSet& joint_target) {
    // check if the nodes in the target belong to the Markov Network
    if (this->__mn == nullptr)
    GUM_ERROR(NullElement,
              "No Markov net has been assigned to the "
              "inference algorithm");

    const auto& dag = this->__mn->dag();
    for (const auto node: joint_target) {
      if (!dag.exists(node)) {
        GUM_ERROR(UndefinedElement,
                  "at least one one in " << joint_target
                                         << " does not belong to the mn");
      }
    }

    // check that the joint_target set does not contain the new target
    if (__joint_targets.contains(joint_target)) return;

    // check if joint_target is a subset of an already existing target
    for (const auto& target: __joint_targets) {
      if (target.isSupersetOf(joint_target)) return;
    }

    // check if joint_target is not a superset of an already existing target
    // in this case, we need to remove old existing target
    for (auto iter = __joint_targets.beginSafe();
         iter != __joint_targets.endSafe();
         ++iter) {
      if (iter->isSubsetOf(joint_target)) eraseJointTarget(*iter);
    }

    this->_setTargetedMode();   // does nothing if already in targeted mode
    __joint_targets.insert(joint_target);
    _onJointTargetAdded(joint_target);
    this->__setState(
       MarkovNetInference< GUM_SCALAR >::StateOfMNInference::OutdatedMNStructure);
  }


  // removes an existing set target
  template < typename GUM_SCALAR >
  void MarkovNetInference< GUM_SCALAR >::eraseJointTarget(
     const NodeSet& joint_target) {
    // check if the nodes in the target belong to the Markov Network
    if (this->__mn == nullptr)
    GUM_ERROR(NullElement,
              "No Markov net has been assigned to the "
              "inference algorithm");

    const auto& dag = this->__mn->dag();
    for (const auto node: joint_target) {
      if (!dag.exists(node)) {
        GUM_ERROR(UndefinedElement,
                  "at least one one in " << joint_target
                                         << " does not belong to the mn");
      }
    }

    // check that the joint_target set does not contain the new target
    if (__joint_targets.contains(joint_target)) {
      // note that we have to be in target mode when we are here
      // so, no this->_setTargetedMode();  is necessary
      _onJointTargetErased(joint_target);
      __joint_targets.erase(joint_target);
      this->__setState(
         MarkovNetInference< GUM_SCALAR >::StateOfMNInference::OutdatedMNStructure);
    }
  }


  /// returns the list of target sets
  template < typename GUM_SCALAR >
  INLINE const Set< NodeSet >&
  MarkovNetInference< GUM_SCALAR >::jointTargets() const noexcept {
    return __joint_targets;
  }

  /// returns the number of target sets
  template < typename GUM_SCALAR >
  INLINE Size MarkovNetInference< GUM_SCALAR >::nbrJointTargets() const
  noexcept {
    return __joint_targets.size();
  }


  // ##############################################################################
  // Inference
  // ##############################################################################

  // Compute the posterior of a nodeset.
  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
  MarkovNetInference< GUM_SCALAR >::jointPosterior(const NodeSet& nodes) {
    // try to get the smallest set of targets that contains "nodes"
    NodeSet set;
    bool    found_exact_target = false;

    if (__joint_targets.contains(nodes)) {
      set = nodes;
      found_exact_target = true;
    } else {
      for (const auto& target: __joint_targets) {
        if (nodes.isSubsetOf(target)) {
          set = target;
          break;
        }
      }
    }

    if (set.empty()) {
      GUM_ERROR(UndefinedElement,
                " no joint target containing " << nodes << " could be found among "
                                               << __joint_targets);
    }

    if (!this->isDone()) { this->makeInference(); }

    if (found_exact_target)
      return _jointPosterior(nodes);
    else
      return _jointPosterior(nodes, set);
  }


  // Compute the posterior of a node
  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
  MarkovNetInference< GUM_SCALAR >::posterior(NodeId node) {
    if (this->isTarget(node))
      return MarginalTargetedInference< GUM_SCALAR >::posterior(node);
    else
      return jointPosterior(NodeSet{node});
  }

  // Compute the posterior of a node
  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
  MarkovNetInference< GUM_SCALAR >::posterior(const std::string& nodeName) {
    return posterior(this->MN().idFromName(nodeName));
  }

  // ##############################################################################
  // Entropy
  // ##############################################################################


  /* Mutual information between X and Y
   * @see http://en.wikipedia.org/wiki/Mutual_information
   *
   * @warning Due to limitation of @joint, may not be able to compute this value
   * @throw OperationNotAllowed in these cases
   */
  template < typename GUM_SCALAR >
  GUM_SCALAR MarkovNetInference< GUM_SCALAR >::I(NodeId X, NodeId Y) {
    Potential< GUM_SCALAR > pX, pY, *pXY = nullptr;
    if (X == Y) {
      GUM_ERROR(OperationNotAllowed, "Mutual Information I(X,Y) with X==Y");
    }

    try {
      // here use unnormalized joint posterior rather than just posterior
      // to avoid saving the posterior in the cache of the inference engines
      // like LazyPropagation or SahferShenoy.
      pXY = this->_unnormalizedJointPosterior({X, Y});
      pXY->normalize();
      pX = pXY->margSumOut({&(this->MN().variable(Y))});
      pY = pXY->margSumOut({&(this->MN().variable(X))});
    } catch (...) {
      if (pXY != nullptr) { delete pXY; }
      throw;
    }

    Instantiation i(*pXY);
    auto          res = (GUM_SCALAR)0;

    for (i.setFirst(); !i.end(); ++i) {
      GUM_SCALAR vXY = (*pXY)[i];
      GUM_SCALAR vX = pX[i];
      GUM_SCALAR vY = pY[i];

      if (vXY > (GUM_SCALAR)0) {
        if (vX == (GUM_SCALAR)0 || vY == (GUM_SCALAR)0) {
          GUM_ERROR(OperationNotAllowed,
                    "Mutual Information (X,Y) with P(X)=0 or P(Y)=0 "
                    "and P(X,Y)>0");
        }

        res += vXY * (log2(vXY) - log2(vX) - log2(vY));
      }
    }

    delete pXY;

    return res;
  }


  /** Variation of information between X and Y
   * @see http://en.wikipedia.org/wiki/Variation_of_information
   *
   * @warning Due to limitation of @joint, may not be able to compute this value
   * @throw OperationNotAllowed in these cases
   */
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR MarkovNetInference< GUM_SCALAR >::VI(NodeId X, NodeId Y) {
    return this->H(X) + this->H(Y) - 2 * I(X, Y);
  }


  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >
  MarkovNetInference< GUM_SCALAR >::evidenceJointImpact(
     const NodeSet& targets, const NodeSet& evs) {
    if (!(evs * targets).empty()) {
      GUM_ERROR(InvalidArgument,
                "Targets (" << targets << ") can not intersect evs (" << evs
                            << ").");
    }
    auto condset = this->MN().minimalCondSet(targets, evs);

    this->eraseAllTargets();
    this->eraseAllEvidence();

    Instantiation           iTarget;
    Potential< GUM_SCALAR > res;
    for (const auto& target: targets) {
      res.add(this->MN().variable(target));
      iTarget.add(this->MN().variable(target));
    }
    this->addJointTarget(targets);

    for (const auto& n: condset) {
      res.add(this->MN().variable(n));
      this->addEvidence(n, 0);
    }

    Instantiation inst(res);
    for (inst.setFirstOut(iTarget); !inst.end(); inst.incOut(iTarget)) {
      // inferring
      for (const auto& n: condset)
        this->chgEvidence(n, inst.val(this->MN().variable(n)));
      this->makeInference();
      // populate res
      for (inst.setFirstIn(iTarget); !inst.end(); inst.incIn(iTarget)) {
        res.set(inst, this->jointPosterior(targets)[inst]);
      }
      inst.setFirstIn(iTarget);   // remove inst.end() flag
    }

    return res;
  }

  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >
  MarkovNetInference< GUM_SCALAR >::evidenceJointImpact(
     const std::vector< std::string >& targets,
     const std::vector< std::string >& evs) {
    const auto& mn = this->MN();

    gum::NodeSet targetsId;
    for (const auto& targetname: targets) {
      targetsId.insert(mn.idFromName(targetname));
    }

    gum::NodeSet evsId;
    for (const auto& evname: evs) {
      evsId.insert(mn.idFromName(evname));
    }

    return evidenceJointImpact(targetsId, evsId);
  }


  template < typename GUM_SCALAR >
  GUM_SCALAR MarkovNetInference< GUM_SCALAR >::jointMutualInformation(
     const NodeSet& targets) {
    const auto& mn = this->MN();
    const Size  siz = targets.size();
    if (siz <= 1) {
      GUM_ERROR(InvalidArgument,
                "jointMutualInformation needs at least 2 variables (targets="
                   << targets << ")");
    }

    this->eraseAllTargets();
    this->eraseAllEvidence();
    this->addJointTarget(targets);
    this->makeInference();
    const auto po = this->jointPosterior(targets);

    gum::Instantiation caracteristic;
    gum::Instantiation variables;
    for (const auto nod: targets) {
      const auto& var = mn.variable(nod);
      auto        pv = new gum::RangeVariable(var.name(), "", 0, 1);
      caracteristic.add(*pv);
      variables.add(var);
    }

    Set< const DiscreteVariable* > sov;

    const GUM_SCALAR start = (siz % 2 == 0) ? GUM_SCALAR(-1.0) : GUM_SCALAR(1.0);
    GUM_SCALAR       sign;
    GUM_SCALAR       res = GUM_SCALAR(0.0);

    caracteristic.setFirst();
    for (caracteristic.inc(); !caracteristic.end(); caracteristic.inc()) {
      sov.clear();
      sign = start;
      for (Idx i = 0; i < caracteristic.nbrDim(); i++) {
        if (caracteristic.val(i) == 1) {
          sign = -sign;
          sov.insert(&variables.variable(i));
        }
      }
      res += sign * po.margSumIn(sov).entropy();
    }

    for (Idx i = 0; i < caracteristic.nbrDim(); i++) {
      delete &caracteristic.variable(i);
    }

    return res;
  }

  template < typename GUM_SCALAR >
  GUM_SCALAR MarkovNetInference< GUM_SCALAR >::jointMutualInformation(
     const std::vector< std::string >& targets) {
    const auto& mn = this->MN();

    gum::NodeSet targetsId;
    for (const auto& targetname: targets) {
      targetsId.insert(mn.idFromName(targetname));
    }

    return jointMutualInformation(targetsId);
  }



  // fired when a new MN is assigned to the inference engine
  template < typename GUM_SCALAR >
  void MarkovNetInference< GUM_SCALAR >::_onMarkovNetChanged(
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
  MarkovNetInference< GUM_SCALAR >::isTarget(NodeId node) const {
    // check that the variable belongs to the mn
    if (this->__mn == nullptr)
    GUM_ERROR(NullElement,
              "No Markov net has been assigned to the "
              "inference algorithm");
    if (!this->__mn->dag().exists(node)) {
      GUM_ERROR(UndefinedElement, node << " is not a NodeId in the mn");
    }

    return __targets.contains(node);
  }

  // Add a single target to the list of targets
  template < typename GUM_SCALAR >
  INLINE bool MarkovNetInference< GUM_SCALAR >::isTarget(
     const std::string& nodeName) const {
    return isTarget(this->__mn->idFromName(nodeName));
  }


  // Clear all previously defined targets (single targets and sets of targets)
  template < typename GUM_SCALAR >
  INLINE void MarkovNetInference< GUM_SCALAR >::eraseAllTargets() {
    _onAllMarginalTargetsErased();

    __targets.clear();
    _setTargetedMode();   // does nothing if already in targeted mode

    this->__setState(
       MarkovNetInference< GUM_SCALAR >::StateOfMNInference::OutdatedMNStructure);
  }


  // Add a single target to the list of targets
  template < typename GUM_SCALAR >
  void MarkovNetInference< GUM_SCALAR >::addTarget(NodeId target) {
    // check if the node belongs to the Markov Network
    if (this->__mn == nullptr)
    GUM_ERROR(NullElement,
              "No Markov net has been assigned to the "
              "inference algorithm");

    if (!this->__mn->dag().exists(target)) {
      GUM_ERROR(UndefinedElement, target << " is not a NodeId in the mn");
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
  void MarkovNetInference< GUM_SCALAR >::addAllTargets() {
    // check if the node belongs to the Markov Network
    if (this->__mn == nullptr)
    GUM_ERROR(NullElement,
              "No Markov net has been assigned to the "
              "inference algorithm");


    _setTargetedMode();   // does nothing if already in targeted mode
    for (const auto target: this->__mn->dag()) {
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
  void MarkovNetInference< GUM_SCALAR >::addTarget(
     const std::string& nodeName) {
    // check if the node belongs to the Markov Network
    if (this->__mn == nullptr)
    GUM_ERROR(NullElement,
              "No Markov net has been assigned to the "
              "inference algorithm");

    addTarget(this->__mn->idFromName(nodeName));
  }


  // removes an existing target
  template < typename GUM_SCALAR >
  void MarkovNetInference< GUM_SCALAR >::eraseTarget(NodeId target) {
    // check if the node belongs to the Markov Network
    if (this->__mn == nullptr)
    GUM_ERROR(NullElement,
              "No Markov net has been assigned to the "
              "inference algorithm");

    if (!this->__mn->dag().exists(target)) {
      GUM_ERROR(UndefinedElement, target << " is not a NodeId in the mn");
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
  void MarkovNetInference< GUM_SCALAR >::eraseTarget(
     const std::string& nodeName) {
    // check if the node belongs to the Markov Network
    if (this->__mn == nullptr)
    GUM_ERROR(NullElement,
              "No Markov net has been assigned to the "
              "inference algorithm");

    eraseTarget(this->__mn->idFromName(nodeName));
  }


  // returns the list of single targets
  template < typename GUM_SCALAR >
  INLINE const NodeSet& MarkovNetInference< GUM_SCALAR >::targets() const
  noexcept {
    return __targets;
  }

  // returns the list of single targets
  template < typename GUM_SCALAR >
  INLINE const Size MarkovNetInference< GUM_SCALAR >::nbrTargets() const
  noexcept {
    return __targets.size();
  }


  /// sets all the nodes of the Markov net as targets
  template < typename GUM_SCALAR >
  void MarkovNetInference< GUM_SCALAR >::__setAllMarginalTargets() {
    __targets.clear();
    if (this->__mn != nullptr) {
      __targets = this->__mn->dag().asNodeSet();
      _onAllMarginalTargetsAdded();
    }
  }


  // ##############################################################################
  // Inference
  // ##############################################################################

  // Compute the posterior of a node.
  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
  MarkovNetInference< GUM_SCALAR >::posterior(NodeId node) {
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
  MarkovNetInference< GUM_SCALAR >::posterior(
     const std::string& nodeName) {
    return posterior(this->MN().idFromName(nodeName));
  }

  /* Entropy
   * Compute Shanon's entropy of a node given the observation
   */
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR MarkovNetInference< GUM_SCALAR >::H(NodeId X) {
    return posterior(X).entropy();
  }

  /* Entropy
   * Compute Shanon's entropy of a node given the observation
   */
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR
  MarkovNetInference< GUM_SCALAR >::H(const std::string& nodeName) {
    return H(this->MN().idFromName(nodeName));
  }


  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >
  MarkovNetInference< GUM_SCALAR >::evidenceImpact(NodeId         target,
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
  Potential< GUM_SCALAR > MarkovNetInference< GUM_SCALAR >::evidenceImpact(
     const std::string& target, const std::vector< std::string >& evs) {
    const auto& mn = this->MN();

    gum::NodeSet evsId;
    for (const auto& evname: evs) {
      evsId.insert(mn.idFromName(evname));
    }

    return evidenceImpact(mn.idFromName(target), evsId);
  }


  template < typename GUM_SCALAR >
  INLINE bool MarkovNetInference< GUM_SCALAR >::_isTargetedMode() const {
    return __targeted_mode;
  }
  template < typename GUM_SCALAR >
  INLINE void MarkovNetInference< GUM_SCALAR >::_setTargetedMode() {
    if (!__targeted_mode) {
      __targets.clear();
      __targeted_mode = true;
    }
  }

} /* namespace gum */
