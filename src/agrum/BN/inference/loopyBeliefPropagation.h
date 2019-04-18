
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief This file contains gibbs sampling (for BNs) class definitions.
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 */
#ifndef GUM_LOOPYBELIEFPROPAGATION_H
#define GUM_LOOPYBELIEFPROPAGATION_H

#include <agrum/BN/inference/tools/approximateInference.h>

namespace gum {
  /**
   * @class LoopyBeliefPropagation loopyBeliefPropagation.h
   *<agrum/BN/inference/loopyBeliefPropagation.h>
   * @brief class for making Loopy Belief Propagation in bayesian networks.
   * @ingroup bn_inference
   *
   */
  template < typename GUM_SCALAR >
  class LoopyBeliefPropagation : public ApproximateInference< GUM_SCALAR > {
    public:
    /**
     * Default constructor
     */
    explicit LoopyBeliefPropagation(const IBayesNet< GUM_SCALAR >* bn);

    /**
     * Destructor.
     */
    virtual ~LoopyBeliefPropagation();

    protected:
    virtual void _onStateChanged(){};

    virtual void _onEvidenceAdded(const NodeId id, bool isHardEvidence){};

    virtual void _onEvidenceErased(const NodeId id, bool isHardEvidence){};

    virtual void _onAllEvidenceErased(bool contains_hard_evidence){};

    virtual void _onEvidenceChanged(const NodeId id, bool hasChangedSoftHard){};

    virtual void _onBayesNetChanged(const IBayesNet< GUM_SCALAR >* bn){};

    virtual void _updateOutdatedBNStructure();

    virtual void _updateOutdatedBNPotentials(){};

    virtual void _onMarginalTargetAdded(const NodeId id){};

    virtual void _onMarginalTargetErased(const NodeId id){};

    virtual void _onAllMarginalTargetsAdded(){};

    virtual void _onAllMarginalTargetsErased(){};

    /// asks derived classes for the posterior of a given variable
    /** @param id The variable's id. */
    virtual const Potential< GUM_SCALAR >& _posterior(NodeId id);

    virtual void _makeInference();

    // will be used in both directions :
    // for x->y, (x,y) and (y,x) will be in __messages
    ArcProperty< Potential< GUM_SCALAR > >  __messages;
    NodeProperty< Potential< GUM_SCALAR > > __posteriors;

    void __initStats();

    void                    __init_messages();
    Potential< GUM_SCALAR > __computeProdPi(NodeId X);
    Potential< GUM_SCALAR > __computeProdPi(NodeId X, NodeId except);
    Potential< GUM_SCALAR > __computeProdLambda(NodeId X);
    Potential< GUM_SCALAR > __computeProdLambda(NodeId X, NodeId except);

    // return the max differential BNdistance for this node
    GUM_SCALAR __updateNodeMessage(NodeId X);
  };

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  // extern template class LoopyBeliefPropagation<float>;
#  endif
#endif
#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  // extern template class LoopyBeliefPropagation<double>;
#  endif
#endif
} /* namespace gum */

#include <agrum/BN/inference/loopyBeliefPropagation_tpl.h>


#endif   // GUM_LOOPYBELIEFPROPAGATION_H
