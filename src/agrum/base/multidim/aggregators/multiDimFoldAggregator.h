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
 * @brief MultiDimFoldAggregator: aggregators computed by folding the indices of the parents
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#ifndef GUM_MULTI_DIM_FOLD_AGGREGATOR_H
#define GUM_MULTI_DIM_FOLD_AGGREGATOR_H

#include <agrum/base/multidim/aggregators/multiDimAggregator.h>

namespace gum {
  namespace aggregator {
    /**
     * @class MultiDimFoldAggregator
     * @headerfile multiDimFoldAggregator.h
     * <agrum/base/multidim/aggregators/multiDimFoldAggregator.h>
     * @ingroup multidim_agg_group
     *
     * @brief Abstract base class for the aggregators computed by a "fold" on the indices of the
     * parents (@ref Count, @ref Exists, @ref Forall, @ref And, @ref Or).
     *
     * \f$P[i,J_1,\cdots,J_n]=1 \iff
     * i==f(J_1,f(J_2,\cdots,f(J_n,NeutraElement)\cdots))\f$ and 0 otherwise.
     * f is a virtual function called the folder function (like folder_left or
     * folder_right in OCaml). NeutralElement is an Idx.
     *
     * A concrete class implements neutralElt_() and fold_(); the result is truncated by get() in
     * order to fit in the domain of the aggregator variable.
     */
    template < typename GUM_ELEMENT >
    class MultiDimFoldAggregator: public MultiDimAggregator< GUM_ELEMENT > {
      public:
      // =======================================================================
      /// @name Constructors / Destructors
      // =======================================================================
      /// @{

      /// Default constructor.
      MultiDimFoldAggregator();

      /// Copy constructor.
      MultiDimFoldAggregator(const MultiDimFoldAggregator< GUM_ELEMENT >& from);

      /// Class destructor.
      ~MultiDimFoldAggregator() override;

      /// @}

      protected:
      /// computes f(J_1,f(J_2,...,f(J_n,neutralElt_())...)) with fold_() (stops when fold_()
      /// sets stop_iteration)
      Idx buildValue_(const gum::Instantiation& i) const final;

      /// neutralElt_() is the result value for the first application of fold_
      virtual Idx neutralElt_() const = 0;

      /// fold_ is applied on value i1 for variable v. the actual result for
      /// precedent applications is i2.
      /// @return the new result for applications up to v.
      virtual Idx fold_(const DiscreteVariable& v, Idx i1, Idx i2, bool& stop_iteration) const = 0;
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class GUM_SHARED_PUBLIC MultiDimFoldAggregator< double >;
#endif

  } /* namespace aggregator */
} /* namespace gum */

#include <agrum/base/multidim/aggregators/multiDimFoldAggregator_tpl.h>

#endif /* GUM_MULTI_DIM_FOLD_AGGREGATOR_H */
