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
 * @brief Implementation of lazy propagation for inference
 * Bayesian Networks.
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <utility>

#include <agrum/config.h>
#include <agrum/multidim/instantiation.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/BayesBall.h>
#include <agrum/BN/inference/dSeparation.h>
#include <agrum/multidim/operators/multiDimProjection.h>
#include <agrum/multidim/operators/multiDimCombineAndProjectDefault.h>
#include <agrum/graphs/binaryJoinTreeConverterDefault.h>
#include <agrum/BN/inference/barrenNodesFinder.h>


namespace gum {
  
  
  // default constructor
  template <typename GUM_SCALAR>
  INLINE LazyPropagation<GUM_SCALAR>::LazyPropagation
  ( const IBayesNet<GUM_SCALAR>* BN,
    FindRelevantPotentialsType relevant_type,
    FindBarrenNodesType barren_type,
    bool use_binary_join_tree ) :
    Inference<GUM_SCALAR>( BN ),
    __use_binary_join_tree ( use_binary_join_tree ) {
    // sets the relevant potential and the barren nodes finding algorithm
    setFindRelevantPotentialsType ( relevant_type );
    setFindBarrenNodesType ( barren_type );

    // create a default triangulation (the user can change it afterwards)
    __triangulation = new DefaultTriangulation;

    // for debugging purposessetRequiredInference
    GUM_CONSTRUCTOR( LazyPropagation );
  }


  // destructor
  template <typename GUM_SCALAR>
  INLINE LazyPropagation<GUM_SCALAR>::~LazyPropagation() {    
    // remove all the potentials created during the last message passing
    for ( const auto& pots : __created_potentials )
      for ( const auto pot : pots.second )
        delete pot;

    // remove the potentials created after removing the nodes that received
    // hard evidence
    for ( const auto& pot : __hard_ev_projected_CPTs )
      delete pot.second;

    // remove the junction tree and the triangulation algorithm
    if ( __JT != nullptr ) delete __JT;
    delete __triangulation;

    // for debugging purposes
    GUM_DESTRUCTOR( LazyPropagation );
  }

  
  /// set a new triangulation algorithm
  template <typename GUM_SCALAR>
  void LazyPropagation<GUM_SCALAR>::setTriangulation
  ( const Triangulation& new_triangulation ) {
    delete __triangulation;
    __triangulation = new_triangulation.newFactory ();
    __is_new_jt_needed = true;
    this->_setUnpreparedStructureState ();
  }
  
  
  /// returns the current join tree used
  template <typename GUM_SCALAR>
  INLINE const JoinTree*
  LazyPropagation<GUM_SCALAR>::joinTree() const {
    return __JT;
  }


  /// sets how we determine the relevant potentials to combine
  template <typename GUM_SCALAR>
  void LazyPropagation<GUM_SCALAR>::setFindRelevantPotentialsType
  ( FindRelevantPotentialsType type ) {
    switch ( type ) {
    case FIND_RELEVANT_D_SEPARATION2:
      __findRelevantPotentials =
        &LazyPropagation<GUM_SCALAR>::__findRelevantPotentialsWithdSeparation2;
      break;

    case FIND_RELEVANT_D_SEPARATION:
      __findRelevantPotentials =
        &LazyPropagation<GUM_SCALAR>::__findRelevantPotentialsWithdSeparation;
      break;

    case FIND_RELEVANT_D_SEPARATION3:
      __findRelevantPotentials =
        &LazyPropagation<GUM_SCALAR>::__findRelevantPotentialsWithdSeparation3;
      break;

    case FIND_RELEVANT_ALL:
      __findRelevantPotentials =
        &LazyPropagation<GUM_SCALAR>::__findRelevantPotentialsGetAll;
      break;

    default:
      GUM_ERROR( InvalidArgument,
                 "setFindRelevantPotentialsType for type " << type
                 << " is not implemented yet" );
    }

    __find_relevant_potential_type = type;
  }


  /// sets how we determine barren nodes
  template <typename GUM_SCALAR>
  void LazyPropagation<GUM_SCALAR>::setFindBarrenNodesType
  ( FindBarrenNodesType type ) {
    // WARNING: if a new type is added here, method __createJT should certainly
    // be updated as well, in particular its step 2.
    switch ( type ) {
    case FIND_BARREN_NODES:
    case FIND_NO_BARREN_NODES:
      break;

    default:
      GUM_ERROR( InvalidArgument,
                 "setFindBarrenNodesType for type " << type
                 << " is not implemented yet" );
    }
      
    __barren_nodes_type = type;
  }

  
  /// fired when a new evidence is inserted
  template <typename GUM_SCALAR>
  INLINE void
  LazyPropagation<GUM_SCALAR>::_onEvidenceAdded ( const NodeId id,
                                                  bool isHardEvidence ) {
    // if we have a new hard evidence, this modifies the undigraph over which
    // the join tree is created. This is also the case if id is not a node of
    // of the undigraph
    if ( isHardEvidence || ! __graph.exists ( id ) )
      __is_new_jt_needed = true;
    else {
      try {
        __evidence_changes.insert ( id, EvidenceChangeType::EVIDENCE_ADDED );
      }
      catch ( DuplicateElement& ) {
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
  LazyPropagation<GUM_SCALAR>::_onEvidenceErased ( const NodeId id,
                                                   bool isHardEvidence ) {
    // if we delete a hard evidence, this modifies the undigraph over which
    // the join tree is created.
    if ( isHardEvidence )
      __is_new_jt_needed = true;
    else {
      try {
        __evidence_changes.insert ( id, EvidenceChangeType::EVIDENCE_ERASED );
      }
      catch ( DuplicateElement& ) {
        // here, the evidence change already existed and it is necessarily an
        // EVIDENCE_ADDED or an EVIDENCE_MODIFIED. So, if the evidence has
        // been added and is now erased, this is similar to not having created
        // it. If the evidence was only modified, it already existed in the
        // last inference and we should now indicate that it has been removed.
        if ( __evidence_changes[id] == EvidenceChangeType::EVIDENCE_ADDED )
          __evidence_changes.erase ( id );
        else
          __evidence_changes[id] = EvidenceChangeType::EVIDENCE_ERASED;
      }
    }
  }
   

  /// fired when all the evidence are erased
  template <typename GUM_SCALAR>
  void
  LazyPropagation<GUM_SCALAR>::_onAllEvidenceErased ( bool has_hard_evidence ) {
    if ( has_hard_evidence || ! this->hardEvidenceNodes ().empty () )
      __is_new_jt_needed = true;
    else {
      for ( const auto node : this->softEvidenceNodes () ) {
        try {
          __evidence_changes.insert ( node, EvidenceChangeType::EVIDENCE_ERASED );
        }
        catch ( DuplicateElement& ) {
          // here, the evidence change already existed and it is necessarily an
          // EVIDENCE_ADDED or an EVIDENCE_MODIFIED. So, if the evidence has
        // been added and is now erased, this is similar to not having created
        // it. If the evidence was only modified, it already existed in the
        // last inference and we should now indicate that it has been removed.
          if ( __evidence_changes[node] == EvidenceChangeType::EVIDENCE_ADDED )
            __evidence_changes.erase ( node );
          else
            __evidence_changes[node] = EvidenceChangeType::EVIDENCE_ERASED;
        }
      }
    }
  }
   
     
  /// fired when an evidence is changed
  template <typename GUM_SCALAR>
  INLINE void
  LazyPropagation<GUM_SCALAR>::_onEvidenceChanged( const NodeId id,
                                                   bool hasChangedSoftHard ) {
    if ( hasChangedSoftHard )
      __is_new_jt_needed = true;
    else {
      try {
        __evidence_changes.insert ( id, EvidenceChangeType::EVIDENCE_MODIFIED );
      }
      catch ( DuplicateElement& ) {
        // here, the evidence change already existed and it is necessarily an
        // EVIDENCE_ADDED. So we should keep this state to indicate that this
        // evidence is new w.r.t. the last inference
      }
    }
  }
        

  // check whether a new junction tree is really needed for the next inference
  template <typename GUM_SCALAR>
  bool LazyPropagation<GUM_SCALAR>::__isNewJTNeeded () const {
    // if we do not have a JT or if __new_jt_needed is set to true, then
    // we know that we need to create a new join tree
    if ( ( __JT == nullptr ) || __is_new_jt_needed ) return true;

    // if some some targets do not belong to the join tree and, consequently,
    // to the undigraph that was used to construct the join tree, then we need
    // to create a new JT. This situation may occur if we constructed the
    // join tree after pruning irrelevant nodes from the BN)
    for ( const auto node : this->targets () ) {
      if ( ! __graph.exists ( node ) ) return true;
    }
    for ( const auto& nodes : this->setTargets() ) {
      // here, we need to check that at least one clique contains all the nodes.
      bool containing_clique_found = false;
      for ( const auto node : nodes ) {
        bool found = true;
        const NodeSet& clique = __JT->clique ( __node_to_clique[node] );
        for ( const auto xnode : nodes ) {
          if ( ! clique.contains ( xnode ) ) {
            found = false;
            break;
          }
        }
        if ( found ) {
          containing_clique_found = true;
          break;
        }
      }
 
      if ( ! containing_clique_found ) return true;
    }

    // if some new evidence have been added on nodes that do not belong to
    // __graph, then we potentially have to reconstruct the join tree
    for ( const auto& change : __evidence_changes ) {
      if ( ( change.second == EvidenceChangeType::EVIDENCE_ADDED ) &&
           ( ! __graph.exists ( change.first ) ) )
        return true;
    }
    
    // here, the current JT is exactly what we need for the next inference
    return false;
  }

  
  /// create a new junction tree as well as its related data structures
  template <typename GUM_SCALAR>
  void LazyPropagation<GUM_SCALAR>::__createNewJT () {
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
    const auto& bn = this->BayesNet ();
    __graph.clear ();
    for ( auto node : bn.dag () )
      __graph.addNode ( node );

    // 2/ if we wish to exploit barren nodes, we shall remove them from the BN
    // to do so: we identify all the nodes that are not targets and have received
    // no evidence and such that their descendants are neither targets nor
    // evidence nodes. Such nodes can be safely discarded from the BN without
    // altering the inference output
    if ( ( __barren_nodes_type == FindBarrenNodesType::FIND_BARREN_NODES ) &&
         ! this->targets ().empty () ) {
      // identify the barren nodes
      NodeSet target_nodes = this->targets ();
      for ( const auto& nodeset : this->setTargets () ) {
        for ( const auto node : nodeset ) {
          target_nodes.insert ( node );
        }
      }
      
      if ( target_nodes.size () != bn.dag().sizeNodes () ) {
        BarrenNodesFinder finder( &( bn.dag() ) );
        finder.setTargets ( &target_nodes );

        NodeSet evidence_nodes;
        for ( const auto& pair : this->evidence () ) {
          evidence_nodes.insert ( pair.first );
        }
        finder.setEvidence ( &evidence_nodes );

        NodeSet barren_nodes = finder.barrenNodes ();

        // remove the barren nodes from the moral graph
        for ( const auto node : barren_nodes ) {
          __graph.eraseNode ( node );
        }
      }
    }

    // 3/ add edges so that each node and its parents in the BN form a clique
    for ( const auto node : __graph ) {
      const NodeSet& parents = bn.dag().parents ( node );
      for ( auto iter1 = parents.cbegin (); iter1 != parents.cend (); ++iter1 ) {
        __graph.addEdge ( *iter1, node );
        auto iter2 = iter1;
        for ( ++iter2; iter2 != parents.cend (); ++iter2 ) {
          __graph.addEdge ( *iter1, *iter2 );
        }
      }
    }

    // 4/ if there exist some set targets, we shall add new edges into the moral
    // graph in order to ensure that there exists a clique containing each set
    for ( const auto& nodeset : this->setTargets () ) {
      for ( auto iter1 = nodeset.cbegin (); iter1 != nodeset.cend (); ++iter1 ) {
        auto iter2 = iter1;
        for ( ++iter2; iter2 != nodeset.cend (); ++iter2 ) {
          __graph.addEdge ( *iter1, *iter2 );
        }
      }
    }

    // 5/ remove all the nodes that received hard evidence
    __hard_ev_nodes = this->hardEvidenceNodes ();
    for ( const auto node : __hard_ev_nodes) {
      __graph.eraseNode ( node );
    }

    // now, we can compute the new junction tree. To speed-up computations
    // (essentially, those of the distribution phase), we construct this
    // junction tree a binary join tree
    if ( __JT != nullptr ) delete __JT;
    __triangulation->setGraph ( &__graph, &( this->domainSizes() ) );
    const JunctionTree& triang_jt = __triangulation->junctionTree ();
    if ( __use_binary_join_tree ) {
      BinaryJoinTreeConverterDefault bjt_converter;
      NodeSet emptyset;
      __JT = new CliqueGraph( bjt_converter.convert( triang_jt,
                                                     this->domainSizes(),
                                                     emptyset ) );
    }
    else {
      __JT = new CliqueGraph( triang_jt );
    }

    // indicate, for each node of the moral graph, a clique in __JT that can
    // contain its conditional probability table
    const std::vector<NodeId>& JT_elim_order = __triangulation->eliminationOrder();
    NodeProperty<int> elim_order( JT_elim_order.size() );
    for ( std::size_t i = 0, size = JT_elim_order.size(); i < size; ++i )
      elim_order.insert( JT_elim_order[i], i );
    const DAG& dag = bn.dag();
    __node_to_clique.clear ();
    for ( const auto node : __graph ) {
      // get the variables in the potential of node (and its parents)
      NodeId first_eliminated_node = node;
      int elim_number = elim_order[node];

      for ( const auto parent : dag.parents( node ) ) {
        if ( elim_order[parent] < elim_number ) {
          elim_number = elim_order[parent];
          first_eliminated_node = parent;
        }
      }

      // first_eliminated_node contains the first var (node or one of its
      // parents) eliminated => the clique created during its elimination
      // contains node and all of its parents => it can contain the potential
      // assigned to the node in the BN
      __node_to_clique.insert
        ( node,
          __triangulation->createdJunctionTreeClique( first_eliminated_node ) );
    }

    // indicate for each settarget a clique that contains it
    __settarget_to_clique.clear ();
    for ( const auto& nodeset : this->setTargets() ) {
      NodeId first_eliminated_node = *( nodeset.begin () );
      int elim_number = elim_order[first_eliminated_node];
      for ( const auto node : nodeset ) {
        if ( elim_order[node] < elim_number ) {
          elim_number = elim_order[node];
          first_eliminated_node = node;
        }
      }
      __settarget_to_clique.insert ( nodeset,
                                     __node_to_clique[first_eliminated_node] );
    }
    
      
    // compute the roots of __JT's connected components
    __computeJoinTreeRoots ();


    // create empty potential lists into the cliques of the joint tree as well
    // as empty lists of evidence
    __PotentialSet empty_set;

    __clique_potentials.clear ();
    __node_to_soft_evidence.clear ();
    for ( const auto node : *__JT ) {
      __clique_potentials.insert( node, empty_set );
    }

    // remove all the potentials created during the last inference
    for ( auto& potlist : __created_potentials )
      for ( auto pot : potlist.second )
        delete pot;
    __created_potentials.clear ();

    // remove all the potentials created to take into account hard evidence
    // during the last inference
    for ( auto pot_pair : __hard_ev_projected_CPTs )
      delete pot_pair.second;
    __hard_ev_projected_CPTs.clear ();

    // remove all the constants created due to projections of CPTs that were
    // defined over only hard evidence nodes
    __constants.clear ();
    
    // create empty lists of potentials for the messages and indicate that no
    // message has been computed yet
    __separator_potentials.clear ();
    __messages_computed.clear ();
    for ( const auto& edge : __JT->edges () ) {
      const Arc arc1 ( edge.first (), edge.second () );
      __separator_potentials.insert ( arc1, empty_set );
      __messages_computed.insert ( arc1, false );
      const Arc arc2 ( Arc ( edge.second (), edge.first () ) );
      __separator_potentials.insert ( arc2, empty_set );
      __messages_computed.insert ( arc2, false );
    }

    
    // put all the CPT's of the Bayes net nodes into the cliques
    // here, beware: all the potentials that are defined over some nodes including
    // hard evidence must be projected so that these nodes are removed from the
    // potential
    const auto& evidence = this->evidence ();
    const auto& hard_evidence = this->hardEvidence ();
    for ( const auto node : __graph ) {
      const Potential<GUM_SCALAR>& cpt = bn.cpt( node );

      // get the list of nodes with hard evidence in cpt
      NodeSet hard_nodes;
      const auto& variables = cpt.variablesSequence ();
      for ( const auto var : variables ) {
        const NodeId xnode = bn.nodeId ( *var );
        if ( __hard_ev_nodes.contains ( xnode ) )
          hard_nodes.insert ( xnode );
      }

      // if hard_nodes contains hard evidence nodes, perform a projection
      // and insert the result into the appropriate clique, else insert
      // directly cpt into the clique
      if ( hard_nodes.empty () ) {
        __clique_potentials[__node_to_clique[node]].insert( &cpt );
      }
      else {
        // marginalize out the hard evidence nodes: if the cpt is defined
        // only over nodes that received hard evidence, do not consider it
        // as a potential anymore but as a constant
        if ( hard_nodes.size () == variables.size () ) {
          Instantiation inst ( cpt );
          for ( Size i = 0; i < hard_nodes.size (); ++i ) {
            inst.chgVal ( variables[i],
                          hard_evidence[bn.nodeId( *( variables[i] ) )] );
          }
          __constants.insert ( node, cpt[inst] );
        }
        else {
          // perform the projection with a combine and project instance
          Set<const DiscreteVariable*> hard_variables;
          __PotentialSet marg_cpt_set { &cpt };
          for ( const auto xnode : hard_nodes ) {
            marg_cpt_set.insert( evidence[xnode] );
            hard_variables.insert ( &( bn.variable ( xnode ) ) );
          }
          // perform the combination of those potentials and their projection
          MultiDimCombineAndProjectDefault<GUM_SCALAR, Potential>
            combine_and_project( __combination_op, LPNewprojPotential );
          __PotentialSet new_cpt_list =
            combine_and_project.combineAndProject( marg_cpt_set, hard_variables );

          // there should be only one potential in new_cpt_list
          if ( new_cpt_list.size () != 1 ) {
            GUM_ERROR ( FatalError, "the projection of a potential containing "
                        << "hard evidence is empty!" );
          }
          const Potential<GUM_SCALAR>* projected_cpt = *( new_cpt_list.begin () );
          __clique_potentials[__node_to_clique[node]].insert ( projected_cpt );
          __hard_ev_projected_CPTs.insert ( node, projected_cpt );
        }
      }
    }


    // we shall now add all the potentials of the soft evidence
    for ( const auto node : this->softEvidenceNodes () ) {
      __node_to_soft_evidence.insert ( node, evidence[node] );
      __clique_potentials[__node_to_clique[node]].insert ( evidence[node] );
    }
  }


  /// prepare the inference structures w.r.t. new targets, soft/hard evidence
  template <typename GUM_SCALAR>
  void LazyPropagation<GUM_SCALAR>::_prepareInferenceStructure () {
    // check if a new JT is really needed. If so, create it
    if ( __isNewJTNeeded () ) {
      __createNewJT ();
    }
    else {
      // here, we can answer the next queries without reconstructing all the
      // junction tree. All we need to do is to indicate that we should
      // update the potentials and messages for these queries
      _updateInferencePotentials ();
    }
  }


  /// invalidate all the messages sent from a given clique
  template <typename GUM_SCALAR>
  void LazyPropagation<GUM_SCALAR>::__diffuseMessageInvalidations
  ( const NodeId from,
    const NodeId to ) {
    // invalidate the current arc
    const Arc arc ( from, to );
    bool& message_computed =  __messages_computed[arc];
    if ( message_computed ) {
      message_computed = false;
      __separator_potentials[arc].clear ();
      if (  __created_potentials.exists ( arc ) ) {
        auto& arc_created_potentials = __created_potentials[arc];
        for ( auto pot : arc_created_potentials )
          delete pot;
        arc_created_potentials.clear ();
      }
      
      // go on with the diffusion
      for ( const auto node : __JT->neighbours ( to ) ) {
        if ( node != from )
          __diffuseMessageInvalidations ( to, node );
      }
    }
  }


  /// update the potentials stored in the cliques and invalidate outdated messages
  template <typename GUM_SCALAR>
  void LazyPropagation<GUM_SCALAR>::_updateInferencePotentials () {
    // get the set of nodes that received evidence changes
    
    
    // invalidate all the messages that are no more correct: start from each of
    // the nodes whose evidence has changed and perform a diffusion from the
    // clique into which the evidence has been entered, indicating that the
    // messages spreading from this clique are now invalid. At the same time,
    // if there were potentials created on the arcs over wich the messages were
    // sent, remove them from memory
    for ( const auto& pair : __evidence_changes ) {
      const auto clique = __node_to_clique[pair.first];
      for ( const auto neighbor : __JT->neighbours ( clique ) ) {
        __diffuseMessageInvalidations ( clique, neighbor );
      }
    }

    // remove all the evidence that were entered into __node_to_soft_evidence
    // and __clique_potentials and add the new soft ones
    for ( auto& pot_pair : __node_to_soft_evidence ) {
      auto& clique_potentials =
        __clique_potentials[__node_to_clique[pot_pair.first]];
      clique_potentials.erase ( pot_pair.second );
    }
    __node_to_soft_evidence.clear ();

    const auto& evidence = this->evidence ();
    for ( const auto node : this->softEvidenceNodes () ) {
      __node_to_soft_evidence.insert ( node, evidence[node] );
      __clique_potentials[__node_to_clique[node]].insert ( evidence[node] );
    }


    // now we shall remove the projections of the CPTs due to hard evidence
    // when the latter have changed
    NodeSet hard_nodes_changed ( __hard_ev_nodes.size () );
    for ( const auto node : __hard_ev_nodes )
      if ( __evidence_changes.exists ( node ) )
        hard_nodes_changed.insert ( node );

    NodeSet nodes_with_projected_CPTs_changed;
    const auto& bn = this->BayesNet ();
    for ( auto pot_iter = __hard_ev_projected_CPTs.beginSafe ();
          pot_iter != __hard_ev_projected_CPTs.endSafe (); ++pot_iter ) {
      const auto pot = pot_iter.val ();
      const auto& variables = pot->variablesSequence ();
      for ( const auto var : variables ) {
        if ( hard_nodes_changed.contains ( bn.nodeId ( *var ) ) ) {
          delete pot;
          nodes_with_projected_CPTs_changed.insert ( pot_iter.key () );
          __hard_ev_projected_CPTs.erase ( pot_iter );
          break;
        }
      }
    }
    
    // Now add the projections of the CPTs due to newly changed hard evidence: 
    // if we are performing _updateInferencePotentials, this means that the
    // set of nodes that received hard evidence has not been changed, only
    // their instantiations can have been changed. So, if there is an entry
    // for node in __constants, there will still be such an entry after
    // performing the new projections. Idem for __hard_ev_projected_CPTs
    for ( const auto node : nodes_with_projected_CPTs_changed ) {
      // perform the projection with a combine and project instance
      const Potential<GUM_SCALAR>& cpt = bn.cpt( node );
      const auto& variables = cpt.variablesSequence ();
      Set<const DiscreteVariable*> hard_variables;
      __PotentialSet marg_cpt_set { &cpt };
      for ( const auto var : variables ) {
        const NodeId xnode = bn.nodeId( *var );
        if ( __hard_ev_nodes.exists ( xnode ) ) {
          marg_cpt_set.insert( evidence[xnode] );
          hard_variables.insert ( &( bn.variable ( xnode ) ) );
        }
      }
      // perform the combination of those potentials and their projection
      MultiDimCombineAndProjectDefault<GUM_SCALAR, Potential>
        combine_and_project( __combination_op, LPNewprojPotential );
      __PotentialSet new_cpt_list =
        combine_and_project.combineAndProject( marg_cpt_set, hard_variables );

      // there should be only one potential in new_cpt_list
      if ( new_cpt_list.size () != 1 ) {
        GUM_ERROR ( FatalError, "the projection of a potential containing "
                    << "hard evidence is empty!" );
      }
      const Potential<GUM_SCALAR>* projected_cpt = *( new_cpt_list.begin () );
      __clique_potentials[__node_to_clique[node]].insert ( projected_cpt );
      __hard_ev_projected_CPTs.insert ( node, projected_cpt );
    }

    // update the constants
    const auto& hard_evidence = this->hardEvidence ();
    for ( auto& node_cst : __constants ) {
      const Potential<GUM_SCALAR>& cpt = bn.cpt( node_cst.first );
      const auto& variables = cpt.variablesSequence ();
      Instantiation inst ( cpt );
      for ( const auto var : variables ) {
        inst.chgVal ( var, hard_evidence[bn.nodeId( *var )] );
      }
      node_cst.second = cpt[inst];
    }

    // indicate that all changes have been performed
    __evidence_changes.clear ();
  }

      
  /// compute a root for each connected component of __JT
  template <typename GUM_SCALAR>
  void LazyPropagation<GUM_SCALAR>::__computeJoinTreeRoots () {
    // get the set of cliques in which we can find the targets and settargets
    NodeSet clique_targets;
    for ( const auto node : this->targets () )
      clique_targets.insert ( __node_to_clique[node] );
    for ( const auto& set : this->setTargets () )
      clique_targets.insert ( __settarget_to_clique[set] );

    // put in a vector these cliques and their size
    std::vector<std::pair<NodeId,Size>> possible_roots ( clique_targets.size () );
    const auto& bn = this->BayesNet ();
    std::size_t i = 0;
    for ( const auto clique_id : clique_targets ) {
      const auto& clique = __JT->clique ( clique_id );
      Size dom_size = 1;
      for ( const auto node : clique ) {
        dom_size *= bn.variable ( node ).domainSize (); 
      }
      possible_roots[i++] = std::pair<NodeId,Size> ( clique_id, dom_size );
    }

    // sort the cliques by increasing domain size
    std::sort ( possible_roots.begin (), possible_roots.end (),
                []( const std::pair<NodeId,Size>& a,
                    const std::pair<NodeId,Size>& b ) -> bool {
                  return a.second < b.second;
                } );

    // pick up the clique with the smallest size in each connected component
    NodeProperty<bool> marked = __JT->nodesProperty ( false );
    std::function<void(NodeId,NodeId)> diffuse_marks =
      [&marked,&diffuse_marks,this]( NodeId node, NodeId from ) {
      if ( ! marked[node] ) {
        marked[node] = true;
        for ( const auto neigh : __JT->neighbours ( node ) )
          if ( ( neigh != from ) && ! marked[neigh] )
            diffuse_marks ( neigh, node );
      }
    };
    __roots.clear ();
    for ( const auto xclique : possible_roots ) {
      const NodeId clique = xclique.first;
      if ( ! marked[clique] ) {
        __roots.insert ( clique );
        diffuse_marks ( clique, clique );
      }
    }
  }

  
  // performs the collect phase of Lazy Propagation
  template <typename GUM_SCALAR>
  INLINE void
  LazyPropagation<GUM_SCALAR>::__collectMessage ( const NodeId id,
                                                  const NodeId from ) {
    for ( const auto other : __JT->neighbours( id ) ) {
      if ( ( other != from ) && ! __messages_computed[Arc ( other, id )] )
        __collectMessage ( other, id );
    }
    
    if ( (id != from ) && ! __messages_computed [Arc ( id, from )] ) {
      __produceMessage( id, from );
    }
  }


  // find the potentials d-connected to a set of variables
  template <typename GUM_SCALAR>
  INLINE void
  LazyPropagation<GUM_SCALAR>::__findRelevantPotentialsGetAll
  ( Set<const Potential<GUM_SCALAR>*>& pot_list,
    Set<const DiscreteVariable*>& kept_vars ) {
  }


  // find the potentials d-connected to a set of variables
  template <typename GUM_SCALAR>
  INLINE void
  LazyPropagation<GUM_SCALAR>::__findRelevantPotentialsWithdSeparation
  ( Set<const Potential<GUM_SCALAR>*>& pot_list,
    Set<const DiscreteVariable*>& kept_vars ) {
    // find the node ids of the kept variables
    NodeSet kept_ids;
    const auto& bn = this->BayesNet ();
    for ( const auto var : kept_vars ) {
      kept_ids.insert( bn.nodeId( *var ) );
    }

    // determine the set of potentials d-connected with the kept variables
    NodeSet requisite_nodes;
    BayesBall bb;
    bb.requisiteNodes( bn.dag(),
                       kept_ids,
                       this->hardEvidenceNodes (),
                       this->softEvidenceNodes (),
                       requisite_nodes );
    for ( auto iter = pot_list.beginSafe(); iter != pot_list.endSafe(); ++iter ) {
      const Sequence<const DiscreteVariable*>& vars =
        ( **iter ).variablesSequence();
      bool found = false;
      for ( auto var : vars ) {
        if ( requisite_nodes.exists( bn.nodeId( *var ) ) ) {
          found = true;
          break;
        }
      }

      if ( !found ) {
        pot_list.erase( iter );
      }
    }
  }


  // find the potentials d-connected to a set of variables
  template <typename GUM_SCALAR>
  INLINE void
  LazyPropagation<GUM_SCALAR>::__findRelevantPotentialsWithdSeparation2
  ( Set<const Potential<GUM_SCALAR>*>& pot_list,
    Set<const DiscreteVariable*>& kept_vars ) {
    // find the node ids of the kept variables
    NodeSet kept_ids;
    const auto& bn = this->BayesNet ();
    for ( const auto var : kept_vars ) {
      kept_ids.insert( bn.nodeId( *var ) );
    }

    // determine the set of potentials d-connected with the kept variables
    BayesBall bb;
    bb.relevantPotentials( bn,
                           kept_ids,
                           this->hardEvidenceNodes (),
                           this->softEvidenceNodes (),
                           pot_list );
  }


  // find the potentials d-connected to a set of variables
  template <typename GUM_SCALAR>
  INLINE void
  LazyPropagation<GUM_SCALAR>::__findRelevantPotentialsWithdSeparation3
  ( Set<const Potential<GUM_SCALAR>*>& pot_list,
    Set<const DiscreteVariable*>& kept_vars ) {
    // find the node ids of the kept variables
    NodeSet kept_ids;
    const auto& bn = this->BayesNet();
    for ( const auto var : kept_vars ) {
      kept_ids.insert( bn.nodeId( *var ) );
    }

    // determine the set of potentials d-connected with the kept variables
    dSeparation dsep;
    dsep.relevantPotentials( bn,
                             kept_ids,
                             this->hardEvidenceNodes (),
                             this->softEvidenceNodes (),
                             pot_list );
  }


  // remove variables del_vars from the list of potentials pot_list
  template <typename GUM_SCALAR>
  Set<const Potential<GUM_SCALAR>*>
  LazyPropagation<GUM_SCALAR>::__marginalizeOut
  ( Set<const Potential<GUM_SCALAR>*>& pot_list,
    Set<const DiscreteVariable*>& del_vars,
    Set<const DiscreteVariable*>& kept_vars ) {
    // use d-separation analysis to check which potentials shall be combined
    ( this->*__findRelevantPotentials )( pot_list, kept_vars );

    // remove the potentials corresponding to barren variables
    // __removeBarrenVariables( pot_list, del_vars );

    // create a combine and project operator that will perform the
    // marginalization
    MultiDimCombineAndProjectDefault<GUM_SCALAR, Potential>
      combine_and_project( __combination_op, __projection_op );
    __PotentialSet new_pot_list =
      combine_and_project.combineAndProject( pot_list, del_vars );

    // determine which new potentials we have created and put them into the
    // set of potentials created during inference. In addition, remove all the
    // potentials that have no dimension
    for ( auto iter_pot = new_pot_list.beginSafe();
          iter_pot != new_pot_list.endSafe(); ++iter_pot ) {
      if ( ( *iter_pot )->variablesSequence().size() == 0 ) {
        // as we have already marginalized out variables that received evidence,
        // it may be the case that, after combining and projecting, some
        // potentials might be empty. In this case, we shall keep their
        // constant and remove them from memory
        delete *iter_pot;
        new_pot_list.erase( iter_pot );
      }
    }

    return new_pot_list;
  }


  // creates the message sent by clique from_id to clique to_id
  template <typename GUM_SCALAR>
  void LazyPropagation<GUM_SCALAR>::__produceMessage( const NodeId from_id,
                                                      const NodeId to_id ) {
    // get the potentials of the clique.
    __PotentialSet pot_list;
    for ( auto pot : __clique_potentials[from_id] )
      pot_list.insert ( pot );

    // add the messages sent by adjacent nodes to from_id
    for ( const auto other : __JT->neighbours( from_id ) )
      if ( other != to_id )
        for ( const auto pot : __separator_potentials[Arc( other, from_id )] )
          pot_list.insert( pot );

    // get the set of variables that need be removed from the potentials
    const NodeSet& from_clique = __JT->clique( from_id );
    const NodeSet& separator = __JT->separator( from_id, to_id );
    Set<const DiscreteVariable*> del_vars( from_clique.size() );
    Set<const DiscreteVariable*> kept_vars( separator.size() );
    const auto& bn = this->BayesNet ();
    
    for ( const auto node : from_clique ) {
      if ( !separator.contains( node ) ) {
        del_vars.insert( &( bn.variable( node ) ) );
      }
      else {
        kept_vars.insert( &( bn.variable( node ) ) );
      }
    }

    // pot_list now contains all the potentials to multiply and marginalize
    // => combine the messages
    __PotentialSet new_pot_list =
      __marginalizeOut( pot_list, del_vars, kept_vars );

    // keep track of the newly created potentials
    const Arc arc (  from_id, to_id );
    for ( auto pot : new_pot_list ) {
      if ( ! pot_list.exists( pot ) ) {
        __created_potentials[arc].insert( pot );
      }
    }

    __separator_potentials[arc] = std::move ( new_pot_list );
    __messages_computed[arc] = true;
  }

  
  // performs a whole inference
  template <typename GUM_SCALAR>
  INLINE void
  LazyPropagation<GUM_SCALAR>::_makeInference () {
    for ( const auto root : __roots )
      __collectMessage ( root, root );
  }

  
  // compute P(1st arg,evidence) and store the result into the second arg
  template <typename GUM_SCALAR>
  void LazyPropagation<GUM_SCALAR>::__aPosterioriMarginal
  ( NodeId id,
    Potential<GUM_SCALAR>& marginal ) {
    // if we still need to perform some inference task, do it
    const NodeId clique_of_id = __node_to_clique[id];
    __collectMessage ( clique_of_id, clique_of_id );

    // now we just need to create the product of the potentials of the clique
    // containing id with the messages received by this clique and
    // marginalize out all variables except id
    __PotentialSet pot_list = __clique_potentials[clique_of_id];

    // add the messages sent by adjacent nodes to targetClique
    for ( const auto other : __JT->neighbours( clique_of_id ) )
      for ( const auto pot : __separator_potentials[Arc( other, clique_of_id )] )
        pot_list.insert( pot );

    // get the set of variables that need be removed from the potentials
    const NodeSet& nodes = __JT->clique( clique_of_id );
    Set<const DiscreteVariable*> del_vars ( nodes.size() );
    Set<const DiscreteVariable*> kept_vars;

    const auto& bn = this->BayesNet ();
    for ( const auto node : nodes ) {
      if ( node != id ) {
        del_vars.insert( &( bn.variable( node ) ) );
      }
      else {
        kept_vars.insert( &( bn.variable( node ) ) );
      }
    }
    
    // pot_list now contains all the potentials to multiply and marginalize
    // => combine the messages
    __PotentialSet new_pot_list =
      __marginalizeOut ( pot_list, del_vars, kept_vars );

    if ( new_pot_list.size() == 1 ) {
      auto pot = *( new_pot_list.begin() );
      if ( ! pot_list.exists ( pot ) ) {
        marginal = std::move ( *pot );
        delete pot;
      }
      else {
        marginal = *pot;
      }
    }
    else {
      MultiDimCombinationDefault<GUM_SCALAR, Potential>
        fast_combination ( __combination_op );
      fast_combination.combine( marginal, new_pot_list );
      for ( auto pot : new_pot_list )
        if ( ! pot_list.exists ( pot ) ) delete pot;
    }

    // check that the marginal is different from a 0 vector: this would
    // indicate that the evidence are not compatibles
    bool nonzero_found = false;
    for ( Instantiation inst ( marginal ); ! inst.end (); ++inst ) {
      if ( marginal[inst] ) {
        nonzero_found = true;
        break;
      }
    }
    if ( ! nonzero_found )
      GUM_ERROR ( IncompatibleEvidence, "some evidence entered into the Bayes "
                  "net are incompatible (their joint proba = 0)" );
  }


  // returns the marginal a posteriori proba of a given node
  template <typename GUM_SCALAR>
  INLINE void
  LazyPropagation<GUM_SCALAR>::_fillPosterior
  ( const NodeId id,
    Potential<GUM_SCALAR>& posterior ) {
    __aPosterioriMarginal( id, posterior );
    posterior.normalize();
  }


  // returns the marginal a posteriori proba of a given node
  template <typename GUM_SCALAR>
  void LazyPropagation<GUM_SCALAR>::__aPosterioriSetMarginal
  ( const NodeSet& set,
    Potential<GUM_SCALAR>& marginal ) {
    // if we still need to perform some inference task, do it
    const NodeId clique_of_set = __settarget_to_clique[set];
    __collectMessage ( clique_of_set, clique_of_set );

    // now we just need to create the product of the potentials of the clique
    // containing set with the messages received by this clique and
    // marginalize out all variables except set
    __PotentialSet pot_list = __clique_potentials[clique_of_set];

    // add the messages sent by adjacent nodes to targetClique
    for ( const auto other : __JT->neighbours( clique_of_set ) )
      for ( const auto pot : __separator_potentials[Arc( other, clique_of_set )] )
        pot_list.insert( pot );

    // get the set of variables that need be removed from the potentials
    const NodeSet& nodes = __JT->clique( clique_of_set );
    Set<const DiscreteVariable*> del_vars ( nodes.size() );
    Set<const DiscreteVariable*> kept_vars;

    const auto& bn = this->BayesNet ();
    for ( const auto node : nodes ) {
      if ( ! set.contains ( node ) ) {
        del_vars.insert( &( bn.variable( node ) ) );
      }
      else {
        kept_vars.insert( &( bn.variable( node ) ) );
      }
    }
    
    // pot_list now contains all the potentials to multiply and marginalize
    // => combine the messages
    __PotentialSet new_pot_list =
      __marginalizeOut ( pot_list, del_vars, kept_vars );
    
    if ( new_pot_list.size() == 1 ) {
      auto pot = *new_pot_list.begin();
      if ( ! pot_list.exists ( pot ) ) {
        marginal = std::move ( *pot );
        delete pot;
      }
      else {
        marginal = *pot;
      }
    }
    else {
      MultiDimCombinationDefault<GUM_SCALAR, Potential>
        fast_combination ( __combination_op );
      fast_combination.combine( marginal, new_pot_list );
      for ( auto pot : new_pot_list )
        if ( ! pot_list.exists ( pot ) ) delete pot;
    }

    // check that the marginal is different from a 0 vector: this would
    // indicate that the evidence are not compatibles
    bool nonzero_found = false;
    for ( Instantiation inst ( marginal ); ! inst.end (); ++inst ) {
      if ( marginal[inst] ) {
        nonzero_found = true;
        break;
      }
    }
    if ( ! nonzero_found )
      GUM_ERROR ( IncompatibleEvidence, "some evidence entered into the Bayes "
                  "net are incompatible (their joint proba = 0)" );
  }

  
  // returns the marginal a posteriori proba of a given set of nodes
  template <typename GUM_SCALAR>
  INLINE void
  LazyPropagation<GUM_SCALAR>::_fillSetPosterior
  ( const NodeSet& set,
    Potential<GUM_SCALAR>& posterior ) {
    __aPosterioriSetMarginal( set, posterior );
    posterior.normalize();
  }



  

  



  template <typename GUM_SCALAR>
  GUM_SCALAR LazyPropagation<GUM_SCALAR>::evidenceProbability() {
    // here, we should check that __find_relevant_potential_type is equal to
    // FIND_RELEVANT_ALL. Otherwise, the computations could be wrong.
    if ( __find_relevant_potential_type != 
         FindRelevantPotentialsType::FIND_RELEVANT_ALL ) {
      GUM_ERROR ( FatalError, "To compute the probability of evidence, use "
                  "setFindRelevantPotentialsType(FindRelevantPotentialsType::"
                  "FIND_RELEVANT_ALL )" );
    }

    // perform inference in each connected component
    this->makeInference ();
    
    // for each connected component, select a variable X and compute the
    // joint probability of X and evidence e. Then marginalize-out X to get
    // p(e) in this connected component. Finally, multiply all the p(e) that
    // we got and the elements in  __constants. The result is the probability
    // of evidence
    GUM_SCALAR prob_ev = 1;
    for ( const auto root : __roots ) {
      // get a node in the clique
      const NodeId node = *( __JT->clique ( root ).begin () );
      Potential<GUM_SCALAR> tmp;
      __aPosterioriMarginal( node, tmp );
      GUM_SCALAR sum = 0;
      for ( Instantiation iter( tmp ); !iter.end(); ++iter )
        sum += tmp.get( iter );
      prob_ev *= sum;
    }

    for ( const auto& projected_cpt : __constants )
      prob_ev *= projected_cpt.second;

    return prob_ev;
  }
  

  /** Entropy
   * Compute Shanon's entropy of a node given the observation
   */
  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR LazyPropagation<GUM_SCALAR>::H( NodeId X ) {
    Potential<GUM_SCALAR> posteriorX;
    __aPosterioriMarginal( X, posteriorX );
    return posteriorX.entropy();
  }

  
  /** Mutual information between X and Y
   * @see http://en.wikipedia.org/wiki/Mutual_information
   *
   * @warning Due to limitation of @joint, may not be able to compute this value
   * @throw OperationNotAllowed in these cases
   */
  template <typename GUM_SCALAR>
  GUM_SCALAR LazyPropagation<GUM_SCALAR>::I( NodeId X, NodeId Y ) {
    Potential<GUM_SCALAR> pX, pY;
    __aPosterioriMarginal( X, pX );
    __aPosterioriMarginal( Y, pY );

    NodeSet XY;
    XY << X << Y;
    Potential<GUM_SCALAR> pXY;
    __aPosterioriSetMarginal( XY, pXY );

    Instantiation i( pXY );
    GUM_SCALAR res = (GUM_SCALAR)0;

    for ( i.setFirst(); !i.end(); ++i ) {
      GUM_SCALAR vXY = pXY[i];
      GUM_SCALAR vX = pX[i];
      GUM_SCALAR vY = pY[i];

      if ( vXY > (GUM_SCALAR)0 ) {
        if ( vX == (GUM_SCALAR)0 || vY == (GUM_SCALAR)0 ) {
          GUM_ERROR( OperationNotAllowed,
                     "Mutual Information (X,Y) with P(X)=0 or P(Y)=0 "
                     "and P(X,Y)>0" );
        }

        res += vXY * ( log2( vXY ) - log2( vX ) - log2( vY ) );
      }
    }

    return res;
  }
  

  /** Variation of information between X and Y
   * @see http://en.wikipedia.org/wiki/Variation_of_information
   *
   * @warning Due to limitation of @joint, may not be able to compute this value
   * @throw OperationNotAllowed in these cases
   */
  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR LazyPropagation<GUM_SCALAR>::VI( NodeId X, NodeId Y ) {
    return H( X ) + H( Y ) - 2 * I( X, Y );
  }
} /* namespace gum */

#endif  // DOXYGEN_SHOULD_SKIP_THIS
// kate: indent-mode cstyle; indent-width 2; replace-tabs on;
