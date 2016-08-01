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
#include <agrum/config.h>
#include <agrum/multidim/instantiation.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/BayesBall.h>
#include <agrum/BN/inference/dSeparation.h>
#include <agrum/multidim/operators/multiDimProjection.h>
#include <agrum/multidim/operators/multiDimCombineAndProjectDefault.h>
#include <agrum/graphs/binaryJoinTreeConverterDefault.h>
#include <agrum/graphs/triangulations/orderedTriangulation.h>
#include <agrum/BN/inference/barrenNodesFinder.h>

// to ease IDE parsers
#include <agrum/BN/inference/BayesNetInference.h>

namespace gum {

  // the function used to combine two tables
  template <typename GUM_SCALAR>
  INLINE static Potential<GUM_SCALAR>*
  LPNewmultiPotential( const Potential<GUM_SCALAR>& t1,
                       const Potential<GUM_SCALAR>& t2 ) {
    return new Potential<GUM_SCALAR>( t1 * t2 );
  }

  // the function used to combine two tables
  template <typename GUM_SCALAR>
  INLINE static Potential<GUM_SCALAR>*
  LPNewprojPotential( const Potential<GUM_SCALAR>& t1,
                      const Set<const DiscreteVariable*>& del_vars ) {
    return new Potential<GUM_SCALAR>( t1.margSumOut( del_vars ) );
  }










  
  
  // default constructor
  template <typename GUM_SCALAR>
  INLINE LazyPropagation<GUM_SCALAR>::LazyPropagation
  ( const IBayesNet<GUM_SCALAR>* BN,
    FindRelevantPotentialsType relevant_type,
    FindBarrenNodesType barren_type ) :
    Inference<GUM_SCALAR>( BN ) {
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
    for ( const auto pot : __created_potentials )
      delete pot;

    // remove the potentials created after removing the nodes that received
    // hard evidence
    for ( const auto pot : __hard_ev_potentials )
      delete pot;

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
    else
      __evidence_changes.insert ( id, EvidenceChangeType::EVIDENCE_ADDED );
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
    else
      __evidence_changes.insert ( id, EvidenceChangeType::EVIDENCE_ERASED );
  }
   

  /// fired when all the evidence are erased
  template <typename GUM_SCALAR>
  void
  LazyPropagation<GUM_SCALAR>::_onAllEvidenceErased ( bool has_hard_evidence ) {
    if ( has_hard_evidence || ! this->hardEvidenceNodes ().empty () )
      __is_new_jt_needed = true;
    else {
      for ( const auto node : this->softEvidenceNodes () )
        __evidence_changes.insert ( node, EvidenceChangeType::EVIDENCE_ERASED );
    }
  }
   
     
  /// fired when an evidence is changed
  template <typename GUM_SCALAR>
  INLINE void
  LazyPropagation<GUM_SCALAR>::_onEvidenceChanged( const NodeId id,
                                                   bool hasChangedSoftHard ) {
    if ( hasChangedSoftHard )
      __is_new_jt_needed = true;
    else
      __evidence_changes.insert ( id, EvidenceChangeType::EVIDENCE_MODIFIED );
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
        BarrenNodesFinder finder( bn.dag() );
        finder.setTargets ( target_nodes );

        NodeSet evidence_nodes;
        for ( const auto& pair : this->evidence () ) {
          evidence_nodes.insert ( pair.first );
        }
        finder.setEvidence ( evidence_nodes );

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
    const auto& hard_ev_nodes = this->hardEvidence ();
    for ( const auto& pair : hard_ev_nodes ) {
      __graph.eraseNode ( pair.first );
    }

    // now, we can compute the new junction tree. To speed-up computations
    // (essentially, those of the distribution phase), we construct this
    // junction tree a binary join tree
    __triangulation->setGraph ( &__graph, &( this->domainSizes() ) );
    const JunctionTree& triang_jt = __triangulation->junctionTree ();
    BinaryJoinTreeConverterDefault bjt_converter;
    NodeSet emptyset;
    if ( __JT != nullptr ) delete __JT;
    __JT = new CliqueGraph( bjt_converter.convert( triang_jt,
                                                   &( this->domainSizes() ),
                                                   emptyset ) );

    // get one arbitrary root per connected component of the binary join tree
    __roots = bjt_converter.roots();

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

    // create empty potential lists into the cliques of the joint tree as well
    // as empty lists of evidence
    List<const Potential<GUM_SCALAR>*> empty_list;

    __clique_potentials.clear ();
    __clique_evidence.clear ();
    for ( const auto node : *__JT ) {
      __clique_potentials.insert( node, empty_list );
    }

    // remove all the potentials created during the last inference
    for ( auto& potlist : __created_potentials )
      for ( auto pot : potlist )
        delete pot;
    __created_potentials.clear ();

    // remove all the potentials created to take into account hard evidence
    // during the last inference
    for ( auto pot : __hard_ev_potentials )
      delete pot.second;
    __hard_ev_potentials.clear ();

    // remove all the constants created due to projections of CPTs that were
    // defined over only hard evidence nodes
    __constants.clear ();
    
    // create empty lists of potentials for the messages and indicate that no
    // message has been computed yet
    __separator_potentials.clear ();
    __messages_computed.clear ();
    for ( const auto& edge : __JT->edges () ) {
      const Arc arc1 ( edge.first (), edge.second () );
      __separator_potentials.insert ( arc1, empty_list );
      __messages_computed.insert ( arc1, false );
      Arc arc2 ( Arc ( edge.second (), edge.first () ) );
      __separator_potentials.insert ( arc2, empty_list );
      __messages_computed.insert ( arc2, false );
    }

    
    // put all the CPT's of the Bayes net nodes into the cliques
    // here, beware: all the potentials that are defined over some nodes including
    // hard evidence must be projected so that these nodes are removed from the
    // potential
    const auto& evidence = this->evidence ();
    for ( const auto node : __graph ) {
      const Potential<GUM_SCALAR>& cpt = bn.cpt( node );

      // get the list of nodes with hard evidence in cpt
      NodeSet hard_nodes;
      const auto& variables = cpt.variablesSequence ();
      for ( const auto var : variables ) {
        const NodeId node = bn.nodeId ( *var );
        if ( hard_ev_nodes.contains ( node ) )
          hard_nodes.insert ( node );
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
          for ( int i = 0; i < hard_nodes.size (); ++i ) {
            inst.chgVal ( variables[i],
                          hard_ev_nodes[bn.nodeId( variables[i] )] );
          }
          __constants.insert ( node, cpt[inst] );
        }
        else {
          // perform the projection with a combine and project instance
          Set<const DiscreteVariable*> hard_variables;
          __PotentialSet marg_cpt_set { cpt };
          for ( const auto node : hard_ev_nodes ) {
            marg_cpt_set.insert( evidence[node] );
            hard_variables.insert ( bn.variable ( node ) );
          }
          // perform the combination of those potentials and their projection
          MultiDimCombineAndProjectDefault<GUM_SCALAR, Potential>
            combine_and_project( __combination, LPNewprojPotential );
          __PotentialSet new_cpt_list =
            combine_and_project.combineAndProject( marg_cpt_set, hard_variables );

          // there should be only one potential in new_cpt_list
          if ( new_cpt_list.size () != 1 ) {
            GUM_ERROR ( FatalError, "the projection of a potential containing "
                        << "hard evidence is empty!" );
          }
          const Potential<GUM_SCALAR>* projected_cpt = *( new_cpt_list.begin () );
          __clique_potentials[__node_to_clique[node]].insert ( projected_cpt );
          __hard_ev_potentials.insert ( node, projected_cpt );
        }
      }
    }


    // we shall now add all the potentials of the soft evidence
    for ( const auto node : this->sotfEvidenceNodes () ) {
      __clique_evidence[__node_to_clique[node]].insert ( evidence[node] );
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
    // invalidate all the messages that are no more correct: start from each of
    // the nodes whose evidence has changed and perform a diffusion from the
    // clique into which the evidence has been entered, indicating that the
    // messages emanating from this clique are now invalid. At the same time,
    // if there were potentials created on the arcs over wich the messages are
    // sent, remove them from memory
    for ( const auto& pair : __evidence_changes ) {
      const auto clique = __node_to_clique[pair.first];
      for ( const auto neighbor : __JT->neighbours ( clique ) ) {
        __diffuseMessageInvalidations ( clique, neighbor );
      }
    }

    // remove all the evidence that were entered into __clique_evidence and
    // __clique_potentials and add the new soft ones
    for ( auto& potlist : __clique_evidence ) {
      auto& clique_potentials = __clique_potentials[potlist.first];
      for ( auto pot : potlist.second ) {
        clique_potentials.eraseByVal ( pot );
      }
    }
    
        
    
    __clique_evidence.clear ();
    const auto& evidence = this->evidence ();
    for ( const auto node : this->softEvidenceNodes () ) {
      __clique_evidence.insert ( __node_to_clique[node], evidence[node] );
    }

    // now we shall remove the projections of the CPTs due to hard evidence
    // when the latter have changed and add new ones  
    const auto& hard_ev_nodes = this->_hardEvidence ();
    NodeSet nodes_potentially_affected;
    for ( const auto node : __evidence_changed ) {
      if ( hard_ev_nodes.exists ( node ) ) {
        nodes_potentially_affected.insert ( node );
        for ( const auto neigh : __graph.neighbours ( node ) ) {
          nodes_potentially_affected.insert ( neigh );
        }
      }
    }
    for ( const auto node : nodes_affected ) {
      // if we are performing _updateInferencePotentials, this means that the
      // set of nodes that received hard evidence has not been changed, only
      // their instantiations can have been changed. So, if there is an entry
      // for node in __constants, there will still be such an entry after
      // performing the new projections. Idem for __hard_ev_potentials
      const Potential<GUM_SCALAR>& cpt = bn.cpt( node );
      const auto& variables = cpt.variablesSequence ();
      if ( __constants.exists ( node ) ) {  
        Instantiation inst ( cpt );
        for ( const auto var : variables ) {
          inst.chgVal ( var, hard_ev_nodes[bn.id( var )] );
        }
        __constants[node] = cpt[inst];
      }
      else if ( __hard_ev_potentials.exists ( node ) ) {
        // perform the projection with a combine and project instance
        Set<const DiscreteVariable*> hard_variables;
        __PotentialSet marg_cpt_set { cpt };
        for ( const auto var : variables ) {
          const NodeId node = bn.id( var );
          if ( hard_ev_nodes.exists ( node ) ) {
            marg_cpt_set.insert( evidence[node] );
            hard_variables.insert ( bn.variable ( node ) );
          }
          // perform the combination of those potentials and their projection
          MultiDimCombineAndProjectDefault<GUM_SCALAR, Potential>
            combine_and_project( __combination, LPNewprojPotential );
          __PotentialSet new_cpt_list =
            combine_and_project.combineAndProject( marg_cpt_set, hard_variables );

          // there should be only one potential in new_cpt_list
          if ( new_cpt_list.size () != 1 ) {
            GUM_ERROR ( FatalError, "the projection of a potential containing "
                        << "hard evidence is empty!" );
          }
          const Potential<GUM_SCALAR>* projected_cpt = *( new_cpt_list.begin () );
          const auto pot = __hard_ev_potentials[node];
          __clique_potentials[__node_to_clique[node]].eraseByVal ( pot );
          __clique_potentials[__node_to_clique[node]].insert ( projected_cpt );
          __hard_ev_potentials.insert ( projected_cpt );
        }
      }
    }
  }




      












    

    
    // for each evidence removed, remove it from __clique_evidence, for each
    // evidence added, add it to __clique_evidence; and for each evidence


    
    { // update the potentials and messages that require it
      // if we do not need to create a new junction tree, this means that the
      // hard evidence did not change and the targets and settargets already
      // belong to the current junction tree. So, the only things that could have
      // changed w.r.t. the last inference are:
      // 1/ the values of the evidence (hard or soft)
      // 2/ the insertion or deletion of some soft evidence
      // So we first determine the evidence that correspond to cases 1 or 2
      // and we invalidate all the messages that were sent from the cliques in
      // which these evidence have been entered

      
    }
  }












  





  

  



  // performs the collect phase of Lazy Propagation
  template <typename GUM_SCALAR>
  INLINE void LazyPropagation<GUM_SCALAR>::__collect( NodeId id, NodeId from ) {
    for ( const auto other : __JT->neighbours( id ) )
      if ( other != from ) __collect( other, id );

    if ( id != from ) {
      __produceMessage( id, from );
    }
  }



  // performs the __collect phase of Lazy Propagation
  template <typename GUM_SCALAR>
  INLINE void LazyPropagation<GUM_SCALAR>::collect( NodeId id,
                                                    bool force_collect ) {
    // get a clique that contains id
    NodeId clique = __node_to_clique[id];
    
    // check if we really need to perform an inference when we do not force the
    // collect to be performed
    if ( ! force_collect ) {
      bool do_collect = false;
      for ( const auto neighbor : __JT->neighbours ( clique ) ) {
        if ( ! __messages_computed[Arc ( neighbor, clique )] ) {
          do_collect = true;
          break;
        }
      }
      if ( ! do_collect ) return;
    }

    // clean-up the area that will receive the __collect
    __setRequiredInference( clique, clique );

    // compute barren nodes if necessary
    __computeBarrenPotentials();

    // perform the __collect
    __collect( clique, clique );
  }




  

















  
  // initialization function
  template <typename GUM_SCALAR>
  INLINE void LazyPropagation<GUM_SCALAR>::__initialize
  ( const IBayesNet<GUM_SCALAR>& BN,
    StaticTriangulation& triangulation,
    const NodeProperty<Size>& modalities ) {


    // initialize the __triangulation algorithm
    OrderedTriangulation triangulation( &( this->bn().moralGraph() ),
                                        &modalities, &elim_order );


    const JunctionTree& triang_jt = triangulation.junctionTree();

    // set the correspondance between variables and their id and get the
    // variables domain sizes
    NodeProperty<Size> modalities;
    auto& bn = this->bn();

    for ( const auto node : bn.dag () )
      modalities.insert( node, bn.variable( node ).domainSize() );
    __triangulation.setGraph( &( bn.moralGraph() ), &modalities );

    BinaryJoinTreeConverterDefault bon_converter;
    NodeSet emptyset;
    __JT = new CliqueGraph( bon_converter.convert( triang_jt, modalities,
                                                   emptyset ) );
    __roots = bon_converter.roots();

    // indicate, for each node of the BN, a clique in __JT that can contain its
    // conditional probability table
    const std::vector<NodeId>& JT_elim_order = triangulation.eliminationOrder();
    HashTable<NodeId, unsigned int> elim_order( JT_elim_order.size() );

    for ( unsigned int i = 0, size = JT_elim_order.size(); i < size; ++i )
      elim_order.insert( JT_elim_order[i], i );

    const DAG& dag = BN.dag();

    for ( const auto node : dag ) {
      // get the variables in the potential of node (and its parents)
      NodeId first_eliminated_node = node;
      unsigned int elim_number = elim_order[node];

      for ( const auto parent : dag.parents( node ) ) {
        if ( elim_order[parent] < elim_number ) {
          elim_number = elim_order[parent];
          first_eliminated_node = parent;
        }
      }

      // first_eliminated_node contains the first var (node or one of its
      // parents) eliminated => the clique created during its elmination
      // contains node and all of its parents => it can contain node's potential
      __node_to_clique.insert
        ( node,
          triangulation.createdJunctionTreeClique( first_eliminated_node ) );
    }

    // create empty potential lists into the cliques of the joint tree as well
    // as empty lists of evidence
    List<const Potential<GUM_SCALAR>*> empty_list;

    for ( const auto node : __JT->nodes() ) {
      __clique_potentials.insert( node, empty_list );
      __clique_evidence.insert( node, empty_list );
    }

    // put all the CPT's of the Bayes net nodes into the cliques
    for ( const auto node : dag ) {
      const Potential<GUM_SCALAR>& cpt = BN.cpt( node );
      __clique_potentials[__node_to_clique[node]].insert( &cpt );
    }

    // create empty messages on the separators and indicate that no message
    // has been computed yet
    __PotentialSet empty_set;

    for ( const auto edge : __JT->edges() ) {
      Arc arc1 ( edge.first(), edge.second() );
      __sep_potentials.insert( arc1, empty_set );
      __messages_computed.insert ( arc1, false );
      Arc arc2 ( edge.second(), edge.first() );
      __sep_potentials.insert( arc2, empty_set );
      __messages_computed.insert ( arc2, false );
    }
  }














  


  // indicates that we need inference in a given Junction tree connected
  // component
  template <typename GUM_SCALAR>
  void LazyPropagation<GUM_SCALAR>::__setRequiredInference( NodeId id,
                                                            NodeId from ) {
    // check if an inference has already happened through clique id
    if ( ( __collected_cliques[id] == false ) &&
         ( __diffused_cliques[id] == false ) )
      return;

    // indicates that clique "id" needs an inference
    __collected_cliques[id] = false;
    __diffused_cliques[id] = false;

    // propagate this requirement to id's neighbours
    for ( const auto other : __JT->neighbours( id ) ) {
      if ( other != from ) {
        // remove the potentials sent on clique id's adjacent separators
        for ( const auto pot : __sep_potentials[Arc( other, id )] ) {
          if ( __created_potentials.exists( pot ) ) {
            delete pot;
            __created_potentials.erase( pot );
          }
        }

        for ( const auto pot : __sep_potentials[Arc( id, other )] ) {
          if ( __created_potentials.exists( pot ) ) {
            delete pot;
            __created_potentials.erase( pot );
          }
        }

        __sep_potentials[Arc( other, id )].clear();
        __sep_potentials[Arc( id, other )].clear();

        // propagate the "required" state to the neighbours
        __setRequiredInference( other, id );
      }
    }
  }


  // remove a given evidence from the graph
  template <typename GUM_SCALAR>
  INLINE void LazyPropagation<GUM_SCALAR>::eraseEvidence(
                                                         const Potential<GUM_SCALAR>* pot ) {
    this->_invalidatePosteriors();

    // if the evidence does not exist, do nothing
    if ( !__evidences.contains( pot ) ) return;

    // get the variable involved in pot and indicate that this variable did
    // not receive any evidence
    const Sequence<const DiscreteVariable*>& vars = pot->variablesSequence();
    const NodeId var = this->bn().nodeId( *vars.atPos( 0 ) );
    __hard_evidence_nodes.erase( var );
    __soft_evidence_nodes.erase( var );

    // remove the potential from the list of evidence of the cliques
    for ( auto& elt : __clique_evidence ) {
      elt.second.eraseByVal( pot );
    }

    // remove the potential from the list of evidence
    __evidences.erase( pot );
    __bn_node2hard_potential.erase( var );

    // indicate that we need to perform both __collect and __diffusion in the
    // connected component containing the variable of the evidence
    const NodeId pot_clique = __node_to_clique[var];
    __setRequiredInference( pot_clique, pot_clique );

    // indicate that we shall recompute the set of barren nodes/potentials
    __need_recompute_barren_potentials = true;
  }


  // remove all evidence from the graph
  template <typename GUM_SCALAR>
  INLINE void LazyPropagation<GUM_SCALAR>::eraseAllEvidence() {
    this->_invalidatePosteriors();
    // remove the evidence store in the cliques
    for ( auto& elt : __clique_evidence )
      elt.second.clear();

    // remove the messages sent during a previous propagation
    for ( auto& elt : __sep_potentials )
      elt.second.clear();

    // remove actually all the evidence taken into account
    __evidences.clear();
    __bn_node2hard_potential.clear();

    // indicate that no node received any evidence
    __hard_evidence_nodes.clear();
    __soft_evidence_nodes.clear();

    // remove from memory all the created potentials
    for ( const auto pot : __created_potentials )
      delete pot;
    __created_potentials.clear();

    for ( auto& elt : __collected_cliques )
      elt.second = false;

    for ( auto& elt : __diffused_cliques )
      elt.second = false;

    // indicate that we shall recompute the set of barren nodes/potentials
    __need_recompute_barren_potentials = true;
  }


  /// clears the messages of previous inferences
  template <typename GUM_SCALAR>
  void LazyPropagation<GUM_SCALAR>::clearInference() {
    this->_invalidatePosteriors();

    // remove the messages sent during a previous propagation
    for ( auto& elt : __sep_potentials )
      elt.second.clear();

    // remove from memory all the created potentials
    for ( const auto pot : __created_potentials )
      delete pot;
    __created_potentials.clear();

    for ( auto& elt : __collected_cliques )
      elt.second = false;

    for ( auto& elt : __diffused_cliques )
      elt.second = false;
  }















  
  // template <typename GUM_SCALAR>
  template <typename GUM_SCALAR>
  INLINE bool LazyPropagation<GUM_SCALAR>::__isHardEvidence(
                                                            const Potential<GUM_SCALAR>* pot ) {
    Instantiation i( pot );
    unsigned int nb_non_zeros = 0;
    for ( i.setFirst(); !i.end(); i.inc() ) {
      if ( pot->operator[]( i ) != 0 ) {
        ++nb_non_zeros;
        if ( nb_non_zeros > 1 ) return false;
      }
    }

    return ( nb_non_zeros == 1 );
  }


  // insert new evidence in the graph
  template <typename GUM_SCALAR>
  INLINE void LazyPropagation<GUM_SCALAR>::insertEvidence(
                                                          const List<const Potential<GUM_SCALAR>*>& pot_list ) {
    this->_invalidatePosteriors();
    List<const Potential<GUM_SCALAR>*> empty_list;

    for ( const auto pot : pot_list ) {
      // check that the evidence is given w.r.t.only one random variable
      const Sequence<const DiscreteVariable*>& vars = pot->variablesSequence();
      if ( vars.size() != 1 ) {
        GUM_ERROR(
                  SizeError,
                  "Evidence can be be given w.r.t only one random variable for "
                  << pot );
      }
    }

    for ( const auto pot : pot_list ) {
      const DiscreteVariable* var = pot->variablesSequence().atPos( 0 );
      NodeId var_id = this->bn().nodeId( *var );
      NodeId clique_id = __node_to_clique[var_id];  // get the clique that
                                                    // contains the var
      auto& clique_evidences = __clique_evidence[clique_id];

      // remove already existing evidence w.r.t. variable in pot
      for ( auto ev_pot : clique_evidences ) {
        if ( var == ev_pot->variablesSequence().atPos( 0 ) ) {
          // here we found the evidence corresponding to the variable
          eraseEvidence( ev_pot );
          break;
        }
      }

      // insert the evidence
      __evidences.insert( pot );
      __clique_evidence[clique_id].insert( pot );
      if ( __isHardEvidence( pot ) ) {
        __hard_evidence_nodes.insert( var_id );
        __bn_node2hard_potential.insert( var_id, pot );
      } else {
        __soft_evidence_nodes.insert( var_id );
      }

      // indicate that, now, new inference is required
      __setRequiredInference( clique_id, clique_id );
    }

    // indicate that we shall recompute the set of barren nodes/potentials
    __need_recompute_barren_potentials = true;
  }



  // find the potentials d-connected to a set of variables
  template <typename GUM_SCALAR>
  INLINE void LazyPropagation<GUM_SCALAR>::__findRelevantPotentialsGetAll(
                                                                          __PotentialSet& pot_list, Set<const DiscreteVariable*>& kept_vars ) {}


  // find the potentials d-connected to a set of variables
  template <typename GUM_SCALAR>
  INLINE void
  LazyPropagation<GUM_SCALAR>::__findRelevantPotentialsWithdSeparation(
                                                                       __PotentialSet& pot_list, Set<const DiscreteVariable*>& kept_vars ) {
    // find the node ids of the kept variables
    NodeSet kept_ids;
    const auto& bn = this->bn();
    for ( const auto var : kept_vars ) {
      kept_ids.insert( bn.nodeId( *var ) );
    }

    // determine the set of potentials d-connected with the kept variables
    NodeSet requisite_nodes;
    BayesBall bb;
    bb.requisiteNodes( this->bn().dag(),
                       kept_ids,
                       __hard_evidence_nodes,
                       __soft_evidence_nodes,
                       requisite_nodes );
    for ( auto iter = pot_list.beginSafe(); iter != pot_list.endSafe();
          ++iter ) {
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
  LazyPropagation<GUM_SCALAR>::__findRelevantPotentialsWithdSeparation2(
                                                                        __PotentialSet& pot_list, Set<const DiscreteVariable*>& kept_vars ) {
    // find the node ids of the kept variables
    NodeSet kept_ids;
    const auto& bn = this->bn();
    for ( const auto var : kept_vars ) {
      kept_ids.insert( bn.nodeId( *var ) );
    }

    // determine the set of potentials d-connected with the kept variables
    BayesBall bb;
    bb.relevantPotentials( this->bn(),
                           kept_ids,
                           __hard_evidence_nodes,
                           __soft_evidence_nodes,
                           pot_list );
  }


  // find the potentials d-connected to a set of variables
  template <typename GUM_SCALAR>
  INLINE void
  LazyPropagation<GUM_SCALAR>::__findRelevantPotentialsWithdSeparation3(
                                                                        __PotentialSet& pot_list, Set<const DiscreteVariable*>& kept_vars ) {
    // find the node ids of the kept variables
    NodeSet kept_ids;
    const auto& bn = this->bn();
    for ( const auto var : kept_vars ) {
      kept_ids.insert( bn.nodeId( *var ) );
    }

    // determine the set of potentials d-connected with the kept variables
    dSeparation dsep;
    dsep.relevantPotentials( this->bn(),
                             kept_ids,
                             __hard_evidence_nodes,
                             __soft_evidence_nodes,
                             pot_list );
  }


  // remove barren variables
  template <typename GUM_SCALAR>
  INLINE void LazyPropagation<GUM_SCALAR>::__removeBarrenVariables(
                                                                   __PotentialSet& pot_list, Set<const DiscreteVariable*>& del_vars ) {
    // remove from del_vars the variables that received some evidence:
    // only those that did not received evidence can be barren variables
    Set<const DiscreteVariable*> the_del_vars = del_vars;
    for ( auto iter = the_del_vars.beginSafe(); iter != the_del_vars.endSafe();
          ++iter ) {
      NodeId id = this->bn().nodeId( **iter );
      if ( __hard_evidence_nodes.exists( id ) ||
           __soft_evidence_nodes.exists( id ) ) {
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
    MultiDimProjection<GUM_SCALAR, Potential> projector( LPNewprojPotential );
    for ( auto elt : pot2barren_var ) {
      // remove the current potential from pot list as, anyway, we will change
      // it
      const Potential<GUM_SCALAR>* pot = elt.first;
      pot_list.erase( pot );

      // check whether we need to add a projected new potential or not (i.e.,
      // whether there exist non-barren variables or not)
      if ( pot->variablesSequence().size() != elt.second.size() ) {
        auto new_pot = projector.project( *pot, elt.second );
        pot_list.insert( new_pot );
        __created_potentials.insert( new_pot );
      }
    }
  }


  // remove variables del_vars from the list of potentials pot_list
  template <typename GUM_SCALAR>
  INLINE void LazyPropagation<GUM_SCALAR>::__marginalizeOut(
                                                            __PotentialSet& pot_list,
                                                            Set<const DiscreteVariable*>& del_vars,
                                                            Set<const DiscreteVariable*>& kept_vars ) {
    // use d-separation analysis to check which potentials shall be combined
    ( this->*__findRelevantPotentials )( pot_list, kept_vars );

    // remove the potentials corresponding to barren variables
    __removeBarrenVariables( pot_list, del_vars );

    // create a combine and project operator that will perform the
    // marginalization
    MultiDimCombineAndProjectDefault<GUM_SCALAR, Potential> combine_and_project(
                                                                                LPNewmultiPotential, LPNewprojPotential );
    __PotentialSet new_pot_list =
      combine_and_project.combineAndProject( pot_list, del_vars );

    // determine which new potentials we have created and put them into the
    // set of potentials created during inference. In addition, remove all the
    // potentials that have no dimension
    for ( auto iter_pot = new_pot_list.beginSafe();
          iter_pot != new_pot_list.endSafe();
          ++iter_pot ) {
      if ( ( *iter_pot )->variablesSequence().size() == 0 ) {
        // as we have already marginalized out variables that received evidence,
        // it may be the case that, after combining and projecting, some
        // potentials might be empty. In this case, we shall remove them
        // from memory
        delete *iter_pot;
        new_pot_list.erase( iter_pot );
      } else {
        if ( !pot_list.exists( *iter_pot ) ) {
          __created_potentials.insert( *iter_pot );
        }
      }
    }

    pot_list = new_pot_list;
  }


  // creates the message sent by clique from_id to clique to_id
  template <typename GUM_SCALAR>
  INLINE void LazyPropagation<GUM_SCALAR>::__produceMessage( NodeId from_id,
                                                             NodeId to_id ) {
    // get the set of CPTs of the BN that are barren w.r.t. the message
    // we wish to produce
    const __PotentialSet& barren_pots =
      __barren_potentials[Arc( from_id, to_id )];

    // get the potentials of the clique. For the potentials that contain a hard
    // evidence, marginalize them over the nodes that received hard evidence
    const List<const Potential<GUM_SCALAR>*>& clique_pot =
      __clique_potentials[from_id];
    __PotentialSet pot_list( clique_pot.size() );
    for ( const auto& cli : clique_pot ) {
      if ( !barren_pots.exists( cli ) ) {  // do not take into account barren CPTs
        // check whether a variable of cli received a hard evidence
        NodeSet hard_vars;
        Set<const DiscreteVariable*> del_vars;
        for ( const auto var : cli->variablesSequence() ) {
          const NodeId id = this->bn().nodeId( *var );
          if ( __hard_evidence_nodes.exists( id ) ) {
            hard_vars.insert( id );
            del_vars.insert( var );
          }
        }
        if ( hard_vars.empty() ) {  // no hard evidence => no marginalization
          pot_list.insert( cli );
        } else {  // marginalize out the hard evidence nodes
          // if all the variables are to be marginalized out, don't take into
          // account the CPT
          if ( hard_vars.size() != cli->variablesSequence().size() ) {
            // get the set of potential to combine (the CPT of the clique + the
            // potentials of the hard evidence)
            __PotentialSet marg_pot_set{cli};
            for ( const auto var_id : hard_vars ) {
              marg_pot_set.insert( __bn_node2hard_potential[var_id] );
            }

            // perform the combination of those potentials and their projection
            MultiDimCombineAndProjectDefault<GUM_SCALAR, Potential>
              combine_and_project( LPNewmultiPotential, LPNewprojPotential );
            __PotentialSet new_pot_list =
              combine_and_project.combineAndProject( marg_pot_set, del_vars );

            // insert the resulting set of potentials into pot_list
            for ( const auto pot : new_pot_list ) {
              pot_list.insert( pot );
              __created_potentials.insert( pot );
            }
          }
        }
      }
    }

    // add the soft evidence to the clique potentials
    const List<const Potential<GUM_SCALAR>*>& evidence_list =
      __clique_evidence[from_id];

    for ( const auto ev_pot : evidence_list ) {
      NodeId ev_node =
        this->bn().nodeId( *( ev_pot->variablesSequence().atPos( 0 ) ) );
      if ( !__hard_evidence_nodes.exists( ev_node ) ) {
        pot_list.insert( ev_pot );
      }
    }

    // add the messages sent by adjacent nodes to from_id
    for ( const auto other : __JT->neighbours( from_id ) )
      if ( other != to_id )
        for ( const auto pot : __sep_potentials[Arc( other, from_id )] )
          pot_list.insert( pot );

    // get the set of variables that need be removed from the potentials
    const NodeSet& from_clique = __JT->clique( from_id );
    const NodeSet& separator = __JT->separator( from_id, to_id );
    Set<const DiscreteVariable*> del_vars( from_clique.size() );
    Set<const DiscreteVariable*> kept_vars( from_clique.size() );

    for ( const auto node : from_clique ) {
      if ( !separator.contains( node ) ) {
        del_vars.insert( &( this->bn().variable( node ) ) );
      }
      else {
        kept_vars.insert( &( this->bn().variable( node ) ) );
      }
    }

    // pot_list now contains all the potentials to multiply and marginalize
    // => combine the messages
    __marginalizeOut( pot_list, del_vars, kept_vars );
    Arc arc (  from_id, to_id );
    __sep_potentials[arc] = pot_list;
    __messages_computed[arc] = true;
  }












  

  

  
  // performs the __diffusion phase of Lazy Propagation
  template <typename GUM_SCALAR>
  INLINE void LazyPropagation<GUM_SCALAR>::__diffusion( NodeId id,
                                                        NodeId from ) {
    // #### TODO: make a more efficient inference using a stack of
    // of partial computations (see GONZALES, Mellouli, Mourali (2007))
    for ( const auto other : __JT->neighbours( id ) ) {
      if ( other != from ) {
        __produceMessage( id, other );
        __diffusion( other, id );
      }
    }
  }
  

  // performs the __collect phase of Lazy Propagation
  template <typename GUM_SCALAR>
  INLINE void LazyPropagation<GUM_SCALAR>::diffusion( NodeId id,
                                                      bool force_diffusion ) {
    // get a clique that contains id
    NodeId clique = __node_to_clique[id];

    // check if we really need to perform an inference
    if ( force_diffusion ) {
      __collect( clique, true );
      __diffusion( clique, clique );
    }
    else {
      bool do_diffuse = false;
      for ( const auto neighbour : __JT->neighbours ( clique ) ) {
        if ( ! __messages_computed[Arc ( clique, neighbour )] ) {
          do_diffuse = true;
          break;
        }
      }
      if ( ! do_diffuse ) return;

      __collect( clique, false );
      __diffusion( clique, clique );
    }
  }

  
  // performs a whole inference (__collect + __diffusion)
  template <typename GUM_SCALAR>
  INLINE void LazyPropagation<GUM_SCALAR>::makeInference() {
    makeInference( false );
  }

  
  // performs a whole inference (__collect + __diffusion)
  template <typename GUM_SCALAR>
  INLINE void
  LazyPropagation<GUM_SCALAR>::makeInference( bool force_inference ) {
    // prepare a new inference from scratch

    if ( force_inference ) {
      // remove all the current separator potentials, if any
      for ( auto& elt : __sep_potentials )
        elt.second.clear();

      // remove all temporary potentials created in previous inferences
      for ( const auto pot : __created_potentials )
        delete pot;
      __created_potentials.clear();

      // indicate that __collect and __diffusion passed through no clique yet
      for ( auto& elt : __messages_computed ) {
        elt.second = false;
      }
    }

    // compute barren nodes if necessary
    __computeBarrenPotentials();

    // perform the __collect in all connected components of the junction tree
    for ( const auto root : __roots )
      __collect( root, root );

    // perform the __diffusion in all connected components of the junction tree
    for ( const auto root : __roots )
      __diffusion( root, root );

    // ##### bug potentiel a virer : s'il y a plusieurs composantes connexes,
    // il faut plusieurs cliques de collecte
  }






  
  // returns the marginal a posteriori proba of a given node
  template <typename GUM_SCALAR>
  void LazyPropagation<GUM_SCALAR>::__aPosterioriMarginal(
                                                          NodeId id, Potential<GUM_SCALAR>& marginal ) {
    // if the node received a hard evidence, just return it
    if ( __hard_evidence_nodes.exists( id ) ) {
      marginal = *( __bn_node2hard_potential[id] );
      return;
    }

    // compute barren nodes if necessary
    __computeBarrenPotentials();

    // check if we performed a __collect on id, else we need some
    NodeId clique_of_id = __node_to_clique[id];

    if ( !__collected_cliques[clique_of_id] ) {
      __collect( clique_of_id, clique_of_id );
      __last_collect_clique = clique_of_id;
    }

    // ok, we performed a __collect, but maybe this __collect was not performed
    // from the clique containing id. In this case, we also need to perform
    // a __diffusion
    const NodeSet& clique_nodes = __JT->clique( __last_collect_clique );

    bool last_collect_clique_contains_id = true;

    if ( !clique_nodes.contains( id ) ) {
      makeInference( false );
      last_collect_clique_contains_id = false;
    }

    // now we just need to create the product of the potentials of the clique
    // containing id with the messages received by this clique and
    // marginalize out all variables except id
    NodeId targetClique =
      last_collect_clique_contains_id ? __last_collect_clique : clique_of_id;

    const List<const Potential<GUM_SCALAR>*>& clique_pot =
      __clique_potentials[targetClique];

    // get the potentials of the clique
    __PotentialSet pot_list( clique_pot.size() +
                             __clique_evidence[targetClique].size() );

    for ( const auto& cli : clique_pot )
      pot_list.insert( cli );

    // add the evidence to the clique potentials
    const List<const Potential<GUM_SCALAR>*>& evidence_list =
      __clique_evidence[targetClique];
    for ( const auto ev_pot : evidence_list ) {
      pot_list.insert( ev_pot );
    }

    // if some node of the clique received a hard evidence, add this evidence
    // this is compulsory because, during the messages computations, we
    // marginalized out the hard evidence nodes from the CPTs prior to the
    // combinations. Hence, we must marginalize out these nodes on all tables
    // that contain them
    for ( const auto node : __JT->clique( targetClique ) ) {
      if ( __bn_node2hard_potential.exists( node ) ) {
        const auto ev_pot = __bn_node2hard_potential[node];
        if ( !pot_list.exists( ev_pot ) ) {
          pot_list.insert( ev_pot );
        }
      }
    }

    // add the messages sent by adjacent nodes to targetClique
    for ( const auto other : __JT->neighbours( targetClique ) )
      for ( const auto pot : __sep_potentials[Arc( other, targetClique )] )
        pot_list.insert( pot );

    // get the set of variables that need be removed from the potentials
    const NodeSet& nodes = __JT->clique( targetClique );
    Set<const DiscreteVariable*> del_vars( nodes.size() );
    Set<const DiscreteVariable*> kept_vars( nodes.size() );

    for ( const auto node : nodes ) {
      if ( node != id ) {
        del_vars.insert( &( this->bn().variable( node ) ) );
      } else {
        kept_vars.insert( &( this->bn().variable( node ) ) );
      }
    }
    // pot_list now contains all the potentials to multiply and marginalize
    // => combine the messages
    __marginalizeOut( pot_list, del_vars, kept_vars );

    if ( pot_list.size() == 1 ) {
      marginal = **pot_list.begin();
    } else {
      Set<const Potential<GUM_SCALAR>*> set;
      for ( const auto pot : pot_list )
        set << pot;

      MultiDimCombinationDefault<GUM_SCALAR, Potential> fast_combination(
                                                                         LPNewmultiPotential );

      fast_combination.combine( marginal, set );
    }
  }


  // returns the joint a posteriori proba of a given set of nodes
  template <typename GUM_SCALAR>
  void LazyPropagation<GUM_SCALAR>::__aPosterioriJoint(
                                                       const NodeSet& ids, Potential<GUM_SCALAR>& marginal ) {
    // compute barren nodes if necessary
    __computeBarrenPotentials();

    // find a clique that contains all the nodes in ids. To do so, we loop over
    // all the cliques and check wheither there exists one with this feature
    NodeId clique_of_ids = 0;
    bool clique_found = false;

    for ( const auto node : __JT->nodes() ) {
      // get the nodes contained in the clique
      const NodeSet& clique = __JT->clique( node );
      // check whether the clique actually contains all of ids
      bool clique_ok = true;

      for ( const auto node2 : ids ) {
        if ( !clique.contains( node2 ) ) {
          clique_ok = false;
          break;
        }
      }

      // check if we found the clique we wanted
      if ( clique_ok ) {
        clique_of_ids = node;
        clique_found = true;
        break;
      }
    }

    // check if we actually found the clique we were interested in
    if ( !clique_found ) {
      GUM_ERROR( OperationNotAllowed,
                 "no clique was found to compute the joint probability" );
    }

    // for the moment, always perform an inference before computing the
    // joint a posteriori distribution
    makeInference( true );

    // now we just need to create the product of the potentials of the clique
    // containing id with the messages received by this clique and
    // marginalize out all variables except id
    const List<const Potential<GUM_SCALAR>*>& clique_pot =
      __clique_potentials[clique_of_ids];

    // get the potentials of the clique
    __PotentialSet pot_list( clique_pot.size() +
                             __clique_evidence[clique_of_ids].size() );

    for ( ListConstIteratorSafe<const Potential<GUM_SCALAR>*> iter =
            clique_pot.cbeginSafe();
          iter != clique_pot.cendSafe();
          ++iter ) {
      pot_list.insert( *iter );
    }

    // add the evidence to the clique potentials
    const List<const Potential<GUM_SCALAR>*>& evidence_list =
      __clique_evidence[clique_of_ids];

    for ( ListConstIteratorSafe<const Potential<GUM_SCALAR>*> iter =
            evidence_list.cbeginSafe();
          iter != evidence_list.cendSafe();
          ++iter ) {
      pot_list.insert( *iter );
    }

    // if some node of the clique received a hard evidence, add this evidence
    // this is compulsory because, during the messages computations, we
    // marginalized out the hard evidence nodes from the CPTs prior to the
    // combinations. Hence, we must marginalize out these nodes on all tables
    // that contain them
    for ( const auto node : __JT->clique( clique_of_ids ) ) {
      if ( __bn_node2hard_potential.exists( node ) ) {
        const auto ev_pot = __bn_node2hard_potential[node];
        if ( !pot_list.exists( ev_pot ) ) {
          pot_list.insert( ev_pot );
        }
      }
    }


    // add the messages sent by adjacent nodes to clique_of_ids
    for ( const auto other : __JT->neighbours( clique_of_ids ) )
      for ( const auto pot : __sep_potentials[Arc( other, clique_of_ids )] ) {
        pot_list.insert( pot );
      }

    // get the set of variables that need be removed from the potentials
    const NodeSet& nodes = __JT->clique( clique_of_ids );

    Set<const DiscreteVariable*> del_vars( nodes.size() );
    Set<const DiscreteVariable*> kept_vars( nodes.size() );

    for ( const auto node : nodes ) {
      if ( !ids.contains( node ) ) {
        del_vars.insert( &( this->bn().variable( node ) ) );
      } else {
        kept_vars.insert( &( this->bn().variable( node ) ) );
      }
    }

    // ok, now, pot_list contains all the potentials to multiply and marginalize
    // => now, combine the messages
    __marginalizeOut( pot_list, del_vars, kept_vars );

    if ( pot_list.size() == 1 ) {
      marginal = **pot_list.begin();
    } else {
      Set<const Potential<GUM_SCALAR>*> set;

      for ( const auto pot : pot_list )
        set << pot;

      MultiDimCombinationDefault<GUM_SCALAR, Potential> fast_combination(
                                                                         LPNewmultiPotential );

      fast_combination.combine( marginal, set );
    }
  }

  template <typename GUM_SCALAR>
  INLINE const JunctionTree* LazyPropagation<GUM_SCALAR>::junctionTree() const {
    return __JT;
  }

  // returns the marginal a posteriori proba of a given node

  template <typename GUM_SCALAR>
  INLINE void LazyPropagation<GUM_SCALAR>::_fillPosterior(
                                                          NodeId id, Potential<GUM_SCALAR>& posterior ) {
    __aPosterioriMarginal( id, posterior );
    posterior.normalize();
  }

  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR LazyPropagation<GUM_SCALAR>::evidenceProbability() {
    // TODO: il y a un bug dans cette fonction: actuellement, je choisis un
    // noeud X sur lequel je fais une collecte, et je calcule P(e) comme etant
    // P(e) = sum_X P(X,e). Mais s'il y a plusieurs composantes connexes dans
    // le reseau bayesien, ca ne fonctionne pas, il faudrait choisir un X par
    // composante connexe et multiplier entre elle les probas P(e) obtenues sur
    // chaque composante. So un TODO a faire rapidement.
    Potential<GUM_SCALAR>* tmp = new Potential<GUM_SCALAR>();
    Id id = __node_to_clique.begin().key();
    __aPosterioriMarginal( id, *tmp );

    GUM_SCALAR sum = 0;
    Instantiation iter( *tmp );

    for ( iter.setFirst(); !iter.end(); ++iter )
      sum += tmp->get( iter );

    delete tmp;

    return sum;
  }

  // returns the joint a posteriori probability P(nodes|e)
  /** @warning right now, method joint cannot compute joint a posteriori
   * probabilities of every nodeset. In cases where it is not able to perform
   * properly this task, it will raise a OperationNotAllowed exception. */

  template <typename GUM_SCALAR>
  INLINE Potential<GUM_SCALAR>*
  LazyPropagation<GUM_SCALAR>::joint( const NodeSet& nodes ) {
    Potential<GUM_SCALAR>* res = new Potential<GUM_SCALAR>();

    try {
      __aPosterioriJoint( nodes, *res );
      res->normalize();
    } catch ( OperationNotAllowed& e ) {
      delete ( res );
      throw;
    }

    return res;
  }

  /** Entropy
   * Compute Shanon's entropy of a node given the observation
   */
  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR LazyPropagation<GUM_SCALAR>::H( NodeId X ) {
    return this->posterior( X ).entropy();
  }

  /** Mutual information between X and Y
   * @see http://en.wikipedia.org/wiki/Mutual_information
   *
   * @warning Due to limitation of @joint, may not be able to compute this value
   * @throw OperationNotAllowed in these cases
   */
  template <typename GUM_SCALAR>
  GUM_SCALAR LazyPropagation<GUM_SCALAR>::I( NodeId X, NodeId Y ) {
    const Potential<GUM_SCALAR>& pX = this->posterior( X );
    const Potential<GUM_SCALAR>& pY = this->posterior( Y );

    NodeSet XY;
    XY << X << Y;
    Potential<GUM_SCALAR>& pXY = *( joint( XY ) );

    Instantiation i( pXY );
    GUM_SCALAR res = (GUM_SCALAR)0;

    for ( i.setFirst(); !i.end(); ++i ) {
      GUM_SCALAR vXY = pXY[i];
      GUM_SCALAR vX = pX[i];
      GUM_SCALAR vY = pY[i];

      if ( vXY > (GUM_SCALAR)0 ) {
        if ( vX == (GUM_SCALAR)0 || vY == (GUM_SCALAR)0 ) {
          GUM_ERROR(
                    OperationNotAllowed,
                    "Mutual Information (X,Y) with P(X)=0 or P(Y)=0 and P(X,Y)>0" );
        }

        res += vXY * ( log2( vXY ) - log2( vX ) - log2( vY ) );
      }
    }

    delete ( &pXY );

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
