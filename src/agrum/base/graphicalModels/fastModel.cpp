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
 * @brief Analysis, without construction, of the fast description of a graphical model.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <unordered_map>

#include <agrum/base/core/fastSyntax.h>
#include <agrum/base/graphicalModels/fastModel.h>
#include <agrum/base/graphs/fastGraph.h>
#include <agrum/base/variables/allDiscreteVariables.h>

#include <agrum/base/core/utils_string.h>

namespace gum::fast::detail {

  namespace {

    /// the model being analysed: a node is identified by the name of its variable
    struct ModelBuilder {
      FastModel                                      model;
      std::unordered_map< std::string, std::size_t > indexOf;

      std::size_t node(std::string description, std::string defaultDomain, FastNodeKind kind) {
        // every occurrence must be valid, the first one gives the variable (and its kind)
        auto name = fastVariable< double >(description, defaultDomain)->name();
        if (const auto it = indexOf.find(name); it != indexOf.end()) return it->second;
        indexOf.emplace(std::move(name), model.nodes.size());
        model.nodes.push_back({std::move(description), std::move(defaultDomain), kind});
        return model.nodes.size() - 1;
      }
    };

    /// refuses an unquoted token with a '<' or a '>' outside its domain: a misplaced operator
    /// ("A<" in "A<->B", "A->B" in a Markov random field)
    void checkOperators(std::string_view token, std::string_view desc) {
      if (findTopLevel(token, 0, "<>") != std::string_view::npos)
        GUM_ERROR(InvalidArgument,
                  "fastPrototype: malformed description '"
                      << desc << "' ('" << token
                      << "' is not a variable description, quote the name if needed)")
    }

  }   // namespace

  FastModel
      parseFastModel(std::string_view desc, FastModelType type, std::string_view defaultDomain) {
    ModelBuilder builder;

    if (type == FastModelType::MarkovRandomField) {
      for (const auto& chain: splitChains(desc)) {
        std::vector< std::size_t > factor;
        for (const auto& piece: splitTopLevel(chain, "--")) {
          auto token = trim_copy(piece);
          if (token.empty()) continue;
          checkOperators(token, desc);
          factor.push_back(
              builder.node(std::move(token), std::string(defaultDomain), FastNodeKind::Chance));
        }
        builder.model.factors.push_back(std::move(factor));
      }
      return std::move(builder.model);
    }

    const auto modelName = type == FastModelType::BayesNet ? "a BayesNet" : "an InfluenceDiagram";
    auto       resolve   = [&](const std::string& token) {
      checkOperators(token, desc);
      if (type == FastModelType::InfluenceDiagram) {
        if (token.front() == '*')
          return builder.node(token.substr(1), std::string(defaultDomain), FastNodeKind::Decision);
        if (token.front() == '$')
          return builder.node(token.substr(1), "[1]", FastNodeKind::Utility);
      }
      return builder.node(token, std::string(defaultDomain), FastNodeKind::Chance);
    };
    auto addArc = [&](std::size_t tail, std::size_t head, const std::string&) {
      builder.model.arcs.emplace_back(tail, head);
    };
    auto addEdge = [&](std::size_t, std::size_t, const std::string& token) {
      GUM_ERROR(InvalidEdge,
                "fastPrototype: '" << token << "' is preceded by '-' but " << modelName
                                   << " does not support edges")
    };
    for (const auto& chain: splitChains(desc)) {
      graph::detail::fastGraphWalkTokens(graph::detail::fastGraphTokenize(chain),
                                         desc,
                                         "fastPrototype",
                                         resolve,
                                         addArc,
                                         addEdge);
    }
    return std::move(builder.model);
  }

}   // namespace gum::fast::detail
