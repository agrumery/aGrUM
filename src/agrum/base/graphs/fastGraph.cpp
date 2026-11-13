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

#include <algorithm>
#include <cctype>
#include <charconv>

#include <agrum/base/core/fastSyntax.h>
#include <agrum/base/graphs/fastGraph.h>

#include <agrum/base/core/utils_string.h>

namespace gum {

  namespace graph::detail {

    std::vector< std::pair< std::string, FastGraphOp > > fastGraphTokenize(std::string_view chain) {
      std::vector< std::pair< std::string, FastGraphOp > > tokens;
      std::size_t                                          pos       = 0;
      FastGraphOp                                          pendingOp = FastGraphOp::None;

      while (pos <= chain.size()) {
        const auto dashPos = fast::detail::findTopLevel(chain, pos, "-");

        std::size_t opStart = dashPos;
        std::size_t opLen   = 1;
        FastGraphOp op      = FastGraphOp::Edge;

        if (dashPos != std::string_view::npos) {
          if ((dashPos + 1 < chain.size()) && (chain[dashPos + 1] == '>')) {
            opLen = 2;
            op    = FastGraphOp::Arc;
          } else if ((dashPos > pos) && (chain[dashPos - 1] == '<')) {
            opStart = dashPos - 1;
            opLen   = 2;
            op      = FastGraphOp::RevArc;
          }
        }

        const auto tokEnd = (dashPos == std::string_view::npos) ? chain.size() : opStart;

        tokens.emplace_back(trim_copy(chain.substr(pos, tokEnd - pos)), pendingOp);

        if (dashPos == std::string_view::npos) { break; }
        pendingOp = op;
        pos       = opStart + opLen;
      }
      return tokens;
    }

    std::optional< NodeId > fastGraphNodeId(std::string_view token) {
      if (token.empty() || token.find_first_not_of("0123456789") != std::string_view::npos)
        return std::nullopt;
      // in the range of an int: a larger id would make addNodeWithId() reserve every smaller id
      int id = 0;
      if (std::from_chars(token.data(), token.data() + token.size(), id).ec != std::errc())
        return std::nullopt;
      return static_cast< NodeId >(id);
    }

    bool fastGraphIsNodeName(std::string_view token) {
      return fast::detail::findTopLevel(token, 0, " \t\n\r\f\v<>") == std::string_view::npos;
    }

    void fastGraphCheckNodeIds(std::string_view desc, std::vector< NodeId > ids) {
      // same bound as the GUM format (graphGumIO.cpp)
      constexpr Size maxHoles = 65536;
      if (ids.empty()) return;
      std::sort(ids.begin(), ids.end());
      ids.erase(std::unique(ids.begin(), ids.end()), ids.end());
      const Size   nbNodes = ids.size();
      const NodeId maxId   = ids.back();
      // maxId + 1 - nbNodes holes > max(nbNodes, maxHoles)
      if (maxId >= nbNodes + std::max(nbNodes, maxHoles))
        GUM_ERROR(InvalidArgument,
                  "fastGraph: the node id " << maxId << " is too large for " << nbNodes
                                            << " nodes in '" << desc << "' (at most max(" << nbNodes
                                            << ", " << maxHoles
                                            << ") absent ids lower than the largest one)")
    }

  }   // namespace graph::detail

}   // namespace gum
