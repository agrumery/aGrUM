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
 * @brief Headers of AggregatorDecomposition.
 *
 * @author Gaspard DUCAMP
 */

#ifndef AGRUM_AGGREGATORDECOMPOSITION_H
#define AGRUM_AGGREGATORDECOMPOSITION_H

#include <ctime>

#include <agrum/BN/BayesNet.h>

namespace gum {
  /**
   * @class AggregatorDecomposition aggregatorDecomposition.h
   *<agrum/BN/inference/tools/aggregatorDecomposition.h>
   * @brief This class is used to decompose aggregator in PRM<GUM_SCALAR>.
   */
  template < typename GUM_SCALAR >
  class AggregatorDecomposition {
    public:
    // ========================================================================
    /// @name Constructor & destructor.
    // ========================================================================
    /// @{

    /// Default constructor.
    AggregatorDecomposition();

    /// Destructor.
    virtual ~AggregatorDecomposition();

    BayesNet< GUM_SCALAR >& getDecomposedAggregator(BayesNet< GUM_SCALAR >& bn);

    /// @}
    // ========================================================================
    /// @name Getters & setters.
    // ========================================================================
    /// @{

    virtual std::string name() const;

    gum::Size getMaximumArity();

    void setMaximumArity(gum::Size arity);

    /// @}

    protected:
    // ========================================================================
    /// @name Private evidence handling methods and members.
    // ========================================================================
    /// @{
    NodeId addAggregator_(BayesNet< GUM_SCALAR >& bn,
                          const std::string&      aggType,
                          const DiscreteVariable& var,
                          Idx                     value);

    BayesNet< GUM_SCALAR >& decomposeAggregator_(BayesNet< GUM_SCALAR >& bn, NodeId node);

    Set< NodeId > addDepthLayer_(BayesNet< GUM_SCALAR >& bn,
                                 Set< NodeId >           nodes,
                                 NodeId                  initialAggregator,
                                 int&                    j);
    /// @}

    private:
    Size _arity_ = Size(2);
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class AggregatorDecomposition< double >;
#endif

} /* namespace gum */

#include <agrum/BN/inference/tools/aggregatorDecomposition_tpl.h>

#endif   // AGRUM_AGGREGATORDECOMPOSITION_H
