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
 * @brief Headers of MultiDimPartialInstantiation.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_MULTI_DIM_PARTIAL_INSTANTIATION_H
#define GUM_MULTI_DIM_PARTIAL_INSTANTIATION_H

#include <utility>

#include <agrum/base/multidim/implementations/multiDimDecorator.h>
#include <agrum/base/multidim/utils/partialInstantiationRegister4MultiDim.h>

namespace gum {

  /**
   * @class MultiDimPartialInstantiation
   * @ingroup multidim_group
   *
   * @warning Doxygen does not like spanning command on multiple line, so we
   * could not configure it with the correct include directive. Use the
   * following code snippet to include this file.
   * @code
   * #include <agrum/base/multidim/implementations/multiDimPartialInstantiation.h>
   * @endcode
   *
   * @brief A generic class to instantiate a subset of variables of a
   * multidimensional table.
   *
   * @tparam GUM_SCALAR The type of scalars stored in this multidimensional
   * table.
   * @tparam Table The class of multidimensional table to use.
   */
  template < typename GUM_SCALAR, template < typename > class TABLE >
  class MultiDimPartialInstantiation {
    public:
    // =========================================================================
    /// @name Constructors, Destructors and Copy
    // =========================================================================
    /// @{

    /**
     * @brief Default constructor.
     */
    MultiDimPartialInstantiation();

    /**
     * @brief Copy constructor.
     * @param src The MultiDimPartialInstantiation to copy.
     */
    MultiDimPartialInstantiation(const MultiDimPartialInstantiation< GUM_SCALAR, TABLE >& src);

    /**
     * @brief Class destructor.
     */
    virtual ~MultiDimPartialInstantiation();

    /**
     * @brief Copy operator.
     * @param src The MultiDimPartialInstantiation to copy.
     * @return Returns this MultiDimPartialInstantiation.
     */
    MultiDimPartialInstantiation< GUM_SCALAR, TABLE >&
        operator=(const MultiDimPartialInstantiation< GUM_SCALAR, TABLE >& src);

    /// @}
    // =========================================================================
    /// @name inherited methdods
    // =========================================================================
    /// @{

    virtual MultiDimPartialInstantiation< GUM_SCALAR, TABLE >* newFactory() const;

    /// @}
    // =========================================================================
    /// @name Accessors/Modifiers
    // =========================================================================
    /// @{

    /**
     * @brief Creates and returns the partial instantiation of the table over a
     * subset of its variables.
     *
     * @warning If inst_vars is precisely equal to the variables of table, the
     * result is an empty table.
     *
     * @param table The table to parially instantiate.
     * @param inst_vars A mapping between variables and their instantiation.
     *
     * @return A new freshly created TABLE which is the result of the
     * instantiation of a subset of the variables of the TABLE passed in
     * argument.
     */
    TABLE< GUM_SCALAR >* instantiate(const TABLE< GUM_SCALAR >&                       table,
                                     const HashTable< const DiscreteVariable*, Idx >& inst_vars);

    /**
     * @brief Creates and returns the partial instantiation of the table over a
     * subset of its variables.
     *
     * @warning If inst_vars is precisely equal to the variables of table, the
     * result is an empty table.
     *
     * @param container Placeholder of the instantiation of table given
     * inst_vars.
     * @param table The table to parially instantiate.
     * @param inst_vars A mapping between variables and their instantiation.
     */
    void instantiate(TABLE< GUM_SCALAR >&                             container,
                     const TABLE< GUM_SCALAR >&                       table,
                     const HashTable< const DiscreteVariable*, Idx >& inst_vars);

    /// @}
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/base/multidim/implementations/multiDimPartialInstantiation_tpl.h>

#endif /* GUM_MULTI_DIM_PARTIAL_INSTANTIATION_H */
