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

#include <agrum/BN/learning/BNLearnUtils/IBNLearner.h>

namespace gum::learning {

  template < typename GUM_SCALAR >
  IBNLearner::Database::Database(const std::string&                filename,
                                 const BayesNet< GUM_SCALAR >&     bn,
                                 const std::vector< std::string >& missing_symbols) {
    // assign to each column name in the database its position
    IBNLearner::isCSVFileName_(filename);
    DBInitializerFromCSV                  initializer(filename);
    const auto&                           xvar_names = initializer.variableNames();
    std::size_t                           nb_vars    = xvar_names.size();
    HashTable< std::string, std::size_t > var_names(nb_vars);
    for (std::size_t i = std::size_t(0); i < nb_vars; ++i)
      var_names.insert(xvar_names[i], i);

    // we use the bn to insert the translators into the database table
    std::vector< NodeId > nodes;
    nodes.reserve(bn.dag().sizeNodes());
    for (const auto node: bn.dag())
      nodes.push_back(node);
    std::sort(nodes.begin(), nodes.end());
    std::size_t i = std::size_t(0);
    for (auto node: nodes) {
      const Variable& var = bn.variable(node);
      auto* p = var_names.tryGet(var.name());
      if (!p) {
        GUM_ERROR(MissingVariableInDatabase, "Variable '" << var.name() << "' is missing")
      }
      _database_.insertTranslator(var, *p, missing_symbols);
      _nodeId2cols_.insert(NodeId(node), i++);
    }

    // fill the database
    initializer.fillDatabase(_database_);

    // get the domain sizes of the variables
    for (auto dom: _database_.domainSizes())
      _domain_sizes_.push_back(dom);

    // create the parser
    _parser_ = new DBRowGeneratorParser(_database_.handler(), DBRowGeneratorSet());
  }

  template < typename GUM_SCALAR >
  BayesNet< GUM_SCALAR > IBNLearner::Database::_BNVars_() const {
    BayesNet< GUM_SCALAR > bn;
    const std::size_t      nb_vars = _database_.nbVariables();
    for (std::size_t i = 0; i < nb_vars; ++i) {
      const DiscreteVariable& var = dynamic_cast< const DiscreteVariable& >(_database_.variable(i));
      bn.add(var);
    }
    return bn;
  }

  template < typename GUM_SCALAR >
  IBNLearner::IBNLearner(const std::string&                 filename,
                         const gum::BayesNet< GUM_SCALAR >& bn,
                         const std::vector< std::string >&  missing_symbols) :
      scoreDatabase_(filename, bn, missing_symbols) {
    filename_     = filename;
    noPrior_      = new NoPrior(scoreDatabase_.databaseTable());
    inducedTypes_ = false;
    GUM_CONSTRUCTOR(IBNLearner);
  }


}   // namespace gum::learning
