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
 * @brief Headers of MultiDimSparse.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_MULTI_DIM_SPARSE_H
#define GUM_MULTI_DIM_SPARSE_H

#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/multidim/implementations/multiDimWithOffset.h>

namespace gum {

  // ===========================================================================
  // ===                          GUM_MULTI_DIM_ARRAY                        ===
  // ===========================================================================
  /**
   * @class MultiDimSparse
   * @headerfile multiDimSparse.h <agrum/base/multidim/multiDimSparse.h>
   * @ingroup multidim_group
   *
   * @brief Multidimensional matrix stored as a sparse array in memory.
   * @tparam GUM_SCALAR The type of scalars stored in the multidimensional
   * table.
   */
  template < typename GUM_SCALAR >
  class MultiDimSparse final: public MultiDimWithOffset< GUM_SCALAR > {
    public:
    // =========================================================================
    /// @name Constructors / Destructors
    // =========================================================================
    /// @{

    /**
     * @brief Default constructor: creates an empty null dimensional matrix.
     * @param default_value The default value of this MultiDimSparse.
     */
    explicit MultiDimSparse(const GUM_SCALAR& default_value);

    /**
     * @brief Copy constructor.
     *
     * The newly created matrix contains the same variables and the same values
     * as from, but no instantiation is associated to it.
     *
     * @param from The MultiDimSparse to copy.
     */
    MultiDimSparse(const MultiDimSparse< GUM_SCALAR >& from);

    /**
     * @todo operator=
     *
     * @brief Copy operator.
     * @param from The MultiDimSparse to copy.
    MultiDimSparse<GUM_SCALAR>& operator=( const MultiDimSparse<GUM_SCALAR>&
    from );
     */

    /**
     * @brief Class destructor.
     */
    virtual ~MultiDimSparse();


    /// @}
    // =========================================================================
    /// @name Inherited methods
    // =========================================================================
    /// @{

    virtual MultiDimContainer< GUM_SCALAR >* newFactory() const;

    void add(const DiscreteVariable& v);

    void erase(const DiscreteVariable& v);

    virtual void fill(const GUM_SCALAR& d) const;

    virtual GUM_SCALAR get(const Instantiation& i) const;

    virtual void set(const Instantiation& i, const GUM_SCALAR& value) const;

    virtual const std::string& name() const;

    virtual Size realSize() const;

    /// @}

    protected:
    /// The true data : the values is mutable since we can change the value
    /// in a const multiDimArray
    mutable HashTable< Size, GUM_SCALAR > params_;

    /// The default value.
    mutable GUM_SCALAR default_;

    /// Synchronise content after MultipleChanges.
    virtual void commitMultipleChanges_();

    /**
     * @warning Will raise an OperationNotAllowed as this use its own internal
     * datastructure.
     */
    virtual GUM_SCALAR& get_(const Instantiation& i) const;

    // =========================================================================
    /// @name Inherited methods
    // =========================================================================
    /// @{

    virtual void replace_(const DiscreteVariable* x, const DiscreteVariable* y);

    /// @}
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class MultiDimSparse< double >;
#endif


} /* namespace gum */

#include <agrum/base/multidim/implementations/multiDimSparse_tpl.h>

#endif /* GUM_MULTI_DIM_SPARSE_H */
