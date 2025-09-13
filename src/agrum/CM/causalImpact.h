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
#ifndef GUM_CAUSAL_IMPACT_H
#define GUM_CAUSAL_IMPACT_H

#include <string>
#include <vector>
#include <memory>

#include <agrum/CM/causalModel.h>
#include <agrum/CM/dSeparation.h>
#include <agrum/CM/doCalculus.h>
#include <agrum/CM/doorCriteria.h>
#include <agrum/CM/doAST.h>
#include <agrum/CM/causalFormula.h>
#include <agrum/CM/hedgeException.h>

namespace gum {

using NameSet           = Set<std::string>;
using VariableName      = std::string;
using VariableValueName = std::string;
using VariableValueId   = Idx;

/**
 * @class CausalImpact
 * @brief Builds a CausalFormula for a query (d-sep -> backdoor -> frontdoor -> (optional) do-calculus).
 *
 * With your CausalFormula’s API (no default ctor, non-assignable), we compute the
 * AST first and initialize `result` in the initializer list (single shot).
 */
template <typename GUM_SCALAR>
class CausalImpact {
public:
  /// Identified symbolic formula. If not identifiable, its AST may be nullptr and
  /// its explanation string (6th ctor arg) will say so.
  CausalFormula<GUM_SCALAR> result;

  // Names interface
  CausalImpact(const CausalModel<GUM_SCALAR>& cm,
               const NameSet&                 on,
               const NameSet&                 doing,
               const NameSet&                 knowing = NameSet{},
               const HashTable<VariableName, VariableValueName>& /*values*/ = HashTable<VariableName, VariableValueName>());

  // IDs interface
  CausalImpact(const CausalModel<GUM_SCALAR>&            cm,
               const NodeSet&                            on,
               const NodeSet&                            doing,
               const NodeSet&                            knowing = NodeSet{},
               const HashTable<NodeId, VariableValueId>& /*values*/ = HashTable<NodeId, VariableValueId>());

private:
  // helpers
  static NameSet _idsToNames_(const CausalModel<GUM_SCALAR>& cm, const NodeSet& ids);
  static NodeSet _namesToIds_(const CausalModel<GUM_SCALAR>& cm, const NameSet& names);
  static bool    _disjoint_(const NameSet& a, const NameSet& b, const NameSet& c);

  // core builders that return the fully-formed formula (for initializer list)
  static CausalFormula<GUM_SCALAR>
  _buildFromNames_(const CausalModel<GUM_SCALAR>& cm,
                   const NameSet&                 on,
                   const NameSet&                 doing,
                   const NameSet&                 knowing);

  static CausalFormula<GUM_SCALAR>
  _buildFromIds_(const CausalModel<GUM_SCALAR>& cm,
                 const NodeSet&                 on,
                 const NodeSet&                 doing,
                 const NodeSet&                 knowing);
};

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class CausalImpact<double>;
#endif

} // namespace gum

#include <agrum/CM/causalImpact_tpl.h>

#endif // GUM_CAUSAL_IMPACT_H
