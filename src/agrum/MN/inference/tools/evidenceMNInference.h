/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief This file contains the abstract class definition for computing
 * the probability of evidence entered in Markov networks
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef GUM_MARKOV_NET_EVIDENCE_INFERENCE_H
#define GUM_MARKOV_NET_EVIDENCE_INFERENCE_H

#include <agrum/MN/inference/tools/MarkovNetInference.h>
#include <agrum/agrum.h>


namespace gum {


  /**
   * @class EvidenceMNInference evidenceMNInference.h
   * <agrum/MN/inference/evidenceMNInference.h>
   * @brief A generic class for the computation of the probability of
   * evidence entered in Markov networks
   * @ingroup mn_group
   *
   * The goal of this class is to provide an interface for the computation
   * of the probability of evidence entered in Markov networks
   */
  template < typename GUM_SCALAR >
  class EvidenceMNInference: public virtual MarkovNetInference< GUM_SCALAR > {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @warning note that, by aGrUM's rule, the MN is not copied but only
     * referenced by the inference algorithm. */
    explicit EvidenceMNInference(const IMarkovNet< GUM_SCALAR >* mn);

    /// destructor
    virtual ~EvidenceMNInference();

    /// @}


    // ############################################################################
    /// @name Probability computations
    // ############################################################################
    /// @{

    /// returns the probability P(e) of the evidence entered into the MN
    virtual GUM_SCALAR evidenceProbability() = 0;

    /// @}
  };


}   // namespace gum


#include <agrum/MN/inference/tools/evidenceMNInference_tpl.h>


#endif   // GUM_MARKOV_NET_EVIDENCE_INFERENCE_H
