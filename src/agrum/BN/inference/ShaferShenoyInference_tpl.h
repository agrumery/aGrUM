/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * Bayesian networks.
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#  include <agrum/BN/inference/ShaferShenoyInference.h>

#  include <agrum/BN/algorithms/BayesBall.h>
#  include <agrum/BN/algorithms/barrenNodesFinder.h>
#  include <agrum/BN/algorithms/dSeparation.h>
#  include <agrum/tools/graphs/algorithms/binaryJoinTreeConverterDefault.h>
#  include <agrum/tools/multidim/instantiation.h>
#  include <agrum/tools/multidim/utils/operators/multiDimCombineAndProjectDefault.h>
#  include <agrum/tools/multidim/utils/operators/multiDimProjection.h>


namespace gum {
  // default constructor
  template < typename GUM_SCALAR >
  INLINE ShaferShenoyInference< GUM_SCALAR >::ShaferShenoyInference(
     const IBayesNet< GUM_SCALAR >* BN,
     FindBarrenNodesType            barren_type,
     bool                           use_binary_join_tree) :
      JointTargetedInference< GUM_SCALAR >(BN),
      EvidenceInference< GUM_SCALAR >(BN),
      use_binary_join_tree__(use_binary_join_tree) {
    // sets the barren nodes finding algorithm
    setFindBarrenNodesType(barren_type);

    // create a default triangulation (the user can change it afterwards)
    triangulation__ = new DefaultTriangulation;

    // for debugging purposessetRequiredInference
    GUM_CONSTRUCTOR(ShaferShenoyInference);
  }


  // destructor
  template < typename GUM_SCALAR >
  INLINE ShaferShenoyInference< GUM_SCALAR >::~ShaferShenoyInference() {
    // remove all the potentials created during the last message passing
    for (const auto& pots: created_potentials__)
      for (const auto pot: pots.second)
        delete pot;

    // remove the potentials created after removing the nodes that received
    // hard evidence
    for (const auto& pot: hard_ev_projected_CPTs__)
      delete pot.second;

    // remove all the potentials in clique_ss_potential__ that do not belong
    // to clique_potentials__ : in this case, those potentials have been
    // created by combination of the corresponding list of potentials in
    // clique_potentials__. In other words, the size of this list is strictly
    // greater than 1.
    for (auto pot: clique_ss_potential__) {
      if (clique_potentials__[pot.first].size() > 1) delete pot.second;
    }

    // remove all the posteriors computed
    for (const auto& pot: target_posteriors__)
      delete pot.second;
    for (const auto& pot: joint_target_posteriors__)
      delete pot.second;

    // remove the junction tree and the triangulation algorithm
    if (JT__ != nullptr) delete JT__;
    if (junctionTree__ != nullptr) delete junctionTree__;
    delete triangulation__;

    // for debugging purposes
    GUM_DESTRUCTOR(ShaferShenoyInference);
  }


  /// set a new triangulation algorithm
  template < typename GUM_SCALAR >
  void ShaferShenoyInference< GUM_SCALAR >::setTriangulation(
     const Triangulation& new_triangulation) {
    delete triangulation__;
    triangulation__    = new_triangulation.newFactory();
    is_new_jt_needed__ = true;
    this->setOutdatedStructureState_();
  }


  /// returns the current join tree used
  template < typename GUM_SCALAR >
  INLINE const JoinTree* ShaferShenoyInference< GUM_SCALAR >::joinTree() {
    createNewJT__();

    return JT__;
  }

  /// returns the current junction tree
  template < typename GUM_SCALAR >
  INLINE const JunctionTree* ShaferShenoyInference< GUM_SCALAR >::junctionTree() {
    createNewJT__();

    return junctionTree__;
  }


  /// sets the operator for performing the projections
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyInference< GUM_SCALAR >::setProjectionFunction__(
     Potential< GUM_SCALAR >* (*proj)(const Potential< GUM_SCALAR >&,
                                      const Set< const DiscreteVariable* >&)) {
    projection_op__ = proj;
  }


  /// sets the operator for performing the combinations
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyInference< GUM_SCALAR >::setCombinationFunction__(
     Potential< GUM_SCALAR >* (*comb)(const Potential< GUM_SCALAR >&,
                                      const Potential< GUM_SCALAR >&)) {
    combination_op__ = comb;
  }


  /// invalidate all messages, posteriors and created potentials
  template < typename GUM_SCALAR >
  void ShaferShenoyInference< GUM_SCALAR >::invalidateAllMessages__() {
    // remove all the messages computed
    for (auto& potset: separator_potentials__)
      potset.second.clear();
    for (auto& mess_computed: messages_computed__)
      mess_computed.second = false;

    // remove all the created potentials
    for (const auto& potset: created_potentials__)
      for (const auto pot: potset.second)
        delete pot;

    // remove all the posteriors
    for (const auto& pot: target_posteriors__)
      delete pot.second;
    for (const auto& pot: joint_target_posteriors__)
      delete pot.second;

    // indicate that new messages need be computed
    if (this->isInferenceReady() || this->isInferenceDone())
      this->setOutdatedPotentialsState_();
  }


  /// sets how we determine barren nodes
  template < typename GUM_SCALAR >
  void ShaferShenoyInference< GUM_SCALAR >::setFindBarrenNodesType(
     FindBarrenNodesType type) {
    if (type != barren_nodes_type__) {
      // WARNING: if a new type is added here, method createJT__ should
      // certainly be updated as well, in particular its step 2.
      switch (type) {
        case FindBarrenNodesType::FIND_BARREN_NODES:
        case FindBarrenNodesType::FIND_NO_BARREN_NODES:
          break;

        default:
          GUM_ERROR(InvalidArgument,
                    "setFindBarrenNodesType for type "
                       << (unsigned int)type << " is not implemented yet");
      }

      barren_nodes_type__ = type;

      // potentially, we may need to reconstruct a junction tree
      this->setOutdatedStructureState_();
    }
  }


  /// fired when a new evidence is inserted
  template < typename GUM_SCALAR >
  INLINE void
     ShaferShenoyInference< GUM_SCALAR >::onEvidenceAdded_(const NodeId id,
                                                           bool isHardEvidence) {
    // if we have a new hard evidence, this modifies the undigraph over which
    // the join tree is created. This is also the case if id is not a node of
    // of the undigraph
    if (isHardEvidence || !graph__.exists(id))
      is_new_jt_needed__ = true;
    else {
      try {
        evidence_changes__.insert(id, EvidenceChangeType::EVIDENCE_ADDED);
      } catch (DuplicateElement&) {
        // here, the evidence change already existed. This necessarily means
        // that the current saved change is an EVIDENCE_ERASED. So if we
        // erased the evidence and added some again, this corresponds to an
        // EVIDENCE_MODIFIED
        evidence_changes__[id] = EvidenceChangeType::EVIDENCE_MODIFIED;
      }
    }
  }


  /// fired when an evidence is removed
  template < typename GUM_SCALAR >
  INLINE void
     ShaferShenoyInference< GUM_SCALAR >::onEvidenceErased_(const NodeId id,
                                                            bool isHardEvidence) {
    // if we delete a hard evidence, this modifies the undigraph over which
    // the join tree is created.
    if (isHardEvidence)
      is_new_jt_needed__ = true;
    else {
      try {
        evidence_changes__.insert(id, EvidenceChangeType::EVIDENCE_ERASED);
      } catch (DuplicateElement&) {
        // here, the evidence change already existed and it is necessarily an
        // EVIDENCE_ADDED or an EVIDENCE_MODIFIED. So, if the evidence has
        // been added and is now erased, this is similar to not having created
        // it. If the evidence was only modified, it already existed in the
        // last inference and we should now indicate that it has been removed.
        if (evidence_changes__[id] == EvidenceChangeType::EVIDENCE_ADDED)
          evidence_changes__.erase(id);
        else
          evidence_changes__[id] = EvidenceChangeType::EVIDENCE_ERASED;
      }
    }
  }


  /// fired when all the evidence are erased
  template < typename GUM_SCALAR >
  void ShaferShenoyInference< GUM_SCALAR >::onAllEvidenceErased_(
     bool has_hard_evidence) {
    if (has_hard_evidence || !this->hardEvidenceNodes().empty())
      is_new_jt_needed__ = true;
    else {
      for (const auto node: this->softEvidenceNodes()) {
        try {
          evidence_changes__.insert(node, EvidenceChangeType::EVIDENCE_ERASED);
        } catch (DuplicateElement&) {
          // here, the evidence change already existed and it is necessarily an
          // EVIDENCE_ADDED or an EVIDENCE_MODIFIED. So, if the evidence has
          // been added and is now erased, this is similar to not having created
          // it. If the evidence was only modified, it already existed in the
          // last inference and we should now indicate that it has been removed.
          if (evidence_changes__[node] == EvidenceChangeType::EVIDENCE_ADDED)
            evidence_changes__.erase(node);
          else
            evidence_changes__[node] = EvidenceChangeType::EVIDENCE_ERASED;
        }
      }
    }
  }


  /// fired when an evidence is changed
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyInference< GUM_SCALAR >::onEvidenceChanged_(
     const NodeId id,
     bool         hasChangedSoftHard) {
    if (hasChangedSoftHard)
      is_new_jt_needed__ = true;
    else {
      try {
        evidence_changes__.insert(id, EvidenceChangeType::EVIDENCE_MODIFIED);
      } catch (DuplicateElement&) {
        // here, the evidence change already existed and it is necessarily an
        // EVIDENCE_ADDED. So we should keep this state to indicate that this
        // evidence is new w.r.t. the last inference
      }
    }
  }


  /// fired after a new target is inserted
  template < typename GUM_SCALAR >
  INLINE void
     ShaferShenoyInference< GUM_SCALAR >::onMarginalTargetAdded_(const NodeId id) {
  }


  /// fired before a target is removed
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyInference< GUM_SCALAR >::onMarginalTargetErased_(
     const NodeId id) {}


  /// fired after a new set target is inserted
  template < typename GUM_SCALAR >
  INLINE void
     ShaferShenoyInference< GUM_SCALAR >::onJointTargetAdded_(const NodeSet& set) {
  }


  /// fired before a set target is removed
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyInference< GUM_SCALAR >::onJointTargetErased_(
     const NodeSet& set) {}


  /// fired after all the nodes of the BN are added as single targets
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyInference< GUM_SCALAR >::onAllMarginalTargetsAdded_() {}


  /// fired before a all the single_targets are removed
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyInference< GUM_SCALAR >::onAllMarginalTargetsErased_() {}

  /// fired after a new Bayes net has been assigned to the engine
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyInference< GUM_SCALAR >::onModelChanged_(
     const GraphicalModel* bn) {}

  /// fired before a all the joint_targets are removed
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyInference< GUM_SCALAR >::onAllJointTargetsErased_() {}


  /// fired before a all the single and joint_targets are removed
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyInference< GUM_SCALAR >::onAllTargetsErased_() {}


  // check whether a new junction tree is really needed for the next inference
  template < typename GUM_SCALAR >
  bool ShaferShenoyInference< GUM_SCALAR >::isNewJTNeeded__() const {
    // if we do not have a JT or if new_jt_needed__ is set to true, then
    // we know that we need to create a new join tree
    if ((JT__ == nullptr) || is_new_jt_needed__) return true;

    // if some some targets do not belong to the join tree and, consequently,
    // to the undigraph that was used to construct the join tree, then we need
    // to create a new JT. This situation may occur if we constructed the
    // join tree after pruning irrelevant/barren nodes from the BN)
    // however, note that the nodes that received hard evidence do not belong to
    // the graph and, therefore, should not be taken into account
    const auto& hard_ev_nodes = this->hardEvidenceNodes();
    for (const auto node: this->targets()) {
      if (!graph__.exists(node) && !hard_ev_nodes.exists(node)) return true;
    }
    for (const auto& joint_target: this->jointTargets()) {
      // here, we need to check that at least one clique contains all the
      // nodes of the joint target.
      bool containing_clique_found = false;
      for (const auto node: joint_target) {
        bool found = true;
        try {
          const NodeSet& clique = JT__->clique(node_to_clique__[node]);
          for (const auto xnode: joint_target) {
            if (!clique.contains(xnode) && !hard_ev_nodes.exists(xnode)) {
              found = false;
              break;
            }
          }
        } catch (NotFound&) { found = false; }

        if (found) {
          containing_clique_found = true;
          break;
        }
      }

      if (!containing_clique_found) return true;
    }

    // if some new evidence have been added on nodes that do not belong
    // to graph__, then we potentially have to reconstruct the join tree
    for (const auto& change: evidence_changes__) {
      if ((change.second == EvidenceChangeType::EVIDENCE_ADDED)
          && !graph__.exists(change.first))
        return true;
    }

    // here, the current JT is exactly what we need for the next inference
    return false;
  }


  /// create a new junction tree as well as its related data structures
  template < typename GUM_SCALAR >
  void ShaferShenoyInference< GUM_SCALAR >::createNewJT__() {
    // to create the JT, we first create the moral graph of the BN in the
    // following way in order to take into account the barren nodes and the
    // nodes that received evidence:
    // 1/ we create an undirected graph containing only the nodes and no edge
    // 2/ if we take into account barren nodes, remove them from the graph
    // 3/ add edges so that each node and its parents in the BN form a clique
    // 4/ add edges so that set targets are cliques of the moral graph
    // 5/ remove the nodes that received hard evidence (by step 3/, their
    //    parents are linked by edges, which is necessary for inference)
    //
    // At the end of step 5/, we have our moral graph and we can triangulate it
    // to get the new junction tree

    // 1/ create an undirected graph containing only the nodes and no edge
    const auto& bn = this->BN();
    graph__.clear();
    for (auto node: bn.dag())
      graph__.addNodeWithId(node);

    // 2/ if we wish to exploit barren nodes, we shall remove them from the
    // BN to do so: we identify all the nodes that are not targets and have
    // received no evidence and such that their descendants are neither
    // targets nor evidence nodes. Such nodes can be safely discarded from
    // the BN without altering the inference output
    if (barren_nodes_type__ == FindBarrenNodesType::FIND_BARREN_NODES) {
      // identify the barren nodes
      NodeSet target_nodes = this->targets();
      for (const auto& nodeset: this->jointTargets()) {
        target_nodes += nodeset;
      }

      // check that all the nodes are not targets, otherwise, there is no
      // barren node
      if (target_nodes.size() != bn.size()) {
        BarrenNodesFinder finder(&(bn.dag()));
        finder.setTargets(&target_nodes);

        NodeSet evidence_nodes;
        for (const auto& pair: this->evidence()) {
          evidence_nodes.insert(pair.first);
        }
        finder.setEvidence(&evidence_nodes);

        NodeSet barren_nodes = finder.barrenNodes();

        // remove the barren nodes from the moral graph
        for (const auto node: barren_nodes) {
          graph__.eraseNode(node);
        }
      }
    }

    // 3/ add edges so that each node and its parents in the BN form a clique
    for (const auto node: graph__) {
      const NodeSet& parents = bn.parents(node);
      for (auto iter1 = parents.cbegin(); iter1 != parents.cend(); ++iter1) {
        graph__.addEdge(*iter1, node);
        auto iter2 = iter1;
        for (++iter2; iter2 != parents.cend(); ++iter2) {
          graph__.addEdge(*iter1, *iter2);
        }
      }
    }

    // 4/ if there exist some joint targets, we shall add new edges into the
    // moral graph in order to ensure that there exists a clique containing
    // each joint
    for (const auto& nodeset: this->jointTargets()) {
      for (auto iter1 = nodeset.cbegin(); iter1 != nodeset.cend(); ++iter1) {
        auto iter2 = iter1;
        for (++iter2; iter2 != nodeset.cend(); ++iter2) {
          graph__.addEdge(*iter1, *iter2);
        }
      }
    }

    // 5/ remove all the nodes that received hard evidence
    hard_ev_nodes__ = this->hardEvidenceNodes();
    for (const auto node: hard_ev_nodes__) {
      graph__.eraseNode(node);
    }


    // now, we can compute the new junction tree. To speed-up computations
    // (essentially, those of a distribution phase), we construct from this
    // junction tree a binary join tree
    if (JT__ != nullptr) delete JT__;
    if (junctionTree__ != nullptr) delete junctionTree__;

    triangulation__->setGraph(&graph__, &(this->domainSizes()));
    const JunctionTree& triang_jt = triangulation__->junctionTree();
    if (use_binary_join_tree__) {
      BinaryJoinTreeConverterDefault bjt_converter;
      NodeSet                        emptyset;
      JT__ = new CliqueGraph(
         bjt_converter.convert(triang_jt, this->domainSizes(), emptyset));
    } else {
      JT__ = new CliqueGraph(triang_jt);
    }
    junctionTree__ = new CliqueGraph(triang_jt);


    // indicate, for each node of the moral graph a clique in JT__ that can
    // contain its conditional probability table
    node_to_clique__.clear();
    const std::vector< NodeId >& JT_elim_order
       = triangulation__->eliminationOrder();
    NodeProperty< int > elim_order(Size(JT_elim_order.size()));
    for (std::size_t i = std::size_t(0), size = JT_elim_order.size(); i < size;
         ++i)
      elim_order.insert(JT_elim_order[i], (int)i);
    const DAG& dag = bn.dag();
    for (const auto node: graph__) {
      // get the variables in the potential of node (and its parents)
      NodeId first_eliminated_node = node;
      int    elim_number           = elim_order[first_eliminated_node];

      for (const auto parent: dag.parents(node)) {
        if (graph__.existsNode(parent) && (elim_order[parent] < elim_number)) {
          elim_number           = elim_order[parent];
          first_eliminated_node = parent;
        }
      }

      // first_eliminated_node contains the first var (node or one of its
      // parents) eliminated => the clique created during its elimination
      // contains node and all of its parents => it can contain the potential
      // assigned to the node in the BN
      node_to_clique__.insert(
         node,
         triangulation__->createdJunctionTreeClique(first_eliminated_node));
    }

    // do the same for the nodes that received evidence. Here, we only store
    // the nodes for which at least one parent belongs to graph__ (otherwise
    // their CPT is just a constant real number).
    for (const auto node: hard_ev_nodes__) {
      // get the set of parents of the node that belong to graph__
      NodeSet pars(dag.parents(node).size());
      for (const auto par: dag.parents(node))
        if (graph__.exists(par)) pars.insert(par);

      if (!pars.empty()) {
        NodeId first_eliminated_node = *(pars.begin());
        int    elim_number           = elim_order[first_eliminated_node];

        for (const auto parent: pars) {
          if (elim_order[parent] < elim_number) {
            elim_number           = elim_order[parent];
            first_eliminated_node = parent;
          }
        }

        // first_eliminated_node contains the first var (node or one of its
        // parents) eliminated => the clique created during its elimination
        // contains node and all of its parents => it can contain the potential
        // assigned to the node in the BN
        node_to_clique__.insert(
           node,
           triangulation__->createdJunctionTreeClique(first_eliminated_node));
      }
    }

    // indicate for each joint_target a clique that contains it
    joint_target_to_clique__.clear();
    for (const auto& set: this->jointTargets()) {
      // remove from set all the nodes that received hard evidence (since they
      // do not belong to the join tree)
      NodeSet nodeset = set;
      for (const auto node: hard_ev_nodes__)
        if (nodeset.contains(node)) nodeset.erase(node);

      if (!nodeset.empty()) {
        // the clique we are looking for is the one that was created when
        // the first element of nodeset was eliminated
        NodeId first_eliminated_node = *(nodeset.begin());
        int    elim_number           = elim_order[first_eliminated_node];
        for (const auto node: nodeset) {
          if (elim_order[node] < elim_number) {
            elim_number           = elim_order[node];
            first_eliminated_node = node;
          }
        }

        joint_target_to_clique__.insert(
           set,
           triangulation__->createdJunctionTreeClique(first_eliminated_node));
      }
    }


    // compute the roots of JT__'s connected components
    computeJoinTreeRoots__();

    // remove all the Shafer-Shenoy potentials stored into the cliques
    for (const auto& xpot: clique_ss_potential__) {
      if (clique_potentials__[xpot.first].size() > 1) delete xpot.second;
    }

    // create empty potential lists into the cliques of the joint tree as well
    // as empty lists of evidence
    PotentialSet__ empty_set;
    clique_potentials__.clear();
    node_to_soft_evidence__.clear();
    for (const auto node: *JT__) {
      clique_potentials__.insert(node, empty_set);
    }

    // remove all the potentials created during the last inference
    for (auto& potlist: created_potentials__)
      for (auto pot: potlist.second)
        delete pot;
    created_potentials__.clear();

    // remove all the potentials created to take into account hard evidence
    // during the last inference
    for (auto pot_pair: hard_ev_projected_CPTs__)
      delete pot_pair.second;
    hard_ev_projected_CPTs__.clear();

    // remove all the constants created due to projections of CPTs that were
    // defined over only hard evidence nodes
    constants__.clear();

    // create empty lists of potentials for the messages and indicate that no
    // message has been computed yet
    separator_potentials__.clear();
    messages_computed__.clear();
    for (const auto& edge: JT__->edges()) {
      const Arc arc1(edge.first(), edge.second());
      separator_potentials__.insert(arc1, empty_set);
      messages_computed__.insert(arc1, false);
      const Arc arc2(Arc(edge.second(), edge.first()));
      separator_potentials__.insert(arc2, empty_set);
      messages_computed__.insert(arc2, false);
    }

    // remove all the posteriors computed so far
    for (const auto& pot: target_posteriors__)
      delete pot.second;
    target_posteriors__.clear();
    for (const auto& pot: joint_target_posteriors__)
      delete pot.second;
    joint_target_posteriors__.clear();


    // put all the CPT's of the Bayes net nodes into the cliques
    // here, beware: all the potentials that are defined over some nodes
    // including hard evidence must be projected so that these nodes are
    // removed from the potential
    const auto& evidence      = this->evidence();
    const auto& hard_evidence = this->hardEvidence();
    for (const auto node: dag) {
      if (graph__.exists(node) || hard_ev_nodes__.contains(node)) {
        const Potential< GUM_SCALAR >& cpt = bn.cpt(node);

        // get the list of nodes with hard evidence in cpt
        NodeSet     hard_nodes;
        const auto& variables = cpt.variablesSequence();
        for (const auto var: variables) {
          NodeId xnode = bn.nodeId(*var);
          if (hard_ev_nodes__.contains(xnode)) hard_nodes.insert(xnode);
        }

        // if hard_nodes contains hard evidence nodes, perform a projection
        // and insert the result into the appropriate clique, else insert
        // directly cpt into the clique
        if (hard_nodes.empty()) {
          clique_potentials__[node_to_clique__[node]].insert(&cpt);
        } else {
          // marginalize out the hard evidence nodes: if the cpt is defined
          // only over nodes that received hard evidence, do not consider it
          // as a potential anymore but as a constant
          if (hard_nodes.size() == variables.size()) {
            Instantiation inst;
            const auto&   vars = cpt.variablesSequence();
            for (auto var: vars)
              inst << *var;
            for (Size i = 0; i < hard_nodes.size(); ++i) {
              inst.chgVal(variables[i], hard_evidence[bn.nodeId(*(variables[i]))]);
            }
            constants__.insert(node, cpt[inst]);
          } else {
            // perform the projection with a combine and project instance
            Set< const DiscreteVariable* > hard_variables;
            PotentialSet__                 marg_cpt_set{&cpt};
            for (const auto xnode: hard_nodes) {
              marg_cpt_set.insert(evidence[xnode]);
              hard_variables.insert(&(bn.variable(xnode)));
            }

            // perform the combination of those potentials and their projection
            MultiDimCombineAndProjectDefault< GUM_SCALAR, Potential >
               combine_and_project(combination_op__, SSNewprojPotential);
            PotentialSet__ new_cpt_list
               = combine_and_project.combineAndProject(marg_cpt_set,
                                                       hard_variables);

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
            clique_potentials__[node_to_clique__[node]].insert(projected_cpt);
            hard_ev_projected_CPTs__.insert(node, projected_cpt);
          }
        }
      }
    }

    // we shall now add all the potentials of the soft evidence
    for (const auto node: this->softEvidenceNodes()) {
      node_to_soft_evidence__.insert(node, evidence[node]);
      clique_potentials__[node_to_clique__[node]].insert(evidence[node]);
    }

    // now, in clique_potentials__, for each clique, we have the list of
    // potentials that must be combined in order to produce the Shafer-Shenoy's
    // potential stored into the clique. So, perform this combination and
    // store the result in clique_ss_potential__
    clique_ss_potential__.clear();
    MultiDimCombinationDefault< GUM_SCALAR, Potential > fast_combination(
       combination_op__);
    for (const auto& xpotset: clique_potentials__) {
      const auto& potset = xpotset.second;
      if (potset.size() > 0) {
        // here, there will be an entry in clique_ss_potential__
        // If there is only one element in potset, this element shall be
        // stored into clique_ss_potential__, else all the elements of potset
        // shall be combined and their result shall be stored
        if (potset.size() == 1) {
          clique_ss_potential__.insert(xpotset.first, *(potset.cbegin()));
        } else {
          auto joint = fast_combination.combine(potset);
          clique_ss_potential__.insert(xpotset.first, joint);
        }
      }
    }

    // indicate that the data structures are up to date.
    evidence_changes__.clear();
    is_new_jt_needed__ = false;
  }


  /// prepare the inference structures w.r.t. new targets, soft/hard evidence
  template < typename GUM_SCALAR >
  void ShaferShenoyInference< GUM_SCALAR >::updateOutdatedStructure_() {
    // check if a new JT is really needed. If so, create it
    if (isNewJTNeeded__()) {
      createNewJT__();
    } else {
      // here, we can answer the next queries without reconstructing all the
      // junction tree. All we need to do is to indicate that we should
      // update the potentials and messages for these queries
      updateOutdatedPotentials_();
    }
  }


  /// invalidate all the messages sent from a given clique
  template < typename GUM_SCALAR >
  void ShaferShenoyInference< GUM_SCALAR >::diffuseMessageInvalidations__(
     NodeId   from_id,
     NodeId   to_id,
     NodeSet& invalidated_cliques) {
    // invalidate the current clique
    invalidated_cliques.insert(to_id);

    // invalidate the current arc
    const Arc arc(from_id, to_id);
    bool&     message_computed = messages_computed__[arc];
    if (message_computed) {
      message_computed = false;
      separator_potentials__[arc].clear();
      if (created_potentials__.exists(arc)) {
        auto& arc_created_potentials = created_potentials__[arc];
        for (auto pot: arc_created_potentials)
          delete pot;
        arc_created_potentials.clear();
      }

      // go on with the diffusion
      for (const auto node_id: JT__->neighbours(to_id)) {
        if (node_id != from_id)
          diffuseMessageInvalidations__(to_id, node_id, invalidated_cliques);
      }
    }
  }


  /// update the potentials stored in the cliques and invalidate outdated
  /// messages
  template < typename GUM_SCALAR >
  void ShaferShenoyInference< GUM_SCALAR >::updateOutdatedPotentials_() {
    // for each clique, indicate whether the potential stored into
    // clique_ss_potentials__[clique] is the result of a combination. In this
    // case, it has been allocated by the combination and will need to be
    // deallocated if its clique has been invalidated
    NodeProperty< bool > ss_potential_to_deallocate(clique_potentials__.size());
    for (auto pot_iter = clique_potentials__.cbegin();
         pot_iter != clique_potentials__.cend();
         ++pot_iter) {
      ss_potential_to_deallocate.insert(pot_iter.key(),
                                        (pot_iter.val().size() > 1));
    }

    // compute the set of CPTs that were projected due to hard evidence and
    // whose hard evidence have changed, so that they need a new projection.
    // By the way, remove these CPTs since they are no more needed
    // Here only the values of the hard evidence can have changed (else a
    // fully new join tree would have been computed).
    // Note also that we know that the CPTs still contain some variable(s) after
    // the projection (else they should be constants)
    NodeSet hard_nodes_changed(hard_ev_nodes__.size());
    for (const auto node: hard_ev_nodes__)
      if (evidence_changes__.exists(node)) hard_nodes_changed.insert(node);

    NodeSet     nodes_with_projected_CPTs_changed;
    const auto& bn = this->BN();
    for (auto pot_iter = hard_ev_projected_CPTs__.beginSafe();
         pot_iter != hard_ev_projected_CPTs__.endSafe();
         ++pot_iter) {
      for (const auto var: bn.cpt(pot_iter.key()).variablesSequence()) {
        if (hard_nodes_changed.contains(bn.nodeId(*var))) {
          nodes_with_projected_CPTs_changed.insert(pot_iter.key());
          delete pot_iter.val();
          clique_potentials__[node_to_clique__[pot_iter.key()]].erase(
             pot_iter.val());
          hard_ev_projected_CPTs__.erase(pot_iter);
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
    NodeSet invalidated_cliques(JT__->size());
    for (const auto& pair: evidence_changes__) {
      if (node_to_clique__.exists(pair.first)) {
        const auto clique = node_to_clique__[pair.first];
        invalidated_cliques.insert(clique);
        for (const auto neighbor: JT__->neighbours(clique)) {
          diffuseMessageInvalidations__(clique, neighbor, invalidated_cliques);
        }
      }
    }

    // now, add to the set of invalidated cliques those that contain projected
    // CPTs that were changed.
    for (auto node: nodes_with_projected_CPTs_changed) {
      const auto clique = node_to_clique__[node];
      invalidated_cliques.insert(clique);
      for (const auto neighbor: JT__->neighbours(clique)) {
        diffuseMessageInvalidations__(clique, neighbor, invalidated_cliques);
      }
    }

    // now that we know the cliques whose set of potentials have been changed,
    // we can discard their corresponding Shafer-Shenoy potential
    for (const auto clique: invalidated_cliques) {
      if (clique_ss_potential__.exists(clique)
          && ss_potential_to_deallocate[clique]) {
        delete clique_ss_potential__[clique];
      }
    }


    // now we shall remove all the posteriors that belong to the
    // invalidated cliques. First, cope only with the nodes that did not
    // received hard evidence since the other nodes do not belong to the
    // join tree
    for (auto iter = target_posteriors__.beginSafe();
         iter != target_posteriors__.endSafe();
         ++iter) {
      if (graph__.exists(iter.key())
          && (invalidated_cliques.exists(node_to_clique__[iter.key()]))) {
        delete iter.val();
        target_posteriors__.erase(iter);
      }
    }

    // now cope with the nodes that received hard evidence
    for (auto iter = target_posteriors__.beginSafe();
         iter != target_posteriors__.endSafe();
         ++iter) {
      if (hard_nodes_changed.contains(iter.key())) {
        delete iter.val();
        target_posteriors__.erase(iter);
      }
    }

    // finally, cope with joint targets
    for (auto iter = joint_target_posteriors__.beginSafe();
         iter != joint_target_posteriors__.endSafe();
         ++iter) {
      if (invalidated_cliques.exists(joint_target_to_clique__[iter.key()])) {
        delete iter.val();
        joint_target_posteriors__.erase(iter);
      }
    }


    // remove all the evidence that were entered into node_to_soft_evidence__
    // and clique_potentials__ and add the new soft ones
    for (auto& pot_pair: node_to_soft_evidence__) {
      clique_potentials__[node_to_clique__[pot_pair.first]].erase(pot_pair.second);
    }
    node_to_soft_evidence__.clear();

    const auto& evidence = this->evidence();
    for (const auto node: this->softEvidenceNodes()) {
      node_to_soft_evidence__.insert(node, evidence[node]);
      clique_potentials__[node_to_clique__[node]].insert(evidence[node]);
    }


    // Now add the projections of the CPTs due to newly changed hard evidence:
    // if we are performing updateOutdatedPotentials_, this means that the
    // set of nodes that received hard evidence has not been changed, only
    // their instantiations can have been changed. So, if there is an entry
    // for node in constants__, there will still be such an entry after
    // performing the new projections. Idem for hard_ev_projected_CPTs__
    for (const auto node: nodes_with_projected_CPTs_changed) {
      // perform the projection with a combine and project instance
      const Potential< GUM_SCALAR >& cpt       = bn.cpt(node);
      const auto&                    variables = cpt.variablesSequence();
      Set< const DiscreteVariable* > hard_variables;
      PotentialSet__                 marg_cpt_set{&cpt};
      for (const auto var: variables) {
        NodeId xnode = bn.nodeId(*var);
        if (hard_ev_nodes__.exists(xnode)) {
          marg_cpt_set.insert(evidence[xnode]);
          hard_variables.insert(var);
        }
      }

      // perform the combination of those potentials and their projection
      MultiDimCombineAndProjectDefault< GUM_SCALAR, Potential >
                     combine_and_project(combination_op__, SSNewprojPotential);
      PotentialSet__ new_cpt_list
         = combine_and_project.combineAndProject(marg_cpt_set, hard_variables);

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
      clique_potentials__[node_to_clique__[node]].insert(projected_cpt);
      hard_ev_projected_CPTs__.insert(node, projected_cpt);
    }

    // here, the list of potentials stored in the invalidated cliques have
    // been updated. So, now, we can combine them to produce the Shafer-Shenoy
    // potential stored into the clique
    MultiDimCombinationDefault< GUM_SCALAR, Potential > fast_combination(
       combination_op__);
    for (const auto clique: invalidated_cliques) {
      const auto& potset = clique_potentials__[clique];

      if (potset.size() > 0) {
        // here, there will be an entry in clique_ss_potential__
        // If there is only one element in potset, this element shall be
        // stored into clique_ss_potential__, else all the elements of potset
        // shall be combined and their result shall be stored
        if (potset.size() == 1) {
          clique_ss_potential__[clique] = *(potset.cbegin());
        } else {
          auto joint                    = fast_combination.combine(potset);
          clique_ss_potential__[clique] = joint;
        }
      }
    }


    // update the constants
    const auto& hard_evidence = this->hardEvidence();
    for (auto& node_cst: constants__) {
      const Potential< GUM_SCALAR >& cpt       = bn.cpt(node_cst.first);
      const auto&                    variables = cpt.variablesSequence();
      Instantiation                  inst;
      for (const auto var: variables)
        inst << *var;
      for (const auto var: variables) {
        inst.chgVal(var, hard_evidence[bn.nodeId(*var)]);
      }
      node_cst.second = cpt[inst];
    }

    // indicate that all changes have been performed
    evidence_changes__.clear();
  }


  /// compute a root for each connected component of JT__
  template < typename GUM_SCALAR >
  void ShaferShenoyInference< GUM_SCALAR >::computeJoinTreeRoots__() {
    // get the set of cliques in which we can find the targets and joint_targets
    NodeSet clique_targets;
    for (const auto node: this->targets()) {
      try {
        clique_targets.insert(node_to_clique__[node]);
      } catch (Exception&) {}
    }
    for (const auto& set: this->jointTargets()) {
      try {
        clique_targets.insert(joint_target_to_clique__[set]);
      } catch (Exception&) {}
    }

    // put in a vector these cliques and their size
    std::vector< std::pair< NodeId, Size > > possible_roots(clique_targets.size());
    const auto&                              bn = this->BN();
    std::size_t                              i  = 0;
    for (const auto clique_id: clique_targets) {
      const auto& clique   = JT__->clique(clique_id);
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
              [](const std::pair< NodeId, Size >& a,
                 const std::pair< NodeId, Size >& b) -> bool {
                return a.second < b.second;
              });

    // pick up the clique with the smallest size in each connected component
    NodeProperty< bool >                  marked = JT__->nodesProperty(false);
    std::function< void(NodeId, NodeId) > diffuse_marks
       = [&marked, &diffuse_marks, this](NodeId node, NodeId from) {
           if (!marked[node]) {
             marked[node] = true;
             for (const auto neigh: JT__->neighbours(node))
               if ((neigh != from) && !marked[neigh]) diffuse_marks(neigh, node);
           }
         };
    roots__.clear();
    for (const auto xclique: possible_roots) {
      NodeId clique = xclique.first;
      if (!marked[clique]) {
        roots__.insert(clique);
        diffuse_marks(clique, clique);
      }
    }
  }


  // performs the collect phase of Lazy Propagation
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyInference< GUM_SCALAR >::collectMessage__(NodeId id,
                                                                    NodeId from) {
    for (const auto other: JT__->neighbours(id)) {
      if ((other != from) && !messages_computed__[Arc(other, id)])
        collectMessage__(other, id);
    }

    if ((id != from) && !messages_computed__[Arc(id, from)]) {
      produceMessage__(id, from);
    }
  }


  // remove barren variables
  template < typename GUM_SCALAR >
  Set< const Potential< GUM_SCALAR >* >
     ShaferShenoyInference< GUM_SCALAR >::removeBarrenVariables__(
        PotentialSet__&                 pot_list,
        Set< const DiscreteVariable* >& del_vars) {
    // remove from del_vars the variables that received some evidence:
    // only those that did not received evidence can be barren variables
    Set< const DiscreteVariable* > the_del_vars = del_vars;
    for (auto iter = the_del_vars.beginSafe(); iter != the_del_vars.endSafe();
         ++iter) {
      NodeId id = this->BN().nodeId(**iter);
      if (this->hardEvidenceNodes().exists(id)
          || this->softEvidenceNodes().exists(id)) {
        the_del_vars.erase(iter);
      }
    }

    // assign to each random variable the set of potentials that contain it
    HashTable< const DiscreteVariable*, PotentialSet__ > var2pots;
    PotentialSet__                                       empty_pot_set;
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
    HashTable< const Potential< GUM_SCALAR >*, Set< const DiscreteVariable* > >
                                   pot2barren_var;
    Set< const DiscreteVariable* > empty_var_set;
    for (auto elt: var2pots) {
      if (elt.second.size() == 1) {   // here we have a barren variable
        const Potential< GUM_SCALAR >* pot = *(elt.second.begin());
        if (!pot2barren_var.exists(pot)) {
          pot2barren_var.insert(pot, empty_var_set);
        }
        pot2barren_var[pot].insert(elt.first);   // insert the barren variable
      }
    }

    // for each potential with barren variables, marginalize them.
    // if the potential has only barren variables, simply remove them from the
    // set of potentials, else just project the potential
    MultiDimProjection< GUM_SCALAR, Potential > projector(SSNewprojPotential);
    PotentialSet__                              projected_pots;
    for (auto elt: pot2barren_var) {
      // remove the current potential from pot_list as, anyway, we will change
      // it
      const Potential< GUM_SCALAR >* pot = elt.first;
      pot_list.erase(pot);

      // check whether we need to add a projected new potential or not (i.e.,
      // whether there exist non-barren variables or not)
      if (pot->variablesSequence().size() != elt.second.size()) {
        auto new_pot = projector.project(*pot, elt.second);
        pot_list.insert(new_pot);
        projected_pots.insert(new_pot);
      }
    }

    return projected_pots;
  }


  // remove variables del_vars from the list of potentials pot_list
  template < typename GUM_SCALAR >
  Set< const Potential< GUM_SCALAR >* >
     ShaferShenoyInference< GUM_SCALAR >::marginalizeOut__(
        Set< const Potential< GUM_SCALAR >* > pot_list,
        Set< const DiscreteVariable* >&       del_vars,
        Set< const DiscreteVariable* >&       kept_vars) {
    // remove the potentials corresponding to barren variables if we want
    // to exploit barren nodes
    PotentialSet__ barren_projected_potentials;
    if (barren_nodes_type__ == FindBarrenNodesType::FIND_BARREN_NODES) {
      barren_projected_potentials = removeBarrenVariables__(pot_list, del_vars);
    }

    // create a combine and project operator that will perform the
    // marginalization
    MultiDimCombineAndProjectDefault< GUM_SCALAR, Potential > combine_and_project(
       combination_op__,
       projection_op__);
    PotentialSet__ new_pot_list
       = combine_and_project.combineAndProject(pot_list, del_vars);

    // remove all the potentials that were created due to projections of
    // barren nodes and that are not part of the new_pot_list: these
    // potentials were just temporary potentials
    for (auto iter = barren_projected_potentials.beginSafe();
         iter != barren_projected_potentials.endSafe();
         ++iter) {
      if (!new_pot_list.exists(*iter)) delete *iter;
    }

    // remove all the potentials that have no dimension
    for (auto iter_pot = new_pot_list.beginSafe();
         iter_pot != new_pot_list.endSafe();
         ++iter_pot) {
      if ((*iter_pot)->variablesSequence().size() == 0) {
        // as we have already marginalized out variables that received evidence,
        // it may be the case that, after combining and projecting, some
        // potentials might be empty. In this case, we shall keep their
        // constant and remove them from memory
        // # TODO: keep the constants!
        delete *iter_pot;
        new_pot_list.erase(iter_pot);
      }
    }

    return new_pot_list;
  }


  // creates the message sent by clique from_id to clique to_id
  template < typename GUM_SCALAR >
  void ShaferShenoyInference< GUM_SCALAR >::produceMessage__(NodeId from_id,
                                                             NodeId to_id) {
    // get the potentials of the clique.
    PotentialSet__ pot_list;
    if (clique_ss_potential__.exists(from_id))
      pot_list.insert(clique_ss_potential__[from_id]);

    // add the messages sent by adjacent nodes to from_id
    for (const auto other_id: JT__->neighbours(from_id))
      if (other_id != to_id)
        pot_list += separator_potentials__[Arc(other_id, from_id)];

    // get the set of variables that need be removed from the potentials
    const NodeSet&                 from_clique = JT__->clique(from_id);
    const NodeSet&                 separator   = JT__->separator(from_id, to_id);
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
    PotentialSet__ new_pot_list = marginalizeOut__(pot_list, del_vars, kept_vars);

    /*
      for the moment, remove this test: due to some optimizations, some
      potentials might have all their cells greater than 1.

    // remove all the potentials that are equal to ones (as probability
    // matrix multiplications are tensorial, such potentials are useless)
    for (auto iter = new_pot_list.beginSafe(); iter != new_pot_list.endSafe();
         ++iter) {
      const auto pot = *iter;
      if (pot->variablesSequence().size() == 1) {
        bool is_all_ones = true;
        for (Instantiation inst(*pot); !inst.end(); ++inst) {
          if ((*pot)[inst] < one_minus_epsilon__) {
            is_all_ones = false;
            break;
          }
        }
        if (is_all_ones) {
          if (!pot_list.exists(pot)) delete pot;
          new_pot_list.erase(iter);
          continue;
        }
      }
    }
    */

    // if there are still potentials in new_pot_list, combine them to
    // produce the message on the separator
    const Arc arc(from_id, to_id);
    if (!new_pot_list.empty()) {
      if (new_pot_list.size() == 1) {   // there is only one potential
        // in new_pot_list, so this corresponds to our message on
        // the separator
        auto pot                    = *(new_pot_list.begin());
        separator_potentials__[arc] = std::move(new_pot_list);
        if (!pot_list.exists(pot)) {
          if (!created_potentials__.exists(arc))
            created_potentials__.insert(arc, PotentialSet__());
          created_potentials__[arc].insert(pot);
        }
      } else {
        // create the message in the separator
        MultiDimCombinationDefault< GUM_SCALAR, Potential > fast_combination(
           combination_op__);
        auto joint = fast_combination.combine(new_pot_list);
        separator_potentials__[arc].insert(joint);
        if (!created_potentials__.exists(arc))
          created_potentials__.insert(arc, PotentialSet__());
        created_potentials__[arc].insert(joint);

        // remove the temporary messages created in new_pot_list
        for (const auto pot: new_pot_list) {
          if (!pot_list.exists(pot)) { delete pot; }
        }
      }
    }

    messages_computed__[arc] = true;
  }


  // performs a whole inference
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyInference< GUM_SCALAR >::makeInference_() {
    // collect messages for all single targets
    for (const auto node: this->targets()) {
      // perform only collects in the join tree for nodes that have
      // not received hard evidence (those that received hard evidence were
      // not included into the join tree for speed-up reasons)
      if (graph__.exists(node)) {
        collectMessage__(node_to_clique__[node], node_to_clique__[node]);
      }
    }

    // collect messages for all set targets
    // by parsing  joint_target_to_clique__, we ensure that the cliques that
    // are referenced belong to the join tree (even if some of the nodes in
    // their associated joint_target do not belong to graph__)
    for (const auto set: joint_target_to_clique__)
      collectMessage__(set.second, set.second);
  }


  /// returns a fresh potential equal to P(1st arg,evidence)
  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >*
     ShaferShenoyInference< GUM_SCALAR >::unnormalizedJointPosterior_(NodeId id) {
    const auto& bn = this->BN();

    // hard evidence do not belong to the join tree
    // # TODO: check for sets of inconsistent hard evidence
    if (this->hardEvidenceNodes().contains(id)) {
      return new Potential< GUM_SCALAR >(*(this->evidence()[id]));
    }

    // if we still need to perform some inference task, do it (this should
    // already have been done by makeInference_)
    NodeId clique_of_id = node_to_clique__[id];
    collectMessage__(clique_of_id, clique_of_id);

    // now we just need to create the product of the potentials of the clique
    // containing id with the messages received by this clique and
    // marginalize out all variables except id
    PotentialSet__ pot_list;
    if (clique_ss_potential__.exists(clique_of_id))
      pot_list.insert(clique_ss_potential__[clique_of_id]);

    // add the messages sent by adjacent nodes to targetCliquxse
    for (const auto other: JT__->neighbours(clique_of_id))
      pot_list += separator_potentials__[Arc(other, clique_of_id)];

    // get the set of variables that need be removed from the potentials
    const NodeSet&                 nodes = JT__->clique(clique_of_id);
    Set< const DiscreteVariable* > kept_vars{&(bn.variable(id))};
    Set< const DiscreteVariable* > del_vars(nodes.size());
    for (const auto node: nodes) {
      if (node != id) del_vars.insert(&(bn.variable(node)));
    }

    // pot_list now contains all the potentials to multiply and marginalize
    // => combine the messages
    PotentialSet__ new_pot_list = marginalizeOut__(pot_list, del_vars, kept_vars);
    Potential< GUM_SCALAR >* joint = nullptr;

    if (new_pot_list.size() == 1) {
      joint = const_cast< Potential< GUM_SCALAR >* >(*(new_pot_list.begin()));
      // if pot already existed, create a copy, so that we can put it into
      // the target_posteriors__ property
      if (pot_list.exists(joint)) {
        joint = new Potential< GUM_SCALAR >(*joint);
      } else {
        // remove the joint from new_pot_list so that it will not be
        // removed just after the else block
        new_pot_list.clear();
      }
    } else {
      MultiDimCombinationDefault< GUM_SCALAR, Potential > fast_combination(
         combination_op__);
      joint = fast_combination.combine(new_pot_list);
    }

    // remove the potentials that were created in new_pot_list
    for (auto pot: new_pot_list)
      if (!pot_list.exists(pot)) delete pot;

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
  const Potential< GUM_SCALAR >&
     ShaferShenoyInference< GUM_SCALAR >::posterior_(NodeId id) {
    // check if we have already computed the posterior
    if (target_posteriors__.exists(id)) { return *(target_posteriors__[id]); }

    // compute the joint posterior and normalize
    auto joint = unnormalizedJointPosterior_(id);
    if (joint->sum() != 1)   // hard test for ReadOnly CPT (as aggregator)
      joint->normalize();
    target_posteriors__.insert(id, joint);

    return *joint;
  }


  // returns the marginal a posteriori proba of a given node
  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >*
     ShaferShenoyInference< GUM_SCALAR >::unnormalizedJointPosterior_(
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

    // if all the nodes have received hard evidence, then compute the
    // joint posterior directly by multiplying the hard evidence potentials
    const auto& evidence = this->evidence();
    if (targets.empty()) {
      PotentialSet__ pot_list;
      for (const auto node: set) {
        pot_list.insert(evidence[node]);
      }
      if (pot_list.size() == 1) {
        auto pot = new Potential< GUM_SCALAR >(**(pot_list.begin()));
        return pot;
      } else {
        MultiDimCombinationDefault< GUM_SCALAR, Potential > fast_combination(
           combination_op__);
        return fast_combination.combine(pot_list);
      }
    }


    // if we still need to perform some inference task, do it: so, first,
    // determine the clique on which we should perform collect to compute
    // the unnormalized joint posterior of a set of nodes containing "set"
    NodeId clique_of_set;
    try {
      clique_of_set = joint_target_to_clique__[set];
    } catch (NotFound&) {
      // here, the precise set of targets does not belong to the set of targets
      // defined by the user. So we will try to find a clique in the junction
      // tree that contains "targets":

      // 1/ we should check that all the nodes belong to the join tree
      for (const auto node: targets) {
        if (!graph__.exists(node)) {
          GUM_ERROR(UndefinedElement, node << " is not a target node")
        }
      }

      // 2/ the clique created by the first eliminated node among target is the
      // one we are looking for
      const std::vector< NodeId >& JT_elim_order
         = triangulation__->eliminationOrder();
      NodeProperty< int > elim_order(Size(JT_elim_order.size()));
      for (std::size_t i = std::size_t(0), size = JT_elim_order.size(); i < size;
           ++i)
        elim_order.insert(JT_elim_order[i], (int)i);
      NodeId first_eliminated_node = *(targets.begin());
      int    elim_number           = elim_order[first_eliminated_node];
      for (const auto node: targets) {
        if (elim_order[node] < elim_number) {
          elim_number           = elim_order[node];
          first_eliminated_node = node;
        }
      }
      clique_of_set
         = triangulation__->createdJunctionTreeClique(first_eliminated_node);

      // 3/ check that cliquee_of_set contains the all the nodes in the target
      const NodeSet& clique_nodes = JT__->clique(clique_of_set);
      for (const auto node: targets) {
        if (!clique_nodes.contains(node)) {
          GUM_ERROR(UndefinedElement, set << " is not a joint target")
        }
      }

      // add the discovered clique to joint_target_to_clique__
      joint_target_to_clique__.insert(set, clique_of_set);
    }

    // now perform a collect on the clique
    collectMessage__(clique_of_set, clique_of_set);

    // now we just need to create the product of the potentials of the clique
    // containing set with the messages received by this clique and
    // marginalize out all variables except set
    PotentialSet__ pot_list;
    if (clique_ss_potential__.exists(clique_of_set))
      pot_list.insert(clique_ss_potential__[clique_of_set]);

    // add the messages sent by adjacent nodes to targetClique
    for (const auto other: JT__->neighbours(clique_of_set))
      pot_list += separator_potentials__[Arc(other, clique_of_set)];

    // get the set of variables that need be removed from the potentials
    const NodeSet&                 nodes = JT__->clique(clique_of_set);
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
    PotentialSet__ new_pot_list = marginalizeOut__(pot_list, del_vars, kept_vars);
    Potential< GUM_SCALAR >* joint = nullptr;

    if ((new_pot_list.size() == 1) && hard_ev_nodes.empty()) {
      joint = const_cast< Potential< GUM_SCALAR >* >(*(new_pot_list.begin()));
      // if pot already existed, create a copy, so that we can put it into
      // the target_posteriors__ property
      if (pot_list.exists(joint)) {
        joint = new Potential< GUM_SCALAR >(*joint);
      } else {
        // remove the joint from new_pot_list so that it will not be
        // removed just after the next else block
        new_pot_list.clear();
      }
    } else {
      // combine all the potentials in new_pot_list with all the hard evidence
      // of the nodes in set
      PotentialSet__ new_new_pot_list = new_pot_list;
      for (const auto node: hard_ev_nodes) {
        new_new_pot_list.insert(evidence[node]);
      }
      MultiDimCombinationDefault< GUM_SCALAR, Potential > fast_combination(
         combination_op__);
      joint = fast_combination.combine(new_new_pot_list);
    }

    // remove the potentials that were created in new_pot_list
    for (auto pot: new_pot_list)
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
     ShaferShenoyInference< GUM_SCALAR >::jointPosterior_(const NodeSet& set) {
    // check if we have already computed the posterior
    if (joint_target_posteriors__.exists(set)) {
      return *(joint_target_posteriors__[set]);
    }

    // compute the joint posterior and normalize
    auto joint = unnormalizedJointPosterior_(set);
    joint->normalize();
    joint_target_posteriors__.insert(set, joint);

    return *joint;
  }


  /// returns the posterior of a given set of variables
  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
     ShaferShenoyInference< GUM_SCALAR >::jointPosterior_(
        const NodeSet& wanted_target,
        const NodeSet& declared_target) {
    // check if we have already computed the posterior of wanted_target
    if (joint_target_posteriors__.exists(wanted_target))
      return *(joint_target_posteriors__[wanted_target]);

    // here, we will have to compute the posterior of declared_target and
    // marginalize out all the variables that do not belong to wanted_target

    // check if we have already computed the posterior of declared_target
    if (!joint_target_posteriors__.exists(declared_target)) {
      jointPosterior_(declared_target);
    }

    // marginalize out all the variables that do not belong to wanted_target
    const auto&                    bn = this->BN();
    Set< const DiscreteVariable* > del_vars;
    for (const auto node: declared_target)
      if (!wanted_target.contains(node)) del_vars.insert(&(bn.variable(node)));
    auto pot = new Potential< GUM_SCALAR >(
       joint_target_posteriors__[declared_target]->margSumOut(del_vars));

    // save the result into the cache
    joint_target_posteriors__.insert(wanted_target, pot);

    return *pot;
  }


  template < typename GUM_SCALAR >
  GUM_SCALAR ShaferShenoyInference< GUM_SCALAR >::evidenceProbability() {
    // perform inference in each connected component
    this->makeInference();

    // for each connected component, select a variable X and compute the
    // joint probability of X and evidence e. Then marginalize-out X to get
    // p(e) in this connected component. Finally, multiply all the p(e) that
    // we got and the elements in  constants__. The result is the probability
    // of evidence

    GUM_SCALAR prob_ev = 1;
    for (const auto root: roots__) {
      // get a node in the clique
      NodeId                   node = *(JT__->clique(root).begin());
      Potential< GUM_SCALAR >* tmp  = unnormalizedJointPosterior_(node);
      GUM_SCALAR               sum  = 0;
      for (Instantiation iter(*tmp); !iter.end(); ++iter)
        sum += tmp->get(iter);
      prob_ev *= sum;
      delete tmp;
    }

    for (const auto& projected_cpt: constants__)
      prob_ev *= projected_cpt.second;

    return prob_ev;
  }


} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
