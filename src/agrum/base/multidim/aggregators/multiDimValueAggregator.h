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
 * @brief MultiDimValueAggregator: aggregators computed on the values of the parents
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#ifndef GUM_MULTI_DIM_VALUE_AGGREGATOR_H
#define GUM_MULTI_DIM_VALUE_AGGREGATOR_H

#include <agrum/base/multidim/aggregators/multiDimAggregator.h>

namespace gum {
  namespace aggregator {
    /**
     * @class MultiDimValueAggregator
     * @headerfile multiDimValueAggregator.h
     * <agrum/base/multidim/aggregators/multiDimValueAggregator.h>
     * @ingroup multidim_agg_group
     *
     * @brief Abstract base class for the aggregators computed on the values of the parents
     * (@ref Min, @ref Max, @ref Median, @ref Amplitude, @ref Sum).
     *
     * A concrete class implements buildValue_() with the values of the parents
     * (parentValue_(), i.e. DiscreteVariable::numerical()) and maps its result onto the closest
     * value of the aggregator (valueToIndex_()). The value of a labelized variable is its index:
     * an aggregator mixing labelized and numerical variables is refused (see
     * checkHomogeneousVariable_() and checkNumericalVariable_(), to be called by checkVariable_()).
     */
    template < typename GUM_ELEMENT >
    class MultiDimValueAggregator: public MultiDimAggregator< GUM_ELEMENT > {
      public:
      // =======================================================================
      /// @name Constructors / Destructors
      // =======================================================================
      /// @{

      /// Default constructor.
      MultiDimValueAggregator();

      /// Copy constructor.
      MultiDimValueAggregator(const MultiDimValueAggregator< GUM_ELEMENT >& from);

      /// Class destructor.
      ~MultiDimValueAggregator() override;

      /// @}

      protected:
      /// @throw TypeError if v is not numerical (see DiscreteVariable::isNumerical())
      void checkNumericalVariable_(const DiscreteVariable& v) const;

      /// @throw TypeError if v is numerical and the variables already in the aggregator are not
      /// (or conversely)
      void checkHomogeneousVariable_(const DiscreteVariable& v) const;

      /// @return the numerical value of the j-th variable (j>=1: a parent) in i
      double parentValue_(const gum::Instantiation& i, Idx j) const;

      /**
       * @brief Maps a value onto an index of the aggregator.
       *
       * Numerical aggregator: DiscreteVariable::closestIndex() (saturated at both bounds).
       * Labelized aggregator: the value is an index (the numerical value of a labelized variable
       * is its index), rounded (half down) and saturated at both bounds.
       */
      Idx valueToIndex_(double value) const;
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class GUM_SHARED_PUBLIC MultiDimValueAggregator< double >;
#endif

  } /* namespace aggregator */
} /* namespace gum */

#include <agrum/base/multidim/aggregators/multiDimValueAggregator_tpl.h>

#endif /* GUM_MULTI_DIM_VALUE_AGGREGATOR_H */
