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

#include <agrum/BN/inference/marginalTargetedInference.h>
#include <agrum/BN/samplers/GibbsSampler.h>
#include <agrum/core/approximations/approximationScheme.h>

namespace gum {

  /**
   * @class GibbsInference GibbsInference.h
   *<agrum/BN/inference/GibbsInference.h>
   * @brief class for making Gibbs sampling inference in bayesian networks.
   * @ingroup bn_inference
   *
   */
  template <typename GUM_SCALAR>

  class GibbsInference : public ApproximationScheme,
                         public MarginalTargetedInference<GUM_SCALAR>,
                         public samplers::GibbsSampler<GUM_SCALAR> {

    public:
    /**
     * Default constructor
     */
    GibbsInference( const IBayesNet<GUM_SCALAR>* BN );

    /**
     * Destructor.
     */
    virtual ~GibbsInference();

    using samplers::GibbsSampler<GUM_SCALAR>::particle;
    using samplers::GibbsSampler<GUM_SCALAR>::initParticle;
    using samplers::GibbsSampler<GUM_SCALAR>::nextParticle;
    using samplers::GibbsSampler<GUM_SCALAR>::bn;
    using samplers::GibbsSampler<GUM_SCALAR>::addSoftEvidenceSampler;
    using samplers::GibbsSampler<GUM_SCALAR>::addHardEvidenceSampler;
    using samplers::GibbsSampler<GUM_SCALAR>::eraseSoftEvidenceSampler;
    using samplers::GibbsSampler<GUM_SCALAR>::eraseHardEvidenceSampler;
    using samplers::GibbsSampler<GUM_SCALAR>::eraseAllEvidenceSampler;

    protected:
    virtual void _onEvidenceAdded( const NodeId id, bool isHardEvidence );
    virtual void _onEvidenceErased( const NodeId id, bool isHardEvidence );
    virtual void _onAllEvidenceErased( bool contains_hard_evidence );

    virtual void _onEvidenceChanged( const NodeId id, bool hasChangedSoftHard );
    virtual void _onBayesNetChanged( const IBayesNet<GUM_SCALAR>* bn );

    virtual void _updateOutdatedBNStructure(){};
    virtual void _updateOutdatedBNPotentials(){};
    virtual void _makeInference();

    virtual void _onMarginalTargetAdded( const NodeId id ){};
    virtual void _onMarginalTargetErased( const NodeId id ){};
    virtual void _onAllMarginalTargetsAdded(){};
    virtual void _onAllMarginalTargetsErased(){};

    /// asks derived classes for the posterior of a given variable
    /** @param id The variable's id. */
    virtual const Potential<GUM_SCALAR>& _posterior( const NodeId id );

    /// the actual number of sampling for each modality by node
    NodeProperty<Potential<GUM_SCALAR>> __sampling_nbr;

    void   __unsetRequiredInference();
    void   __initStats();
    void   __updateStats_without_err();
    double __updateStats_with_err( Size nbr );
  };


  extern template class GibbsInference<float>;
  extern template class GibbsInference<double>;


} /* namespace gum */

#include <agrum/BN/inference/GibbsInference.tcc>

#endif /* GUM_GIBBS_INFERENCE_H */
