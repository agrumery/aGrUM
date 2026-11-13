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

#include <agrum/base/core/fastSyntax.h>
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
     * @brief Splits a description into its chains of tokens (see fast::detail::splitChains()) and
     * decides how to read the nodes: as NodeIds if every token is a non-negative integer, else as
     * names.
     *
     * A quoted token (see fastSyntax.h) is replaced by its name and is always a name ('12' is the
     * name 12, not a NodeId).
     *
     * @param desc the description.
     * @param tokenize `std::vector< TOKEN > tokenize(std::string_view chain)`.
     * @param tokenName `std::string& tokenName(TOKEN&)`: the node name of a token.
     * @param caller the name of the calling function, for the error messages.
     * @param circleMarks true for a marked graph (see fast::detail::opensQuote()).
     * @throw InvalidArgument if the description is malformed (an operator without a node token, a
     *        token which is not a node name (see fastGraphIsNodeName()), a malformed quoted name)
     *        or if the NodeIds leave too many holes (see fastGraphCheckNodeIds()).
     */
    template < typename TOKENIZE, typename TOKEN_NAME >
    auto fastGraphParseChains(std::string_view desc,
                              TOKENIZE         tokenize,
                              TOKEN_NAME       tokenName,
                              std::string_view caller,
                              bool             circleMarks = false) {
      auto malformed = [&](std::string_view reason) {
        GUM_ERROR(InvalidArgument,
                  caller << ": malformed description '" << desc << "' (" << reason << ")")
      };
      FastGraphChains< typename decltype(tokenize(desc))::value_type > parsed;
      std::vector< NodeId >                                            ids;
      for (const auto& chainStr: fast::detail::splitChains(desc, circleMarks)) {
        auto tokens = tokenize(chainStr);
        for (auto& token: tokens) {
          std::string&                 name = tokenName(token);
          std::optional< std::string > unquoted;
          try {
            unquoted = fast::detail::unquoteFastToken(name);
          } catch (const InvalidArgument& e) { malformed(e.errorContent()); }
          if (unquoted) {
            name          = std::move(*unquoted);
            parsed.useIds = false;
            continue;
          }
          if (name.empty()) { malformed("operator without a node token"); }
          if (!fastGraphIsNodeName(name)) { malformed("'" + name + "' is not a node name"); }
          if (!parsed.useIds) continue;
          if (const auto id = fastGraphNodeId(name)) {
            ids.push_back(*id);
          } else {
            parsed.useIds = false;
          }
        }
        parsed.chains.push_back(std::move(tokens));
      }
      if (parsed.useIds) { fastGraphCheckNodeIds(desc, std::move(ids)); }
      return parsed;
    }

    /// gets or creates the node denoted by token, in NodeId or name mode
    template < typename GRAPH_TYPE >
    NodeId fastGraphBuildNode(GRAPH_TYPE& g, const std::string& token, bool useIds) {
      if (useIds) {
        const NodeId id = *fastGraphNodeId(token);   // checked by fastGraphParseChains
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
     * This is the shared topology grammar behind fastGraph() and fast::detail::parseFastModel()
     * (BayesNet and InfluenceDiagram): what a "node" is (a bare graph node, a variable...) is
     * entirely delegated to @p resolve, so callers with no notion of undirected edges can simply
     * pass an @p addEdge that throws.
     *
     * @param tokens the chain, as produced by fastGraphTokenize().
     * @param desc the full original description, only used to report errors.
     * @param caller the name of the calling function, for the error messages.
     * @param resolve `NodeId resolve(const std::string& token)`: gets or creates the node
     *        denoted by a raw token.
     * @param addArc `void addArc(NodeId tail, NodeId head, const std::string& token)`: wires a
     *        directed arc; `token` is the node that carried the "->"/"<-" operator, for error
     *        messages.
     * @param addEdge `void addEdge(NodeId a, NodeId b, const std::string& token)`: wires an
     *        undirected edge; same `token` convention.
     * @throw InvalidArgument if @p desc is malformed (an operator without a node token).
     */
    template < typename Resolve, typename AddArc, typename AddEdge >
    void fastGraphWalkTokens(const std::vector< std::pair< std::string, FastGraphOp > >& tokens,
                             std::string_view                                            desc,
                             std::string_view                                            caller,
                             Resolve                                                     resolve,
                             AddArc                                                      addArc,
                             AddEdge                                                     addEdge) {
      NodeId lastId = 0;
      bool   first  = true;

      for (const auto& [token, op]: tokens) {
        if (token.empty()) {
          GUM_ERROR(InvalidArgument,
                    caller << ": malformed description '" << desc
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
        [](std::pair< std::string, FastGraphOp >& token) -> std::string& { return token.first; },
        "fastGraph");

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
      fastGraphWalkTokens(tokens, desc, "fastGraph", resolve, addArc, addEdge);
    }

    return g;
  }

}   // namespace gum
