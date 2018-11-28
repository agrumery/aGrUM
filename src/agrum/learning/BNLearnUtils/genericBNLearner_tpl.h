
/**************************************************************************
 *   Copyright (C) 2017 by Pierre-Henri WUILLEMIN  and Christophe GONZALES *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


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
      try {
        for (auto node : bn.dag()) {
          const Variable& var = bn.variable(node);
          __database.insertTranslator(var, var_names[var.name()], missing_symbols);
        }
      } catch (NotFound&) {
        GUM_ERROR(MissingVariableInDatabase,
                  "the database does not contain variable ");
      }

      // fill the database
      initializer.fillDatabase(__database);

      // check that the database does not contain any missing value
      if (__database.hasMissingValues())
        GUM_ERROR(MissingValueInDatabase,
                  "For the moment, the BNLearner is unable to cope "
                  "with missing values in databases");

      // get the domain sizes of the variables
      for (auto dom : __database.domainSizes())
        __domain_sizes.push_back(dom);

      nb_vars = __database.nbVariables();
      for (std::size_t i = std::size_t(0); i < nb_vars; ++i)
        __nodeId2cols.insert(NodeId(i), i);

      // create the parser
      __parser =
        new DBRowGeneratorParser<>(__database.handler(), DBRowGeneratorSet<>());
    }


    template < typename GUM_SCALAR >
    genericBNLearner::Database::Database(
      const std::string&                filename,
      Database&                         score_database,
      const BayesNet< GUM_SCALAR >&     bn,
      const std::vector< std::string >& missing_symbols) :
        __database(genericBNLearner::__readFile(filename, bn, missing_symbols)) {}


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
