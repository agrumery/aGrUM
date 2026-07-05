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
 * @brief This file contains abstract class definitions influence diagrams
 *        inference classes.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Christophe Gonzalez(@
 */
#ifndef GUM_DECISIONTENSOR_H
#define GUM_DECISIONTENSOR_H

#include <agrum/base/multidim/tensor.h>

#include <string_view>

namespace gum {
  /**
   * @class DecisionTensor decisionTensor.h
   *<agrum/ID/inference/decisionTensor.h>
   * @brief Tensor for optimization in LIMIDS (such as Single Policy Update)
   */
  template < GUM_Numeric GUM_SCALAR >
  class DecisionTensor {
    public:
    Tensor< GUM_SCALAR > probPot;
    Tensor< GUM_SCALAR > utilPot;

    explicit DecisionTensor();

    ~DecisionTensor();

    DecisionTensor(const Tensor< GUM_SCALAR >& prob, const Tensor< GUM_SCALAR >& util);

    DecisionTensor(const DecisionTensor< GUM_SCALAR >& dp);

    void clear();

    DecisionTensor< GUM_SCALAR >& operator=(const DecisionTensor< GUM_SCALAR >& src);

    DecisionTensor(DecisionTensor< GUM_SCALAR >&& dp);

    DecisionTensor< GUM_SCALAR >& operator=(DecisionTensor< GUM_SCALAR >&& src);

    bool operator==(const DecisionTensor< GUM_SCALAR >& p) const;

    const DiscreteVariable* variable(std::string_view name) const;

    void insertProba(const gum::Tensor< GUM_SCALAR >& proba);

    void insertUtility(const gum::Tensor< GUM_SCALAR >& util);

    DecisionTensor< GUM_SCALAR > operator*(const DecisionTensor< GUM_SCALAR >& dp1) const;

    DecisionTensor< GUM_SCALAR > operator*=(const DecisionTensor< GUM_SCALAR >& dp1);

    DecisionTensor< GUM_SCALAR > operator^(const gum::VariableSet& onto) const;

    DecisionTensor< GUM_SCALAR > operator^(const std::vector< std::string >& ontonames) const;

    static Tensor< GUM_SCALAR > divideEvenZero(const Tensor< GUM_SCALAR >& p1,
                                               const Tensor< GUM_SCALAR >& p2);

    static DecisionTensor< GUM_SCALAR > combination(const DecisionTensor< GUM_SCALAR >& dp1,
                                                    const DecisionTensor< GUM_SCALAR >& dp2);

    static DecisionTensor< GUM_SCALAR > marginalization(const DecisionTensor< GUM_SCALAR >& dp,
                                                        const gum::VariableSet&             onto);

    static DecisionTensor< GUM_SCALAR >
        marginalization(const DecisionTensor< GUM_SCALAR >& dp,
                        const std::vector< std::string >&   ontonames);

    std::pair< GUM_SCALAR, GUM_SCALAR > meanVar();

    virtual std::string toString() const;
  };

  template < GUM_Numeric GUM_SCALAR >
  std::ostream& operator<<(std::ostream& out, const DecisionTensor< GUM_SCALAR >& array);
}   // namespace gum

#include <agrum/ID/inference/tools/decisionTensor_tpl.h>

#endif   // GUM_DECISIONTENSOR_H
