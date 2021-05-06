/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) et Christophe GONZALES(@AMU)
 * (@AMU) info_at_agrum_dot_org
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
 * Markov Networks.
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#  include <agrum/MN/inference/ShaferShenoyMNInference.h>

#  include <agrum/tools/graphs/algorithms/binaryJoinTreeConverterDefault.h>
#  include <agrum/tools/multidim/instantiation.h>
#  include <agrum/tools/multidim/utils/operators/multiDimCombineAndProjectDefault.h>
#  include <agrum/tools/multidim/utils/operators/multiDimProjection.h>


namespace gum {
  // default constructor
  template < typename GUM_SCALAR >
  INLINE ShaferShenoyMNInference< GUM_SCALAR >::ShaferShenoyMNInference(
     const IMarkovNet< GUM_SCALAR >* MN,
     bool                            use_binary_join_tree) :
      JointTargetedMNInference< GUM_SCALAR >(MN),
      EvidenceMNInference< GUM_SCALAR >(MN),
      use_binary_join_tree__(use_binary_join_tree) {
    // create a default triangulation (the user can change it afterwards)
    triangulation__ = new DefaultTriangulation;

    // for debugging purposes
    GUM_CONSTRUCTOR(ShaferShenoyMNInference);
  }


  // destructor
  template < typename GUM_SCALAR >
  INLINE ShaferShenoyMNInference< GUM_SCALAR >::~ShaferShenoyMNInference() {
    // remove all the potentials created during the last message passing
    for (const auto& pots: created_messages__) {
      for (const auto pot: pots.second)
        delete pot;
    }

    // remove the potentials created after removing the nodes that received
    // hard evidence
    for (const auto& pot: hard_ev_projected_factors__) {
      delete pot.second;
    }

    // remove all the potentials in clique_potentials__ that do not belong
    // to clique_potentials_list__ : in this case, those potentials have been
    // created by combination of the corresponding list of potentials in
    // clique_potentials_list__. In other words, the size of this list is strictly
    // greater than 1.
    for (auto pot: clique_potentials__) {
      if (clique_potentials_list__[pot.first].size() > 1) { delete pot.second; }
    }

    // remove all the posteriors computed
    for (const auto& pot: target_posteriors__)
      delete pot.second;
    for (const auto& pot: joint_target_posteriors__)
      delete pot.second;

    // remove the junction tree and the triangulation algorithm
    if (propagator__ != nullptr) delete propagator__;
    if (junctionTree__ != nullptr) delete junctionTree__;
    delete triangulation__;

    // for debugging purposes
    GUM_DESTRUCTOR(ShaferShenoyMNInference);
  }


  /// set a new triangulation algorithm
  template < typename GUM_SCALAR >
  void ShaferShenoyMNInference< GUM_SCALAR >::setTriangulation(
     const Triangulation& new_triangulation) {
    delete triangulation__;
    triangulation__    = new_triangulation.newFactory();
    is_new_jt_needed__ = true;
    this->setOutdatedStructureState_();
  }


  /// returns the current join tree used
  template < typename GUM_SCALAR >
  INLINE const JoinTree* ShaferShenoyMNInference< GUM_SCALAR >::joinTree() {
    createNewJT__();

    return propagator__;
  }

  /// returns the current junction tree
  template < typename GUM_SCALAR >
  INLINE const JunctionTree*
               ShaferShenoyMNInference< GUM_SCALAR >::junctionTree() {
    createNewJT__();

    return junctionTree__;
  }


  /// sets the operator for performing the projections
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyMNInference< GUM_SCALAR >::setProjectionFunction__(
     Potential< GUM_SCALAR >* (*proj)(const Potential< GUM_SCALAR >&,
                                      const Set< const DiscreteVariable* >&)) {
    projection_op__ = proj;
  }


  /// sets the operator for performing the combinations
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyMNInference< GUM_SCALAR >::setCombinationFunction__(
     Potential< GUM_SCALAR >* (*comb)(const Potential< GUM_SCALAR >&,
                                      const Potential< GUM_SCALAR >&)) {
    combination_op__ = comb;
  }


  /// invalidate all messages, posteriors and created potentials
  template < typename GUM_SCALAR >
  void ShaferShenoyMNInference< GUM_SCALAR >::invalidateAllMessages__() {
    // remove all the messages computed
    for (auto& potset: separator_potentials__)
      potset.second.clear();
    for (auto& mess_computed: messages_computed__)
      mess_computed.second = false;

    // remove all the created potentials
    for (const auto& potset: created_messages__)
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

  /// fired when a new evidence is inserted
  template < typename GUM_SCALAR >
  INLINE void
     ShaferShenoyMNInference< GUM_SCALAR >::onEvidenceAdded_(const NodeId id,
                                                             bool isHardEvidence) {
    // if we have a new hard evidence, this modifies the undigraph over which
    // the join tree is created. This is also the case if id is not a node of
    // of the undigraph
    if (isHardEvidence || !reduced_graph__.exists(id))
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
  INLINE void ShaferShenoyMNInference< GUM_SCALAR >::onEvidenceErased_(
     const NodeId id,
     bool         isHardEvidence) {
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
  void ShaferShenoyMNInference< GUM_SCALAR >::onAllEvidenceErased_(
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
  INLINE void ShaferShenoyMNInference< GUM_SCALAR >::onEvidenceChanged_(
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
  INLINE void ShaferShenoyMNInference< GUM_SCALAR >::onMarginalTargetAdded_(
     const NodeId id) {}


  /// fired before a target is removed
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyMNInference< GUM_SCALAR >::onMarginalTargetErased_(
     const NodeId id) {}


  /// fired after a new set target is inserted
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyMNInference< GUM_SCALAR >::onJointTargetAdded_(
     const NodeSet& set) {}


  /// fired before a set target is removed
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyMNInference< GUM_SCALAR >::onJointTargetErased_(
     const NodeSet& set) {}


  /// fired after all the nodes of the MN are added as single targets
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyMNInference< GUM_SCALAR >::onAllMarginalTargetsAdded_() {
  }


  /// fired before a all the single_targets are removed
  template < typename GUM_SCALAR >
  INLINE void
     ShaferShenoyMNInference< GUM_SCALAR >::onAllMarginalTargetsErased_() {}

  /// fired after a new Markov net has been assigned to the engine
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyMNInference< GUM_SCALAR >::onMarkovNetChanged_(
     const IMarkovNet< GUM_SCALAR >* mn) {}

  /// fired before a all the joint_targets are removed
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyMNInference< GUM_SCALAR >::onAllJointTargetsErased_() {}


  /// fired before a all the single and joint_targets are removed
  template < typename GUM_SCALAR >
  INLINE void ShaferShenoyMNInference< GUM_SCALAR >::onAllTargetsErased_() {}


  // check whether a new junction tree is really needed for the next inference
  template < typename GUM_SCALAR >
  bool ShaferShenoyMNInference< GUM_SCALAR >::isNewJTNeeded__() const {
    // if we do not have a JT or if new_jt_needed__ is set to true, then
    // we know that we need to create a new join tree
    if ((propagator__ == nullptr) || is_new_jt_needed__) return true;

    // if some some targets do not belong to the join tree and, consequently,
    // to the undigraph that was used to construct the join tree, then we need
    // to create a new JT. This situation may occur if we constructed the
    // join tree after pruning irrelevant/barren nodes from the MN)
    // however, note that the nodes that received hard evidence do not belong to
    // the graph and, therefore, should not be taken into account
    const auto& hard_ev_nodes = this->hardEvidenceNodes();
    for (const auto node: this->targets()) {
      if (!reduced_graph__.exists(node) && !hard_ev_nodes.exists(node))
        return true;
    }
    for (const auto& joint_target: this->jointTargets()) {
      // here, we need to check that at least one clique contains all the
      // nodes of the joint target.
      bool containing_clique_found = false;
      for (const auto node: joint_target) {
        bool found = true;
        try {
          const NodeSet& clique = propagator__->clique(node_to_clique__[node]);
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
    // to reduced_graph__, then we potentially have to reconstruct the join tree
    for (const auto& change: evidence_changes__) {
      if ((change.second == EvidenceChangeType::EVIDENCE_ADDED)
          && !reduced_graph__.exists(change.first))
        return true;
    }

    // here, the current JT is exactly what we need for the next inference
    return false;
  }


  /// create a new junction tree as well as its related data structures
  template < typename GUM_SCALAR >
  void ShaferShenoyMNInference< GUM_SCALAR >::createNewJT__() {
    // to create the JT, we first create the moral graph of the MN in the
    // following way in order to take into account the barren nodes and the
    // nodes that received evidence:
    // 1/ we create an undirected graph containing only the nodes and no edge
    // 2/ add edges so that set targets are cliques of the moral graph
    // 3/ remove the nodes that received hard evidence (by step 3/, their
    //    parents are linked by edges, which is necessary for inference)
    //
    // At the end of step 3/, we have our moral graph and we can triangulate it
    // to get the new junction tree

    // 1/ create an undirected graph containing only the nodes and no edge
    const auto& mn  = this->MN();
    reduced_graph__ = mn.graph();

    // 2/ if there exists some joint targets, we shall add new edges into the
    // moral graph in order to ensure that there exists a clique containing
    // each joint
    for (const auto& nodeset: this->jointTargets())
      for (const auto n1: nodeset)
        for (const auto n2: nodeset)
          if (n1 < n2) reduced_graph__.addEdge(n1, n2);

    // 3/ remove all the nodes that received hard evidence
    hard_ev_nodes__ = this->hardEvidenceNodes();
    for (const auto node: hard_ev_nodes__) {
      reduced_graph__.eraseNode(node);
    }

    // now, we can compute the new junction tree. To speed-up computations
    // (essentially, those of a distribution phase), we construct from this
    // junction tree a binary join tree
    if (propagator__ != nullptr) delete propagator__;
    if (junctionTree__ != nullptr) delete junctionTree__;

    triangulation__->setGraph(&reduced_graph__, &(this->domainSizes()));
    const JunctionTree& triang_jt = triangulation__->junctionTree();
    if (use_binary_join_tree__) {
      BinaryJoinTreeConverterDefault bjt_converter;
      NodeSet                        emptyset;
      propagator__ = new CliqueGraph(
         bjt_converter.convert(triang_jt, this->domainSizes(), emptyset));
    } else {
      propagator__ = new CliqueGraph(triang_jt);
    }
    junctionTree__ = new CliqueGraph(triang_jt);

    const std::vector< NodeId >& JT_elim_order
       = triangulation__->eliminationOrder();
    Size                size_elim_order = JT_elim_order.size();
    NodeProperty< Idx > elim_order(size_elim_order);
    for (Idx i = Idx(0); i < size_elim_order; ++i)
      elim_order.insert(JT_elim_order[i], i);

    // indicate, for each factor of the markov network a clique in propagator__
    // that can contain its conditional probability table
    factor_to_clique__.clear();
    // TO REMOVE const UndiGraph& graph = mn.graph();
    for (const auto& kv: mn.factors()) {
      const auto& factor = kv.first;   // kv.second is the Potential()
      // not a true id since smallest_elim_number==size_elim_order+100
      NodeId first_eliminated_node = 0;
      // (impossible smallest_elim_number \in [0...size_elim_order-1]
      Idx smallest_elim_number = size_elim_order;
      for (const auto nod: factor) {
        if (reduced_graph__.exists(nod)) {
          if (elim_order[nod] < smallest_elim_number) {
            smallest_elim_number  = elim_order[nod];
            first_eliminated_node = nod;
          }
        }
      }

      if (smallest_elim_number != size_elim_order) {
        // first_eliminated_node contains the first var (node or one of its
        // parents) eliminated => the clique created during its elimination
        // contains node and all of its parents => it can contain the potential
        // assigned to the node in the MN
        factor_to_clique__.insert(
           factor,
           triangulation__->createdJunctionTreeClique(first_eliminated_node));
      }
    }

    // attribute a clique in the joint tree MN for each node in the MN
    // (using a NodeProperty instead of using mn.smallestFactorFromNode()
    // could it have a better choice ?)
    node_to_clique__.clear();
    for (const NodeId node: mn.nodes()) {
      if (reduced_graph__.exists(node)) {
        node_to_clique__.insert(
           node,
           factor_to_clique__[mn.smallestFactorFromNode(node)]);
      }
    }

    // indicate for each joint_target a clique that contains it
    joint_target_to_clique__.clear();
    for (const auto& target: this->jointTargets()) {
      // remove from target all the nodes that received hard evidence (since they
      // do not belong to the join tree)
      NodeSet nodeset = target;
      for (const auto node: hard_ev_nodes__)
        if (nodeset.contains(node)) nodeset.erase(node);

      if (!nodeset.empty()) {
        // not a true id since smallest_elim_number==size_elim_order
        NodeId first_eliminated_node = 0;
        // (smallest_elim_number \in [0...size_elim_order-1])
        Idx smallest_elim_number = size_elim_order;
        for (const auto nod: nodeset) {
          if (reduced_graph__.exists(nod)) {
            if (elim_order[nod] < smallest_elim_number) {
              smallest_elim_number  = elim_order[nod];
              first_eliminated_node = nod;
            }
          }
        }

        if (smallest_elim_number != size_elim_order) {
          // first_eliminated_node contains the first var (node or one of its
          // parents) eliminated => the clique created during its elimination
          // contains node and all of its parents => it can contain the potential
          // assigned to the node in the MN
          factor_to_clique__.insert(
             nodeset,
             triangulation__->createdJunctionTreeClique(first_eliminated_node));
        }
      }
    }

    // compute the roots of propagator__'s connected components
    computeJoinTreeRoots__();

    // remove all the Shafer-Shenoy potentials stored into the cliques
    for (const auto& xpot: clique_potentials__) {
      if (clique_potentials_list__[xpot.first].size() > 1) delete xpot.second;
    }
    clique_potentials_list__.clear();

    // create empty potential lists into the cliques of the joint tree as well
    // as empty lists of evidence
    PotentialSet__ empty_set;
    node_to_soft_evidence__.clear();
    for (const auto clik: *propagator__) {
      clique_potentials_list__.insert(clik, empty_set);
    }

    // remove all the potentials created during the last inference
    for (const auto& potlist: created_messages__)
      for (auto pot: potlist.second)
        delete pot;
    created_messages__.clear();

    // remove all the potentials created to take into account hard evidence
    // during the last inference
    for (auto pot_pair: hard_ev_projected_factors__)
      delete pot_pair.second;
    hard_ev_projected_factors__.clear();

    // remove all the constants created due to projections of factors that were
    // defined over only hard evidence nodes
    //__constants.clear();

    // create empty lists of potentials for the messages and indicate that no
    // message has been computed yet
    separator_potentials__.clear();
    messages_computed__.clear();
    for (const auto& edge: propagator__->edges()) {
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


    // put all the factors of the Markov net nodes into the cliques
    // here, beware: all the potentials that are defined over some nodes
    // including hard evidence must be projected so that these nodes are
    // removed from the potential
    const auto& evidence = this->evidence();
    for (const auto& kv: mn.factors()) {
      const auto& factor = kv.first;
      const auto& pot    = *(kv.second);

      NodeSet hard_nodes_in_factor;
      for (const auto xnode: factor) {
        if (hard_ev_nodes__.contains(xnode)) hard_nodes_in_factor.insert(xnode);
      }

      // if hard_nodes_in_factor contains hard evidence nodes, perform a projection
      // and insert the result into the appropriate clique, else insert
      // directly factor into the clique
      if (hard_nodes_in_factor.empty()) {
        clique_potentials_list__[factor_to_clique__[factor]].insert(&pot);
      } else {
        // marginalize out the hard evidence nodes: if the factor is defined
        // only over nodes that received hard evidence, do not consider it
        // as a potential anymore but as a constant
        if (hard_nodes_in_factor.size() != factor.size()) {
          // perform the projection with a combine and project instance
          Set< const DiscreteVariable* > hard_variables;
          PotentialSet__                 marg_factor_set;
          marg_factor_set.insert(&pot);
          for (const auto xnode: hard_nodes_in_factor) {
            marg_factor_set.insert(evidence[xnode]);
            hard_variables.insert(&(mn.variable(xnode)));
          }

          // perform the combination of those potentials and their projection
          MultiDimCombineAndProjectDefault< GUM_SCALAR, Potential >
             combine_and_project(combination_op__, SSNewMNprojPotential);
          PotentialSet__ new_factor_list
             = combine_and_project.combineAndProject(marg_factor_set,
                                                     hard_variables);

          // there should be only one potential in new_factor_list
          if (new_factor_list.size() != 1) {
            for (auto pot: new_factor_list) {
              if (!marg_factor_set.contains(pot)) delete pot;
            }
            GUM_ERROR(FatalError,
                      "the projection of a potential containing "
                         << "hard evidence is empty!");
          }
          const Potential< GUM_SCALAR >* projected_factor
             = *(new_factor_list.begin());
          clique_potentials_list__[factor_to_clique__[factor]].insert(
             projected_factor);
          hard_ev_projected_factors__.insert(factor, projected_factor);
        }
      }
    }

    // we shall now add all the potentials of the soft evidence
    for (const auto node: this->softEvidenceNodes()) {
      node_to_soft_evidence__.insert(node, evidence[node]);
      clique_potentials_list__[node_to_clique__[node]].insert(evidence[node]);
    }

    // now, in clique_potentials_list__, for each clique, we have the list of
    // potentials that must be combined in order to produce the Shafer-Shenoy's
    // potential stored into the clique. So, perform this combination and
    // store the result in clique_potentials__
    clique_potentials__.clear();
    MultiDimCombinationDefault< GUM_SCALAR, Potential > fast_combination(
       combination_op__);
    for (const auto& xpotset: clique_potentials_list__) {
      const auto& potset = xpotset.second;
      if (potset.size() > 0) {
        // here, there will be an entry in clique_potentials__
        // If there is only one element in potset, this element shall be
        // stored into clique_potentials__, else all the elements of potset
        // shall be combined and their result shall be stored
        if (potset.size() == 1) {
          clique_potentials__.insert(xpotset.first, *(potset.cbegin()));
        } else {
          auto joint = fast_combination.combine(potset);
          clique_potentials__.insert(xpotset.first, joint);
        }
      }
    }

    // indicate that the data structures are up to date.
    evidence_changes__.clear();
    is_new_jt_needed__ = false;
  }   // namespace gum


  /// prepare the inference structures w.r.t. new targets, soft/hard evidence
  template < typename GUM_SCALAR >
  void ShaferShenoyMNInference< GUM_SCALAR >::updateOutdatedStructure_() {
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
  void ShaferShenoyMNInference< GUM_SCALAR >::diffuseMessageInvalidations__(
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
      if (created_messages__.exists(arc)) {
        auto& arc_created_potentials = created_messages__[arc];
        for (auto pot: arc_created_potentials)
          delete pot;
        arc_created_potentials.clear();
      }

      // go on with the diffusion
      for (const auto node_id: propagator__->neighbours(to_id)) {
        if (node_id != from_id)
          diffuseMessageInvalidations__(to_id, node_id, invalidated_cliques);
      }
    }
  }


  /// update the potentials stored in the cliques and invalidate outdated
  /// messages
  template < typename GUM_SCALAR >
  void ShaferShenoyMNInference< GUM_SCALAR >::updateOutdatedPotentials_() {
    // for each clique, indicate whether the potential stored into
    // clique_ss_potentials__[clique] is the result of a combination. In this
    // case, it has been allocated by the combination and will need to be
    // deallocated if its clique has been invalidated
    const auto& mn = this->MN();

    NodeProperty< bool > to_deallocate(clique_potentials_list__.size());
    for (auto pot_iter = clique_potentials_list__.cbegin();
         pot_iter != clique_potentials_list__.cend();
         ++pot_iter) {
      to_deallocate.insert(pot_iter.key(), (pot_iter.val().size() > 1));
    }

    // compute the set of factors that were projected due to hard evidence and
    // whose hard evidence have changed, so that they need a new projection.
    // By the way, remove these factors since they are no more needed
    // Here only the values of the hard evidence can have changed (else a
    // fully new join tree would have been computed).
    // Note also that we know that the factors still contain some variable(s) after
    // the projection (else they should be constants)
    NodeSet        hard_nodes_changed(hard_ev_nodes__.size());
    Set< NodeId >  hard_cliques_changed;
    Set< NodeSet > hard_factors_changed;
    for (const auto node: hard_ev_nodes__) {
      if (evidence_changes__.exists(node)) {
        hard_nodes_changed.insert(node);
        for (const auto& elt: mn.factors()) {
          const auto& chgFactor = elt.first;
          if (chgFactor.contains(node)) {
            if (!hard_factors_changed.contains(chgFactor)) {
              hard_factors_changed.insert(chgFactor);

              const auto&  chgPot    = hard_ev_projected_factors__[chgFactor];
              const NodeId chgClique = factor_to_clique__[chgFactor];
              clique_potentials_list__[chgClique].erase(chgPot);
              delete chgPot;
              hard_ev_projected_factors__.erase(chgFactor);

              if (!hard_cliques_changed.contains(chgClique))
                hard_cliques_changed.insert(chgClique);
            }
          }
        }
      }
    }

    // invalidate all the messages that are no more correct: start from each of
    // the nodes whose soft evidence has changed and perform a diffusion from
    // the clique into which the soft evidence has been entered, indicating that
    // the messages spreading from this clique are now invalid. At the same time,
    // if there were potentials created on the arcs over which the messages were
    // sent, remove them from memory. For all the cliques that received some
    // projected factors that should now be changed, do the same.
    NodeSet invalidated_cliques(propagator__->size());
    for (const auto& pair: evidence_changes__) {
      if (node_to_clique__.exists(pair.first)) {
        const auto clique = node_to_clique__[pair.first];
        invalidated_cliques.insert(clique);
        for (const auto neighbor: propagator__->neighbours(clique)) {
          diffuseMessageInvalidations__(clique, neighbor, invalidated_cliques);
        }
      }
    }

    // now, add to the set of invalidated cliques those that contain projected
    // factors that were changed.
    for (const auto clique: hard_cliques_changed) {
      invalidated_cliques.insert(clique);
      for (const auto neighbor: propagator__->neighbours(clique)) {
        diffuseMessageInvalidations__(clique, neighbor, invalidated_cliques);
      }
    }

    // now that we know the cliques whose set of potentials have been changed,
    // we can discard their corresponding Shafer-Shenoy potential
    for (const auto clique: invalidated_cliques)
      if (clique_potentials__.exists(clique) && to_deallocate[clique])
        delete clique_potentials__[clique];

    // now we shall remove all the posteriors that belong to the
    // invalidated cliques. First, cope only with the nodes that did not
    // received hard evidence since the other nodes do not belong to the
    // join tree
    if (!target_posteriors__.empty()) {
      for (auto iter = target_posteriors__.beginSafe();
           iter != target_posteriors__.endSafe();
           ++iter) {
        if (reduced_graph__.exists(iter.key())
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
    // and clique_potentials_list__ and add the new soft ones
    for (auto& pot_pair: node_to_soft_evidence__)
      clique_potentials_list__[node_to_clique__[pot_pair.first]].erase(
         pot_pair.second);
    node_to_soft_evidence__.clear();

    const auto& evidence = this->evidence();
    for (const auto node: this->softEvidenceNodes()) {
      node_to_soft_evidence__.insert(node, evidence[node]);
      clique_potentials_list__[node_to_clique__[node]].insert(evidence[node]);
    }


    // Now add the projections of the factors due to newly changed hard evidence:
    // if we are performing updateOutdatedPotentials_, this means that the
    // set of nodes that received hard evidence has not been changed, only
    // their instantiations can have been changed. So, if there is an entry
    // for node in constants__, there will still be such an entry after
    // performing the new projections. Idem for hard_ev_projected_factors__

    // for (const auto node: factors_with_projected_factors_changed) {
    for (const auto& iter_factor: mn.factors()) {
      NodeSet hard_nodes = hard_nodes_changed * iter_factor.first;
      if (hard_nodes.empty()) continue;   // no change in iter_factor

      // perform the projection with a combine and project instance
      PotentialSet__                 marg_factor_set{iter_factor.second};
      Set< const DiscreteVariable* > hard_variables;
      for (const auto xnode: hard_nodes) {
        marg_factor_set.insert(evidence[xnode]);
        hard_variables.insert(&mn.variable(xnode));
      }

      // perform the combination of those potentials and their projection
      MultiDimCombineAndProjectDefault< GUM_SCALAR, Potential >
                     combine_and_project(combination_op__, SSNewMNprojPotential);
      PotentialSet__ new_potentials_list
         = combine_and_project.combineAndProject(marg_factor_set, hard_variables);

      // there should be only one potential in new_potentials_list
      if (new_potentials_list.size() != 1) {
        // remove the potentials created to avoid memory leaks
        for (auto pot: new_potentials_list) {
          if (!marg_factor_set.contains(pot)) delete pot;
        }
        GUM_ERROR(FatalError,
                  "the projection of a potential containing "
                     << "hard evidence is empty!");
      }

      const Potential< GUM_SCALAR >* projected_factor
         = *(new_potentials_list.begin());
      clique_potentials_list__[factor_to_clique__[iter_factor.first]].insert(
         projected_factor);
      hard_ev_projected_factors__.insert(iter_factor.first, projected_factor);
    }

    // here, the list of potentials stored in the invalidated cliques have
    // been updated. So, now, we can combine them to produce the Shafer-Shenoy
    // potential stored into the clique
    MultiDimCombinationDefault< GUM_SCALAR, Potential > fast_combination(
       combination_op__);
    for (const auto clique: invalidated_cliques) {
      const auto& potset = clique_potentials_list__[clique];

      if (potset.size() > 0) {
        // here, there will be an entry in clique_potentials__
        // If there is only one element in potset, this element shall be
        // stored into clique_potentials__, else all the elements of potset
        // shall be combined and their result shall be stored
        if (potset.size() == 1) {
          clique_potentials__[clique] = *(potset.cbegin());
        } else {
          auto joint                  = fast_combination.combine(potset);
          clique_potentials__[clique] = joint;
        }
      }
    }

    // update the constants
    /*const auto& hard_evidence = this->hardEvidence();
    for (auto& node_cst: constants__) {
      const Potential< GUM_SCALAR >& cpt = mn.cpt(node_cst.first);
      const auto&                    variables = cpt.variablesSequence();
      Instantiation                  inst;
      for (const auto var: variables)
        inst << *var;
      for (const auto var: variables) {
        inst.chgVal(var, hard_evidence[mn.nodeId(*var)]);
      }
      node_cst.second = cpt[inst];
    }*/

    // indicate that all changes have been performed
    evidence_changes__.clear();
  }


  /// compute a root for each connected component of propagator__
  template < typename GUM_SCALAR >
  void ShaferShenoyMNInference< GUM_SCALAR >::computeJoinTreeRoots__() {
    const auto& mn = this->MN();

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
    std::size_t                              i = 0;
    for (const auto clique_id: clique_targets) {
      const auto& clique   = propagator__->clique(clique_id);
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
              [](const std::pair< NodeId, Size >& a,
                 const std::pair< NodeId, Size >& b) -> bool {
                return a.second < b.second;
              });

    // pick up the clique with the smallest size in each connected component
    NodeProperty< bool > marked = propagator__->nodesProperty(false);
    std::function< void(NodeId, NodeId) > diffuse_marks
       = [&marked, &diffuse_marks, this](NodeId node, NodeId from) {
           if (!marked[node]) {
             marked[node] = true;
             for (const auto neigh: propagator__->neighbours(node))
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
  INLINE void
     ShaferShenoyMNInference< GUM_SCALAR >::collectMessage__(NodeId id,
                                                             NodeId from) {
    for (const auto other: propagator__->neighbours(id)) {
      if ((other != from) && !messages_computed__[Arc(other, id)])
        collectMessage__(other, id);
    }

    if ((id != from) && !messages_computed__[Arc(id, from)]) {
      produceMessage__(id, from);
    }
  }

  // remove variables del_vars from the list of potentials pot_list
  template < typename GUM_SCALAR >
  Set< const Potential< GUM_SCALAR >* >
     ShaferShenoyMNInference< GUM_SCALAR >::marginalizeOut__(
        Set< const Potential< GUM_SCALAR >* > pot_list,
        Set< const DiscreteVariable* >&       del_vars,
        Set< const DiscreteVariable* >&       kept_vars) {
    // remove the potentials corresponding to barren variables if we want
    // to exploit barren nodes
    /*PotentialSet__ barren_projected_potentials;
    if (barren_nodes_type__ == FindBarrenNodesType::FIND_BARREN_NODES) {
      barren_projected_potentials = removeBarrenVariables__(pot_list, del_vars);
    }*/

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
    /*for (auto iter = barren_projected_potentials.beginSafe();
         iter != barren_projected_potentials.endSafe();
         ++iter) {
      if (!new_pot_list.exists(*iter)) delete *iter;
    }*/

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
  void ShaferShenoyMNInference< GUM_SCALAR >::produceMessage__(NodeId from_id,
                                                               NodeId to_id) {
    // get the potentials of the clique.
    PotentialSet__ pot_list;
    if (clique_potentials__.exists(from_id))
      pot_list.insert(clique_potentials__[from_id]);

    // add the messages sent by adjacent nodes to from_id
    for (const auto other_id: propagator__->neighbours(from_id))
      if (other_id != to_id)
        pot_list += separator_potentials__[Arc(other_id, from_id)];

    // get the set of variables that need be removed from the potentials
    const NodeSet& from_clique = propagator__->clique(from_id);
    const NodeSet& separator   = propagator__->separator(from_id, to_id);
    Set< const DiscreteVariable* > del_vars(from_clique.size());
    Set< const DiscreteVariable* > kept_vars(separator.size());
    const auto&                    mn = this->MN();

    for (const auto node: from_clique) {
      if (!separator.contains(node)) {
        del_vars.insert(&(mn.variable(node)));
      } else {
        kept_vars.insert(&(mn.variable(node)));
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
          if (!created_messages__.exists(arc))
            created_messages__.insert(arc, PotentialSet__());
          created_messages__[arc].insert(pot);
        }
      } else {
        // create the message in the separator
        MultiDimCombinationDefault< GUM_SCALAR, Potential > fast_combination(
           combination_op__);
        auto joint = fast_combination.combine(new_pot_list);
        separator_potentials__[arc].insert(joint);
        if (!created_messages__.exists(arc))
          created_messages__.insert(arc, PotentialSet__());
        created_messages__[arc].insert(joint);

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
  INLINE void ShaferShenoyMNInference< GUM_SCALAR >::makeInference_() {
    // collect messages for all single targets
    for (const auto node: this->targets()) {
      // perform only collects in the join tree for nodes that have
      // not received hard evidence (those that received hard evidence were
      // not included into the join tree for speed-up reasons)
      if (reduced_graph__.exists(node)) {
        collectMessage__(node_to_clique__[node], node_to_clique__[node]);
      }
    }

    // collect messages for all set targets
    // by parsing  joint_target_to_clique__, we ensure that the cliques that
    // are referenced belong to the join tree (even if some of the nodes in
    // their associated joint_target do not belong to reduced_graph__)
    for (const auto set: joint_target_to_clique__)
      collectMessage__(set.second, set.second);
  }


  /// returns a fresh potential equal to P(1st arg,evidence)
  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >*
     ShaferShenoyMNInference< GUM_SCALAR >::unnormalizedJointPosterior_(
        NodeId id) {
    const auto& mn = this->MN();

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
    if (clique_potentials__.exists(clique_of_id))
      pot_list.insert(clique_potentials__[clique_of_id]);

    // add the messages sent by adjacent nodes to targetCliquxse
    for (const auto other: propagator__->neighbours(clique_of_id))
      pot_list += separator_potentials__[Arc(other, clique_of_id)];

    // get the set of variables that need be removed from the potentials
    const NodeSet&                 nodes = propagator__->clique(clique_of_id);
    Set< const DiscreteVariable* > kept_vars{&(mn.variable(id))};
    Set< const DiscreteVariable* > del_vars(nodes.size());
    for (const auto node: nodes) {
      if (node != id) del_vars.insert(&(mn.variable(node)));
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
                "some evidence entered into the Markov "
                "net are incompatible (their joint proba = 0)");
    }
    return joint;
  }


  /// returns the posterior of a given variable
  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
     ShaferShenoyMNInference< GUM_SCALAR >::posterior_(NodeId id) {
    // check if we have already computed the posterior
    if (target_posteriors__.exists(id)) { return *(target_posteriors__[id]); }

    // compute the joint posterior and normalize
    auto joint = unnormalizedJointPosterior_(id);
    joint->normalize();
    target_posteriors__.insert(id, joint);

    return *joint;
  }


  // returns the marginal a posteriori proba of a given node
  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >*
     ShaferShenoyMNInference< GUM_SCALAR >::unnormalizedJointPosterior_(
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
        if (!reduced_graph__.exists(node)) {
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
      const NodeSet& clique_nodes = propagator__->clique(clique_of_set);
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
    if (clique_potentials__.exists(clique_of_set))
      pot_list.insert(clique_potentials__[clique_of_set]);

    // add the messages sent by adjacent nodes to targetClique
    for (const auto other: propagator__->neighbours(clique_of_set))
      pot_list += separator_potentials__[Arc(other, clique_of_set)];

    // get the set of variables that need be removed from the potentials
    const NodeSet&                 nodes = propagator__->clique(clique_of_set);
    Set< const DiscreteVariable* > del_vars(nodes.size());
    Set< const DiscreteVariable* > kept_vars(targets.size());
    const auto&                    mn = this->MN();
    for (const auto node: nodes) {
      if (!targets.contains(node)) {
        del_vars.insert(&(mn.variable(node)));
      } else {
        kept_vars.insert(&(mn.variable(node)));
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
                "some evidence entered into the Markov "
                "net are incompatible (their joint proba = 0)");
    }

    return joint;
  }


  /// returns the posterior of a given set of variables
  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
     ShaferShenoyMNInference< GUM_SCALAR >::jointPosterior_(const NodeSet& set) {
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
     ShaferShenoyMNInference< GUM_SCALAR >::jointPosterior_(
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
    const auto&                    mn = this->MN();
    Set< const DiscreteVariable* > del_vars;
    for (const auto node: declared_target)
      if (!wanted_target.contains(node)) del_vars.insert(&(mn.variable(node)));
    auto pot = new Potential< GUM_SCALAR >(
       joint_target_posteriors__[declared_target]->margSumOut(del_vars));

    // save the result into the cache
    joint_target_posteriors__.insert(wanted_target, pot);

    return *pot;
  }


  template < typename GUM_SCALAR >
  GUM_SCALAR ShaferShenoyMNInference< GUM_SCALAR >::evidenceProbability() {
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
      NodeId                   node = *(propagator__->clique(root).begin());
      Potential< GUM_SCALAR >* tmp  = unnormalizedJointPosterior_(node);
      GUM_SCALAR               sum  = 0;
      for (Instantiation iter(*tmp); !iter.end(); ++iter)
        sum += tmp->get(iter);
      prob_ev *= sum;
      delete tmp;
    }

    // for (const auto& projected_cpt: constants__)
    //  prob_ev *= projected_cpt.second;

    return prob_ev;
  }

  template < typename GUM_SCALAR >
  bool ShaferShenoyMNInference< GUM_SCALAR >::isExactJointComputable_(
     const NodeSet& vars) {
    if (JointTargetedMNInference< GUM_SCALAR >::isExactJointComputable_(vars))
      return true;

    this->prepareInference();

    for (const auto& cliknode: this->propagator__->nodes()) {
      GUM_TRACE_VAR(cliknode);
      const auto clique = propagator__->clique(cliknode);
      GUM_TRACE_VAR(clique);
      if (vars == clique) return true;
      GUM_CHECKPOINT;
    }
    GUM_CHECKPOINT;
    return false;
  }

  template < typename GUM_SCALAR >
  NodeSet ShaferShenoyMNInference< GUM_SCALAR >::superForJointComputable_(
     const NodeSet& vars) {
    const auto superset
       = JointTargetedMNInference< GUM_SCALAR >::superForJointComputable_(vars);
    if (!superset.empty()) return superset;

    this->prepareInference();

    for (const auto& cliknode: propagator__->nodes()) {
      const auto clique = propagator__->clique(cliknode);
      if (vars.isProperSubsetOf(clique)) return clique;
    }


    return NodeSet();
  }

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
