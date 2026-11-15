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


/** @file
 * @brief Template implementation of the fast description of a graphical model.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#pragma once

#include <algorithm>
#include <string>
#include <string_view>
#include <vector>

#include <agrum/base/core/fastSyntax.h>
#include <agrum/base/graphicalModels/fastModel.h>

namespace gum::fast::detail {

  template < typename MODEL, typename PREFIX >
  std::string chainsToFast(const MODEL&                                model,
                           const std::vector< std::vector< NodeId > >& chains,
                           std::string_view                            link,
                           PREFIX                                      prefix) {
    NodeSet     written;
    std::string fast;
    // a node is written in full at its first occurrence, by its name afterwards
    auto write = [&](NodeId node) {
      const auto& var = model.variable(node);
      if (written.exists(node)) return quoteFastName(var.name());
      written.insert(node);
      return prefix(node) + var.toFast();
    };
    auto append = [&](const std::string& chain) {
      if (!fast.empty()) fast += ';';
      fast += chain;
    };
    for (const auto& nodes: chains) {
      std::string chain;
      for (const auto node: nodes) {
        if (!chain.empty()) chain += link;
        chain += write(node);
      }
      append(chain);
    }
    for (const auto node: model.nodes())
      if (!written.exists(node)) append(write(node));
    return fast;
  }

  template < typename MODEL, typename PREFIX >
  std::string directedModelToFast(const MODEL& model, PREFIX prefix) {
    // in the order of the NodeIds: the description does not depend on the hash tables
    std::vector< std::vector< NodeId > > arcs;
    for (const auto& arc: model.arcs())
      arcs.push_back({arc.tail(), arc.head()});
    std::sort(arcs.begin(), arcs.end());
    return chainsToFast(model, arcs, "->", prefix);
  }

}   // namespace gum::fast::detail
