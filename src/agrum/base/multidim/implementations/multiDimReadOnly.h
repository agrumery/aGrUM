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
 * @brief Header of the MultiDimReadOnly class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#ifndef GUM_MULTI_DIM_READONLY_H
#define GUM_MULTI_DIM_READONLY_H

#include <agrum/base/multidim/implementations/multiDimImplementation.h>

namespace gum {

  /**
   * @class MultiDimReadOnly
   * @headerfile multiDimReadOnly.h <agrum/base/multidim/multiDimReadOnly.h>
   * @ingroup multidim_group
   *
   * @brief Abstract base class for all multi dimensionnal read only structure.
   * @tparam GUM_SCALAR The type of scalars stored in the multidimensional
   * table.
   */
  template < typename GUM_SCALAR >
  class MultiDimReadOnly: public MultiDimImplementation< GUM_SCALAR > {
    public:
    // =========================================================================
    /// @name Constructors / Destructors
    // =========================================================================
    /// @{

    /**
     * @brief Default constructor.
     */
    MultiDimReadOnly();

    /**
     * @brief Copy constructor.
     * @param from The MultiDimReadOnly to copy.
     */
    MultiDimReadOnly(const MultiDimReadOnly< GUM_SCALAR >& from);

    /**
     * @brief Class destructor.
     */
    virtual ~MultiDimReadOnly();

    virtual MultiDimContainer< GUM_SCALAR >* newFactory() const = 0;

    /// @}
    // =========================================================================
    /// @name Accessors / Modifiers
    // =========================================================================
    /// @{

    /**
     * @warning An OperationNotAllowed will be raised because this is a read
     * only table.
     */
    virtual void set(const Instantiation& i, const GUM_SCALAR& value) const;

    /**
     * @warning An OperationNotAllowed will be raised because this is a read
     * only table.
     */
    virtual void fill(const GUM_SCALAR&) const;

    /// @}

    protected:
    /**
     * @warning An OperationNotAllowed will be raised because this is a read
     * only table.
     */
    virtual GUM_SCALAR& get_(const Instantiation& i) const;
  };
} /* namespace gum */

#include <agrum/base/multidim/implementations/multiDimReadOnly_tpl.h>

#endif /* GUM_MULTI_DIM_READONLY_H */
