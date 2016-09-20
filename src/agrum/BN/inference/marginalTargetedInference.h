/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
 * @brief This file contains abstract class definitions for Bayesian networks
 *        inference classes (that support single targets computations).
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 */

#ifndef GUM_BAYES_NET_MARGINAL_TARGETED_INFERENCE_H
#define GUM_BAYES_NET_MARGINAL_TARGETED_INFERENCE_H

#include <agrum/config.h>
#include <agrum/BN/inference/inference.h>


namespace gum {

  
  /**
   * @class MarginalTargetedInference marginalTargetedInference.h
   * <agrum/BN/inference/marginalTargetedInference.h>
   * @brief A generic class for the computation of marginal posteriors
   * @ingroup bn_group
   *
   * The goal of the Inference class is twofold:
   * i) handling the common resources of BN inference (bn, soft/hard
   *    evidence and (single, i.e., not joint) targets); and
   * ii) propose a general scheme for all inference methods.
   *
   * A specialized inference just has to specify how to prepare inference, how
   * to make inference and how to get the posteriors for nodes in
   * pure virtual protected methods. and the scheme for every inference
   * derived from Inference will be the same:
   *
   * 1- ie=SpecificInference(bn);              // state <- UnpreparedStructure
   * 2- set targets and evidence in ie
   * 3- ie.prepareInference();                 // state <- Ready4Inference
   * 4.a- change values of evidence in ie      // state <- OutdatedPotentials
   * 4.b- change some hard evidence or targets // state <- UnpreparedStructure
   * 5- ie.makeInference();                    // state <- Done
   * 6- get posteriors
   * 7- goto 2 or 4
   *
   * Inference can be in one of 4 different states:
   * - UnpreparedStructure: in this state, the inference is fully unprepared
   *   to be applied. It (probably) needs a significant amount of preparation
   *   to be ready. In a Lazy propagation, for instance, this step amounts to
   *   compute a new junction tree, hence a new structure in which inference
   *   will be applied. Note that classes that inherit from Inference may be
   *   smarter than Inference and may, in some situations, find out that their
   *   data structures are still ok for inference and, therefore, only resort to
   *   perform the actions related to the OutdatedPotentials state. As an example,
   *   consider a LazyPropagation inference in Bayes Net A->B->C->D->E in which
   *   C has received hard evidence e_C and E is the only target. In this case, A
   *   and B are not needed for inference, the only potentials that matter are
   *   P(D|e_C) and P(E|D). So the smallest junction tree needed for inference
   *   contains only one clique DE. Now, adding new evidence e_A on A has no
   *   impact on E given hard evidence e_C. In this case, LazyPropagation should
   *   be smart and not update its junction tree.
   * - OutdatedPotentials: in this state, the inference just needs to invalidate
   *   some already computed potentials to be ready. Only a light amount of
   *   preparation is needed to be able to perform inference.
   * - Ready4Inference: in this state, all the data structures are ready for
   *   inference. There just remains to perform the inference computations.
   * - Done: the heavy computations of inference have been done. There might
   *   still remain a few light computations to perform to get the posterior
   *   potentials we need. Typically, in Lazy Propagation, all the messages in
   *   the junction tree have been computed but, to get the potentials, we still
   *   need to perform the combinations of the potentials in the cliques with
   *   the messages sent to the cliques. In some inference algorithms, this
   *   step may even be empty.
   */

  template <typename GUM_SCALAR>
  class MarginalTargetedInference : public virtual Inference<GUM_SCALAR> {
  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @warning By default, all the nodes of the Bayes net are targets.
     * @warning note that, by aGrUM's rule, the BN is not copied but only
     * referenced by the inference algorithm. */
    MarginalTargetedInference( const IBayesNet<GUM_SCALAR>* bn );

    /// destructor
    virtual ~MarginalTargetedInference();

    /// @}

    
    // ############################################################################
    /// @name Probability computations
    // ############################################################################
    /// @{

    /// Computes and returns the posterior of a node.
    /**
     * @returns a const ref to the posterior probability of the node.
     * @param node the node for which we need a posterior probability
     *
     * @warning for efficiency reasons, the potential is stored into the
     * inference engine and is returned by reference. In order to ensure
     * that the potential may still exist even if the Inference object is
     * destroyed, the user has to copy it explicitly.
     *
     * @warning prepareInference and makeInference may be applied if needed.
     *
     * @throw UndefinedElement if node is not in the set of targets
     */
    virtual const Potential<GUM_SCALAR>& posterior( const NodeId node ) final;

    /// @}

    
    // ############################################################################
    /// @name Targets
    // ############################################################################
    /// @{

    /// Clear all previously defined targets
    virtual void eraseAllTargets ();

    /// Add a single target to the list of targets
    /**
     * @throw UndefinedElement if target is not a NodeId in the Bayes net
     */
    virtual void addTarget ( const NodeId target ) final;

    /// removes an existing (single) target
    /** @warning If the target does not already exist, the method does nothing.
     * In particular, it does not raise any exception. */
    virtual void eraseTarget ( const NodeId target ) final;

    /// return true if variable is a (single) target
    virtual bool isTarget( const NodeId variable ) const final;

    /// returns the list of single targets
    virtual const NodeSet& targets() const noexcept final;

    /// @}

    
  protected:
    /// fired after a new single target is inserted
    /** @param id The target variable's id. */
    virtual void _onMarginalTargetAdded ( const NodeId id ) = 0;

    /// fired before a single target is removed
    /** @param id The target variable's id. */
    virtual void _onMarginalTargetErased ( const NodeId id ) = 0;

    /// fired after all the nodes of the BN are added as single targets
    virtual void _onAllMarginalTargetsAdded () = 0;

    /// fired before a all single targets are removed
    virtual void _onAllMarginalTargetsErased () = 0;

    /// fired after a new Bayes net has been assigned to the engine
    virtual void _onBayesNetChanged ( const IBayesNet<GUM_SCALAR>* bn );

    /// asks derived classes for the posterior of a given variable
    /** @param id The variable's id. */
    virtual const Potential<GUM_SCALAR>& _posterior( const NodeId id ) = 0;

    
  private:
    /// the set of single targets
    NodeSet __targets;


    /// remove all the posteriors computed (single and set targets)
    void __invalidatePosteriors() noexcept;

     /// sets all the nodes of the Bayes net as targets
    void __setAllMarginalTargets ();

  };


}  // namespace gum


#include <agrum/BN/inference/marginalTargetedInference.tcc>



#endif // GUM_BAYES_NET_MARGINAL_TARGETED_INFERENCE_H
