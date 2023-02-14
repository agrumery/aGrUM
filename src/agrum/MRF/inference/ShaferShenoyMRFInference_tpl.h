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
 * @brief Implementation of Shafer-Shenoy's propagation for inference in
 * Markov random fields.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#  include <algorithm>
#  include <agrum/MRF/inference/ShaferShenoyMRFInference.h>

#  include <agrum/tools/graphs/algorithms/binaryJoinTreeConverterDefault.h>
#  include <agrum/tools/multidim/instantiation.h>
#  include <agrum/tools/multidim/utils/operators/multiDimCombineAndProjectDefault.h>
#  include <agrum/tools/multidim/utils/operators/multiDimProjection.h>


namespace gum {
  // default constructor
  template < typename GUM_SCALAR >
  INLINE ShaferShenoyMRFInference< GUM_SCALAR >::ShaferShenoyMRFInference(
     const IMarkovRandomField< GUM_SCALAR >* MN,
     bool                                    use_binary_join_tree) :
      JointTargetedMRFInference< GUM_SCALAR >(MN),
      EvidenceMRFInference< GUM_SCALAR >(MN), _use_binary_join_tree_(use_binary_join_tree) {
    // create a default triangulation (the user can change it afterwards)
    _triangulation_ = new DefaultTriangulation;

    // for each node in the MRF, assign the set of factors that contain it
    const auto& graph = this->MRF().graph();
    _node_to_factors_.resize(graph.size());
    _PotentialSet_ empty;
    for (const auto node: graph)
      _node_to_factors_.insert(node, empty);
    for (const auto& factor: this->MRF().factors()) {
      for (const auto node: factor.first) {
        _node_to_factors_[node].insert(factor.second);
      }
    }

    // for debugging purposes
    GUM_CONSTRUCTOR(ShaferShenoyMRFInference);
  }


  // destructor
  template < typename GUM_SCALAR >
  INLINE ShaferShenoyMRFInference< GUM_SCALAR >::~ShaferShenoyMRFInference() {
    // remove all the potentials created during the last message passing
    for (const auto& pot: _arc_to_created_potentials_)
      delete pot.second;

    // remove all the potentials in _clique_ss_potential_ that do not belong
    // to _clique_potentials_: in this case, those potentials have been
    // created by combination of the corresponding list of potentials in
    // _clique_potentials_. In other words, the size of this list is strictly
    // greater than 1.
    for (auto pot: _clique_ss_potential_) {
      if (_clique_potentials_[pot.first].size() > 1) delete pot.second;
    }

    for (auto potset: _clique_potentials_) {
      for (auto pot: potset.second)
        delete pot;
    }

    // remove all the posteriors computed
    for (const auto& pot: _target_posteriors_)
      delete pot.second;
    for (const auto& pot: _joint_target_posteriors_)
      delete pot.second;

    // remove the junction tree and the triangulation algorithm
    if (_JT_ != nullptr) delete _JT_;
    if (_junctionTree_ != nullptr) delete _junctionTree_;
    delete _triangulation_;

    // for debugging purposes
    GUM_DESTRUCTOR(ShaferShenoyMRFInference);
  }


  /// set a new triangulation algorithm
  template < typename GUM_SCALAR >
  void ShaferShenoyMRFInference< GUM_SCALAR >::setTriangulation(
     const Triangulation& new_triangulation) {
    delete _triangulation_;
    _triangulation_    = new_triangulation.newFactory();
    _is_new_jt_needed_ = true;
    this->setOutdatedStructureState_();
  }


  /// returns the current join tree used
  template < typename GUM_SCALAR >
  INLINE const JoinTree* ShaferShenoyMRFInference< GUM_SCALAR >::joinTree() {
    if (_is_new_jt_needed_) _createNewJT_();

    return _JT_;
  }

  /// returns the current junction tree
  template < typename GUM_SCALAR >
  INLINE const JunctionTree* ShaferShenoyMRFInference< GUM_SCALAR >::junctionTree() {
    if (_is_new_jt_needed_) _createNewJT_();

    return _junctionTree_;
  }


  /// sets the operator for performing the projections
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyMRFInference< GUM_SCALAR >::_setProjectionFunction_(
     Potential< GUM_SCALAR > (*proj)(const Potential< GUM_SCALAR >&,
                                     const Set< const DiscreteVariable* >&)) {
    _projection_op_ = proj;

    // indicate that all messages need be reconstructed to take into account
    // the change in of the projection operator
    _invalidateAllMessages_();
  }


  /// sets the operator for performing the combinations
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyMRFInference< GUM_SCALAR >::_setCombinationFunction_(
     Potential< GUM_SCALAR > (*comb)(const Potential< GUM_SCALAR >&,
                                     const Potential< GUM_SCALAR >&)) {
    _combination_op_ = comb;

    // indicate that all messages need be reconstructed to take into account
    // the change of the combination operator
    _invalidateAllMessages_();
  }


  /// invalidate all messages, posteriors and created potentials
  template < typename GUM_SCALAR >
  void ShaferShenoyMRFInference< GUM_SCALAR >::_invalidateAllMessages_() {
    // remove all the messages computed
    for (auto& pot: _separator_potentials_)
      pot.second = nullptr;

    for (auto& mess_computed: _messages_computed_)
      mess_computed.second = false;

    // remove all the created potentials kept on the arcs
    for (const auto& pot: _arc_to_created_potentials_)
      if (pot.second != nullptr) delete pot.second;
    _arc_to_created_potentials_.clear();

    // remove all the posteriors
    for (const auto& pot: _target_posteriors_)
      delete pot.second;
    _target_posteriors_.clear();
    for (const auto& pot: _joint_target_posteriors_)
      delete pot.second;
    _joint_target_posteriors_.clear();

    // indicate that new messages need be computed
    if (this->isInferenceReady() || this->isInferenceDone()) this->setOutdatedPotentialsState_();
  }


  /// fired when a new evidence is inserted
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyMRFInference< GUM_SCALAR >::onEvidenceAdded_(const NodeId id,
                                                                       bool isHardEvidence) {
    // if we have a new hard evidence, this modifies the undigraph over which
    // the join tree is created. This is also the case if id is not a node of
    // of the undigraph
    if (isHardEvidence || !_graph_.exists(id)) _is_new_jt_needed_ = true;
    else {
      try {
        _evidence_changes_.insert(id, EvidenceChangeType::EVIDENCE_ADDED);
      } catch (DuplicateElement const&) {
        // here, the evidence change already existed. This necessarily means
        // that the current saved change is an EVIDENCE_ERASED. So if we
        // erased the evidence and added some again, this corresponds to an
        // EVIDENCE_MODIFIED
        _evidence_changes_[id] = EvidenceChangeType::EVIDENCE_MODIFIED;
      }
    }
  }


  /// fired when an evidence is removed
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyMRFInference< GUM_SCALAR >::onEvidenceErased_(const NodeId id,
                                                                        bool isHardEvidence) {
    // if we delete a hard evidence, this modifies the undigraph over which
    // the join tree is created.
    if (isHardEvidence) _is_new_jt_needed_ = true;
    else {
      try {
        _evidence_changes_.insert(id, EvidenceChangeType::EVIDENCE_ERASED);
      } catch (DuplicateElement const&) {
        // here, the evidence change already existed and it is necessarily an
        // EVIDENCE_ADDED or an EVIDENCE_MODIFIED. So, if the evidence has
        // been added and is now erased, this is similar to not having created
        // it. If the evidence was only modified, it already existed in the
        // last inference and we should now indicate that it has been removed.
        if (_evidence_changes_[id] == EvidenceChangeType::EVIDENCE_ADDED)
          _evidence_changes_.erase(id);
        else _evidence_changes_[id] = EvidenceChangeType::EVIDENCE_ERASED;
      }
    }
  }


  /// fired when all the evidence are erased
  template < typename GUM_SCALAR >
  void ShaferShenoyMRFInference< GUM_SCALAR >::onAllEvidenceErased_(bool has_hard_evidence) {
    if (has_hard_evidence || !this->hardEvidenceNodes().empty()) _is_new_jt_needed_ = true;
    else {
      for (const auto node: this->softEvidenceNodes()) {
        try {
          _evidence_changes_.insert(node, EvidenceChangeType::EVIDENCE_ERASED);
        } catch (DuplicateElement const&) {
          // here, the evidence change already existed and it is necessarily an
          // EVIDENCE_ADDED or an EVIDENCE_MODIFIED. So, if the evidence has
          // been added and is now erased, this is similar to not having created
          // it. If the evidence was only modified, it already existed in the
          // last inference and we should now indicate that it has been removed.
          if (_evidence_changes_[node] == EvidenceChangeType::EVIDENCE_ADDED)
            _evidence_changes_.erase(node);
          else _evidence_changes_[node] = EvidenceChangeType::EVIDENCE_ERASED;
        }
      }
    }
  }


  /// fired when an evidence is changed
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyMRFInference< GUM_SCALAR >::onEvidenceChanged_(const NodeId id,
                                                                         bool hasChangedSoftHard) {
    if (hasChangedSoftHard) _is_new_jt_needed_ = true;
    else {
      try {
        _evidence_changes_.insert(id, EvidenceChangeType::EVIDENCE_MODIFIED);
      } catch (DuplicateElement const&) {
        // here, the evidence change already existed and it is necessarily an
        // EVIDENCE_ADDED. So we should keep this state to indicate that this
        // evidence is new w.r.t. the last inference
      }
    }
  }


  /// fired after a new target is inserted
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyMRFInference< GUM_SCALAR >::onMarginalTargetAdded_(const NodeId id) {}


  /// fired before a target is removed
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyMRFInference< GUM_SCALAR >::onMarginalTargetErased_(const NodeId id) {}


  /// fired after a new set target is inserted
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyMRFInference< GUM_SCALAR >::onJointTargetAdded_(const NodeSet& set) {}


  /// fired before a set target is removed
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyMRFInference< GUM_SCALAR >::onJointTargetErased_(const NodeSet& set) {}


  /// fired after all the nodes of the MRF are added as single targets
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyMRFInference< GUM_SCALAR >::onAllMarginalTargetsAdded_() {}


  /// fired before a all the single_targets are removed
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyMRFInference< GUM_SCALAR >::onAllMarginalTargetsErased_() {}

  /// fired after a new Markov net has been assigned to the engine
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyMRFInference< GUM_SCALAR >::onMRFChanged_(
     const IMarkovRandomField< GUM_SCALAR >* mn) {}

  /// fired before a all the joint_targets are removed
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyMRFInference< GUM_SCALAR >::onAllJointTargetsErased_() {}


  /// fired before a all the single and joint_targets are removed
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyMRFInference< GUM_SCALAR >::onAllTargetsErased_() {}


  // check whether a new junction tree is really needed for the next inference
  template < typename GUM_SCALAR >
  bool ShaferShenoyMRFInference< GUM_SCALAR >::_isNewJTNeeded_() const {
    // if we do not have a JT or if _new_jt_needed_ is set to true, then
    // we know that we need to create a new join tree
    if ((_JT_ == nullptr) || _is_new_jt_needed_) return true;

    // if some targets do not belong to the join tree and, consequently, to the
    // undirected graph that was used to construct the join tree, then we need
    // to create a new JT: this may happen due to the nodes that received hard
    // evidence (which do not belong to the graph).
    const auto& hard_ev_nodes = this->hardEvidenceNodes();
    for (const auto node: this->targets()) {
      if (!_graph_.exists(node) && !hard_ev_nodes.exists(node)) return true;
    }

    // now, do the same for the joint targets
    const std::vector< NodeId >& JT_elim_order = _triangulation_->eliminationOrder();
    NodeProperty< int >          elim_order(Size(JT_elim_order.size()));
    for (std::size_t i = std::size_t(0), size = JT_elim_order.size(); i < size; ++i)
      elim_order.insert(JT_elim_order[i], (int)i);
    NodeSet unobserved_set;

    for (const auto& joint_target: this->jointTargets()) {
      // here, we need to check that at least one clique contains all the
      // nodes of the joint target.
      NodeId first_eliminated_node = std::numeric_limits< NodeId >::max();
      int    elim_number           = std::numeric_limits< int >::max();
      unobserved_set.clear();
      for (const auto node: joint_target) {
        if (!_graph_.exists(node)) {
          if (!hard_ev_nodes.exists(node)) return true;
        } else {
          unobserved_set.insert(node);
          if (elim_order[node] < elim_number) {
            elim_number           = elim_order[node];
            first_eliminated_node = node;
          }
        }
      }
      if (!unobserved_set.empty()) {
        // here, first_eliminated_node contains the first var (node or one of its
        // parents) eliminated => the clique created during its elimination
        // should contain all the nodes in unobserved_set
        const auto  clique_id = _node_to_clique_[first_eliminated_node];
        const auto& clique    = _JT_->clique(clique_id);
        for (const auto node: unobserved_set) {
          if (!clique.contains(node)) return true;
        }
      }
    }

    // if some new evidence have been added on nodes that do not belong
    // to _graph_, then we potentially have to reconstruct the join tree
    for (const auto& change: _evidence_changes_) {
      if ((change.second == EvidenceChangeType::EVIDENCE_ADDED) && !_graph_.exists(change.first))
        return true;
    }

    // here, the current JT is exactly what we need for the next inference
    return false;
  }


  /// create a new junction tree as well as its related data structures
  template < typename GUM_SCALAR >
  void ShaferShenoyMRFInference< GUM_SCALAR >::_createNewJT_() {
    // to create the JT, we first create the required subgraph of the MRF in the
    // following way, in order to take into account the nodes that received
    // evidence:
    // 1/ we copy the graph of the MRF
    // 2/ add edges so that joint targets form a clique of the graph
    // 3/ remove the nodes that received hard evidence
    //
    // At the end of step 3/, we have our required graph and we can triangulate
    // it to get the new junction tree

    // 1/ copy the undirected graph of the MRF
    const auto& mn = this->MRF();
    _graph_        = mn.graph();

    // 2/ if there exist some joint targets, we shall add new edges into the
    // undirected graph in order to ensure that there exists a clique containing
    // each joint target
    for (const auto& nodeset: this->jointTargets()) {
      for (auto iter1 = nodeset.cbegin(); iter1 != nodeset.cend(); ++iter1) {
        auto iter2 = iter1;
        for (++iter2; iter2 != nodeset.cend(); ++iter2) {
          _graph_.addEdge(*iter1, *iter2);
        }
      }
    }

    // 3/ remove all the nodes that received hard evidence
    _hard_ev_nodes_ = this->hardEvidenceNodes();
    for (const auto node: _hard_ev_nodes_) {
      _graph_.eraseNode(node);
    }


    // now, we can compute the new junction tree. To speed-up computations
    // (essentially, those of a distribution phase), we construct from this
    // junction tree a binary join tree
    if (_JT_ != nullptr) delete _JT_;
    if (_junctionTree_ != nullptr) delete _junctionTree_;

    const auto& domain_sizes = this->domainSizes();
    _triangulation_->setGraph(&_graph_, &domain_sizes);
    const JunctionTree& triang_jt = _triangulation_->junctionTree();
    if (_use_binary_join_tree_) {
      BinaryJoinTreeConverterDefault bjt_converter;
      NodeSet                        emptyset;
      _JT_ = new CliqueGraph(bjt_converter.convert(triang_jt, domain_sizes, emptyset));
    } else {
      _JT_ = new CliqueGraph(triang_jt);
    }
    _junctionTree_ = new CliqueGraph(triang_jt);


    // assign to each node the order in which it was eliminated by the triangulation
    const std::vector< NodeId >& JT_elim_order   = _triangulation_->eliminationOrder();
    Size                         size_elim_order = JT_elim_order.size();
    NodeProperty< int >          elim_order(size_elim_order);
    for (Idx i = Idx(0); i < size_elim_order; ++i)
      elim_order.insert(JT_elim_order[i], (int)i);

    // assign to each factor of the Markov random field a clique in _JT_
    // that can contain its conditional probability table
    _factor_to_clique_.clear();
    _factor_to_clique_.resize(mn.factors().size());
    for (const auto& factor: mn.factors()) {
      const auto& nodes                 = factor.first;   // factor.second is the Potential()
      NodeId      first_eliminated_node = std::numeric_limits< NodeId >::max();
      int         elim_number           = std::numeric_limits< int >::max();
      for (const auto node: nodes) {
        if (_graph_.exists(node) && (elim_order[node] < elim_number)) {
          elim_number           = elim_order[node];
          first_eliminated_node = node;
        }
      }

      if (elim_number != std::numeric_limits< int >::max()) {
        // first_eliminated_node contains the first var/node eliminated => the
        // clique created during its elimination must contain node and all of its
        // neighbors => it necessarily contains all the nodes of factor
        _factor_to_clique_.insert(
           factor.second,
           _triangulation_->createdJunctionTreeClique(first_eliminated_node));
      }
    }

    // assign to each node that did not receive some hard evidence the smallest
    // clique that contains it
    _node_to_clique_.clear();
    _node_to_clique_.resize(_graph_.size());
    NodeProperty< double > node_to_clique_size(_graph_.size());
    for (const auto node: _graph_) {
      _node_to_clique_.insert(node, std::numeric_limits< NodeId >::max());
      node_to_clique_size.insert(node, std::numeric_limits< double >::max());
    }
    double overall_size = 0;
    for (const auto clique_id: *_JT_) {
      // determine the size of clique_id
      const auto& clique_nodes = _JT_->clique(clique_id);
      double      clique_size  = 1.0;
      for (const auto node: clique_nodes)
        clique_size *= double(domain_sizes[node]);
      overall_size += clique_size;

      // assign the clique to the nodes if its size is smaller than that of the
      // current cliques assigned to them
      for (const auto node: clique_nodes) {
        if (clique_size < node_to_clique_size[node]) {
          _node_to_clique_[node]    = clique_id;
          node_to_clique_size[node] = clique_size;
        }
      }
    }

    // indicate for each joint_target a clique that contains it
    _joint_target_to_clique_.clear();
    for (const auto& set: this->jointTargets()) {
      NodeId first_eliminated_node = std::numeric_limits< NodeId >::max();
      int    elim_number           = std::numeric_limits< int >::max();

      // do not take into account the nodes that received hard evidence
      // (since they do not belong to the join tree)
      for (const auto node: set) {
        if (!_hard_ev_nodes_.contains(node)) {
          // the clique we are looking for is the one that was created when
          // the first element of nodeset was eliminated
          if (elim_order[node] < elim_number) {
            elim_number           = elim_order[node];
            first_eliminated_node = node;
          }
        }
      }

      if (elim_number != std::numeric_limits< int >::max()) {
        _joint_target_to_clique_.insert(
           set,
           _triangulation_->createdJunctionTreeClique(first_eliminated_node));
      }
    }

    // compute the roots of  _JT_'s connected components
    _computeJoinTreeRoots_();

    // remove all the potentials stored into the cliques. Note that these include
    // the factors resulting from the projections of hard evidence as well as the
    // CPTs of the soft evidence
    for (const auto& pot: _clique_ss_potential_) {
      if (_clique_potentials_[pot.first].size() > 1) delete pot.second;
    }
    _clique_ss_potential_.clear();
    for (const auto& potlist: _clique_potentials_)
      for (const auto pot: potlist.second)
        delete pot;
    _clique_potentials_.clear();

    // remove all the potentials created during the last inference
    for (const auto& pot: _arc_to_created_potentials_)
      delete pot.second;
    _arc_to_created_potentials_.clear();

    // remove all the potentials created to take into account hard evidence
    // during the last inference (they have already been deleted from memory
    // by the clearing of _clique_potentials_).
    _hard_ev_projected_factors_.clear();

    // remove all the soft evidence.
    _node_to_soft_evidence_.clear();

    // create empty potential lists into the cliques of the joint tree as well
    // as empty lists of evidence
    _ScheduleMultiDimSet_ empty_set;
    for (const auto node: *_JT_) {
      _clique_potentials_.insert(node, empty_set);
      _clique_ss_potential_.insert(node, nullptr);
    }

    // remove all the constants created due to projections of CPTs that were
    // defined over only hard evidence nodes
    _constants_.clear();

    // create empty messages and indicate that no message has been computed yet
    _separator_potentials_.clear();
    _messages_computed_.clear();
    for (const auto& edge: _JT_->edges()) {
      const Arc arc1(edge.first(), edge.second());
      _separator_potentials_.insert(arc1, nullptr);
      _messages_computed_.insert(arc1, false);
      const Arc arc2(edge.second(), edge.first());
      _separator_potentials_.insert(arc2, nullptr);
      _messages_computed_.insert(arc2, false);
    }

    // remove all the posteriors computed so far
    for (const auto& pot: _target_posteriors_)
      delete pot.second;
    _target_posteriors_.clear();
    for (const auto& pot: _joint_target_posteriors_)
      delete pot.second;
    _joint_target_posteriors_.clear();

    // here, we determine whether we should use schedules during the inference.
    // the rule is: if the sum of the domain sizes of the cliques is greater
    // than a threshold, use schedules
    _use_schedules_ = (overall_size > _schedule_threshold_);

    // we shall now add all the potentials of the soft evidence to the cliques
    const NodeProperty< const Potential< GUM_SCALAR >* >& evidence = this->evidence();
    for (const auto node: this->softEvidenceNodes()) {
      auto ev_pot = new ScheduleMultiDim< Potential< GUM_SCALAR > >(*evidence[node], false);
      _node_to_soft_evidence_.insert(node, ev_pot);
      _clique_potentials_[_node_to_clique_[node]].insert(ev_pot);
    }

    // put all the factors of the MRF into the cliques
    // here, beware: all the potentials that are defined over some nodes
    // including hard evidence must be projected so that these nodes are
    // removed from the potential
    if (_use_schedules_) {
      Schedule schedule;
      _initializeJTCliques_(schedule);
    } else {
      _initializeJTCliques_();
    }


    // indicate that the data structures are up to date.
    _evidence_changes_.clear();
    _is_new_jt_needed_ = false;
  }


  /// put all the CPTs into the cliques when creating the JT without using a schedule
  template < typename GUM_SCALAR >
  void ShaferShenoyMRFInference< GUM_SCALAR >::_initializeJTCliques_() {
    const auto& mn = this->MRF();

    // put all the factors of the MRF into the cliques
    // here, beware: all the potentials that are defined over some nodes
    // including hard evidence must be projected so that these nodes are
    // removed from the potential
    const NodeProperty< const Potential< GUM_SCALAR >* >& evidence      = this->evidence();
    const NodeProperty< Idx >&                            hard_evidence = this->hardEvidence();

    for (const auto& factor: mn.factors()) {
      const auto& factor_nodes = factor.first;
      const auto& pot          = *(factor.second);
      const auto& variables    = pot.variablesSequence();

      // get the list of nodes with hard evidence in the factor
      NodeSet hard_nodes(factor_nodes.size());
      bool    graph_contains_nodes = false;
      for (const auto node: factor_nodes) {
        if (_hard_ev_nodes_.contains(node)) hard_nodes.insert(node);
        else if (_graph_.exists(node)) graph_contains_nodes = true;
      }

      // if hard_nodes contains hard evidence nodes, perform a projection
      // and insert the result into the appropriate clique, else insert
      // directly pot into the clique
      if (hard_nodes.empty()) {
        auto sched_cpt = new ScheduleMultiDim< Potential< GUM_SCALAR > >(pot, false);
        _clique_potentials_[_factor_to_clique_[&pot]].insert(sched_cpt);
      } else {
        // marginalize out the hard evidence nodes: if factor_nodes is defined
        // only over nodes that received hard evidence, do not consider it
        // as a potential anymore but as a constant
        // TODO substitute constants by 0-dimensional potentials
        if (hard_nodes.size() == factor_nodes.size()) {
          Instantiation inst(pot);
          for (Size i = 0; i < hard_nodes.size(); ++i) {
            inst.chgVal(variables[i], hard_evidence[mn.nodeId(*(variables[i]))]);
          }
          _constants_.insert(&pot, pot.get(inst));
        } else {
          // here, we have a factor defined over some nodes that received hard
          // evidence and other nodes that did not receive it. If none of the
          // latter belong to the graph, then the factor is useless for inference
          if (!graph_contains_nodes) continue;

          // prepare the projection with a combine and project instance
          Set< const DiscreteVariable* > hard_variables;
          _PotentialSet_                 marg_factor_set(1 + hard_nodes.size());
          marg_factor_set.insert(&pot);
          for (const auto node: hard_nodes) {
            marg_factor_set.insert(evidence[node]);
            hard_variables.insert(&(mn.variable(node)));
          }

          // perform the combination of those potentials and their projection
          MultiDimCombineAndProjectDefault< Potential< GUM_SCALAR > > combine_and_project(
             _combination_op_,
             _projection_op_);

          _PotentialSet_ new_factor_list
             = combine_and_project.execute(marg_factor_set, hard_variables);

          // there should be only one potential in new_factor_list
          if (new_factor_list.size() != 1) {
            for (const auto pot: new_factor_list) {
              if (!marg_factor_set.contains(pot)) delete pot;
            }
            GUM_ERROR(FatalError,
                      "the projection of a potential containing "
                         << "hard evidence is empty!");
          }
          auto new_factor = const_cast< Potential< GUM_SCALAR >* >(*(new_factor_list.begin()));
          auto projected_factor
             = new ScheduleMultiDim< Potential< GUM_SCALAR > >(std::move(*new_factor));
          delete new_factor;

          _clique_potentials_[_factor_to_clique_[&pot]].insert(projected_factor);
          _hard_ev_projected_factors_.insert(&pot, projected_factor);
        }
      }
    }

    // now, in _clique_potentials_, for each clique, we have the list of
    // potentials that must be combined in order to produce the Shafer-Shenoy's
    // potential stored into the clique. So, perform this combination and
    // store the result in _clique_ss_potential_
    MultiDimCombinationDefault< Potential< GUM_SCALAR > > fast_combination(_combination_op_);
    for (const auto& xpotset: _clique_potentials_) {
      const auto& potset = xpotset.second;
      if (potset.size() > 0) {
        // here, there will be an entry in _clique_ss_potential_
        // If there is only one element in potset, this element shall be
        // stored into _clique_ss_potential_, else all the elements of potset
        // shall be combined and their result shall be stored
        if (potset.size() == 1) {
          _clique_ss_potential_[xpotset.first] = *(potset.cbegin());
        } else {
          _PotentialSet_ p_potset(potset.size());
          for (const auto pot: potset)
            p_potset.insert(&(
               static_cast< const ScheduleMultiDim< Potential< GUM_SCALAR > >* >(pot)->multiDim()));

          Potential< GUM_SCALAR >* joint
             = const_cast< Potential< GUM_SCALAR >* >(fast_combination.execute(p_potset));
          _clique_ss_potential_[xpotset.first]
             = new ScheduleMultiDim< Potential< GUM_SCALAR > >(std::move(*joint));
          delete joint;
        }
      }
    }
  }


  /// put all the CPTs into the cliques when creating the JT using a schedule
  template < typename GUM_SCALAR >
  void ShaferShenoyMRFInference< GUM_SCALAR >::_initializeJTCliques_(Schedule& schedule) {
    const auto& mn = this->MRF();

    // put all the factors of the MRF into the cliques
    // here, beware: all the potentials that are defined over some nodes
    // including hard evidence must be projected so that these nodes are
    // removed from the potential
    const NodeProperty< const Potential< GUM_SCALAR >* >& evidence      = this->evidence();
    const NodeProperty< Idx >&                            hard_evidence = this->hardEvidence();

    for (const auto& factor: mn.factors()) {
      const auto& factor_nodes = factor.first;
      const auto& pot          = *(factor.second);
      const auto& variables    = pot.variablesSequence();

      // get the list of nodes with hard evidence in the factor
      NodeSet hard_nodes;
      bool    graph_contains_nodes = false;
      for (const auto node: factor_nodes) {
        if (_hard_ev_nodes_.contains(node)) hard_nodes.insert(node);
        else if (_graph_.exists(node)) graph_contains_nodes = true;
      }

      // if hard_nodes contains hard evidence nodes, perform a projection
      // and insert the result into the appropriate clique, else insert
      // directly pot into the clique
      if (hard_nodes.empty()) {
        auto sched_cpt = new ScheduleMultiDim< Potential< GUM_SCALAR > >(pot, false);
        _clique_potentials_[_factor_to_clique_[&pot]].insert(sched_cpt);
      } else {
        // marginalize out the hard evidence nodes: if factor_nodes is defined
        // only over nodes that received hard evidence, do not consider it
        // as a potential anymore but as a constant
        // TODO substitute constants by 0-dimensional potentials
        if (hard_nodes.size() == factor_nodes.size()) {
          Instantiation inst(pot);
          for (Size i = 0; i < hard_nodes.size(); ++i) {
            inst.chgVal(variables[i], hard_evidence[mn.nodeId(*(variables[i]))]);
          }
          _constants_.insert(&pot, pot.get(inst));
        } else {
          // here, we have a factor defined over some nodes that received hard
          // evidence and other nodes that did not receive it. If none of the
          // latter belong to the graph, then the factor is useless for inference
          if (!graph_contains_nodes) continue;

          // prepare the projection with a combine and project instance
          Set< const DiscreteVariable* > hard_variables;
          _ScheduleMultiDimSet_          marg_factor_set(1 + hard_nodes.size());
          const IScheduleMultiDim*       sched_pot
             = schedule.insertTable< Potential< GUM_SCALAR > >(pot, false);
          marg_factor_set.insert(sched_pot);

          for (const auto node: hard_nodes) {
            const IScheduleMultiDim* pot
               = schedule.insertTable< Potential< GUM_SCALAR > >(*evidence[node], false);
            marg_factor_set.insert(pot);
            hard_variables.insert(&(mn.variable(node)));
          }

          // perform the combination of those potentials and their projection
          MultiDimCombineAndProjectDefault< Potential< GUM_SCALAR > > combine_and_project(
             _combination_op_,
             _projection_op_);

          _ScheduleMultiDimSet_ new_factor_list
             = combine_and_project.schedule(schedule, marg_factor_set, hard_variables);

          // there should be only one potential in new_factor_list
          if (new_factor_list.size() != 1) {
            GUM_ERROR(FatalError,
                      "the projection of a potential containing "
                         << "hard evidence is empty!");
          }
          auto projected_factor = const_cast< ScheduleMultiDim< Potential< GUM_SCALAR > >* >(
             static_cast< const ScheduleMultiDim< Potential< GUM_SCALAR > >* >(
                *new_factor_list.begin()));
          const_cast< ScheduleOperator* >(schedule.scheduleMultiDimCreator(projected_factor))
             ->makeResultsPersistent(true);

          _clique_potentials_[_factor_to_clique_[&pot]].insert(projected_factor);
          _hard_ev_projected_factors_.insert(&pot, projected_factor);
        }
      }
    }
    this->scheduler().execute(schedule);

    // now, in _clique_potentials_, for each clique, we have the list of
    // potentials that must be combined in order to produce the Shafer-Shenoy's
    // potential stored into the clique. So, perform this combination and
    // store the result in _clique_ss_potential_
    schedule.clear();
    MultiDimCombinationDefault< Potential< GUM_SCALAR > > fast_combination(_combination_op_);
    for (const auto& xpotset: _clique_potentials_) {
      const auto& potset = xpotset.second;
      if (potset.size() > 0) {
        // here, there will be an entry in _clique_ss_potential_
        // If there is only one element in potset, this element shall be
        // stored into _clique_ss_potential_, else all the elements of potset
        // shall be combined and their result shall be stored
        if (potset.size() == 1) {
          _clique_ss_potential_[xpotset.first] = *(potset.cbegin());
        } else {
          // add the tables to combine into the schedule
          for (const auto pot: potset) {
            schedule.emplaceScheduleMultiDim(*pot);
          }

          auto joint = const_cast< ScheduleMultiDim< Potential< GUM_SCALAR > >* >(
             static_cast< const ScheduleMultiDim< Potential< GUM_SCALAR > >* >(
                fast_combination.schedule(schedule, potset)));
          const_cast< ScheduleOperator* >(schedule.scheduleMultiDimCreator(joint))
             ->makeResultsPersistent(true);
          _clique_ss_potential_[xpotset.first] = joint;
        }
      }
    }
    this->scheduler().execute(schedule);
  }


  /// prepare the inference structures w.r.t. new targets, soft/hard evidence
  template < typename GUM_SCALAR >
  void ShaferShenoyMRFInference< GUM_SCALAR >::updateOutdatedStructure_() {
    // check if a new JT is really needed. If so, create it
    if (_isNewJTNeeded_()) {
      _createNewJT_();
    } else {
      // here, we can answer the next queries without reconstructing all the
      // junction tree. All we need to do is to indicate that we should
      // update the potentials and messages for these queries
      updateOutdatedPotentials_();
    }
  }


  /// invalidate all the messages sent from a given clique
  template < typename GUM_SCALAR >
  void ShaferShenoyMRFInference< GUM_SCALAR >::_diffuseMessageInvalidations_(
     NodeId   from_id,
     NodeId   to_id,
     NodeSet& invalidated_cliques) {
    // invalidate the current clique
    invalidated_cliques.insert(to_id);

    // invalidate the current arc
    const Arc arc(from_id, to_id);
    bool&     message_computed = _messages_computed_[arc];
    if (message_computed) {
      message_computed            = false;
      _separator_potentials_[arc] = nullptr;
      if (_arc_to_created_potentials_.exists(arc)) {
        delete _arc_to_created_potentials_[arc];
        _arc_to_created_potentials_.erase(arc);
      }

      // go on with the diffusion
      for (const auto node_id: _JT_->neighbours(to_id)) {
        if (node_id != from_id) _diffuseMessageInvalidations_(to_id, node_id, invalidated_cliques);
      }
    }
  }


  /// update the potentials stored in the cliques and invalidate outdated
  /// messages
  template < typename GUM_SCALAR >
  void ShaferShenoyMRFInference< GUM_SCALAR >::updateOutdatedPotentials_() {
    // for each clique, indicate whether the potential stored into
    // _clique_ss_potential_[clique] is the result of a combination. In this
    // case, it has been allocated by the combination and will need to be
    // deallocated if its clique has been invalidated
    NodeProperty< bool > ss_potential_to_deallocate(_clique_potentials_.size());
    for (const auto& potset: _clique_potentials_) {
      ss_potential_to_deallocate.insert(potset.first, (potset.second.size() > 1));
    }

    // compute the set of factors that were projected due to hard evidence and
    // whose hard evidence have changed, so that they need a new projection.
    // By the way, remove these factors since they are no more needed
    // Here only the values of the hard evidence can have changed (else a
    // fully new join tree would have been computed).
    // Note also that we know that the factors still contain some variable(s) after
    // the projection (else they should be constants)
    const auto&                           mn = this->MRF();
    NodeSet                               hard_nodes_changed(_hard_ev_nodes_.size());
    Set< const Potential< GUM_SCALAR >* > hard_projected_factors_changed(mn.factors().size());
    for (const auto node: _hard_ev_nodes_) {
      if (_evidence_changes_.exists(node)) {
        hard_nodes_changed.insert(node);
        for (const auto pot: _node_to_factors_[node]) {
          if (_hard_ev_projected_factors_.exists(pot)
              && !hard_projected_factors_changed.exists(pot)) {
            hard_projected_factors_changed.insert(pot);
          }
        }
      }
    }

    NodeSet hard_cliques_changed(hard_projected_factors_changed.size());
    for (const auto pot: hard_projected_factors_changed) {
      const auto   chgPot    = _hard_ev_projected_factors_[pot];
      const NodeId chgClique = _factor_to_clique_[pot];
      _clique_potentials_[chgClique].erase(chgPot);
      _hard_ev_projected_factors_.erase(pot);
      if (!hard_cliques_changed.contains(chgClique)) hard_cliques_changed.insert(chgClique);
      delete chgPot;
    }


    // invalidate all the messages that are no more correct: start from each of
    // the nodes whose soft evidence has changed and perform a diffusion from
    // the clique into which the soft evidence has been entered, indicating that
    // the messages spreading from this clique are now invalid. At the same time,
    // if there were potentials created on the arcs over which the messages were
    // sent, remove them from memory. For all the cliques that received some
    // projected factors that should now be changed, do the same.
    NodeSet invalidated_cliques(_JT_->size());
    for (const auto& pair: _evidence_changes_) {
      if (_node_to_clique_.exists(pair.first)) {
        const auto clique = _node_to_clique_[pair.first];
        invalidated_cliques.insert(clique);
        for (const auto neighbor: _JT_->neighbours(clique)) {
          _diffuseMessageInvalidations_(clique, neighbor, invalidated_cliques);
        }
      }
    }

    // now, add to the set of invalidated cliques those that contain projected
    // factors that were changed.
    for (const auto clique: hard_cliques_changed) {
      invalidated_cliques.insert(clique);
      for (const auto neighbor: _JT_->neighbours(clique)) {
        _diffuseMessageInvalidations_(clique, neighbor, invalidated_cliques);
      }
    }

    // now that we know the cliques whose set of potentials have been changed,
    // we can discard their corresponding Shafer-Shenoy potential
    for (const auto clique: invalidated_cliques) {
      if (ss_potential_to_deallocate[clique]) {
        delete _clique_ss_potential_[clique];
        _clique_ss_potential_[clique] = nullptr;
      }
    }


    // now we shall remove all the posteriors that belong to the
    // invalidated cliques.
    if (!_target_posteriors_.empty()) {
      for (auto iter = _target_posteriors_.beginSafe(); iter != _target_posteriors_.endSafe();
           ++iter) {
        // first, cope only with the nodes that did not receive hard evidence
        // since the other nodes do not belong to the join tree
        if (_graph_.exists(iter.key())
            && (invalidated_cliques.exists(_node_to_clique_[iter.key()]))) {
          delete iter.val();
          _target_posteriors_.erase(iter);
        }
        // now cope with the nodes that received hard evidence
        else if (hard_nodes_changed.contains(iter.key())) {
          delete iter.val();
          _target_posteriors_.erase(iter);
        }
      }
    }

    // finally, cope with joint targets. Notably, remove the joint posteriors whose
    // nodes have all received changed evidence
    for (auto iter = _joint_target_posteriors_.beginSafe();
         iter != _joint_target_posteriors_.endSafe();
         ++iter) {
      if (invalidated_cliques.exists(_joint_target_to_clique_[iter.key()])) {
        delete iter.val();
        _joint_target_posteriors_.erase(iter);
      } else {
        // check for sets in which all nodes have received evidence
        bool has_unevidenced_node = false;
        for (const auto node: iter.key()) {
          if (!hard_nodes_changed.exists(node)) {
            has_unevidenced_node = true;
            break;
          }
        }
        if (!has_unevidenced_node) {
          delete iter.val();
          _joint_target_posteriors_.erase(iter);
        }
      }
    }

    // remove all the evidence that were entered into _node_to_soft_evidence_
    // and _clique_ss_potential_ and add the new soft ones
    for (const auto& pot_pair: _node_to_soft_evidence_) {
      delete pot_pair.second;
      _clique_potentials_[_node_to_clique_[pot_pair.first]].erase(pot_pair.second);
    }
    _node_to_soft_evidence_.clear();

    const auto& evidence = this->evidence();
    for (const auto node: this->softEvidenceNodes()) {
      auto ev_pot = new ScheduleMultiDim< Potential< GUM_SCALAR > >(*evidence[node], false);
      _node_to_soft_evidence_.insert(node, ev_pot);
      _clique_potentials_[_node_to_clique_[node]].insert(ev_pot);
    }


    // Now add the projections of the factors due to newly changed hard evidence:
    // if we are performing updateOutdatedPotentials_, this means that the
    // set of nodes that received hard evidence has not changed, only
    // their instantiations can have changed. So, if there is an entry
    // for node in _constants_, there will still be such an entry after
    // performing the new projections. Idem for _hard_ev_projected_factors_
    if (_use_schedules_) {
      Schedule schedule;
      for (const auto pot: hard_projected_factors_changed) {
        _ScheduleMultiDimSet_ marg_pot_set;
        const auto sched_pot = schedule.insertTable< Potential< GUM_SCALAR > >(*pot, false);
        marg_pot_set.insert(sched_pot);
        const auto&                    variables = pot->variablesSequence();
        Set< const DiscreteVariable* > hard_variables(variables.size());
        for (const auto var: variables) {
          NodeId xnode = mn.nodeId(*var);
          if (_hard_ev_nodes_.exists(xnode)) {
            const auto ev_pot
               = schedule.insertTable< Potential< GUM_SCALAR > >(*evidence[xnode], false);
            marg_pot_set.insert(ev_pot);
            hard_variables.insert(var);
          }
        }

        // perform the combination of those potentials and their projection
        MultiDimCombineAndProjectDefault< Potential< GUM_SCALAR > > combine_and_project(
           _combination_op_,
           _projection_op_);

        _ScheduleMultiDimSet_ new_pot_list
           = combine_and_project.schedule(schedule, marg_pot_set, hard_variables);

        // there should be only one potential in new_cpt_list
        if (new_pot_list.size() != 1) {
          GUM_ERROR(FatalError,
                    "the projection of a potential containing "
                       << "hard evidence is empty!");
        }
        auto projected_pot = const_cast< ScheduleMultiDim< Potential< GUM_SCALAR > >* >(
           static_cast< const ScheduleMultiDim< Potential< GUM_SCALAR > >* >(
              *new_pot_list.begin()));
        const_cast< ScheduleOperator* >(schedule.scheduleMultiDimCreator(projected_pot))
           ->makeResultsPersistent(true);
        _clique_potentials_[_factor_to_clique_[pot]].insert(projected_pot);
        _hard_ev_projected_factors_.insert(pot, projected_pot);
      }

      // here, the list of potentials stored in the invalidated cliques have
      // been updated. So, now, we can combine them to produce the Shafer-Shenoy
      // potential stored into the clique
      MultiDimCombinationDefault< Potential< GUM_SCALAR > > fast_combination(_combination_op_);
      for (const auto clique: invalidated_cliques) {
        const auto& potset = _clique_potentials_[clique];

        if (potset.size() > 0) {
          // here, there will be an entry in _clique_ss_potential_
          // If there is only one element in potset, this element shall be
          // stored into _clique_ss_potential_, else all the elements of potset
          // shall be combined and their result shall be stored
          if (potset.size() == 1) {
            _clique_ss_potential_[clique] = *(potset.cbegin());
          } else {
            for (const auto pot: potset)
              if (!schedule.existsScheduleMultiDim(pot->id()))
                schedule.emplaceScheduleMultiDim(*pot);
            auto joint = const_cast< ScheduleMultiDim< Potential< GUM_SCALAR > >* >(
               static_cast< const ScheduleMultiDim< Potential< GUM_SCALAR > >* >(
                  fast_combination.schedule(schedule, potset)));
            const_cast< ScheduleOperator* >(schedule.scheduleMultiDimCreator(joint))
               ->makeResultsPersistent(true);
            _clique_ss_potential_[clique] = joint;
          }
        }
      }
      this->scheduler().execute(schedule);
    } else {
      for (const auto pot: hard_projected_factors_changed) {
        _PotentialSet_ marg_pot_set;
        marg_pot_set.insert(pot);
        const auto& variables = pot->variablesSequence();

        Set< const DiscreteVariable* > hard_variables(variables.size());
        for (const auto var: variables) {
          NodeId xnode = mn.nodeId(*var);
          if (_hard_ev_nodes_.exists(xnode)) {
            marg_pot_set.insert(evidence[xnode]);
            hard_variables.insert(var);
          }
        }

        // perform the combination of those potentials and their projection
        MultiDimCombineAndProjectDefault< Potential< GUM_SCALAR > > combine_and_project(
           _combination_op_,
           _projection_op_);

        _PotentialSet_ new_pot_list = combine_and_project.execute(marg_pot_set, hard_variables);

        // there should be only one potential in new_cpt_list
        if (new_pot_list.size() != 1) {
          GUM_ERROR(FatalError,
                    "the projection of a potential containing "
                       << "hard evidence is empty!");
        }
        Potential< GUM_SCALAR >* xprojected_pot
           = const_cast< Potential< GUM_SCALAR >* >(*new_pot_list.begin());
        auto projected_pot
           = new ScheduleMultiDim< Potential< GUM_SCALAR > >(std::move(*xprojected_pot));
        delete xprojected_pot;
        _clique_potentials_[_factor_to_clique_[pot]].insert(projected_pot);
        _hard_ev_projected_factors_.insert(pot, projected_pot);
      }

      // here, the list of potentials stored in the invalidated cliques have
      // been updated. So, now, we can combine them to produce the Shafer-Shenoy
      // potential stored into the clique
      MultiDimCombinationDefault< Potential< GUM_SCALAR > > fast_combination(_combination_op_);
      for (const auto clique: invalidated_cliques) {
        const auto& potset = _clique_potentials_[clique];

        if (potset.size() > 0) {
          // here, there will be an entry in _clique_ss_potential_
          // If there is only one element in potset, this element shall be
          // stored into _clique_ss_potential_, else all the elements of potset
          // shall be combined and their result shall be stored
          if (potset.size() == 1) {
            _clique_ss_potential_[clique] = *(potset.cbegin());
          } else {
            _PotentialSet_ p_potset(potset.size());
            for (const auto pot: potset)
              p_potset.insert(
                 &(static_cast< const ScheduleMultiDim< Potential< GUM_SCALAR > >* >(pot)
                      ->multiDim()));

            Potential< GUM_SCALAR >* joint
               = const_cast< Potential< GUM_SCALAR >* >(fast_combination.execute(p_potset));
            _clique_ss_potential_[clique]
               = new ScheduleMultiDim< Potential< GUM_SCALAR > >(std::move(*joint));
            delete joint;
          }
        }
      }
    }

    // update the constants
    const auto& hard_evidence = this->hardEvidence();
    for (auto& node_cst: _constants_) {
      const Potential< GUM_SCALAR >& pot = *(node_cst.first);
      Instantiation                  inst(pot);
      for (const auto var: pot.variablesSequence()) {
        inst.chgVal(var, hard_evidence[mn.nodeId(*var)]);
      }
      node_cst.second = pot.get(inst);
    }

    // indicate that all changes have been performed
    _evidence_changes_.clear();
  }


  /// compute a root for each connected component of  _JT_
  template < typename GUM_SCALAR >
  void ShaferShenoyMRFInference< GUM_SCALAR >::_computeJoinTreeRoots_() {
    // get the set of cliques in which we can find the targets and joint_targets.
    // Due to hard evidence, the cliques related to a given target node
    // might not exist, hence the try..catch.
    NodeSet clique_targets;
    for (const auto node: this->targets()) {
      try {
        clique_targets.insert(_node_to_clique_[node]);
      } catch (Exception const&) {}
    }
    for (const auto& set: this->jointTargets()) {
      try {
        clique_targets.insert(_joint_target_to_clique_[set]);
      } catch (Exception const&) {}
    }

    // put in a vector these cliques and their sizes
    std::vector< std::pair< NodeId, Size > > possible_roots(clique_targets.size());
    const auto&                              mn = this->MRF();
    std::size_t                              i  = 0;
    for (const auto clique_id: clique_targets) {
      const auto& clique   = _JT_->clique(clique_id);
      Size        dom_size = 1;
      for (const auto node: clique) {
        dom_size *= mn.variable(node).domainSize();
      }
      possible_roots[i] = std::pair< NodeId, Size >(clique_id, dom_size);
      ++i;
    }

    // sort the cliques by increasing domain size
    std::sort(possible_roots.begin(),
              possible_roots.end(),
              [](const std::pair< NodeId, Size >& a, const std::pair< NodeId, Size >& b) -> bool {
                return a.second < b.second;
              });

    // pick up the clique with the smallest size in each connected component
    NodeProperty< bool >                  marked = _JT_->nodesProperty(false);
    std::function< void(NodeId, NodeId) > diffuse_marks
       = [&marked, &diffuse_marks, this](NodeId node, NodeId from) {
           if (!marked[node]) {
             marked[node] = true;
             for (const auto neigh: _JT_->neighbours(node))
               if ((neigh != from) && !marked[neigh]) diffuse_marks(neigh, node);
           }
         };
    _roots_.clear();
    for (const auto& xclique: possible_roots) {
      NodeId clique = xclique.first;
      if (!marked[clique]) {
        _roots_.insert(clique);
        diffuse_marks(clique, clique);
      }
    }
  }


  // performs the collect phase of Shafer-Shenoy using schedules
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyMRFInference< GUM_SCALAR >::_collectMessage_(Schedule& schedule,
                                                                       NodeId    id,
                                                                       NodeId    from) {
    for (const auto other: _JT_->neighbours(id)) {
      if ((other != from) && !_messages_computed_[Arc(other, id)])
        _collectMessage_(schedule, other, id);
    }

    if ((id != from) && !_messages_computed_[Arc(id, from)]) {
      _produceMessage_(schedule, id, from);
    }
  }


  // performs the collect phase of Shafer-Shenoy without schedules
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyMRFInference< GUM_SCALAR >::_collectMessage_(NodeId id, NodeId from) {
    for (const auto other: _JT_->neighbours(id)) {
      if ((other != from) && !_messages_computed_[Arc(other, id)]) _collectMessage_(other, id);
    }

    if ((id != from) && !_messages_computed_[Arc(id, from)]) { _produceMessage_(id, from); }
  }


  // remove variables del_vars from the list of potentials pot_list
  template < typename GUM_SCALAR >
  const IScheduleMultiDim* ShaferShenoyMRFInference< GUM_SCALAR >::_marginalizeOut_(
     Schedule&                       schedule,
     Set< const IScheduleMultiDim* > pot_list,
     Set< const DiscreteVariable* >& del_vars,
     Set< const DiscreteVariable* >& kept_vars) {
    // let's guarantee that all the potentials to be combined and projected
    // belong to the schedule
    for (const auto pot: pot_list) {
      if (!schedule.existsScheduleMultiDim(pot->id())) schedule.emplaceScheduleMultiDim(*pot);
    }

    // create a combine and project operator that will perform the
    // marginalization
    MultiDimCombineAndProjectDefault< Potential< GUM_SCALAR > > combine_and_project(
       _combination_op_,
       _projection_op_);
    _ScheduleMultiDimSet_ new_pot_list = combine_and_project.schedule(schedule, pot_list, del_vars);

    // combine all the remaining potentials in order to create only one resulting potential
    if (new_pot_list.size() == 1) return *(new_pot_list.begin());
    MultiDimCombinationDefault< Potential< GUM_SCALAR > > fast_combination(_combination_op_);
    return fast_combination.schedule(schedule, new_pot_list);
  }


  // remove variables del_vars from the list of potentials pot_list
  template < typename GUM_SCALAR >
  const IScheduleMultiDim* ShaferShenoyMRFInference< GUM_SCALAR >::_marginalizeOut_(
     Set< const IScheduleMultiDim* >& pot_list,
     Set< const DiscreteVariable* >&  del_vars,
     Set< const DiscreteVariable* >&  kept_vars) {
    _PotentialSet_ xpot_list(pot_list.size());
    for (auto pot: pot_list)
      xpot_list.insert(
         &(static_cast< const ScheduleMultiDim< Potential< GUM_SCALAR > >* >(pot)->multiDim()));

    // create a combine and project operator that will perform the
    // marginalization
    MultiDimCombineAndProjectDefault< Potential< GUM_SCALAR > > combine_and_project(
       _combination_op_,
       _projection_op_);
    _PotentialSet_ xnew_pot_list = combine_and_project.execute(xpot_list, del_vars);

    // combine all the remaining potentials in order to create only one resulting potential
    const Potential< GUM_SCALAR >* xres_pot;
    if (xnew_pot_list.size() == 1) {
      xres_pot = *(xnew_pot_list.begin());
    } else {
      // combine all the potentials that resulted from the above combine and
      // projet execution
      MultiDimCombinationDefault< Potential< GUM_SCALAR > > fast_combination(_combination_op_);
      xres_pot = fast_combination.execute(xnew_pot_list);
      for (const auto pot: xnew_pot_list) {
        if (!xpot_list.contains(pot) && (pot != xres_pot)) delete pot;
      }
    }

    // transform xres_pot into a ScheduleMultiDim
    ScheduleMultiDim< Potential< GUM_SCALAR > >* res_pot;
    if (xpot_list.contains(xres_pot))
      res_pot = new ScheduleMultiDim< Potential< GUM_SCALAR > >(*xres_pot, false);
    else {
      res_pot = new ScheduleMultiDim< Potential< GUM_SCALAR > >(
         std::move(const_cast< Potential< GUM_SCALAR >& >(*xres_pot)));
      delete xres_pot;
    }

    return res_pot;
  }


  // creates the message sent by clique from_id to clique to_id
  template < typename GUM_SCALAR >
  void ShaferShenoyMRFInference< GUM_SCALAR >::_produceMessage_(Schedule& schedule,
                                                                NodeId    from_id,
                                                                NodeId    to_id) {
    // get the potentials of the clique.
    _ScheduleMultiDimSet_ pot_list;
    if (_clique_ss_potential_[from_id] != nullptr) pot_list.insert(_clique_ss_potential_[from_id]);

    // add the messages sent by adjacent nodes to from_id.
    for (const auto other_id: _JT_->neighbours(from_id)) {
      if (other_id != to_id) {
        const auto separator_pot = _separator_potentials_[Arc(other_id, from_id)];
        if (separator_pot != nullptr) pot_list.insert(separator_pot);
      }
    }

    // get the set of variables that need be removed from the potentials
    const NodeSet&                 from_clique = _JT_->clique(from_id);
    const NodeSet&                 separator   = _JT_->separator(from_id, to_id);
    Set< const DiscreteVariable* > del_vars(from_clique.size());
    Set< const DiscreteVariable* > kept_vars(separator.size());
    const auto&                    mn = this->MRF();

    for (const auto node: from_clique) {
      if (!separator.contains(node)) {
        del_vars.insert(&(mn.variable(node)));
      } else {
        kept_vars.insert(&(mn.variable(node)));
      }
    }

    // pot_list now contains all the potentials to multiply and marginalize
    // => combine the messages
    const IScheduleMultiDim* new_pot = _marginalizeOut_(schedule, pot_list, del_vars, kept_vars);

    // keep track of the newly created potential
    const Arc arc(from_id, to_id);
    if (!pot_list.exists(new_pot)) {
      if (!_arc_to_created_potentials_.exists(arc)) {
        _arc_to_created_potentials_.insert(arc, new_pot);

        // do not forget to make the ScheduleMultiDim persistent
        auto op = schedule.scheduleMultiDimCreator(new_pot);
        if (op != nullptr) const_cast< ScheduleOperator* >(op)->makeResultsPersistent(true);
      }
    }

    _separator_potentials_[arc] = new_pot;
    _messages_computed_[arc]    = true;
  }


  // creates the message sent by clique from_id to clique to_id
  template < typename GUM_SCALAR >
  void ShaferShenoyMRFInference< GUM_SCALAR >::_produceMessage_(NodeId from_id, NodeId to_id) {
    // get the potentials of the clique.
    _ScheduleMultiDimSet_ pot_list;
    if (_clique_ss_potential_[from_id] != nullptr) pot_list.insert(_clique_ss_potential_[from_id]);

    // add the messages sent by adjacent nodes to from_id.
    for (const auto other_id: _JT_->neighbours(from_id)) {
      if (other_id != to_id) {
        const auto separator_pot = _separator_potentials_[Arc(other_id, from_id)];
        if (separator_pot != nullptr) pot_list.insert(separator_pot);
      }
    }

    // get the set of variables that need be removed from the potentials
    const NodeSet&                 from_clique = _JT_->clique(from_id);
    const NodeSet&                 separator   = _JT_->separator(from_id, to_id);
    Set< const DiscreteVariable* > del_vars(from_clique.size());
    Set< const DiscreteVariable* > kept_vars(separator.size());
    const auto&                    mn = this->MRF();

    for (const auto node: from_clique) {
      if (!separator.contains(node)) {
        del_vars.insert(&(mn.variable(node)));
      } else {
        kept_vars.insert(&(mn.variable(node)));
      }
    }

    // pot_list now contains all the potentials to multiply and marginalize
    // => combine the messages
    const IScheduleMultiDim* new_pot = _marginalizeOut_(pot_list, del_vars, kept_vars);

    // keep track of the newly created potential
    const Arc arc(from_id, to_id);
    if (!pot_list.exists(new_pot)) {
      if (!_arc_to_created_potentials_.exists(arc)) {
        _arc_to_created_potentials_.insert(arc, new_pot);
      }
    }

    _separator_potentials_[arc] = new_pot;
    _messages_computed_[arc]    = true;
  }


  // fired after a new Markov net has been assigned to the inference engine
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyMRFInference< GUM_SCALAR >::onModelChanged_(const GraphicalModel* mn) {
    JointTargetedMRFInference< GUM_SCALAR >::onModelChanged_(mn);
  }


  // performs a whole inference
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyMRFInference< GUM_SCALAR >::makeInference_() {
    if (_use_schedules_) {
      Schedule schedule;

      // collect messages for all single targets
      for (const auto node: this->targets()) {
        // perform only collects in the join tree for nodes that have
        // not received hard evidence (those that received hard evidence were
        // not included into the join tree for speed-up reasons)
        if (_graph_.exists(node)) {
          _collectMessage_(schedule, _node_to_clique_[node], _node_to_clique_[node]);
        }
      }

      // collect messages for all set targets
      // by parsing _joint_target_to_clique_, we ensure that the cliques that
      // are referenced belong to the join tree (even if some of the nodes in
      // their associated joint_target do not belong to _graph_)
      for (const auto& set: _joint_target_to_clique_)
        _collectMessage_(schedule, set.second, set.second);

      // really perform the computations
      this->scheduler().execute(schedule);
    } else {
      // collect messages for all single targets
      for (const auto node: this->targets()) {
        // perform only collects in the join tree for nodes that have
        // not received hard evidence (those that received hard evidence were
        // not included into the join tree for speed-up reasons)
        if (_graph_.exists(node)) {
          _collectMessage_(_node_to_clique_[node], _node_to_clique_[node]);
        }
      }

      // collect messages for all set targets
      // by parsing _joint_target_to_clique_, we ensure that the cliques that
      // are referenced belong to the join tree (even if some of the nodes in
      // their associated joint_target do not belong to _graph_)
      for (const auto& set: _joint_target_to_clique_)
        _collectMessage_(set.second, set.second);
    }
  }


  /// returns a fresh potential equal to P(1st arg,evidence)
  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >*
     ShaferShenoyMRFInference< GUM_SCALAR >::unnormalizedJointPosterior_(NodeId id) {
    if (_use_schedules_) {
      Schedule schedule;
      return _unnormalizedJointPosterior_(schedule, id);
    } else {
      return _unnormalizedJointPosterior_(id);
    }
  }


  /// returns a fresh potential equal to P(1st arg,evidence)
  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >*
     ShaferShenoyMRFInference< GUM_SCALAR >::_unnormalizedJointPosterior_(Schedule& schedule,
                                                                          NodeId    id) {
    const auto& mn = this->MRF();

    // hard evidence do not belong to the join tree
    // # TODO: check for sets of inconsistent hard evidence
    if (this->hardEvidenceNodes().contains(id)) {
      return new Potential< GUM_SCALAR >(*(this->evidence()[id]));
    }

    auto& scheduler = this->scheduler();

    // if we still need to perform some inference task, do it (this should
    // already have been done by makeInference_)
    const NodeId clique_of_id = _node_to_clique_[id];
    _collectMessage_(schedule, clique_of_id, clique_of_id);

    // now we just need to create the product of the potentials of the clique
    // containing id with the messages received by this clique and
    // marginalize out all variables except id
    _ScheduleMultiDimSet_ pot_list;
    if (_clique_ss_potential_[clique_of_id] != nullptr)
      pot_list.insert(_clique_ss_potential_[clique_of_id]);

    // add the messages sent by adjacent nodes to targetClique
    for (const auto other: _JT_->neighbours(clique_of_id))
      pot_list.insert(_separator_potentials_[Arc(other, clique_of_id)]);

    // get the set of variables that need be removed from the potentials
    const NodeSet&                 nodes = _JT_->clique(clique_of_id);
    Set< const DiscreteVariable* > kept_vars{&(mn.variable(id))};
    Set< const DiscreteVariable* > del_vars(nodes.size());
    for (const auto node: nodes) {
      if (node != id) del_vars.insert(&(mn.variable(node)));
    }

    // pot_list now contains all the potentials to multiply and marginalize
    // => combine the messages
    auto resulting_pot = const_cast< ScheduleMultiDim< Potential< GUM_SCALAR > >* >(
       static_cast< const ScheduleMultiDim< Potential< GUM_SCALAR > >* >(
          _marginalizeOut_(schedule, pot_list, del_vars, kept_vars)));
    Potential< GUM_SCALAR >* joint = nullptr;

    scheduler.execute(schedule);

    // if pot already existed, create a copy, so that we can put it into
    // the _target_posteriors_ property
    if (pot_list.exists(resulting_pot)) {
      joint = new Potential< GUM_SCALAR >(resulting_pot->multiDim());
    } else {
      joint = resulting_pot->exportMultiDim();
    }

    // check that the joint posterior is different from a 0 vector: this would
    // indicate that some hard evidence are not compatible (their joint
    // probability is equal to 0)
    bool nonzero_found = false;
    for (Instantiation inst(*joint); !inst.end(); ++inst) {
      if (joint->get(inst)) {
        nonzero_found = true;
        break;
      }
    }
    if (!nonzero_found) {
      // remove joint from memory to avoid memory leaks
      delete joint;
      GUM_ERROR(IncompatibleEvidence,
                "some evidence entered into the Markov "
                "net are incompatible (their joint proba = 0)");
    }
    return joint;
  }


  /// returns a fresh potential equal to P(1st arg,evidence)
  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >*
     ShaferShenoyMRFInference< GUM_SCALAR >::_unnormalizedJointPosterior_(NodeId id) {
    const auto& mn = this->MRF();

    // hard evidence do not belong to the join tree
    // # TODO: check for sets of inconsistent hard evidence
    if (this->hardEvidenceNodes().contains(id)) {
      return new Potential< GUM_SCALAR >(*(this->evidence()[id]));
    }

    // if we still need to perform some inference task, do it (this should
    // already have been done by makeInference_)
    NodeId clique_of_id = _node_to_clique_[id];
    _collectMessage_(clique_of_id, clique_of_id);

    // now we just need to create the product of the potentials of the clique
    // containing id with the messages received by this clique and
    // marginalize out all variables except id
    _ScheduleMultiDimSet_ pot_list;
    if (_clique_ss_potential_[clique_of_id] != nullptr)
      pot_list.insert(_clique_ss_potential_[clique_of_id]);

    // add the messages sent by adjacent nodes to targetClique
    for (const auto other: _JT_->neighbours(clique_of_id))
      pot_list.insert(_separator_potentials_[Arc(other, clique_of_id)]);

    // get the set of variables that need be removed from the potentials
    const NodeSet&                 nodes = _JT_->clique(clique_of_id);
    Set< const DiscreteVariable* > kept_vars{&(mn.variable(id))};
    Set< const DiscreteVariable* > del_vars(nodes.size());
    for (const auto node: nodes) {
      if (node != id) del_vars.insert(&(mn.variable(node)));
    }

    // pot_list now contains all the potentials to multiply and marginalize
    // => combine the messages
    auto resulting_pot = const_cast< ScheduleMultiDim< Potential< GUM_SCALAR > >* >(
       static_cast< const ScheduleMultiDim< Potential< GUM_SCALAR > >* >(
          _marginalizeOut_(pot_list, del_vars, kept_vars)));
    Potential< GUM_SCALAR >* joint = nullptr;

    // if pot already existed, create a copy, so that we can put it into
    // the _target_posteriors_ property
    if (pot_list.exists(resulting_pot)) {
      joint = new Potential< GUM_SCALAR >(resulting_pot->multiDim());
    } else {
      joint = resulting_pot->exportMultiDim();
      delete resulting_pot;
    }

    // check that the joint posterior is different from a 0 vector: this would
    // indicate that some hard evidence are not compatible (their joint
    // probability is equal to 0)
    bool nonzero_found = false;
    for (Instantiation inst(*joint); !inst.end(); ++inst) {
      if (joint->get(inst)) {
        nonzero_found = true;
        break;
      }
    }
    if (!nonzero_found) {
      // remove joint from memory to avoid memory leaks
      delete joint;
      GUM_ERROR(IncompatibleEvidence,
                "some evidence entered into the Markov "
                "net are incompatible (their joint proba = 0)");
    }
    return joint;
  }


  /// returns the posterior of a given variable
  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >& ShaferShenoyMRFInference< GUM_SCALAR >::posterior_(NodeId id) {
    // check if we have already computed the posterior
    if (_target_posteriors_.exists(id)) { return *(_target_posteriors_[id]); }

    // compute the joint posterior and normalize
    auto joint = unnormalizedJointPosterior_(id);
    if (joint->sum() != 1)   // hard test for ReadOnly CPT (as aggregator)
      joint->normalize();
    _target_posteriors_.insert(id, joint);

    return *joint;
  }


  /// returns the marginal a posteriori proba of a given node
  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >*
     ShaferShenoyMRFInference< GUM_SCALAR >::unnormalizedJointPosterior_(const NodeSet& set) {
    if (_use_schedules_) {
      Schedule schedule;
      return _unnormalizedJointPosterior_(schedule, set);
    } else {
      return _unnormalizedJointPosterior_(set);
    }
  }


  /// returns the marginal a posteriori proba of a given node
  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >*
     ShaferShenoyMRFInference< GUM_SCALAR >::_unnormalizedJointPosterior_(Schedule&      schedule,
                                                                          const NodeSet& set) {
    // hard evidence do not belong to the join tree, so extract the nodes
    // from targets that are not hard evidence
    NodeSet targets = set, hard_ev_nodes;
    for (const auto node: this->hardEvidenceNodes()) {
      if (targets.contains(node)) {
        targets.erase(node);
        hard_ev_nodes.insert(node);
      }
    }

    auto& scheduler = this->scheduler();

    // if all the nodes have received hard evidence, then compute the
    // joint posterior directly by multiplying the hard evidence potentials
    const auto& evidence = this->evidence();
    if (targets.empty()) {
      if (set.size() == 1) {
        return new Potential< GUM_SCALAR >(*evidence[*set.begin()]);
      } else {
        _ScheduleMultiDimSet_ pot_list;
        for (const auto node: set) {
          auto new_pot_ev = schedule.insertTable< Potential< GUM_SCALAR > >(*evidence[node], false);
          pot_list.insert(new_pot_ev);
        }

        // combine all the potentials of the nodes in set
        MultiDimCombinationDefault< Potential< GUM_SCALAR > > fast_combination(_combination_op_);
        const IScheduleMultiDim* pot = fast_combination.schedule(schedule, pot_list);
        auto schedule_pot            = const_cast< ScheduleMultiDim< Potential< GUM_SCALAR > >* >(
           static_cast< const ScheduleMultiDim< Potential< GUM_SCALAR > >* >(pot));
        scheduler.execute(schedule);
        auto result = schedule_pot->exportMultiDim();

        return result;
      }
    }


    // if we still need to perform some inference task, do it: so, first,
    // determine the clique on which we should perform collect to compute
    // the unnormalized joint posterior of a set of nodes containing "targets"
    NodeId clique_of_set;
    try {
      clique_of_set = _joint_target_to_clique_[set];
    } catch (NotFound const&) {
      // here, the precise set of targets does not belong to the set of targets
      // defined by the user. So we will try to find a clique in the junction
      // tree that contains "targets":

      // 1/ we should check that all the nodes belong to the join tree
      for (const auto node: targets) {
        if (!_graph_.exists(node)) { GUM_ERROR(UndefinedElement, node << " is not a target node") }
      }

      // 2/ the clique created by the first eliminated node among target is the
      // one we are looking for
      const std::vector< NodeId >& JT_elim_order = _triangulation_->eliminationOrder();
      NodeProperty< int >          elim_order(Size(JT_elim_order.size()));
      for (std::size_t i = std::size_t(0), size = JT_elim_order.size(); i < size; ++i)
        elim_order.insert(JT_elim_order[i], (int)i);
      NodeId first_eliminated_node = *(targets.begin());
      int    elim_number           = elim_order[first_eliminated_node];
      for (const auto node: targets) {
        if (elim_order[node] < elim_number) {
          elim_number           = elim_order[node];
          first_eliminated_node = node;
        }
      }

      clique_of_set = _triangulation_->createdJunctionTreeClique(first_eliminated_node);


      // 3/ check that clique_of_set contains the all the nodes in the target
      const NodeSet& clique_nodes = _JT_->clique(clique_of_set);
      for (const auto node: targets) {
        if (!clique_nodes.contains(node)) {
          GUM_ERROR(UndefinedElement, set << " is not a joint target")
        }
      }

      // add the discovered clique to  _joint_target_to_clique_
      _joint_target_to_clique_.insert(set, clique_of_set);
    }

    // now perform a collect on the clique
    _collectMessage_(schedule, clique_of_set, clique_of_set);

    // now we just need to create the product of the potentials of the clique
    // containing set with the messages received by this clique and
    // marginalize out all variables except set
    _ScheduleMultiDimSet_ pot_list;
    if (_clique_ss_potential_[clique_of_set] != nullptr) {
      auto pot = _clique_ss_potential_[clique_of_set];
      if (!schedule.existsScheduleMultiDim(pot->id())) schedule.emplaceScheduleMultiDim(*pot);
      pot_list.insert(_clique_ss_potential_[clique_of_set]);
    }

    // add the messages sent by adjacent nodes to targetClique
    for (const auto other: _JT_->neighbours(clique_of_set)) {
      const auto pot = _separator_potentials_[Arc(other, clique_of_set)];
      if (pot != nullptr) pot_list.insert(pot);
    }


    // get the set of variables that need be removed from the potentials
    const NodeSet&                 nodes = _JT_->clique(clique_of_set);
    Set< const DiscreteVariable* > del_vars(nodes.size());
    Set< const DiscreteVariable* > kept_vars(targets.size());
    const auto&                    mn = this->MRF();
    for (const auto node: nodes) {
      if (!targets.contains(node)) {
        del_vars.insert(&(mn.variable(node)));
      } else {
        kept_vars.insert(&(mn.variable(node)));
      }
    }

    // pot_list now contains all the potentials to multiply and marginalize
    // => combine the messages
    const IScheduleMultiDim* new_pot = _marginalizeOut_(schedule, pot_list, del_vars, kept_vars);
    scheduler.execute(schedule);
    ScheduleMultiDim< Potential< GUM_SCALAR > >* resulting_pot
       = const_cast< ScheduleMultiDim< Potential< GUM_SCALAR > >* >(
          static_cast< const ScheduleMultiDim< Potential< GUM_SCALAR > >* >(new_pot));

    // if pot already existed, create a copy, so that we can put it into
    // the _target_posteriors_ property
    Potential< GUM_SCALAR >* joint = nullptr;
    if (pot_list.exists(resulting_pot)) {
      joint = new Potential< GUM_SCALAR >(resulting_pot->multiDim());
    } else {
      joint = resulting_pot->exportMultiDim();
    }

    // check that the joint posterior is different from a 0 vector: this would
    // indicate that some hard evidence are not compatible
    bool nonzero_found = false;
    for (Instantiation inst(*joint); !inst.end(); ++inst) {
      if ((*joint)[inst]) {
        nonzero_found = true;
        break;
      }
    }
    if (!nonzero_found) {
      // remove joint from memory to avoid memory leaks
      delete joint;
      GUM_ERROR(IncompatibleEvidence,
                "some evidence entered into the Markov "
                "net are incompatible (their joint proba = 0)");
    }

    return joint;
  }


  /// returns the marginal a posteriori proba of a given node
  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >*
     ShaferShenoyMRFInference< GUM_SCALAR >::_unnormalizedJointPosterior_(const NodeSet& set) {
    // hard evidence do not belong to the join tree, so extract the nodes
    // from targets that are not hard evidence
    NodeSet targets = set, hard_ev_nodes;
    for (const auto node: this->hardEvidenceNodes()) {
      if (targets.contains(node)) {
        targets.erase(node);
        hard_ev_nodes.insert(node);
      }
    }

    // if all the nodes have received hard evidence, then compute the
    // joint posterior directly by multiplying the hard evidence potentials
    const auto& evidence = this->evidence();
    if (targets.empty()) {
      if (set.size() == 1) {
        return new Potential< GUM_SCALAR >(*evidence[*set.begin()]);
      } else {
        _PotentialSet_ pot_list;
        for (const auto node: set) {
          pot_list.insert(evidence[node]);
        }

        // combine all the potentials of the nodes in set
        MultiDimCombinationDefault< Potential< GUM_SCALAR > > fast_combination(_combination_op_);
        const Potential< GUM_SCALAR >* pot = fast_combination.execute(pot_list);

        return const_cast< Potential< GUM_SCALAR >* >(pot);
      }
    }


    // if we still need to perform some inference task, do it: so, first,
    // determine the clique on which we should perform collect to compute
    // the unnormalized joint posterior of a set of nodes containing "targets"
    NodeId clique_of_set;
    try {
      clique_of_set = _joint_target_to_clique_[set];
    } catch (NotFound const&) {
      // here, the precise set of targets does not belong to the set of targets
      // defined by the user. So we will try to find a clique in the junction
      // tree that contains "targets":

      // 1/ we should check that all the nodes belong to the join tree
      for (const auto node: targets) {
        if (!_graph_.exists(node)) { GUM_ERROR(UndefinedElement, node << " is not a target node") }
      }

      // 2/ the clique created by the first eliminated node among target is the
      // one we are looking for
      const std::vector< NodeId >& JT_elim_order = _triangulation_->eliminationOrder();
      NodeProperty< int >          elim_order(Size(JT_elim_order.size()));
      for (std::size_t i = std::size_t(0), size = JT_elim_order.size(); i < size; ++i)
        elim_order.insert(JT_elim_order[i], (int)i);
      NodeId first_eliminated_node = *(targets.begin());
      int    elim_number           = elim_order[first_eliminated_node];
      for (const auto node: targets) {
        if (elim_order[node] < elim_number) {
          elim_number           = elim_order[node];
          first_eliminated_node = node;
        }
      }

      clique_of_set = _triangulation_->createdJunctionTreeClique(first_eliminated_node);


      // 3/ check that clique_of_set contains the all the nodes in the target
      const NodeSet& clique_nodes = _JT_->clique(clique_of_set);
      for (const auto node: targets) {
        if (!clique_nodes.contains(node)) {
          GUM_ERROR(UndefinedElement, set << " is not a joint target")
        }
      }

      // add the discovered clique to  _joint_target_to_clique_
      _joint_target_to_clique_.insert(set, clique_of_set);
    }

    // now perform a collect on the clique
    _collectMessage_(clique_of_set, clique_of_set);

    // now we just need to create the product of the potentials of the clique
    // containing set with the messages received by this clique and
    // marginalize out all variables except set
    _ScheduleMultiDimSet_ pot_list;
    if (_clique_ss_potential_[clique_of_set] != nullptr) {
      auto pot = _clique_ss_potential_[clique_of_set];
      if (pot != nullptr) pot_list.insert(_clique_ss_potential_[clique_of_set]);
    }

    // add the messages sent by adjacent nodes to targetClique
    for (const auto other: _JT_->neighbours(clique_of_set)) {
      const auto pot = _separator_potentials_[Arc(other, clique_of_set)];
      if (pot != nullptr) pot_list.insert(pot);
    }


    // get the set of variables that need be removed from the potentials
    const NodeSet&                 nodes = _JT_->clique(clique_of_set);
    Set< const DiscreteVariable* > del_vars(nodes.size());
    Set< const DiscreteVariable* > kept_vars(targets.size());
    const auto&                    mn = this->MRF();
    for (const auto node: nodes) {
      if (!targets.contains(node)) {
        del_vars.insert(&(mn.variable(node)));
      } else {
        kept_vars.insert(&(mn.variable(node)));
      }
    }

    // pot_list now contains all the potentials to multiply and marginalize
    // => combine the messages
    const IScheduleMultiDim* new_pot = _marginalizeOut_(pot_list, del_vars, kept_vars);
    ScheduleMultiDim< Potential< GUM_SCALAR > >* resulting_pot
       = const_cast< ScheduleMultiDim< Potential< GUM_SCALAR > >* >(
          static_cast< const ScheduleMultiDim< Potential< GUM_SCALAR > >* >(new_pot));

    // if pot already existed, create a copy, so that we can put it into
    // the _target_posteriors_ property
    Potential< GUM_SCALAR >* joint = nullptr;
    if (pot_list.exists(resulting_pot)) {
      joint = new Potential< GUM_SCALAR >(resulting_pot->multiDim());
    } else {
      joint = resulting_pot->exportMultiDim();
      delete new_pot;
    }

    // check that the joint posterior is different from a 0 vector: this would
    // indicate that some hard evidence are not compatible
    bool nonzero_found = false;
    for (Instantiation inst(*joint); !inst.end(); ++inst) {
      if ((*joint)[inst]) {
        nonzero_found = true;
        break;
      }
    }
    if (!nonzero_found) {
      // remove joint from memory to avoid memory leaks
      delete joint;
      GUM_ERROR(IncompatibleEvidence,
                "some evidence entered into the Markov "
                "net are incompatible (their joint proba = 0)");
    }

    return joint;
  }


  /// returns the posterior of a given set of variables
  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
     ShaferShenoyMRFInference< GUM_SCALAR >::jointPosterior_(const NodeSet& set) {
    // check if we have already computed the posterior
    if (_joint_target_posteriors_.exists(set)) { return *(_joint_target_posteriors_[set]); }

    // compute the joint posterior and normalize
    auto joint = unnormalizedJointPosterior_(set);
    joint->normalize();
    _joint_target_posteriors_.insert(set, joint);

    return *joint;
  }


  /// returns the posterior of a given set of variables
  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
     ShaferShenoyMRFInference< GUM_SCALAR >::jointPosterior_(const NodeSet& wanted_target,
                                                             const NodeSet& declared_target) {
    // check if we have already computed the posterior of wanted_target
    if (_joint_target_posteriors_.exists(wanted_target))
      return *(_joint_target_posteriors_[wanted_target]);

    // here, we will have to compute the posterior of declared_target and
    // marginalize out all the variables that do not belong to wanted_target

    // check if we have already computed the posterior of declared_target
    if (!_joint_target_posteriors_.exists(declared_target)) { jointPosterior_(declared_target); }

    // marginalize out all the variables that do not belong to wanted_target
    const auto&                    mn = this->MRF();
    Set< const DiscreteVariable* > del_vars;
    for (const auto node: declared_target)
      if (!wanted_target.contains(node)) del_vars.insert(&(mn.variable(node)));
    auto pot = new Potential< GUM_SCALAR >(
       _joint_target_posteriors_[declared_target]->margSumOut(del_vars));

    // save the result into the cache
    _joint_target_posteriors_.insert(wanted_target, pot);

    return *pot;
  }


  template < typename GUM_SCALAR >
  GUM_SCALAR ShaferShenoyMRFInference< GUM_SCALAR >::evidenceProbability() {
    // perform inference in each connected component
    this->makeInference();

    // for each connected component, select a variable X and compute the
    // joint probability of X and evidence e. Then marginalize-out X to get
    // p(e) in this connected component. Finally, multiply all the p(e) that
    // we got and the elements in   _constants_. The result is the probability
    // of evidence

    GUM_SCALAR prob_ev = 1;
    for (const auto root: _roots_) {
      // get a node in the clique
      NodeId                   node = *(_JT_->clique(root).begin());
      Potential< GUM_SCALAR >* tmp  = unnormalizedJointPosterior_(node);
      prob_ev *= tmp->sum();
      delete tmp;
    }

    for (const auto& projected_cpt: _constants_)
      prob_ev *= projected_cpt.second;

    return prob_ev;
  }


  template < typename GUM_SCALAR >
  bool ShaferShenoyMRFInference< GUM_SCALAR >::isExactJointComputable_(const NodeSet& vars) {
    if (JointTargetedMRFInference< GUM_SCALAR >::isExactJointComputable_(vars)) return true;

    this->prepareInference();

    for (const auto& node: this->_JT_->nodes()) {
      const auto clique = _JT_->clique(node);
      if (vars == clique) return true;
    }
    return false;
  }

  template < typename GUM_SCALAR >
  NodeSet ShaferShenoyMRFInference< GUM_SCALAR >::superForJointComputable_(const NodeSet& vars) {
    const auto superset = JointTargetedMRFInference< GUM_SCALAR >::superForJointComputable_(vars);
    if (!superset.empty()) return superset;

    this->prepareInference();

    for (const auto& node: _JT_->nodes()) {
      const auto clique = _JT_->clique(node);
      if (vars.isProperSubsetOf(clique)) return clique;
    }


    return NodeSet();
  }

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
