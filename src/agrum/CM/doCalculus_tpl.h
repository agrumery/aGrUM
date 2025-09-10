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
 * @brief Do-calculus utilities and AST builders bound to a single CausalModel.
 *
 * This class mirrors the layout of pyAgrum's `_doCalculus.py`: it provides
 * helpers to construct adjustment formulas (backdoor/frontdoor) as ASTs and
 * the public entry points for general identification à la Shpitser–Pearl.
 */

 /*
 * Template implementation for gum::DoCalculus
 * Keep this file included at the end of doCalculus.h
 */


#include <algorithm>

#include <agrum/CM/doCalculus.h>

namespace gum {

/* ========================================================================== */
/*                              Backdoor / Frontdoor                          */
/* ========================================================================== */

template <typename GUM_SCALAR>
typename DoCalculus<GUM_SCALAR>::FormulaPtr
DoCalculus<GUM_SCALAR>::getBackDoorTree(NodeId cause,
                                        NodeId effect,
                                        const NodeSet& zset) const {
  // Preconditions: cause/effect must be observed (non-latent), Z must not contain latents.
  const NodeSet lat = _cm.latentVariablesIds();
  if (lat.contains(cause) || lat.contains(effect)) {
    GUM_ERROR(InvalidArgument, "getBackDoorTree: 'cause' and 'effect' must be observed (non-latent).");
  }
  for (auto z : zset) {
    if (lat.contains(z)) {
      GUM_ERROR(InvalidArgument, "getBackDoorTree: zset contains a latent variable.");
    }
  }

  // ids -> names
  const auto&        bn    = _cm.observationalBN();
  const std::string  xName = _cm.nameFromId(cause);
  const std::string  yName = _cm.nameFromId(effect);

  Set<std::string>        zNames;
  std::vector<std::string> zOrder;
  zOrder.reserve(zset.size());
  for (auto z : zset) {
    const std::string zn = _cm.nameFromId(z);
    zNames.insert(zn);
    zOrder.push_back(zn);
  }
  std::sort(zOrder.begin(), zOrder.end());

  // Build P(y | x, z)
  Set<std::string> lhsY;    lhsY.insert(yName);
  Set<std::string> knwYXZ;  knwYXZ.insert(xName);
  for (const auto& zn : zNames) knwYXZ.insert(zn);
  auto Pyxz = std::make_unique<ASTposteriorProba<GUM_SCALAR>>(bn, lhsY, knwYXZ);

  if (zNames.empty()) {
    // No adjustment: returns P(y | x)
    return Pyxz;
  }

  // Build P(z)
  auto Pz = std::make_unique<ASTjointProba<GUM_SCALAR>>(zNames);

  // Product P(y|x,z) · P(z), then sum over Z in sorted order
  std::vector<std::unique_ptr<ASTtree<GUM_SCALAR>>> terms;
  terms.emplace_back(std::move(Pyxz));
  terms.emplace_back(std::move(Pz));
  auto prod = productOfTrees<GUM_SCALAR>(std::move(terms));

  return std::make_unique<ASTsum<GUM_SCALAR>>(zOrder, std::move(prod));
}

template <typename GUM_SCALAR>
typename DoCalculus<GUM_SCALAR>::FormulaPtr
DoCalculus<GUM_SCALAR>::getBackDoorTree(const std::string& causeName,
                                        const std::string& effectName,
                                        const NameList&    zNames) const {
  const NodeId x = _cm.idFromName(causeName);
  const NodeId y = _cm.idFromName(effectName);

  NodeSet Z;
  for (const auto& zn : zNames) Z.insert(_cm.idFromName(zn));

  return getBackDoorTree(x, y, Z);
}

template <typename GUM_SCALAR>
typename DoCalculus<GUM_SCALAR>::FormulaPtr
DoCalculus<GUM_SCALAR>::getFrontDoorTree(NodeId cause,
                                         NodeId effect,
                                         const NodeSet& zset) const {
  // --- Preconditions ---
  const NodeSet lat = _cm.latentVariablesIds();
  if (lat.contains(cause) || lat.contains(effect)) {
    GUM_ERROR(InvalidArgument, "getFrontDoorTree: 'cause' and 'effect' must be observed (non-latent).");
  }
  if (zset.empty()) {
    GUM_ERROR(InvalidArgument, "getFrontDoorTree: zset must be non-empty for frontdoor.");
  }
  if (zset.contains(cause) || zset.contains(effect)) {
    GUM_ERROR(InvalidArgument, "getFrontDoorTree: zset must be disjoint from {cause,effect}.");
  }
  for (auto z : zset) {
    if (lat.contains(z)) {
      GUM_ERROR(InvalidArgument, "getFrontDoorTree: zset contains a latent variable.");
    }
  }

  // --- DAG + bijection (observed only) ---
  const DAG& dag = _cm.causalDAG();
  auto id2nm     = _cm.id2name(/*includeLatentVariables=*/false);

  // --- ids → names ---
  const std::string xName = _cm.nameFromId(cause);
  const std::string yName = _cm.nameFromId(effect);

  Set<std::string>         zNames;
  std::vector<std::string> zOrder;
  zOrder.reserve(zset.size());
  for (auto z : zset) {
    const auto zn = _cm.nameFromId(z);
    zNames.insert(zn);
    zOrder.push_back(zn);
  }
  std::sort(zOrder.begin(), zOrder.end()); // deterministic summation/printing

  // --- Build P(Z | X) using DAG ctor ---
  Set<std::string> condX; condX.insert(xName);
  auto Pz_given_x = std::make_unique<ASTposteriorProba<GUM_SCALAR>>(dag, id2nm, zNames, condX);

  // --- Build sum_x P(Y | X, Z) * P(X) ---
  Set<std::string> ySet; ySet.insert(yName);

  Set<std::string> condXZ = zNames; // copy Z
  condXZ.insert(xName);
  auto Py_given_xz = std::make_unique<ASTposteriorProba<GUM_SCALAR>>(dag, id2nm, ySet, condXZ);

  Set<std::string> xSet; xSet.insert(xName);
  auto Px = std::make_unique<ASTjointProba<GUM_SCALAR>>(xSet);

  auto inner_prod = std::make_unique<ASTmult<GUM_SCALAR>>(std::move(Py_given_xz), std::move(Px));
  auto inner_sum  = std::make_unique<ASTsum<GUM_SCALAR>>(std::vector<std::string>{xName}, std::move(inner_prod));

  // --- P(Z|X) * (sum_x ...) then sum over Z ---
  auto outer_prod = std::make_unique<ASTmult<GUM_SCALAR>>(std::move(Pz_given_x), std::move(inner_sum));
  return std::make_unique<ASTsum<GUM_SCALAR>>(zOrder, std::move(outer_prod));
}

template <typename GUM_SCALAR>
typename DoCalculus<GUM_SCALAR>::FormulaPtr
DoCalculus<GUM_SCALAR>::getFrontDoorTree(const std::string& causeName,
                                         const std::string& effectName,
                                         const NameList&    zNames) const {
  const NodeId x = _cm.idFromName(causeName);
  const NodeId y = _cm.idFromName(effectName);

  NodeSet Z;
  for (const auto& zn : zNames) Z.insert(_cm.idFromName(zn));

  return getFrontDoorTree(x, y, Z);
}

/* ========================================================================== */
/*                           General identification (stubs)                   */
/* ========================================================================== */

template <typename GUM_SCALAR>
typename DoCalculus<GUM_SCALAR>::FormulaPtr
DoCalculus<GUM_SCALAR>::identifyingIntervention(const NodeSet& Y,
                                                const NodeSet& X) const {
  // Placeholder for Shpitser–Pearl ID algorithm over the bound model.
  GUM_ERROR(NotImplementedYet,
            "DoCalculus::identifyingIntervention: general do-calculus "
            "identification is not implemented yet.");
  return nullptr; // Unreachable, satisfies some compilers.
}

template <typename GUM_SCALAR>
typename DoCalculus<GUM_SCALAR>::FormulaPtr
DoCalculus<GUM_SCALAR>::identifyingIntervention(const NameList& Y,
                                                const NameList& X) const {
  NodeSet YY;
  for (const auto& y : Y) YY.insert(_cm.idFromName(y));

  NodeSet XX;
  for (const auto& x : X) XX.insert(_cm.idFromName(x));

  return identifyingIntervention(YY, XX);
}

template <typename GUM_SCALAR>
typename DoCalculus<GUM_SCALAR>::FormulaPtr
DoCalculus<GUM_SCALAR>::doCalculus(const NodeSet& on,
                                   const NodeSet& doing) const {
  return identifyingIntervention(on, doing);
}

template <typename GUM_SCALAR>
typename DoCalculus<GUM_SCALAR>::FormulaPtr
DoCalculus<GUM_SCALAR>::doCalculus(const NameList& on,
                                   const NameList& doing) const {
  return identifyingIntervention(on, doing);
}

template <typename GUM_SCALAR>
typename DoCalculus<GUM_SCALAR>::FormulaPtr
DoCalculus<GUM_SCALAR>::doCalculusWithObservation(const NodeSet& on,
                                                  const NodeSet& doing,
                                                  const NodeSet& knowing) const {
  // You may later implement the ratio trick here:
  //   P(on,knowing | do(doing)) / P(knowing | do(doing)),
  // but for now keep a clear "not implemented" signal.
  if (knowing.empty()) return doCalculus(on, doing);

  GUM_ERROR(NotImplementedYet,
            "DoCalculus::doCalculusWithObservation: conditional interventional "
            "queries are not implemented yet.");
  return nullptr; // Unreachable, satisfies some compilers.
}

template <typename GUM_SCALAR>
typename DoCalculus<GUM_SCALAR>::FormulaPtr
DoCalculus<GUM_SCALAR>::doCalculusWithObservation(const NameList& on,
                                                  const NameList& doing,
                                                  const NameList& knowing) const {
  NodeSet ON;
  NodeSet DOING;
  NodeSet K;

  for (const auto& s : on)     ON.insert(_cm.idFromName(s));
  for (const auto& s : doing)  DOING.insert(_cm.idFromName(s));
  for (const auto& s : knowing) K.insert(_cm.idFromName(s));

  return doCalculusWithObservation(ON, DOING, K);
}

} // namespace gum
