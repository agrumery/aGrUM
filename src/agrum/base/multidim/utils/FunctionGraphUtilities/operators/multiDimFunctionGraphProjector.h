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
 * @brief Class used to compute the projection of a function graph
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_MULTI_DIM_FUNCTION_GRAPH_PROJECTOR_H
#define GUM_MULTI_DIM_FUNCTION_GRAPH_PROJECTOR_H

#include <functional>

#include <agrum/base/multidim/implementations/multiDimFunctionGraph.h>

namespace gum {

  // clang-format off
  /**
   * @class MultiDimFunctionGraphProjector
   * @headerfile multiDimFunctionGraphProjector.h <agrum/base/multidim/patterns/multiDimFunctionGraphProjector.h>
   * @ingroup multidim_group
   *
   * @brief Class used to perform Function Graph projections
   */
  // clang-format on

  template < typename GUM_SCALAR,
             template < typename > class FUNCTOR,
             template < typename > class TerminalNodePolicy = ExactTerminalNodePolicy >
  class MultiDimFunctionGraphProjector {
    public:
    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Default constructor.
     */
    MultiDimFunctionGraphProjector(
        const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* src,
        const gum::VariableSet&                                        delVars,
        const GUM_SCALAR                                               neutral);

    /**
     * @brief Default destructor.
     */
    ~MultiDimFunctionGraphProjector();

    /// @}
    // ============================================================================
    /// @name Main Method
    // ============================================================================
    /// @{

    /**
     * @brief Computes and builds the Function Graph that is the result of the
     * Projection.
     */
    MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* project();

    /// @}

    private:
    /// One of the two function graphs used for the Projection
    const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* _src_;

    /// The list of variables on which the projection is performed
    const gum::VariableSet& _delVars_;

    /// The resulting function graph
    MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* _rd_;

    /// The function to be performed on the leaves
    const FUNCTOR< GUM_SCALAR > _function_;

    /// The function to be performed on the leaves
    const GUM_SCALAR _neutral_;
  };

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class MultiDimFunctionGraphProjector< double, std::plus >;
#endif

}   // namespace gum

#include <agrum/base/multidim/utils/FunctionGraphUtilities/operators/multiDimFunctionGraphProjector_tpl.h>

#endif   // GUM_MULTI_DIM_FUNCTION_GRAPH_PROJECTOR_H
