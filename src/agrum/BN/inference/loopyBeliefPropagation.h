/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief This file contains gibbs sampling (for BNs) class definitions.
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_LOOPYBELIEFPROPAGATION_H
#define GUM_LOOPYBELIEFPROPAGATION_H

#include <agrum/BN/inference/tools/approximateInference.h>

namespace gum {
  /**
   * @class LoopyBeliefPropagation loopyBeliefPropagation.h
   *<agrum/BN/inference/loopyBeliefPropagation.h>
   * @brief class for making Loopy Belief Propagation in Bayesian networks.
   * @ingroup bn_inference
   *
   */
  template < typename GUM_SCALAR >
  class LoopyBeliefPropagation: public ApproximateInference< GUM_SCALAR > {
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
    virtual void onStateChanged_() {};

    virtual void onEvidenceAdded_(const NodeId id, bool isHardEvidence) {};

    virtual void onEvidenceErased_(const NodeId id, bool isHardEvidence) {};

    virtual void onAllEvidenceErased_(bool contains_hard_evidence) {};

    virtual void onEvidenceChanged_(const NodeId id, bool hasChangedSoftHard) {};

    virtual void onModelChanged_(const GraphicalModel* bn) {};

    virtual void updateOutdatedStructure_();

    virtual void updateOutdatedTensors_() {};

    virtual void onMarginalTargetAdded_(const NodeId id) {};

    virtual void onMarginalTargetErased_(const NodeId id) {};

    virtual void onAllMarginalTargetsAdded_() {};

    virtual void onAllMarginalTargetsErased_() {};

    /// asks derived classes for the posterior of a given variable
    /** @param id The variable's id. */
    virtual const Tensor< GUM_SCALAR >& posterior_(NodeId id);

    virtual void makeInference_();

    // will be used in both directions :
    // for x->y, (x,y) and (y,x) will be in  _messages_
    ArcProperty< Tensor< GUM_SCALAR > >  _messages_;
    NodeProperty< Tensor< GUM_SCALAR > > _posteriors_;

    void _initStats_();

    void                    _init_messages_();
    Tensor< GUM_SCALAR > _computeProdPi_(NodeId X);
    Tensor< GUM_SCALAR > _computeProdPi_(NodeId X, NodeId except);
    Tensor< GUM_SCALAR > _computeProdLambda_(NodeId X);
    Tensor< GUM_SCALAR > _computeProdLambda_(NodeId X, NodeId except);

    // return the max differential BNdistance for this node
    GUM_SCALAR _updateNodeMessage_(NodeId X);
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
