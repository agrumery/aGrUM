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
 * @brief Implementation of the non pure virtual methods of class
 * JointTargetedInference.
 */
#include <agrum/BN/inference/tools/jointTargetedInference.h>
#include <agrum/tools/variables/rangeVariable.h>

namespace gum {


  // Default Constructor
  template < typename GUM_SCALAR >
  JointTargetedInference< GUM_SCALAR >::JointTargetedInference(const IBayesNet< GUM_SCALAR >* bn) :
      MarginalTargetedInference< GUM_SCALAR >(bn) {
    // assign a BN if this has not been done before (due to virtual inheritance)
    if (this->hasNoModel_()) {
      BayesNetInference< GUM_SCALAR >::_setBayesNetDuringConstruction_(bn);
    }
    GUM_CONSTRUCTOR(JointTargetedInference);
  }


  // Destructor
  template < typename GUM_SCALAR >
  JointTargetedInference< GUM_SCALAR >::~JointTargetedInference() {
    GUM_DESTRUCTOR(JointTargetedInference);
  }


  // assigns a new BN to the inference engine
  template < typename GUM_SCALAR >
  void JointTargetedInference< GUM_SCALAR >::onModelChanged_(const GraphicalModel* bn) {
    MarginalTargetedInference< GUM_SCALAR >::onModelChanged_(bn);
    onAllJointTargetsErased_();
    _joint_targets_.clear();
  }


  // ##############################################################################
  // Targets
  // ##############################################################################

  // return true if target is a nodeset target.
  template < typename GUM_SCALAR >
  INLINE bool JointTargetedInference< GUM_SCALAR >::isJointTarget(const NodeSet& vars) const {
    if (this->hasNoModel_())
      GUM_ERROR(NullElement,
                "No Bayes net has been assigned to the "
                "inference algorithm");

    const auto& dag = this->BN().dag();
    for (const auto var: vars) {
      if (!dag.exists(var)) { GUM_ERROR(UndefinedElement, var << " is not a NodeId in the bn") }
    }

    return _joint_targets_.contains(vars);
  }


  // Clear all previously defined single targets
  template < typename GUM_SCALAR >
  INLINE void JointTargetedInference< GUM_SCALAR >::eraseAllMarginalTargets() {
    MarginalTargetedInference< GUM_SCALAR >::eraseAllTargets();
  }


  // Clear all previously defined targets (single targets and sets of targets)
  template < typename GUM_SCALAR >
  INLINE void JointTargetedInference< GUM_SCALAR >::eraseAllJointTargets() {
    if (_joint_targets_.size() > 0) {
      // we already are in target mode. So no this->setTargetedMode_();  is needed
      onAllJointTargetsErased_();
      _joint_targets_.clear();
      this->setState_(GraphicalModelInference< GUM_SCALAR >::StateOfInference::OutdatedStructure);
    }
  }


  // Clear all previously defined targets (single and joint targets)
  template < typename GUM_SCALAR >
  INLINE void JointTargetedInference< GUM_SCALAR >::eraseAllTargets() {
    eraseAllMarginalTargets();
    eraseAllJointTargets();
  }


  // Add a set of nodes as a new target
  template < typename GUM_SCALAR >
  void JointTargetedInference< GUM_SCALAR >::addJointTarget(const NodeSet& joint_target) {
    // check if the nodes in the target belong to the Bayesian network
    if (this->hasNoModel_())
      GUM_ERROR(NullElement,
                "No Bayes net has been assigned to the "
                "inference algorithm");

    const auto& dag = this->BN().dag();
    for (const auto node: joint_target) {
      if (!dag.exists(node)) {
        GUM_ERROR(UndefinedElement,
                  "at least one one in " << joint_target << " does not belong to the bn");
      }
    }

    // check that the joint_target set does not contain the new target
    if (_joint_targets_.contains(joint_target)) return;

    // check if joint_target is a subset of an already existing target
    for (const auto& target: _joint_targets_) {
      if (target.isProperSupersetOf(joint_target)) return;
    }

    // check if joint_target is not a superset of an already existing target
    // in this case, we need to remove old existing target
    for (auto iter = _joint_targets_.beginSafe(); iter != _joint_targets_.endSafe(); ++iter) {
      if (iter->isProperSubsetOf(joint_target)) eraseJointTarget(*iter);
    }

    this->setTargetedMode_();   // does nothing if already in targeted mode
    _joint_targets_.insert(joint_target);
    onJointTargetAdded_(joint_target);
    this->setState_(GraphicalModelInference< GUM_SCALAR >::StateOfInference::OutdatedStructure);
  }


  // removes an existing set target
  template < typename GUM_SCALAR >
  void JointTargetedInference< GUM_SCALAR >::eraseJointTarget(const NodeSet& joint_target) {
    // check if the nodes in the target belong to the Bayesian network
    if (this->hasNoModel_())
      GUM_ERROR(NullElement,
                "No Bayes net has been assigned to the "
                "inference algorithm");

    const auto& dag = this->BN().dag();
    for (const auto node: joint_target) {
      if (!dag.exists(node)) {
        GUM_ERROR(UndefinedElement,
                  "at least one one in " << joint_target << " does not belong to the bn");
      }
    }

    // check that the joint_target set does not contain the new target
    if (_joint_targets_.contains(joint_target)) {
      // note that we have to be in target mode when we are here
      // so, no this->setTargetedMode_();  is necessary
      onJointTargetErased_(joint_target);
      _joint_targets_.erase(joint_target);
      this->setState_(GraphicalModelInference< GUM_SCALAR >::StateOfInference::OutdatedStructure);
    }
  }


  /// returns the list of target sets
  template < typename GUM_SCALAR >
  INLINE const Set< NodeSet >& JointTargetedInference< GUM_SCALAR >::jointTargets() const noexcept {
    return _joint_targets_;
  }

  /// returns the number of target sets
  template < typename GUM_SCALAR >
  INLINE Size JointTargetedInference< GUM_SCALAR >::nbrJointTargets() const noexcept {
    return _joint_targets_.size();
  }


  // ##############################################################################
  // Inference
  // ##############################################################################

  // Compute the posterior of a nodeset.
  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
     JointTargetedInference< GUM_SCALAR >::jointPosterior(const NodeSet& nodes) {
    // try to get the smallest set of targets that contains "nodes"
    NodeSet set;
    bool    found_exact_target = false;

    if (_joint_targets_.contains(nodes)) {
      set                = nodes;
      found_exact_target = true;
    } else {
      for (const auto& target: _joint_targets_) {
        if (nodes.isProperSubsetOf(target)) {
          set = target;
          break;
        }
      }
    }

    if (set.empty()) {
      GUM_ERROR(UndefinedElement,
                " no joint target containing " << nodes << " could be found among "
                                               << _joint_targets_);
    }

    if (!this->isInferenceDone()) { this->makeInference(); }

    if (found_exact_target) return jointPosterior_(nodes);
    else return jointPosterior_(nodes, set);
  }


  // Compute the posterior of a node
  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >& JointTargetedInference< GUM_SCALAR >::posterior(NodeId node) {
    if (this->isTarget(node)) return MarginalTargetedInference< GUM_SCALAR >::posterior(node);
    else return jointPosterior(NodeSet{node});
  }

  // Compute the posterior of a node
  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
     JointTargetedInference< GUM_SCALAR >::posterior(const std::string& nodeName) {
    return posterior(this->BN().idFromName(nodeName));
  }

  // ##############################################################################
  // Mutual Information
  // ##############################################################################
  template < typename GUM_SCALAR >
  GUM_SCALAR JointTargetedInference< GUM_SCALAR >::I(const std::string& Xname,
                                                     const std::string& Yname) {
    return I(this->BN().idFromName(Xname), this->BN().idFromName(Yname));
  }

  template < typename GUM_SCALAR >
  GUM_SCALAR JointTargetedInference< GUM_SCALAR >::VI(const std::string& Xname,
                                                      const std::string& Yname) {
    return VI(this->BN().idFromName(Xname), this->BN().idFromName(Yname));
  }

  /* Mutual information between X and Y
   *
   * @see http://en.wikipedia.org/wiki/Mutual_information
   *
   * @warning Due to limitation of @joint, may not be able to compute this value
   * @throw OperationNotAllowed in these cases
   */
  template < typename GUM_SCALAR >
  GUM_SCALAR JointTargetedInference< GUM_SCALAR >::I(NodeId X, NodeId Y) {
    Potential< GUM_SCALAR > pX, pY, *pXY = nullptr;
    if (X == Y) { GUM_ERROR(OperationNotAllowed, "Mutual Information I(X,Y) with X==Y") }

    try {
      // here use unnormalized joint posterior rather than just posterior
      // to avoid saving the posterior in the cache of the inference engines
      // like LazyPropagation or SahferShenoy.
      pXY = this->unnormalizedJointPosterior_({X, Y});
      pXY->normalize();
      pX = pXY->margSumOut({&(this->BN().variable(Y))});
      pY = pXY->margSumOut({&(this->BN().variable(X))});
    } catch (...) {
      if (pXY != nullptr) { delete pXY; }
      throw;
    }

    Instantiation i(*pXY);
    auto          res = (GUM_SCALAR)0;

    for (i.setFirst(); !i.end(); ++i) {
      GUM_SCALAR vXY = (*pXY)[i];
      GUM_SCALAR vX  = pX[i];
      GUM_SCALAR vY  = pY[i];

      if (vXY > (GUM_SCALAR)0) {
        if (vX == (GUM_SCALAR)0 || vY == (GUM_SCALAR)0) {
          GUM_ERROR(OperationNotAllowed,
                    "Mutual Information (X,Y) with P(X)=0 or P(Y)=0 "
                    "and P(X,Y)>0");
        }

        res += vXY * (std::log2(vXY) - std::log2(vX) - std::log2(vY));
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
  INLINE GUM_SCALAR JointTargetedInference< GUM_SCALAR >::VI(NodeId X, NodeId Y) {
    return this->H(X) + this->H(Y) - 2 * I(X, Y);
  }


  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >
     JointTargetedInference< GUM_SCALAR >::evidenceJointImpact(const NodeSet& targets,
                                                               const NodeSet& evs) {
    if (!(evs * targets).empty()) {
      GUM_ERROR(InvalidArgument,
                "Targets (" << targets << ") can not intersect evs (" << evs << ").");
    }
    auto condset = this->BN().minimalCondSet(targets, evs);

    this->eraseAllTargets();
    this->eraseAllEvidence();

    Instantiation           iTarget;
    Potential< GUM_SCALAR > res;
    for (const auto& target: targets) {
      res.add(this->BN().variable(target));
      iTarget.add(this->BN().variable(target));
    }
    this->addJointTarget(targets);

    for (const auto& n: condset) {
      res.add(this->BN().variable(n));
      this->addEvidence(n, 0);
    }

    Instantiation inst(res);
    for (inst.setFirstOut(iTarget); !inst.end(); inst.incOut(iTarget)) {
      // inferring
      for (const auto& n: condset)
        this->chgEvidence(n, inst.val(this->BN().variable(n)));
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
  Potential< GUM_SCALAR > JointTargetedInference< GUM_SCALAR >::evidenceJointImpact(
     const std::vector< std::string >& targets,
     const std::vector< std::string >& evs) {
    const auto& bn = this->BN();
    return evidenceJointImpact(bn.nodeset(targets), bn.nodeset(evs));
  }


  template < typename GUM_SCALAR >
  GUM_SCALAR JointTargetedInference< GUM_SCALAR >::jointMutualInformation(const NodeSet& targets) {
    const auto& bn  = this->BN();
    const Size  siz = targets.size();
    if (siz <= 1) {
      GUM_ERROR(InvalidArgument,
                "jointMutualInformation needs at least 2 variables (targets=" << targets << ")");
    }

    this->eraseAllTargets();
    this->eraseAllEvidence();
    this->addJointTarget(targets);
    this->makeInference();
    const auto po = this->jointPosterior(targets);

    gum::Instantiation caracteristic;
    gum::Instantiation variables;
    for (const auto nod: targets) {
      const auto& var = bn.variable(nod);
      auto        pv  = new gum::RangeVariable(var.name(), "", 0, 1);
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
  GUM_SCALAR JointTargetedInference< GUM_SCALAR >::jointMutualInformation(
     const std::vector< std::string >& targets) {
    return jointMutualInformation(this->BN().ids(targets));
  }

} /* namespace gum */
