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
 * @brief Implementation of Gibbs sampling for inference in Bayesian Networks.
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <vector>

// to ease parsing for IDE
#include <agrum/BN/particles/Gibbs.h>

#define DEFAULT_DRAWN 10

namespace gum {

  namespace particle {

    template <typename GUM_SCALAR>
    Gibbs<GUM_SCALAR>::Gibbs(const IBayesNet<GUM_SCALAR> &BN)
        : __nbr_drawn_by_sample(DEFAULT_DRAWN), __bayesNet(BN) {
      GUM_CONSTRUCTOR(Gibbs);

      BN.completeInstantiation(__particle);

      // set the correspondance between variables
      const DAG &dag = this->bn().dag();
      //    const NodeSet& nodes = dag.nodes();

      for (auto node : dag.nodes()) {
        const DiscreteVariable &var = this->bn().variable(node);

        // feed the __sampling
        Potential<GUM_SCALAR> *newPot = new Potential<GUM_SCALAR>();
        __sampling_posterior.insert(node, newPot);
        (*newPot) << var;
        Instantiation *new_idx = new Instantiation(*newPot);
        __sampling_idx.insert(node, new_idx);
        // feed the children
        std::vector<NodeId> *newChildren = new std::vector<NodeId>();

        // const NodeSet& arcs = dag.children ( node );

        for (auto chil : dag.children(node)) {
          newChildren->push_back(chil);
        }

        __node_children.insert(node, newChildren);

        // feed the instantiation for each cpt
        Instantiation *newInst = new Instantiation(this->bn().cpt(node));
        __cpt_idx.insert(node, newInst);
      }
    }

    template <typename GUM_SCALAR> Gibbs<GUM_SCALAR>::~Gibbs() {
      GUM_DESTRUCTOR(Gibbs);

      for (auto &elt : __sampling_idx)
        delete (elt.second);

      for (auto &elt : __sampling_posterior)
        delete (elt.second);

      for (auto &elt : __node_children)
        delete (elt.second);

      for (auto &elt : __cpt_idx)
        delete (elt.second);
    }

    template <typename GUM_SCALAR>
    INLINE const IBayesNet<GUM_SCALAR> &Gibbs<GUM_SCALAR>::bn(void) {
      return __bayesNet;
    }

    template <typename GUM_SCALAR>
    INLINE void Gibbs<GUM_SCALAR>::__setValVar(NodeId id, Idx choice) {
      const DiscreteVariable &v = this->bn().variable(id);
      // the change is directly in __particle in order to use the new
      // drawn values for future draws
      __particle.chgVal(v, choice);
      // propagate the change in each concerned __cpt_idx
      // the first : CPT of node id
      __cpt_idx[id]->chgVal(v, choice);
      // the others : CPTs of its children

      for (std::vector<NodeId>::iterator iter = __node_children[id]->begin();
           iter != __node_children[id]->end(); iter++) {
        __cpt_idx[*iter]->chgVal(v, choice);
      }
    }

    /// how many sample between 2 stop test
    template <typename GUM_SCALAR>
    INLINE void Gibbs<GUM_SCALAR>::setNbrDrawnBySample(Size s) {
      __nbr_drawn_by_sample = s;
    }

    /// how many sample between 2 stop test
    template <typename GUM_SCALAR>
    INLINE Size Gibbs<GUM_SCALAR>::nbrDrawnBySample() {
      return __nbr_drawn_by_sample;
    }

    // give the actual particle
    template <typename GUM_SCALAR>
    inline const Instantiation &Gibbs<GUM_SCALAR>::particle(void) {
      return __particle;
    }

    /// put in __next_sample a value for variable id,
    /// @warning : proba is a probability for variable id
    template <typename GUM_SCALAR> void Gibbs<GUM_SCALAR>::__drawVar(NodeId id) {
      const DiscreteVariable &v = this->bn().variable(id);
      Potential<GUM_SCALAR> &proba = *__sampling_posterior[id];
      GUM_SCALAR p = (GUM_SCALAR)randomProba();
      // use of __sampling_idx for shrink the number of temporary Instantiation
      Instantiation &I = *__sampling_idx[id];
      Idx choice = 0;
      // normalisation
      GUM_SCALAR s = (GUM_SCALAR)0;

      for (I.setFirst(); !I.end(); I.inc())
        s += proba[I];

      if (s == (GUM_SCALAR)0) {
        GUM_ERROR(FatalError, "A normalisation factor is 0 in node "
                                  << id << " (" << v << ") : " << proba);
      }

      // draw value
      for (I.setFirst(); !I.end(); I.inc()) {
        if (proba[I] == (GUM_SCALAR)0)
          continue;

        choice = I.val(v);

        GUM_SCALAR q = proba[I] / s;

        if (p <= q) {
          break;
        }

        p -= q;
      }

      __setValVar(id, choice);
    }

    /// change in __particle a new drawn value for id
    template <typename GUM_SCALAR> void Gibbs<GUM_SCALAR>::__GibbsSample(NodeId id) {
      const DiscreteVariable &v = this->bn().variable(id);
      // we have to build P(x \given instantiation_markovblanket(x)) in posterior
      // see Pearl'88, 4.71 p218
      Potential<GUM_SCALAR> &posterior = *__sampling_posterior[id];
      Instantiation &posterior_idx = *__sampling_idx[id];

      Instantiation *tmp;
      GUM_SCALAR value;

      for (posterior_idx.setFirst(); !posterior_idx.end(); posterior_idx.inc()) {
        Idx current_mod_id = posterior_idx.val(v);
        tmp = __cpt_idx[id];
        tmp->chgVal(v, current_mod_id);
        posterior.set(posterior_idx, value = this->bn().cpt(id)[*tmp]);

        if (value == (GUM_SCALAR)0) {
          continue;
        }

        if (__evidences.exists(id)) {
          posterior.set(
              posterior_idx,
              posterior[posterior_idx] *(value = (*__evidences[id])[posterior_idx]));

          if (value == (GUM_SCALAR)0) {
            continue;
          }
        }

        for (std::vector<NodeId>::iterator iter = __node_children[id]->begin();
             iter != __node_children[id]->end(); iter++) {
          tmp = __cpt_idx[*iter];
          tmp->chgVal(v, current_mod_id);
          // posterior[posterior_idx]*=( value=this->bn().cpt( *iter )[*tmp] );
          posterior.set(posterior_idx, posterior[posterior_idx] *(
                                           value = this->bn().cpt(*iter)[*tmp]));

          if (value == (GUM_SCALAR)0) {
            continue;
          }
        }
      }

      __drawVar(id);
    }

    /// remove a given evidence from the graph
    template <typename GUM_SCALAR>
    INLINE void Gibbs<GUM_SCALAR>::eraseEvidence(const Potential<GUM_SCALAR> *pot) {
      const Sequence<const DiscreteVariable *> &vars = pot->variablesSequence();

      if (vars.size() != 1) {
        GUM_ERROR(SizeError, "The evidence should be one-dimensionnal");
      }

      NodeId id = this->bn().nodeId(*(vars.atPos(0)));

      if (__evidences.exists(id))
        __evidences.erase(id);

      if (__hard_evidences.exists(id))
        __hard_evidences.erase(id);
    }

    /// remove all evidence from the graph
    template <typename GUM_SCALAR>
    INLINE void Gibbs<GUM_SCALAR>::eraseAllEvidence() {
      __evidences.clear();
      __hard_evidences.clear();
    }

    /// insert new evidence in the graph
    template <typename GUM_SCALAR>
    void Gibbs<GUM_SCALAR>::insertEvidence(
        const List<const Potential<GUM_SCALAR> *> &pot_list) {
      for (ListConstIteratorSafe<const Potential<GUM_SCALAR> *> iter =
               pot_list.cbeginSafe();
           iter != pot_list.cendSafe(); ++iter) {
        // check that the evidence is given w.r.t.only one random variable
        const Potential<GUM_SCALAR> &pot = **iter;
        const Sequence<const DiscreteVariable *> &vars = pot.variablesSequence();

        if (vars.size() != 1) {
          GUM_ERROR(SizeError, "The evidence should be one-dimensionnal");
        }

        Size nb_un = 0;

        Size nb_zero = 0;

        Idx pos_un = 0;

        Instantiation I(pot);

        for (I.setFirst(); !I.end(); ++I) {
          if (pot[I] == (GUM_SCALAR)0) {
            nb_zero++;
          } else {
            pos_un = I.val(*vars.atPos(0));
            nb_un++;
          }
        }

        // insert the evidence
        if ((nb_un == 1) && (nb_un + nb_zero == I.domainSize())) {
          __hard_evidences.insert(this->bn().nodeId(*(vars.atPos(0))), pos_un);
        } else {
          __evidences.insert(this->bn().nodeId(*(vars.atPos(0))), *iter);
        }
      }
    }

    /// put in __particle a MC sample
    /// This is not a really sample since we take into account evidence without
    /// care about parent of evidence, etc.
    /// This is just a not-so-bad first sample for GIBBS
    template <typename GUM_SCALAR> void Gibbs<GUM_SCALAR>::__MonteCarloSample() {
      // _nodes_array is assumed to be the list of nodes to draw; in a
      // topological-compatible order
      for (unsigned int it = 0; it < __nodes_array.size(); it++) {
        Idx id = __nodes_array[it];

        const Potential<GUM_SCALAR> &cpt = this->bn().cpt(id);

        const DiscreteVariable &v = this->bn().variable(id);

        // we have to build P(x \given instantiation_parent(x)) in proba
        Instantiation I(cpt);

        for (auto var : I.variablesSequence()) {
          I.chgVal(I.pos(*var), __particle.valFromPtr(var));
        }

        Potential<GUM_SCALAR> &proba = *__sampling_posterior[id];

        for (I.setFirstVar(v); !I.end(); I.incVar(v)) {
          if (__evidences.exists(id)) {
            proba.set(I, (*__evidences[id])[I]);
          } else {
            proba.set(I, cpt[I]);
          }
        }

        __drawVar(id);
      }
    }

    template <typename GUM_SCALAR> void Gibbs<GUM_SCALAR>::initParticle() {
      __nodes_array.clear();
      // nodes to be drawn : not the ones with hard evidence

      for (auto node : this->bn().topologicalOrder()) {
        if (!__hard_evidences.exists(node)) {
          __nodes_array.push_back(node);
        } else {
          // put the right value for instantiated variables
          __setValVar(node, __hard_evidences[node]);
        }
      }

      __MonteCarloSample();

      if (__nbr_drawn_by_sample == 0) { // means topological ordre for every sample
        __nbr_of_iterations = __nodes_array.size();
      } else { // randomly choosen set of vars of size __nbr_drawn_by_sample
        __nbr_of_iterations = __nbr_drawn_by_sample;

        if (__nbr_of_iterations > __nodes_array.size())
          __nbr_of_iterations = __nodes_array.size();

        std::random_shuffle(__nodes_array.begin(), __nodes_array.end());
      }
    }

    template <typename GUM_SCALAR> void Gibbs<GUM_SCALAR>::nextParticle() {
      // GENERATE NEXT SAMPLE
      if (__nbr_drawn_by_sample == 0) {
        // we want to draw all the node always in the same order
        for (std::vector<NodeId>::iterator it = __nodes_array.begin();
             it != __nodes_array.end(); ++it) {
          __GibbsSample(*it);
        }
      } else if (__nbr_of_iterations ==
                 1) { // we want to draw only one randomly chosen node
        __GibbsSample(__nodes_array[std::rand() % __nodes_array.size()]);
      } else { // we want to draw nbr randomly chosen nodes.
        std::vector<NodeId>::iterator it = __nodes_array.begin();

        for (Size j = 0; j < __nbr_of_iterations; j++, ++it) {
          __GibbsSample(*it);
        }

        std::random_shuffle(__nodes_array.begin(), __nodes_array.end());
      }
    }
  } // namespace particle
} // namespace gum

#endif // DOXYGEN_SHOULD_SKIP_THIS
