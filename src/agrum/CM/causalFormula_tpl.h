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
 * @brief Represents an identified causal formula and its query context.
 */

#include <sstream>
#include <algorithm>
#include <vector>

#include <agrum/CM/causalFormula.h>

namespace gum {

template<typename GUM_SCALAR>
CausalFormula<GUM_SCALAR>::CausalFormula(const CausalModel<GUM_SCALAR>& cm,
                                         std::unique_ptr<ASTtree<GUM_SCALAR>> root,
                                         const NameSet& on,
                                         const NameSet& doing,
                                         const NameSet& knowing)
    : _cm(cm),
      _root(std::move(root)),
      _on(on),
      _doing(doing),
      _knowing(knowing){

  _ensureVariablesExist();
}

template<typename GUM_SCALAR>
Tensor<GUM_SCALAR> CausalFormula<GUM_SCALAR>::eval() const {
    return _root->eval(_cm.observationalBN());
}

template<typename GUM_SCALAR>
std::string CausalFormula<GUM_SCALAR>::toString() const {
    return _root->toString();
}

template<typename GUM_SCALAR>
std::string CausalFormula<GUM_SCALAR>::toLatex() const {
    // Initialize occurrence count for variable name hygiene (e.g., for X vs X')
    HashTable<std::string, int> nameOccur;
    for (const auto& name : _on)     nameOccur.insert(name, 1);
    for (const auto& name : _doing)  nameOccur.insert(name, 1);
    for (const auto& name : _knowing) nameOccur.insert(name, 1);

    return latexQuery() + " = " + _root->toLatex(nameOccur);
}

template<typename GUM_SCALAR>
std::string CausalFormula<GUM_SCALAR>::latexQuery() const {
    std::stringstream ss;
    ss << "P\\left(";

    // Sort names for deterministic output
    std::vector<std::string> on_v(_on.begin(), _on.end());
    std::sort(on_v.begin(), on_v.end());

    for (size_t i = 0; i < on_v.size(); ++i) {
        ss << on_v[i] << (i < on_v.size() - 1 ? "," : "");
    }

    if (!_doing.empty() || !_knowing.empty()) {
        ss << " \\mid ";

        // do(...) part
        if (!_doing.empty()) {
            std::vector<std::string> doing_v(_doing.begin(), _doing.end());
            std::sort(doing_v.begin(), doing_v.end());
            ss << "do(" << doing_v[0];
            for (size_t i = 1; i < doing_v.size(); ++i) {
                ss << "," << doing_v[i];
            }
            ss << ")";
        }

        // knowing part
        if (!_knowing.empty()) {
            if (!_doing.empty()) {
                ss << ",";
            }
            std::vector<std::string> knowing_v(_knowing.begin(), _knowing.end());
            std::sort(knowing_v.begin(), knowing_v.end());
            ss << knowing_v[0];
            for (size_t i = 1; i < knowing_v.size(); ++i) {
                ss << "," << knowing_v[i];
            }
        }
    }

    ss << "\\right)";
    return ss.str();
}

template<typename GUM_SCALAR>
void CausalFormula<GUM_SCALAR>::_ensureVariablesExist() const {
  // Collect all variable names from the member sets.
  NameSet all_vars = _on;
  for (const auto& var_name : _knowing) {
      all_vars.insert(var_name);
  }
  for (const auto& var_name : _doing) {
      all_vars.insert(var_name);
  }

  // Check each name against the causal model.
  // The idFromName() method will throw gum::NotFound if a name is invalid.
  for (const auto& var_name : all_vars) {
      _cm.idFromName(var_name);
  }
}

template<typename GUM_SCALAR>
std::unique_ptr<CausalFormula<GUM_SCALAR>> CausalFormula<GUM_SCALAR>::copy() const {
    return std::make_unique<CausalFormula<GUM_SCALAR>>(_cm, _root->copy(), _on, _doing, _knowing);
}

} // namespace gum