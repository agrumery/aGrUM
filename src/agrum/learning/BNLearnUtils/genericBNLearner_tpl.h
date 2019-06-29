/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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

#include <agrum/learning/BNLearnUtils/genericBNLearner.h>

namespace gum {

  namespace learning {

    template < typename GUM_SCALAR >
    genericBNLearner::Database::Database(
       const std::string&                filename,
       const BayesNet< GUM_SCALAR >&     bn,
       const std::vector< std::string >& missing_symbols) {
      // assign to each column name in the database its position
      genericBNLearner::__checkFileName(filename);
      DBInitializerFromCSV<> initializer(filename);
      const auto&            xvar_names = initializer.variableNames();
      std::size_t            nb_vars = xvar_names.size();
      HashTable< std::string, std::size_t > var_names(nb_vars);
      for (std::size_t i = std::size_t(0); i < nb_vars; ++i)
        var_names.insert(xvar_names[i], i);

      // we use the bn to insert the translators into the database table
      std::vector< NodeId > nodes;
      nodes.reserve(bn.dag().sizeNodes());
      for (const auto node : bn.dag())
        nodes.push_back(node);
      std::sort(nodes.begin(), nodes.end());
      std::size_t i = std::size_t(0);
      for (auto node : nodes) {
        const Variable& var = bn.variable(node);
        try {
          __database.insertTranslator(var, var_names[var.name()], missing_symbols);
        } catch (NotFound&) {
          GUM_ERROR(MissingVariableInDatabase,
                    "Variable '" << var.name() << "' is missing");
        }
        __nodeId2cols.insert(NodeId(node), i++);
      }

      // fill the database
      initializer.fillDatabase(__database);

      // get the domain sizes of the variables
      for (auto dom : __database.domainSizes())
        __domain_sizes.push_back(dom);

      // create the parser
      __parser =
         new DBRowGeneratorParser<>(__database.handler(), DBRowGeneratorSet<>());
    }


    template < typename GUM_SCALAR >
    BayesNet< GUM_SCALAR > genericBNLearner::Database::__BNVars() const {
      BayesNet< GUM_SCALAR > bn;
      const std::size_t      nb_vars = __database.nbVariables();
      for (std::size_t i = 0; i < nb_vars; ++i) {
        const DiscreteVariable& var =
           dynamic_cast< const DiscreteVariable& >(__database.variable(i));
        bn.add(var);
      }
      return bn;
    }


    template < typename GUM_SCALAR >
    genericBNLearner::genericBNLearner(
       const std::string&                 filename,
       const gum::BayesNet< GUM_SCALAR >& bn,
       const std::vector< std::string >&  missing_symbols) :
        __score_database(filename, bn, missing_symbols) {
      __no_apriori = new AprioriNoApriori<>(__score_database.databaseTable());
      GUM_CONSTRUCTOR(genericBNLearner);
    }


    /// use a new set of database rows' ranges to perform learning
    template < template < typename > class XALLOC >
    void genericBNLearner::useDatabaseRanges(
       const std::vector< std::pair< std::size_t, std::size_t >,
                          XALLOC< std::pair< std::size_t, std::size_t > > >&
          new_ranges) {
      // use a score to detect whether the ranges are ok
      ScoreLog2Likelihood<> score(__score_database.parser(), *__no_apriori);
      score.setRanges(new_ranges);
      __ranges = score.ranges();
    }
  }   // namespace learning
}   // namespace gum
