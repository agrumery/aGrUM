
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
 * @brief This file contains the abstract class definition for computing
 * the probability of evidence entered in Bayesian networks
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_BAYES_NET_EVIDENCE_INFERENCE_H
#define GUM_BAYES_NET_EVIDENCE_INFERENCE_H

#include <agrum/BN/inference/tools/BayesNetInference.h>
#include <agrum/agrum.h>


namespace gum {


  /**
   * @class EvidenceInference evidenceInference.h
   * <agrum/BN/inference/evidenceInference.h>
   * @brief A generic class for the computation of the probability of
   * evidence entered in Bayesian networks
   * @ingroup bn_group
   *
   * The goal of this class is to provide an interface for the computation
   * of the probability of evidence entered in Bayesian networks
   */
  template < typename GUM_SCALAR >
  class EvidenceInference : public virtual BayesNetInference< GUM_SCALAR > {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @warning note that, by aGrUM's rule, the BN is not copied but only
     * referenced by the inference algorithm. */
    explicit EvidenceInference(const IBayesNet< GUM_SCALAR >* bn);

    /// destructor
    virtual ~EvidenceInference();

    /// @}


    // ############################################################################
    /// @name Probability computations
    // ############################################################################
    /// @{

    /// returns the probability P(e) of the evidence enterred into the BN
    virtual GUM_SCALAR evidenceProbability() = 0;

    /// @}
  };


}   // namespace gum


#include <agrum/BN/inference/tools/evidenceInference_tpl.h>


#endif   // GUM_BAYES_NET_EVIDENCE_INFERENCE_H
