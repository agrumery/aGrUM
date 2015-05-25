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
 * @brief This file contains gibbs sampling (for BNs) class definitions.
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 */
#ifndef GUM_GIBBS_INFERENCE_H
#define GUM_GIBBS_INFERENCE_H

#include <agrum/BN/inference/BayesNetInference.h>
#include <agrum/BN/particles/Gibbs.h>
#include <agrum/core/algorithms/approximationScheme/approximationScheme.h>

namespace gum {

  /**
   * @class GibbsInference GibbsInference.h <agrum/BN/inference/GibbsInference.h>
   * @brief class for making Gibbs sampling inference in bayesian networks.
   * @ingroup bn_group
   *
   */
  template <typename GUM_SCALAR>

  class GibbsInference : public ApproximationScheme,
                         public BayesNetInference<GUM_SCALAR>,
                         public particle::Gibbs<GUM_SCALAR> {

    public:
    /**
     * Default constructor
     */
    GibbsInference(const IBayesNet<GUM_SCALAR> &BN);

    /**
     * Destructor.
     */
    virtual ~GibbsInference();

    /**
     * Makes the inference: all posteriors are computed.
     */
    virtual void makeInference();

    /**
     * Insert new evidence in the graph.
     * @warning if an evidence already w.r.t. a given node and a new
     * evidence w.r.t. this node is inserted, the old evidence is removed.
     */
    virtual void insertEvidence(const List<const Potential<GUM_SCALAR> *> &pot_list);

    /**
     * Remove a given evidence from the graph.
     */
    virtual void eraseEvidence(const Potential<GUM_SCALAR> *e);

    /**
     * Remove all evidence from the graph.
     */
    virtual void eraseAllEvidence();

    /// lazy management of infenrence
    ///@{
    void setRequiredInference();
    bool isInferenceRequired();
    ///@}

    using particle::Gibbs<GUM_SCALAR>::particle;
    using particle::Gibbs<GUM_SCALAR>::initParticle;
    using particle::Gibbs<GUM_SCALAR>::nextParticle;
    using particle::Gibbs<GUM_SCALAR>::bn;

    protected:
    /**
     * Returns the probability of the variable.
     *
     * @param id The variable's id.
     * @param posterior the potential to fill
     * @throw ElementNotFound Raised if no variable matches id.
     */
    virtual void _fillPosterior(NodeId id, Potential<GUM_SCALAR> &posterior);

    private:
    /// inference flag
    bool __inference_is_required;

    /// the actual number of sampling for each modality by node
    NodeProperty<Potential<GUM_SCALAR> *> __sampling_nbr;

    void __unsetRequiredInference();
    void __initStats();
    void __updateStats_without_err();
    double __updateStats_with_err(Size nbr);
  };

  extern template class GibbsInference<float>;
  extern template class GibbsInference<double>;

} /* namespace gum */

#include <agrum/BN/inference/GibbsInference.tcc>

#endif /* GUM_GIBBS_INFERENCE_H */
