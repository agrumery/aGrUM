/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES et Pierre-Henri WUILLEMIN   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 * @file
 * @brief Implementation of Shafer-Shenoy's propagation for inference in
 * Bayesian Networks.
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include <agrum/BN/inference/ShaferShenoyInference.h>

#include <agrum/BN/inference/BayesBall.h>
#include <agrum/BN/inference/barrenNodesFinder.h>
#include <agrum/BN/inference/dSeparation.h>
#include <agrum/graphs/binaryJoinTreeConverterDefault.h>
#include <agrum/multidim/instantiation.h>
#include <agrum/multidim/operators/multiDimCombineAndProjectDefault.h>
#include <agrum/multidim/operators/multiDimProjection.h>


namespace gum {
  // default constructor
  template <typename GUM_SCALAR>
  INLINE ShaferShenoyInference<GUM_SCALAR>::ShaferShenoyInference(
      const IBayesNet<GUM_SCALAR>* BN,
      FindBarrenNodesType          barren_type,
      bool                         use_binary_join_tree )
      : JointTargetedInference<GUM_SCALAR>( BN )
      , EvidenceInference<GUM_SCALAR>( BN )
      , __use_binary_join_tree( use_binary_join_tree ) {
    // sets the barren nodes finding algorithm
    setFindBarrenNodesType( barren_type );

    // create a default triangulation (the user can change it afterwards)
    __triangulation = new DefaultTriangulation;

    // for debugging purposessetRequiredInference
    GUM_CONSTRUCTOR( ShaferShenoyInference );
  }


  // destructor
  template <typename GUM_SCALAR>
  INLINE ShaferShenoyInference<GUM_SCALAR>::~ShaferShenoyInference() {
    // remove all the potentials created during the last message passing
    for ( const auto& pots : __created_potentials )
      for ( const auto pot : pots.second )
        delete pot;

    // remove the potentials created after removing the nodes that received
    // hard evidence
    for ( const auto& pot : __hard_ev_projected_CPTs )
      delete pot.second;

    // remove all the potentials in __clique_ss_potential that do not belong
    // to __clique_potentials : in this case, those potentials have been
    // created by combination of the corresponding list of potentials in
    // __clique_potentials. In other words, the size of this list is strictly
    // greater than 1.
    for ( auto pot : __clique_ss_potential ) {
      if ( __clique_potentials[pot.first].size() > 1 ) delete pot.second;
    }

    // remove all the posteriors computed
    for ( const auto& pot : __target_posteriors )
      delete pot.second;
    for ( const auto& pot : __joint_target_posteriors )
      delete pot.second;

    // remove the junction tree and the triangulation algorithm
    if ( __JT != nullptr ) delete __JT;
    if ( __junctionTree != nullptr ) delete __junctionTree;
    delete __triangulation;

    // for debugging purposes
    GUM_DESTRUCTOR( ShaferShenoyInference );
  }


  /// set a new triangulation algorithm
  template <typename GUM_SCALAR>
  void ShaferShenoyInference<GUM_SCALAR>::setTriangulation(
      const Triangulation& new_triangulation ) {
    delete __triangulation;
    __triangulation = new_triangulation.newFactory();
    __is_new_jt_needed = true;
    this->_setOutdatedBNStructureState();
  }


  /// returns the current join tree used
  template <typename GUM_SCALAR>
  INLINE const JoinTree* ShaferShenoyInference<GUM_SCALAR>::joinTree() {
    __createNewJT();

    return __JT;
  }

  /// returns the current junction tree
  template <typename GUM_SCALAR>
  INLINE const JunctionTree* ShaferShenoyInference<GUM_SCALAR>::junctionTree() {
    __createNewJT();

    return __junctionTree;
  }


  /// sets the operator for performing the projections
  template <typename GUM_SCALAR>
  INLINE void ShaferShenoyInference<GUM_SCALAR>::__setProjectionFunction(
      Potential<GUM_SCALAR>* ( *proj )(const Potential<GUM_SCALAR>&,
                                       const Set<const DiscreteVariable*>&)) {
    __projection_op = proj;
  }


  /// sets the operator for performing the combinations
  template <typename GUM_SCALAR>
  INLINE void ShaferShenoyInference<GUM_SCALAR>::__setCombinationFunction(
      Potential<GUM_SCALAR>* ( *comb )(const Potential<GUM_SCALAR>&,
                                       const Potential<GUM_SCALAR>&)) {
    __combination_op = comb;
  }


  /// invalidate all messages, posteriors and created potentials
  template <typename GUM_SCALAR>
  void ShaferShenoyInference<GUM_SCALAR>::__invalidateAllMessages() {
    // remove all the messages computed
    for ( auto& potset : __separator_potentials )
      potset.second.clear();
    for ( auto& mess_computed : __messages_computed )
      mess_computed.second = false;

    // remove all the created potentials
    for ( const auto& potset : __created_potentials )
      for ( const auto pot : potset.second )
        delete pot;

    // remove all the posteriors
    for ( const auto& pot : __target_posteriors )
      delete pot.second;
    for ( const auto& pot : __joint_target_posteriors )
      delete pot.second;

    // indicate that new messages need be computed
    if ( this->isInferenceReady() || this->isDone() )
      this->_setOutdatedBNPotentialsState();
  }


  /// sets how we determine barren nodes
  template <typename GUM_SCALAR>
  void ShaferShenoyInference<GUM_SCALAR>::setFindBarrenNodesType(
      FindBarrenNodesType type ) {
    if ( type != __barren_nodes_type ) {
      // WARNING: if a new type is added here, method __createJT should
      // certainly be updated as well, in particular its step 2.
      switch ( type ) {
        case FindBarrenNodesType::FIND_BARREN_NODES:
        case FindBarrenNodesType::FIND_NO_BARREN_NODES:
          break;

        default:
          GUM_ERROR( InvalidArgument,
                     "setFindBarrenNodesType for type "
                         << (unsigned int)type
                         << " is not implemented yet" );
      }

      __barren_nodes_type = type;

      // potentially, we may need to reconstruct a junction tree
      this->_setOutdatedBNStructureState();
    }
  }


  /// fired when a new evidence is inserted
  template <typename GUM_SCALAR>
  INLINE void
  ShaferShenoyInference<GUM_SCALAR>::_onEvidenceAdded( const NodeId id,
                                                       bool isHardEvidence ) {
    // if we have a new hard evidence, this modifies the undigraph over which
    // the join tree is created. This is also the case if id is not a node of
    // of the undigraph
    if ( isHardEvidence || !__graph.exists( id ) )
      __is_new_jt_needed = true;
    else {
      try {
        __evidence_changes.insert( id, EvidenceChangeType::EVIDENCE_ADDED );
      } catch ( DuplicateElement& ) {
        // here, the evidence change already existed. This necessarily means
        // that the current saved change is an EVIDENCE_ERASED. So if we
        // erased the evidence and added some again, this corresponds to an
        // EVIDENCE_MODIFIED
        __evidence_changes[id] = EvidenceChangeType::EVIDENCE_MODIFIED;
      }
    }
  }


  /// fired when an evidence is removed
  template <typename GUM_SCALAR>
  INLINE void
  ShaferShenoyInference<GUM_SCALAR>::_onEvidenceErased( const NodeId id,
                                                        bool isHardEvidence ) {
    // if we delete a hard evidence, this modifies the undigraph over which
    // the join tree is created.
    if ( isHardEvidence )
      __is_new_jt_needed = true;
    else {
      try {
        __evidence_changes.insert( id, EvidenceChangeType::EVIDENCE_ERASED );
      } catch ( DuplicateElement& ) {
        // here, the evidence change already existed and it is necessarily an
        // EVIDENCE_ADDED or an EVIDENCE_MODIFIED. So, if the evidence has
        // been added and is now erased, this is similar to not having created
        // it. If the evidence was only modified, it already existed in the
        // last inference and we should now indicate that it has been removed.
        if ( __evidence_changes[id] == EvidenceChangeType::EVIDENCE_ADDED )
          __evidence_changes.erase( id );
        else
          __evidence_changes[id] = EvidenceChangeType::EVIDENCE_ERASED;
      }
    }
  }


  /// fired when all the evidence are erased
  template <typename GUM_SCALAR>
  void ShaferShenoyInference<GUM_SCALAR>::_onAllEvidenceErased(
      bool has_hard_evidence ) {
    if ( has_hard_evidence || !this->hardEvidenceNodes().empty() )
      __is_new_jt_needed = true;
    else {
      for ( const auto node : this->softEvidenceNodes() ) {
        try {
          __evidence_changes.insert( node, EvidenceChangeType::EVIDENCE_ERASED );
        } catch ( DuplicateElement& ) {
          // here, the evidence change already existed and it is necessarily an
          // EVIDENCE_ADDED or an EVIDENCE_MODIFIED. So, if the evidence has
          // been added and is now erased, this is similar to not having created
          // it. If the evidence was only modified, it already existed in the
          // last inference and we should now indicate that it has been removed.
          if ( __evidence_changes[node] == EvidenceChangeType::EVIDENCE_ADDED )
            __evidence_changes.erase( node );
          else
            __evidence_changes[node] = EvidenceChangeType::EVIDENCE_ERASED;
        }
      }
    }
  }


  /// fired when an evidence is changed
  template <typename GUM_SCALAR>
  INLINE void ShaferShenoyInference<GUM_SCALAR>::_onEvidenceChanged(
      const NodeId id, bool hasChangedSoftHard ) {
    if ( hasChangedSoftHard )
      __is_new_jt_needed = true;
    else {
      try {
        __evidence_changes.insert( id, EvidenceChangeType::EVIDENCE_MODIFIED );
      } catch ( DuplicateElement& ) {
        // here, the evidence change already existed and it is necessarily an
        // EVIDENCE_ADDED. So we should keep this state to indicate that this
        // evidence is new w.r.t. the last inference
      }
    }
  }


  /// fired after a new target is inserted
  template <typename GUM_SCALAR>
  INLINE void
  ShaferShenoyInference<GUM_SCALAR>::_onMarginalTargetAdded( const NodeId id ) {}


  /// fired before a target is removed
  template <typename GUM_SCALAR>
  INLINE void
  ShaferShenoyInference<GUM_SCALAR>::_onMarginalTargetErased( const NodeId id ) {}


  /// fired after a new set target is inserted
  template <typename GUM_SCALAR>
  INLINE void
  ShaferShenoyInference<GUM_SCALAR>::_onJointTargetAdded( const NodeSet& set ) {}


  /// fired before a set target is removed
  template <typename GUM_SCALAR>
  INLINE void
  ShaferShenoyInference<GUM_SCALAR>::_onJointTargetErased( const NodeSet& set ) {}


  /// fired after all the nodes of the BN are added as single targets
  template <typename GUM_SCALAR>
  INLINE void ShaferShenoyInference<GUM_SCALAR>::_onAllMarginalTargetsAdded() {}


  /// fired before a all the single_targets are removed
  template <typename GUM_SCALAR>
  INLINE void ShaferShenoyInference<GUM_SCALAR>::_onAllMarginalTargetsErased() {}


  /// fired before a all the joint_targets are removed
  template <typename GUM_SCALAR>
  INLINE void ShaferShenoyInference<GUM_SCALAR>::_onAllJointTargetsErased() {}


  /// fired before a all the single and joint_targets are removed
  template <typename GUM_SCALAR>
  INLINE void ShaferShenoyInference<GUM_SCALAR>::_onAllTargetsErased() {}


  // check whether a new junction tree is really needed for the next inference
  template <typename GUM_SCALAR>
  bool ShaferShenoyInference<GUM_SCALAR>::__isNewJTNeeded() const {
    // if we do not have a JT or if __new_jt_needed is set to true, then
    // we know that we need to create a new join tree
    if ( ( __JT == nullptr ) || __is_new_jt_needed ) return true;

    // if some some targets do not belong to the join tree and, consequently,
    // to the undigraph that was used to construct the join tree, then we need
    // to create a new JT. This situation may occur if we constructed the
    // join tree after pruning irrelevant/barren nodes from the BN)
    // however, note that the nodes that received hard evidence do not belong to
    // the graph and, therefore, should not be taken into account
    const auto& hard_ev_nodes = this->hardEvidenceNodes();
    for ( const auto node : this->targets() ) {
      if ( !__graph.exists( node ) && !hard_ev_nodes.exists( node ) ) return true;
    }
    for ( const auto& nodes : this->jointTargets() ) {
      // here, we need to check that at least one clique contains all the nodes.
      bool containing_clique_found = false;
      for ( const auto node : nodes ) {
        bool           found = true;
        const NodeSet& clique = __JT->clique( __node_to_clique[node] );
        for ( const auto xnode : nodes ) {
          if ( !clique.contains( xnode ) && !hard_ev_nodes.exists( xnode ) ) {
            found = false;
            break;
          }
        }
        if ( found ) {
          containing_clique_found = true;
          break;
        }
      }

      if ( !containing_clique_found ) return true;
    }

    // if some new evidence have been added on nodes that do not belong
    // to __graph, then we potentially have to reconstruct the join tree
    for ( const auto& change : __evidence_changes ) {
      if ( ( change.second == EvidenceChangeType::EVIDENCE_ADDED ) &&
           !__graph.exists( change.first ) )
        return true;
    }

    // here, the current JT is exactly what we need for the next inference
    return false;
  }


  /// create a new junction tree as well as its related data structures
  template <typename GUM_SCALAR>
  void ShaferShenoyInference<GUM_SCALAR>::__createNewJT() {
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
    __graph.clear();
    for ( auto node : bn.dag() )
      __graph.addNode( node );

    // 2/ if we wish to exploit barren nodes, we shall remove them from the
    // BN to do so: we identify all the nodes that are not targets and have
    // received no evidence and such that their descendants are neither
    // targets nor evidence nodes. Such nodes can be safely discarded from
    // the BN without altering the inference output
    if ( __barren_nodes_type == FindBarrenNodesType::FIND_BARREN_NODES ) {
      // identify the barren nodes
      NodeSet target_nodes = this->targets();
      for ( const auto& nodeset : this->jointTargets() ) {
        target_nodes += nodeset;
      }

      // check that all the nodes are not targets, otherwise, there is no
      // barren node
      if ( target_nodes.size() != bn.dag().sizeNodes() ) {
        BarrenNodesFinder finder( &( bn.dag() ) );
        finder.setTargets( &target_nodes );

        NodeSet evidence_nodes;
        for ( const auto& pair : this->evidence() ) {
          evidence_nodes.insert( pair.first );
        }
        finder.setEvidence( &evidence_nodes );

        NodeSet barren_nodes = finder.barrenNodes();

        // remove the barren nodes from the moral graph
        for ( const auto node : barren_nodes ) {
          __graph.eraseNode( node );
        }
      }
    }

    // 3/ add edges so that each node and its parents in the BN form a clique
    for ( const auto node : __graph ) {
      const NodeSet& parents = bn.dag().parents( node );
      for ( auto iter1 = parents.cbegin(); iter1 != parents.cend(); ++iter1 ) {
        __graph.addEdge( *iter1, node );
        auto iter2 = iter1;
        for ( ++iter2; iter2 != parents.cend(); ++iter2 ) {
          __graph.addEdge( *iter1, *iter2 );
        }
      }
    }

    // 4/ if there exist some joint targets, we shall add new edges into the
    // moral graph in order to ensure that there exists a clique containing
    // each joint
    for ( const auto& nodeset : this->jointTargets() ) {
      for ( auto iter1 = nodeset.cbegin(); iter1 != nodeset.cend(); ++iter1 ) {
        auto iter2 = iter1;
        for ( ++iter2; iter2 != nodeset.cend(); ++iter2 ) {
          __graph.addEdge( *iter1, *iter2 );
        }
      }
    }

    // 5/ remove all the nodes that received hard evidence
    __hard_ev_nodes = this->hardEvidenceNodes();
    for ( const auto node : __hard_ev_nodes ) {
      __graph.eraseNode( node );
    }


    // now, we can compute the new junction tree. To speed-up computations
    // (essentially, those of a distribution phase), we construct from this
    // junction tree a binary join tree
    if ( __JT != nullptr ) delete __JT;
    if ( __junctionTree != nullptr ) delete __junctionTree;

    __triangulation->setGraph( &__graph, &( this->domainSizes() ) );
    const JunctionTree& triang_jt = __triangulation->junctionTree();
    if ( __use_binary_join_tree ) {
      BinaryJoinTreeConverterDefault bjt_converter;
      NodeSet                        emptyset;
      __JT = new CliqueGraph(
          bjt_converter.convert( triang_jt, this->domainSizes(), emptyset ) );
    } else {
      __JT = new CliqueGraph( triang_jt );
    }
    __junctionTree = new CliqueGraph( triang_jt );


    // indicate, for each node of the moral graph a clique in __JT that can
    // contain its conditional probability table
    __node_to_clique.clear();
    const std::vector<NodeId>& JT_elim_order = __triangulation->eliminationOrder();
    NodeProperty<int>          elim_order( Size( JT_elim_order.size() ) );
    for ( std::size_t i = std::size_t( 0 ), size = JT_elim_order.size(); i < size;
          ++i )
      elim_order.insert( JT_elim_order[i], (int)i );
    const DAG& dag = bn.dag();
    for ( const auto node : __graph ) {
      // get the variables in the potential of node (and its parents)
      NodeId first_eliminated_node = node;
      int    elim_number = elim_order[first_eliminated_node];

      for ( const auto parent : dag.parents( node ) ) {
        if ( __graph.existsNode( parent ) &&
             ( elim_order[parent] < elim_number ) ) {
          elim_number = elim_order[parent];
          first_eliminated_node = parent;
        }
      }

      // first_eliminated_node contains the first var (node or one of its
      // parents) eliminated => the clique created during its elimination
      // contains node and all of its parents => it can contain the potential
      // assigned to the node in the BN
      __node_to_clique.insert(
          node,
          __triangulation->createdJunctionTreeClique( first_eliminated_node ) );
    }

    // do the same for the nodes that received evidence. Here, we only store
    // the nodes for which at least one parent belongs to __graph (otherwise
    // their CPT is just a constant real number).
    for ( const auto node : __hard_ev_nodes ) {
      // get the set of parents of the node that belong to __graph
      NodeSet pars( dag.parents( node ).size() );
      for ( const auto par : dag.parents( node ) )
        if ( __graph.exists( par ) ) pars.insert( par );

      if ( !pars.empty() ) {
        NodeId first_eliminated_node = *( pars.begin() );
        int    elim_number = elim_order[first_eliminated_node];

        for ( const auto parent : pars ) {
          if ( elim_order[parent] < elim_number ) {
            elim_number = elim_order[parent];
            first_eliminated_node = parent;
          }
        }

        // first_eliminated_node contains the first var (node or one of its
        // parents) eliminated => the clique created during its elimination
        // contains node and all of its parents => it can contain the potential
        // assigned to the node in the BN
        __node_to_clique.insert(
            node,
            __triangulation->createdJunctionTreeClique( first_eliminated_node ) );
      }
    }

    // indicate for each joint_target a clique that contains it
    __joint_target_to_clique.clear();
    for ( const auto& set : this->jointTargets() ) {
      // remove from set all the nodes that received hard evidence (since they
      // do not belong to the join tree)
      NodeSet nodeset = set;
      for ( const auto node : __hard_ev_nodes )
        if ( nodeset.contains( node ) ) nodeset.erase( node );

      if ( !nodeset.empty() ) {
        // the clique we are looking for is the one that was created when
        // the first element of nodeset was eliminated
        NodeId first_eliminated_node = *( nodeset.begin() );
        int    elim_number = elim_order[first_eliminated_node];
        for ( const auto node : nodeset ) {
          if ( elim_order[node] < elim_number ) {
            elim_number = elim_order[node];
            first_eliminated_node = node;
          }
        }
        
        __joint_target_to_clique.insert(
          set,
          __triangulation->createdJunctionTreeClique( first_eliminated_node ) );
      }
    }


    // compute the roots of __JT's connected components
    __computeJoinTreeRoots();

    // remove all the Shafer-Shenoy potentials stored into the cliques
    for ( const auto& xpot : __clique_ss_potential ) {
      if ( __clique_potentials[xpot.first].size() > 1 ) delete xpot.second;
    }

    // create empty potential lists into the cliques of the joint tree as well
    // as empty lists of evidence
    __PotentialSet empty_set;
    __clique_potentials.clear();
    __node_to_soft_evidence.clear();
    for ( const auto node : *__JT ) {
      __clique_potentials.insert( node, empty_set );
    }

    // remove all the potentials created during the last inference
    for ( auto& potlist : __created_potentials )
      for ( auto pot : potlist.second )
        delete pot;
    __created_potentials.clear();

    // remove all the potentials created to take into account hard evidence
    // during the last inference
    for ( auto pot_pair : __hard_ev_projected_CPTs )
      delete pot_pair.second;
    __hard_ev_projected_CPTs.clear();

    // remove all the constants created due to projections of CPTs that were
    // defined over only hard evidence nodes
    __constants.clear();

    // create empty lists of potentials for the messages and indicate that no
    // message has been computed yet
    __separator_potentials.clear();
    __messages_computed.clear();
    for ( const auto& edge : __JT->edges() ) {
      const Arc arc1( edge.first(), edge.second() );
      __separator_potentials.insert( arc1, empty_set );
      __messages_computed.insert( arc1, false );
      const Arc arc2( Arc( edge.second(), edge.first() ) );
      __separator_potentials.insert( arc2, empty_set );
      __messages_computed.insert( arc2, false );
    }

    // remove all the posteriors computed so far
    for ( const auto& pot : __target_posteriors )
      delete pot.second;
    __target_posteriors.clear();
    for ( const auto& pot : __joint_target_posteriors )
      delete pot.second;
    __joint_target_posteriors.clear();


    // put all the CPT's of the Bayes net nodes into the cliques
    // here, beware: all the potentials that are defined over some nodes
    // including hard evidence must be projected so that these nodes are
    // removed from the potential
    const auto& evidence = this->evidence();
    const auto& hard_evidence = this->hardEvidence();
    for ( const auto node : dag ) {
      if ( __graph.exists( node ) || __hard_ev_nodes.contains( node ) ) {
        const Potential<GUM_SCALAR>& cpt = bn.cpt( node );

        // get the list of nodes with hard evidence in cpt
        NodeSet     hard_nodes;
        const auto& variables = cpt.variablesSequence();
        for ( const auto var : variables ) {
          const NodeId xnode = bn.nodeId( *var );
          if ( __hard_ev_nodes.contains( xnode ) ) hard_nodes.insert( xnode );
        }

        // if hard_nodes contains hard evidence nodes, perform a projection
        // and insert the result into the appropriate clique, else insert
        // directly cpt into the clique
        if ( hard_nodes.empty() ) {
          __clique_potentials[__node_to_clique[node]].insert( &cpt );
        } else {
          // marginalize out the hard evidence nodes: if the cpt is defined
          // only over nodes that received hard evidence, do not consider it
          // as a potential anymore but as a constant
          if ( hard_nodes.size() == variables.size() ) {
            Instantiation inst( cpt );
            for ( Size i = 0; i < hard_nodes.size(); ++i ) {
              inst.chgVal( variables[i],
                           hard_evidence[bn.nodeId( *( variables[i] ) )] );
            }
            __constants.insert( node, cpt[inst] );
          } else {
            // perform the projection with a combine and project instance
            Set<const DiscreteVariable*> hard_variables;
            __PotentialSet               marg_cpt_set{&cpt};
            for ( const auto xnode : hard_nodes ) {
              marg_cpt_set.insert( evidence[xnode] );
              hard_variables.insert( &( bn.variable( xnode ) ) );
            }

            // perform the combination of those potentials and their projection
            MultiDimCombineAndProjectDefault<GUM_SCALAR, Potential>
                           combine_and_project( __combination_op, SSNewprojPotential );
            __PotentialSet new_cpt_list = combine_and_project.combineAndProject(
                marg_cpt_set, hard_variables );

            // there should be only one potential in new_cpt_list
            if ( new_cpt_list.size() != 1 ) {
              // remove the CPT created to avoid memory leaks
              for ( auto pot : new_cpt_list ) {
                if ( !marg_cpt_set.contains( pot ) ) delete pot;
              }
              GUM_ERROR( FatalError,
                         "the projection of a potential containing "
                             << "hard evidence is empty!" );
            }
            const Potential<GUM_SCALAR>* projected_cpt = *( new_cpt_list.begin() );
            __clique_potentials[__node_to_clique[node]].insert( projected_cpt );
            __hard_ev_projected_CPTs.insert( node, projected_cpt );
          }
        }
      }
    }

    // we shall now add all the potentials of the soft evidence
    for ( const auto node : this->softEvidenceNodes() ) {
      __node_to_soft_evidence.insert( node, evidence[node] );
      __clique_potentials[__node_to_clique[node]].insert( evidence[node] );
    }

    // now, in __clique_potentials, for each clique, we have the list of
    // potentials that must be comined in order to produce the Shafer-Shenoy's
    // potential stored into the clique. So, perform this combination and
    // store the result in __clique_ss_potential
    __clique_ss_potential.clear();
    MultiDimCombinationDefault<GUM_SCALAR, Potential> fast_combination(
        __combination_op );
    for ( const auto& xpotset : __clique_potentials ) {
      const auto& potset = xpotset.second;
      if ( potset.size() > 0 ) {
        // here, there will be an entry in __clique_ss_potential
        // If there is only one element in potset, this element shall be
        // stored into __clique_ss_potential, else all the elements of potset
        // shall be combined and their result shall be stored
        if ( potset.size() == 1 ) {
          __clique_ss_potential.insert( xpotset.first, *( potset.cbegin() ) );
        } else {
          auto joint = fast_combination.combine( potset );
          __clique_ss_potential.insert( xpotset.first, joint );
        }
      }
    }

    // indicate that the data structures are up to date.
    __evidence_changes.clear();
    __is_new_jt_needed = false;
  }


  /// prepare the inference structures w.r.t. new targets, soft/hard evidence
  template <typename GUM_SCALAR>
  void ShaferShenoyInference<GUM_SCALAR>::_updateOutdatedBNStructure() {
    // check if a new JT is really needed. If so, create it
    if ( __isNewJTNeeded() ) {
      __createNewJT();
    } else {
      // here, we can answer the next queries without reconstructing all the
      // junction tree. All we need to do is to indicate that we should
      // update the potentials and messages for these queries
      _updateOutdatedBNPotentials();
    }
  }


  /// invalidate all the messages sent from a given clique
  template <typename GUM_SCALAR>
  void ShaferShenoyInference<GUM_SCALAR>::__diffuseMessageInvalidations(
      const NodeId from_id, const NodeId to_id, NodeSet& invalidated_cliques ) {
    // invalidate the current clique
    invalidated_cliques.insert( to_id );

    // invalidate the current arc
    const Arc arc( from_id, to_id );
    bool&     message_computed = __messages_computed[arc];
    if ( message_computed ) {
      message_computed = false;
      __separator_potentials[arc].clear();
      if ( __created_potentials.exists( arc ) ) {
        auto& arc_created_potentials = __created_potentials[arc];
        for ( auto pot : arc_created_potentials )
          delete pot;
        arc_created_potentials.clear();
      }

      // go on with the diffusion
      for ( const auto node_id : __JT->neighbours( to_id ) ) {
        if ( node_id != from_id )
          __diffuseMessageInvalidations( to_id, node_id, invalidated_cliques );
      }
    }
  }


  /// update the potentials stored in the cliques and invalidate outdated
  /// messages
  template <typename GUM_SCALAR>
  void ShaferShenoyInference<GUM_SCALAR>::_updateOutdatedBNPotentials() {
    // for each clique, indicate whether the potential stored into
    // __clique_ss_potentials[clique] is the result of a combination. In this
    // case, it has been allocated by the combination and will need to be
    // deallocated if its clique has been invalidated
    NodeProperty<bool> ss_potential_to_deallocate ( __clique_potentials.size () );
    for ( auto pot_iter = __clique_potentials.cbegin ();
          pot_iter != __clique_potentials.cend (); ++pot_iter ) {
      ss_potential_to_deallocate.insert ( pot_iter.key(),
                                          ( pot_iter.val().size () > 1 ) );
    }
    
    // compute the set of CPTs that were projected due to hard evidence and
    // whose hard evidence have changed, so that they need a new projection.
    // By the way, remove these CPTs since they are no more needed
    // Here only the values of the hard evidence can have changed (else a
    // fully new join tree would have been computed).
    // Note also that we know that the CPTs still contain some variable(s) after
    // the projection (else they should be constants)
    NodeSet hard_nodes_changed( __hard_ev_nodes.size() );
    for ( const auto node : __hard_ev_nodes )
      if ( __evidence_changes.exists( node ) ) hard_nodes_changed.insert( node );

    NodeSet nodes_with_projected_CPTs_changed;
    const auto& bn = this->BN();
    for ( auto pot_iter = __hard_ev_projected_CPTs.beginSafe();
          pot_iter != __hard_ev_projected_CPTs.endSafe();
          ++pot_iter ) {
      for ( const auto var : bn.cpt( pot_iter.key() ).variablesSequence() ) {
        if ( hard_nodes_changed.contains( bn.nodeId( *var ) ) ) {
          nodes_with_projected_CPTs_changed.insert( pot_iter.key() );
          delete pot_iter.val();
          __clique_potentials[__node_to_clique[pot_iter.key()]].erase(
              pot_iter.val() );
          __hard_ev_projected_CPTs.erase( pot_iter );
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
    NodeSet invalidated_cliques( __JT->size() );
    for ( const auto& pair : __evidence_changes ) {
      if ( __node_to_clique.exists( pair.first ) ) {
        const auto clique = __node_to_clique[pair.first];
        invalidated_cliques.insert( clique );
        for ( const auto neighbor : __JT->neighbours( clique ) ) {
          __diffuseMessageInvalidations( clique, neighbor, invalidated_cliques );
        }
      }
    }
    
    // now, add to the set of invalidated cliques those that contain projected
    // CPTs that were changed.
    for ( auto node : nodes_with_projected_CPTs_changed ) {
      const auto clique = __node_to_clique[node];
      invalidated_cliques.insert( clique );
      for ( const auto neighbor : __JT->neighbours( clique ) ) {
        __diffuseMessageInvalidations( clique, neighbor, invalidated_cliques );
      }
    }

    // now that we know the cliques whose set of potentials have been changed,
    // we can discard their corresponding Shafer-Shenoy potential
    for ( const auto clique : invalidated_cliques ) {
      if ( __clique_ss_potential.exists( clique ) &&
           ss_potential_to_deallocate[clique] ) {
        delete __clique_ss_potential[clique];
      }
    }


    // now we shall remove all the posteriors that belong to the
    // invalidated cliques. First, cope only with the nodes that did not
    // received hard evidence since the other nodes do not belong to the
    // join tree
    for ( auto iter = __target_posteriors.beginSafe();
          iter != __target_posteriors.endSafe();
          ++iter ) {
      if ( __graph.exists( iter.key() ) &&
           ( invalidated_cliques.exists( __node_to_clique[iter.key()] ) ) ) {
        delete iter.val();
        __target_posteriors.erase( iter );
      }
    }

    // now cope with the nodes that received hard evidence
    for ( auto iter = __target_posteriors.beginSafe();
          iter != __target_posteriors.endSafe();
          ++iter ) {
      if ( hard_nodes_changed.contains( iter.key() ) ) {
        delete iter.val();
        __target_posteriors.erase( iter );
      }
    }

    // finally, cope with joint targets
    for ( auto iter = __joint_target_posteriors.beginSafe();
          iter != __joint_target_posteriors.endSafe();
          ++iter ) {
      if ( invalidated_cliques.exists( __joint_target_to_clique[iter.key()] ) ) {
        delete iter.val();
        __joint_target_posteriors.erase( iter );
      }
    }


    // remove all the evidence that were entered into __node_to_soft_evidence
    // and __clique_potentials and add the new soft ones
    for ( auto& pot_pair : __node_to_soft_evidence ) {
      __clique_potentials[__node_to_clique[pot_pair.first]].erase(
          pot_pair.second );
    }
    __node_to_soft_evidence.clear();

    const auto& evidence = this->evidence();
    for ( const auto node : this->softEvidenceNodes() ) {
      __node_to_soft_evidence.insert( node, evidence[node] );
      __clique_potentials[__node_to_clique[node]].insert( evidence[node] );
    }


    // Now add the projections of the CPTs due to newly changed hard evidence:
    // if we are performing _updateOutdatedBNPotentials, this means that the
    // set of nodes that received hard evidence has not been changed, only
    // their instantiations can have been changed. So, if there is an entry
    // for node in __constants, there will still be such an entry after
    // performing the new projections. Idem for __hard_ev_projected_CPTs
    for ( const auto node : nodes_with_projected_CPTs_changed ) {
      // perform the projection with a combine and project instance
      const Potential<GUM_SCALAR>& cpt = bn.cpt( node );
      const auto&                  variables = cpt.variablesSequence();
      Set<const DiscreteVariable*> hard_variables;
      __PotentialSet               marg_cpt_set{&cpt};
      for ( const auto var : variables ) {
        const NodeId xnode = bn.nodeId( *var );
        if ( __hard_ev_nodes.exists( xnode ) ) {
          marg_cpt_set.insert( evidence[xnode] );
          hard_variables.insert( var );
        }
      }

      // perform the combination of those potentials and their projection
      MultiDimCombineAndProjectDefault<GUM_SCALAR, Potential> combine_and_project(
          __combination_op, SSNewprojPotential );
      __PotentialSet new_cpt_list =
          combine_and_project.combineAndProject( marg_cpt_set, hard_variables );

      // there should be only one potential in new_cpt_list
      if ( new_cpt_list.size() != 1 ) {
        // remove the CPT created to avoid memory leaks
        for ( auto pot : new_cpt_list ) {
          if ( !marg_cpt_set.contains( pot ) ) delete pot;
        }
        GUM_ERROR( FatalError,
                   "the projection of a potential containing "
                       << "hard evidence is empty!" );
      }
      const Potential<GUM_SCALAR>* projected_cpt = *( new_cpt_list.begin() );
      __clique_potentials[__node_to_clique[node]].insert( projected_cpt );
      __hard_ev_projected_CPTs.insert( node, projected_cpt );
    }

    // here, the list of potentials stored in the invalidated cliques have
    // been updated. So, now, we can combine them to produce the Shafer-Shenoy
    // potential stored into the clique
    MultiDimCombinationDefault<GUM_SCALAR, Potential> fast_combination(
        __combination_op );
    for ( const auto clique : invalidated_cliques ) {
      const auto& potset = __clique_potentials[clique];

      if ( potset.size() > 0 ) {
        // here, there will be an entry in __clique_ss_potential
        // If there is only one element in potset, this element shall be
        // stored into __clique_ss_potential, else all the elements of potset
        // shall be combined and their result shall be stored
        if ( potset.size() == 1 ) {
          __clique_ss_potential[clique] = *( potset.cbegin() );
        } else {
          auto joint = fast_combination.combine( potset );
          __clique_ss_potential[clique] = joint;
        }
      }
    }


    // update the constants
    const auto& hard_evidence = this->hardEvidence();
    for ( auto& node_cst : __constants ) {
      const Potential<GUM_SCALAR>& cpt = bn.cpt( node_cst.first );
      const auto&                  variables = cpt.variablesSequence();
      Instantiation                inst( cpt );
      for ( const auto var : variables ) {
        inst.chgVal( var, hard_evidence[bn.nodeId( *var )] );
      }
      node_cst.second = cpt[inst];
    }

    // indicate that all changes have been performed
    __evidence_changes.clear();
  }


  /// compute a root for each connected component of __JT
  template <typename GUM_SCALAR>
  void ShaferShenoyInference<GUM_SCALAR>::__computeJoinTreeRoots() {
    // get the set of cliques in which we can find the targets and joint_targets
    NodeSet clique_targets;
    for ( const auto node : this->targets() ) {
      try {
        clique_targets.insert( __node_to_clique[node] );
      } catch ( Exception& ) {
      }
    }
    for ( const auto& set : this->jointTargets() ) {
      try {
        clique_targets.insert( __joint_target_to_clique[set] );
      } catch ( Exception& ) {
      }
    }

    // put in a vector these cliques and their size
    std::vector<std::pair<NodeId, Size>> possible_roots( clique_targets.size() );
    const auto& bn = this->BN();
    std::size_t i = 0;
    for ( const auto clique_id : clique_targets ) {
      const auto& clique = __JT->clique( clique_id );
      Size        dom_size = 1;
      for ( const auto node : clique ) {
        dom_size *= bn.variable( node ).domainSize();
      }
      possible_roots[i] = std::pair<NodeId, Size>( clique_id, dom_size );
      ++i;
    }

    // sort the cliques by increasing domain size
    std::sort( possible_roots.begin(),
               possible_roots.end(),
               []( const std::pair<NodeId, Size>& a,
                   const std::pair<NodeId, Size>& b ) -> bool {
                 return a.second < b.second;
               } );

    // pick up the clique with the smallest size in each connected component
    NodeProperty<bool> marked = __JT->nodesProperty( false );
    std::function<void( NodeId, NodeId )> diffuse_marks =
        [&marked, &diffuse_marks, this]( NodeId node, NodeId from ) {
          if ( !marked[node] ) {
            marked[node] = true;
            for ( const auto neigh : __JT->neighbours( node ) )
              if ( ( neigh != from ) && !marked[neigh] )
                diffuse_marks( neigh, node );
          }
        };
    __roots.clear();
    for ( const auto xclique : possible_roots ) {
      const NodeId clique = xclique.first;
      if ( !marked[clique] ) {
        __roots.insert( clique );
        diffuse_marks( clique, clique );
      }
    }
  }


  // performs the collect phase of Lazy Propagation
  template <typename GUM_SCALAR>
  INLINE void
  ShaferShenoyInference<GUM_SCALAR>::__collectMessage( const NodeId id,
                                                       const NodeId from ) {
    for ( const auto other : __JT->neighbours( id ) ) {
      if ( ( other != from ) && !__messages_computed[Arc( other, id )] )
        __collectMessage( other, id );
    }

    if ( ( id != from ) && !__messages_computed[Arc( id, from )] ) {
      __produceMessage( id, from );
    }
  }


  // remove barren variables
  template <typename GUM_SCALAR>
  Set<const Potential<GUM_SCALAR>*>
  ShaferShenoyInference<GUM_SCALAR>::__removeBarrenVariables(
      __PotentialSet& pot_list, Set<const DiscreteVariable*>& del_vars ) {
    // remove from del_vars the variables that received some evidence:
    // only those that did not received evidence can be barren variables
    Set<const DiscreteVariable*> the_del_vars = del_vars;
    for ( auto iter = the_del_vars.beginSafe(); iter != the_del_vars.endSafe();
          ++iter ) {
      NodeId id = this->BN().nodeId( **iter );
      if ( this->hardEvidenceNodes().exists( id ) ||
           this->softEvidenceNodes().exists( id ) ) {
        the_del_vars.erase( iter );
      }
    }

    // assign to each random variable the set of potentials that contain it
    HashTable<const DiscreteVariable*, __PotentialSet> var2pots;
    __PotentialSet empty_pot_set;
    for ( const auto pot : pot_list ) {
      const Sequence<const DiscreteVariable*>& vars = pot->variablesSequence();
      for ( const auto var : vars ) {
        if ( the_del_vars.exists( var ) ) {
          if ( !var2pots.exists( var ) ) {
            var2pots.insert( var, empty_pot_set );
          }
          var2pots[var].insert( pot );
        }
      }
    }

    // each variable with only one potential is a barren variable
    // assign to each potential with barren nodes its set of barren variables
    HashTable<const Potential<GUM_SCALAR>*, Set<const DiscreteVariable*>>
                                 pot2barren_var;
    Set<const DiscreteVariable*> empty_var_set;
    for ( auto elt : var2pots ) {
      if ( elt.second.size() == 1 ) {  // here we have a barren variable
        const Potential<GUM_SCALAR>* pot = *( elt.second.begin() );
        if ( !pot2barren_var.exists( pot ) ) {
          pot2barren_var.insert( pot, empty_var_set );
        }
        pot2barren_var[pot].insert( elt.first );  // insert the barren variable
      }
    }

    // for each potential with barren variables, marginalize them.
    // if the potential has only barren variables, simply remove them from the
    // set of potentials, else just project the potential
    MultiDimProjection<GUM_SCALAR, Potential> projector( SSNewprojPotential );
    __PotentialSet projected_pots;
    for ( auto elt : pot2barren_var ) {
      // remove the current potential from pot_list as, anyway, we will change
      // it
      const Potential<GUM_SCALAR>* pot = elt.first;
      pot_list.erase( pot );

      // check whether we need to add a projected new potential or not (i.e.,
      // whether there exist non-barren variables or not)
      if ( pot->variablesSequence().size() != elt.second.size() ) {
        auto new_pot = projector.project( *pot, elt.second );
        pot_list.insert( new_pot );
        projected_pots.insert( new_pot );
      }
    }

    return projected_pots;
  }


  // remove variables del_vars from the list of potentials pot_list
  template <typename GUM_SCALAR>
  Set<const Potential<GUM_SCALAR>*>
  ShaferShenoyInference<GUM_SCALAR>::__marginalizeOut(
      Set<const Potential<GUM_SCALAR>*> pot_list,
      Set<const DiscreteVariable*>&     del_vars,
      Set<const DiscreteVariable*>&     kept_vars ) {
    // remove the potentials corresponding to barren variables if we want
    // to exploit barren nodes
    __PotentialSet barren_projected_potentials;
    if ( __barren_nodes_type == FindBarrenNodesType::FIND_BARREN_NODES ) {
      barren_projected_potentials = __removeBarrenVariables( pot_list, del_vars );
    }

    // create a combine and project operator that will perform the
    // marginalization
    MultiDimCombineAndProjectDefault<GUM_SCALAR, Potential> combine_and_project(
        __combination_op, __projection_op );
    __PotentialSet new_pot_list =
        combine_and_project.combineAndProject( pot_list, del_vars );

    // remove all the potentials that were created due to projections of
    // barren nodes and that are not part of the new_pot_list: these
    // potentials were just temporary potentials
    for ( auto iter = barren_projected_potentials.beginSafe();
          iter != barren_projected_potentials.endSafe();
          ++iter ) {
      if ( !new_pot_list.exists( *iter ) ) delete *iter;
    }

    // remove all the potentials that have no dimension
    for ( auto iter_pot = new_pot_list.beginSafe();
          iter_pot != new_pot_list.endSafe();
          ++iter_pot ) {
      if ( ( *iter_pot )->variablesSequence().size() == 0 ) {
        // as we have already marginalized out variables that received evidence,
        // it may be the case that, after combining and projecting, some
        // potentials might be empty. In this case, we shall keep their
        // constant and remove them from memory
        // # TODO: keep the constants!
        delete *iter_pot;
        new_pot_list.erase( iter_pot );
      }
    }

    return new_pot_list;
  }


  // creates the message sent by clique from_id to clique to_id
  template <typename GUM_SCALAR>
  void ShaferShenoyInference<GUM_SCALAR>::__produceMessage( const NodeId from_id,
                                                            const NodeId to_id ) {
    // get the potentials of the clique.
    __PotentialSet pot_list;
    if ( __clique_ss_potential.exists( from_id ) )
      pot_list.insert( __clique_ss_potential[from_id] );

    // add the messages sent by adjacent nodes to from_id
    for ( const auto other_id : __JT->neighbours( from_id ) )
      if ( other_id != to_id )
        pot_list += __separator_potentials[Arc( other_id, from_id )];

    // get the set of variables that need be removed from the potentials
    const NodeSet&               from_clique = __JT->clique( from_id );
    const NodeSet&               separator = __JT->separator( from_id, to_id );
    Set<const DiscreteVariable*> del_vars( from_clique.size() );
    Set<const DiscreteVariable*> kept_vars( separator.size() );
    const auto&                  bn = this->BN();

    for ( const auto node : from_clique ) {
      if ( !separator.contains( node ) ) {
        del_vars.insert( &( bn.variable( node ) ) );
      } else {
        kept_vars.insert( &( bn.variable( node ) ) );
      }
    }

    // pot_list now contains all the potentials to multiply and marginalize
    // => combine the messages
    __PotentialSet new_pot_list =
        __marginalizeOut( pot_list, del_vars, kept_vars );

    // remove all the potentials that are equal to ones (as probability
    // matrix multiplications are tensorial, such potentials are useless)
    for ( auto iter = new_pot_list.beginSafe(); iter != new_pot_list.endSafe();
          ++iter ) {
      const auto pot = *iter;
      if ( pot->variablesSequence().size() == 1 ) {
        bool is_all_ones = true;
        for ( Instantiation inst( *pot ); !inst.end(); ++inst ) {
          if ( ( *pot )[inst] < __1_minus_epsilon ) {
            is_all_ones = false;
            break;
          }
        }
        if ( is_all_ones ) {
          if ( !pot_list.exists( pot ) ) delete pot;
          new_pot_list.erase( iter );
          continue;
        }
      }
    }

    // if there are still potentials in new_pot_list, combine them to
    // produce the message on the separator
    const Arc arc( from_id, to_id );
    if ( !new_pot_list.empty() ) {
      if ( new_pot_list.size() == 1 ) {  // there is only one potential
        // in new_pot_list, so this corresponds to our message on
        // the separator
        auto pot = *( new_pot_list.begin() );
        __separator_potentials[arc] = std::move( new_pot_list );
        if ( !pot_list.exists( pot ) ) {
          if ( !__created_potentials.exists( arc ) )
            __created_potentials.insert( arc, __PotentialSet() );
          __created_potentials[arc].insert( pot );
        }
      } else {
        // create the message in the separator
        MultiDimCombinationDefault<GUM_SCALAR, Potential> fast_combination(
            __combination_op );
        auto joint = fast_combination.combine( new_pot_list );
        __separator_potentials[arc].insert( joint );
        if ( !__created_potentials.exists( arc ) )
          __created_potentials.insert( arc, __PotentialSet() );
        __created_potentials[arc].insert( joint );

        // remove the temporary messages created in new_pot_list
        for ( const auto pot : new_pot_list ) {
          if ( !pot_list.exists( pot ) ) {
            delete pot;
          }
        }
      }
    }

    __messages_computed[arc] = true;
  }


  // performs a whole inference
  template <typename GUM_SCALAR>
  INLINE void ShaferShenoyInference<GUM_SCALAR>::_makeInference() {
    // collect messages for all single targets
    for ( const auto node : this->targets() ) {
      // perform only collects in the join tree for nodes that have
      // not received hard evidence (those that received hard evidence were
      // not included into the join tree for speed-up reasons)
      if ( __graph.exists( node ) ) {
        __collectMessage( __node_to_clique[node], __node_to_clique[node] );
      }
    }

    // collect messages for all set targets
    // by parsing  __joint_target_to_clique, we ensure that the cliques that
    // are referenced belong to the join tree (even if some of the nodes in
    // their associated joint_target do not belong to __graph)
    for ( const auto set : __joint_target_to_clique )
      __collectMessage( set.second, set.second );
  }


  /// returns a fresh potential equal to P(1st arg,evidence)
  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>*
  ShaferShenoyInference<GUM_SCALAR>::_unnormalizedJointPosterior(
      const NodeId id ) {
    const auto& bn = this->BN();

    // hard evidence do not belong to the join tree
    // # TODO: check for sets of inconsistent hard evidence
    if ( this->hardEvidenceNodes().contains( id ) ) {
      return new Potential<GUM_SCALAR>( *( this->evidence()[id] ) );
    }

    // if we still need to perform some inference task, do it (this should
    // already have been done by _makeInference)
    const NodeId clique_of_id = __node_to_clique[id];
    __collectMessage( clique_of_id, clique_of_id );

    // now we just need to create the product of the potentials of the clique
    // containing id with the messages received by this clique and
    // marginalize out all variables except id
    __PotentialSet pot_list;
    if ( __clique_ss_potential.exists( clique_of_id ) )
      pot_list.insert( __clique_ss_potential[clique_of_id] );

    // add the messages sent by adjacent nodes to targetCliquxse
    for ( const auto other : __JT->neighbours( clique_of_id ) )
      pot_list += __separator_potentials[Arc( other, clique_of_id )];

    // get the set of variables that need be removed from the potentials
    const NodeSet&               nodes = __JT->clique( clique_of_id );
    Set<const DiscreteVariable*> kept_vars{&( bn.variable( id ) )};
    Set<const DiscreteVariable*> del_vars( nodes.size() );
    for ( const auto node : nodes ) {
      if ( node != id ) del_vars.insert( &( bn.variable( node ) ) );
    }

    // pot_list now contains all the potentials to multiply and marginalize
    // => combine the messages
    __PotentialSet new_pot_list =
        __marginalizeOut( pot_list, del_vars, kept_vars );
    Potential<GUM_SCALAR>* joint = nullptr;

    if ( new_pot_list.size() == 1 ) {
      joint = const_cast<Potential<GUM_SCALAR>*>( *( new_pot_list.begin() ) );
      // if pot already existed, create a copy, so that we can put it into
      // the __target_posteriors property
      if ( pot_list.exists( joint ) ) {
        joint = new Potential<GUM_SCALAR>( *joint );
      } else {
        // remove the joint from new_pot_list so that it will not be
        // removed just after the else block
        new_pot_list.clear();
      }
    } else {
      MultiDimCombinationDefault<GUM_SCALAR, Potential> fast_combination(
          __combination_op );
      joint = fast_combination.combine( new_pot_list );
    }

    // remove the potentials that were created in new_pot_list
    for ( auto pot : new_pot_list )
      if ( !pot_list.exists( pot ) ) delete pot;

    // check that the joint posterior is different from a 0 vector: this would
    // indicate that some hard evidence are not compatible (their joint
    // probability is equal to 0)
    bool nonzero_found = false;
    for ( Instantiation inst( *joint ); !inst.end(); ++inst ) {
      if ( ( *joint )[inst] ) {
        nonzero_found = true;
        break;
      }
    }
    if ( !nonzero_found ) {
      // remove joint from memory to avoid memory leaks
      delete joint;
      GUM_ERROR( IncompatibleEvidence,
                 "some evidence entered into the Bayes "
                 "net are incompatible (their joint proba = 0)" );
    }
    return joint;
  }


  /// returns the posterior of a given variable
  template <typename GUM_SCALAR>
  const Potential<GUM_SCALAR>&
  ShaferShenoyInference<GUM_SCALAR>::_posterior( const NodeId id ) {
    // check if we have already computed the posterior
    if ( __target_posteriors.exists( id ) ) {
      return *( __target_posteriors[id] );
    }

    // compute the joint posterior and normalize
    auto joint = _unnormalizedJointPosterior( id );
    joint->normalize();
    __target_posteriors.insert( id, joint );

    return *joint;
  }


  // returns the marginal a posteriori proba of a given node
  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>*
  ShaferShenoyInference<GUM_SCALAR>::_unnormalizedJointPosterior(
      const NodeSet& set ) {
    // hard evidence do not belong to the join tree, so extract the nodes
    // from targets that are not hard evidence
    NodeSet targets = set, hard_ev_nodes;
    for ( const auto node : this->hardEvidenceNodes() ) {
      if ( targets.contains( node ) ) {
        targets.erase( node );
        hard_ev_nodes.insert( node );
      }
    }

    // if all the nodes have received hard evidence, then compute the
    // joint posterior directly by multiplying the hard evidence potentials
    const auto& evidence = this->evidence();
    if ( targets.empty() ) {
      __PotentialSet pot_list;
      for ( const auto node : set ) {
        pot_list.insert( evidence[node] );
      }
      MultiDimCombinationDefault<GUM_SCALAR, Potential> fast_combination(
          __combination_op );
      return fast_combination.combine( pot_list );
    }


    // if we still need to perform some inference task, do it: so, first,
    // determine the clique on which we should perform collect to compute
    // the unnormalized joint posterior of a set of nodes containing "set"
    NodeId clique_of_set;
    try {
      clique_of_set = __joint_target_to_clique[set];
    } catch ( NotFound& ) {
      // here, the precise set of targets does not belong to the set of targets
      // defined by the user. So we will try to find a clique in the junction
      // tree that contains "targets":

      // 1/ we should check that all the nodes belong to the join tree
      for ( const auto node : targets ) {
        if ( !__graph.exists( node ) ) {
          GUM_ERROR( UndefinedElement, node << " is not a target node" );
        }
      }

      // 2/ the clique created by the first eliminated node among target is the
      // one we are looking for
      const std::vector<NodeId>& JT_elim_order =
          __triangulation->eliminationOrder();
      NodeProperty<int> elim_order( Size( JT_elim_order.size() ) );
      for ( std::size_t i = std::size_t( 0 ), size = JT_elim_order.size();
            i < size;
            ++i )
        elim_order.insert( JT_elim_order[i], (int)i );
      NodeId first_eliminated_node = *( targets.begin() );
      int    elim_number = elim_order[first_eliminated_node];
      for ( const auto node : targets ) {
        if ( elim_order[node] < elim_number ) {
          elim_number = elim_order[node];
          first_eliminated_node = node;
        }
      }
      clique_of_set =
        __triangulation->createdJunctionTreeClique( first_eliminated_node );

      // 3/ check that cliquee_of_set contains the all the nodes in the target
      const NodeSet& clique_nodes = __JT->clique( clique_of_set );
      for ( const auto node : targets ) {
        if ( !clique_nodes.contains( node ) ) {
          GUM_ERROR( UndefinedElement, set << " is not a joint target" );
        }
      }

      // add the discovered clique to __joint_target_to_clique
      __joint_target_to_clique.insert ( set, clique_of_set );
    }

    // now perform a collect on the clique
    __collectMessage( clique_of_set, clique_of_set );

    // now we just need to create the product of the potentials of the clique
    // containing set with the messages received by this clique and
    // marginalize out all variables except set
    __PotentialSet pot_list;
    if ( __clique_ss_potential.exists( clique_of_set ) )
      pot_list.insert( __clique_ss_potential[clique_of_set] );

    // add the messages sent by adjacent nodes to targetClique
    for ( const auto other : __JT->neighbours( clique_of_set ) )
      pot_list += __separator_potentials[Arc( other, clique_of_set )];

    // get the set of variables that need be removed from the potentials
    const NodeSet&               nodes = __JT->clique( clique_of_set );
    Set<const DiscreteVariable*> del_vars( nodes.size() );
    Set<const DiscreteVariable*> kept_vars( targets.size() );
    const auto&                  bn = this->BN();
    for ( const auto node : nodes ) {
      if ( !targets.contains( node ) ) {
        del_vars.insert( &( bn.variable( node ) ) );
      } else {
        kept_vars.insert( &( bn.variable( node ) ) );
      }
    }

    // pot_list now contains all the potentials to multiply and marginalize
    // => combine the messages
    __PotentialSet new_pot_list =
        __marginalizeOut( pot_list, del_vars, kept_vars );
    Potential<GUM_SCALAR>* joint = nullptr;

    if ( ( new_pot_list.size() == 1 ) && hard_ev_nodes.empty() ) {
      joint = const_cast<Potential<GUM_SCALAR>*>( *( new_pot_list.begin() ) );
      // if pot already existed, create a copy, so that we can put it into
      // the __target_posteriors property
      if ( pot_list.exists( joint ) ) {
        joint = new Potential<GUM_SCALAR>( *joint );
      } else {
        // remove the joint from new_pot_list so that it will not be
        // removed just after the next else block
        new_pot_list.clear();
      }
    } else {
      // combine all the potentials in new_pot_list with all the hard evidence
      // of the nodes in set
      __PotentialSet new_new_pot_list = new_pot_list;
      for ( const auto node : hard_ev_nodes ) {
        new_new_pot_list.insert( evidence[node] );
      }
      MultiDimCombinationDefault<GUM_SCALAR, Potential> fast_combination(
          __combination_op );
      joint = fast_combination.combine( new_new_pot_list );
    }

    // remove the potentials that were created in new_pot_list
    for ( auto pot : new_pot_list )
      if ( !pot_list.exists( pot ) ) delete pot;

    // check that the joint posterior is different from a 0 vector: this would
    // indicate that some hard evidence are not compatible
    bool nonzero_found = false;
    for ( Instantiation inst( *joint ); !inst.end(); ++inst ) {
      if ( ( *joint )[inst] ) {
        nonzero_found = true;
        break;
      }
    }
    if ( !nonzero_found ) {
      // remove joint from memory to avoid memory leaks
      delete joint;
      GUM_ERROR( IncompatibleEvidence,
                 "some evidence entered into the Bayes "
                 "net are incompatible (their joint proba = 0)" );
    }

    return joint;
  }


  /// returns the posterior of a given set of variables
  template <typename GUM_SCALAR>
  const Potential<GUM_SCALAR>&
  ShaferShenoyInference<GUM_SCALAR>::_jointPosterior( const NodeSet& set ) {
    // check if we have already computed the posterior
    if ( __joint_target_posteriors.exists( set ) ) {
      return *( __joint_target_posteriors[set] );
    }

    // compute the joint posterior and normalize
    auto joint = _unnormalizedJointPosterior( set );
    joint->normalize();
    __joint_target_posteriors.insert( set, joint );

    return *joint;
  }


  /// returns the posterior of a given set of variables
  template <typename GUM_SCALAR>
  const Potential<GUM_SCALAR>& ShaferShenoyInference<GUM_SCALAR>::_jointPosterior(
      const NodeSet& wanted_target, const NodeSet& declared_target ) {
    // check if we have already computed the posterior of wanted_target
    if ( __joint_target_posteriors.exists( wanted_target ) )
      return *( __joint_target_posteriors[wanted_target] );

    // here, we will have to compute the posterior of declared_target and
    // marginalize out all the variables that do not belong to wanted_target

    // check if we have already computed the posterior of declared_target
    if ( !__joint_target_posteriors.exists( declared_target ) ) {
      _jointPosterior( declared_target );
    }

    // marginalize out all the variables that do not belong to wanted_target
    const auto&                  bn = this->BN();
    Set<const DiscreteVariable*> del_vars;
    for ( const auto node : declared_target )
      if ( !wanted_target.contains( node ) )
        del_vars.insert( &( bn.variable( node ) ) );
    Potential<GUM_SCALAR>* pot = new Potential<GUM_SCALAR>(
        __joint_target_posteriors[declared_target]->margSumOut( del_vars ) );

    // save the result into the cache
    __joint_target_posteriors.insert( wanted_target, pot );

    return *pot;
  }


  template <typename GUM_SCALAR>
  GUM_SCALAR ShaferShenoyInference<GUM_SCALAR>::evidenceProbability() {
    // perform inference in each connected component
    this->makeInference();

    // for each connected component, select a variable X and compute the
    // joint probability of X and evidence e. Then marginalize-out X to get
    // p(e) in this connected component. Finally, multiply all the p(e) that
    // we got and the elements in  __constants. The result is the probability
    // of evidence

    GUM_SCALAR prob_ev = 1;
    for ( const auto root : __roots ) {
      // get a node in the clique
      const NodeId           node = *( __JT->clique( root ).begin() );
      Potential<GUM_SCALAR>* tmp = _unnormalizedJointPosterior( node );
      GUM_SCALAR             sum = 0;
      for ( Instantiation iter( *tmp ); !iter.end(); ++iter )
        sum += tmp->get( iter );
      prob_ev *= sum;
      delete tmp;
    }

    for ( const auto& projected_cpt : __constants )
      prob_ev *= projected_cpt.second;

    return prob_ev;
  }


} /* namespace gum */

#endif  // DOXYGEN_SHOULD_SKIP_THIS
// kate: indent-mode cstyle; indent-width 2; replace-tabs on;
