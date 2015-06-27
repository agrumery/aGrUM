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
  INLINE static Potential<GUM_SCALAR> *
  LPNewmultiPotential(const Potential<GUM_SCALAR> &t1,
                      const Potential<GUM_SCALAR> &t2) {
    return new Potential<GUM_SCALAR>(t1 * t2);
  }

  // the function used to combine two tables
  template <typename GUM_SCALAR>
  INLINE static Potential<GUM_SCALAR> *
  LPNewprojPotential(const Potential<GUM_SCALAR> &t1,
                     const Set<const DiscreteVariable *> &del_vars) {
    return new Potential<GUM_SCALAR>(projectSum(t1, del_vars));
  }


  

  // initialization function
  template <typename GUM_SCALAR>
  INLINE void
  LazyPropagation<GUM_SCALAR>::__initialize(const IBayesNet<GUM_SCALAR> &BN,
                                            StaticTriangulation &triangulation,
                                            const NodeProperty<Size> &modalities) {
    const JunctionTree &triang_jt = triangulation.junctionTree();
    BinaryJoinTreeConverterDefault bon_converter;
    NodeSet emptyset;
    __JT = new CliqueGraph(bon_converter.convert(triang_jt, modalities, emptyset));
    __roots = bon_converter.roots();

    // indicate, for each node of the BN, a clique in __JT that can contain its
    // conditional probability table
    const std::vector<NodeId> &JT_elim_order = triangulation.eliminationOrder();
    HashTable<NodeId, unsigned int> elim_order(JT_elim_order.size());

    for (unsigned int i = 0; i < JT_elim_order.size(); ++i)
      elim_order.insert(JT_elim_order[i], i);

    const DAG &dag = BN.dag();

    for (const auto node : dag ) {
      // get the variables in the potential of iter_node
      NodeId first_eliminated_node = node;
      unsigned int elim_number = elim_order[node];

      for (const auto parent : dag.parents(node)) {
        if (elim_order[parent] < elim_number) {
          elim_number = elim_order[parent];
          first_eliminated_node = parent;
        }
      }

      // first_eliminated_node contains the first var (iter or one of its parents)
      // eliminated => the clique created during its elmination contains iter
      // and all of its parents => it can contain iter's potential
      __node_to_clique.insert( node, triangulation.createdJunctionTreeClique(first_eliminated_node));
    }

    // create empty potential lists into the cliques of the joint tree as well
    // as empty lists of evidence
    List<const Potential<GUM_SCALAR> *> empty_list;

    for (const auto node : __JT->nodes()) {
      __clique_potentials.insert(node, empty_list);
      __clique_evidence.insert(node, empty_list);
    }

    // put all the CPT's of the Bayes net nodes into the cliques
    for (const auto node : dag ) {
      const Potential<GUM_SCALAR> &cpt = BN.cpt(node);
      __clique_potentials[__node_to_clique[node]].insert(&cpt);
    }

    // create empty messages on the separators
    __PotentialSet empty_set;

    for (const auto edge : __JT->edges()) {
      __sep_potentials.insert(Arc(edge.first(), edge.second()), empty_set);
      __sep_potentials.insert(Arc(edge.second(), edge.first()), empty_set);
    }

    // indicate that __collect and __diffusion passed through no clique yet
    for (const auto node : __JT->nodes()) {
      __collected_cliques.insert(node, false);
      __diffused_cliques.insert(node, false);
    }
  }

  
  // default constructor
  template <typename GUM_SCALAR>
  INLINE
  LazyPropagation<GUM_SCALAR>::LazyPropagation(const IBayesNet<GUM_SCALAR> &BN)
    : BayesNetInference<GUM_SCALAR>(BN) {
    // for debugging purposessetRequiredInference
    GUM_CONSTRUCTOR(LazyPropagation);

    // set the findRelevantPotentials function
    __findRelevantPotentials =
      & LazyPropagation<GUM_SCALAR>::__findRelevantPotentialsWithdSeparation2;
  
    // set the correspondance between variables and their id and get the variables
    // domain sizes
    NodeProperty<Size> modalities;
    auto &bn = this->bn();

    for (const auto node : bn.dag().nodes())
      modalities.insert(node, bn.variable(node).domainSize());

    // initialize the __triangulation algorithm
    __triangulation.setGraph(&(bn.moralGraph()), &modalities);
    __initialize(bn, __triangulation, modalities);
  }

  
  // constructor with a given elimination sequence
  template <typename GUM_SCALAR>
  INLINE
  LazyPropagation<GUM_SCALAR>::LazyPropagation(const IBayesNet<GUM_SCALAR> &BN,
                                               const std::vector<NodeId> &elim_order)
    : BayesNetInference<GUM_SCALAR>(BN) {
    // for debugging purposessetRequiredInference
    GUM_CONSTRUCTOR(LazyPropagation);

    // set the findRelevantPotentials function
    __findRelevantPotentials =
      & LazyPropagation<GUM_SCALAR>::__findRelevantPotentialsWithdSeparation2;

    // set the correspondance between variables and their id and get the variables
    // domain sizes
    NodeProperty<Size> modalities;

    for (const auto node : this->bn().dag().nodes()) {
      const DiscreteVariable &var = this->bn().variable(node);
      modalities.insert(node, var.domainSize());
    }

    // initialize the __triangulation algorithm
    OrderedTriangulation triangulation(&(this->bn().moralGraph()), &modalities,
                                       &elim_order);
    __initialize(this->bn(), triangulation, modalities);
  }

  
  // destructor
  template <typename GUM_SCALAR>
  INLINE LazyPropagation<GUM_SCALAR>::~LazyPropagation() {
    // for debugging purposes
    GUM_DESTRUCTOR(LazyPropagation);
    // remove all the created potentials

    for (const auto pot : __created_potentials)
      delete (pot);

    __created_potentials.clear();

    delete __JT;
  }


  // indicates that we need inference in a given Junction tree connected component
  template <typename GUM_SCALAR>
  void LazyPropagation<GUM_SCALAR>::__setRequiredInference(NodeId id,
                                                           NodeId from) {
    // check if an inference has already happened through clique id
    if ((__collected_cliques[id] == false) && (__diffused_cliques[id] == false))
      return;

    // indicates that clique "id" needs an inference
    __collected_cliques[id] = false;
    __diffused_cliques[id] = false;

    // propagate this requirement to id's neighbours
    for (const auto other : __JT->neighbours(id)) {
      if (other != from) {
        // remove the potentials sent on clique id's adjacent separators
        for (const auto pot : __sep_potentials[Arc(other, id)]) {
          if ( __created_potentials.exists ( pot ) ) {
            delete pot;
            __created_potentials.erase ( pot );
          }
        }
        
        for (const auto pot : __sep_potentials[Arc(id, other)]) {
          if ( __created_potentials.exists ( pot ) ) {
            delete pot;
            __created_potentials.erase ( pot );
          }
        }

        __sep_potentials[Arc(other, id)].clear();
        __sep_potentials[Arc(id, other)].clear();

        // propagate the "required" state to the neighbours
        __setRequiredInference(other, id);
      }
    }
  }

  
  // remove a given evidence from the graph
  template <typename GUM_SCALAR>
  INLINE void
  LazyPropagation<GUM_SCALAR>::eraseEvidence(const Potential<GUM_SCALAR> *pot) {
    this->_invalidatePosteriors();
    
    // if the evidence does not exist, do nothing
    if (!__evidences.contains(pot))
      return;

    // get the variable involved in pot and indicate that this variable did
    // not receive any evidence
    const Sequence<const DiscreteVariable *> &vars = pot->variablesSequence();
    const NodeId var = this->bn().nodeId(*vars.atPos(0));
    __hard_evidence_nodes.erase ( var );
    __soft_evidence_nodes.erase ( var );
    
    // remove the potential from the list of evidence of the cliques
    for ( auto &elt : __clique_evidence ) {
      elt.second.eraseByVal(pot);
    }

    // remove the potential from the list of evidence
    __evidences.erase(pot);
    __bn_node2hard_potential.erase ( var );

    // indicate that we need to perform both __collect and __diffusion in the
    // connected component containing the variable of the evidence
    const NodeId pot_clique = __node_to_clique[var];
    __setRequiredInference(pot_clique, pot_clique);

    // indicate that we shall recompute the set of barren nodes/potentials
    __need_recompute_barren_potentials = true;
  }
  

  // remove all evidence from the graph
  template <typename GUM_SCALAR>
  INLINE void LazyPropagation<GUM_SCALAR>::eraseAllEvidence() {
    this->_invalidatePosteriors();
    // remove the evidence store in the cliques
    for (auto &elt : __clique_evidence)
      elt.second.clear();

    // remove the messages sent during a previous propagation
    for (auto &elt : __sep_potentials)
      elt.second.clear();

    // remove actually all the evidence taken into account
    __evidences.clear();
    __bn_node2hard_potential.clear();

    // indicate that no node received any evidence
    __hard_evidence_nodes.clear ();
    __soft_evidence_nodes.clear ();
 
    // remove from memory all the created potentials
    for (const auto pot : __created_potentials)
      delete pot;
    __created_potentials.clear();

    for (auto &elt : __collected_cliques)
      elt.second = false;

    for (auto &elt : __diffused_cliques)
      elt.second = false;

    // indicate that we shall recompute the set of barren nodes/potentials
    __need_recompute_barren_potentials = true;
  }

  
  /// clears the messages of previous inferences
  template <typename GUM_SCALAR>
  void LazyPropagation<GUM_SCALAR>::clearInference () {
    this->_invalidatePosteriors();

    // remove the messages sent during a previous propagation
    for (auto &elt : __sep_potentials)
      elt.second.clear();
 
    // remove from memory all the created potentials
    for (const auto pot : __created_potentials)
      delete pot;
    __created_potentials.clear();

    for (auto &elt : __collected_cliques)
      elt.second = false;

    for (auto &elt : __diffused_cliques)
      elt.second = false;
  }


  //template <typename GUM_SCALAR>
  template <typename GUM_SCALAR>
  INLINE bool
  LazyPropagation<GUM_SCALAR>::__isHardEvidence ( const Potential<GUM_SCALAR>* pot ) {
    Instantiation i(pot);
    unsigned int nb_non_zeros = 0;
    for ( i.setFirst (); ! i.end (); i.inc () ) {
      if ( pot->operator[] ( i ) != 0 ) {
        ++nb_non_zeros;
        if ( nb_non_zeros > 1 ) return false;
      }
    }

    return ( nb_non_zeros == 1);
  }


  // insert new evidence in the graph
  template <typename GUM_SCALAR>
  INLINE void LazyPropagation<GUM_SCALAR>::insertEvidence
  ( const List<const Potential<GUM_SCALAR> *> &pot_list) {
    this->_invalidatePosteriors();
    List<const Potential<GUM_SCALAR> *> empty_list;

    for (const auto pot : pot_list) {
      // check that the evidence is given w.r.t.only one random variable
      const Sequence<const DiscreteVariable *> &vars = pot->variablesSequence();
      if (vars.size() != 1) {
        GUM_ERROR(SizeError,
                  "Evidence can be be given w.r.t only one random variable for "
                  << pot);
      }
    }

    for (const auto pot : pot_list) {
      const DiscreteVariable *var = pot->variablesSequence().atPos(0);
      NodeId var_id = this->bn().nodeId(*var);
      NodeId clique_id = __node_to_clique[var_id]; // get the clique that
                                                   // contains the var
      auto& clique_evidences = __clique_evidence[clique_id];
 
      // remove already existing evidence w.r.t. variable in pot
      for ( auto ev_pot : clique_evidences ) {
        if ( var == ev_pot->variablesSequence().atPos(0) ) {
          // here we found the evidence corresponding to the variable
          eraseEvidence ( ev_pot );
          break;
        }
      }
      
      // insert the evidence
      __evidences.insert(pot);
      __clique_evidence[clique_id].insert( pot );
      if ( __isHardEvidence ( pot ) ) {
        __hard_evidence_nodes.insert ( var_id );
        __bn_node2hard_potential.insert ( var_id, pot );
      }
      else {
        __soft_evidence_nodes.insert ( var_id );
      }
      
      // indicate that, now, new inference is required
      __setRequiredInference(clique_id, clique_id);
    }

    // indicate that we shall recompute the set of barren nodes/potentials
    __need_recompute_barren_potentials = true;
  }

  
  /// sets how we determine the relevant potentials to combine
  template <typename GUM_SCALAR>
  INLINE void
  LazyPropagation<GUM_SCALAR>::setFindRelevantPotentialsType
  ( FindRelevantPotentialsType type ) {
    switch ( type ) {
    case FIND_RELEVANT_D_SEPARATION2:
      __findRelevantPotentials =
        & LazyPropagation<GUM_SCALAR>::__findRelevantPotentialsWithdSeparation2;
      break;
      
    case FIND_RELEVANT_D_SEPARATION:
      __findRelevantPotentials =
        & LazyPropagation<GUM_SCALAR>::__findRelevantPotentialsWithdSeparation;
      break;
      
    case FIND_RELEVANT_ALL:
      __findRelevantPotentials =
        & LazyPropagation<GUM_SCALAR>::__findRelevantPotentialsGetAll;
      break;

    default:
      GUM_ERROR ( InvalidArgument,
                  "setFindRelevantPotentialsType for this type not implemented yet" );
    }
  }

  
  // find the potentials d-connected to a set of variables
  template <typename GUM_SCALAR>
  INLINE void
  LazyPropagation<GUM_SCALAR>::__findRelevantPotentialsGetAll
  ( __PotentialSet& pot_list,
    Set<const DiscreteVariable *>& kept_vars ) {
  }


  // find the potentials d-connected to a set of variables
  template <typename GUM_SCALAR>
  INLINE void
  LazyPropagation<GUM_SCALAR>::__findRelevantPotentialsWithdSeparation
  ( __PotentialSet& pot_list,
    Set<const DiscreteVariable *>& kept_vars ) {
    // find the node ids of the kept variables
    NodeSet kept_ids;
    const auto& bn = this->bn();
    for ( const auto var : kept_vars ) {
      kept_ids.insert ( bn.nodeId ( *var ) );
    }
    
    // determine the set of potentials d-connected with the kept variables
    NodeSet requisite_nodes;
    BayesBall bb;
    bb.requisiteNodes ( this->bn().dag(), kept_ids,
                        __hard_evidence_nodes, __soft_evidence_nodes,
                        requisite_nodes );
    for ( auto iter = pot_list.beginSafe ();
          iter != pot_list.endSafe (); ++iter ) {
      const Sequence<const DiscreteVariable *>& vars =
        (**iter).variablesSequence();
      bool found = false;
      for ( auto var : vars ) {
        if ( requisite_nodes.exists ( bn.nodeId( *var ) ) ) {
          found = true;
          break;
        }
      }
      
      if ( ! found ) {
        pot_list.erase ( iter );
      }
    }
  }

  
  // find the potentials d-connected to a set of variables
  template <typename GUM_SCALAR>
  INLINE void
  LazyPropagation<GUM_SCALAR>::__findRelevantPotentialsWithdSeparation2
  ( __PotentialSet& pot_list,
    Set<const DiscreteVariable *>& kept_vars ) {
    // find the node ids of the kept variables
    NodeSet kept_ids;
    const auto& bn = this->bn();
    for ( const auto var : kept_vars ) {
      kept_ids.insert ( bn.nodeId ( *var ) );
    }
    
    // determine the set of potentials d-connected with the kept variables
    BayesBall bb;
    bb.relevantPotentials ( this->bn(), kept_ids,
                            __hard_evidence_nodes, __soft_evidence_nodes,
                            pot_list );
  }
  

  // remove barren variables
  template <typename GUM_SCALAR>
  INLINE void
  LazyPropagation<GUM_SCALAR>::__removeBarrenVariables
  ( __PotentialSet& pot_list,
    Set<const DiscreteVariable *>& del_vars ) {
    // remove from del_vars the variables that received some evidence:
    // only those that did not received evidence can be barren variables
    Set<const DiscreteVariable *> the_del_vars = del_vars;
    for ( auto iter = the_del_vars.beginSafe ();
          iter != the_del_vars.endSafe (); ++iter ) {
      NodeId id = this->bn().nodeId ( **iter );
      if ( __hard_evidence_nodes.exists ( id ) ||
           __soft_evidence_nodes.exists ( id ) ) {
        the_del_vars.erase ( iter );
      }
    }
    
    // assign to each random variable the set of potentials that contain it
    HashTable<const DiscreteVariable *, __PotentialSet> var2pots;
    __PotentialSet empty_pot_set;
    for ( const auto pot : pot_list ) {
      const Sequence<const DiscreteVariable *>& vars = pot->variablesSequence();
      for ( const auto var : vars ) {
        if ( the_del_vars.exists ( var ) ) {
          if ( ! var2pots.exists ( var ) ) {
            var2pots.insert ( var, empty_pot_set );
          }
          var2pots[var].insert ( pot );
        }
      }
    }

    // each variable with only one potential is a barren variable
    // assign to each potential with barren nodes its set of barren variables
    HashTable<const Potential<GUM_SCALAR>*, Set<const DiscreteVariable*> >
      pot2barren_var;
    Set<const DiscreteVariable*> empty_var_set;
    for ( auto elt : var2pots ) {
      if ( elt.second.size () == 1 ) { // here we have a barren variable
        const Potential<GUM_SCALAR>* pot = *( elt.second.begin () );
        if ( ! pot2barren_var.exists ( pot ) ) {
          pot2barren_var.insert ( pot, empty_var_set );
        }
        pot2barren_var[pot].insert ( elt.first ); // insert the barren variable
      }
    }

    // for each potential with barren variables, marginalize them.
    // if the potential has only barren variables, simply remove them from the
    // set of potentials, else just project the potential
    MultiDimProjection<GUM_SCALAR,Potential> projector ( LPNewprojPotential );
    for ( auto elt : pot2barren_var ) {
      // remove the current potential from pot list as, anyway, we will change it
      const Potential<GUM_SCALAR>* pot = elt.first;
      pot_list.erase ( pot );

      // check whether we need to add a projected new potential or not (i.e.,
      // whether there exist non-barren variables or not)
      if ( pot->variablesSequence().size () !=  elt.second.size () ) {
        auto new_pot = projector.project ( *pot, elt.second );
        pot_list.insert ( new_pot );
        __created_potentials.insert ( new_pot );
      }
    }
  }

  
  // remove variables del_vars from the list of potentials pot_list
  template <typename GUM_SCALAR>
  INLINE void LazyPropagation<GUM_SCALAR>::__marginalizeOut
  ( __PotentialSet& pot_list,
    Set<const DiscreteVariable *>& del_vars,
    Set<const DiscreteVariable *>& kept_vars ) {
    // use d-separation analysis to check which potentials shall be combined
    (this->*__findRelevantPotentials) ( pot_list, kept_vars );

    // remove the potentials corresponding to barren variables
    __removeBarrenVariables ( pot_list, del_vars );
    
    // create a combine and project operator that will perform the marginalization
    MultiDimCombineAndProjectDefault<GUM_SCALAR,Potential>
      combine_and_project (LPNewmultiPotential,LPNewprojPotential);
    __PotentialSet new_pot_list =
      combine_and_project.combineAndProject ( pot_list, del_vars );

    // determine which new potentials we have created and put them into the
    // set of potentials created during inference. In addition, remove all the
    // potentials that have no dimension
    for ( auto iter_pot = new_pot_list.beginSafe ();
          iter_pot != new_pot_list.endSafe (); ++iter_pot ) {
      if ( (*iter_pot)->variablesSequence ().size () == 0 ) {
        // as we have already marginalized out variables that received evidence,
        // it may be the case that, after combining and projecting, some
        // potentials might be empty. In this case, we shall remove them
        // from memory
        delete *iter_pot;
        new_pot_list.erase ( iter_pot );
      }
      else {
        if ( ! pot_list.exists ( *iter_pot ) ) {
          __created_potentials.insert( *iter_pot );
        }
      }
    }

    pot_list = new_pot_list;
  }


  // creates the message sent by clique from_id to clique to_id
  template <typename GUM_SCALAR>
  INLINE void LazyPropagation<GUM_SCALAR>::__produceMessage(NodeId from_id,
                                                            NodeId to_id) {
    // get the set of CPTs of the BN that are barren w.r.t. the message
    // we wish to produce
    const __PotentialSet& barren_pots =
      __barren_potentials[Arc ( from_id, to_id )];

    // get the potentials of the clique. For the potentials that contain a hard
    // evidence, marginalize them over the nodes that received hard evidence
    const List<const Potential<GUM_SCALAR> *>& clique_pot =
      __clique_potentials[from_id];
    __PotentialSet pot_list(clique_pot.size());
    for ( const auto &cli : clique_pot ) {
      if ( ! barren_pots.exists ( cli ) ) { // do not take into account barren CPTs
        // check whether a variable of cli received a hard evidence
        NodeSet hard_vars;
        Set< const DiscreteVariable * > del_vars;
        for ( const auto var : cli->variablesSequence () ) {
          const NodeId id = this->bn().nodeId ( *var );
          if ( __hard_evidence_nodes.exists ( id ) ) {
            hard_vars.insert ( id );
            del_vars.insert ( var );
          }
        }
        if ( hard_vars.empty () ) { // no hard evidence => no marginalization
          pot_list.insert( cli );
        }
        else { // marginalize out the hard evidence nodes
          // if all the variables are to be marginalized out, don't take into
          // account the CPT
          if ( hard_vars.size () != cli->variablesSequence ().size () ) {
            // get the set of potential to combine (the CPT of the clique + the
            // potentials of the hard evidence)
            __PotentialSet marg_pot_set { cli };
            for ( const auto var_id : hard_vars ) {
              marg_pot_set.insert ( __bn_node2hard_potential[var_id] );
            }

            // perform the combination of those potentials and their projection
            MultiDimCombineAndProjectDefault<GUM_SCALAR,Potential>
              combine_and_project (LPNewmultiPotential,LPNewprojPotential);
            __PotentialSet new_pot_list =
              combine_and_project.combineAndProject ( marg_pot_set, del_vars );

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
    const List<const Potential<GUM_SCALAR> *> &evidence_list =
      __clique_evidence[from_id];

    for ( const auto ev_pot : evidence_list ) {
      NodeId ev_node =
        this->bn().nodeId ( *( ev_pot->variablesSequence ().atPos(0) ) );
      if ( ! __hard_evidence_nodes.exists ( ev_node ) ) {
        pot_list.insert( ev_pot );
      }
    }

    // add the messages sent by adjacent nodes to from_id
    for (const auto other : __JT->neighbours(from_id))
      if (other != to_id)
        for (const auto pot : __sep_potentials[Arc(other, from_id)])
          pot_list.insert(pot);

    // get the set of variables that need be removed from the potentials
    const NodeSet &from_clique = __JT->clique(from_id);
    const NodeSet &separator = __JT->separator(from_id, to_id);
    Set<const DiscreteVariable *> del_vars(from_clique.size());
    Set<const DiscreteVariable *> kept_vars(from_clique.size());
    
    for (const auto node : from_clique) {
      if (!separator.contains(node)) {
        del_vars.insert(&(this->bn().variable(node)));
      }
      else {
        kept_vars.insert(&(this->bn().variable(node)));
      }
    }
            
    // pot_list now contains all the potentials to multiply and marginalize
    // => combine the messages
    __marginalizeOut(pot_list, del_vars, kept_vars );
    __sep_potentials[Arc(from_id, to_id)] = pot_list;
  }


  /// compute barren nodes if necessary
  template <typename GUM_SCALAR>
  INLINE void LazyPropagation<GUM_SCALAR>::__computeBarrenPotentials () {
    if ( __need_recompute_barren_potentials ) {
      BarrenNodesFinder finder ( this->bn().dag () );

      // insert the evidence into the finder
      finder.setEvidence ( __soft_evidence_nodes + __hard_evidence_nodes );

      // compute the barren potentials w.r.t. evidence
      __barren_potentials =
        std::move ( finder.barrenPotentials ( *__JT, this->bn() ) );
      
      __need_recompute_barren_potentials = false;
    }
  }
    
  
  // performs the __collect phase of Lazy Propagation
  template <typename GUM_SCALAR>
  INLINE void LazyPropagation<GUM_SCALAR>::__collect(NodeId id, NodeId from) {
    __collected_cliques[id] = true;

    for (const auto other : __JT->neighbours(id))
      if (other != from)
        __collect(other, id);

    if (id != from) {
      __produceMessage(id, from);
    }
  }
  

  // performs the __collect phase of Lazy Propagation
  template <typename GUM_SCALAR>
  INLINE void LazyPropagation<GUM_SCALAR>::collect(NodeId id, bool force_collect) {
    // get a clique that contains id
    NodeId clique = __node_to_clique[id];
    
    // check if we really need to perform an inference
    if (!force_collect && __collected_cliques[clique])
      return;

    // clean-up the area that will receive the __collect
    __setRequiredInference(clique, clique);

    // compute barren nodes if necessary
    __computeBarrenPotentials ();
    
    // perform the __collect
    __last_collect_clique = clique;

    __collect(clique, clique);
  }

  // performs the __diffusion phase of Lazy Propagation
  template <typename GUM_SCALAR>
  INLINE void LazyPropagation<GUM_SCALAR>::__diffusion(NodeId id, NodeId from) {
    __diffused_cliques[id] = true;

    // #### TODO: make a more efficient inference using a stack of
    // of partial computations (see GONZALES, Mellouli, Mourali (2007))
    for (const auto other : __JT->neighbours(id))
      if (other != from) {
        __produceMessage(id, other);
        __diffusion(other, id);
      }
  }

  // performs the __collect phase of Lazy Propagation
  template <typename GUM_SCALAR>
  INLINE void LazyPropagation<GUM_SCALAR>::diffusion(NodeId id,
                                                     bool force_diffusion) {
    // get a clique that contains id
    NodeId clique = __node_to_clique[id];

    // check if we really need to perform an inference
    if (force_diffusion) {
      __collect(clique, true);
      __diffusion(clique, clique);
    } else if (__diffused_cliques[clique])
      return;

    if (!__collected_cliques[clique])
      __collect(clique, false);

    __diffusion(clique, clique);
  }

  // performs a whole inference (__collect + __diffusion)

  template <typename GUM_SCALAR>
  INLINE void LazyPropagation<GUM_SCALAR>::makeInference() {
    makeInference(false);
  }

  // performs a whole inference (__collect + __diffusion)

  template <typename GUM_SCALAR>
  INLINE void LazyPropagation<GUM_SCALAR>::makeInference(bool force_inference) {
    // prepare a new inference from scratch

    if (force_inference) {
      // remove all the separator potentials, if any
      for (auto &elt : __sep_potentials)
        elt.second.clear();

      for (const auto pot : __created_potentials)
        delete pot;

      __created_potentials.clear();

      // indicate that __collect and __diffusion passed through no clique yet
      for (auto &elt : __collected_cliques) {
        elt.second = false;
      }

      for (auto &elt : __diffused_cliques) {
        elt.second = false;
      }
    }
    
    // compute barren nodes if necessary
    __computeBarrenPotentials ();

    // perform the __collect in all connected components of the junction tree
    for (const auto &elt : __collected_cliques)
      if (!elt.second)
        __collect(elt.first, elt.first);

    // perform the __diffusion in all connected components of the junction tree
    for (const auto &elt : __diffused_cliques)
      if (!elt.second)
        __diffusion(elt.first, elt.first);

    // indicate that we performed the inference with root =
    // __collected_cliques.begin()
    __last_collect_clique = __collected_cliques.begin().key();

    // ##### bug potentiel a virer : s'il y a plusieurs composantes connexes,
    // il faut plusieurs cliques de collecte
  }

  // returns the marginal a posteriori proba of a given node
  template <typename GUM_SCALAR>
  void LazyPropagation<GUM_SCALAR>::__aPosterioriMarginal(NodeId id, Potential<GUM_SCALAR> &marginal) {
    // if the node received a hard evidence, just return it
    if ( __hard_evidence_nodes.exists ( id ) ) {
      marginal = *( __bn_node2hard_potential[id] );
      return;
    }
    
    // compute barren nodes if necessary
    __computeBarrenPotentials ();

    // check if we performed a __collect on id, else we need some
    NodeId clique_of_id = __node_to_clique[id];

    if (!__collected_cliques[clique_of_id]) {
      __collect(clique_of_id, clique_of_id);
      __last_collect_clique = clique_of_id;
    }
        
    // ok, we performed a __collect, but maybe this __collect was not performed
    // from the clique containing id. In this case, we also need to perform
    // a __diffusion
    const NodeSet &clique_nodes = __JT->clique(__last_collect_clique);

    bool last_collect_clique_contains_id = true;

    if (!clique_nodes.contains(id)) {
      makeInference(false);
      last_collect_clique_contains_id = false;
    }

    // now we just need to create the product of the potentials of the clique
    // containing id with the messages received by this clique and
    // marginalize out all variables except id
    NodeId targetClique =
      last_collect_clique_contains_id ? __last_collect_clique : clique_of_id;

    const List<const Potential<GUM_SCALAR> *> &clique_pot =
      __clique_potentials[targetClique];

    // get the potentials of the clique
    __PotentialSet pot_list(clique_pot.size() +
                            __clique_evidence[targetClique].size());

    for (const auto &cli : clique_pot)
      pot_list.insert(cli);

    // add the evidence to the clique potentials
    const List<const Potential<GUM_SCALAR> *> &evidence_list =
      __clique_evidence[targetClique];
    for ( const auto ev_pot : evidence_list ) {
      pot_list.insert( ev_pot );
    }

    // if some node of the clique received a hard evidence, add this evidence
    // this is compulsory because, during the messages computations, we
    // marginalized out the hard evidence nodes from the CPTs prior to the
    // combinations. Hence, we must marginalize out these nodes on all tables
    // that contain them
    for ( const auto node : __JT->clique ( targetClique ) ) {
      if ( __bn_node2hard_potential.exists ( node ) ) {
        const auto ev_pot = __bn_node2hard_potential[node];
        if ( ! pot_list.exists ( ev_pot ) ) {
          pot_list.insert( ev_pot );
        }
      }
    }

    // add the messages sent by adjacent nodes to targetClique
    for (const auto other : __JT->neighbours(targetClique))
      for (const auto pot : __sep_potentials[Arc(other, targetClique)])
        pot_list.insert(pot);

    // get the set of variables that need be removed from the potentials
    const NodeSet &nodes = __JT->clique(targetClique);
    Set<const DiscreteVariable *> del_vars(nodes.size());
    Set<const DiscreteVariable *> kept_vars(nodes.size());

    for (const auto node : nodes) {
      if (node != id) {
        del_vars.insert(&(this->bn().variable(node)));
      }
      else {
        kept_vars.insert(&(this->bn().variable(node)));
      }
    }
    // pot_list now contains all the potentials to multiply and marginalize
    // => combine the messages
    __marginalizeOut(pot_list, del_vars, kept_vars );

    if (pot_list.size() == 1) {
      marginal = **pot_list.begin();
    }
    else {
      Set<const Potential<GUM_SCALAR> *> set;
      for (const auto pot : pot_list)
        set << pot;

      MultiDimCombinationDefault<GUM_SCALAR, Potential>
        fast_combination( LPNewmultiPotential );

      fast_combination.combine(marginal, set);
    }
  }

  
  // returns the joint a posteriori proba of a given set of nodes
  template <typename GUM_SCALAR>
  void
  LazyPropagation<GUM_SCALAR>::__aPosterioriJoint(const NodeSet &ids,
                                                  Potential<GUM_SCALAR> &marginal) {
    // compute barren nodes if necessary
    __computeBarrenPotentials ();
    
    // find a clique that contains all the nodes in ids. To do so, we loop over
    // all the cliques and check wheither there exists one with this feature
    NodeId clique_of_ids = 0;
    bool clique_found = false;

    for (const auto node : __JT->nodes()) {
      // get the nodes contained in the clique
      const NodeSet &clique = __JT->clique(node);
      // check whether the clique actually contains all of ids
      bool clique_ok = true;

      for (const auto node2 : ids) {
        if (!clique.contains(node2)) {
          clique_ok = false;
          break;
        }
      }

      // check if we found the clique we wanted
      if (clique_ok) {
        clique_of_ids = node;
        clique_found = true;
        break;
      }
    }

    // check if we actually found the clique we were interested in
    if (!clique_found) {
      GUM_ERROR(OperationNotAllowed,
                "no clique was found to compute the joint probability");
    }

    // for the moment, always perform an inference before computing the
    // joint a posteriori distribution
    makeInference(true);

    // now we just need to create the product of the potentials of the clique
    // containing id with the messages received by this clique and
    // marginalize out all variables except id
    const List<const Potential<GUM_SCALAR> *> &clique_pot =
      __clique_potentials[clique_of_ids];

    // get the potentials of the clique
    __PotentialSet pot_list(clique_pot.size() +
                            __clique_evidence[clique_of_ids].size());

    for (ListConstIteratorSafe<const Potential<GUM_SCALAR> *> iter =
           clique_pot.cbeginSafe();
         iter != clique_pot.cendSafe(); ++iter) {
      pot_list.insert(*iter);
    }

    // add the evidence to the clique potentials
    const List<const Potential<GUM_SCALAR> *> &evidence_list =
      __clique_evidence[clique_of_ids];

    for (ListConstIteratorSafe<const Potential<GUM_SCALAR> *> iter =
           evidence_list.cbeginSafe();
         iter != evidence_list.cendSafe(); ++iter) {
      pot_list.insert(*iter);
    }

    // if some node of the clique received a hard evidence, add this evidence
    // this is compulsory because, during the messages computations, we
    // marginalized out the hard evidence nodes from the CPTs prior to the
    // combinations. Hence, we must marginalize out these nodes on all tables
    // that contain them
    for ( const auto node : __JT->clique ( clique_of_ids ) ) {
      if ( __bn_node2hard_potential.exists ( node ) ) {
        const auto ev_pot = __bn_node2hard_potential[node];
        if ( ! pot_list.exists ( ev_pot ) ) {
          pot_list.insert( ev_pot );
        }
      }
    }


    // add the messages sent by adjacent nodes to clique_of_ids
    for (const auto other : __JT->neighbours(clique_of_ids))
      for (const auto pot : __sep_potentials[Arc(other, clique_of_ids)]) {
        pot_list.insert(pot);
      }

    // get the set of variables that need be removed from the potentials
    const NodeSet &nodes = __JT->clique(clique_of_ids);

    Set<const DiscreteVariable *> del_vars(nodes.size());
    Set<const DiscreteVariable *> kept_vars(nodes.size());

    for (const auto node : nodes) {
      if (!ids.contains(node)) {
        del_vars.insert(&(this->bn().variable(node)));
      }
      else {
        kept_vars.insert(&(this->bn().variable(node)));
      }
    }
        
    // ok, now, pot_list contains all the potentials to multiply and marginalize
    // => now, combine the messages
    __marginalizeOut(pot_list, del_vars, kept_vars );

    if (pot_list.size() == 1) {
      marginal = **pot_list.begin();
    } else {
      Set<const Potential<GUM_SCALAR> *> set;

      for (const auto pot : pot_list)
        set << pot;

      MultiDimCombinationDefault<GUM_SCALAR, Potential> fast_combination(
                                                                         LPNewmultiPotential);

      fast_combination.combine(marginal, set);
    }
  }

  template <typename GUM_SCALAR>
  INLINE const JunctionTree *LazyPropagation<GUM_SCALAR>::junctionTree() const {
    return __JT;
  }

  // returns the marginal a posteriori proba of a given node

  template <typename GUM_SCALAR>
  INLINE void
  LazyPropagation<GUM_SCALAR>::_fillPosterior(NodeId id,
                                              Potential<GUM_SCALAR> &posterior) {
    __aPosterioriMarginal(id, posterior);
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
    Potential<GUM_SCALAR> *tmp = new Potential<GUM_SCALAR>();
    Id id = __node_to_clique.begin().key();
    __aPosterioriMarginal(id, *tmp);

    GUM_SCALAR sum = 0;
    Instantiation iter(*tmp);

    for (iter.setFirst(); !iter.end(); ++iter)
      sum += tmp->get(iter);

    delete tmp;

    return sum;
  }

  // returns the joint a posteriori probability P(nodes|e)
  /** @warning right now, method joint cannot compute joint a posteriori
   * probabilities of every nodeset. In cases where it is not able to perform
   * properly this task, it will raise a OperationNotAllowed exception. */

  template <typename GUM_SCALAR>
  INLINE Potential<GUM_SCALAR> *
  LazyPropagation<GUM_SCALAR>::joint(const NodeSet &nodes) {
    Potential<GUM_SCALAR> *res = new Potential<GUM_SCALAR>();

    try {
      __aPosterioriJoint(nodes, *res);
      res->normalize();
    } catch (OperationNotAllowed &e) {
      delete (res);
      throw;
    }

    return res;
  }

  /** Entropy
   * Compute Shanon's entropy of a node given the observation
   */
  template <typename GUM_SCALAR>
  GUM_SCALAR LazyPropagation<GUM_SCALAR>::H(NodeId X) {
    const Potential<GUM_SCALAR> &pX = this->posterior(X);

    Instantiation i(pX);
    GUM_SCALAR res = (GUM_SCALAR)0;

    for (i.setFirst(); !i.end(); ++i) {
      GUM_SCALAR a = pX[i];

      if (a > (GUM_SCALAR)0) {
        res += a * log2(a);
      }
    }

    return -res;
  }

  /** Mutual information between X and Y
   * @see http://en.wikipedia.org/wiki/Mutual_information
   *
   * @warning Due to limitation of @joint, may not be able to compute this value
   * @throw OperationNotAllowed in these cases
   */
  template <typename GUM_SCALAR>
  GUM_SCALAR LazyPropagation<GUM_SCALAR>::I(NodeId X, NodeId Y) {
    const Potential<GUM_SCALAR> &pX = this->posterior(X);
    const Potential<GUM_SCALAR> &pY = this->posterior(Y);

    NodeSet XY;
    XY << X << Y;
    Potential<GUM_SCALAR> &pXY = *(joint(XY));

    Instantiation i(pXY);
    GUM_SCALAR res = (GUM_SCALAR)0;

    for (i.setFirst(); !i.end(); ++i) {
      GUM_SCALAR vXY = pXY[i];
      GUM_SCALAR vX = pX[i];
      GUM_SCALAR vY = pY[i];

      if (vXY > (GUM_SCALAR)0) {
        if (vX == (GUM_SCALAR)0 || vY == (GUM_SCALAR)0) {
          GUM_ERROR(OperationNotAllowed,
                    "Mutual Information (X,Y) with P(X)=0 or P(Y)=0 and P(X,Y)>0");
        }

        res += vXY * (log2(vXY) - log2(vX) - log2(vY));
      }
    }

    delete (&pXY);

    return res;
  }

  /** Variation of information between X and Y
   * @see http://en.wikipedia.org/wiki/Variation_of_information
   *
   * @warning Due to limitation of @joint, may not be able to compute this value
   * @throw OperationNotAllowed in these cases
   */
  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR LazyPropagation<GUM_SCALAR>::VI(NodeId X, NodeId Y) {
    return H(X) + H(Y) - 2 * I(X, Y);
  }
} /* namespace gum */

#endif // DOXYGEN_SHOULD_SKIP_THIS
// kate: indent-mode cstyle; indent-width 2; replace-tabs on;
