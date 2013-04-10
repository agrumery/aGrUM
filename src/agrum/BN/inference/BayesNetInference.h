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
 * @brief This file contains abstract class definitions bayesian networks
 *        inference classes.
 *
 * @author Lionel Torti.
 */
// ============================================================================
#ifndef GUM_INFERENCE_H
#define GUM_INFERENCE_H
// ============================================================================
#include <agrum/config.h>
// ============================================================================
#include <agrum/BN/BayesNet.h>
// ============================================================================

namespace gum {
  /**
   * @class BayesNetInference BayesNetInference.h <agrum/BN/inference/BayesNetInference.h>
   * @brief Abstract class for making inference in bayesian networks.
   * @ingroup bn_group
   *
   */
  template <typename GUM_SCALAR>

  class BayesNetInference {
    public:
      /**
       * Default constructor
       */
      BayesNetInference( const AbstractBayesNet<GUM_SCALAR>& bn );

      /**
       * Destructor.
       */
      virtual ~BayesNetInference();

      /**
       * Makes the inference
       */
      virtual void makeInference() = 0;

      /**
       * @brief Returns the probability of the variable.
       *
       * If makeInference() wasn't called yet, then only the marginal
       * of the given variable will be computed.
       *
       * @param id The variable's id.
       * @return The probability of the variable.
       * @throw NotFound Raised if no variable matches id.
       * @throw OperationNotAllowed Raised if the inference can not be done.
       */
      virtual const Potential<GUM_SCALAR>& marginal( NodeId id );

      /**
       * Insert new evidence in the graph.
       * @warning if an evidence already w.r.t. a given node and a new
       * evidence w.r.t. this node is onserted, the old evidence is removed.
       * @throw OperationNotAllowed Raised if an evidence is over more than one variable.
       */
      virtual void insertEvidence( const List<const Potential<GUM_SCALAR>*>& pot_list ) = 0;

      /**
       * Remove a given evidence from the graph.
       */
      virtual void eraseEvidence( const Potential<GUM_SCALAR>* e ) = 0;

      /**
       * Remove all evidence from the graph.
       */
      virtual void eraseAllEvidence() = 0;

      /**
       * Returns a constant reference over the BayesNet on which this class work.
       */
      const AbstractBayesNet<GUM_SCALAR>& bn() const;

    protected:

      /**
       * @brief READ FULL DOCUMENTATION (THAT MEANS CLICK ON ME!!)
       *
       * This method is called when a BayesNetInference user ask for the marginal of
       * a given variable.
       *
       * The reference "marginal" is a reference over an empty Potential, it doesn't
       * even contains a reference over the variable's DiscreteVariable (don't forget
       * to add it!).
       *
       * TODO Change this method and either return a pointer or delegate marginal
       *      handling to subclasses.
       *
       * @param id The variable's id.
       * @param marginal The completely empty potential to fill.
       * @throw ElementNotFound Raised if no variable matches id.
       */
      virtual void _fillMarginal( NodeId id, Potential<GUM_SCALAR>& marginal ) = 0;

      /**
       * Invalidate the set of marginals kept here.
       */
      void _invalidateMarginals();

      /**
       * Mapping between marginals and __bayesNet's nodes.
       */
      typename Property<Potential<GUM_SCALAR>*>::onNodes _marginals;

    private:
      /**
       * The Bayes net we wish to perform inference on.
       */
      const AbstractBayesNet<GUM_SCALAR>& __bayesNet;
  };

// ============================================================================
} /* namespace gum */

// ============================================================================
#include <agrum/BN/inference/BayesNetInference.tcc>
// ============================================================================
#endif /* GUM_INFERENCE_H */
// ============================================================================
// kate: indent-mode cstyle; indent-width 1; replace-tabs on; ;
