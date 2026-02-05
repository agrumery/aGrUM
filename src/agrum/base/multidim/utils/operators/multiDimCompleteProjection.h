/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
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
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_MULTI_DIM_COMPLETE_PROJECTION_H
#define GUM_MULTI_DIM_COMPLETE_PROJECTION_H

#include <utility>

#include <agrum/base/multidim/instantiation.h>

namespace gum {

  // clang-format off
  /** @class MultiDimCompleteProjection
   * @headerfile multiDimCompleteProjection.h <agrum/base/multidim/operators/multiDimCompleteProjection.h>
   * @ingroup multidim_op_group
   *
   * @brief A generic class to project efficiently a MultiDim table over all
   * of its variables
   */
  // clang-format on

  template < typename GUM_SCALAR, template < typename > class TABLE >
  class MultiDimCompleteProjection final {
    public:
    // ========================================================================
    /// @name Constructors / Destructors
    // ========================================================================
    /// @{

    /// Default constructor
    MultiDimCompleteProjection(GUM_SCALAR (*proj)(const TABLE< GUM_SCALAR >&, Instantiation*));

    /// Copy constructor
    MultiDimCompleteProjection(const MultiDimCompleteProjection< GUM_SCALAR, TABLE >&);

    /// Destructor
    virtual ~MultiDimCompleteProjection();

    /**
     * @brief Virtual constructor
     *
     * @return a new fresh MultiDimCombinator with the same projection
     * function.
     */
    virtual MultiDimCompleteProjection< GUM_SCALAR, TABLE >* newFactory() const;

    /// @}
    // ========================================================================
    /// @name Accessors/Modifiers
    // ========================================================================
    /// @{

    /// Creates and returns the projection of the table over all of its vars
    GUM_SCALAR project(const TABLE< GUM_SCALAR >& table, Instantiation* instantiation = 0);

    /// Changes the function used for projecting TABLES
    void setProjectFunction(GUM_SCALAR (*proj)(const TABLE< GUM_SCALAR >&, Instantiation*));

    /// Returns the projection function currently used by the projector
    GUM_SCALAR(*projectFunction())
    (const TABLE< GUM_SCALAR >&, Instantiation*);

    /// @}

    protected:
    /// The projection function actually used
    GUM_SCALAR (*proj_)(const TABLE< GUM_SCALAR >&, Instantiation*);

    private:
    /// Forbid copy operators
    MultiDimCompleteProjection< GUM_SCALAR, TABLE >&
        operator=(const MultiDimCompleteProjection< GUM_SCALAR, TABLE >&);
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/base/multidim/utils/operators/multiDimCompleteProjection_tpl.h>

#endif /* GUM_MULTI_DIM_COMPLETE_PROJECTION_H */
