/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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


#include <algorithm>

#include <agrum/BN/learning/BNLearnUtils/genericBNLearner.h>

namespace gum {

  namespace learning {

    template < typename GUM_SCALAR >
    genericBNLearner::Database::Database(
       const std::string&                filename,
       const BayesNet< GUM_SCALAR >&     bn,
       const std::vector< std::string >& missing_symbols) {
      // assign to each column name in the database its position
      genericBNLearner::checkFileName__(filename);
      DBInitializerFromCSV<> initializer(filename);
      const auto&            xvar_names = initializer.variableNames();
      std::size_t            nb_vars = xvar_names.size();
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
        try {
          database__.insertTranslator(var, var_names[var.name()], missing_symbols);
        } catch (NotFound&) {
          GUM_ERROR(MissingVariableInDatabase,
                    "Variable '" << var.name() << "' is missing");
        }
        nodeId2cols__.insert(NodeId(node), i++);
      }

      // fill the database
      initializer.fillDatabase(database__);

      // get the domain sizes of the variables
      for (auto dom: database__.domainSizes())
        domain_sizes__.push_back(dom);

      // create the parser
      parser__ =
         new DBRowGeneratorParser<>(database__.handler(), DBRowGeneratorSet<>());
    }


    template < typename GUM_SCALAR >
    BayesNet< GUM_SCALAR > genericBNLearner::Database::BNVars__() const {
      BayesNet< GUM_SCALAR > bn;
      const std::size_t      nb_vars = database__.nbVariables();
      for (std::size_t i = 0; i < nb_vars; ++i) {
        const DiscreteVariable& var =
           dynamic_cast< const DiscreteVariable& >(database__.variable(i));
        bn.add(var);
      }
      return bn;
    }


    template < typename GUM_SCALAR >
    genericBNLearner::genericBNLearner(
       const std::string&                 filename,
       const gum::BayesNet< GUM_SCALAR >& bn,
       const std::vector< std::string >&  missing_symbols) :
        score_database__(filename, bn, missing_symbols) {
      no_apriori__ = new AprioriNoApriori<>(score_database__.databaseTable());
      GUM_CONSTRUCTOR(genericBNLearner);
    }


    /// use a new set of database rows' ranges to perform learning
    template < template < typename > class XALLOC >
    void genericBNLearner::useDatabaseRanges(
       const std::vector< std::pair< std::size_t, std::size_t >,
                          XALLOC< std::pair< std::size_t, std::size_t > > >&
          new_ranges) {
      // use a score to detect whether the ranges are ok
      ScoreLog2Likelihood<> score(score_database__.parser(), *no_apriori__);
      score.setRanges(new_ranges);
      ranges__ = score.ranges();
    }
  }   // namespace learning
}   // namespace gum
