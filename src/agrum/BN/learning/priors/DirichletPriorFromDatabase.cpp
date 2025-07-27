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
 * @brief A dirichlet priori: computes its N'_ijk from a database
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <agrum/BN/learning/priors/DirichletPriorFromDatabase.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/BN/learning/priors/DirichletPriorFromDatabase_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    DirichletPriorFromDatabase::DirichletPriorFromDatabase(
        const DatabaseTable&                    learning_db,
        const DBRowGeneratorParser&             prior_parser,
        const Bijection< NodeId, std::size_t >& nodeId2columns) :
        Prior(prior_parser.database(), Bijection< NodeId, std::size_t >()),
        _counter_(prior_parser,
                  std::vector< std::pair< std::size_t, std::size_t > >(),
                  nodeId2columns) {
      // we check that the variables in the learning database also exist in the
      // prior database and that they are precisely equal.
      const DatabaseTable&                  prior_db    = prior_parser.database();
      const auto&                           prior_names = prior_db.variableNames();
      const std::size_t                     prior_size  = prior_names.size();
      HashTable< std::string, std::size_t > names2col(prior_size);
      for (std::size_t i = std::size_t(0); i < prior_size; ++i)
        names2col.insert(prior_names[i], i);

      const auto&                           learning_names = learning_db.variableNames();
      const std::size_t                     learning_size  = learning_names.size();
      HashTable< std::size_t, std::size_t > learning2prior_index(learning_size);
      bool                                  different_index = false;
      for (auto i = std::size_t(0); i < learning_size; ++i) {
        // get the column of the variable in the prior database
        std::size_t prior_col;
        try {
          prior_col = names2col[learning_names[i]];
        } catch (...) {
          GUM_ERROR(DatabaseError,
                    "Variable " << learning_names[i]
                                << " could not be found in the prior database");
        }

        // check that both variables are the same
        const Variable& learning_var = learning_db.variable(i);
        const Variable& prior_var    = prior_db.variable(prior_col);
        if (learning_var.varType() != prior_var.varType()) {
          GUM_ERROR(DatabaseError,
                    "Variable " << learning_names[i]
                                << " has not the same type in the learning database "
                                   " and the prior database");
        }
        if (learning_var.domain() != prior_var.domain()) {
          GUM_ERROR(DatabaseError,
                    "Variable " << learning_names[i] << " has domain " << learning_var.domain()
                                << " in the learning database and domain " << prior_var.domain()
                                << " in the aprioi database");
        }

        // save the mapping from i to col
        learning2prior_index.insert(i, prior_col);
        if (i != prior_col) different_index = true;
      }

      // here we are guaranteed that the variables in the learning database
      // have their equivalent in the a priordatabase. Now, we should
      // fill the bijection from ids to columns
      if (!different_index) {
        this->nodeId2columns_ = nodeId2columns;
      } else {
        if (nodeId2columns.empty()) {
          for (auto i = std::size_t(0); i < learning_size; ++i) {
            this->nodeId2columns_.insert(NodeId(i), learning2prior_index[i]);
          }
        } else {
          for (auto iter = nodeId2columns.begin(); iter != nodeId2columns.end(); ++iter) {
            this->nodeId2columns_.insert(iter.first(), learning2prior_index[iter.second()]);
          }
        }
      }

      // recreate the record counter with the appropriate node2col mapping
      std::vector< std::pair< std::size_t, std::size_t > > ranges;
      RecordCounter good_counter(prior_parser, ranges, this->nodeId2columns_);
      _counter_ = std::move(good_counter);

      if (prior_db.nbRows() == std::size_t(0)) _internal_weight_ = 0.0;
      else _internal_weight_ = this->weight_ / double(prior_db.nbRows());

      GUM_CONSTRUCTOR(DirichletPriorFromDatabase);
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
