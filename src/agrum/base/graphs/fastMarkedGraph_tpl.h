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

#pragma once

#include <string>
#include <string_view>

#include <agrum/base/graphs/fastMarkedGraph.h>

namespace gum {

  template < typename GRAPH_TYPE >
    requires std::derived_from< GRAPH_TYPE, MarkedGraph >
  GRAPH_TYPE fastMarkedGraph(std::string_view desc) {
    using namespace graph::detail;

    const auto parsed = fastGraphParseChains(
        desc,
        fastMarkedGraphTokenize,
        [](FastMarkedToken& token) -> std::string& { return token.token; },
        "fastMarkedGraph",
        true);

    GRAPH_TYPE g;
    for (const auto& tokens: parsed.chains) {
      NodeId previous = 0;
      bool   first    = true;
      for (const auto& t: tokens) {
        const NodeId id = fastGraphBuildNode(g, t.token, parsed.useIds);
        if (!first) {
          // addEdge would silently overwrite the marks of an existing edge
          if (g.existsEdge(previous, id)
              && (g.markAt(id, previous) != t.markAtPrevious
                  || g.markAt(previous, id) != t.markAtThis)) {
            GUM_ERROR(InvalidEdge,
                      "fastMarkedGraph: the edge between '"
                          << g.nameFromId(previous) << "' and '" << g.nameFromId(id)
                          << "' is described with conflicting marks in '" << desc << "'")
          }
          g.addEdge(previous, id, t.markAtPrevious, t.markAtThis);
        }
        previous = id;
        first    = false;
      }
    }
    return g;
  }

}   // namespace gum
