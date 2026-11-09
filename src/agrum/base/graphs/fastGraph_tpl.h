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
#include <string>
#include <utility>
#include <vector>

#include <agrum/base/graphs/fastGraph.h>

#include <agrum/base/core/utils_string.h>

namespace gum {

  namespace graph::detail {

    /// the chains of tokens of a description, and whether their tokens are NodeIds
    template < typename TOKEN >
    struct FastGraphChains {
      std::vector< std::vector< TOKEN > > chains;
      bool                                useIds = true;
    };

    /**
     * @brief Splits a description into its ";"-delimited chains of tokens and decides how to
     * read the nodes: as NodeIds if every token is a non-negative integer, else as names.
     *
     * @param desc the description.
     * @param tokenize `std::vector< TOKEN > tokenize(std::string_view chain)`.
     * @param tokenName `const std::string& tokenName(const TOKEN&)`: the node name of a token.
     * @param onEmptyToken called for an operator without a node token: must throw.
     */
    template < typename TOKENIZE, typename TOKEN_NAME, typename ON_EMPTY_TOKEN >
    auto fastGraphParseChains(std::string_view desc,
                              TOKENIZE         tokenize,
                              TOKEN_NAME       tokenName,
                              ON_EMPTY_TOKEN   onEmptyToken) {
      FastGraphChains< typename decltype(tokenize(desc))::value_type > parsed;
      for (const auto& chainStr: split(remove_newline(desc), ";")) {
        if (trim_copy(chainStr).empty()) { continue; }
        auto tokens = tokenize(chainStr);
        for (const auto& token: tokens) {
          const std::string& name = tokenName(token);
          if (name.empty()) { onEmptyToken(); }
          if (!fastGraphIsNodeIdToken(name)) { parsed.useIds = false; }
        }
        parsed.chains.push_back(std::move(tokens));
      }
      return parsed;
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

    /**
     * @brief Walks one already-tokenized chain (see fastGraphTokenize()), resolving each token
     * to a NodeId via @p resolve and wiring consecutive nodes via @p addArc / @p addEdge
     * according to the operator preceding each token.
     *
     * This is the shared topology grammar behind fastGraph(), BayesNet::fastPrototype() and
     * InfluenceDiagram::fastPrototype(): what a "node" is (a bare graph node, a chance/utility/
     * decision DiscreteVariable, ...) is entirely delegated to @p resolve, so callers with no
     * notion of undirected edges (BayesNet, InfluenceDiagram) can simply pass an @p addEdge that
     * throws.
     *
     * @param tokens the chain, as produced by fastGraphTokenize().
     * @param desc the full original description, only used to report errors.
     * @param resolve `NodeId resolve(const std::string& token)`: gets or creates the node
     *        denoted by a raw token.
     * @param addArc `void addArc(NodeId tail, NodeId head, const std::string& token)`: wires a
     *        directed arc; `token` is the node that carried the "->"/"<-" operator, for error
     *        messages.
     * @param addEdge `void addEdge(NodeId a, NodeId b, const std::string& token)`: wires an
     *        undirected edge; same `token` convention.
     * @throw InvalidArc if @p desc is malformed (an operator without a following node token).
     */
    template < typename Resolve, typename AddArc, typename AddEdge >
    void fastGraphWalkTokens(const std::vector< std::pair< std::string, FastGraphOp > >& tokens,
                             std::string_view                                            desc,
                             Resolve                                                     resolve,
                             AddArc                                                      addArc,
                             AddEdge                                                     addEdge) {
      NodeId lastId = 0;
      bool   first  = true;

      for (const auto& [token, op]: tokens) {
        if (token.empty()) {
          GUM_ERROR(InvalidArc,
                    "fastGraph: malformed description '" << desc
                                                         << "' (operator without a node token)")
        }
        const NodeId id = resolve(token);

        if (!first) {
          if (op == FastGraphOp::Arc) {
            addArc(lastId, id, token);
          } else if (op == FastGraphOp::RevArc) {
            addArc(id, lastId, token);
          } else {
            addEdge(lastId, id, token);
          }
        }

        lastId = id;
        first  = false;
      }
    }

  }   // namespace graph::detail

  template < GUM_NodeGraphable GRAPH_TYPE >
  GRAPH_TYPE fastGraph(std::string_view desc) {
    using namespace graph::detail;

    const auto parsed = fastGraphParseChains(
        desc,
        fastGraphTokenize,
        [](const std::pair< std::string, FastGraphOp >& token) -> const std::string& {
          return token.first;
        },
        [&] {
          GUM_ERROR(InvalidArc,
                    "fastGraph: malformed description '" << desc
                                                         << "' (operator without a node token)")
        });

    GRAPH_TYPE g;

    auto resolve
        = [&](const std::string& token) { return fastGraphBuildNode(g, token, parsed.useIds); };
    auto addArc = [&](NodeId tail, NodeId head, const std::string& token) {
      if constexpr (GUM_DiGraphable< GRAPH_TYPE >) {
        g.addArc(tail, head);
      } else {
        GUM_ERROR(InvalidArc,
                  "fastGraph: '" << token << "' is preceded by an arc operator but the "
                                 << "requested graph type does not support arcs")
      }
    };
    auto addEdge = [&](NodeId a, NodeId b, const std::string& token) {
      if constexpr (GUM_UndiGraphable< GRAPH_TYPE >) {
        g.addEdge(a, b);
      } else {
        GUM_ERROR(InvalidEdge,
                  "fastGraph: '" << token << "' is preceded by '-' but the "
                                 << "requested graph type does not support edges")
      }
    };

    for (const auto& tokens: parsed.chains) {
      fastGraphWalkTokens(tokens, desc, resolve, addArc, addEdge);
    }

    return g;
  }

}   // namespace gum
