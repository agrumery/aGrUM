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
 * @brief Implementation of Variable Elimination for inference in
 * Bayesian networks.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#  include <agrum/BN/inference/variableElimination.h>

#  include <algorithm>
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
  INLINE VariableElimination< GUM_SCALAR >::VariableElimination(
     const IBayesNet< GUM_SCALAR >* BN,
     RelevantPotentialsFinderType   relevant_type,
     FindBarrenNodesType            barren_type) :
      JointTargetedInference< GUM_SCALAR >(BN) {
    // sets the relevant potential and the barren nodes finding algorithm
    _findRelevantPotentials_
       = &VariableElimination< GUM_SCALAR >::_findRelevantPotentialsWithdSeparation2_;
    setRelevantPotentialsFinderType(relevant_type);
    setFindBarrenNodesType(barren_type);

    // create a default triangulation (the user can change it afterwards)
    _triangulation_ = new DefaultTriangulation;

    // for debugging purposes
    GUM_CONSTRUCTOR(VariableElimination);
  }


  // destructor
  template < typename GUM_SCALAR >
  INLINE VariableElimination< GUM_SCALAR >::~VariableElimination() {
    // remove the junction tree and the triangulation algorithm
    if (_JT_ != nullptr) delete _JT_;
    delete _triangulation_;
    if (_target_posterior_ != nullptr) delete _target_posterior_;

    // for debugging purposes
    GUM_DESTRUCTOR(VariableElimination);
  }


  /// set a new triangulation algorithm
  template < typename GUM_SCALAR >
  void VariableElimination< GUM_SCALAR >::setTriangulation(const Triangulation& new_triangulation) {
    delete _triangulation_;
    _triangulation_ = new_triangulation.newFactory();
  }


  /// returns the current join tree used
  template < typename GUM_SCALAR >
  INLINE const JunctionTree* VariableElimination< GUM_SCALAR >::junctionTree(NodeId id) {
    _createNewJT_(NodeSet{id});

    return _JT_;
  }


  /// sets how we determine the relevant potentials to combine
  template < typename GUM_SCALAR >
  void VariableElimination< GUM_SCALAR >::setRelevantPotentialsFinderType(
     RelevantPotentialsFinderType type) {
    if (type != _find_relevant_potential_type_) {
      switch (type) {
        case RelevantPotentialsFinderType::DSEP_BAYESBALL_POTENTIALS:
          _findRelevantPotentials_
             = &VariableElimination< GUM_SCALAR >::_findRelevantPotentialsWithdSeparation2_;
          break;

        case RelevantPotentialsFinderType::DSEP_BAYESBALL_NODES:
          _findRelevantPotentials_
             = &VariableElimination< GUM_SCALAR >::_findRelevantPotentialsWithdSeparation_;
          break;

        case RelevantPotentialsFinderType::DSEP_KOLLER_FRIEDMAN_2009:
          _findRelevantPotentials_
             = &VariableElimination< GUM_SCALAR >::_findRelevantPotentialsWithdSeparation3_;
          break;

        case RelevantPotentialsFinderType::FIND_ALL:
          _findRelevantPotentials_
             = &VariableElimination< GUM_SCALAR >::_findRelevantPotentialsGetAll_;
          break;

        default:
          GUM_ERROR(InvalidArgument,
                    "setRelevantPotentialsFinderType for type " << (unsigned int)type
                                                                << " is not implemented yet");
      }

      _find_relevant_potential_type_ = type;
    }
  }


  /// sets the operator for performing the projections
  template < typename GUM_SCALAR >
  INLINE void VariableElimination< GUM_SCALAR >::_setProjectionFunction_(Potential< GUM_SCALAR > (
     *proj)(const Potential< GUM_SCALAR >&, const Set< const DiscreteVariable* >&)) {
    _projection_op_ = proj;
  }


  /// sets the operator for performing the combinations
  template < typename GUM_SCALAR >
  INLINE void VariableElimination< GUM_SCALAR >::_setCombinationFunction_(Potential< GUM_SCALAR > (
     *comb)(const Potential< GUM_SCALAR >&, const Potential< GUM_SCALAR >&)) {
    _combination_op_ = comb;
  }


  /// sets how we determine barren nodes
  template < typename GUM_SCALAR >
  void VariableElimination< GUM_SCALAR >::setFindBarrenNodesType(FindBarrenNodesType type) {
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
    }
  }


  /// fired when a new evidence is inserted
  template < typename GUM_SCALAR >
  INLINE void VariableElimination< GUM_SCALAR >::onEvidenceAdded_(const NodeId, bool) {}


  /// fired when an evidence is removed
  template < typename GUM_SCALAR >
  INLINE void VariableElimination< GUM_SCALAR >::onEvidenceErased_(const NodeId, bool) {}


  /// fired when all the evidence are erased
  template < typename GUM_SCALAR >
  void VariableElimination< GUM_SCALAR >::onAllEvidenceErased_(bool) {}


  /// fired when an evidence is changed
  template < typename GUM_SCALAR >
  INLINE void VariableElimination< GUM_SCALAR >::onEvidenceChanged_(const NodeId, bool) {}


  /// fired after a new target is inserted
  template < typename GUM_SCALAR >
  INLINE void VariableElimination< GUM_SCALAR >::onMarginalTargetAdded_(const NodeId) {}


  /// fired before a target is removed
  template < typename GUM_SCALAR >
  INLINE void VariableElimination< GUM_SCALAR >::onMarginalTargetErased_(const NodeId) {}

  /// fired after a new Bayes net has been assigned to the engine
  template < typename GUM_SCALAR >
  INLINE void VariableElimination< GUM_SCALAR >::onModelChanged_(const GraphicalModel* bn) {}

  /// fired after a new set target is inserted
  template < typename GUM_SCALAR >
  INLINE void VariableElimination< GUM_SCALAR >::onJointTargetAdded_(const NodeSet&) {}


  /// fired before a set target is removed
  template < typename GUM_SCALAR >
  INLINE void VariableElimination< GUM_SCALAR >::onJointTargetErased_(const NodeSet&) {}


  /// fired after all the nodes of the BN are added as single targets
  template < typename GUM_SCALAR >
  INLINE void VariableElimination< GUM_SCALAR >::onAllMarginalTargetsAdded_() {}


  /// fired before a all the single_targets are removed
  template < typename GUM_SCALAR >
  INLINE void VariableElimination< GUM_SCALAR >::onAllMarginalTargetsErased_() {}


  /// fired before a all the joint_targets are removed
  template < typename GUM_SCALAR >
  INLINE void VariableElimination< GUM_SCALAR >::onAllJointTargetsErased_() {}


  /// fired before a all the single and joint_targets are removed
  template < typename GUM_SCALAR >
  INLINE void VariableElimination< GUM_SCALAR >::onAllTargetsErased_() {}


  /// create a new junction tree as well as its related data structures
  template < typename GUM_SCALAR >
  void VariableElimination< GUM_SCALAR >::_createNewJT_(const NodeSet& targets) {
    // to create the JT, we first create the moral graph of the BN in the
    // following way in order to take into account the barren nodes and the
    // nodes that received evidence:
    // 1/ we create an undirected graph containing only the nodes and no edge
    // 2/ if we take into account barren nodes, remove them from the graph
    // 3/ if we take d-separation into account, remove the d-separated nodes
    // 4/ add edges so that each node and its parents in the BN form a clique
    // 5/ add edges so that joint targets form a clique of the moral graph
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

    // 2/ if we wish to exploit barren nodes, we shall remove them from the
    // BN. To do so: we identify all the nodes that are not targets and have
    // received no evidence and such that their descendants are neither
    // targets nor evidence nodes. Such nodes can be safely discarded from
    // the BN without altering the inference output
    if (_barren_nodes_type_ == FindBarrenNodesType::FIND_BARREN_NODES) {
      // check that all the nodes are not targets, otherwise, there is no
      // barren node
      if (targets.size() != bn.size()) {
        BarrenNodesFinder finder(&(bn.dag()));
        finder.setTargets(&targets);

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
    // d-separated from our targets
    {
      NodeSet requisite_nodes;
      bool    dsep_analysis = false;
      switch (_find_relevant_potential_type_) {
        case RelevantPotentialsFinderType::DSEP_BAYESBALL_POTENTIALS:
        case RelevantPotentialsFinderType::DSEP_BAYESBALL_NODES: {
          BayesBall::requisiteNodes(bn.dag(),
                                    targets,
                                    this->hardEvidenceNodes(),
                                    this->softEvidenceNodes(),
                                    requisite_nodes);
          dsep_analysis = true;
        } break;

        case RelevantPotentialsFinderType::DSEP_KOLLER_FRIEDMAN_2009: {
          dSeparationAlgorithm dsep;
          dsep.requisiteNodes(bn.dag(),
                              targets,
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
        // parent belong to the graph. Actually, when d-separated nodes are
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

    // 5/ if targets contains several nodes, we shall add new edges into the
    // moral graph in order to ensure that there exists a clique containing
    // their joint distribution
    for (auto iter1 = targets.cbegin(); iter1 != targets.cend(); ++iter1) {
      auto iter2 = iter1;
      for (++iter2; iter2 != targets.cend(); ++iter2) {
        _graph_.addEdge(*iter1, *iter2);
      }
    }

    // 6/ remove all the nodes that received hard evidence
    const auto& hard_ev_nodes = this->hardEvidenceNodes();
    for (const auto node: hard_ev_nodes) {
      _graph_.eraseNode(node);
    }


    // now, we can compute the new junction tree.
    if (_JT_ != nullptr) delete _JT_;
    _triangulation_->setGraph(&_graph_, &(this->domainSizes()));
    const JunctionTree& triang_jt = _triangulation_->junctionTree();
    _JT_                          = new CliqueGraph(triang_jt);

    // indicate, for each node of the moral graph, a clique in _JT_ that can
    // contain its conditional probability table
    _node_to_clique_.clear();
    _clique_to_nodes_.clear();
    NodeSet emptyset(_JT_->size());
    for (auto clique: *_JT_)
      _clique_to_nodes_.insert(clique, emptyset);
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
      NodeId clique = _triangulation_->createdJunctionTreeClique(first_eliminated_node);
      _node_to_clique_.insert(node, clique);
      _clique_to_nodes_[clique].insert(node);
    }

    // do the same for the nodes that received hard evidence. Here, we only store
    // the nodes for which at least one parent belongs to _graph_ (otherwise
    // their CPT is just a constant real number).
    for (const auto node: hard_ev_nodes) {
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
        NodeId clique = _triangulation_->createdJunctionTreeClique(first_eliminated_node);
        _node_to_clique_.insert(node, clique);
        _clique_to_nodes_[clique].insert(node);
      }
    }


    // indicate a clique that contains all the nodes of targets
    _targets2clique_ = std::numeric_limits< NodeId >::max();
    {
      // note that we remove from set all the nodes that received hard evidence
      // (since they do not belong to the join tree)
      NodeId first_eliminated_node = std::numeric_limits< NodeId >::max();
      int    elim_number           = std::numeric_limits< int >::max();

      for (const auto node: targets) {
        if (!hard_ev_nodes.contains(node) && (elim_order[node] < elim_number)) {
          elim_number           = elim_order[node];
          first_eliminated_node = node;
        }
      }

      if (elim_number != std::numeric_limits< int >::max()) {
        _targets2clique_ = _triangulation_->createdJunctionTreeClique(first_eliminated_node);
      }
    }
  }


  /// prepare the inference structures w.r.t. new targets, soft/hard evidence
  template < typename GUM_SCALAR >
  void VariableElimination< GUM_SCALAR >::updateOutdatedStructure_() {}


  /// update the potentials stored in the cliques and invalidate outdated
  /// messages
  template < typename GUM_SCALAR >
  void VariableElimination< GUM_SCALAR >::updateOutdatedPotentials_() {}


  // find the potentials d-connected to a set of variables
  template < typename GUM_SCALAR >
  void VariableElimination< GUM_SCALAR >::_findRelevantPotentialsGetAll_(
     Set< const IScheduleMultiDim* >& pot_list,
     Set< const DiscreteVariable* >&  kept_vars) {}


  // find the potentials d-connected to a set of variables
  template < typename GUM_SCALAR >
  void VariableElimination< GUM_SCALAR >::_findRelevantPotentialsWithdSeparation_(
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
  void VariableElimination< GUM_SCALAR >::_findRelevantPotentialsWithdSeparation2_(
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
  void VariableElimination< GUM_SCALAR >::_findRelevantPotentialsWithdSeparation3_(
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
  void VariableElimination< GUM_SCALAR >::_findRelevantPotentialsXX_(
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
  Set< const IScheduleMultiDim* > VariableElimination< GUM_SCALAR >::_removeBarrenVariables_(
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
  Set< const Potential< GUM_SCALAR >* > VariableElimination< GUM_SCALAR >::_removeBarrenVariables_(
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


  // performs the collect phase of Variable Elimination
  template < typename GUM_SCALAR >
  Set< const IScheduleMultiDim* >
     VariableElimination< GUM_SCALAR >::_collectMessage_(Schedule& schedule,
                                                         NodeId    id,
                                                         NodeId    from) {
    // collect messages from all the neighbors
    _ScheduleMultiDimSet_ collected_messages;
    for (const auto other: _JT_->neighbours(id)) {
      if (other != from) {
        _ScheduleMultiDimSet_ message(_collectMessage_(schedule, other, id));
        collected_messages += message;
      }
    }

    // combine the collect messages with those of id's clique
    return _produceMessage_(schedule, id, from, std::move(collected_messages));
  }


  // performs the collect phase of Variable Elimination
  template < typename GUM_SCALAR >
  std::pair< Set< const Potential< GUM_SCALAR >* >, Set< const Potential< GUM_SCALAR >* > >
     VariableElimination< GUM_SCALAR >::_collectMessage_(NodeId id, NodeId from) {
    // collect messages from all the neighbors
    std::pair< _PotentialSet_, _PotentialSet_ > collected_messages;
    for (const auto other: _JT_->neighbours(id)) {
      if (other != from) {
        std::pair< _PotentialSet_, _PotentialSet_ > message(_collectMessage_(other, id));
        collected_messages.first += message.first;
        collected_messages.second += message.second;
      }
    }

    // combine the collect messages with those of id's clique
    return _produceMessage_(id, from, std::move(collected_messages));
  }


  // get the CPT + evidence of a node projected w.r.t. hard evidence
  template < typename GUM_SCALAR >
  Set< const IScheduleMultiDim* >
     VariableElimination< GUM_SCALAR >::_NodePotentials_(Schedule& schedule, NodeId node) {
    _ScheduleMultiDimSet_ res;
    const auto&           bn = this->BN();

    // get the CPT's of the node
    // beware: all the potentials that are defined over some nodes
    // including hard evidence must be projected so that these nodes are
    // removed from the potential
    // also beware that the CPT of a hard evidence node may be defined over
    // parents that do not belong to _graph_ and that are not hard evidence.
    // In this case, those parents have been removed by d-separation and it is
    // easy to show that, in this case all the parents have been removed, so
    // that the CPT does not need to be taken into account
    const auto& evidence      = this->evidence();
    const auto& hard_evidence = this->hardEvidence();
    const auto& hard_ev_nodes = this->hardEvidenceNodes();
    if (_graph_.exists(node) || hard_ev_nodes.contains(node)) {
      const Potential< GUM_SCALAR >& cpt       = bn.cpt(node);
      const auto&                    variables = cpt.variablesSequence();

      // check if the parents of a hard evidence node do not belong to _graph_
      // and are not themselves hard evidence. In this case, discard the CPT as
      // it is useless for inference (see the above comment)
      if (hard_ev_nodes.contains(node)) {
        for (const auto var: variables) {
          NodeId xnode = bn.nodeId(*var);
          if (!hard_ev_nodes.contains(xnode) && !_graph_.existsNode(xnode)) return res;
        }
      }

      // get the list of nodes with hard evidence in cpt
      NodeSet hard_nodes(variables.size());
      for (const auto var: variables) {
        NodeId xnode = bn.nodeId(*var);
        if (hard_ev_nodes.contains(xnode)) hard_nodes.insert(xnode);
      }

      // if hard_nodes contains hard evidence nodes, perform a projection
      // and insert the result into the appropriate clique, else insert
      // directly cpt into the clique
      if (hard_nodes.empty()) {
        const IScheduleMultiDim* sched_cpt
           = schedule.insertTable< Potential< GUM_SCALAR > >(cpt, false);
        res.insert(sched_cpt);
      } else {
        // marginalize out the hard evidence nodes: if the cpt is defined
        // only over nodes that received hard evidence, do not consider it
        // as a potential anymore
        if (hard_nodes.size() != variables.size()) {
          // perform the projection with a combine and project instance
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
          res.insert(projected_pot);
        }
      }

      // if the node received some soft evidence, add it
      if (evidence.exists(node) && !hard_evidence.exists(node)) {
        const IScheduleMultiDim* pot
           = schedule.insertTable< Potential< GUM_SCALAR > >(*evidence[node], false);
        res.insert(pot);
      }
    }

    return res;
  }


  // get the CPT + evidence of a node projected w.r.t. hard evidence
  template < typename GUM_SCALAR >
  std::pair< Set< const Potential< GUM_SCALAR >* >, Set< const Potential< GUM_SCALAR >* > >
     VariableElimination< GUM_SCALAR >::_NodePotentials_(NodeId node) {
    std::pair< _PotentialSet_, _PotentialSet_ > res;
    const auto&                                 bn = this->BN();

    // get the CPT's of the node
    // beware: all the potentials that are defined over some nodes
    // including hard evidence must be projected so that these nodes are
    // removed from the potential
    // also beware that the CPT of a hard evidence node may be defined over
    // parents that do not belong to _graph_ and that are not hard evidence.
    // In this case, those parents have been removed by d-separation and it is
    // easy to show that, in this case all the parents have been removed, so
    // that the CPT does not need to be taken into account
    const auto& evidence      = this->evidence();
    const auto& hard_evidence = this->hardEvidence();
    const auto& hard_ev_nodes = this->hardEvidenceNodes();
    if (_graph_.exists(node) || hard_ev_nodes.contains(node)) {
      const Potential< GUM_SCALAR >& cpt       = bn.cpt(node);
      const auto&                    variables = cpt.variablesSequence();

      // check if the parents of a hard evidence node do not belong to _graph_
      // and are not themselves hard evidence. In this case, discard the CPT as
      // it is useless for inference (see the above comment)
      if (hard_ev_nodes.contains(node)) {
        for (const auto var: variables) {
          NodeId xnode = bn.nodeId(*var);
          if (!hard_ev_nodes.contains(xnode) && !_graph_.existsNode(xnode)) return res;
        }
      }

      // get the list of nodes with hard evidence in cpt
      NodeSet hard_nodes(variables.size());
      for (const auto var: variables) {
        NodeId xnode = bn.nodeId(*var);
        if (hard_ev_nodes.contains(xnode)) hard_nodes.insert(xnode);
      }

      // if hard_nodes contains hard evidence nodes, perform a projection
      // and insert the result into the appropriate clique, else insert
      // directly cpt into the clique
      if (hard_nodes.empty()) {
        res.first.insert(&cpt);
      } else {
        // marginalize out the hard evidence nodes: if the cpt is defined
        // only over nodes that received hard evidence, do not consider it
        // as a potential anymore
        if (hard_nodes.size() != variables.size()) {
          // perform the projection with a combine and project instance
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
             VENewprojPotential);
          _PotentialSet_ new_cpt_list = combine_and_project.execute(marg_cpt_set, hard_variables);

          // there should be only one potential in new_cpt_list
          if (new_cpt_list.size() != 1) {
            // remove the CPT created to avoid memory leaks
            for (auto pot: new_cpt_list) {
              if (!marg_cpt_set.contains(pot)) delete pot;
            }
            GUM_ERROR(FatalError,
                      "the projection of a potential containing "
                         << "hard evidence is empty!");
          }
          const Potential< GUM_SCALAR >* projected_cpt = *(new_cpt_list.begin());
          res.first.insert(projected_cpt);
          res.second.insert(projected_cpt);
        }
      }

      // if the node received some soft evidence, add it
      if (evidence.exists(node) && !hard_evidence.exists(node)) {
        res.first.insert(this->evidence()[node]);
      }
    }

    return res;
  }


  // creates the message sent by clique from_id to clique to_id
  template < typename GUM_SCALAR >
  std::pair< Set< const Potential< GUM_SCALAR >* >, Set< const Potential< GUM_SCALAR >* > >
     VariableElimination< GUM_SCALAR >::_produceMessage_(
        NodeId from_id,
        NodeId to_id,
        std::pair< Set< const Potential< GUM_SCALAR >* >, Set< const Potential< GUM_SCALAR >* > >&&
           incoming_messages) {
    // get the messages sent by adjacent nodes to from_id
    std::pair< _PotentialSet_, _PotentialSet_ > pot_list(std::move(incoming_messages));

    // get the potentials of the clique
    for (const auto node: _clique_to_nodes_[from_id]) {
      auto new_pots = _NodePotentials_(node);
      pot_list.first += new_pots.first;
      pot_list.second += new_pots.second;
    }

    // if from_id = to_id: this is the endpoint of a collect
    if (!_JT_->existsEdge(from_id, to_id)) {
      return pot_list;
    } else {
      // get the set of variables that need to be removed from the potentials
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
      _PotentialSet_ new_pot_list = _marginalizeOut_(pot_list.first, del_vars, kept_vars);

      // remove the unnecessary temporary messages
      for (auto iter = pot_list.second.beginSafe(); iter != pot_list.second.endSafe(); ++iter) {
        if (!new_pot_list.contains(*iter)) {
          delete *iter;
          pot_list.second.erase(iter);
        }
      }

      // keep track of all the newly created potentials
      for (const auto pot: new_pot_list) {
        if (!pot_list.first.contains(pot)) { pot_list.second.insert(pot); }
      }

      // return the new set of potentials
      return std::pair< _PotentialSet_, _PotentialSet_ >(std::move(new_pot_list),
                                                         std::move(pot_list.second));
    }
  }


  // creates the message sent by clique from_id to clique to_id
  template < typename GUM_SCALAR >
  Set< const IScheduleMultiDim* > VariableElimination< GUM_SCALAR >::_produceMessage_(
     Schedule&                         schedule,
     NodeId                            from_id,
     NodeId                            to_id,
     Set< const IScheduleMultiDim* >&& incoming_messages) {
    // get the messages sent by adjacent nodes to from_id
    _ScheduleMultiDimSet_ pot_list(std::move(incoming_messages));

    // get the potentials of the clique
    for (const auto node: _clique_to_nodes_[from_id]) {
      pot_list += _NodePotentials_(schedule, node);
    }

    // if from_id = to_id: this is the endpoint of a collect
    if (!_JT_->existsEdge(from_id, to_id)) {
      return pot_list;
    } else {
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
      _ScheduleMultiDimSet_ new_pot_list
         = _marginalizeOut_(schedule, pot_list, del_vars, kept_vars);

      // remove the unnecessary temporary messages
      for (auto pot: pot_list) {
        if (!new_pot_list.contains(pot)) {
          const auto sched_pot
             = static_cast< const ScheduleMultiDim< Potential< GUM_SCALAR > >* >(pot);
          schedule.emplaceDeletion(*sched_pot);
        }
      }

      // return the new set of potentials
      return new_pot_list;
    }
  }


  // remove variables del_vars from the list of potentials pot_list
  template < typename GUM_SCALAR >
  Set< const Potential< GUM_SCALAR >* > VariableElimination< GUM_SCALAR >::_marginalizeOut_(
     Set< const Potential< GUM_SCALAR >* > pot_list,
     Set< const DiscreteVariable* >&       del_vars,
     Set< const DiscreteVariable* >&       kept_vars) {
    // use d-separation analysis to check which potentials shall be combined
    // _findRelevantPotentialsXX_(pot_list, kept_vars);

    // remove the potentials corresponding to barren variables if we want
    // to exploit barren nodes
    _PotentialSet_ barren_projected_potentials;
    if (_barren_nodes_type_ == FindBarrenNodesType::FIND_BARREN_NODES) {
      barren_projected_potentials = _removeBarrenVariables_(pot_list, del_vars);
    }

    // create a combine and project operator that will perform the
    // marginalization
    MultiDimCombineAndProjectDefault< Potential< GUM_SCALAR > > combine_and_project(
       _combination_op_,
       _projection_op_);
    _PotentialSet_ new_pot_list = combine_and_project.execute(pot_list, del_vars);

    // remove all the potentials that were created due to projections of
    // barren nodes and that are not part of the new_pot_list: these
    // potentials were just temporary potentials
    for (auto iter = barren_projected_potentials.beginSafe();
         iter != barren_projected_potentials.endSafe();
         ++iter) {
      if (!new_pot_list.exists(*iter)) delete *iter;
    }

    return new_pot_list;
  }


  // remove variables del_vars from the list of potentials pot_list
  template < typename GUM_SCALAR >
  Set< const IScheduleMultiDim* > VariableElimination< GUM_SCALAR >::_marginalizeOut_(
     Schedule&                       schedule,
     Set< const IScheduleMultiDim* > pot_list,
     Set< const DiscreteVariable* >& del_vars,
     Set< const DiscreteVariable* >& kept_vars) {
    // use d-separation analysis to check which potentials shall be combined
    // _findRelevantPotentialsXX_(pot_list, kept_vars);

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
    for (auto pot: barren_projected_potentials) {
      if (!new_pot_list.exists(pot)) {
        const auto sched_pot
           = static_cast< const ScheduleMultiDim< Potential< GUM_SCALAR > >* >(pot);
        schedule.emplaceDeletion(*sched_pot);
      }
    }

    return new_pot_list;
  }


  // performs a whole inference
  template < typename GUM_SCALAR >
  INLINE void VariableElimination< GUM_SCALAR >::makeInference_() {}


  /// returns a fresh potential equal to P(1st arg,evidence)
  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >*
     VariableElimination< GUM_SCALAR >::unnormalizedJointPosterior_(NodeId id) {
    // hard evidence do not belong to the join tree
    // # TODO: check for sets of inconsistent hard evidence
    if (this->hardEvidenceNodes().contains(id)) {
      return new Potential< GUM_SCALAR >(*(this->evidence()[id]));
    }

    // if we still need to perform some inference task, do it
    _createNewJT_(NodeSet{id});

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
    const bool use_schedules = (overall_size > _schedule_threshold_);

    if (use_schedules) {
      Schedule schedule;
      return _unnormalizedJointPosterior_(schedule, id);
    } else {
      return _unnormalizedJointPosterior_(id);
    }
  }


  /// returns a fresh potential equal to P(1st arg,evidence) without using schedules
  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >*
     VariableElimination< GUM_SCALAR >::_unnormalizedJointPosterior_(NodeId id) {
    const auto& bn = this->BN();

    NodeId                                      clique_of_id = _node_to_clique_[id];
    std::pair< _PotentialSet_, _PotentialSet_ > pot_list
       = _collectMessage_(clique_of_id, clique_of_id);

    // get the set of variables that need be removed from the potentials
    const NodeSet&                 nodes = _JT_->clique(clique_of_id);
    Set< const DiscreteVariable* > kept_vars{&(bn.variable(id))};
    Set< const DiscreteVariable* > del_vars(nodes.size());
    for (const auto node: nodes) {
      if (node != id) del_vars.insert(&(bn.variable(node)));
    }

    // pot_list now contains all the potentials to multiply and marginalize
    // => combine the messages
    _PotentialSet_           new_pot_list = _marginalizeOut_(pot_list.first, del_vars, kept_vars);
    Potential< GUM_SCALAR >* joint        = nullptr;

    if (new_pot_list.size() == 1) {
      joint = const_cast< Potential< GUM_SCALAR >* >(*(new_pot_list.begin()));
      // if joint already existed, create a copy, so that we can put it into
      // the _target_posterior_ property
      if (pot_list.first.exists(joint)) {
        joint = new Potential< GUM_SCALAR >(*joint);
      } else {
        // remove the joint from new_pot_list so that it will not be
        // removed just after the else block
        new_pot_list.clear();
      }
    } else {
      MultiDimCombinationDefault< Potential< GUM_SCALAR > > fast_combination(_combination_op_);
      joint = fast_combination.execute(new_pot_list);
    }

    // remove the potentials that were created in new_pot_list
    for (auto pot: new_pot_list)
      if (!pot_list.first.exists(pot)) delete pot;

    // remove all the temporary potentials created in pot_list
    for (auto pot: pot_list.second)
      delete pot;

    // check that the joint posterior is different from a 0 vector: this would
    // indicate that some hard evidence are not compatible (their joint
    // probability is equal to 0)
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


  /// returns a fresh potential equal to P(1st arg,evidence) without using schedules
  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >*
     VariableElimination< GUM_SCALAR >::_unnormalizedJointPosterior_(Schedule& schedule,
                                                                     NodeId    id) {
    const auto& bn = this->BN();

    NodeId                clique_of_id = _node_to_clique_[id];
    _ScheduleMultiDimSet_ pot_list     = _collectMessage_(schedule, clique_of_id, clique_of_id);

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

    auto& scheduler = this->scheduler();
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


  /// returns the posterior of a given variable
  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >& VariableElimination< GUM_SCALAR >::posterior_(NodeId id) {
    // compute the joint posterior and normalize
    auto joint = unnormalizedJointPosterior_(id);
    if (joint->sum() != 1)   // hard test for ReadOnly CPT (as aggregator)
      joint->normalize();

    if (_target_posterior_ != nullptr) delete _target_posterior_;
    _target_posterior_ = joint;

    return *joint;
  }


  // returns the marginal a posteriori proba of a given node
  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >*
     VariableElimination< GUM_SCALAR >::unnormalizedJointPosterior_(const NodeSet& set) {
    // hard evidence do not belong to the join tree, so extract the nodes
    // from targets that are not hard evidence
    NodeSet targets = set, hard_ev_nodes(this->hardEvidenceNodes().size());
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
      _PotentialSet_ pot_list;
      for (const auto node: set) {
        pot_list.insert(evidence[node]);
      }
      if (pot_list.size() == 1) {
        return new Potential< GUM_SCALAR >(**(pot_list.begin()));
      } else {
        MultiDimCombinationDefault< Potential< GUM_SCALAR > > fast_combination(_combination_op_);
        return fast_combination.execute(pot_list);
      }
    }

    // if we still need to perform some inference task, do it
    _createNewJT_(set);

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
    const bool use_schedules = (overall_size > _schedule_threshold_);

    if (use_schedules) {
      Schedule schedule;
      return _unnormalizedJointPosterior_(schedule, set, targets, hard_ev_nodes);
    } else {
      return _unnormalizedJointPosterior_(set, targets, hard_ev_nodes);
    }
  }


  // returns the marginal a posteriori proba of a given node
  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >*
     VariableElimination< GUM_SCALAR >::_unnormalizedJointPosterior_(const NodeSet& set,
                                                                     const NodeSet& targets,
                                                                     const NodeSet& hard_ev_nodes) {
    std::pair< _PotentialSet_, _PotentialSet_ > pot_list
       = _collectMessage_(_targets2clique_, _targets2clique_);

    // get the set of variables that need be removed from the potentials
    const NodeSet&                 nodes = _JT_->clique(_targets2clique_);
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
    _PotentialSet_           new_pot_list = _marginalizeOut_(pot_list.first, del_vars, kept_vars);
    Potential< GUM_SCALAR >* joint        = nullptr;

    if ((new_pot_list.size() == 1) && hard_ev_nodes.empty()) {
      joint = const_cast< Potential< GUM_SCALAR >* >(*(new_pot_list.begin()));
      // if pot already existed, create a copy, so that we can put it into
      // the  _target_posteriors_ property
      if (pot_list.first.exists(joint)) {
        joint = new Potential< GUM_SCALAR >(*joint);
      } else {
        // remove the joint from new_pot_list so that it will not be
        // removed just after the next else block
        new_pot_list.clear();
      }
    } else {
      // combine all the potentials in new_pot_list with all the hard evidence
      // of the nodes in set
      const auto&    evidence         = this->evidence();
      _PotentialSet_ new_new_pot_list = new_pot_list;
      for (const auto node: hard_ev_nodes) {
        new_new_pot_list.insert(evidence[node]);
      }
      MultiDimCombinationDefault< Potential< GUM_SCALAR > > fast_combination(_combination_op_);
      joint = fast_combination.execute(new_new_pot_list);
    }

    // remove the potentials that were created in new_pot_list
    for (auto pot: new_pot_list)
      if (!pot_list.first.exists(pot)) delete pot;

    // remove all the temporary potentials created in pot_list
    for (auto pot: pot_list.second)
      delete pot;

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
     VariableElimination< GUM_SCALAR >::_unnormalizedJointPosterior_(Schedule&      schedule,
                                                                     const NodeSet& set,
                                                                     const NodeSet& targets,
                                                                     const NodeSet& hard_ev_nodes) {
    _ScheduleMultiDimSet_ pot_list = _collectMessage_(schedule, _targets2clique_, _targets2clique_);

    // get the set of variables that need be removed from the potentials
    const NodeSet&                 nodes = _JT_->clique(_targets2clique_);
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
    auto&                                        scheduler     = this->scheduler();

    if ((new_pot_list.size() == 1) && hard_ev_nodes.empty()) {
      scheduler.execute(schedule);
      resulting_pot = const_cast< ScheduleMultiDim< Potential< GUM_SCALAR > >* >(
         static_cast< const ScheduleMultiDim< Potential< GUM_SCALAR > >* >(*new_pot_list.begin()));
    } else {
      // combine all the potentials in new_pot_list with all the hard evidence
      // of the nodes in set
      const auto& evidence = this->evidence();
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


  /// returns the posterior of a given set of variables
  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
     VariableElimination< GUM_SCALAR >::jointPosterior_(const NodeSet& set) {
    // compute the joint posterior and normalize
    auto joint = unnormalizedJointPosterior_(set);
    joint->normalize();

    if (_target_posterior_ != nullptr) delete _target_posterior_;
    _target_posterior_ = joint;

    return *joint;
  }


  /// returns the posterior of a given set of variables
  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
     VariableElimination< GUM_SCALAR >::jointPosterior_(const NodeSet& wanted_target,
                                                        const NodeSet& declared_target) {
    return jointPosterior_(wanted_target);
  }


} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
