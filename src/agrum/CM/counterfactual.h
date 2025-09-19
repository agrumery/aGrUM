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
#ifndef GUM_COUNTERFACTUAL_H
#define GUM_COUNTERFACTUAL_H

#include <ostream>
#include <string>
#include <memory>

#include <agrum/CM/causalImpact.h>

namespace gum {

using NameSet           = Set<std::string>;
using VariableName      = std::string;
using VariableValueName = std::string;
using VariableValueId   = Idx;

template <typename GUM_SCALAR>
class Counterfactual {
public:
  using VarName = VariableName;
  using ValName = VariableValueName;

  // ---------- Names-based constructor ----------
  Counterfactual(const CausalModel<GUM_SCALAR>& cm,
                 const NameSet& on                               = NameSet(),
                 const NameSet& whatif                           = NameSet(),
                 const HashTable<VarName, ValName>& profile      = HashTable<VarName, ValName>(),
                 const HashTable<VarName, ValName>& values       = HashTable<VarName, ValName>());

  // ---------- IDs-based constructor (NodeSet overload) ----------
  Counterfactual(const CausalModel<GUM_SCALAR>& cm,
                 const NodeSet& onIds,
                 const NodeSet& whatifIds,
                 const HashTable<NodeId, VariableValueId>& profileIds = HashTable<NodeId, VariableValueId>(),
                 const HashTable<NodeId, VariableValueId>& valuesIds  = HashTable<NodeId, VariableValueId>());

  // ---------- Twin builders (overloaded like CausalImpact) ----------
  static CausalModel<GUM_SCALAR>
  counterFactualModel(const CausalModel<GUM_SCALAR>& cm,
                      const HashTable<VarName, ValName>& profile,
                      const NameSet& whatif);

  static CausalModel<GUM_SCALAR>
  counterFactualModel(const CausalModel<GUM_SCALAR>& cm,
                      const HashTable<NodeId, VariableValueId>& profileIds,
                      const NodeSet& whatifIds);

  /// Rebuild the internal CausalImpact (ctor already calls this).
  void run();

  // ---------- Accessors ----------
  const CausalModel<GUM_SCALAR>&       originalModel()  const { return _cm; }
  const CausalModel<GUM_SCALAR>&       twinModel()      const { return _twin; }
  const CausalFormula<GUM_SCALAR>&     getResult()      const { return _ciResult(); }
  const Tensor<GUM_SCALAR>&            value()          const { return _adaptedValue; }

  const NameSet&                       on()             const { return _on; }
  const NameSet&                       whatif()         const { return _whatif; }
  const HashTable<VarName,ValName>&    profile()        const { return _profile; }
  const HashTable<VarName,ValName>&    values()         const { return _values; }

  void print(std::ostream& os) const;

private:
  static Tensor<GUM_SCALAR>
  _adaptToOriginalVariables_(const Tensor<GUM_SCALAR>& adj,
                             const CausalModel<GUM_SCALAR>& cm);

  const CausalFormula<GUM_SCALAR>& _ciResult() const;

  static NameSet _idsToNames_(const CausalModel<GUM_SCALAR>& cm, const NodeSet& ids);
  static HashTable<VarName, ValName>
  _idAssignToNameAssign_(const CausalModel<GUM_SCALAR>& cm, const HashTable<NodeId, VariableValueId>& idAssign);

private:
  const CausalModel<GUM_SCALAR>& _cm;
  NameSet                        _on;
  NameSet                        _whatif;
  HashTable<VarName, ValName>    _profile;
  HashTable<VarName, ValName>    _values;

  CausalModel<GUM_SCALAR>        _twin;
  Tensor<GUM_SCALAR>             _adaptedValue;

  std::unique_ptr< CausalImpact<GUM_SCALAR> > _ci;
};

}  // namespace gum

#include "counterfactual_tpl.h"

#endif  // GUM_COUNTERFACTUAL_H
