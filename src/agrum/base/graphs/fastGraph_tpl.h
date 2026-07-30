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

#include <algorithm>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include <agrum/base/core/utils_string.h>
#include <agrum/base/graphs/fastGraph.h>

namespace gum {

  namespace detail {

    /// which operator (if any) precedes a node token in a fastGraph chain
    enum class FastGraphOp : std::uint8_t { None, Arc, RevArc, Edge };

    /**
     * @brief Scans one ";"-delimited chain, splitting it into node tokens
     * while remembering, for each token, whether it was preceded by "->"
     * (Arc), "<-" (RevArc) or "-" (Edge). A plain split() cannot preserve
     * this, since it only handles one delimiter at a time.
     *
     * "-" (not "--") is used for edges: MRF's own fast syntax already uses
     * "--" to list the variables of a single factor (a clique), which is a
     * different construct from a chain of pairwise edges. Reusing "--" here
     * would silently give the same token two incompatible meanings.
     */
    inline std::vector< std::pair< std::string, FastGraphOp > >
        fastGraphTokenize(std::string_view chain) {
      std::vector< std::pair< std::string, FastGraphOp > > tokens;
      std::size_t                                          pos       = 0;
      FastGraphOp                                          pendingOp = FastGraphOp::None;

      while (pos <= chain.size()) {
        const auto dashPos = chain.find('-', pos);

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

    /// true if token must be read as a NodeId (non-negative integer)
    inline bool fastGraphIsNodeIdToken(const std::string& token) {
      int value = 0;
      return isIntegerWithResult(token, &value) && (value >= 0);
    }

    /// gets or creates the node denoted by token, in NodeId or name mode
    template < typename GRAPH_TYPE >
    NodeId fastGraphBuildNode(GRAPH_TYPE& g, const std::string& token, bool useIds) {
      if (useIds) {
        int value = 0;
        isIntegerWithResult(token, &value);
        const auto id = static_cast< NodeId >(value);
        if (!g.existsNode(id)) { g.addNodeWithId(id); }
        return id;
      }

      if (const auto id = g.idFromName(token)) { return *id; }
      const NodeId id = g.addNode();
      g.setName(id, token);
      return id;
    }

  }   // namespace detail

  template < GUM_NodeGraphable GRAPH_TYPE >
  GRAPH_TYPE fastGraph(std::string_view desc) {
    using namespace detail;

    std::vector< std::vector< std::pair< std::string, FastGraphOp > > > chains;
    bool                                                                useIds = true;

    for (const auto& chainStr: split(remove_newline(desc), ";")) {
      if (trim_copy(chainStr).empty()) { continue; }

      auto tokens = fastGraphTokenize(chainStr);
      for (const auto& [token, op]: tokens) {
        if (token.empty()) {
          GUM_ERROR(InvalidArc,
                    "fastGraph: malformed description '"
                        << desc << "' (operator without a node token)")
        }
        if (!fastGraphIsNodeIdToken(token)) { useIds = false; }
      }
      chains.push_back(std::move(tokens));
    }

    GRAPH_TYPE g;

    for (const auto& tokens: chains) {
      NodeId lastId = 0;
      bool   first  = true;

      for (const auto& [token, op]: tokens) {
        const NodeId id = fastGraphBuildNode(g, token, useIds);

        if (!first) {
          if (op == FastGraphOp::Arc) {
            if constexpr (GUM_DiGraphable< GRAPH_TYPE >) {
              g.addArc(lastId, id);
            } else {
              GUM_ERROR(InvalidArc,
                        "fastGraph: '" << token << "' is preceded by '->' but the "
                                       << "requested graph type does not support arcs")
            }
          } else if (op == FastGraphOp::RevArc) {
            if constexpr (GUM_DiGraphable< GRAPH_TYPE >) {
              g.addArc(id, lastId);
            } else {
              GUM_ERROR(InvalidArc,
                        "fastGraph: '" << token << "' is preceded by '<-' but the "
                                       << "requested graph type does not support arcs")
            }
          } else {
            if constexpr (GUM_UndiGraphable< GRAPH_TYPE >) {
              g.addEdge(lastId, id);
            } else {
              GUM_ERROR(InvalidEdge,
                        "fastGraph: '" << token << "' is preceded by '-' but the "
                                       << "requested graph type does not support edges")
            }
          }
        }

        lastId = id;
        first  = false;
      }
    }

    return g;
  }

}   // namespace gum
