/**
 *
 *   Copyright (c) 2005-2022 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief A dirichlet priori: computes its N'_ijk from a database
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <agrum/BN/learning/aprioris/aprioriDirichletFromDatabase.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/BN/learning/aprioris/aprioriDirichletFromDatabase_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    AprioriDirichletFromDatabase::AprioriDirichletFromDatabase(
       const DatabaseTable&                    learning_db,
       const DBRowGeneratorParser&             apriori_parser,
       const Bijection< NodeId, std::size_t >& nodeId2columns) :
        Apriori(apriori_parser.database(), Bijection< NodeId, std::size_t >()),
        _counter_(apriori_parser,
                  std::vector< std::pair< std::size_t, std::size_t > >(),
                  nodeId2columns) {
      // we check that the variables in the learning database also exist in the
      // apriori database and that they are precisely equal.
      const DatabaseTable&                  apriori_db    = apriori_parser.database();
      const auto&                           apriori_names = apriori_db.variableNames();
      const std::size_t                     apriori_size  = apriori_names.size();
      HashTable< std::string, std::size_t > names2col(apriori_size);
      for (std::size_t i = std::size_t(0); i < apriori_size; ++i)
        names2col.insert(apriori_names[i], i);

      const auto&                           learning_names = learning_db.variableNames();
      const std::size_t                     learning_size  = learning_names.size();
      HashTable< std::size_t, std::size_t > learning2apriori_index(learning_size);
      bool                                  different_index = false;
      for (std::size_t i = std::size_t(0); i < learning_size; ++i) {
        // get the column of the variable in the apriori database
        std::size_t apriori_col;
        try {
          apriori_col = names2col[learning_names[i]];
        } catch (...) {
          GUM_ERROR(DatabaseError,
                    "Variable " << learning_names[i]
                                << " could not be found in the apriori database");
        }

        // check that both variables are the same
        const Variable& learning_var = learning_db.variable(i);
        const Variable& apriori_var  = apriori_db.variable(apriori_col);
        if (learning_var.varType() != apriori_var.varType()) {
          GUM_ERROR(DatabaseError,
                    "Variable " << learning_names[i]
                                << " has not the same type in the learning database "
                                   " and the apriori database");
        }
        if (learning_var.domain() != apriori_var.domain()) {
          GUM_ERROR(DatabaseError,
                    "Variable " << learning_names[i] << " has domain " << learning_var.domain()
                                << " in the learning database and domain " << apriori_var.domain()
                                << " in the aprioi database");
        }

        // save the mapping from i to col
        learning2apriori_index.insert(i, apriori_col);
        if (i != apriori_col) different_index = true;
      }

      // here we are guaranteed that the variables in the learning database
      // have their equivalent in the a priori database. Now, we should
      // fill the bijection from ids to columns
      if (!different_index) {
        this->nodeId2columns_ = nodeId2columns;
      } else {
        if (nodeId2columns.empty()) {
          for (std::size_t i = std::size_t(0); i < learning_size; ++i) {
            this->nodeId2columns_.insert(NodeId(i), learning2apriori_index[i]);
          }
        } else {
          for (auto iter = nodeId2columns.begin(); iter != nodeId2columns.end(); ++iter) {
            this->nodeId2columns_.insert(iter.first(), learning2apriori_index[iter.second()]);
          }
        }
      }

      // recreate the record counter with the appropriate node2col mapping
      std::vector< std::pair< std::size_t, std::size_t > > ranges;
      RecordCounter good_counter(apriori_parser, ranges, this->nodeId2columns_);
      _counter_ = std::move(good_counter);

      if (apriori_db.nbRows() == std::size_t(0))
        _internal_weight_ = 0.0;
      else
        _internal_weight_ = this->weight_ / apriori_db.nbRows();

      GUM_CONSTRUCTOR(AprioriDirichletFromDatabase);
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
