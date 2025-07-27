/****************************************************************************
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
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief This file contains the abstract class definition for computing
 * the probability of evidence entered in Markov random fields
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_MARKOV_RANDOM_FIELD_EVIDENCE_INFERENCE_H
#define GUM_MARKOV_RANDOM_FIELD_EVIDENCE_INFERENCE_H

#include <agrum/agrum.h>

#include <agrum/MRF/inference/tools/MRFInference.h>

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
