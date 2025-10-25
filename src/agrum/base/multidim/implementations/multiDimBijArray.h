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
 * @brief Headers of the MultiDimBijArray class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 * @author Lionel TORTI
 */

#ifndef GUM_MULTIDIMBIJARRAY_H
#define GUM_MULTIDIMBIJARRAY_H

#include <agrum/base/core/bijection.h>
#include <agrum/base/multidim/implementations/multiDimArray.h>

#include <initializer_list>

namespace gum {

  /**
   * @class MultiDimBijArray
   * @headerfile multiDimBijArray.h <agrum/base/multidim/multiDimBijArray.h>
   * @ingroup multidim_group
   *
   * @brief Decorator of a MultiDimArray, using a bijection over the variables.
   *
   * @tparam GUM_SCALAR The type of scaler stored in this multidimensional
   * table.
   */
  template < typename GUM_SCALAR >
  class MultiDimBijArray final: public MultiDimWithOffset< GUM_SCALAR > {
    public:
    using VarBijection = Bijection< const DiscreteVariable*, const DiscreteVariable* >;

    // =========================================================================
    /// @name Constructors / Destructors
    // =========================================================================
    /// @{

    /**
     * @brief Class constructor.
     *
     * @param bijection The bijection between variables in array and variable
     * in this.
     * @param array The MultiDimArray decorated by this MultiDimBijArray.
     */
    MultiDimBijArray(const VarBijection& bijection, const MultiDimArray< GUM_SCALAR >& array);

    /**
     * @brief Class constructor.
     *
     * @param bijection The bijection between variables in array and variable
     * in this.
     * @param array The MultiDimBijArray decorated by this MultiDimBijArray.
     */
    MultiDimBijArray(const VarBijection& bijection, const MultiDimBijArray< GUM_SCALAR >& array);

    /**
     * @brief Copy constructor.
     *
     * The newly created matrix contains the same variables and the same values
     * as from, but no instantiation is associated to it.
     *
     * @param from The MultiDimBijArray to copy.
     */
    MultiDimBijArray(const MultiDimBijArray< GUM_SCALAR >& from);

    /**
     * @brief Class destructor.
     */
    virtual ~MultiDimBijArray();

    /// @}
    // ========================================================================
    /// @name Modifiers -- will raise OperationNotAllowed exceptions
    // ========================================================================
    /// @{
    /**
     * @warning This will raise an exception: read only structure.
     * @throw OperationNotAllowed Raised since you can't change a readonly
     * structure.
     */
    MultiDimBijArray< GUM_SCALAR >& operator=(const MultiDimBijArray< GUM_SCALAR >& from);

    /**
     * @warning This will raise an exception: read only structure.
     * @throw OperationNotAllowed Raised since you can't change a readonly
     * structure.
     */
    virtual void set(const Instantiation& i, const GUM_SCALAR& value) const;

    /**
     * This will raise an exception: you can't change the variables in a
     * MultiDimBijArray.
     * @param v The variable not added.
     * @throw OperationNotAllowed You can't add variable in a MultiDimBijArray.
     */
    virtual void add(const DiscreteVariable& v);

    /**
     * This will raise an exception: you can't change the variables in a
     * MultiDimBijArray.
     * @param v The variable not added.
     * @throw OperationNotAllowed You can't add variable in a MultiDimBijArray.
     */
    virtual void erase(const DiscreteVariable& v);

    /**
     * This will raise an exception: you can't change the data
     * @param d the value changed
     * @throw OperationNotAllowed You can't change data.
     */
    virtual void fill(const GUM_SCALAR& d) const;

    /**
     * This will raise an exception: you can't change the variables in a
     * MultiDimBijArray.
     * @param v The variable not added.
     * @throw OperationNotAllowed You can't add variable in a MultiDimBijArray.
     */
    virtual void populate(const std::vector< GUM_SCALAR >& v) const;

    /**
     * This will raise an exception: you can't change the variables in a
     * MultiDimBijArray.
     * @param l The variable not added.
     * @throw OperationNotAllowed You can't add variable in a MultiDimBijArray.
     */
    virtual void populate(std::initializer_list< GUM_SCALAR > l) const;

    /// @}
    // ========================================================================
    /// @name Inherited methods
    // ========================================================================
    /// @{

    virtual const std::string& name() const;

    virtual GUM_SCALAR get(const Instantiation& i) const;

    virtual Size realSize() const;

    virtual MultiDimBijArray< GUM_SCALAR >* newFactory() const;

    /// @}

    protected:
    virtual GUM_SCALAR& get_(const Instantiation& i) const;

    virtual void commitMultipleChanges_();

    virtual void replace_(const DiscreteVariable* x, const DiscreteVariable* y);

    private:
    /// The true data.
    const MultiDimArray< GUM_SCALAR >& _array_;

    /// The class name.
    std::string _name_;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class MultiDimBijArray< double >;
#endif

}   // namespace gum

#include <agrum/base/multidim/implementations/multiDimBijArray_tpl.h>

#endif   // GUM_MULTIDIMBIJARRAY_H
