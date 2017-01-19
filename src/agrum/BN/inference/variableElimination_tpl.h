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
 * @brief Implementation of Variable Elimination for inference in
 * Bayesian Networks.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <agrum/BN/inference/variableElimination.h>

#include <agrum/BN/inference/BayesBall.h>
#include <agrum/BN/inference/dSeparation.h>
#include <agrum/graphs/binaryJoinTreeConverterDefault.h>
#include <agrum/multidim/instantiation.h>
#include <agrum/multidim/operators/multiDimCombineAndProjectDefault.h>
#include <agrum/multidim/operators/multiDimProjection.h>


namespace gum {


  // default constructor
  template <typename GUM_SCALAR>
  INLINE VariableElimination<GUM_SCALAR>::VariableElimination(
      const IBayesNet<GUM_SCALAR>* BN,
      RelevantPotentialsFinderType relevant_type,
      FindBarrenNodesType          barren_type )
      : JointTargetedInference<GUM_SCALAR>( BN ) {
    // sets the relevant potential and the barren nodes finding algorithm
    setRelevantPotentialsFinderType( relevant_type );
    setFindBarrenNodesType( barren_type );

    // create a default triangulation (the user can change it afterwards)
    __triangulation = new DefaultTriangulation;

    // for debugging purposessetRequiredInference
    GUM_CONSTRUCTOR( VariableElimination );
  }


  // destructor
  template <typename GUM_SCALAR>
  INLINE VariableElimination<GUM_SCALAR>::~VariableElimination() {
    // remove the junction tree and the triangulation algorithm
    if ( __JT != nullptr ) delete __JT;
    delete __triangulation;
    if ( __target_posterior != nullptr ) delete __target_posterior;

    // for debugging purposes
    GUM_DESTRUCTOR( VariableElimination );
  }


  /// set a new triangulation algorithm
  template <typename GUM_SCALAR>
  void VariableElimination<GUM_SCALAR>::setTriangulation(
      const Triangulation& new_triangulation ) {
    delete __triangulation;
    __triangulation = new_triangulation.newFactory();
  }


  /// returns the current join tree used
  template <typename GUM_SCALAR>
  INLINE const JunctionTree*
  VariableElimination<GUM_SCALAR>::junctionTree( const NodeId id ) {
    __createNewJT( NodeSet{id} );

    return __JT;
  }


  /// sets the operator for performing the projections
  template <typename GUM_SCALAR>
  INLINE void VariableElimination<GUM_SCALAR>::__setProjectionFunction(
      Potential<GUM_SCALAR>* ( *proj )(const Potential<GUM_SCALAR>&,
                                       const Set<const DiscreteVariable*>&)) {
    __projection_op = proj;
  }


  /// sets the operator for performing the combinations
  template <typename GUM_SCALAR>
  INLINE void VariableElimination<GUM_SCALAR>::__setCombinationFunction(
      Potential<GUM_SCALAR>* ( *comb )(const Potential<GUM_SCALAR>&,
                                       const Potential<GUM_SCALAR>&)) {
    __combination_op = comb;
  }


  /// sets how we determine the relevant potentials to combine
  template <typename GUM_SCALAR>
  void VariableElimination<GUM_SCALAR>::setRelevantPotentialsFinderType(
      RelevantPotentialsFinderType type ) {
    if ( type != __find_relevant_potential_type ) {
      switch ( type ) {
        case RelevantPotentialsFinderType::DSEP_BAYESBALL_POTENTIALS:
          __findRelevantPotentials = &VariableElimination<
              GUM_SCALAR>::__findRelevantPotentialsWithdSeparation2;
          break;

        case RelevantPotentialsFinderType::DSEP_BAYESBALL_NODES:
          __findRelevantPotentials = &VariableElimination<
              GUM_SCALAR>::__findRelevantPotentialsWithdSeparation;
          break;

        case RelevantPotentialsFinderType::DSEP_KOLLER_FRIEDMAN_2009:
          __findRelevantPotentials = &VariableElimination<
              GUM_SCALAR>::__findRelevantPotentialsWithdSeparation3;
          break;

        case RelevantPotentialsFinderType::FIND_ALL:
          __findRelevantPotentials =
              &VariableElimination<GUM_SCALAR>::__findRelevantPotentialsGetAll;
          break;

        default:
          GUM_ERROR( InvalidArgument,
                     "setRelevantPotentialsFinderType for type "
                         << (unsigned int)type
                         << " is not implemented yet" );
      }

      __find_relevant_potential_type = type;
    }
  }


  /// sets how we determine barren nodes
  template <typename GUM_SCALAR>
  void VariableElimination<GUM_SCALAR>::setFindBarrenNodesType(
      FindBarrenNodesType type ) {
    if ( type != __barren_nodes_type ) {
      // WARNING: if a new type is added here, method __createJT should
      // certainly
      // be updated as well, in particular its step 2.
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
    }
  }


  /// fired when a new evidence is inserted
  template <typename GUM_SCALAR>
  INLINE void VariableElimination<GUM_SCALAR>::_onEvidenceAdded( const NodeId,
                                                                 bool ) {}


  /// fired when an evidence is removed
  template <typename GUM_SCALAR>
  INLINE void VariableElimination<GUM_SCALAR>::_onEvidenceErased( const NodeId,
                                                                  bool ) {}


  /// fired when all the evidence are erased
  template <typename GUM_SCALAR>
  void VariableElimination<GUM_SCALAR>::_onAllEvidenceErased( bool ) {}


  /// fired when an evidence is changed
  template <typename GUM_SCALAR>
  INLINE void VariableElimination<GUM_SCALAR>::_onEvidenceChanged( const NodeId,
                                                                   bool ) {}


  /// fired after a new target is inserted
  template <typename GUM_SCALAR>
  INLINE void
  VariableElimination<GUM_SCALAR>::_onMarginalTargetAdded( const NodeId ) {}


  /// fired before a target is removed
  template <typename GUM_SCALAR>
  INLINE void
  VariableElimination<GUM_SCALAR>::_onMarginalTargetErased( const NodeId ) {}


  /// fired after a new set target is inserted
  template <typename GUM_SCALAR>
  INLINE void
  VariableElimination<GUM_SCALAR>::_onJointTargetAdded( const NodeSet& ) {}


  /// fired before a set target is removed
  template <typename GUM_SCALAR>
  INLINE void
  VariableElimination<GUM_SCALAR>::_onJointTargetErased( const NodeSet& ) {}


  /// fired after all the nodes of the BN are added as single targets
  template <typename GUM_SCALAR>
  INLINE void VariableElimination<GUM_SCALAR>::_onAllMarginalTargetsAdded() {}


  /// fired before a all the single_targets are removed
  template <typename GUM_SCALAR>
  INLINE void VariableElimination<GUM_SCALAR>::_onAllMarginalTargetsErased() {}


  /// fired before a all the joint_targets are removed
  template <typename GUM_SCALAR>
  INLINE void VariableElimination<GUM_SCALAR>::_onAllJointTargetsErased() {}


  /// fired before a all the single and joint_targets are removed
  template <typename GUM_SCALAR>
  INLINE void VariableElimination<GUM_SCALAR>::_onAllTargetsErased() {}


  /// create a new junction tree as well as its related data structures
  template <typename GUM_SCALAR>
  void VariableElimination<GUM_SCALAR>::__createNewJT( const NodeSet& targets ) {
    // to create the JT, we first create the moral graph of the BN in the
    // following way in order to take into account the barren nodes and the
    // nodes that received evidence:
    // 1/ we create an undirected graph containing only the nodes and no edge
    // 2/ if we take into account barren nodes, remove them from the graph
    // 3/ if we take d-separation into account, remove the d-separated nodes
    // 4/ add edges so that each node and its parents in the BN form a clique
    // 5/ add edges so that the targets form a clique of the moral graph
    // 6/ remove the nodes that received hard evidence (by step 4/, their
    //    parents are linked by edges, which is necessary for inference)
    //
    // At the end of step 6/, we have our moral graph and we can triangulate it
    // to get the new junction tree

    // 1/ create an undirected graph containing only the nodes and no edge
    const auto& bn = this->BN();
    __graph.clear();
    for ( auto node : bn.dag() )
      __graph.addNode( node );

    // 2/ if we wish to exploit barren nodes, we shall remove them from the BN
    // to do so: we identify all the nodes that are not targets and have
    // received no evidence and such that their descendants are neither targets
    // nor evidence nodes. Such nodes can be safely discarded from the BN
    // without altering the inference output
    if ( __barren_nodes_type == FindBarrenNodesType::FIND_BARREN_NODES ) {
      // check that all the nodes are not targets, otherwise, there is no
      // barren node
      if ( targets.size() != bn.dag().sizeNodes() ) {
        BarrenNodesFinder finder( &( bn.dag() ) );
        finder.setTargets( &targets );

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

    // 3/ if we wish to exploit d-separation, remove all the nodes that are
    // d-separated from our targets
    {
      NodeSet requisite_nodes;
      bool    dsep_analysis = false;
      switch ( __find_relevant_potential_type ) {
        case RelevantPotentialsFinderType::DSEP_BAYESBALL_POTENTIALS:
        case RelevantPotentialsFinderType::DSEP_BAYESBALL_NODES: {
          BayesBall::requisiteNodes( bn.dag(),
                                     targets,
                                     this->hardEvidenceNodes(),
                                     this->softEvidenceNodes(),
                                     requisite_nodes );
          dsep_analysis = true;
        } break;

        case RelevantPotentialsFinderType::DSEP_KOLLER_FRIEDMAN_2009: {
          dSeparation dsep;
          dsep.requisiteNodes( bn.dag(),
                               targets,
                               this->hardEvidenceNodes(),
                               this->softEvidenceNodes(),
                               requisite_nodes );
          dsep_analysis = true;
        } break;

        case RelevantPotentialsFinderType::FIND_ALL:
          break;

        default:
          GUM_ERROR( FatalError, "not implemented yet" );
      }

      // remove all the nodes that are not requisite
      if ( dsep_analysis ) {
        for ( auto iter = __graph.beginSafe(); iter != __graph.endSafe();
              ++iter ) {
          if ( !requisite_nodes.contains( *iter ) &&
               !this->hardEvidenceNodes().contains( *iter ) ) {
            __graph.eraseNode( *iter );
          }
        }
      }
    }

    // 4/ add edges so that each node and its parents in the BN form a clique
    for ( const auto node : __graph ) {
      const NodeSet& parents = bn.dag().parents( node );
      for ( auto iter1 = parents.cbegin(); iter1 != parents.cend(); ++iter1 ) {
        // before adding an edge between node and its parent, check that the
        // parent belong to the graph. Actually, when d-separated nodes are
        // removed, it may be the case that the parents of hard evidence nodes
        // are removed. But the latter still exist in the graph.
        if ( __graph.existsNode( *iter1 ) ) __graph.addEdge( *iter1, node );

        auto iter2 = iter1;
        for ( ++iter2; iter2 != parents.cend(); ++iter2 ) {
          // before adding an edge, check that both extremities belong to
          // the graph. Actually, when d-separated nodes are removed, it may
          // be the case that the parents of hard evidence nodes are removed.
          // But the latter still exist in the graph.
          if ( __graph.existsNode( *iter1 ) && __graph.existsNode( *iter2 ) )
            __graph.addEdge( *iter1, *iter2 );
        }
      }
    }

    // 5/ if targets contains several nodes, we shall add new edges into the
    // moral graph in order to ensure that there exists a clique containing
    // thier joint distribution
    for ( auto iter1 = targets.cbegin(); iter1 != targets.cend(); ++iter1 ) {
      auto iter2 = iter1;
      for ( ++iter2; iter2 != targets.cend(); ++iter2 ) {
        __graph.addEdge( *iter1, *iter2 );
      }
    }

    // 6/ remove all the nodes that received hard evidence
    for ( const auto node : this->hardEvidenceNodes() ) {
      __graph.eraseNode( node );
    }


    // now, we can compute the new junction tree.
    if ( __JT != nullptr ) delete __JT;
    __triangulation->setGraph( &__graph, &( this->domainSizes() ) );
    const JunctionTree& triang_jt = __triangulation->junctionTree();
    __JT = new CliqueGraph( triang_jt );

    // indicate, for each node of the moral graph a clique in __JT that can
    // contain its conditional probability table
    __node_to_clique.clear();
    __clique_potentials.clear();
    NodeSet emptyset;
    for ( auto clique : *__JT )
      __clique_potentials.insert( clique, emptyset );
    const std::vector<NodeId>& JT_elim_order = __triangulation->eliminationOrder();
    NodeProperty<int>          elim_order( Size( JT_elim_order.size() ) );
    for ( std::size_t i = std::size_t( 0 ), size = JT_elim_order.size(); i < size;
          ++i )
      elim_order.insert( JT_elim_order[i], NodeId( i ) );
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
      NodeId clique =
          __triangulation->createdJunctionTreeClique( first_eliminated_node );
      __node_to_clique.insert( node, clique );
      __clique_potentials[clique].insert( node );
    }

    // do the same for the nodes that received evidence. Here, we only store
    // the nodes whose at least one parent belongs to __graph (otherwise
    // their CPT is just a constant real number).
    for ( const auto node : this->hardEvidenceNodes() ) {
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
        NodeId clique =
            __triangulation->createdJunctionTreeClique( first_eliminated_node );
        __node_to_clique.insert( node, clique );
        __clique_potentials[clique].insert( node );
      }
    }


    // indicate a clique that contains all the nodes of targets
    __targets2clique = std::numeric_limits<NodeId>::max();
    {
      // remove from set all the nodes that received hard evidence (since they
      // do not belong to the join tree)
      NodeSet nodeset = targets;
      for ( const auto node : this->hardEvidenceNodes() )
        if ( nodeset.contains( node ) ) nodeset.erase( node );

      if ( !nodeset.empty() ) {
        NodeId first_eliminated_node = *( nodeset.begin() );
        int    elim_number = elim_order[first_eliminated_node];
        for ( const auto node : nodeset ) {
          if ( elim_order[node] < elim_number ) {
            elim_number = elim_order[node];
            first_eliminated_node = node;
          }
        }
        __targets2clique =
          __triangulation->createdJunctionTreeClique( first_eliminated_node );
      }
    }
  }


  /// prepare the inference structures w.r.t. new targets, soft/hard evidence
  template <typename GUM_SCALAR>
  void VariableElimination<GUM_SCALAR>::_updateOutdatedBNStructure() {}


  /// update the potentials stored in the cliques and invalidate outdated
  /// messages
  template <typename GUM_SCALAR>
  void VariableElimination<GUM_SCALAR>::_updateOutdatedBNPotentials() {}


  // find the potentials d-connected to a set of variables
  template <typename GUM_SCALAR>
  void VariableElimination<GUM_SCALAR>::__findRelevantPotentialsGetAll(
      Set<const Potential<GUM_SCALAR>*>& pot_list,
      Set<const DiscreteVariable*>&      kept_vars ) {}


  // find the potentials d-connected to a set of variables
  template <typename GUM_SCALAR>
  void VariableElimination<GUM_SCALAR>::__findRelevantPotentialsWithdSeparation(
      Set<const Potential<GUM_SCALAR>*>& pot_list,
      Set<const DiscreteVariable*>&      kept_vars ) {
    // find the node ids of the kept variables
    NodeSet     kept_ids;
    const auto& bn = this->BN();
    for ( const auto var : kept_vars ) {
      kept_ids.insert( bn.nodeId( *var ) );
    }

    // determine the set of potentials d-connected with the kept variables
    NodeSet requisite_nodes;
    BayesBall::requisiteNodes( bn.dag(),
                               kept_ids,
                               this->hardEvidenceNodes(),
                               this->softEvidenceNodes(),
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
  void VariableElimination<GUM_SCALAR>::__findRelevantPotentialsWithdSeparation2(
      Set<const Potential<GUM_SCALAR>*>& pot_list,
      Set<const DiscreteVariable*>&      kept_vars ) {
    // find the node ids of the kept variables
    NodeSet     kept_ids;
    const auto& bn = this->BN();
    for ( const auto var : kept_vars ) {
      kept_ids.insert( bn.nodeId( *var ) );
    }

    // determine the set of potentials d-connected with the kept variables
    BayesBall::relevantPotentials( bn,
                                   kept_ids,
                                   this->hardEvidenceNodes(),
                                   this->softEvidenceNodes(),
                                   pot_list );
  }


  // find the potentials d-connected to a set of variables
  template <typename GUM_SCALAR>
  void VariableElimination<GUM_SCALAR>::__findRelevantPotentialsWithdSeparation3(
      Set<const Potential<GUM_SCALAR>*>& pot_list,
      Set<const DiscreteVariable*>&      kept_vars ) {
    // find the node ids of the kept variables
    NodeSet     kept_ids;
    const auto& bn = this->BN();
    for ( const auto var : kept_vars ) {
      kept_ids.insert( bn.nodeId( *var ) );
    }

    // determine the set of potentials d-connected with the kept variables
    dSeparation dsep;
    dsep.relevantPotentials( bn,
                             kept_ids,
                             this->hardEvidenceNodes(),
                             this->softEvidenceNodes(),
                             pot_list );
  }


  // find the potentials d-connected to a set of variables
  template <typename GUM_SCALAR>
  void VariableElimination<GUM_SCALAR>::__findRelevantPotentialsXX(
      Set<const Potential<GUM_SCALAR>*>& pot_list,
      Set<const DiscreteVariable*>&      kept_vars ) {
    switch ( __find_relevant_potential_type ) {
      case RelevantPotentialsFinderType::DSEP_BAYESBALL_POTENTIALS:
        __findRelevantPotentialsWithdSeparation2( pot_list, kept_vars );
        break;

      case RelevantPotentialsFinderType::DSEP_BAYESBALL_NODES:
        __findRelevantPotentialsWithdSeparation( pot_list, kept_vars );
        break;

      case RelevantPotentialsFinderType::DSEP_KOLLER_FRIEDMAN_2009:
        __findRelevantPotentialsWithdSeparation3( pot_list, kept_vars );
        break;

      case RelevantPotentialsFinderType::FIND_ALL:
        __findRelevantPotentialsGetAll( pot_list, kept_vars );
        break;

      default:
        GUM_ERROR( FatalError, "not implemented yet" );
    }
  }


  // remove barren variables
  template <typename GUM_SCALAR>
  Set<const Potential<GUM_SCALAR>*>
  VariableElimination<GUM_SCALAR>::__removeBarrenVariables(
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
    MultiDimProjection<GUM_SCALAR, Potential> projector( VENewprojPotential );
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


  // performs the collect phase of Lazy Propagation
  template <typename GUM_SCALAR>
  std::pair<Set<const Potential<GUM_SCALAR>*>, Set<const Potential<GUM_SCALAR>*>>
  VariableElimination<GUM_SCALAR>::__collectMessage( const NodeId id,
                                                     const NodeId from ) {
    // collect messages from all the neighbors
    std::pair<__PotentialSet, __PotentialSet> collect_messages;
    for ( const auto other : __JT->neighbours( id ) ) {
      if ( other != from ) {
        std::pair<__PotentialSet, __PotentialSet> message(
            __collectMessage( other, id ) );
        collect_messages.first += message.first;
        collect_messages.second += message.second;
      }
    }

    // combine the collect messages with those of id's clique
    return __produceMessage( id, from, std::move( collect_messages ) );
  }


  // get the CPT + evidence of a node projected w.r.t. hard evidence
  template <typename GUM_SCALAR>
  std::pair<Set<const Potential<GUM_SCALAR>*>, Set<const Potential<GUM_SCALAR>*>>
  VariableElimination<GUM_SCALAR>::__NodePotentials( const NodeId node ) {
    std::pair<__PotentialSet, __PotentialSet> res;
    const auto& bn = this->BN();

    // get the CPT's of the node
    // beware: all the potentials that are defined over some nodes
    // including hard evidence must be projected so that these nodes are
    // removed from the potential
    // also beware that the CPT of a hard evidence node may be defined over
    // parents that do not belong to __graph and that are not hard evidence.
    // In this case, those parents have been removed by d-separation and it is
    // easy to show that, in this case all the parents have been removed, so
    // that the CPT does not need to be taken into account
    const auto& evidence = this->evidence();
    const auto& hard_evidence = this->hardEvidence();
    if ( __graph.exists( node ) || this->hardEvidenceNodes().contains( node ) ) {
      const Potential<GUM_SCALAR>& cpt = bn.cpt( node );
      const auto&                  variables = cpt.variablesSequence();

      // check if the parents of a hard evidence node do not belong to __graph
      // and are not themselves hard evidence, discard the CPT, it is useless
      // for inference
      if ( this->hardEvidenceNodes().contains( node ) ) {
        for ( const auto var : variables ) {
          const NodeId xnode = bn.nodeId( *var );
          if ( !this->hardEvidenceNodes().contains( xnode ) &&
               !__graph.existsNode( xnode ) )
            return res;
        }
      }

      // get the list of nodes with hard evidence in cpt
      NodeSet hard_nodes;
      for ( const auto var : variables ) {
        const NodeId xnode = bn.nodeId( *var );
        if ( this->hardEvidenceNodes().contains( xnode ) )
          hard_nodes.insert( xnode );
      }

      // if hard_nodes contains hard evidence nodes, perform a projection
      // and insert the result into the appropriate clique, else insert
      // directly cpt into the clique
      if ( hard_nodes.empty() ) {
        res.first.insert( &cpt );
      } else {
        // marginalize out the hard evidence nodes: if the cpt is defined
        // only over nodes that received hard evidence, do not consider it
        // as a potential anymore
        if ( hard_nodes.size() != variables.size() ) {
          // perform the projection with a combine and project instance
          Set<const DiscreteVariable*> hard_variables;
          __PotentialSet               marg_cpt_set{&cpt};
          for ( const auto xnode : hard_nodes ) {
            marg_cpt_set.insert( evidence[xnode] );
            hard_variables.insert( &( bn.variable( xnode ) ) );
          }
          // perform the combination of those potentials and their projection
          MultiDimCombineAndProjectDefault<GUM_SCALAR, Potential>
                         combine_and_project( __combination_op, VENewprojPotential );
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
          res.first.insert( projected_cpt );
          res.second.insert( projected_cpt );
        }
      }

      // if the node received some soft evidence, add it
      if ( evidence.exists( node ) && !hard_evidence.exists( node ) ) {
        res.first.insert( this->evidence()[node] );
      }
    }

    return res;
  }


  // creates the message sent by clique from_id to clique to_id
  template <typename GUM_SCALAR>
  std::pair<Set<const Potential<GUM_SCALAR>*>, Set<const Potential<GUM_SCALAR>*>>
  VariableElimination<GUM_SCALAR>::__produceMessage(
      const NodeId from_id,
      const NodeId to_id,
      std::pair<Set<const Potential<GUM_SCALAR>*>,
                Set<const Potential<GUM_SCALAR>*>>&& incoming_messages ) {
    // get the messages sent by adjacent nodes to from_id
    std::pair<Set<const Potential<GUM_SCALAR>*>, Set<const Potential<GUM_SCALAR>*>>
        pot_list( std::move( incoming_messages ) );

    // get the potentials of the clique
    for ( const auto node : __clique_potentials[from_id] ) {
      auto new_pots = __NodePotentials( node );
      pot_list.first += new_pots.first;
      pot_list.second += new_pots.second;
    }

    // if from_id = to_id: this is the endpoint of a collect
    if ( !__JT->existsEdge( from_id, to_id ) ) {
      return pot_list;
    } else {
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
          __marginalizeOut( pot_list.first, del_vars, kept_vars );

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
            if ( !pot_list.first.exists( pot ) ) delete pot;
            new_pot_list.erase( iter );
            continue;
          }
        }
      }

      // remove the unnecessary temporary messages
      for ( auto iter = pot_list.second.beginSafe();
            iter != pot_list.second.endSafe();
            ++iter ) {
        if ( !new_pot_list.contains( *iter ) ) {
          delete *iter;
          pot_list.second.erase( iter );
        }
      }

      // keep track of all the newly created potentials
      for ( const auto pot : new_pot_list ) {
        if ( !pot_list.first.contains( pot ) ) {
          pot_list.second.insert( pot );
        }
      }

      // return the new set of potentials
      return std::pair<__PotentialSet, __PotentialSet>(
          std::move( new_pot_list ), std::move( pot_list.second ) );
    }
  }


  // remove variables del_vars from the list of potentials pot_list
  template <typename GUM_SCALAR>
  Set<const Potential<GUM_SCALAR>*>
  VariableElimination<GUM_SCALAR>::__marginalizeOut(
      Set<const Potential<GUM_SCALAR>*> pot_list,
      Set<const DiscreteVariable*>&     del_vars,
      Set<const DiscreteVariable*>&     kept_vars ) {
    // use d-separation analysis to check which potentials shall be combined
    __findRelevantPotentialsXX( pot_list, kept_vars );

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


  // performs a whole inference
  template <typename GUM_SCALAR>
  INLINE void VariableElimination<GUM_SCALAR>::_makeInference() {}


  /// returns a fresh potential equal to P(1st arg,evidence)
  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>*
  VariableElimination<GUM_SCALAR>::_unnormalizedJointPosterior( const NodeId id ) {
    const auto& bn = this->BN();

    // hard evidence do not belong to the join tree
    // # TODO: check for sets of inconsistent hard evidence
    if ( this->hardEvidenceNodes().contains( id ) ) {
      return new Potential<GUM_SCALAR>( *( this->evidence()[id] ) );
    }

    // if we still need to perform some inference task, do it
    __createNewJT( NodeSet{id} );
    const NodeId clique_of_id = __node_to_clique[id];
    auto         pot_list = __collectMessage( clique_of_id, clique_of_id );

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
        __marginalizeOut( pot_list.first, del_vars, kept_vars );
    Potential<GUM_SCALAR>* joint = nullptr;

    if ( new_pot_list.size() == 1 ) {
      joint = const_cast<Potential<GUM_SCALAR>*>( *( new_pot_list.begin() ) );
      // if joint already existed, create a copy, so that we can put it into
      // the __target_posterior property
      if ( pot_list.first.exists( joint ) ) {
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
      if ( !pot_list.first.exists( pot ) ) delete pot;

    // remove all the temporary potentials created in pot_list
    for ( auto pot : pot_list.second )
      delete pot;

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
  VariableElimination<GUM_SCALAR>::_posterior( const NodeId id ) {
    // compute the joint posterior and normalize
    auto joint = _unnormalizedJointPosterior( id );
    joint->normalize();

    if ( __target_posterior != nullptr ) delete __target_posterior;
    __target_posterior = joint;

    return *joint;
  }


  // returns the marginal a posteriori proba of a given node
  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>*
  VariableElimination<GUM_SCALAR>::_unnormalizedJointPosterior(
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


    // if we still need to perform some inference task, do it
    __createNewJT( set );
    auto pot_list = __collectMessage( __targets2clique, __targets2clique );

    // get the set of variables that need be removed from the potentials
    const NodeSet&               nodes = __JT->clique( __targets2clique );
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
        __marginalizeOut( pot_list.first, del_vars, kept_vars );
    Potential<GUM_SCALAR>* joint = nullptr;

    if ( ( new_pot_list.size() == 1 ) && hard_ev_nodes.empty() ) {
      joint = const_cast<Potential<GUM_SCALAR>*>( *( new_pot_list.begin() ) );
      // if pot already existed, create a copy, so that we can put it into
      // the __target_posteriors property
      if ( pot_list.first.exists( joint ) ) {
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
      if ( !pot_list.first.exists( pot ) ) delete pot;

    // remove all the temporary potentials created in pot_list
    for ( auto pot : pot_list.second )
      delete pot;

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
  VariableElimination<GUM_SCALAR>::_jointPosterior( const NodeSet& set ) {
    // compute the joint posterior and normalize
    auto joint = _unnormalizedJointPosterior( set );
    joint->normalize();

    if ( __target_posterior != nullptr ) delete __target_posterior;
    __target_posterior = joint;

    return *joint;
  }


  /// returns the posterior of a given set of variables
  template <typename GUM_SCALAR>
  const Potential<GUM_SCALAR>& VariableElimination<GUM_SCALAR>::_jointPosterior(
      const NodeSet& wanted_target, const NodeSet& declared_target ) {
    return _jointPosterior( wanted_target );
  }


} /* namespace gum */

#endif  // DOXYGEN_SHOULD_SKIP_THIS
// kate: indent-mode cstyle; indent-width 2; replace-tabs on;
