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
 * @brief This file contains abstract class definitions for Markov random fields
 *        inference classes.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_MARKOV_RANDOM_FIELD_INFERENCE_H
#define GUM_MARKOV_RANDOM_FIELD_INFERENCE_H


#include <agrum/agrum.h>
#include <agrum/tools/graphicalModels/inference/graphicalModelInference.h>
#include <agrum/MRF/IMarkovRandomField.h>

namespace gum {

  // JointTargetedMRFInference, the class for computing joint posteriors, should
  // have access to the states of Inference and change them when needed: this
  // will be a friend of Inference
  template < typename GUM_SCALAR >
  class JointTargetedMRFInference;

  // MarginalTargetedMRFInference, the class for computing marginal posteriors,
  // should have access to the states of Inference and change them when needed:
  // this should be a friend of Inference
  template < typename GUM_SCALAR >
  class MarginalTargetedMRFInference;

  // EvidenceMRFInference, the class for computing the probability of evidence,
  // should have access to the states of Inference and change them when needed:
  // this will be a friend of Inference
  template < typename GUM_SCALAR >
  class EvidenceMRFInference;


  /**
   * @class MRFInference inference.h
   * <agrum/MRF/inference/MRFInference.h>
   * @brief A generic class for Markov net inference: handles evidence and the
   * current state of the (incremental) inference
   * @ingroup mn_group
   *
   * The goal of the MRFInference class is twofold:
   * i) handling the common resources of MRF inference (mn, soft/hard evidence);
   * ii) propose a general high-level scheme for all the inference methods.
   *
   * A specialized inference just has to specify how to prepare inference, how
   * to make inference and how to get the posteriors for nodes and set of nodes.
   * The scheme for every inference derived from MRFInference will be
   * the same:
   *
   * 1- ie=SpecificInference(mn);              // state <- OutdatedStructure
   * 2- set targets and evidence in ie
   * 3- ie.prepareInference();                 // state <- ReadyForMRFInference
   * 4.a- change values of evidence in ie      // state <- OutdatedPotentials
   * 4.b- change some hard evidence or targets // state <- OutdatedStructure
   * 5- ie.makeInference();                    // state <- Done
   * 6- get posteriors
   * 7- goto 2 or 4
   *
   * MRFInference can be in one of 4 different states:
   * - OutdatedStructure: in this state, the inference is fully unprepared
   *   to be applied because some events changed the "logical" structure of the
   *   MRF: for instance a node received a hard evidence, which implies that
   *   its outgoing arcs can be removed from the MRF, hence involving a
   *   structural change in the MRF. As a consequence, the (incremental)
   *   inference (probably) needs a significant amount of preparation to be ready
   *   for the next inference. In a Lazy propagation, for instance, this step
   *   amounts to compute a new join tree, hence a new structure in which inference
   *   will be applied. Note that classes that inherit from MRFInference
   *   may be smarter than MRFInference and may, in some situations,
   *   find out that their data structures are still ok for inference and,
   *   therefore, only resort to perform the actions related to the
   *   OutdatedPotentials state.
   * - OutdatedPotentials: in this state, the structure of the MRF remains
   *   unchanged, only some potentials stored in it have changed. Therefore,
   *   the inference probably just needs to invalidate some already computed
   *   potentials to be ready. Only a light amount of preparation is needed to
   *   be able to perform inference.
   * - ReadyForMRFInference: in this state, all the data structures are ready for
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
  class MRFInference: public GraphicalModelInference< GUM_SCALAR > {
    public:
    /**
     * current state of the inference
     *
     * MRFInference can be in one of 4 different states:
     * - OutdatedStructure: in this state, the inference is fully unprepared
     *   to be applied because some events changed the "logical" structure of
     *   the MRF: for instance a node received a hard evidence, which implies
     *   that its outgoing arcs can be removed from the MRF, hence involving a
     *   structural change in the MRF. As a consequence, the (incremental)
     *   inference (probably) needs a significant amount of preparation to be
     *   ready for the next inference. In a Lazy propagation, for instance,
     *   this step amounts to compute a new join tree, hence a new structure
     *   in which inference will be applied. Note that classes that inherit
     *   from MRFInference may be smarter than MRFInference and may,
     *   in some situations, find out that their data structures are still ok for
     *   inference and, therefore, only resort to perform the actions related
     *   to the OutdatedPotentials state.
     * - OutdatedPotentials: in this state, the structure of the MRF remains
     *   unchanged, only some potentials stored in it have changed. Therefore,
     *   the inference probably just needs to invalidate some already computed
     *   potentials to be ready. Only a light amount of preparation is needed to
     *   be able to perform inference.
     * - ReadyForMRFInference: in this state, all the data structures are ready for
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
    /** @warning note that, by aGrUM's rule, the MRF is not copied but only
     * referenced by the inference algorithm. */
    explicit MRFInference(const IMarkovRandomField< GUM_SCALAR >* mn);

    /// default constructor with a null MRF (useful for virtual inheritance)
    /** @warning MRFInference is virtually inherited by
     * MarginalTargetedMRFInference. As a result, the lowest descendant of
     * MRFInference will create the latter. To avoid requiring developpers
     * to add in the constructors of their inference algorithms a call to
     * MRFInference( mn ), we added constructor MRFInference(),
     * which will be called automatically by the lowest descendant.
     * Then, MarginalTargetedMRFInference and JointTargetedMRFInference will take
     * care of setting the appropriate mn into MRFInference. */
    MRFInference();

    /// destructor
    virtual ~MRFInference();

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// assigns a new MRF to the inference engine
    /** Assigns a new MRF to the MRFInference engine and sends messages to the
     * descendants of MRFInference to inform them that the MRF has changed.
     * @warning By default, all the nodes of the Markov net are targets.
     * @warning note that, by aGrUM's rule, the mn is not copied into the
     * inference engine but only referenced. */
    virtual void setMRF(const IMarkovRandomField< GUM_SCALAR >* mrf);

    /// Returns a constant reference over the IMarkovRandomField referenced by this class
    /** @throws UndefinedElement is raised if no Markov net has been assigned to
     * the inference. */
    virtual const IMarkovRandomField< GUM_SCALAR >& MRF() const final;

    private:
    /// assigns a MRF during the inference engine construction
    void _setMRFDuringConstruction_(const IMarkovRandomField< GUM_SCALAR >* mn);


    /// allow JointInference to access the single targets and inference states
    friend MarginalTargetedMRFInference< GUM_SCALAR >;
    friend JointTargetedMRFInference< GUM_SCALAR >;
    friend EvidenceMRFInference< GUM_SCALAR >;
  };


}   // namespace gum


#include <agrum/MRF/inference/tools/MRFInference_tpl.h>


#endif   // GUM_MARKOV_RANDOM_FIELD_INFERENCE_H
