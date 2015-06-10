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
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_INFERENCE_H
#define GUM_INFERENCE_H

#include <agrum/config.h>
#include <agrum/BN/IBayesNet.h>

namespace gum {
  /**
   * @class BayesNetInference BayesNetInference.h
   *<agrum/BN/inference/BayesNetInference.h>
   * @brief Abstract class for making inference in bayesian networks.
   * @ingroup bn_group
   *
   */
  template <typename GUM_SCALAR> class BayesNetInference {
    public:
    /**
     * Default constructor
     */
    BayesNetInference(const IBayesNet<GUM_SCALAR> &bn);

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
     * If makeInference() wasn't called yet, then only the posterior
     * of the given variable will be computed.
     *
     * @param id The variable's id.
     * @return The probability of the variable.
     * @throw NotFound Raised if no variable matches id.
     * @throw OperationNotAllowed Raised if the inference can not be done.
     */
    virtual const Potential<GUM_SCALAR> &posterior(NodeId id);

    /**
     * Insert new evidence in the inference.
     * @warning if an evidence already w.r.t. a given node and a new
     * evidence w.r.t. this node is obserted, the old evidence is removed.
     *
     * @warning The potentials are not copied.
     *
     * @throw OperationNotAllowed Raised if an evidence is over more than one
     *variable.
     */
    virtual void
    insertEvidence(const List<const Potential<GUM_SCALAR> *> &pot_list) = 0;

    /**
     * Insert a new hard evidence in the inference
     *
     * @throw OutOfBound if val is not in the domainSize of node id
     */
    void addHardEvidence(const NodeId id, const Idx val);

    /**
     * Remove a given evidence from the graph.
     *
     * @warning the potential has to be deleted aterward.
     */
    virtual void eraseEvidence(const Potential<GUM_SCALAR> *e) = 0;

    /**
     * Remove all evidence from the graph.
     */
    virtual void eraseAllEvidence() = 0;

    /**
     * Returns a constant reference over the IBayesNet on which this class work.
     */
    const IBayesNet<GUM_SCALAR> &bn() const;

    protected:
    /**
     * @brief Fill the potential with the computed posterior
     *
     * This method is called when a BayesNetInference user ask for the posterior of
     * a given variable.
     *
     * The reference "posterior" is a reference over an empty Potential, it doesn't
     * even contains a reference over the variable's DiscreteVariable (don't forget
     * to add it!).
     *
     * @param id The variable's id.
     * @param posterior The completely empty potential to fill.
     * @throw ElementNotFound Raised if no variable matches id.
     */
    virtual void _fillPosterior(NodeId id, Potential<GUM_SCALAR> &posterior) = 0;

    /**
     * Invalidate the set of posterior kept here.
     */
    void _invalidatePosteriors();

    /**
     * Mapping between posterior and __bayesNet's nodes.
     */
    NodeProperty<Potential<GUM_SCALAR> *> _posteriors;

    private:
    /**
     * The Bayes net we wish to perform inference on.
     */
    const IBayesNet<GUM_SCALAR> &__bayesNet;

    // list of potential created by internal methods (addHardEvidence for instance)
    // to be deleted during the destruction of the inference
    List<const Potential<GUM_SCALAR> *> __garbages;
  };

  extern template class BayesNetInference<float>;
  extern template class BayesNetInference<double>;

} /* namespace gum */

#include <agrum/BN/inference/BayesNetInference.tcc>

#endif /* GUM_INFERENCE_H */
