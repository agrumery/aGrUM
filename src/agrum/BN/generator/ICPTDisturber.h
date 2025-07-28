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


/** @file
 * @brief Interface-like class for disturbing Conditional Probability Tables.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Ariele-Paolo MAESANO
 */

#ifndef GUM_I_CPT_DISTURBER_H
#define GUM_I_CPT_DISTURBER_H

#include <agrum/agrum.h>

#include <agrum/BN/BayesNet.h>

namespace gum {
  /** @class ICPTDisturber
   * @headerfile ICPTDisturber.h <agrum/BN/generator/ICPTDisturber.h>
   * @brief Abstract class for Modifying Conditional Probability Tables.
   * @ingroup bn_generator
   *
   * This abstract class is an implementation of the strategy pattern to
   * help changing CPT generation policies for the BayesNetGenerator class.
   */
  template < typename GUM_SCALAR >
  class ICPTDisturber {
    public:
    // ############################################################################
    /// @name Constructors / Destructor
    // ############################################################################
    /// @{
    /**
     * Default constructor.
     */
    ICPTDisturber();

    /**
     * Destructor.
     */
    virtual ~ICPTDisturber();
    /// @}

    // ############################################################################
    /// @name CPT disturbing methods
    // ############################################################################
    /**
     * Generates a CPT using GUM_SCALAR.
     * @param node The variable on the CPT owner.
     * @param bayesNet the Bayesian network.
     * @param src copy of the CPT before reduction.
     * @param marg of the inference before reduction on the node varIdi.
     */
    virtual void disturbReducCPT(NodeId                  node,
                                 BayesNet< GUM_SCALAR >& bayesNet,
                                 Tensor< GUM_SCALAR >&   src,
                                 Tensor< GUM_SCALAR >&   marg)
        = 0;

    /**
     * Disturb a CPT using GUM_SCALAR when inserting a new parent.
     * @param node A reference on the CPT owner.
     * @param bayesNet the Bayesian network.
     * @param src copy of the CPT before augmentation.
     * @param variation degree of variation from the initial probability.
     */
    virtual void disturbAugmCPT(NodeId                  node,
                                BayesNet< GUM_SCALAR >& bayesNet,
                                Tensor< GUM_SCALAR >&   src,
                                GUM_SCALAR              variation)
        = 0;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class ICPTDisturber< double >;
#endif


} /* namespace gum */

#include <agrum/BN/generator/ICPTDisturber_tpl.h>
#endif   // GUM_I_CPT_DISTURBER_H
