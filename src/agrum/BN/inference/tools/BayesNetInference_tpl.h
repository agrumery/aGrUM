
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
 * @brief Implementation of the non pure virtual methods of class
 * BayesNetInference.
 */

#include <agrum/BN/inference/tools/BayesNetInference.h>

namespace gum {


  // Default Constructor
  template < typename GUM_SCALAR >
  BayesNetInference< GUM_SCALAR >::BayesNetInference(
     const IBayesNet< GUM_SCALAR >* bn) :
      bn__(bn) {
    computeDomainSizes__();

    GUM_CONSTRUCTOR(BayesNetInference);
  }


  // Default Constructor
  template < typename GUM_SCALAR >
  BayesNetInference< GUM_SCALAR >::BayesNetInference() {
    GUM_CONSTRUCTOR(BayesNetInference);
  }


  // Destructor
  template < typename GUM_SCALAR >
  BayesNetInference< GUM_SCALAR >::~BayesNetInference() {
    // clear all evidence.
    // Warning: Do not use method eraseAllEvidence () because it contains a call
    // to pure virtual method onAllEvidenceErased_ which belongs to an inherited
    // instance and, therefore, does not exist anymore when ~BayesNetInference ()
    // is called
    for (const auto& pair: evidence__) {
      if (pair.second != nullptr) { delete (pair.second); }
    }

    GUM_DESTRUCTOR(BayesNetInference);
  }


  // returns whether the inference object is in a ready state
  template < typename GUM_SCALAR >
  INLINE bool BayesNetInference< GUM_SCALAR >::isInferenceReady() const noexcept {
    return (state__ == StateOfInference::ReadyForInference);
  }
  // returns whether the inference object is in a OutdatedBNStructure state
  template < typename GUM_SCALAR >
  INLINE bool BayesNetInference< GUM_SCALAR >::isInferenceOutdatedBNStructure()
     const noexcept {
    return (state__ == StateOfInference::OutdatedBNStructure);
  }
  // returns whether the inference object is in a OutdatedBNPotential state
  template < typename GUM_SCALAR >
  INLINE bool BayesNetInference< GUM_SCALAR >::isInferenceOutdatedBNPotentials()
     const noexcept {
    return (state__ == StateOfInference::OutdatedBNPotentials);
  }
  // returns whether the inference object is in a InferenceDone state
  template < typename GUM_SCALAR >
  INLINE bool BayesNetInference< GUM_SCALAR >::isInferenceDone() const noexcept {
    return (state__ == StateOfInference::Done);
  }


  // returns whether the inference object is in a done state
  template < typename GUM_SCALAR >
  INLINE bool BayesNetInference< GUM_SCALAR >::isDone() const noexcept {
    return (state__ == StateOfInference::Done);
  }


  // returns the state of the inference engine
  template < typename GUM_SCALAR >
  INLINE typename BayesNetInference< GUM_SCALAR >::StateOfInference
     BayesNetInference< GUM_SCALAR >::state() const noexcept {
    return state__;
  }

  // set the state of the inference
  template < typename GUM_SCALAR >
  INLINE void
     BayesNetInference< GUM_SCALAR >::setState__(const StateOfInference state) {
    if (state__ != state) {
      state__ = state;
      onStateChanged_();
    }
  }

  // Returns a constant reference over the IBayesNet referenced by this class
  template < typename GUM_SCALAR >
  INLINE const IBayesNet< GUM_SCALAR >&
               BayesNetInference< GUM_SCALAR >::BN() const {
    if (bn__ == nullptr)
      GUM_ERROR(UndefinedElement,
                "No Bayes net has been assigned to "
                "the inference algorithm.");
    return *bn__;
  }


  // assigns a new BN to the inference engine
  template < typename GUM_SCALAR >
  void BayesNetInference< GUM_SCALAR >::setBN(const IBayesNet< GUM_SCALAR >* bn) {
    clear();
    bn__ = bn;
    computeDomainSizes__();
    onBayesNetChanged_(bn);
    setState__(StateOfInference::OutdatedBNStructure);
  }


  // assigns a BN to a newly constructed inference engine
  template < typename GUM_SCALAR >
  void BayesNetInference< GUM_SCALAR >::setBayesNetDuringConstruction__(
     const IBayesNet< GUM_SCALAR >* bn) {
    bn__ = bn;
    computeDomainSizes__();
    setState__(StateOfInference::OutdatedBNStructure);
  }


  // clears all the data structures allocated for the last inference
  template < typename GUM_SCALAR >
  INLINE void BayesNetInference< GUM_SCALAR >::clear() {
    eraseAllEvidence();
    setState__(StateOfInference::OutdatedBNStructure);
  }


  /// computes the domain sizes of the random variables
  template < typename GUM_SCALAR >
  void BayesNetInference< GUM_SCALAR >::computeDomainSizes__() {
    domain_sizes__.clear();
    if (bn__ != nullptr) {
      for (const auto node: bn__->dag()) {
        domain_sizes__.insert(node, bn__->variable(node).domainSize());
      }
    }
  }


  // get the domain sizes of the random variables of the BN
  template < typename GUM_SCALAR >
  INLINE const NodeProperty< Size >&
               BayesNetInference< GUM_SCALAR >::domainSizes() const {
    return domain_sizes__;
  }


  // ##############################################################################
  // Evidence
  // ##############################################################################

  // create the internal structure for a hard evidence
  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >
     BayesNetInference< GUM_SCALAR >::createHardEvidence__(NodeId    id,
                                                           const Idx val) const {
    // check that it is possible to create the evidence
    if (bn__ == nullptr)
      GUM_ERROR(NullElement,
                "No Bayes net has been assigned to the "
                "inference algorithm");

    if (!bn__->dag().exists(id)) {
      GUM_ERROR(UndefinedElement, id << " is not a NodeId in the bn");
    }

    if (bn__->variable(id).domainSize() <= val) {
      GUM_ERROR(InvalidArgument,
                "node " << bn__->variable(id) << " has fewer possible values than "
                        << val);
    }

    // create the deterministic potential
    Potential< GUM_SCALAR > pot;
    pot.beginMultipleChanges();
    pot << bn__->variable(id);
    pot.endMultipleChanges(0.0);

    Instantiation I(pot);
    I.chgVal(bn__->variable(id), val);
    pot.set(I, 1.0);

    return pot;
  }


  // checks wether a potential corresponds to a hard evidence
  template < typename GUM_SCALAR >
  bool BayesNetInference< GUM_SCALAR >::isHardEvidence__(
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
  INLINE void BayesNetInference< GUM_SCALAR >::addEvidence(NodeId    id,
                                                           const Idx val) {
    addEvidence(createHardEvidence__(id, val));
  }

  // adds a new hard evidence on node id
  template < typename GUM_SCALAR >
  INLINE void
     BayesNetInference< GUM_SCALAR >::addEvidence(const std::string& nodeName,
                                                  const Idx          val) {
    addEvidence(this->BN().idFromName(nodeName), val);
  }

  // adds a new hard evidence on node id
  template < typename GUM_SCALAR >
  INLINE void
     BayesNetInference< GUM_SCALAR >::addEvidence(NodeId             id,
                                                  const std::string& label) {
    addEvidence(id, this->BN().variable(id)[label]);
  }

  // adds a new hard evidence on node id
  template < typename GUM_SCALAR >
  INLINE void
     BayesNetInference< GUM_SCALAR >::addEvidence(const std::string& nodeName,
                                                  const std::string& label) {
    NodeId id = this->BN().idFromName(nodeName);
    addEvidence(id, this->BN().variable(id)[label]);
  }

  // adds a new evidence on node id (might be soft or hard)
  template < typename GUM_SCALAR >
  void BayesNetInference< GUM_SCALAR >::addEvidence(
     NodeId id, const std::vector< GUM_SCALAR >& vals) {
    // checks that the evidence is meaningful
    if (bn__ == nullptr)
      GUM_ERROR(NullElement,
                "No Bayes net has been assigned to the "
                "inference algorithm");

    if (!bn__->dag().exists(id)) {
      GUM_ERROR(UndefinedElement, id << " is not a NodeId in the bn");
    }

    if (bn__->variable(id).domainSize() != vals.size()) {
      GUM_ERROR(InvalidArgument,
                "node " << bn__->variable(id)
                        << " and its evidence vector have different sizes.");
    }

    Potential< GUM_SCALAR > pot;
    pot.add(bn__->variable(id));
    pot.fillWith(vals);
    addEvidence(std::move(pot));
  }

  // adds a new evidence on node id (might be soft or hard)
  template < typename GUM_SCALAR >
  void BayesNetInference< GUM_SCALAR >::addEvidence(
     const std::string& nodeName, const std::vector< GUM_SCALAR >& vals) {
    addEvidence(this->BN().idFromName(nodeName), vals);
  }

  // adds a new evidence on node id (might be soft or hard)
  template < typename GUM_SCALAR >
  void
     BayesNetInference< GUM_SCALAR >::addEvidence(Potential< GUM_SCALAR >&& pot) {
    // check if the potential corresponds to an evidence
    if (pot.nbrDim() != 1) {
      GUM_ERROR(InvalidArgument, pot << " is not mono-dimensional.");
    }
    if (bn__ == nullptr)
      GUM_ERROR(NullElement,
                "No Bayes net has been assigned to the "
                "inference algorithm");

    NodeId id = bn__->nodeId(pot.variable(0));

    if (hasEvidence(id)) {
      GUM_ERROR(InvalidArgument,
                " node " << id
                         << " already has an evidence. Please use chgEvidence().");
    }

    // check whether we have a hard evidence (and also check whether the
    // potential only contains 0 (in this case, this will automatically raise
    // an exception) )
    Idx  val;
    bool is_hard_evidence = isHardEvidence__(pot, val);

    // insert the evidence
    evidence__.insert(
       id,
       new Potential< GUM_SCALAR >(std::forward< Potential< GUM_SCALAR > >(pot)));
    if (is_hard_evidence) {   // pot is deterministic
      hard_evidence__.insert(id, val);
      hard_evidence_nodes__.insert(id);
    } else {
      soft_evidence_nodes__.insert(id);
    }
    setState__(StateOfInference::OutdatedBNStructure);
    onEvidenceAdded_(id, is_hard_evidence);
  }


  // adds a new evidence on node id (might be soft or hard)
  template < typename GUM_SCALAR >
  INLINE void BayesNetInference< GUM_SCALAR >::addEvidence(
     const Potential< GUM_SCALAR >& pot) {
    Potential< GUM_SCALAR > new_pot(pot);
    addEvidence(std::move(new_pot));
  }


  /// adds a new list of evidence
  template < typename GUM_SCALAR >
  INLINE void BayesNetInference< GUM_SCALAR >::addListOfEvidence(
     const List< const Potential< GUM_SCALAR >* >& potlist) {
    for (const auto pot: potlist)
      addEvidence(*pot);
  }


  /// adds a new set of evidence
  template < typename GUM_SCALAR >
  INLINE void BayesNetInference< GUM_SCALAR >::addSetOfEvidence(
     const Set< const Potential< GUM_SCALAR >* >& potset) {
    for (const auto pot: potset)
      addEvidence(*pot);
  }


  // indicates whether some node(s) have received evidence
  template < typename GUM_SCALAR >
  INLINE bool BayesNetInference< GUM_SCALAR >::hasEvidence() const {
    return !evidence__.empty();
  }


  // indicates whether node id has received an evidence
  template < typename GUM_SCALAR >
  INLINE bool BayesNetInference< GUM_SCALAR >::hasEvidence(NodeId id) const {
    return evidence__.exists(id);
  }


  // indicates whether node id has received a hard evidence
  template < typename GUM_SCALAR >
  INLINE bool BayesNetInference< GUM_SCALAR >::hasHardEvidence(NodeId id) const {
    return hard_evidence_nodes__.exists(id);
  }


  // indicates whether node id has received a soft evidence
  template < typename GUM_SCALAR >
  INLINE bool BayesNetInference< GUM_SCALAR >::hasSoftEvidence(NodeId id) const {
    return soft_evidence_nodes__.exists(id);
  }


  // indicates whether node id has received an evidence
  template < typename GUM_SCALAR >
  INLINE bool BayesNetInference< GUM_SCALAR >::hasEvidence(
     const std::string& nodeName) const {
    return hasEvidence(this->BN().idFromName(nodeName));
  }


  // indicates whether node id has received a hard evidence
  template < typename GUM_SCALAR >
  INLINE bool BayesNetInference< GUM_SCALAR >::hasHardEvidence(
     const std::string& nodeName) const {
    return hasHardEvidence(this->BN().idFromName(nodeName));
  }


  // indicates whether node id has received a soft evidence
  template < typename GUM_SCALAR >
  INLINE bool BayesNetInference< GUM_SCALAR >::hasSoftEvidence(
     const std::string& nodeName) const {
    return hasSoftEvidence(this->BN().idFromName(nodeName));
  }

  // change the value of an already existing hard evidence
  template < typename GUM_SCALAR >
  INLINE void BayesNetInference< GUM_SCALAR >::chgEvidence(NodeId    id,
                                                           const Idx val) {
    chgEvidence(createHardEvidence__(id, val));
  }

  // change the value of an already existing hard evidence
  template < typename GUM_SCALAR >
  INLINE void
     BayesNetInference< GUM_SCALAR >::chgEvidence(const std::string& nodeName,
                                                  const Idx          val) {
    chgEvidence(this->BN().idFromName(nodeName), val);
  }

  // change the value of an already existing hard evidence
  template < typename GUM_SCALAR >
  INLINE void
     BayesNetInference< GUM_SCALAR >::chgEvidence(NodeId             id,
                                                  const std::string& label) {
    chgEvidence(id, this->BN().variable(id)[label]);
  }

  // change the value of an already existing hard evidence
  template < typename GUM_SCALAR >
  INLINE void
     BayesNetInference< GUM_SCALAR >::chgEvidence(const std::string& nodeName,
                                                  const std::string& label) {
    NodeId id = this->BN().idFromName(nodeName);
    chgEvidence(id, this->BN().variable(id)[label]);
  }

  // change the value of an already existing evidence (might be soft or hard)
  template < typename GUM_SCALAR >
  INLINE void BayesNetInference< GUM_SCALAR >::chgEvidence(
     NodeId id, const std::vector< GUM_SCALAR >& vals) {
    // check whether this corresponds to an evidence
    if (bn__ == nullptr)
      GUM_ERROR(NullElement,
                "No Bayes net has been assigned to the "
                "inference algorithm");

    if (!bn__->dag().exists(id)) {
      GUM_ERROR(UndefinedElement, id << " is not a NodeId in the bn");
    }

    if (bn__->variable(id).domainSize() != vals.size()) {
      GUM_ERROR(InvalidArgument,
                "node " << bn__->variable(id)
                        << " and its evidence have different sizes.");
    }

    // create the potential corresponding to vals
    Potential< GUM_SCALAR > pot;
    pot.add(bn__->variable(id));
    pot.fillWith(vals);
    chgEvidence(pot);
  }

  // change the value of an already existing evidence (might be soft or hard)
  template < typename GUM_SCALAR >
  INLINE void BayesNetInference< GUM_SCALAR >::chgEvidence(
     const std::string& nodeName, const std::vector< GUM_SCALAR >& vals) {
    chgEvidence(this->BN().idFromName(nodeName), vals);
  }


  // change the value of an already existing evidence (might be soft or hard)
  template < typename GUM_SCALAR >
  void BayesNetInference< GUM_SCALAR >::chgEvidence(
     const Potential< GUM_SCALAR >& pot) {
    // check if the potential corresponds to an evidence
    if (pot.nbrDim() != 1) {
      GUM_ERROR(InvalidArgument, pot << " is not a mono-dimensional potential.");
    }
    if (bn__ == nullptr)
      GUM_ERROR(NullElement,
                "No Bayes net has been assigned to the "
                "inference algorithm");

    NodeId id = bn__->nodeId(pot.variable(0));

    if (!hasEvidence(id)) {
      GUM_ERROR(InvalidArgument,
                id << " has no evidence. Please use addEvidence().");
    }

    // check whether we have a hard evidence (and also check whether the
    // potential only contains 0 (in this case, this will automatically raise
    // an exception) )
    Idx  val;
    bool is_hard_evidence = isHardEvidence__(pot, val);

    // modify the evidence already stored
    const Potential< GUM_SCALAR >* localPot = evidence__[id];
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
        hard_evidence__.insert(id, val);
        hard_evidence_nodes__.insert(id);
        soft_evidence_nodes__.erase(id);
      } else {
        hard_evidence__[id] = val;
      }
    } else {
      if (hasHardEvidence(id)) {   // evidence was hard
        hard_evidence__.erase(id);
        hard_evidence_nodes__.erase(id);
        soft_evidence_nodes__.insert(id);
        hasChangedSoftHard = true;
      }
    }

    if (hasChangedSoftHard) {
      setState__(StateOfInference::OutdatedBNStructure);
    } else {
      if (!isInferenceOutdatedBNStructure()) {
        setState__(StateOfInference::OutdatedBNPotentials);
      }
    }

    onEvidenceChanged_(id, hasChangedSoftHard);
  }


  // removed the evidence, if any, corresponding to node id
  template < typename GUM_SCALAR >
  INLINE void BayesNetInference< GUM_SCALAR >::eraseEvidence(NodeId id) {
    if (hasEvidence(id)) {
      if (hasHardEvidence(id)) {
        onEvidenceErased_(id, true);
        hard_evidence__.erase(id);
        hard_evidence_nodes__.erase(id);
        setState__(StateOfInference::OutdatedBNStructure);
      } else {
        onEvidenceErased_(id, false);
        soft_evidence_nodes__.erase(id);
        if (!isInferenceOutdatedBNStructure()) {
          setState__(StateOfInference::OutdatedBNPotentials);
        }
      }

      delete (evidence__[id]);
      evidence__.erase(id);
    }
  }
  // removed the evidence, if any, corresponding to node of name nodeName
  template < typename GUM_SCALAR >
  INLINE void
     BayesNetInference< GUM_SCALAR >::eraseEvidence(const std::string& nodeName) {
    eraseEvidence(this->BN().idFromName(nodeName));
  }


  // removes all the evidence entered into the network
  template < typename GUM_SCALAR >
  INLINE void BayesNetInference< GUM_SCALAR >::eraseAllEvidence() {
    bool has_hard_evidence = !hard_evidence__.empty();
    this->onAllEvidenceErased_(has_hard_evidence);

    for (const auto& pair: evidence__) {
      if (pair.second != nullptr) { delete (pair.second); }
    }

    evidence__.clear();
    hard_evidence__.clear();
    hard_evidence_nodes__.clear();
    soft_evidence_nodes__.clear();

    if (has_hard_evidence) {
      setState__(StateOfInference::OutdatedBNStructure);
    } else {
      if (!isInferenceOutdatedBNStructure()) {
        setState__(StateOfInference::OutdatedBNPotentials);
      }
    }
  }


  // returns the number of evidence entered into the Bayesian network
  template < typename GUM_SCALAR >
  INLINE Size BayesNetInference< GUM_SCALAR >::nbrEvidence() const {
    return evidence__.size();
  }


  // returns the number of hard evidence entered into the Bayesian network
  template < typename GUM_SCALAR >
  INLINE Size BayesNetInference< GUM_SCALAR >::nbrHardEvidence() const {
    return hard_evidence_nodes__.size();
  }


  // returns the number of soft evidence entered into the Bayesian network
  template < typename GUM_SCALAR >
  INLINE Size BayesNetInference< GUM_SCALAR >::nbrSoftEvidence() const {
    return soft_evidence_nodes__.size();
  }


  // indicate for each node with hard evidence which value it took
  template < typename GUM_SCALAR >
  INLINE const NodeProperty< Idx >&
               BayesNetInference< GUM_SCALAR >::hardEvidence() const {
    return hard_evidence__;
  }


  // the set of evidence entered into the network
  template < typename GUM_SCALAR >
  INLINE const NodeProperty< const Potential< GUM_SCALAR >* >&
               BayesNetInference< GUM_SCALAR >::evidence() const {
    return evidence__;
  }


  /// the set of nodes that received soft evidence
  template < typename GUM_SCALAR >
  INLINE const NodeSet&
               BayesNetInference< GUM_SCALAR >::softEvidenceNodes() const {
    return soft_evidence_nodes__;
  }


  /// the set of nodes that received hard evidence
  template < typename GUM_SCALAR >
  INLINE const NodeSet&
               BayesNetInference< GUM_SCALAR >::hardEvidenceNodes() const {
    return hard_evidence_nodes__;
  }


  // ##############################################################################
  // Inference
  // ##############################################################################

  // put the inference into an unprepared state
  template < typename GUM_SCALAR >
  INLINE void BayesNetInference< GUM_SCALAR >::setOutdatedBNStructureState_() {
    setState__(StateOfInference::OutdatedBNStructure);
  }


  /** puts the inference into an OutdatedBNPotentials state if it is not
   *  already in an OutdatedBNStructure state */
  template < typename GUM_SCALAR >
  INLINE void BayesNetInference< GUM_SCALAR >::setOutdatedBNPotentialsState_() {
    setState__(StateOfInference::OutdatedBNPotentials);
  }


  // prepare the internal inference structures for the next inference
  template < typename GUM_SCALAR >
  INLINE void BayesNetInference< GUM_SCALAR >::prepareInference() {
    if (isInferenceReady() || isDone()) { return; }

    if (bn__ == nullptr)
      GUM_ERROR(NullElement,
                "No Bayes net has been assigned to the "
                "inference algorithm");

    if (state__ == StateOfInference::OutdatedBNStructure)
      updateOutdatedBNStructure_();
    else
      updateOutdatedBNPotentials_();

    setState__(StateOfInference::ReadyForInference);
  }


  // perform the heavy computations needed to compute the targets' posteriors
  template < typename GUM_SCALAR >
  INLINE void BayesNetInference< GUM_SCALAR >::makeInference() {
    if (isDone()) { return; }

    if (!isInferenceReady()) { prepareInference(); }

    makeInference_();

    setState__(StateOfInference::Done);
  }


} /* namespace gum */
