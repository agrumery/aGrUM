
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


/** @file
 * @brief A pack of learning algorithms that can easily be used
 *
 * The pack currently contains K2, GreedyHillClimbing and
 *LocalSearchWithTabuList
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <fstream>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to help IDE parser
#  include <agrum/learning/BNLearner.h>

#  include <agrum/learning/BNLearnUtils/BNLearnerListener.h>

namespace gum {

  namespace learning {
    template < typename GUM_SCALAR >
    BNLearner< GUM_SCALAR >::BNLearner(
       const std::string&                filename,
       const std::vector< std::string >& missing_symbols) :
        genericBNLearner(filename, missing_symbols) {
      GUM_CONSTRUCTOR(BNLearner);
    }

    template < typename GUM_SCALAR >
    BNLearner< GUM_SCALAR >::BNLearner(const DatabaseTable<>& db) :
        genericBNLearner(db) {
      GUM_CONSTRUCTOR(BNLearner);
    }

    template < typename GUM_SCALAR >
    BNLearner< GUM_SCALAR >::BNLearner(
       const std::string&                 filename,
       const gum::BayesNet< GUM_SCALAR >& bn,
       const std::vector< std::string >&  missing_symbols) :
        genericBNLearner(filename, bn, missing_symbols) {
      GUM_CONSTRUCTOR(BNLearner);
    }

    /// copy constructor
    template < typename GUM_SCALAR >
    BNLearner< GUM_SCALAR >::BNLearner(const BNLearner< GUM_SCALAR >& src) :
        genericBNLearner(src) {
      GUM_CONSTRUCTOR(BNLearner);
    }

    /// move constructor
    template < typename GUM_SCALAR >
    BNLearner< GUM_SCALAR >::BNLearner(BNLearner< GUM_SCALAR >&& src) :
        genericBNLearner(src) {
      GUM_CONSTRUCTOR(BNLearner);
    }

    /// destructor
    template < typename GUM_SCALAR >
    BNLearner< GUM_SCALAR >::~BNLearner() {
      GUM_DESTRUCTOR(BNLearner);
    }

    /// @}

    // ##########################################################################
    /// @name Operators
    // ##########################################################################
    /// @{

    /// copy operator
    template < typename GUM_SCALAR >
    BNLearner< GUM_SCALAR >& BNLearner< GUM_SCALAR >::
                             operator=(const BNLearner< GUM_SCALAR >& src) {
      genericBNLearner::operator=(src);
      return *this;
    }

    /// move operator
    template < typename GUM_SCALAR >
    BNLearner< GUM_SCALAR >& BNLearner< GUM_SCALAR >::
                             operator=(BNLearner< GUM_SCALAR >&& src) {
      genericBNLearner::operator=(std::move(src));
      return *this;
    }

    /// learn a Bayes Net from a file
    template < typename GUM_SCALAR >
    BayesNet< GUM_SCALAR > BNLearner< GUM_SCALAR >::learnBN() {
      // create the score, the apriori and the estimator
      auto notification = checkScoreAprioriCompatibility();
      if (notification != "") {
        std::cout << "[aGrUM notification] " << notification << std::endl;
      }
      __createApriori();
      __createScore();

      std::unique_ptr< ParamEstimator<> > param_estimator(
         __createParamEstimator(__score_database.parser(), true));

      return __Dag2BN.createBN< GUM_SCALAR >(*(param_estimator.get()),
                                             __learnDAG());
    }

    /// learns a BN (its parameters) when its structure is known
    template < typename GUM_SCALAR >
    BayesNet< GUM_SCALAR >
       BNLearner< GUM_SCALAR >::learnParameters(const DAG& dag,
                                                bool take_into_account_score) {
      // if the dag contains no node, return an empty BN
      if (dag.size() == 0) return BayesNet< GUM_SCALAR >();

      // check that the dag corresponds to the database
      std::vector< NodeId > ids;
      ids.reserve(dag.sizeNodes());
      for (const auto node : dag)
        ids.push_back(node);
      std::sort(ids.begin(), ids.end());

      if (ids.back() >= __score_database.names().size()) {
        std::stringstream str;
        str << "Learning parameters corresponding to the dag is impossible "
            << "because the database does not contain the following nodeID";
        std::vector< NodeId > bad_ids;
        for (const auto node : ids) {
          if (node >= __score_database.names().size()) bad_ids.push_back(node);
        }
        if (bad_ids.size() > 1) str << 's';
        str << ": ";
        bool deja = false;
        for (const auto node : bad_ids) {
          if (deja)
            str << ", ";
          else
            deja = true;
          str << node;
        }
        GUM_ERROR(MissingVariableInDatabase, str.str());
      }

      // create the apriori
      __createApriori();

      if (__EMepsilon == 0.0) {
        // check that the database does not contain any missing value
        if (__score_database.databaseTable().hasMissingValues()
            || ((__apriori_database != nullptr)
                && (__apriori_type == AprioriType::DIRICHLET_FROM_DATABASE)
                && __apriori_database->databaseTable().hasMissingValues())) {
          GUM_ERROR(MissingValueInDatabase,
                    "In general, the BNLearner is unable to cope with "
                       << "missing values in databases. To learn parameters in "
                       << "such situations, you should first use method "
                       << "useEM()");
        }

        // create the usual estimator
        DBRowGeneratorParser<> parser(__score_database.databaseTable().handler(),
                                      DBRowGeneratorSet<>());
        std::unique_ptr< ParamEstimator<> > param_estimator(
           __createParamEstimator(parser, take_into_account_score));

        return __Dag2BN.createBN< GUM_SCALAR >(*(param_estimator.get()), dag);
      } else {
        // EM !
        BNLearnerListener listener(this, __Dag2BN);

        // get the column types
        const auto&       database = __score_database.databaseTable();
        const std::size_t nb_vars = database.nbVariables();
        const std::vector< gum::learning::DBTranslatedValueType > col_types(
           nb_vars, gum::learning::DBTranslatedValueType::DISCRETE);

        // create the bootstrap estimator
        DBRowGenerator4CompleteRows<> generator_bootstrap(col_types);
        DBRowGeneratorSet<>           genset_bootstrap;
        genset_bootstrap.insertGenerator(generator_bootstrap);
        DBRowGeneratorParser<>              parser_bootstrap(database.handler(),
                                                genset_bootstrap);
        std::unique_ptr< ParamEstimator<> > param_estimator_bootstrap(
           __createParamEstimator(parser_bootstrap, take_into_account_score));

        // create the EM estimator
        BayesNet< GUM_SCALAR >         dummy_bn;
        DBRowGeneratorEM< GUM_SCALAR > generator_EM(col_types, dummy_bn);
        DBRowGenerator<>&              gen_EM = generator_EM;   // fix for g++-4.8
        DBRowGeneratorSet<>            genset_EM;
        genset_EM.insertGenerator(gen_EM);
        DBRowGeneratorParser<> parser_EM(database.handler(), genset_EM);
        std::unique_ptr< ParamEstimator<> > param_estimator_EM(
           __createParamEstimator(parser_EM, take_into_account_score));

        __Dag2BN.setEpsilon(__EMepsilon);
        return __Dag2BN.createBN< GUM_SCALAR >(
           *(param_estimator_bootstrap.get()), *(param_estimator_EM.get()), dag);
      }
    }


    /// learns a BN (its parameters) when its structure is known
    template < typename GUM_SCALAR >
    BayesNet< GUM_SCALAR >
       BNLearner< GUM_SCALAR >::learnParameters(bool take_into_account_score) {
      return learnParameters(__initial_dag, take_into_account_score);
    }


    template < typename GUM_SCALAR >
    NodeProperty< Sequence< std::string > >
       BNLearner< GUM_SCALAR >::__labelsFromBN(const std::string& filename,
                                               const BayesNet< GUM_SCALAR >& src) {
      std::ifstream in(filename, std::ifstream::in);

      if ((in.rdstate() & std::ifstream::failbit) != 0) {
        GUM_ERROR(gum::IOError, "File " << filename << " not found");
      }

      CSVParser<> parser(in);
      parser.next();
      auto names = parser.current();

      NodeProperty< Sequence< std::string > > modals;

      for (gum::Idx col = 0; col < names.size(); col++) {
        try {
          gum::NodeId graphId = src.idFromName(names[col]);
          modals.insert(col, gum::Sequence< std::string >());

          for (gum::Size i = 0; i < src.variable(graphId).domainSize(); ++i)
            modals[col].insert(src.variable(graphId).label(i));
        } catch (const gum::NotFound&) {
          // no problem : a column which is not in the BN...
        }
      }

      return modals;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
