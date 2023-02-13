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
 * @brief This file contains the abstract class definition for computing
 * the probability of evidence entered in Markov random fields
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_MARKOV_RANDOM_FIELD_EVIDENCE_INFERENCE_H
#define GUM_MARKOV_RANDOM_FIELD_EVIDENCE_INFERENCE_H

#include <agrum/MRF/inference/tools/MRFInference.h>
#include <agrum/agrum.h>


namespace gum {


  /**
   * @class EvidenceMRFInference evidenceMRFInference.h
   * <agrum/MRF/inference/evidenceMRFInference.h>
   * @brief A generic class for the computation of the probability of
   * evidence entered in Markov random fields
   * @ingroup mn_group
   *
   * The goal of this class is to provide an interface for the computation
   * of the probability of evidence entered in Markov random fields
   */
  template < typename GUM_SCALAR >
  class EvidenceMRFInference: public virtual MRFInference< GUM_SCALAR > {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @warning note that, by aGrUM's rule, the MRF is not copied but only
     * referenced by the inference algorithm. */
    explicit EvidenceMRFInference(const IMarkovRandomField< GUM_SCALAR >* mn);

    /// destructor
    virtual ~EvidenceMRFInference();

    /// @}


    // ############################################################################
    /// @name Probability computations
    // ############################################################################
    /// @{

    /// returns the probability P(e) of the evidence entered into the MRF
    virtual GUM_SCALAR evidenceProbability() = 0;

    /// @}
  };


}   // namespace gum


#include <agrum/MRF/inference/tools/evidenceMRFInference_tpl.h>


#endif   // GUM_MARKOV_RANDOM_FIELD_EVIDENCE_INFERENCE_H
