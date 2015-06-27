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
#include <agrum/BN/inference/lazyPropagationOld.h>
#include <agrum/multidim/operators/multiDimCombinationDefault.h>
#include <agrum/graphs/binaryJoinTreeConverterDefault.h>
#include <agrum/graphs/triangulations/orderedTriangulation.h>

// to ease IDE parsers
#include <agrum/BN/inference/BayesNetInference.h>

namespace gum {

  // the function used to combine two tables
  template <typename GUM_SCALAR>
  INLINE static Potential<GUM_SCALAR> *
  multiPotential(const Potential<GUM_SCALAR> &t1, const Potential<GUM_SCALAR> &t2) {
    return new Potential<GUM_SCALAR>(t1 * t2);
  }

  // the function used to combine two tables
  template <typename GUM_SCALAR>
  INLINE static Potential<GUM_SCALAR> *
  projPotential(const Potential<GUM_SCALAR> &t1,
                const Set<const DiscreteVariable *> &del_vars) {
    return new Potential<GUM_SCALAR>(projectSum(t1, del_vars));
  }

  // initialization function
  template <typename GUM_SCALAR>
  INLINE void
  LazyPropagationOld<GUM_SCALAR>::__initialize(const IBayesNet<GUM_SCALAR> &BN,
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

    for (const auto node : dag.nodes()) {
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
      __node_to_clique.insert(
                              node, triangulation.createdJunctionTreeClique(first_eliminated_node));
    }

    // create empty potential lists into the cliques of the joint tree as well
    // as empty lists of evidence
    List<const Potential<GUM_SCALAR> *> empty_list;

    for (const auto node : __JT->nodes()) {
      __clique_potentials.insert(node, empty_list);
      __clique_evidence.insert(node, empty_list);
    }

    // put all the CPT's of the Bayes net nodes into the cliques
    for (const auto node : dag.nodes()) {
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
  LazyPropagationOld<GUM_SCALAR>::LazyPropagationOld(const IBayesNet<GUM_SCALAR> &BN)
    : BayesNetInference<GUM_SCALAR>(BN) {
    // for debugging purposessetRequiredInference
    GUM_CONSTRUCTOR(LazyPropagationOld);

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
  LazyPropagationOld<GUM_SCALAR>::LazyPropagationOld(const IBayesNet<GUM_SCALAR> &BN,
                                                     const std::vector<NodeId> &elim_order)
    : BayesNetInference<GUM_SCALAR>(BN) {
    // for debugging purposessetRequiredInference
    GUM_CONSTRUCTOR(LazyPropagationOld);

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
  INLINE LazyPropagationOld<GUM_SCALAR>::~LazyPropagationOld() {
    // for debugging purposes
    GUM_DESTRUCTOR(LazyPropagationOld);
    // remove all the created potentials

    for (const auto pot : __created_potentials)
      delete (pot);

    __created_potentials.clear();

    delete __JT;
  }

  // indicates that we need inference in a given Junction tree connected component
  template <typename GUM_SCALAR>
  INLINE void LazyPropagationOld<GUM_SCALAR>::__setRequiredInference(NodeId id,
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
  LazyPropagationOld<GUM_SCALAR>::eraseEvidence(const Potential<GUM_SCALAR> *pot) {
    this->_invalidatePosteriors();
    // if the evidence does not exist, do nothing

    if (!__evidences.contains(pot))
      return;

    // remove the potential from the list of evidence of the cliques
    // @todo : elle n'est pas que dans une seule clique ?
    for (auto &elt : __clique_evidence) {
      elt.second.eraseByVal(pot);
    }

    // remove the potential from the list of evidence
    __evidences.erase(pot);

    // indicate that we need to perform both __collect and __diffusion in the
    // connected component containing the variable of the evidence
    const Sequence<const DiscreteVariable *> &vars = pot->variablesSequence();

    NodeId pot_clique = __node_to_clique[this->bn().nodeId(*vars.atPos(0))];

    __setRequiredInference(pot_clique, pot_clique);
  }

  // remove all evidence from the graph

  template <typename GUM_SCALAR>
  INLINE void LazyPropagationOld<GUM_SCALAR>::eraseAllEvidence() {
    this->_invalidatePosteriors();
    // remove the evidence store in the cliques

    for (auto &elt : __clique_evidence)
      elt.second.clear();

    // remove the messages sent during a previous propagation
    for (auto &elt : __sep_potentials)
      elt.second.clear();

    // remove actually all the evidence taken into account
    __evidences.clear();

    // remove from memory all the created potentials
    for (const auto pot : __created_potentials)
      delete pot;

    __created_potentials.clear();

    for (auto &elt : __collected_cliques)
      elt.second = false;

    for (auto &elt : __diffused_cliques)
      elt.second = false;
  }

  // insert new evidence in the graph

  template <typename GUM_SCALAR>
  INLINE void LazyPropagationOld<GUM_SCALAR>::insertEvidence(
                                                             const List<const Potential<GUM_SCALAR> *> &pot_list) {
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

      // remove already existing evidence w.r.t. variable in pot
      const DiscreteVariable *var = vars.atPos(0);
      NodeId var_id = this->bn().nodeId(*var);

      for (__PotentialSetIterator iter2 =
             __evidences.beginSafe(); // safe iterator needed here
           iter2 != __evidences.endSafe();
           ++iter2) {
        if (var == (*iter2)->variablesSequence().atPos(0)) {
          eraseEvidence(*iter2); // erase in _evidences => safe iterator needed here
          break;
        }
      }

      // insert the evidence
      __evidences.insert(pot);

      NodeId clique_id = __node_to_clique[var_id];
      __clique_evidence[clique_id].insert(pot);

      // indicate that, now, new inference is required
      __setRequiredInference(clique_id, clique_id);
    }
  }

  // remove variables del_vars from the list of potentials pot_list

  template <typename GUM_SCALAR>
  INLINE void LazyPropagationOld<GUM_SCALAR>::__marginalizeOut(
                                                               __PotentialSet &pot_list, Set<const DiscreteVariable *> &del_vars) {
    // when we remove a variable, we need to combine all the tables containing this
    // variable in order to produce a new unique table containing this variable.
    // removing the variable is then performed by marginalizing it out of the
    // table. In the marginalizeOut algorithm, we wish to remove first variables that
    // produce small tables. This should speed up the marginalizing process
    // the sizes of the tables produced when removing a given discrete variable
    PriorityQueue<const DiscreteVariable *, double> product_size;
    // the potentials containing a given variable
    HashTable<const DiscreteVariable *, List<const Potential<GUM_SCALAR> *>>
      pot_per_var;
    // for a given variable X to be deleted, the list of all the variables of
    // the potentials containing X (actually, we count the number of potentials
    // containing the variable. This is more efficient for computing and updating
    // the product_size priority queue when some potentials are removed)
    HashTable<const DiscreteVariable *,
              HashTable<const DiscreteVariable *, unsigned int>> pot_vars_per_var;
    // initialize pot_vars_per_var and pot_per_var
    List<const Potential<GUM_SCALAR> *> empty_list;
    HashTable<const DiscreteVariable *, unsigned int> empty_hash(
                                                                 16); // @todo why 16 ?

    for (const auto var : del_vars) {
      pot_per_var.insert(var, empty_list);
      pot_vars_per_var.insert(var, empty_hash);
    }

    // update properly pot_per_var and pot_vars_per_var
    for (const auto pot : pot_list) {
      const Sequence<const DiscreteVariable *> &vars = pot->variablesSequence();

      for (unsigned int i = 0; i < vars.size(); ++i) {
        if (del_vars.contains(vars[i])) {
          // add the potential to the set of potentials related to vars[i]
          pot_per_var[vars[i]].insert(pot);
          // add the variables of the potential to pot_vars_per_var[vars[i]]
          HashTable<const DiscreteVariable *, unsigned int> &iter_vars =
            pot_vars_per_var[vars[i]];

          for (unsigned int j = 0; j < vars.size(); ++j) {
            try {
              ++iter_vars[vars[j]];
            } catch (const NotFound &) {
              iter_vars.insert(vars[j], 1);
            }
          }
        }
      }
    }

    // initialize properly product_size
    for (const auto &elt : pot_vars_per_var) {
      double size = 1.0;

      if (elt.second.size()) {
        for (const auto var : elt.second)
          size *= var.first->domainSize();

        product_size.insert(elt.first, size);
      }
    }

    // create a hashtable of the temporary potentials created during the
    // marginalization process
    __PotentialSet tmp_marginals(30); //@todo why 30 ????

    // now, remove all the variables in del_vars, starting from those that produce
    // the smallest tables
    while (!product_size.empty()) {
      // get the best variable to remove
      const DiscreteVariable *del_var = product_size.pop();
      del_vars.erase(del_var);
      // get the list of potentials to multiply
      List<const Potential<GUM_SCALAR> *> &pot_to_mult = pot_per_var[del_var];
      // if there is no poential to multiply, do nothing

      if (pot_to_mult.size() == 0)
        continue;

      // compute the product of all the potentials
      Potential<GUM_SCALAR> *joint;

      bool joint_to_delete = false;

      if (pot_to_mult.size() == 1) {
        joint = const_cast<Potential<GUM_SCALAR> *>(pot_to_mult[0]);
        joint_to_delete = false;
      } else {
        if (pot_to_mult.size() == 1) {
          joint = const_cast<Potential<GUM_SCALAR> *>(*(pot_to_mult.begin()));
          joint_to_delete = false;
        } else {
          Set<const Potential<GUM_SCALAR> *> set;

          for (const auto pot : pot_to_mult) {
            set << pot;
          }

          MultiDimCombinationDefault<GUM_SCALAR, Potential>

            fast_combination(multiPotential);
          joint = fast_combination.combine(set);
          joint_to_delete = true;
        }
      }

      // compute the table resulting from marginalizing out del_var from joint
      Set<const DiscreteVariable *> del_one_var;

      del_one_var << del_var;

      Potential<GUM_SCALAR> *posterior = projPotential(*joint, del_one_var);

      // new Potential<GUM_SCALAR>( new MultiDimArray<GUM_SCALAR>() );

      //   // const Sequence<const DiscreteVariable*>& joint_vars =
      //   // joint->variablesSequence();
      // for ( unsigned int i = 0; i < joint_vars.size(); ++i ) {
      //   if ( joint_vars[i] != del_var )
      //     posterior->add( *joint_vars[i] );
      // }

      // posterior->marginalize( *joint );

      if (joint_to_delete)
        delete joint;

      // update pot_vars_per_var : remove the variables of the potential we
      // multiplied from this table
      // update accordingly pot_per_vars : remove these potentials
      // update accordingly product_size : when a variable is no more used by
      // any potential, divide product_size by its domain size
      for (const auto pot : pot_to_mult) {
        const Sequence<const DiscreteVariable *> &pot_vars =
          pot->variablesSequence();

        for (Idx i = 0; i < pot_vars.size(); ++i) {
          if (del_vars.contains(pot_vars[i])) {
            // ok, here we have a variable that needed to be removed => update
            // product_size, pot_per_var and pot_vars_per_var
            HashTable<const DiscreteVariable *, unsigned int> &pot_vars_of_var_i =
              pot_vars_per_var[pot_vars[i]];
            double div_size = 1;

            for (Idx j = 0; j < pot_vars.size(); ++j) {
              Idx k = --pot_vars_of_var_i[pot_vars[j]];

              if (k == 0) {
                div_size *= pot_vars[j]->domainSize();
                pot_vars_of_var_i.erase(pot_vars[j]);
              }
            }

            pot_per_var[pot_vars[i]].eraseByVal(pot);

            if (div_size != 1) {
              product_size.setPriority(
                                       pot_vars[i], product_size.priority(pot_vars[i]) / div_size);
            }
          }
        }

        if (tmp_marginals.contains(pot)) {
          delete pot;
          tmp_marginals.erase(pot);
        }

        pot_list.erase(pot);
      }

      pot_per_var.erase(del_var);

      // add the new posterior to the list of potentials
      const Sequence<const DiscreteVariable *> &marginal_vars =
        posterior->variablesSequence();

      for (unsigned int i = 0; i < marginal_vars.size(); ++i) {
        if (del_vars.contains(marginal_vars[i])) {
          // add the new marginal potential to the ser of potentials of var i
          pot_per_var[marginal_vars[i]].insert(posterior);
          // add the variables of the potential to pot_vars_per_var[vars[i]]
          HashTable<const DiscreteVariable *, unsigned int> &iter_vars =
            pot_vars_per_var[marginal_vars[i]];
          double mult_size = 1;

          for (unsigned int j = 0; j < marginal_vars.size(); ++j) {
            try {
              ++iter_vars[marginal_vars[j]];
            } catch (const NotFound &) {
              iter_vars.insert(marginal_vars[j], 1);
              mult_size *= marginal_vars[j]->domainSize();
            }
          }

          if (mult_size != 1) {
            product_size.setPriority(marginal_vars[i],
                                     product_size.priority(marginal_vars[i]) *
                                     mult_size);
          }
        }
      }

      pot_list.insert(posterior);

      tmp_marginals.insert(posterior);
    }

    // add to the list of potentials created during propagation the set of marginals
    // that appear in the final list of potentials returned after marginalization
    for (const auto pot : tmp_marginals)
      __created_potentials.insert(pot);
  }

  // creates the message sent by clique from_id to clique to_id

  template <typename GUM_SCALAR>
  INLINE void LazyPropagationOld<GUM_SCALAR>::__produceMessage(NodeId from_id,
                                                               NodeId to_id) {
    // get the potentials of the clique
    const List<const Potential<GUM_SCALAR> *> &clique_pot =
      __clique_potentials[from_id];
    __PotentialSet pot_list(clique_pot.size());

    for (const auto &cli : clique_pot)
      pot_list.insert(cli);

    // add the evidence to the clique potentials
    const List<const Potential<GUM_SCALAR> *> &evidence_list =
      __clique_evidence[from_id];

    for (ListConstIteratorSafe<const Potential<GUM_SCALAR> *> iter =
           evidence_list.cbeginSafe();
         iter != evidence_list.cendSafe(); ++iter)
      pot_list.insert(*iter);

    // add the messages sent by adjacent nodes to from_id
    for (const auto other : __JT->neighbours(from_id))
      if (other != to_id)
        for (const auto pot : __sep_potentials[Arc(other, from_id)])
          pot_list.insert(pot);

    // get the set of variables that need be removed from the potentials
    const NodeSet &from_clique = __JT->clique(from_id);
    const NodeSet &separator = __JT->separator(from_id, to_id);
    Set<const DiscreteVariable *> del_vars(from_clique.size());

    for (const auto node : from_clique)
      if (!separator.contains(node))
        del_vars.insert(&(this->bn().variable(node)));

    // pot_list now contains all the potentials to multiply and marginalize
    // => combine the messages
    __marginalizeOut(pot_list, del_vars);
    __sep_potentials[Arc(from_id, to_id)] = pot_list;
  }

  // performs the __collect phase of Lazy PropagationOld

  template <typename GUM_SCALAR>
  INLINE void LazyPropagationOld<GUM_SCALAR>::__collect(NodeId id, NodeId from) {
    __collected_cliques[id] = true;

    for (const auto other : __JT->neighbours(id))
      if (other != from)
        __collect(other, id);

    if (id != from) {
      __produceMessage(id, from);
    }
  }

  // performs the __collect phase of Lazy PropagationOld
  template <typename GUM_SCALAR>
  INLINE void LazyPropagationOld<GUM_SCALAR>::collect(NodeId id, bool force_collect) {
    // get a clique that contains id
    NodeId clique = __node_to_clique[id];
    // check if we really need to perform an inference

    if (!force_collect && __collected_cliques[clique])
      return;

    // clean-up the area that will receive the __collect
    __setRequiredInference(clique, clique);

    // perform the __collect
    __last_collect_clique = clique;

    __collect(clique, clique);
  }

  // performs the __diffusion phase of Lazy PropagationOld
  template <typename GUM_SCALAR>
  INLINE void LazyPropagationOld<GUM_SCALAR>::__diffusion(NodeId id, NodeId from) {
    __diffused_cliques[id] = true;

    // #### TODO: make a more efficient inference using a stack of
    // of partial computations (see GONZALES, Mellouli, Mourali (2007))
    for (const auto other : __JT->neighbours(id))
      if (other != from) {
        __produceMessage(id, other);
        __diffusion(other, id);
      }
  }

  // performs the __collect phase of Lazy PropagationOld
  template <typename GUM_SCALAR>
  INLINE void LazyPropagationOld<GUM_SCALAR>::diffusion(NodeId id,
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
  INLINE void LazyPropagationOld<GUM_SCALAR>::makeInference() {
    makeInference(false);
  }

  // performs a whole inference (__collect + __diffusion)

  template <typename GUM_SCALAR>
  INLINE void LazyPropagationOld<GUM_SCALAR>::makeInference(bool force_inference) {
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
  }

  // returns the marginal a posteriori proba of a given node
  template <typename GUM_SCALAR>
  void LazyPropagationOld<GUM_SCALAR>::__aPosterioriMarginal(
                                                             NodeId id, Potential<GUM_SCALAR> &marginal) {
    // check if we performed a __collect on id, else we need some
    NodeId clique_of_id = __node_to_clique[id];

    if (!__collected_cliques[clique_of_id])
      __collect(clique_of_id, clique_of_id);

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

    for ( const auto ev : evidence_list ) {
      pot_list.insert( ev );
    }

    // add the messages sent by adjacent nodes to targetClique
    for (const auto other : __JT->neighbours(targetClique))
      for (const auto pot : __sep_potentials[Arc(other, targetClique)])
        pot_list.insert(pot);

    // get the set of variables that need be removed from the potentials
    const NodeSet &nodes = __JT->clique(targetClique);
    Set<const DiscreteVariable *> del_vars(nodes.size());

    for (const auto node : nodes)
      if (node != id)
        del_vars.insert(&(this->bn().variable(node)));

    // pot_list now contains all the potentials to multiply and marginalize
    // => combine the messages
    __marginalizeOut(pot_list, del_vars);

    if (pot_list.size() == 1) {
      marginal = **pot_list.begin();
    }
    else {
      Set<const Potential<GUM_SCALAR> *> set;

      for (const auto pot : pot_list)
        set << pot;

      MultiDimCombinationDefault<GUM_SCALAR, Potential> fast_combination(
                                                                         multiPotential);

      fast_combination.combine(marginal, set);
    }
  }

  // returns the joint a posteriori proba of a given set of nodes
  template <typename GUM_SCALAR>
  void
  LazyPropagationOld<GUM_SCALAR>::__aPosterioriJoint(const NodeSet &ids,
                                                     Potential<GUM_SCALAR> &marginal) {
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

    // add the messages sent by adjacent nodes to clique_of_ids
    for (const auto other : __JT->neighbours(clique_of_ids))
      for (const auto pot : __sep_potentials[Arc(other, clique_of_ids)]) {
        pot_list.insert(pot);
      }

    // get the set of variables that need be removed from the potentials
    const NodeSet &nodes = __JT->clique(clique_of_ids);

    Set<const DiscreteVariable *> del_vars(nodes.size());

    for (const auto node : nodes)
      if (!ids.contains(node))
        del_vars.insert(&(this->bn().variable(node)));

    // ok, now, pot_list contains all the potentials to multiply and marginalize
    // => now, combine the messages
    __marginalizeOut(pot_list, del_vars);

    if (pot_list.size() == 1) {
      marginal = **pot_list.begin();
    } else {
      Set<const Potential<GUM_SCALAR> *> set;

      for (const auto pot : pot_list)
        set << pot;

      MultiDimCombinationDefault<GUM_SCALAR, Potential> fast_combination(
                                                                         multiPotential);

      fast_combination.combine(marginal, set);
    }
  }

  template <typename GUM_SCALAR>
  INLINE const JunctionTree *LazyPropagationOld<GUM_SCALAR>::junctionTree() const {
    return __JT;
  }

  // returns the marginal a posteriori proba of a given node

  template <typename GUM_SCALAR>
  INLINE void
  LazyPropagationOld<GUM_SCALAR>::_fillPosterior(NodeId id,
                                                 Potential<GUM_SCALAR> &posterior) {
    __aPosterioriMarginal(id, posterior);
    posterior.normalize();
  }

  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR LazyPropagationOld<GUM_SCALAR>::evidenceProbability() {
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
  LazyPropagationOld<GUM_SCALAR>::joint(const NodeSet &nodes) {
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
  GUM_SCALAR LazyPropagationOld<GUM_SCALAR>::H(NodeId X) {
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
  GUM_SCALAR LazyPropagationOld<GUM_SCALAR>::I(NodeId X, NodeId Y) {
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
  INLINE GUM_SCALAR LazyPropagationOld<GUM_SCALAR>::VI(NodeId X, NodeId Y) {
    return H(X) + H(Y) - 2 * I(X, Y);
  }
} /* namespace gum */

#endif // DOXYGEN_SHOULD_SKIP_THIS
// kate: indent-mode cstyle; indent-width 2; replace-tabs on;
