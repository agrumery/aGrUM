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
 * @brief Implementation of lazy propagation for inference in
 * Bayesian networks.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#  include <algorithm>
#  include <limits>

#  include <agrum/BN/algorithms/BayesBall.h>
#  include <agrum/BN/algorithms/barrenNodesFinder.h>
#  include <agrum/BN/algorithms/dSeparationAlgorithm.h>
#  include <agrum/tools/graphs/algorithms/binaryJoinTreeConverterDefault.h>
#  include <agrum/tools/multidim/instantiation.h>
#  include <agrum/tools/multidim/utils/operators/multiDimCombineAndProjectDefault.h>
#  include <agrum/tools/multidim/utils/operators/multiDimProjection.h>


namespace gum {


  // default constructor
  template < typename GUM_SCALAR >
  INLINE LazyPropagation< GUM_SCALAR >::LazyPropagation(const IBayesNet< GUM_SCALAR >* BN,
                                                        RelevantPotentialsFinderType relevant_type,
                                                        FindBarrenNodesType          barren_type,
                                                        bool use_binary_join_tree) :
      JointTargetedInference< GUM_SCALAR >(BN),
      EvidenceInference< GUM_SCALAR >(BN), _use_binary_join_tree_(use_binary_join_tree) {
    // sets the relevant potential and the barren nodes finding algorithm
    _findRelevantPotentials_
       = &LazyPropagation< GUM_SCALAR >::_findRelevantPotentialsWithdSeparation2_;
    setRelevantPotentialsFinderType(relevant_type);
    setFindBarrenNodesType(barren_type);

    // create a default triangulation (the user can change it afterwards)
    _triangulation_ = new DefaultTriangulation;

    // for debugging purposes
    GUM_CONSTRUCTOR(LazyPropagation);
  }


  // destructor
  template < typename GUM_SCALAR >
  INLINE LazyPropagation< GUM_SCALAR >::~LazyPropagation() {
    // remove all the potentials created during the last message passing
    for (const auto& pots: _arc_to_created_potentials_)
      for (const auto pot: pots.second)
        delete pot;

    // remove all the potentials stored into the cliques
    for (const auto& pots: _clique_potentials_)
      for (const auto pot: pots.second)
        delete pot;

    // remove all the posteriors computed
    for (const auto& pot: _target_posteriors_)
      delete pot.second;
    for (const auto& pot: _joint_target_posteriors_)
      delete pot.second;

    // remove the junction tree and the triangulation algorithm
    if (_JT_ != nullptr) delete _JT_;
    if (_junctionTree_ != nullptr) delete _junctionTree_;
    delete _triangulation_;

    GUM_DESTRUCTOR(LazyPropagation);
  }


  /// set a new triangulation algorithm
  template < typename GUM_SCALAR >
  void LazyPropagation< GUM_SCALAR >::setTriangulation(const Triangulation& new_triangulation) {
    delete _triangulation_;
    _triangulation_    = new_triangulation.newFactory();
    _is_new_jt_needed_ = true;
    this->setOutdatedStructureState_();
  }


  /// returns the current join (or junction) tree used
  template < typename GUM_SCALAR >
  INLINE const JoinTree* LazyPropagation< GUM_SCALAR >::joinTree() {
    if (_is_new_jt_needed_) _createNewJT_();

    return _JT_;
  }

  /// returns the current junction tree
  template < typename GUM_SCALAR >
  INLINE const JunctionTree* LazyPropagation< GUM_SCALAR >::junctionTree() {
    if (_is_new_jt_needed_) _createNewJT_();

    return _junctionTree_;
  }


  /// sets how we determine the relevant potentials to combine
  template < typename GUM_SCALAR >
  void LazyPropagation< GUM_SCALAR >::setRelevantPotentialsFinderType(
     RelevantPotentialsFinderType type) {
    if (type != _find_relevant_potential_type_) {
      switch (type) {
        case RelevantPotentialsFinderType::DSEP_BAYESBALL_POTENTIALS:
          _findRelevantPotentials_
             = &LazyPropagation< GUM_SCALAR >::_findRelevantPotentialsWithdSeparation2_;
          break;

        case RelevantPotentialsFinderType::DSEP_BAYESBALL_NODES:
          _findRelevantPotentials_
             = &LazyPropagation< GUM_SCALAR >::_findRelevantPotentialsWithdSeparation_;
          break;

        case RelevantPotentialsFinderType::DSEP_KOLLER_FRIEDMAN_2009:
          _findRelevantPotentials_
             = &LazyPropagation< GUM_SCALAR >::_findRelevantPotentialsWithdSeparation3_;
          break;

        case RelevantPotentialsFinderType::FIND_ALL:
          _findRelevantPotentials_ = &LazyPropagation< GUM_SCALAR >::_findRelevantPotentialsGetAll_;
          break;

        default:
          GUM_ERROR(InvalidArgument,
                    "setRelevantPotentialsFinderType for type " << (unsigned int)type
                                                                << " is not implemented yet");
      }

      _find_relevant_potential_type_ = type;

      // indicate that all messages need be reconstructed to take into account
      // the change in d-separation analysis
      _invalidateAllMessages_();
    }
  }


  /// sets the operator for performing the projections
  template < typename GUM_SCALAR >
  INLINE void LazyPropagation< GUM_SCALAR >::_setProjectionFunction_(Potential< GUM_SCALAR > (
     *proj)(const Potential< GUM_SCALAR >&, const Set< const DiscreteVariable* >&)) {
    _projection_op_ = proj;

    // indicate that all messages need be reconstructed to take into account
    // the change in of the projection operator
    _invalidateAllMessages_();
  }


  /// sets the operator for performing the combinations
  template < typename GUM_SCALAR >
  INLINE void LazyPropagation< GUM_SCALAR >::_setCombinationFunction_(Potential< GUM_SCALAR > (
     *comb)(const Potential< GUM_SCALAR >&, const Potential< GUM_SCALAR >&)) {
    _combination_op_ = comb;

    // indicate that all messages need be reconstructed to take into account
    // the change of the combination operator
    _invalidateAllMessages_();
  }


  /// invalidate all messages, posteriors and created potentials
  template < typename GUM_SCALAR >
  void LazyPropagation< GUM_SCALAR >::_invalidateAllMessages_() {
    // remove all the messages computed
    for (auto& potset: _separator_potentials_)
      potset.second.clear();
    for (auto& mess_computed: _messages_computed_)
      mess_computed.second = false;

    // remove all the created potentials kept on the arcs
    for (const auto& potset: _arc_to_created_potentials_)
      for (const auto pot: potset.second)
        delete pot;

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


  /// sets how we determine barren nodes
  template < typename GUM_SCALAR >
  void LazyPropagation< GUM_SCALAR >::setFindBarrenNodesType(FindBarrenNodesType type) {
    if (type != _barren_nodes_type_) {
      // WARNING: if a new type is added here, method  _createJT_ should certainly
      // be updated as well, in particular its step 2.
      switch (type) {
        case FindBarrenNodesType::FIND_BARREN_NODES:
        case FindBarrenNodesType::FIND_NO_BARREN_NODES: break;

        default:
          GUM_ERROR(InvalidArgument,
                    "setFindBarrenNodesType for type " << (unsigned int)type
                                                       << " is not implemented yet");
      }

      _barren_nodes_type_ = type;

      // potentially, we may need to reconstruct a junction tree
      this->setOutdatedStructureState_();
    }
  }


  /// fired when a new evidence is inserted
  template < typename GUM_SCALAR >
  INLINE void LazyPropagation< GUM_SCALAR >::onEvidenceAdded_(const NodeId id,
                                                              bool         isHardEvidence) {
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
  INLINE void LazyPropagation< GUM_SCALAR >::onEvidenceErased_(const NodeId id,
                                                               bool         isHardEvidence) {
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
  void LazyPropagation< GUM_SCALAR >::onAllEvidenceErased_(bool has_hard_evidence) {
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
  INLINE void LazyPropagation< GUM_SCALAR >::onEvidenceChanged_(const NodeId id,
                                                                bool         hasChangedSoftHard) {
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


  /// fired after a new Bayes net has been assigned to the engine
  template < typename GUM_SCALAR >
  INLINE void LazyPropagation< GUM_SCALAR >::onModelChanged_(const GraphicalModel* bn) {}


  /// fired after a new target is inserted
  template < typename GUM_SCALAR >
  INLINE void LazyPropagation< GUM_SCALAR >::onMarginalTargetAdded_(const NodeId id) {
    // if the graph does not contain the node, either this is due to the fact that
    // the node has received a hard evidence or because it was d-separated from the
    // target nodes during the last inference. In the latter case, we should change
    // the graph, hence we should recompute the JT
    if (!_graph_.exists(id) && !_hard_ev_nodes_.contains(id)) { _is_new_jt_needed_ = true; }
  }


  /// fired before a target is removed
  template < typename GUM_SCALAR >
  INLINE void LazyPropagation< GUM_SCALAR >::onMarginalTargetErased_(const NodeId id) {}


  /// fired after a new set target is inserted
  template < typename GUM_SCALAR >
  void LazyPropagation< GUM_SCALAR >::onJointTargetAdded_(const NodeSet& set) {
    // if there is no current joint tree, obviously, we need one.
    if (_JT_ == nullptr) {
      _is_new_jt_needed_ = true;
      return;
    }

    // here, we will remove from set the nodes that received hard evidence and try
    // to find a clique in the current _JT_ that can contain the resulting set. If
    // we cannot find one, we must recompute the _JT_
    NodeId                       first_eliminated_node = std::numeric_limits< NodeId >::max();
    int                          elim_number           = std::numeric_limits< int >::max();
    const std::vector< NodeId >& JT_elim_order         = _triangulation_->eliminationOrder();
    NodeProperty< int >          elim_order(Size(JT_elim_order.size()));
    for (std::size_t i = std::size_t(0), size = JT_elim_order.size(); i < size; ++i)
      elim_order.insert(JT_elim_order[i], (int)i);
    NodeSet unobserved_set(set.size());
    for (const auto node: set) {
      if (!_graph_.exists(node)) {
        if (!_hard_ev_nodes_.contains(node)) {
          _is_new_jt_needed_ = true;
          return;
        }
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
        if (!clique.contains(node)) {
          _is_new_jt_needed_ = true;
          return;
        }
      }
    }
  }


  /// fired before a set target is removed
  template < typename GUM_SCALAR >
  INLINE void LazyPropagation< GUM_SCALAR >::onJointTargetErased_(const NodeSet& set) {}


  /// fired after all the nodes of the BN are added as single targets
  template < typename GUM_SCALAR >
  INLINE void LazyPropagation< GUM_SCALAR >::onAllMarginalTargetsAdded_() {
    for (const auto node: this->BN().dag()) {
      // if the graph does not contain the node, either this is due to the fact
      // that the node has received a hard evidence or because it was d-separated
      // from the target nodes during the last inference. In the latter case, we
      // should change the graph, hence we should recompute the JT
      if (!_graph_.exists(node) && !_hard_ev_nodes_.contains(node)) {
        _is_new_jt_needed_ = true;
        return;
      }
    }
  }


  /// fired before a all the single_targets are removed
  template < typename GUM_SCALAR >
  INLINE void LazyPropagation< GUM_SCALAR >::onAllMarginalTargetsErased_() {}


  /// fired before a all the joint_targets are removed
  template < typename GUM_SCALAR >
  INLINE void LazyPropagation< GUM_SCALAR >::onAllJointTargetsErased_() {}


  /// fired before a all the single and joint_targets are removed
  template < typename GUM_SCALAR >
  INLINE void LazyPropagation< GUM_SCALAR >::onAllTargetsErased_() {}


  // check whether a new junction tree is really needed for the next inference
  template < typename GUM_SCALAR >
  bool LazyPropagation< GUM_SCALAR >::_isNewJTNeeded_() const {
    // if we do not have a JT or if _new_jt_needed_ is set to true, then
    // we know that we need to create a new join tree
    if ((_JT_ == nullptr) || _is_new_jt_needed_) return true;

    // if some targets do not belong to the join tree and, consequently, to the
    // undirected graph that was used to construct the join tree, then we need
    // to create a new JT. This situation may occur if we constructed the
    // join tree after pruning irrelevant/barren nodes from the BN.
    // However, note that the nodes that received hard evidence do not belong to
    // the graph and, therefore, should not be taken into account
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
  void LazyPropagation< GUM_SCALAR >::_createNewJT_() {
    // to create the JT, we first create the moral graph of the BN in the
    // following way in order to take into account the barren nodes and the
    // nodes that received evidence:
    // 1/ we create an undirected graph containing only the nodes and no edge
    // 2/ if we take into account barren nodes, remove them from the graph
    // 3/ if we take d-separation into account, remove the d-separated nodes
    // 4/ add edges so that each node and its parents in the BN form a clique
    // 5/ add edges so that join targets form a clique of the moral graph
    // 6/ remove the nodes that received hard evidence (by step 4/, their
    //    parents are linked by edges, which is necessary for inference)
    //
    // At the end of step 6/, we have our moral graph and we can triangulate it
    // to get the new junction tree

    // 1/ create an undirected graph containing only the nodes and no edge
    const auto& bn = this->BN();
    _graph_.clear();
    for (const auto node: bn.dag())
      _graph_.addNodeWithId(node);

    // identify the target nodes
    NodeSet target_nodes = this->targets();
    for (const auto& nodeset: this->jointTargets()) {
      target_nodes += nodeset;
    }

    // 2/ if we wish to exploit barren nodes, we shall remove them from the
    // BN. To do so: we identify all the nodes that are not targets and have
    // received no evidence and such that their descendants are neither
    // targets nor evidence nodes. Such nodes can be safely discarded from
    // the BN without altering the inference output
    if ((this->nbrTargets() != bn.dag().size())
        && (_barren_nodes_type_ == FindBarrenNodesType::FIND_BARREN_NODES)) {
      // check that all the nodes are not targets, otherwise, there is no
      // barren node
      if (target_nodes.size() != bn.size()) {
        BarrenNodesFinder finder(&(bn.dag()));
        finder.setTargets(&target_nodes);

        NodeSet evidence_nodes(this->evidence().size());
        for (const auto& pair: this->evidence()) {
          evidence_nodes.insert(pair.first);
        }
        finder.setEvidence(&evidence_nodes);

        NodeSet barren_nodes = finder.barrenNodes();

        // remove the barren nodes from the moral graph
        for (const auto node: barren_nodes) {
          _graph_.eraseNode(node);
        }
      }
    }

    // 3/ if we wish to exploit d-separation, remove all the nodes that are
    // d-separated from our targets. Of course, if all the nodes are targets,
    // no need to perform a d-separation analysis
    if (this->nbrTargets() != bn.dag().size()) {
      NodeSet requisite_nodes;
      bool    dsep_analysis = false;
      switch (_find_relevant_potential_type_) {
        case RelevantPotentialsFinderType::DSEP_BAYESBALL_POTENTIALS:
        case RelevantPotentialsFinderType::DSEP_BAYESBALL_NODES: {
          BayesBall::requisiteNodes(bn.dag(),
                                    target_nodes,
                                    this->hardEvidenceNodes(),
                                    this->softEvidenceNodes(),
                                    requisite_nodes);
          dsep_analysis = true;
        } break;

        case RelevantPotentialsFinderType::DSEP_KOLLER_FRIEDMAN_2009: {
          dSeparationAlgorithm dsep;
          dsep.requisiteNodes(bn.dag(),
                              target_nodes,
                              this->hardEvidenceNodes(),
                              this->softEvidenceNodes(),
                              requisite_nodes);
          dsep_analysis = true;
        } break;

        case RelevantPotentialsFinderType::FIND_ALL: break;

        default: GUM_ERROR(FatalError, "not implemented yet")
      }

      // remove all the nodes that are not requisite
      if (dsep_analysis) {
        for (auto iter = _graph_.beginSafe(); iter != _graph_.endSafe(); ++iter) {
          if (!requisite_nodes.contains(*iter) && !this->hardEvidenceNodes().contains(*iter)) {
            _graph_.eraseNode(*iter);
          }
        }
      }
    }

    // 4/ add edges so that each node and its parents in the BN form a clique
    for (const auto node: _graph_) {
      const NodeSet& parents = bn.parents(node);
      for (auto iter1 = parents.cbegin(); iter1 != parents.cend(); ++iter1) {
        // before adding an edge between node and its parent, check that the
        // parent belongs to the graph. Actually, when d-separated nodes are
        // removed, it may be the case that the parents of hard evidence nodes
        // are removed. But the latter still exist in the graph.
        if (_graph_.existsNode(*iter1)) {
          _graph_.addEdge(*iter1, node);

          auto iter2 = iter1;
          for (++iter2; iter2 != parents.cend(); ++iter2) {
            // before adding an edge, check that both extremities belong to
            // the graph. Actually, when d-separated nodes are removed, it may
            // be the case that the parents of hard evidence nodes are removed.
            // But the latter still exist in the graph.
            if (_graph_.existsNode(*iter2)) _graph_.addEdge(*iter1, *iter2);
          }
        }
      }
    }

    // 5/ if there exist some joint targets, we shall add new edges
    // into the moral graph in order to ensure that there exists a clique
    // containing each joint
    for (const auto& nodeset: this->jointTargets()) {
      for (auto iter1 = nodeset.cbegin(); iter1 != nodeset.cend(); ++iter1) {
        auto iter2 = iter1;
        for (++iter2; iter2 != nodeset.cend(); ++iter2) {
          _graph_.addEdge(*iter1, *iter2);
        }
      }
    }

    // 6/ remove all the nodes that received hard evidence
    _hard_ev_nodes_ = this->hardEvidenceNodes();
    for (const auto node: _hard_ev_nodes_) {
      _graph_.eraseNode(node);
    }


    // now, we can compute the new junction tree. To speed-up computations
    // (essentially, those of a distribution phase), we construct from this
    // junction tree a binary join tree
    if (_JT_ != nullptr) delete _JT_;
    if (_junctionTree_ != nullptr) delete _junctionTree_;

    _triangulation_->setGraph(&_graph_, &(this->domainSizes()));
    const JunctionTree& triang_jt = _triangulation_->junctionTree();
    if (_use_binary_join_tree_) {
      BinaryJoinTreeConverterDefault bjt_converter;
      NodeSet                        emptyset;
      _JT_ = new CliqueGraph(bjt_converter.convert(triang_jt, this->domainSizes(), emptyset));
    } else {
      _JT_ = new CliqueGraph(triang_jt);
    }
    _junctionTree_ = new CliqueGraph(triang_jt);


    // indicate, for each node of the moral graph, a clique in _JT_ that can
    // contain its conditional probability table
    _node_to_clique_.clear();
    const std::vector< NodeId >& JT_elim_order = _triangulation_->eliminationOrder();
    NodeProperty< int >          elim_order(Size(JT_elim_order.size()));
    for (std::size_t i = std::size_t(0), size = JT_elim_order.size(); i < size; ++i)
      elim_order.insert(JT_elim_order[i], (int)i);
    const DAG& dag = bn.dag();
    for (const auto node: _graph_) {
      // get the variables in the potential of node (and its parents)
      NodeId first_eliminated_node = node;
      int    elim_number           = elim_order[first_eliminated_node];

      for (const auto parent: dag.parents(node)) {
        if (_graph_.existsNode(parent) && (elim_order[parent] < elim_number)) {
          elim_number           = elim_order[parent];
          first_eliminated_node = parent;
        }
      }

      // first_eliminated_node contains the first var (node or one of its
      // parents) eliminated => the clique created during its elimination
      // contains node and all of its parents => it can contain the potential
      // assigned to the node in the BN
      _node_to_clique_.insert(node,
                              _triangulation_->createdJunctionTreeClique(first_eliminated_node));
    }

    // do the same for the nodes that received hard evidence. Here, we only store
    // the nodes for which at least one parent belongs to _graph_ (otherwise
    // their CPT is just a constant real number).
    for (const auto node: _hard_ev_nodes_) {
      NodeId first_eliminated_node = std::numeric_limits< NodeId >::max();
      int    elim_number           = std::numeric_limits< int >::max();

      for (const auto parent: dag.parents(node)) {
        if (_graph_.exists(parent) && (elim_order[parent] < elim_number)) {
          elim_number           = elim_order[parent];
          first_eliminated_node = parent;
        }
      }

      // first_eliminated_node contains the first var (node or one of its
      // parents) eliminated => the clique created during its elimination
      // contains node and all of its parents => it can contain the potential
      // assigned to the node in the BN
      if (elim_number != std::numeric_limits< int >::max()) {
        _node_to_clique_.insert(node,
                                _triangulation_->createdJunctionTreeClique(first_eliminated_node));
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
    // the CPTs resulting from the projections of hard evidence as well as the
    // CPTs of the soft evidence
    for (const auto& potlist: _clique_potentials_)
      for (const auto pot: potlist.second)
        delete pot;
    _clique_potentials_.clear();

    // remove all the potentials created during the last inference
    for (const auto& potlist: _arc_to_created_potentials_)
      for (const auto pot: potlist.second)
        delete pot;
    _arc_to_created_potentials_.clear();

    // remove all the potentials created to take into account hard evidence
    // during the last inference (they have already been deleted from memory
    // by the clearing of _clique_potentials_).
    _node_to_hard_ev_projected_CPTs_.clear();

    // remove all the soft evidence.
    _node_to_soft_evidence_.clear();

    // create empty potential lists into the cliques of the joint tree as well
    // as empty lists of evidence
    _ScheduleMultiDimSet_ empty_set;
    for (const auto node: *_JT_) {
      _clique_potentials_.insert(node, empty_set);
    }

    // remove all the constants created due to projections of CPTs that were
    // defined over only hard evidence nodes
    _constants_.clear();

    // create empty lists of potentials for the messages and indicate that no
    // message has been computed yet
    _separator_potentials_.clear();
    _messages_computed_.clear();
    for (const auto& edge: _JT_->edges()) {
      const Arc arc1(edge.first(), edge.second());
      _separator_potentials_.insert(arc1, empty_set);
      _messages_computed_.insert(arc1, false);
      const Arc arc2(edge.second(), edge.first());
      _separator_potentials_.insert(arc2, empty_set);
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
    double overall_size = 0;
    for (const auto clique: *_JT_) {
      double clique_size = 1.0;
      for (const auto node: _JT_->clique(clique))
        clique_size *= this->domainSizes()[node];
      overall_size += clique_size;
    }
    _use_schedules_ = (overall_size > _schedule_threshold_);

    // put all the CPTs of the Bayes net nodes into the cliques
    // here, beware: all the potentials that are defined over some nodes
    // including hard evidence must be projected so that these nodes are
    // removed from the potential
    if (_use_schedules_) {
      Schedule schedule;
      _initializeJTCliques_(schedule);
    } else {
      _initializeJTCliques_();
    }

    // we shall now add all the potentials of the soft evidence
    const NodeProperty< const Potential< GUM_SCALAR >* >& evidence = this->evidence();
    for (const auto node: this->softEvidenceNodes()) {
      auto ev_pot = new ScheduleMultiDim< Potential< GUM_SCALAR > >(*evidence[node], false);
      _node_to_soft_evidence_.insert(node, ev_pot);
      _clique_potentials_[_node_to_clique_[node]].insert(ev_pot);
    }

    // indicate that the data structures are up to date.
    _evidence_changes_.clear();
    _is_new_jt_needed_ = false;
  }


  /// put all the CPTs into the cliques when creating the JT without using a schedule
  template < typename GUM_SCALAR >
  void LazyPropagation< GUM_SCALAR >::_initializeJTCliques_() {
    const auto& bn  = this->BN();
    const DAG&  dag = bn.dag();

    // put all the CPTs of the Bayes net nodes into the cliques
    // here, beware: all the potentials that are defined over some nodes
    // including hard evidence must be projected so that these nodes are
    // removed from the potential
    const NodeProperty< const Potential< GUM_SCALAR >* >& evidence      = this->evidence();
    const NodeProperty< Idx >&                            hard_evidence = this->hardEvidence();

    for (const auto node: dag) {
      if (_graph_.exists(node) || _hard_ev_nodes_.contains(node)) {
        const Potential< GUM_SCALAR >& cpt = bn.cpt(node);

        // get the list of nodes with hard evidence in cpt
        NodeSet     hard_nodes;
        const auto& variables            = cpt.variablesSequence();
        bool        graph_contains_nodes = false;
        for (const auto var: variables) {
          NodeId xnode = bn.nodeId(*var);
          if (_hard_ev_nodes_.contains(xnode)) hard_nodes.insert(xnode);
          else if (_graph_.exists(xnode)) graph_contains_nodes = true;
        }

        // if hard_nodes contains hard evidence nodes, perform a projection
        // and insert the result into the appropriate clique, else insert
        // directly cpt into the clique
        if (hard_nodes.empty()) {
          auto sched_cpt = new ScheduleMultiDim< Potential< GUM_SCALAR > >(cpt, false);
          _clique_potentials_[_node_to_clique_[node]].insert(sched_cpt);
        } else {
          // marginalize out the hard evidence nodes: if the cpt is defined
          // only over nodes that received hard evidence, do not consider it
          // as a potential anymore but as a constant
          // TODO substitute constants by 0-dimensional potentials
          if (hard_nodes.size() == variables.size()) {
            Instantiation inst(cpt);
            for (Size i = 0; i < hard_nodes.size(); ++i) {
              inst.chgVal(variables[i], hard_evidence[bn.nodeId(*(variables[i]))]);
            }
            _constants_.insert(node, cpt.get(inst));
          } else {
            // here, we have a CPT defined over some nodes that received hard
            // evidence and other nodes that did not receive it. If none of the
            // latter belong to the graph, then the CPT is useless for inference
            if (!graph_contains_nodes) continue;

            // prepare the projection with a combine and project instance
            Set< const DiscreteVariable* > hard_variables;
            _PotentialSet_                 marg_cpt_set(1 + hard_nodes.size());
            marg_cpt_set.insert(&cpt);
            for (const auto xnode: hard_nodes) {
              marg_cpt_set.insert(evidence[xnode]);
              hard_variables.insert(&(bn.variable(xnode)));
            }

            // perform the combination of those potentials and their projection
            MultiDimCombineAndProjectDefault< Potential< GUM_SCALAR > > combine_and_project(
               _combination_op_,
               _projection_op_);

            _PotentialSet_ new_cpt_list = combine_and_project.execute(marg_cpt_set, hard_variables);

            // there should be only one potential in new_cpt_list
            if (new_cpt_list.size() != 1) {
              for (const auto pot: new_cpt_list) {
                if (!marg_cpt_set.contains(pot)) delete pot;
              }
              GUM_ERROR(FatalError,
                        "the projection of a potential containing "
                           << "hard evidence is empty!");
            }
            auto new_pot = const_cast< Potential< GUM_SCALAR >* >(*(new_cpt_list.begin()));
            auto projected_pot
               = new ScheduleMultiDim< Potential< GUM_SCALAR > >(std::move(*new_pot));
            delete new_pot;

            _clique_potentials_[_node_to_clique_[node]].insert(projected_pot);
            _node_to_hard_ev_projected_CPTs_.insert(node, projected_pot);
          }
        }
      }
    }
  }


  /// put all the CPTs into the cliques when creating the JT using a schedule
  template < typename GUM_SCALAR >
  void LazyPropagation< GUM_SCALAR >::_initializeJTCliques_(Schedule& schedule) {
    const auto& bn  = this->BN();
    const DAG&  dag = bn.dag();

    // put all the CPTs of the Bayes net nodes into the cliques
    // here, beware: all the potentials that are defined over some nodes
    // including hard evidence must be projected so that these nodes are
    // removed from the potential
    const NodeProperty< const Potential< GUM_SCALAR >* >& evidence      = this->evidence();
    const NodeProperty< Idx >&                            hard_evidence = this->hardEvidence();

    for (const auto node: dag) {
      if (_graph_.exists(node) || _hard_ev_nodes_.contains(node)) {
        const Potential< GUM_SCALAR >& cpt = bn.cpt(node);

        // get the list of nodes with hard evidence in cpt
        NodeSet     hard_nodes;
        const auto& variables            = cpt.variablesSequence();
        bool        graph_contains_nodes = false;
        for (const auto var: variables) {
          NodeId xnode = bn.nodeId(*var);
          if (_hard_ev_nodes_.contains(xnode)) hard_nodes.insert(xnode);
          else if (_graph_.exists(xnode)) graph_contains_nodes = true;
        }

        // if hard_nodes contains hard evidence nodes, perform a projection
        // and insert the result into the appropriate clique, else insert
        // directly cpt into the clique
        if (hard_nodes.empty()) {
          auto sched_cpt = new ScheduleMultiDim< Potential< GUM_SCALAR > >(cpt, false);
          _clique_potentials_[_node_to_clique_[node]].insert(sched_cpt);
        } else {
          // marginalize out the hard evidence nodes: if the cpt is defined
          // only over nodes that received hard evidence, do not consider it
          // as a potential anymore but as a constant
          // TODO substitute constants by 0-dimensional potentials
          if (hard_nodes.size() == variables.size()) {
            Instantiation inst(cpt);
            for (Size i = 0; i < hard_nodes.size(); ++i) {
              inst.chgVal(variables[i], hard_evidence[bn.nodeId(*(variables[i]))]);
            }
            _constants_.insert(node, cpt.get(inst));
          } else {
            // here, we have a CPT defined over some nodes that received hard
            // evidence and other nodes that did not receive it. If none of the
            // latter belong to the graph, then the CPT is useless for inference
            if (!graph_contains_nodes) continue;

            // prepare the projection with a combine and project instance
            Set< const DiscreteVariable* > hard_variables;
            _ScheduleMultiDimSet_          marg_cpt_set(1 + hard_nodes.size());
            const IScheduleMultiDim*       sched_cpt
               = schedule.insertTable< Potential< GUM_SCALAR > >(cpt, false);
            marg_cpt_set.insert(sched_cpt);

            for (const auto xnode: hard_nodes) {
              const IScheduleMultiDim* pot
                 = schedule.insertTable< Potential< GUM_SCALAR > >(*evidence[xnode], false);
              marg_cpt_set.insert(pot);
              hard_variables.insert(&(bn.variable(xnode)));
            }

            // perform the combination of those potentials and their projection
            MultiDimCombineAndProjectDefault< Potential< GUM_SCALAR > > combine_and_project(
               _combination_op_,
               _projection_op_);

            _ScheduleMultiDimSet_ new_cpt_list
               = combine_and_project.schedule(schedule, marg_cpt_set, hard_variables);

            // there should be only one potential in new_cpt_list
            if (new_cpt_list.size() != 1) {
              GUM_ERROR(FatalError,
                        "the projection of a potential containing "
                           << "hard evidence is empty!");
            }
            auto projected_pot = const_cast< ScheduleMultiDim< Potential< GUM_SCALAR > >* >(
               static_cast< const ScheduleMultiDim< Potential< GUM_SCALAR > >* >(
                  *new_cpt_list.begin()));
            const_cast< ScheduleOperator* >(schedule.scheduleMultiDimCreator(projected_pot))
               ->makeResultsPersistent(true);
            _clique_potentials_[_node_to_clique_[node]].insert(projected_pot);
            _node_to_hard_ev_projected_CPTs_.insert(node, projected_pot);
          }
        }
      }
    }
    this->scheduler().execute(schedule);
  }


  /// prepare the inference structures w.r.t. new targets, soft/hard evidence
  template < typename GUM_SCALAR >
  void LazyPropagation< GUM_SCALAR >::updateOutdatedStructure_() {
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
  void LazyPropagation< GUM_SCALAR >::_diffuseMessageInvalidations_(NodeId   from_id,
                                                                    NodeId   to_id,
                                                                    NodeSet& invalidated_cliques) {
    // invalidate the current clique
    invalidated_cliques.insert(to_id);

    // invalidate the current arc
    const Arc arc(from_id, to_id);
    bool&     message_computed = _messages_computed_[arc];
    if (message_computed) {
      message_computed = false;
      _separator_potentials_[arc].clear();
      if (_arc_to_created_potentials_.exists(arc)) {
        _ScheduleMultiDimSet_& arc_created_potentials = _arc_to_created_potentials_[arc];
        for (const auto pot: arc_created_potentials)
          delete pot;
        arc_created_potentials.clear();
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
  void LazyPropagation< GUM_SCALAR >::updateOutdatedPotentials_() {
    // compute the set of CPTs that were projected due to hard evidence and
    // whose hard evidence have changed, so that they need a new projection.
    // By the way, remove these CPTs since they are no more needed
    // Here only the values of the hard evidence can have changed (else a
    // fully new join tree would have been computed).
    // Note also that we know that the CPTs still contain some variable(s) after
    // the projection (else they should be constants)
    NodeSet hard_nodes_changed(_hard_ev_nodes_.size());
    for (const auto node: _hard_ev_nodes_)
      if (_evidence_changes_.exists(node)) hard_nodes_changed.insert(node);

    NodeSet     nodes_with_projected_CPTs_changed;
    const auto& bn = this->BN();
    for (auto pot_iter = _node_to_hard_ev_projected_CPTs_.beginSafe();
         pot_iter != _node_to_hard_ev_projected_CPTs_.endSafe();
         ++pot_iter) {
      for (const auto var: bn.cpt(pot_iter.key()).variablesSequence()) {
        if (hard_nodes_changed.contains(bn.nodeId(*var))) {
          nodes_with_projected_CPTs_changed.insert(pot_iter.key());
          delete pot_iter.val();
          _clique_potentials_[_node_to_clique_[pot_iter.key()]].erase(pot_iter.val());
          _node_to_hard_ev_projected_CPTs_.erase(pot_iter);
          break;
        }
      }
    }


    // invalidate all the messages that are no more correct: start from each of
    // the nodes whose soft evidence has changed and perform a diffusion from
    // the clique into which the soft evidence has been entered, indicating that
    // the messages spreading from this clique are now invalid. At the same time,
    // if there were potentials created on the arcs over which the messages were
    // sent, remove them from memory. For all the cliques that received some
    // projected CPT that should now be changed, do the same.
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
    // CPTs that were changed.
    for (const auto node: nodes_with_projected_CPTs_changed) {
      const auto clique = _node_to_clique_[node];
      invalidated_cliques.insert(clique);
      for (const auto neighbor: _JT_->neighbours(clique)) {
        _diffuseMessageInvalidations_(clique, neighbor, invalidated_cliques);
      }
    }


    // now we shall remove all the posteriors that belong to the
    // invalidated cliques. First, cope only with the nodes that did not
    // receive hard evidence since the other nodes do not belong to the
    // join tree
    for (auto iter = _target_posteriors_.beginSafe(); iter != _target_posteriors_.endSafe();
         ++iter) {
      if (_graph_.exists(iter.key())
          && (invalidated_cliques.exists(_node_to_clique_[iter.key()]))) {
        delete iter.val();
        _target_posteriors_.erase(iter);
      }
    }

    // now cope with the nodes that received hard evidence
    for (auto iter = _target_posteriors_.beginSafe(); iter != _target_posteriors_.endSafe();
         ++iter) {
      if (hard_nodes_changed.contains(iter.key())) {
        delete iter.val();
        _target_posteriors_.erase(iter);
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
    // and _clique_potentials_ and add the new soft ones
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


    // Now add the projections of the CPTs due to newly changed hard evidence:
    // if we are performing updateOutdatedPotentials_, this means that the
    // set of nodes that received hard evidence has not changed, only
    // their instantiations can have changed. So, if there is an entry
    // for node in _constants_, there will still be such an entry after
    // performing the new projections. Idem for _node_to_hard_ev_projected_CPTs_
    if (_use_schedules_) {
      Schedule schedule;
      for (const auto node: nodes_with_projected_CPTs_changed) {
        // perform the projection with a combine and project instance
        const Potential< GUM_SCALAR >& cpt       = bn.cpt(node);
        const auto&                    variables = cpt.variablesSequence();
        _ScheduleMultiDimSet_          marg_cpt_set;
        const auto sched_cpt = schedule.insertTable< Potential< GUM_SCALAR > >(cpt, false);
        marg_cpt_set.insert(sched_cpt);

        Set< const DiscreteVariable* > hard_variables;
        for (const auto var: variables) {
          NodeId xnode = bn.nodeId(*var);
          if (_hard_ev_nodes_.exists(xnode)) {
            const auto pot
               = schedule.insertTable< Potential< GUM_SCALAR > >(*evidence[xnode], false);
            marg_cpt_set.insert(pot);
            hard_variables.insert(var);
          }
        }

        // perform the combination of those potentials and their projection
        MultiDimCombineAndProjectDefault< Potential< GUM_SCALAR > > combine_and_project(
           _combination_op_,
           _projection_op_);

        _ScheduleMultiDimSet_ new_cpt_list
           = combine_and_project.schedule(schedule, marg_cpt_set, hard_variables);

        // there should be only one potential in new_cpt_list
        if (new_cpt_list.size() != 1) {
          GUM_ERROR(FatalError,
                    "the projection of a potential containing "
                       << "hard evidence is empty!");
        }
        auto projected_pot = const_cast< ScheduleMultiDim< Potential< GUM_SCALAR > >* >(
           static_cast< const ScheduleMultiDim< Potential< GUM_SCALAR > >* >(
              *new_cpt_list.begin()));
        const_cast< ScheduleOperator* >(schedule.scheduleMultiDimCreator(projected_pot))
           ->makeResultsPersistent(true);
        _clique_potentials_[_node_to_clique_[node]].insert(projected_pot);
        _node_to_hard_ev_projected_CPTs_.insert(node, projected_pot);
      }
      this->scheduler().execute(schedule);
    } else {
      for (const auto node: nodes_with_projected_CPTs_changed) {
        // perform the projection with a combine and project instance
        const Potential< GUM_SCALAR >& cpt       = bn.cpt(node);
        const auto&                    variables = cpt.variablesSequence();
        _PotentialSet_                 marg_cpt_set(1 + variables.size());
        marg_cpt_set.insert(&cpt);

        Set< const DiscreteVariable* > hard_variables;
        for (const auto var: variables) {
          NodeId xnode = bn.nodeId(*var);
          if (_hard_ev_nodes_.exists(xnode)) {
            marg_cpt_set.insert(evidence[xnode]);
            hard_variables.insert(var);
          }
        }

        // perform the combination of those potentials and their projection
        MultiDimCombineAndProjectDefault< Potential< GUM_SCALAR > > combine_and_project(
           _combination_op_,
           _projection_op_);

        _PotentialSet_ new_cpt_list = combine_and_project.execute(marg_cpt_set, hard_variables);

        // there should be only one potential in new_cpt_list
        if (new_cpt_list.size() != 1) {
          GUM_ERROR(FatalError,
                    "the projection of a potential containing "
                       << "hard evidence is empty!");
        }
        Potential< GUM_SCALAR >* sched_pot
           = const_cast< Potential< GUM_SCALAR >* >(*new_cpt_list.begin());
        auto projected_pot = new ScheduleMultiDim< Potential< GUM_SCALAR > >(std::move(*sched_pot));
        delete sched_pot;
        _clique_potentials_[_node_to_clique_[node]].insert(projected_pot);
        _node_to_hard_ev_projected_CPTs_.insert(node, projected_pot);
      }
    }

    // update the constants
    const auto& hard_evidence = this->hardEvidence();
    for (auto& node_cst: _constants_) {
      const Potential< GUM_SCALAR >& cpt       = bn.cpt(node_cst.first);
      const auto&                    variables = cpt.variablesSequence();
      Instantiation                  inst(cpt);
      for (const auto var: variables) {
        inst.chgVal(var, hard_evidence[bn.nodeId(*var)]);
      }
      node_cst.second = cpt.get(inst);
    }

    // indicate that all changes have been performed
    _evidence_changes_.clear();
  }


  /// compute a root for each connected component of  _JT_
  template < typename GUM_SCALAR >
  void LazyPropagation< GUM_SCALAR >::_computeJoinTreeRoots_() {
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
    const auto&                              bn = this->BN();
    std::size_t                              i  = 0;
    for (const auto clique_id: clique_targets) {
      const auto& clique   = _JT_->clique(clique_id);
      Size        dom_size = 1;
      for (const auto node: clique) {
        dom_size *= bn.variable(node).domainSize();
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


  // find the potentials d-connected to a set of variables
  template < typename GUM_SCALAR >
  void LazyPropagation< GUM_SCALAR >::_findRelevantPotentialsGetAll_(
     Set< const IScheduleMultiDim* >& pot_list,
     Set< const DiscreteVariable* >&  kept_vars) {}


  // find the potentials d-connected to a set of variables
  template < typename GUM_SCALAR >
  void LazyPropagation< GUM_SCALAR >::_findRelevantPotentialsWithdSeparation_(
     Set< const IScheduleMultiDim* >& pot_list,
     Set< const DiscreteVariable* >&  kept_vars) {
    // find the node ids of the kept variables
    NodeSet     kept_ids(kept_vars.size());
    const auto& bn = this->BN();
    for (const auto var: kept_vars) {
      kept_ids.insert(bn.nodeId(*var));
    }

    // determine the set of potentials d-connected with the kept variables
    NodeSet requisite_nodes;
    BayesBall::requisiteNodes(bn.dag(),
                              kept_ids,
                              this->hardEvidenceNodes(),
                              this->softEvidenceNodes(),
                              requisite_nodes);
    for (auto iter = pot_list.beginSafe(); iter != pot_list.endSafe(); ++iter) {
      const Sequence< const DiscreteVariable* >& vars  = (*iter)->variablesSequence();
      bool                                       found = false;
      for (const auto var: vars) {
        if (requisite_nodes.exists(bn.nodeId(*var))) {
          found = true;
          break;
        }
      }

      if (!found) { pot_list.erase(iter); }
    }
  }


  // find the potentials d-connected to a set of variables
  template < typename GUM_SCALAR >
  void LazyPropagation< GUM_SCALAR >::_findRelevantPotentialsWithdSeparation2_(
     Set< const IScheduleMultiDim* >& pot_list,
     Set< const DiscreteVariable* >&  kept_vars) {
    // find the node ids of the kept variables
    NodeSet     kept_ids(kept_vars.size());
    const auto& bn = this->BN();
    for (const auto var: kept_vars) {
      kept_ids.insert(bn.nodeId(*var));
    }

    // determine the set of potentials d-connected with the kept variables
    BayesBall::relevantPotentials(bn,
                                  kept_ids,
                                  this->hardEvidenceNodes(),
                                  this->softEvidenceNodes(),
                                  pot_list);
  }


  // find the potentials d-connected to a set of variables
  template < typename GUM_SCALAR >
  void LazyPropagation< GUM_SCALAR >::_findRelevantPotentialsWithdSeparation3_(
     Set< const IScheduleMultiDim* >& pot_list,
     Set< const DiscreteVariable* >&  kept_vars) {
    // find the node ids of the kept variables
    NodeSet     kept_ids(kept_vars.size());
    const auto& bn = this->BN();
    for (const auto var: kept_vars) {
      kept_ids.insert(bn.nodeId(*var));
    }

    // determine the set of potentials d-connected with the kept variables
    dSeparationAlgorithm dsep;
    dsep.relevantPotentials(bn,
                            kept_ids,
                            this->hardEvidenceNodes(),
                            this->softEvidenceNodes(),
                            pot_list);
  }


  // find the potentials d-connected to a set of variables
  template < typename GUM_SCALAR >
  void LazyPropagation< GUM_SCALAR >::_findRelevantPotentialsXX_(
     Set< const IScheduleMultiDim* >& pot_list,
     Set< const DiscreteVariable* >&  kept_vars) {
    switch (_find_relevant_potential_type_) {
      case RelevantPotentialsFinderType::DSEP_BAYESBALL_POTENTIALS:
        _findRelevantPotentialsWithdSeparation2_(pot_list, kept_vars);
        break;

      case RelevantPotentialsFinderType::DSEP_BAYESBALL_NODES:
        _findRelevantPotentialsWithdSeparation_(pot_list, kept_vars);
        break;

      case RelevantPotentialsFinderType::DSEP_KOLLER_FRIEDMAN_2009:
        _findRelevantPotentialsWithdSeparation3_(pot_list, kept_vars);
        break;

      case RelevantPotentialsFinderType::FIND_ALL:
        _findRelevantPotentialsGetAll_(pot_list, kept_vars);
        break;

      default: GUM_ERROR(FatalError, "not implemented yet")
    }
  }


  // remove barren variables using schedules
  template < typename GUM_SCALAR >
  Set< const IScheduleMultiDim* > LazyPropagation< GUM_SCALAR >::_removeBarrenVariables_(
     Schedule&                       schedule,
     _ScheduleMultiDimSet_&          pot_list,
     Set< const DiscreteVariable* >& del_vars) {
    // remove from del_vars the variables that received some evidence:
    // only those that did not receive evidence can be barren variables
    Set< const DiscreteVariable* > the_del_vars = del_vars;
    for (auto iter = the_del_vars.beginSafe(); iter != the_del_vars.endSafe(); ++iter) {
      NodeId id = this->BN().nodeId(**iter);
      if (this->hardEvidenceNodes().exists(id) || this->softEvidenceNodes().exists(id)) {
        the_del_vars.erase(iter);
      }
    }

    // assign to each random variable the set of potentials that contain it
    HashTable< const DiscreteVariable*, _ScheduleMultiDimSet_ > var2pots(the_del_vars.size());
    _ScheduleMultiDimSet_                                       empty_pot_set;
    for (const auto pot: pot_list) {
      const auto& vars = pot->variablesSequence();
      for (const auto var: vars) {
        if (the_del_vars.exists(var)) {
          if (!var2pots.exists(var)) { var2pots.insert(var, empty_pot_set); }
          var2pots[var].insert(pot);
        }
      }
    }

    // each variable with only one potential is necessarily a barren variable
    // assign to each potential with barren nodes its set of barren variables
    HashTable< const IScheduleMultiDim*, Set< const DiscreteVariable* > > pot2barren_var;
    Set< const DiscreteVariable* >                                        empty_var_set;
    for (const auto& elt: var2pots) {
      if (elt.second.size() == 1) {   // here we have a barren variable
        const IScheduleMultiDim* pot = *(elt.second.begin());
        if (!pot2barren_var.exists(pot)) { pot2barren_var.insert(pot, empty_var_set); }
        pot2barren_var[pot].insert(elt.first);   // insert the barren variable
      }
    }

    // for each potential with barren variables, marginalize them.
    // if the potential has only barren variables, simply remove them from the
    // set of potentials, else just project the potential
    MultiDimProjection< Potential< GUM_SCALAR > > projector(_projection_op_);
    _ScheduleMultiDimSet_                         projected_pots;
    for (const auto& elt: pot2barren_var) {
      // remove the current potential from pot_list as, anyway, we will change it
      const IScheduleMultiDim* pot = elt.first;
      pot_list.erase(pot);

      // check whether we need to add a projected new potential or not (i.e.,
      // whether there exist non-barren variables or not)
      if (pot->variablesSequence().size() != elt.second.size()) {
        const IScheduleMultiDim* new_pot = projector.schedule(schedule, pot, elt.second);
        // here, there is no need to enforce that new_pot is persistent since,
        // if this is needed, the function that called _removeBarrenVariables_ will
        // do it
        pot_list.insert(new_pot);
        projected_pots.insert(new_pot);
      }
    }

    return projected_pots;
  }


  // remove barren variables directly without schedules
  template < typename GUM_SCALAR >
  Set< const Potential< GUM_SCALAR >* > LazyPropagation< GUM_SCALAR >::_removeBarrenVariables_(
     _PotentialSet_&                 pot_list,
     Set< const DiscreteVariable* >& del_vars) {
    // remove from del_vars the variables that received some evidence:
    // only those that did not receive evidence can be barren variables
    Set< const DiscreteVariable* > the_del_vars = del_vars;
    for (auto iter = the_del_vars.beginSafe(); iter != the_del_vars.endSafe(); ++iter) {
      NodeId id = this->BN().nodeId(**iter);
      if (this->hardEvidenceNodes().exists(id) || this->softEvidenceNodes().exists(id)) {
        the_del_vars.erase(iter);
      }
    }

    // assign to each random variable the set of potentials that contain it
    HashTable< const DiscreteVariable*, _PotentialSet_ > var2pots;
    _PotentialSet_                                       empty_pot_set;
    for (const auto pot: pot_list) {
      const Sequence< const DiscreteVariable* >& vars = pot->variablesSequence();
      for (const auto var: vars) {
        if (the_del_vars.exists(var)) {
          if (!var2pots.exists(var)) { var2pots.insert(var, empty_pot_set); }
          var2pots[var].insert(pot);
        }
      }
    }

    // each variable with only one potential is a barren variable
    // assign to each potential with barren nodes its set of barren variables
    HashTable< const Potential< GUM_SCALAR >*, Set< const DiscreteVariable* > > pot2barren_var;
    Set< const DiscreteVariable* >                                              empty_var_set;
    for (const auto& elt: var2pots) {
      if (elt.second.size() == 1) {   // here we have a barren variable
        const Potential< GUM_SCALAR >* pot = *(elt.second.begin());
        if (!pot2barren_var.exists(pot)) { pot2barren_var.insert(pot, empty_var_set); }
        pot2barren_var[pot].insert(elt.first);   // insert the barren variable
      }
    }

    // for each potential with barren variables, marginalize them.
    // if the potential has only barren variables, simply remove them from the
    // set of potentials, else just project the potential
    MultiDimProjection< Potential< GUM_SCALAR > > projector(_projection_op_);
    _PotentialSet_                                projected_pots;
    for (const auto& elt: pot2barren_var) {
      // remove the current potential from pot_list as, anyway, we will change it
      const Potential< GUM_SCALAR >* pot = elt.first;
      pot_list.erase(pot);

      // check whether we need to add a projected new potential or not (i.e.,
      // whether there exist non-barren variables or not)
      if (pot->variablesSequence().size() != elt.second.size()) {
        const Potential< GUM_SCALAR >* new_pot = projector.execute(*pot, elt.second);
        pot_list.insert(new_pot);
        projected_pots.insert(new_pot);
      }
    }

    return projected_pots;
  }


  // performs the collect phase of Lazy Propagation using schedules
  template < typename GUM_SCALAR >
  INLINE void
     LazyPropagation< GUM_SCALAR >::_collectMessage_(Schedule& schedule, NodeId id, NodeId from) {
    for (const auto other: _JT_->neighbours(id)) {
      if ((other != from) && !_messages_computed_[Arc(other, id)])
        _collectMessage_(schedule, other, id);
    }

    if ((id != from) && !_messages_computed_[Arc(id, from)]) {
      _produceMessage_(schedule, id, from);
    }
  }


  // performs the collect phase of Lazy Propagation without schedules
  template < typename GUM_SCALAR >
  INLINE void LazyPropagation< GUM_SCALAR >::_collectMessage_(NodeId id, NodeId from) {
    for (const auto other: _JT_->neighbours(id)) {
      if ((other != from) && !_messages_computed_[Arc(other, id)]) _collectMessage_(other, id);
    }

    if ((id != from) && !_messages_computed_[Arc(id, from)]) { _produceMessage_(id, from); }
  }


  // remove variables del_vars from the list of potentials pot_list
  template < typename GUM_SCALAR >
  Set< const IScheduleMultiDim* >
     LazyPropagation< GUM_SCALAR >::_marginalizeOut_(Schedule&                       schedule,
                                                     Set< const IScheduleMultiDim* > pot_list,
                                                     Set< const DiscreteVariable* >& del_vars,
                                                     Set< const DiscreteVariable* >& kept_vars) {
    // use d-separation analysis to check which potentials shall be combined
    // _findRelevantPotentialsXX_(pot_list, kept_vars);

    // now, let's guarantee that all the potentials to be combined and projected
    // belong to the schedule
    for (const auto pot: pot_list) {
      if (!schedule.existsScheduleMultiDim(pot->id())) schedule.emplaceScheduleMultiDim(*pot);
    }

    // remove the potentials corresponding to barren variables if we want
    // to exploit barren nodes
    _ScheduleMultiDimSet_ barren_projected_potentials;
    if (_barren_nodes_type_ == FindBarrenNodesType::FIND_BARREN_NODES) {
      barren_projected_potentials = _removeBarrenVariables_(schedule, pot_list, del_vars);
    }

    // create a combine and project operator that will perform the
    // marginalization
    MultiDimCombineAndProjectDefault< Potential< GUM_SCALAR > > combine_and_project(
       _combination_op_,
       _projection_op_);
    _ScheduleMultiDimSet_ new_pot_list = combine_and_project.schedule(schedule, pot_list, del_vars);

    // remove all the potentials that were created due to projections of
    // barren nodes and that are not part of the new_pot_list: these
    // potentials were just temporary potentials
    for (auto barren_pot: barren_projected_potentials) {
      if (!new_pot_list.exists(barren_pot))
        schedule.emplaceDeletion(
           static_cast< const ScheduleMultiDim< Potential< GUM_SCALAR > >& >(*barren_pot));
    }

    return new_pot_list;
  }


  // remove variables del_vars from the list of potentials pot_list
  template < typename GUM_SCALAR >
  Set< const IScheduleMultiDim* >
     LazyPropagation< GUM_SCALAR >::_marginalizeOut_(Set< const IScheduleMultiDim* >& pot_list,
                                                     Set< const DiscreteVariable* >&  del_vars,
                                                     Set< const DiscreteVariable* >&  kept_vars) {
    Set< const Potential< GUM_SCALAR >* > xpot_list(pot_list.size());
    for (auto pot: pot_list)
      xpot_list.insert(
         &(static_cast< const ScheduleMultiDim< Potential< GUM_SCALAR > >* >(pot)->multiDim()));

    // use d-separation analysis to check which potentials shall be combined
    // _findRelevantPotentialsXX_(pot_list, kept_vars);

    // remove the potentials corresponding to barren variables if we want
    // to exploit barren nodes
    _PotentialSet_ barren_projected_potentials;
    if (_barren_nodes_type_ == FindBarrenNodesType::FIND_BARREN_NODES) {
      barren_projected_potentials = _removeBarrenVariables_(xpot_list, del_vars);
    }

    // create a combine and project operator that will perform the
    // marginalization
    MultiDimCombineAndProjectDefault< Potential< GUM_SCALAR > > combine_and_project(
       _combination_op_,
       _projection_op_);
    _PotentialSet_        xnew_pot_list = combine_and_project.execute(xpot_list, del_vars);
    _ScheduleMultiDimSet_ new_pot_list(xnew_pot_list.size());
    for (auto xpot: xnew_pot_list) {
      ScheduleMultiDim< Potential< GUM_SCALAR > >* pot;
      if (xpot_list.contains(xpot))
        pot = new ScheduleMultiDim< Potential< GUM_SCALAR > >(*xpot, false);
      else {
        pot = new ScheduleMultiDim< Potential< GUM_SCALAR > >(
           std::move(const_cast< Potential< GUM_SCALAR >& >(*xpot)));
        delete xpot;
      }
      new_pot_list.insert(pot);
    }

    // remove all the potentials that were created due to projections of
    // barren nodes and that are not part of the new_pot_list: these
    // potentials were just temporary potentials
    for (const auto barren_pot: barren_projected_potentials) {
      if (!xnew_pot_list.exists(barren_pot)) delete barren_pot;
    }

    return new_pot_list;
  }


  // creates the message sent by clique from_id to clique to_id
  template < typename GUM_SCALAR >
  void LazyPropagation< GUM_SCALAR >::_produceMessage_(Schedule& schedule,
                                                       NodeId    from_id,
                                                       NodeId    to_id) {
    // get the potentials of the clique
    _ScheduleMultiDimSet_ pot_list = _clique_potentials_[from_id];

    // add the messages sent by adjacent nodes to from_id.
    for (const auto other_id: _JT_->neighbours(from_id)) {
      if (other_id != to_id) pot_list += _separator_potentials_[Arc(other_id, from_id)];
    }

    // get the set of variables that need be removed from the potentials
    const NodeSet&                 from_clique = _JT_->clique(from_id);
    const NodeSet&                 separator   = _JT_->separator(from_id, to_id);
    Set< const DiscreteVariable* > del_vars(from_clique.size());
    Set< const DiscreteVariable* > kept_vars(separator.size());
    const auto&                    bn = this->BN();

    for (const auto node: from_clique) {
      if (!separator.contains(node)) {
        del_vars.insert(&(bn.variable(node)));
      } else {
        kept_vars.insert(&(bn.variable(node)));
      }
    }

    // pot_list now contains all the potentials to multiply and marginalize
    // => combine the messages
    _ScheduleMultiDimSet_ new_pot_list = _marginalizeOut_(schedule, pot_list, del_vars, kept_vars);

    // keep track of the newly created potentials but first replace all the
    // potentials whose values are all equal by constant potentials (nbrDim=0)
    // with this very value (as probability matrix multiplications
    // are tensorial, replacing the former potential by constants provides the
    // same computation results but speeds-up these computations)
    const Arc arc(from_id, to_id);

    if (!_arc_to_created_potentials_.exists(arc))
      _arc_to_created_potentials_.insert(arc, _ScheduleMultiDimSet_());

    for (auto iter = new_pot_list.beginSafe(); iter != new_pot_list.endSafe(); ++iter) {
      const auto pot = *iter;

      if (!pot_list.exists(pot)) {
        _arc_to_created_potentials_[arc].insert(pot);

        // do not forget to make the ScheduleMultiDim persistent
        auto op = schedule.scheduleMultiDimCreator(pot);
        if (op != nullptr) const_cast< ScheduleOperator* >(op)->makeResultsPersistent(true);
      }
    }

    _separator_potentials_[arc] = std::move(new_pot_list);
    _messages_computed_[arc]    = true;
  }


  // creates the message sent by clique from_id to clique to_id
  template < typename GUM_SCALAR >
  void LazyPropagation< GUM_SCALAR >::_produceMessage_(NodeId from_id, NodeId to_id) {
    // get the potentials of the clique
    _ScheduleMultiDimSet_ pot_list = _clique_potentials_[from_id];

    // add the messages sent by adjacent nodes to from_id.
    for (const auto other_id: _JT_->neighbours(from_id)) {
      if (other_id != to_id) pot_list += _separator_potentials_[Arc(other_id, from_id)];
    }

    // get the set of variables that need be removed from the potentials
    const NodeSet&                 from_clique = _JT_->clique(from_id);
    const NodeSet&                 separator   = _JT_->separator(from_id, to_id);
    Set< const DiscreteVariable* > del_vars(from_clique.size());
    Set< const DiscreteVariable* > kept_vars(separator.size());
    const auto&                    bn = this->BN();

    for (const auto node: from_clique) {
      if (!separator.contains(node)) {
        del_vars.insert(&(bn.variable(node)));
      } else {
        kept_vars.insert(&(bn.variable(node)));
      }
    }

    // pot_list now contains all the potentials to multiply and marginalize
    // => combine the messages
    _ScheduleMultiDimSet_ new_pot_list = _marginalizeOut_(pot_list, del_vars, kept_vars);

    // keep track of the newly created potentials but first replace all the
    // potentials whose values are all equal by constant potentials (nbrDim=0)
    // with this very value (as probability matrix multiplications
    // are tensorial, replacing the former potential by constants provides the
    // same computation results but speeds-up these computations)
    const Arc arc(from_id, to_id);

    if (!_arc_to_created_potentials_.exists(arc))
      _arc_to_created_potentials_.insert(arc, _ScheduleMultiDimSet_());

    for (const auto pot: new_pot_list) {
      if (!pot_list.exists(pot)) { _arc_to_created_potentials_[arc].insert(pot); }
    }

    _separator_potentials_[arc] = std::move(new_pot_list);
    _messages_computed_[arc]    = true;
  }


  // performs a whole inference
  template < typename GUM_SCALAR >
  INLINE void LazyPropagation< GUM_SCALAR >::makeInference_() {
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
  Potential< GUM_SCALAR >* LazyPropagation< GUM_SCALAR >::unnormalizedJointPosterior_(NodeId id) {
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
     LazyPropagation< GUM_SCALAR >::_unnormalizedJointPosterior_(Schedule& schedule, NodeId id) {
    const auto& bn = this->BN();

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
    _ScheduleMultiDimSet_ pot_list = _clique_potentials_[clique_of_id];

    // add the messages sent by adjacent nodes to targetClique
    for (const auto other: _JT_->neighbours(clique_of_id))
      pot_list += _separator_potentials_[Arc(other, clique_of_id)];

    // get the set of variables that need be removed from the potentials
    const NodeSet&                 nodes = _JT_->clique(clique_of_id);
    Set< const DiscreteVariable* > kept_vars{&(bn.variable(id))};
    Set< const DiscreteVariable* > del_vars(nodes.size());
    for (const auto node: nodes) {
      if (node != id) del_vars.insert(&(bn.variable(node)));
    }

    // pot_list now contains all the potentials to multiply and marginalize
    // => combine the messages
    _ScheduleMultiDimSet_ new_pot_list = _marginalizeOut_(schedule, pot_list, del_vars, kept_vars);
    Potential< GUM_SCALAR >*                     joint         = nullptr;
    ScheduleMultiDim< Potential< GUM_SCALAR > >* resulting_pot = nullptr;

    if (new_pot_list.size() == 1) {
      scheduler.execute(schedule);
      resulting_pot = const_cast< ScheduleMultiDim< Potential< GUM_SCALAR > >* >(
         static_cast< const ScheduleMultiDim< Potential< GUM_SCALAR > >* >(*new_pot_list.begin()));
    } else {
      MultiDimCombinationDefault< Potential< GUM_SCALAR > > fast_combination(_combination_op_);
      const IScheduleMultiDim* pot = fast_combination.schedule(schedule, new_pot_list);
      scheduler.execute(schedule);
      resulting_pot = const_cast< ScheduleMultiDim< Potential< GUM_SCALAR > >* >(
         static_cast< const ScheduleMultiDim< Potential< GUM_SCALAR > >* >(pot));
    }

    // if resulting_pot already existed, create a copy, so that we can put it into
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
                "some evidence entered into the Bayes "
                "net are incompatible (their joint proba = 0)");
    }
    return joint;
  }


  /// returns a fresh potential equal to P(1st arg,evidence)
  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >* LazyPropagation< GUM_SCALAR >::_unnormalizedJointPosterior_(NodeId id) {
    const auto& bn = this->BN();

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
    _ScheduleMultiDimSet_ pot_list = _clique_potentials_[clique_of_id];

    // add the messages sent by adjacent nodes to targetClique
    for (const auto other: _JT_->neighbours(clique_of_id))
      pot_list += _separator_potentials_[Arc(other, clique_of_id)];

    // get the set of variables that need be removed from the potentials
    const NodeSet&                 nodes = _JT_->clique(clique_of_id);
    Set< const DiscreteVariable* > kept_vars{&(bn.variable(id))};
    Set< const DiscreteVariable* > del_vars(nodes.size());
    for (const auto node: nodes) {
      if (node != id) del_vars.insert(&(bn.variable(node)));
    }

    // pot_list now contains all the potentials to multiply and marginalize
    // => combine the messages
    _ScheduleMultiDimSet_    new_pot_list = _marginalizeOut_(pot_list, del_vars, kept_vars);
    Potential< GUM_SCALAR >* joint        = nullptr;

    if (new_pot_list.size() == 1) {
      auto sched_joint = const_cast< ScheduleMultiDim< Potential< GUM_SCALAR > >* >(
         static_cast< const ScheduleMultiDim< Potential< GUM_SCALAR > >* >(
            *(new_pot_list.begin())));

      // if pot already existed, create a copy, so that we can put it into
      // the _target_posteriors_ property
      if (pot_list.exists(sched_joint)) {
        joint = new Potential< GUM_SCALAR >(sched_joint->multiDim());
      } else {
        joint = sched_joint->exportMultiDim();

        // remove the joint from new_pot_list so that it will not be
        // removed just after the else block
        delete sched_joint;
        new_pot_list.clear();
      }
    } else {
      MultiDimCombinationDefault< Potential< GUM_SCALAR > > fast_combination(_combination_op_);

      // get the potentials stored into the ScheduleMultiDims
      _PotentialSet_ xnew_pot_list(new_pot_list.size());
      for (auto xpot: new_pot_list) {
        xnew_pot_list.insert(
           &(static_cast< const ScheduleMultiDim< Potential< GUM_SCALAR > >* >(xpot)->multiDim()));
      }

      joint = fast_combination.execute(xnew_pot_list);
    }

    // remove the potentials that were created in new_pot_list
    // note that, if joint was the result of _marginalize_out_, it has been copied,
    // so we can remove safely all the elements of new_pot_list
    for (const auto pot: new_pot_list)
      if (!pot_list.exists(pot)) delete pot;

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
                "some evidence entered into the Bayes "
                "net are incompatible (their joint proba = 0)");
    }
    return joint;
  }


  /// returns the posterior of a given variable
  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >& LazyPropagation< GUM_SCALAR >::posterior_(NodeId id) {
    // check if we have already computed the posterior
    if (_target_posteriors_.exists(id)) { return *(_target_posteriors_[id]); }

    // compute the joint posterior and normalize
    auto joint = unnormalizedJointPosterior_(id);
    if (joint->sum() != 1)   // hard test for ReadOnly CPT (as aggregator)
      joint->normalize();
    _target_posteriors_.insert(id, joint);

    return *joint;
  }


  // returns the marginal a posteriori proba of a given node
  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >*
     LazyPropagation< GUM_SCALAR >::unnormalizedJointPosterior_(const NodeSet& set) {
    if (_use_schedules_) {
      Schedule schedule;
      return _unnormalizedJointPosterior_(schedule, set);
    } else {
      return _unnormalizedJointPosterior_(set);
    }
  }


  // returns the marginal a posteriori proba of a given node
  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >*
     LazyPropagation< GUM_SCALAR >::_unnormalizedJointPosterior_(Schedule&      schedule,
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

      NodeProperty< int > elim_order(Size(JT_elim_order.size()));
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
    _ScheduleMultiDimSet_ pot_list = _clique_potentials_[clique_of_set];

    // add the messages sent by adjacent nodes to targetClique
    for (const auto other: _JT_->neighbours(clique_of_set))
      pot_list += _separator_potentials_[Arc(other, clique_of_set)];

    // get the set of variables that need be removed from the potentials
    const NodeSet&                 nodes = _JT_->clique(clique_of_set);
    Set< const DiscreteVariable* > del_vars(nodes.size());
    Set< const DiscreteVariable* > kept_vars(targets.size());
    const auto&                    bn = this->BN();
    for (const auto node: nodes) {
      if (!targets.contains(node)) {
        del_vars.insert(&(bn.variable(node)));
      } else {
        kept_vars.insert(&(bn.variable(node)));
      }
    }

    // pot_list now contains all the potentials to multiply and marginalize
    // => combine the messages
    _ScheduleMultiDimSet_ new_pot_list = _marginalizeOut_(schedule, pot_list, del_vars, kept_vars);
    ScheduleMultiDim< Potential< GUM_SCALAR > >* resulting_pot = nullptr;

    if ((new_pot_list.size() == 1) && hard_ev_nodes.empty()) {
      scheduler.execute(schedule);
      resulting_pot = const_cast< ScheduleMultiDim< Potential< GUM_SCALAR > >* >(
         static_cast< const ScheduleMultiDim< Potential< GUM_SCALAR > >* >(*new_pot_list.begin()));
    } else {
      // combine all the potentials in new_pot_list with all the hard evidence
      // of the nodes in set
      for (const auto node: hard_ev_nodes) {
        auto new_pot_ev = schedule.insertTable< Potential< GUM_SCALAR > >(*evidence[node], false);
        new_pot_list.insert(new_pot_ev);
      }
      MultiDimCombinationDefault< Potential< GUM_SCALAR > > fast_combination(_combination_op_);
      const auto pot = fast_combination.schedule(schedule, new_pot_list);
      scheduler.execute(schedule);
      resulting_pot = const_cast< ScheduleMultiDim< Potential< GUM_SCALAR > >* >(
         static_cast< const ScheduleMultiDim< Potential< GUM_SCALAR > >* >(pot));
    }

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
                "some evidence entered into the Bayes "
                "net are incompatible (their joint proba = 0)");
    }

    return joint;
  }


  // returns the marginal a posteriori proba of a given node
  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >*
     LazyPropagation< GUM_SCALAR >::_unnormalizedJointPosterior_(const NodeSet& set) {
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

      NodeProperty< int > elim_order(Size(JT_elim_order.size()));
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
    _ScheduleMultiDimSet_ pot_list = _clique_potentials_[clique_of_set];

    // add the messages sent by adjacent nodes to targetClique
    for (const auto other: _JT_->neighbours(clique_of_set))
      pot_list += _separator_potentials_[Arc(other, clique_of_set)];

    // get the set of variables that need be removed from the potentials
    const NodeSet&                 nodes = _JT_->clique(clique_of_set);
    Set< const DiscreteVariable* > del_vars(nodes.size());
    Set< const DiscreteVariable* > kept_vars(targets.size());
    const auto&                    bn = this->BN();
    for (const auto node: nodes) {
      if (!targets.contains(node)) {
        del_vars.insert(&(bn.variable(node)));
      } else {
        kept_vars.insert(&(bn.variable(node)));
      }
    }

    // pot_list now contains all the potentials to multiply and marginalize
    // => combine the messages
    _ScheduleMultiDimSet_    new_pot_list = _marginalizeOut_(pot_list, del_vars, kept_vars);
    Potential< GUM_SCALAR >* joint        = nullptr;

    if ((new_pot_list.size() == 1) && hard_ev_nodes.empty()) {
      auto sched_joint = const_cast< ScheduleMultiDim< Potential< GUM_SCALAR > >* >(
         static_cast< const ScheduleMultiDim< Potential< GUM_SCALAR > >* >(*new_pot_list.begin()));

      // if pot already existed, create a copy, so that we can put it into
      // the _target_posteriors_ property
      if (pot_list.exists(sched_joint)) {
        joint = new Potential< GUM_SCALAR >(sched_joint->multiDim());
      } else {
        joint = sched_joint->exportMultiDim();

        // remove the joint from new_pot_list so that it will not be
        // removed just after the else block
        delete sched_joint;
        new_pot_list.clear();
      }
    } else {
      // combine all the potentials in new_pot_list with all the hard evidence
      // of the nodes in set
      _PotentialSet_ xnew_pot_list(new_pot_list.size());
      for (auto xpot: new_pot_list) {
        xnew_pot_list.insert(
           &(static_cast< const ScheduleMultiDim< Potential< GUM_SCALAR > >* >(xpot)->multiDim()));
      }
      for (const auto node: hard_ev_nodes) {
        xnew_pot_list.insert(evidence[node]);
      }
      MultiDimCombinationDefault< Potential< GUM_SCALAR > > fast_combination(_combination_op_);
      joint = fast_combination.execute(xnew_pot_list);
    }

    // remove the potentials that were created in new_pot_list
    // note that, if joint was the result of _marginalize_out_, it has been copied,
    // so we can remove safely all the elements of new_pot_list
    for (const auto pot: new_pot_list)
      if (!pot_list.exists(pot)) delete pot;

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
                "some evidence entered into the Bayes "
                "net are incompatible (their joint proba = 0)");
    }

    return joint;
  }


  /// returns the posterior of a given set of variables
  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
     LazyPropagation< GUM_SCALAR >::jointPosterior_(const NodeSet& set) {
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
     LazyPropagation< GUM_SCALAR >::jointPosterior_(const NodeSet& wanted_target,
                                                    const NodeSet& declared_target) {
    // check if we have already computed the posterior of wanted_target
    if (_joint_target_posteriors_.exists(wanted_target))
      return *(_joint_target_posteriors_[wanted_target]);

    // here, we will have to compute the posterior of declared_target and
    // marginalize out all the variables that do not belong to wanted_target

    // check if we have already computed the posterior of declared_target
    if (!_joint_target_posteriors_.exists(declared_target)) { jointPosterior_(declared_target); }

    // marginalize out all the variables that do not belong to wanted_target
    const auto&                    bn = this->BN();
    Set< const DiscreteVariable* > del_vars;
    for (const auto node: declared_target)
      if (!wanted_target.contains(node)) del_vars.insert(&(bn.variable(node)));
    auto pot = new Potential< GUM_SCALAR >(
       _joint_target_posteriors_[declared_target]->margSumOut(del_vars));

    // save the result into the cache
    _joint_target_posteriors_.insert(wanted_target, pot);

    return *pot;
  }


  template < typename GUM_SCALAR >
  GUM_SCALAR LazyPropagation< GUM_SCALAR >::evidenceProbability() {
    // here, we should check that  _find_relevant_potential_type_ is equal to
    // FIND_ALL. Otherwise, the computations could be wrong.
    RelevantPotentialsFinderType old_relevant_type = _find_relevant_potential_type_;

    // if the relevant potentials finder is not equal to FIND_ALL, all the
    // current computations may lead to incorrect results, so we shall
    // discard them
    if (old_relevant_type != RelevantPotentialsFinderType::FIND_ALL) {
      _find_relevant_potential_type_ = RelevantPotentialsFinderType::FIND_ALL;
      _is_new_jt_needed_             = true;
      this->setOutdatedStructureState_();
    }

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

    // put back the relevant potential type selected by the user
    _find_relevant_potential_type_ = old_relevant_type;

    return prob_ev;
  }

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
