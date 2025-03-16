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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/





/** @file
 * @brief Interface-like class for generating Conditional Probability Tables.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6) and
 *Ariele-Paolo MAESANO
 */
#ifndef GUM_I_CPT_GENERATOR_H
#define GUM_I_CPT_GENERATOR_H

#include <agrum/agrum.h>

#include <agrum/base/multidim/tensor.h>

namespace gum {
  /** @class ICPTGenerator
   * @headerfile ICPTGenerator.h <agrum/BN/generator/ICPTGenerator.h>
   * @brief Abstract class for generating Conditional Probability Tables.
   * @ingroup bn_generator
   *
   * This abstract class is an implementation of the strategy pattern to
   * help changing CPT generation policies for the BayesNetGenerator class.
   */
  template < typename GUM_SCALAR >
  class ICPTGenerator {
    public:
    // ############################################################################
    /// @name Constructors / Destructor
    // ############################################################################
    /// @{
    /**
     * Default constructor.
     */
    ICPTGenerator();

    /**
     * Destructor.
     */
    virtual ~ICPTGenerator();
    /// @}

    // ############################################################################
    /// @name CPT generation methods
    // ############################################################################
    /// @{
    /**
     * Generates a CPT using floats.
     * @param varId The variable id of the CPT owner.
     * @param cpt A reference on the CPT to fill.
     */
    virtual void generateCPT(const Idx& varId, const Tensor< GUM_SCALAR >& cpt) const = 0;
    /// @}
  };

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class ICPTGenerator< double >;
#endif

} /* namespace gum */

#include <agrum/BN/generator/ICPTGenerator_tpl.h>
#endif /* GUM_I_CPT_GENERATOR_H */
