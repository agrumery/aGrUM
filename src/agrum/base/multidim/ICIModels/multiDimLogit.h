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
 * @brief class for LOGIT implementation as multiDim
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_MULTI_DIM_LOGIT_H
#define GUM_MULTI_DIM_LOGIT_H

#include <agrum/base/multidim/ICIModels/multiDimICIModel.h>

namespace gum {

  /**
   * @class MultiDimLogit
   * @headerfile multiDimLogit.h <agrum/base/multidim/ICIModels/multiDimLogit.h>
   * @ingroup multidim_group
   *
   * @brief Logit representation
   *
   * Logit instantiation of GLM (Generalized Linear Models)
   *
   * @warning
   *   - The first variable is assumed to be the class estimated by a logit.
   *     The latter are the features.
   *   - This code give probabilities for BINARY VARIABLES (other values are
   *     assumed to be of probability 0). But for optimization reason, we will
   *     never check if it is the case.
   */
  template < typename GUM_SCALAR >
  class MultiDimLogit final: public MultiDimICIModel< GUM_SCALAR > {
    public:
    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Default constructor.
     *
     * default_weight is 0 for logit model.
     **/
    MultiDimLogit(GUM_SCALAR external_weight, GUM_SCALAR default_weight = (GUM_SCALAR)0.0);

    MultiDimLogit(const MultiDimLogit< GUM_SCALAR >& from);

    /**
     * Copy constructor using a bijection to swap variables from source.
     *
     * @param bij First variables are new variables, seconds are in from.
     * @param from the copied instance
     */
    MultiDimLogit(const Bijection< const DiscreteVariable*, const DiscreteVariable* >& bij,
                  const MultiDimLogit< GUM_SCALAR >&                                   from);

    /// Destructor.
    virtual ~MultiDimLogit();

    /// @}

    /**
     * This method creates a clone of this object, withouth its content
     * (including variable), you must use this method if you want to ensure
     * that the generated object has the same type than the object containing
     * the called newFactory()
     *
     * For example :
     *   MultiDimArray<double> y;
     *   MultiDimContainer<double>* x = y.newFactory();
     * Then x is a MultiDimArray<double>*
     *
     * @warning you must desallocate by yourself the memory
     * @return an empty clone of this object with the same type
     */
    virtual MultiDimContainer< GUM_SCALAR >* newFactory() const;

    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    public:
    virtual GUM_SCALAR get(const Instantiation& i) const;

    std::string toString() const;

    /**
     * @brief Returns the real name of the multiDimArray.
     *
     * In aGrUM, all the types of multi-dimensional arrays/functionals have a
     * name that describes what they are in reality. For instance, a table
     * stored in extension is a "MultiDimArray", one that stores only non zero
     * elements is a "MultiDimSparseArray", and so on. These names are unique
     * for each type of implementation and is used by the system to determine
     * which is the best functions to use, say, when we wish to use operators
     * such as operator+ on two MultiDimImplementations.
     */
    virtual const std::string& name() const;

    /// @}
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class MultiDimLogit< double >;
#endif

  /// For friendly displaying the content of the array.
  template < typename GUM_SCALAR >
  std::ostream& operator<<(std::ostream& s, const MultiDimLogit< GUM_SCALAR >& ag);

} /* namespace gum */

#include <agrum/base/multidim/ICIModels/multiDimLogit_tpl.h>

#endif /* GUM_MULTI_DIM_LOGIT_H */
