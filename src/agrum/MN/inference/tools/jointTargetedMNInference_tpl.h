
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
 * JointTargetedMNInference.
 */
#include <agrum/MN/inference/tools/jointTargetedMNInference.h>
#include <agrum/tools/variables/rangeVariable.h>

namespace gum {


  // Default Constructor
  template < typename GUM_SCALAR >
  JointTargetedMNInference< GUM_SCALAR >::JointTargetedMNInference(
     const IMarkovNet< GUM_SCALAR >* mn) :
      MarginalTargetedMNInference< GUM_SCALAR >(mn) {
    // assign a MN if this has not been done before (due to virtual inheritance)
    if (this->mn__ == nullptr) {
      MarkovNetInference< GUM_SCALAR >::setMarkovNetDuringConstruction__(mn);
    }
    GUM_CONSTRUCTOR(JointTargetedMNInference);
  }


  // Destructor
  template < typename GUM_SCALAR >
  JointTargetedMNInference< GUM_SCALAR >::~JointTargetedMNInference() {
    GUM_DESTRUCTOR(JointTargetedMNInference);
  }


  // assigns a new MN to the inference engine
  template < typename GUM_SCALAR >
  void JointTargetedMNInference< GUM_SCALAR >::onMarkovNetChanged_(
     const IMarkovNet< GUM_SCALAR >* mn) {
    MarginalTargetedMNInference< GUM_SCALAR >::onMarkovNetChanged_(mn);
    onAllJointTargetsErased_();
    joint_targets__.clear();
  }


  // ##############################################################################
  // Targets
  // ##############################################################################

  // return true if target is a nodeset target.
  template < typename GUM_SCALAR >
  INLINE bool JointTargetedMNInference< GUM_SCALAR >::isJointTarget(
     const NodeSet& vars) const {
    if (this->mn__ == nullptr)
      GUM_ERROR(NullElement,
                "No Markov net has been assigned to the "
                "inference algorithm");

    const auto& gra = this->mn__->graph();
    for (const auto var: vars) {
      if (!gra.exists(var)) {
        GUM_ERROR(UndefinedElement,
                  var << " is not a NodeId in the Markov network");
      }
    }

    return joint_targets__.contains(vars);
  }


  // Clear all previously defined single targets
  template < typename GUM_SCALAR >
  INLINE void JointTargetedMNInference< GUM_SCALAR >::eraseAllMarginalTargets() {
    MarginalTargetedMNInference< GUM_SCALAR >::eraseAllTargets();
  }


  // Clear all previously defined targets (single targets and sets of targets)
  template < typename GUM_SCALAR >
  INLINE void JointTargetedMNInference< GUM_SCALAR >::eraseAllJointTargets() {
    if (joint_targets__.size() > 0) {
      // we already are in target mode. So no this->setTargetedMode_();  is needed
      onAllJointTargetsErased_();
      joint_targets__.clear();
      this->setState__(MarkovNetInference<
                       GUM_SCALAR >::StateOfMNInference::OutdatedMNStructure);
    }
  }


  // Clear all previously defined targets (single and joint targets)
  template < typename GUM_SCALAR >
  INLINE void JointTargetedMNInference< GUM_SCALAR >::eraseAllTargets() {
    eraseAllMarginalTargets();
    eraseAllJointTargets();
  }


  // Add a set of nodes as a new target
  template < typename GUM_SCALAR >
  void JointTargetedMNInference< GUM_SCALAR >::addJointTarget(
     const NodeSet& joint_target) {
    // check if the nodes in the target belong to the Markov network
    if (this->mn__ == nullptr)
      GUM_ERROR(NullElement,
                "No Markov net has been assigned to the "
                "inference algorithm");

    const auto& dag = this->mn__->graph();
    for (const auto node: joint_target) {
      if (!dag.exists(node)) {
        GUM_ERROR(UndefinedElement,
                  "at least one one in " << joint_target
                                         << " does not belong to the mn");
      }
    }

    // check that the joint_target set does not contain the new target
    if (joint_targets__.contains(joint_target)) return;

    // check if joint_target is a subset of an already existing target
    for (const auto& target: joint_targets__) {
      if (target.isSupersetOf(joint_target)) return;
    }

    // check if joint_target is not a superset of an already existing target
    // in this case, we need to remove old existing target
    for (auto iter = joint_targets__.beginSafe();
         iter != joint_targets__.endSafe();
         ++iter) {
      if (iter->isSubsetOf(joint_target)) eraseJointTarget(*iter);
    }

    this->setTargetedMode_();   // does nothing if already in targeted mode
    joint_targets__.insert(joint_target);
    onJointTargetAdded_(joint_target);
    this->setState__(
       MarkovNetInference< GUM_SCALAR >::StateOfMNInference::OutdatedMNStructure);
  }


  // removes an existing set target
  template < typename GUM_SCALAR >
  void JointTargetedMNInference< GUM_SCALAR >::eraseJointTarget(
     const NodeSet& joint_target) {
    // check if the nodes in the target belong to the Markov network
    if (this->mn__ == nullptr)
      GUM_ERROR(NullElement,
                "No Markov net has been assigned to the "
                "inference algorithm");

    const auto& dag = this->mn__->graph();
    for (const auto node: joint_target) {
      if (!dag.exists(node)) {
        GUM_ERROR(UndefinedElement,
                  "at least one one in " << joint_target
                                         << " does not belong to the mn");
      }
    }

    // check that the joint_target set does not contain the new target
    if (joint_targets__.contains(joint_target)) {
      // note that we have to be in target mode when we are here
      // so, no this->setTargetedMode_();  is necessary
      onJointTargetErased_(joint_target);
      joint_targets__.erase(joint_target);
      this->setState__(MarkovNetInference<
                       GUM_SCALAR >::StateOfMNInference::OutdatedMNStructure);
    }
  }


  /// returns the list of target sets
  template < typename GUM_SCALAR >
  INLINE const Set< NodeSet >&
               JointTargetedMNInference< GUM_SCALAR >::jointTargets() const noexcept {
    return joint_targets__;
  }

  /// returns the number of target sets
  template < typename GUM_SCALAR >
  INLINE Size
     JointTargetedMNInference< GUM_SCALAR >::nbrJointTargets() const noexcept {
    return joint_targets__.size();
  }


  // ##############################################################################
  // Inference
  // ##############################################################################

  // Compute the posterior of a nodeset.
  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
     JointTargetedMNInference< GUM_SCALAR >::jointPosterior(const NodeSet& nodes) {
    // try to get the smallest set of targets that contains "nodes"
    NodeSet set;
    bool    found_exact_target = false;

    if (joint_targets__.contains(nodes)) {
      set = nodes;
      found_exact_target = true;
    } else {
      for (const auto& target: joint_targets__) {
        if (nodes.isSubsetOf(target)) {
          set = target;
          break;
        }
      }
    }

    if (set.empty()) {
      GUM_ERROR(UndefinedElement,
                " no joint target containing " << nodes << " could be found among "
                                               << joint_targets__);
    }

    if (!this->isDone()) { this->makeInference(); }

    if (found_exact_target)
      return jointPosterior_(nodes);
    else
      return jointPosterior_(nodes, set);
  }


  // Compute the posterior of a node
  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
     JointTargetedMNInference< GUM_SCALAR >::posterior(NodeId node) {
    if (this->isTarget(node))
      return MarginalTargetedMNInference< GUM_SCALAR >::posterior(node);
    else
      return jointPosterior(NodeSet{node});
  }

  // Compute the posterior of a node
  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >& JointTargetedMNInference< GUM_SCALAR >::posterior(
     const std::string& nodeName) {
    return posterior(this->MN().idFromName(nodeName));
  }

  // ##############################################################################
  // Entropy
  // ##############################################################################
  template < typename GUM_SCALAR >
  GUM_SCALAR JointTargetedMNInference< GUM_SCALAR >::I(const std::string& Xname, const std::string& Yname) {
    return I(this->MN().idFromName(Xname),this->MN().idFromName(Yname));
  }

  template < typename GUM_SCALAR >
  GUM_SCALAR JointTargetedMNInference< GUM_SCALAR >::VI(const std::string& Xname, const std::string& Yname) {
    return VI(this->MN().idFromName(Xname),this->MN().idFromName(Yname));
  }


  /* Mutual information between X and Y
   * @see http://en.wikipedia.org/wiki/Mutual_information
   *
   * @warning Due to limitation of @joint, may not be able to compute this value
   * @throw OperationNotAllowed in these cases
   */
  template < typename GUM_SCALAR >
  GUM_SCALAR JointTargetedMNInference< GUM_SCALAR >::I(NodeId X, NodeId Y) {
    Potential< GUM_SCALAR > pX, pY, *pXY = nullptr;
    if (X == Y) {
      GUM_ERROR(OperationNotAllowed, "Mutual Information I(X,Y) with X==Y");
    }

    try {
      // here use unnormalized joint posterior rather than just posterior
      // to avoid saving the posterior in the cache of the inference engines
      // like LazyPropagation or Shafer-Shenoy.
      pXY = this->unnormalizedJointPosterior_({X, Y});
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
  INLINE GUM_SCALAR JointTargetedMNInference< GUM_SCALAR >::VI(NodeId X,
                                                               NodeId Y) {
    return this->H(X) + this->H(Y) - 2 * I(X, Y);
  }


  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >
     JointTargetedMNInference< GUM_SCALAR >::evidenceJointImpact(
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
     JointTargetedMNInference< GUM_SCALAR >::evidenceJointImpact(
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
  GUM_SCALAR JointTargetedMNInference< GUM_SCALAR >::jointMutualInformation(
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
  GUM_SCALAR JointTargetedMNInference< GUM_SCALAR >::jointMutualInformation(
     const std::vector< std::string >& targets) {
    const auto& mn = this->MN();

    gum::NodeSet targetsId;
    for (const auto& targetname: targets) {
      targetsId.insert(mn.idFromName(targetname));
    }

    return jointMutualInformation(targetsId);
  }

} /* namespace gum */
