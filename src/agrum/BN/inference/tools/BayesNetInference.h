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
 * @brief This file contains abstract class definitions for Bayesian networks
 *        inference classes.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_BAYES_NET_INFERENCE_H
#define GUM_BAYES_NET_INFERENCE_H


#include <agrum/BN/IBayesNet.h>
#include <agrum/tools/graphicalModels/inference/graphicalModelInference.h>
#include <agrum/agrum.h>


namespace gum {


  /** @brief type of algorithm to determine barren nodes
   *
   * When constructing messages from one clique to its neighbor, we can
   * determine that some nodes are barren, i.e., they are the only one
   * at the left hand side of a conditioning bar and they appear in only one
   * potential. In such case, in a classical BN inference, there is no need
   * to take them into account since their removal will necessarily create
   * a constant potential. So, we can discard their potential from the
   * computation. However, when computing p(evidence), we should not do that
   * because the constant is important and need be computed.
   */
  enum class FindBarrenNodesType {
    FIND_NO_BARREN_NODES,   // do not try to find barren nodes
    FIND_BARREN_NODES       // use a bottom-up algorithm to detect barren nodes
  };


  // JointTargetedInference, the class for computing joint posteriors, should
  // have access to the states of Inference and change them when needed: this
  // will be a friend of Inference
  template < typename GUM_SCALAR >
  class JointTargetedInference;

  // MarginalTargetedInference, the class for computing marginal posteriors,
  // should have access to the states of Inference and change them when needed:
  // this should be a friend of Inference
  template < typename GUM_SCALAR >
  class MarginalTargetedInference;

  // EvidenceInference, the class for computing the probability of evidence,
  // should have access to the states of Inference and change them when needed:
  // this will be a friend of Inference
  template < typename GUM_SCALAR >
  class EvidenceInference;


  /**
   * @class BayesNetInference inference.h
   * <agrum/BN/inference/BayesNetInference.h>
   * @brief A generic class for Bayes net inference: handles evidence and the
   * current state of the (incremental) inference
   * @ingroup bn_group
   *
   * The goal of the BayesNetInference class is twofold:
   * i) handling the common resources of BN inference (bn, soft/hard evidence);
   * ii) propose a general high-level scheme for all the inference methods.
   *
   * A specialized inference just has to specify how to prepare inference, how
   * to make inference and how to get the posteriors for nodes and set of nodes.
   * The scheme for every inference derived from BayesNetInference will be
   * the same:
   *
   * 1- ie=SpecificInference(bn);              // state <- OutdatedStructure
   * 2- set targets and evidence in ie
   * 3- ie.prepareInference();                 // state <- Ready4Inference
   * 4.a- change values of evidence in ie      // state <- OutdatedPotentials
   * 4.b- change some hard evidence or targets // state <- OutdatedStructure
   * 5- ie.makeInference();                    // state <- Done
   * 6- get posteriors
   * 7- goto 2 or 4
   *
   * BayesNetInference can be in one of 4 different states:
   * - OutdatedStructure: in this state, the inference is fully unprepared
   *   to be applied because some events changed the "logical" structure of the
   *   BN: for instance a node received a hard evidence, which implies that
   *   its outgoing arcs can be removed from the BN, hence involving a
   *   structural change in the BN. As a consequence, the (incremental)
   *   inference
   *   (probably) needs a significant amount of preparation to be ready for the
   *   next inference. In a Lazy propagation, for instance, this step amounts to
   *   compute a new join tree, hence a new structure in which inference
   *   will be applied. Note that classes that inherit from BayesNetInference
   *   may be smarter than BayesNetInference and may, in some situations,
   *   find out that their
   *   data structures are still ok for inference and, therefore, only resort to
   *   perform the actions related to the OutdatedPotentials state. As an
   *   example, consider a LazyPropagation inference in Bayes Net A->B->C->D->E
   *   in which C has received hard evidence e_C and E is the only target. In
   *   this case, A and B are not needed for inference, the only potentials that
   *   matter are P(D|e_C) and P(E|D). So the smallest join tree needed for
   *   inference contains only one clique DE. Now, adding new evidence e_A on A
   *   has no impact on E given hard evidence e_C. In this case, LazyPropagation
   *   can be smart and not update its join tree.
   * - OutdatedPotentials: in this state, the structure of the BN remains
   *   unchanged, only some potentials stored in it have changed. Therefore,
   *   the inference probably just needs to invalidate some already computed
   *   potentials to be ready. Only a light amount of preparation is needed to
   *   be able to perform inference.
   * - Ready4Inference: in this state, all the data structures are ready for
   *   inference. There just remains to perform the inference computations.
   * - Done: the heavy computations of inference have been done. There might
   *   still remain a few light computations to perform to get the posterior
   *   potentials we need. Typically, in Lazy Propagation, all the messages in
   *   the join tree have been computed but, to get the potentials, we still
   *   need to perform the combinations of the potentials in the cliques with
   *   the messages sent to the cliques. In some inference algorithms, this
   *   step may even be empty.
   */

  template < typename GUM_SCALAR >
  class BayesNetInference: public GraphicalModelInference< GUM_SCALAR > {
    public:
    /**
     * current state of the inference
     *
     * BayesNetInference can be in one of 4 different states:
     * - OutdatedStructure: in this state, the inference is fully unprepared
     *   to be applied because some events changed the "logical" structure of
     *   the BN: for instance a node received a hard evidence, which implies
     *   that its outgoing arcs can be removed from the BN, hence involving a
     *   structural change in the BN. As a consequence, the (incremental)
     *   inference (probably) needs a significant amount of preparation to be
     *   ready for the next inference. In a Lazy propagation, for instance,
     *   this step amounts to compute a new join tree, hence a new structure
     *   in which inference will be applied. Note that classes that inherit
     *   from BayesNetInference may be smarter than BayesNetInference and may,
     *   in some situations, find out that their data structures are still ok for
     *   inference and, therefore, only resort to perform the actions related
     *   to the OutdatedPotentials state. As an example, consider a
     *   LazyPropagation inference in Bayes Net A->B->C->D->E
     *   in which C has received hard evidence e_C and E is the only target. In
     *   this case, A and B are not needed for inference, the only potentials
     *   that matter are P(D|e_C) and P(E|D). So the smallest join tree needed
     *   for inference contains only one clique DE. Now, adding new evidence
     *   e_A on A has no impact on E given hard evidence e_C. In this case,
     *   LazyPropagation can be smart and not update its join tree.
     * - OutdatedPotentials: in this state, the structure of the BN remains
     *   unchanged, only some potentials stored in it have changed. Therefore,
     *   the inference probably just needs to invalidate some already computed
     *   potentials to be ready. Only a light amount of preparation is needed to
     *   be able to perform inference.
     * - Ready4Inference: in this state, all the data structures are ready for
     *   inference. There just remains to perform the inference computations.
     * - Done: the heavy computations of inference have been done. There might
     *   still remain a few light computations to perform to get the posterior
     *   potentials we need. Typically, in Lazy Propagation, all the messages in
     *   the join tree have been computed but, to get the potentials, we still
     *   need to perform the combinations of the potentials in the cliques with
     *   the messages sent to the cliques. In some inference algorithms, this
     *   step may even be empty.
     */


    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @warning note that, by aGrUM's rule, the BN is not copied but only
     * referenced by the inference algorithm. */
    explicit BayesNetInference(const IBayesNet< GUM_SCALAR >* bn);

    /// default constructor with a null BN (useful for virtual inheritance)
    /** @warning BayesNetInference is virtually inherited by
     * MarginalTargetedInference. As a result, the lowest descendant of
     * BayesNetInference will create the latter. To avoid requiring developers
     * to add in the constructors of their inference algorithms a call to
     * BayesNetInference( bn ), we added constructor BayesNetInference(),
     * which will be called automatically by the lowest descendant.
     * Then, MarginalTargetedInference and JointTargetedInference will take care
     * of setting the appropriate bn into BayesNetInference. */
    BayesNetInference();

    /// destructor
    virtual ~BayesNetInference();

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// assigns a new BN to the inference engine
    /** Assigns a new BN to the BayesNetInference engine and sends messages to the
     * descendants of BayesNetInference to inform them that the BN has changed.
     * @warning By default, all the nodes of the Bayes net are targets.
     * @warning note that, by aGrUM's rule, the bn is not copied into the
     * inference engine but only referenced. */
    virtual void setBN(const IBayesNet< GUM_SCALAR >* bn);

    /// Returns a constant reference over the IBayesNet referenced by this class
    /** @throws UndefinedElement is raised if no Bayes net has been assigned to
     * the inference. */
    virtual const IBayesNet< GUM_SCALAR >& BN() const final;

    private:
    /// assigns a BN during the inference engine construction
    void _setBayesNetDuringConstruction_(const IBayesNet< GUM_SCALAR >* bn);


    /// allow JointInference to access the single targets and inference states
    friend MarginalTargetedInference< GUM_SCALAR >;
    friend JointTargetedInference< GUM_SCALAR >;
    friend EvidenceInference< GUM_SCALAR >;
  };


}   // namespace gum


#include <agrum/BN/inference/tools/BayesNetInference_tpl.h>


#endif   // GUM_BAYES_NET_INFERENCE_H
