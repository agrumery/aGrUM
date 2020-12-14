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


/** @file
 * @brief A pack of learning algorithms that can easily be used
 *
 * The pack currently contains K2, GreedyHillClimbing, miic, 3off2 and
 *LocalSearchWithTabuList
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <algorithm>
#include <iterator>

#include <agrum/agrum.h>
#include <agrum/BN/learning/BNLearnUtils/BNLearnerListener.h>
#include <agrum/BN/learning/BNLearnUtils/genericBNLearner.h>
#include <agrum/tools/stattests/indepTestChi2.h>
#include <agrum/tools/stattests/indepTestG2.h>
#include <agrum/BN/learning/scores_and_tests/scoreLog2Likelihood.h>
#include <agrum/tools/stattests/pseudoCount.h>

// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#  include <agrum/BN/learning/BNLearnUtils/genericBNLearner_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {


    genericBNLearner::Database::Database(const DatabaseTable<>& db) :
        database__(db) {
      // get the variables names
      const auto&       var_names = database__.variableNames();
      const std::size_t nb_vars = var_names.size();
      for (auto dom: database__.domainSizes())
        domain_sizes__.push_back(dom);
      for (std::size_t i = 0; i < nb_vars; ++i) {
        nodeId2cols__.insert(NodeId(i), i);
      }

      // create the parser
      parser__ =
         new DBRowGeneratorParser<>(database__.handler(), DBRowGeneratorSet<>());
    }


    genericBNLearner::Database::Database(
       const std::string&                filename,
       const std::vector< std::string >& missing_symbols) :
        Database(genericBNLearner::readFile__(filename, missing_symbols)) {}


    genericBNLearner::Database::Database(
       const std::string&                CSV_filename,
       Database&                         score_database,
       const std::vector< std::string >& missing_symbols) {
      // assign to each column name in the CSV file its column
      genericBNLearner::checkFileName__(CSV_filename);
      DBInitializerFromCSV<> initializer(CSV_filename);
      const auto&            apriori_names = initializer.variableNames();
      std::size_t            apriori_nb_vars = apriori_names.size();
      HashTable< std::string, std::size_t > apriori_names2col(apriori_nb_vars);
      for (std::size_t i = std::size_t(0); i < apriori_nb_vars; ++i)
        apriori_names2col.insert(apriori_names[i], i);

      // check that there are at least as many variables in the a priori
      // database as those in the score_database
      if (apriori_nb_vars < score_database.database__.nbVariables()) {
        GUM_ERROR(InvalidArgument,
                  "the a apriori database has fewer variables "
                  "than the observed database");
      }

      // get the mapping from the columns of score_database to those of
      // the CSV file
      const std::vector< std::string >& score_names =
         score_database.databaseTable().variableNames();
      const std::size_t                     score_nb_vars = score_names.size();
      HashTable< std::size_t, std::size_t > mapping(score_nb_vars);
      for (std::size_t i = std::size_t(0); i < score_nb_vars; ++i) {
        try {
          mapping.insert(i, apriori_names2col[score_names[i]]);
        } catch (Exception&) {
          GUM_ERROR(MissingVariableInDatabase,
                    "Variable "
                       << score_names[i]
                       << " of the observed database does not belong to the "
                       << "apriori database");
        }
      }

      // create the translators for CSV database
      for (std::size_t i = std::size_t(0); i < score_nb_vars; ++i) {
        const Variable& var = score_database.databaseTable().variable(i);
        database__.insertTranslator(var, mapping[i], missing_symbols);
      }

      // fill the database
      initializer.fillDatabase(database__);

      // get the domain sizes of the variables
      for (auto dom: database__.domainSizes())
        domain_sizes__.push_back(dom);

      // compute the mapping from node ids to column indices
      nodeId2cols__ = score_database.nodeId2Columns();

      // create the parser
      parser__ =
         new DBRowGeneratorParser<>(database__.handler(), DBRowGeneratorSet<>());
    }


    genericBNLearner::Database::Database(const Database& from) :
        database__(from.database__), domain_sizes__(from.domain_sizes__),
        nodeId2cols__(from.nodeId2cols__) {
      // create the parser
      parser__ =
         new DBRowGeneratorParser<>(database__.handler(), DBRowGeneratorSet<>());
    }


    genericBNLearner::Database::Database(Database&& from) :
        database__(std::move(from.database__)),
        domain_sizes__(std::move(from.domain_sizes__)),
        nodeId2cols__(std::move(from.nodeId2cols__)) {
      // create the parser
      parser__ =
         new DBRowGeneratorParser<>(database__.handler(), DBRowGeneratorSet<>());
    }


    genericBNLearner::Database::~Database() { delete parser__; }

    genericBNLearner::Database&
       genericBNLearner::Database::operator=(const Database& from) {
      if (this != &from) {
        delete parser__;
        database__ = from.database__;
        domain_sizes__ = from.domain_sizes__;
        nodeId2cols__ = from.nodeId2cols__;

        // create the parser
        parser__ =
           new DBRowGeneratorParser<>(database__.handler(), DBRowGeneratorSet<>());
      }

      return *this;
    }

    genericBNLearner::Database&
       genericBNLearner::Database::operator=(Database&& from) {
      if (this != &from) {
        delete parser__;
        database__ = std::move(from.database__);
        domain_sizes__ = std::move(from.domain_sizes__);
        nodeId2cols__ = std::move(from.nodeId2cols__);

        // create the parser
        parser__ =
           new DBRowGeneratorParser<>(database__.handler(), DBRowGeneratorSet<>());
      }

      return *this;
    }


    // ===========================================================================

    genericBNLearner::genericBNLearner(
       const std::string&                filename,
       const std::vector< std::string >& missing_symbols) :
        score_database__(filename, missing_symbols) {
      no_apriori__ = new AprioriNoApriori<>(score_database__.databaseTable());

      // for debugging purposes
      GUM_CONSTRUCTOR(genericBNLearner);
    }


    genericBNLearner::genericBNLearner(const DatabaseTable<>& db) :
        score_database__(db) {
      no_apriori__ = new AprioriNoApriori<>(score_database__.databaseTable());

      // for debugging purposes
      GUM_CONSTRUCTOR(genericBNLearner);
    }


    genericBNLearner::genericBNLearner(const genericBNLearner& from) :
        score_type__(from.score_type__),
        param_estimator_type__(from.param_estimator_type__),
        EMepsilon__(from.EMepsilon__), apriori_type__(from.apriori_type__),
        apriori_weight__(from.apriori_weight__),
        constraint_SliceOrder__(from.constraint_SliceOrder__),
        constraint_Indegree__(from.constraint_Indegree__),
        constraint_TabuList__(from.constraint_TabuList__),
        constraint_ForbiddenArcs__(from.constraint_ForbiddenArcs__),
        constraint_MandatoryArcs__(from.constraint_MandatoryArcs__),
        selected_algo__(from.selected_algo__), K2__(from.K2__),
        miic_3off2__(from.miic_3off2__), kmode_3off2__(from.kmode_3off2__),
        greedy_hill_climbing__(from.greedy_hill_climbing__),
        local_search_with_tabu_list__(from.local_search_with_tabu_list__),
        score_database__(from.score_database__), ranges__(from.ranges__),
        apriori_dbname__(from.apriori_dbname__),
        initial_dag__(from.initial_dag__) {
      no_apriori__ = new AprioriNoApriori<>(score_database__.databaseTable());

      // for debugging purposes
      GUM_CONS_CPY(genericBNLearner);
    }

    genericBNLearner::genericBNLearner(genericBNLearner&& from) :
        score_type__(from.score_type__),
        param_estimator_type__(from.param_estimator_type__),
        EMepsilon__(from.EMepsilon__), apriori_type__(from.apriori_type__),
        apriori_weight__(from.apriori_weight__),
        constraint_SliceOrder__(std::move(from.constraint_SliceOrder__)),
        constraint_Indegree__(std::move(from.constraint_Indegree__)),
        constraint_TabuList__(std::move(from.constraint_TabuList__)),
        constraint_ForbiddenArcs__(std::move(from.constraint_ForbiddenArcs__)),
        constraint_MandatoryArcs__(std::move(from.constraint_MandatoryArcs__)),
        selected_algo__(from.selected_algo__), K2__(std::move(from.K2__)),
        miic_3off2__(std::move(from.miic_3off2__)),
        kmode_3off2__(from.kmode_3off2__),
        greedy_hill_climbing__(std::move(from.greedy_hill_climbing__)),
        local_search_with_tabu_list__(
           std::move(from.local_search_with_tabu_list__)),
        score_database__(std::move(from.score_database__)),
        ranges__(std::move(from.ranges__)),
        apriori_dbname__(std::move(from.apriori_dbname__)),
        initial_dag__(std::move(from.initial_dag__)) {
      no_apriori__ = new AprioriNoApriori<>(score_database__.databaseTable());

      // for debugging purposes
      GUM_CONS_MOV(genericBNLearner);
    }

    genericBNLearner::~genericBNLearner() {
      if (score__) delete score__;

      if (apriori__) delete apriori__;

      if (no_apriori__) delete no_apriori__;

      if (apriori_database__) delete apriori_database__;

      if (mutual_info__) delete mutual_info__;

      GUM_DESTRUCTOR(genericBNLearner);
    }

    genericBNLearner& genericBNLearner::operator=(const genericBNLearner& from) {
      if (this != &from) {
        if (score__) {
          delete score__;
          score__ = nullptr;
        }

        if (apriori__) {
          delete apriori__;
          apriori__ = nullptr;
        }

        if (apriori_database__) {
          delete apriori_database__;
          apriori_database__ = nullptr;
        }

        if (mutual_info__) {
          delete mutual_info__;
          mutual_info__ = nullptr;
        }

        score_type__ = from.score_type__;
        param_estimator_type__ = from.param_estimator_type__;
        EMepsilon__ = from.EMepsilon__;
        apriori_type__ = from.apriori_type__;
        apriori_weight__ = from.apriori_weight__;
        constraint_SliceOrder__ = from.constraint_SliceOrder__;
        constraint_Indegree__ = from.constraint_Indegree__;
        constraint_TabuList__ = from.constraint_TabuList__;
        constraint_ForbiddenArcs__ = from.constraint_ForbiddenArcs__;
        constraint_MandatoryArcs__ = from.constraint_MandatoryArcs__;
        selected_algo__ = from.selected_algo__;
        K2__ = from.K2__;
        miic_3off2__ = from.miic_3off2__;
        kmode_3off2__ = from.kmode_3off2__;
        greedy_hill_climbing__ = from.greedy_hill_climbing__;
        local_search_with_tabu_list__ = from.local_search_with_tabu_list__;
        score_database__ = from.score_database__;
        ranges__ = from.ranges__;
        apriori_dbname__ = from.apriori_dbname__;
        initial_dag__ = from.initial_dag__;
        current_algorithm__ = nullptr;
      }

      return *this;
    }

    genericBNLearner& genericBNLearner::operator=(genericBNLearner&& from) {
      if (this != &from) {
        if (score__) {
          delete score__;
          score__ = nullptr;
        }

        if (apriori__) {
          delete apriori__;
          apriori__ = nullptr;
        }

        if (apriori_database__) {
          delete apriori_database__;
          apriori_database__ = nullptr;
        }

        if (mutual_info__) {
          delete mutual_info__;
          mutual_info__ = nullptr;
        }

        score_type__ = from.score_type__;
        param_estimator_type__ = from.param_estimator_type__;
        EMepsilon__ = from.EMepsilon__;
        apriori_type__ = from.apriori_type__;
        apriori_weight__ = from.apriori_weight__;
        constraint_SliceOrder__ = std::move(from.constraint_SliceOrder__);
        constraint_Indegree__ = std::move(from.constraint_Indegree__);
        constraint_TabuList__ = std::move(from.constraint_TabuList__);
        constraint_ForbiddenArcs__ = std::move(from.constraint_ForbiddenArcs__);
        constraint_MandatoryArcs__ = std::move(from.constraint_MandatoryArcs__);
        selected_algo__ = from.selected_algo__;
        K2__ = from.K2__;
        miic_3off2__ = std::move(from.miic_3off2__);
        kmode_3off2__ = from.kmode_3off2__;
        greedy_hill_climbing__ = std::move(from.greedy_hill_climbing__);
        local_search_with_tabu_list__ =
           std::move(from.local_search_with_tabu_list__);
        score_database__ = std::move(from.score_database__);
        ranges__ = std::move(from.ranges__);
        apriori_dbname__ = std::move(from.apriori_dbname__);
        initial_dag__ = std::move(from.initial_dag__);
        current_algorithm__ = nullptr;
      }

      return *this;
    }


    DatabaseTable<> readFile(const std::string& filename) {
      // get the extension of the file
      Size filename_size = Size(filename.size());

      if (filename_size < 4) {
        GUM_ERROR(FormatNotFound,
                  "genericBNLearner could not determine the "
                  "file type of the database");
      }

      std::string extension = filename.substr(filename.size() - 4);
      std::transform(
         extension.begin(), extension.end(), extension.begin(), ::tolower);

      if (extension != ".csv") {
        GUM_ERROR(OperationNotAllowed,
                  "genericBNLearner does not support yet this type "
                  "of database file");
      }

      DBInitializerFromCSV<> initializer(filename);

      const auto&       var_names = initializer.variableNames();
      const std::size_t nb_vars = var_names.size();

      DBTranslatorSet<>                translator_set;
      DBTranslator4LabelizedVariable<> translator;
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      DatabaseTable<> database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);

      return database;
    }


    void genericBNLearner::checkFileName__(const std::string& filename) {
      // get the extension of the file
      Size filename_size = Size(filename.size());

      if (filename_size < 4) {
        GUM_ERROR(FormatNotFound,
                  "genericBNLearner could not determine the "
                  "file type of the database");
      }

      std::string extension = filename.substr(filename.size() - 4);
      std::transform(
         extension.begin(), extension.end(), extension.begin(), ::tolower);

      if (extension != ".csv") {
        GUM_ERROR(
           OperationNotAllowed,
           "genericBNLearner does not support yet this type of database file");
      }
    }


    DatabaseTable<> genericBNLearner::readFile__(
       const std::string&                filename,
       const std::vector< std::string >& missing_symbols) {
      // get the extension of the file
      checkFileName__(filename);

      DBInitializerFromCSV<> initializer(filename);

      const auto&       var_names = initializer.variableNames();
      const std::size_t nb_vars = var_names.size();

      DBTranslatorSet<>                translator_set;
      DBTranslator4LabelizedVariable<> translator(missing_symbols);
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      DatabaseTable<> database(missing_symbols, translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);

      database.reorder();

      return database;
    }


    void genericBNLearner::createApriori__() {
      // first, save the old apriori, to be delete if everything is ok
      Apriori<>* old_apriori = apriori__;

      // create the new apriori
      switch (apriori_type__) {
        case AprioriType::NO_APRIORI:
          apriori__ = new AprioriNoApriori<>(score_database__.databaseTable(),
                                             score_database__.nodeId2Columns());
          break;

        case AprioriType::SMOOTHING:
          apriori__ = new AprioriSmoothing<>(score_database__.databaseTable(),
                                             score_database__.nodeId2Columns());
          break;

        case AprioriType::DIRICHLET_FROM_DATABASE:
          if (apriori_database__ != nullptr) {
            delete apriori_database__;
            apriori_database__ = nullptr;
          }

          apriori_database__ = new Database(apriori_dbname__,
                                            score_database__,
                                            score_database__.missingSymbols());

          apriori__ = new AprioriDirichletFromDatabase<>(
             score_database__.databaseTable(),
             apriori_database__->parser(),
             apriori_database__->nodeId2Columns());
          break;

        case AprioriType::BDEU:
          apriori__ = new AprioriBDeu<>(score_database__.databaseTable(),
                                        score_database__.nodeId2Columns());
          break;

        default:
          GUM_ERROR(OperationNotAllowed,
                    "The BNLearner does not support yet this apriori");
      }

      // do not forget to assign a weight to the apriori
      apriori__->setWeight(apriori_weight__);

      // remove the old apriori, if any
      if (old_apriori != nullptr) delete old_apriori;
    }

    void genericBNLearner::createScore__() {
      // first, save the old score, to be delete if everything is ok
      Score<>* old_score = score__;

      // create the new scoring function
      switch (score_type__) {
        case ScoreType::AIC:
          score__ = new ScoreAIC<>(score_database__.parser(),
                                   *apriori__,
                                   ranges__,
                                   score_database__.nodeId2Columns());
          break;

        case ScoreType::BD:
          score__ = new ScoreBD<>(score_database__.parser(),
                                  *apriori__,
                                  ranges__,
                                  score_database__.nodeId2Columns());
          break;

        case ScoreType::BDeu:
          score__ = new ScoreBDeu<>(score_database__.parser(),
                                    *apriori__,
                                    ranges__,
                                    score_database__.nodeId2Columns());
          break;

        case ScoreType::BIC:
          score__ = new ScoreBIC<>(score_database__.parser(),
                                   *apriori__,
                                   ranges__,
                                   score_database__.nodeId2Columns());
          break;

        case ScoreType::K2:
          score__ = new ScoreK2<>(score_database__.parser(),
                                  *apriori__,
                                  ranges__,
                                  score_database__.nodeId2Columns());
          break;

        case ScoreType::LOG2LIKELIHOOD:
          score__ = new ScoreLog2Likelihood<>(score_database__.parser(),
                                              *apriori__,
                                              ranges__,
                                              score_database__.nodeId2Columns());
          break;

        default:
          GUM_ERROR(OperationNotAllowed,
                    "genericBNLearner does not support yet this score");
      }

      // remove the old score, if any
      if (old_score != nullptr) delete old_score;
    }

    ParamEstimator<>*
       genericBNLearner::createParamEstimator__(DBRowGeneratorParser<>& parser,
                                                bool take_into_account_score) {
      ParamEstimator<>* param_estimator = nullptr;

      // create the new estimator
      switch (param_estimator_type__) {
        case ParamEstimatorType::ML:
          if (take_into_account_score && (score__ != nullptr)) {
            param_estimator =
               new ParamEstimatorML<>(parser,
                                      *apriori__,
                                      score__->internalApriori(),
                                      ranges__,
                                      score_database__.nodeId2Columns());
          } else {
            param_estimator =
               new ParamEstimatorML<>(parser,
                                      *apriori__,
                                      *no_apriori__,
                                      ranges__,
                                      score_database__.nodeId2Columns());
          }

          break;

        default:
          GUM_ERROR(OperationNotAllowed,
                    "genericBNLearner does not support "
                       << "yet this parameter estimator");
      }

      // assign the set of ranges
      param_estimator->setRanges(ranges__);

      return param_estimator;
    }

    /// prepares the initial graph for 3off2 or miic
    MixedGraph genericBNLearner::prepare_miic_3off2__() {
      // Initialize the mixed graph to the fully connected graph
      MixedGraph mgraph;
      for (Size i = 0; i < score_database__.databaseTable().nbVariables(); ++i) {
        mgraph.addNodeWithId(i);
        for (Size j = 0; j < i; ++j) {
          mgraph.addEdge(j, i);
        }
      }

      // translating the constraints for 3off2 or miic
      HashTable< std::pair< NodeId, NodeId >, char > initial_marks;
      const ArcSet& mandatory_arcs = constraint_MandatoryArcs__.arcs();
      for (const auto& arc: mandatory_arcs) {
        initial_marks.insert({arc.tail(), arc.head()}, '>');
      }

      const ArcSet& forbidden_arcs = constraint_ForbiddenArcs__.arcs();
      for (const auto& arc: forbidden_arcs) {
        initial_marks.insert({arc.tail(), arc.head()}, '-');
      }
      miic_3off2__.addConstraints(initial_marks);

      // create the mutual entropy object
      // if (mutual_info__ == nullptr) { this->useNML(); }
      createCorrectedMutualInformation__();

      return mgraph;
    }

    MixedGraph genericBNLearner::learnMixedStructure() {
      if (selected_algo__ != AlgoType::MIIC_THREE_OFF_TWO) {
        GUM_ERROR(OperationNotAllowed, "Must be using the miic/3off2 algorithm");
      }
      // check that the database does not contain any missing value
      if (score_database__.databaseTable().hasMissingValues()) {
        GUM_ERROR(MissingValueInDatabase,
                  "For the moment, the BNLearner is unable to learn "
                     << "structures with missing values in databases");
      }
      BNLearnerListener listener(this, miic_3off2__);

      // create the mixedGraph_constraint_MandatoryArcs.arcs();
      MixedGraph mgraph = this->prepare_miic_3off2__();

      return miic_3off2__.learnMixedStructure(*mutual_info__, mgraph);
    }

    DAG genericBNLearner::learnDAG() {
      // create the score and the apriori
      createApriori__();
      createScore__();

      return learnDAG__();
    }

    void genericBNLearner::createCorrectedMutualInformation__() {
      if (mutual_info__ != nullptr) delete mutual_info__;

      mutual_info__ =
         new CorrectedMutualInformation<>(score_database__.parser(),
                                          *no_apriori__,
                                          ranges__,
                                          score_database__.nodeId2Columns());
      switch (kmode_3off2__) {
        case CorrectedMutualInformation<>::KModeTypes::MDL:
          mutual_info__->useMDL();
          break;

        case CorrectedMutualInformation<>::KModeTypes::NML:
          mutual_info__->useNML();
          break;

        case CorrectedMutualInformation<>::KModeTypes::NoCorr:
          mutual_info__->useNoCorr();
          break;

        default:
          GUM_ERROR(NotImplementedYet,
                    "The BNLearner's corrected mutual information class does "
                       << "not support yet penalty mode " << int(kmode_3off2__));
      }
    }

    DAG genericBNLearner::learnDAG__() {
      // check that the database does not contain any missing value
      if (score_database__.databaseTable().hasMissingValues()
          || ((apriori_database__ != nullptr)
              && (apriori_type__ == AprioriType::DIRICHLET_FROM_DATABASE)
              && apriori_database__->databaseTable().hasMissingValues())) {
        GUM_ERROR(MissingValueInDatabase,
                  "For the moment, the BNLearner is unable to cope "
                  "with missing values in databases");
      }
      // add the mandatory arcs to the initial dag and remove the forbidden ones
      // from the initial graph
      DAG init_graph = initial_dag__;

      const ArcSet& mandatory_arcs = constraint_MandatoryArcs__.arcs();

      for (const auto& arc: mandatory_arcs) {
        if (!init_graph.exists(arc.tail())) init_graph.addNodeWithId(arc.tail());

        if (!init_graph.exists(arc.head())) init_graph.addNodeWithId(arc.head());

        init_graph.addArc(arc.tail(), arc.head());
      }

      const ArcSet& forbidden_arcs = constraint_ForbiddenArcs__.arcs();

      for (const auto& arc: forbidden_arcs) {
        init_graph.eraseArc(arc);
      }

      switch (selected_algo__) {
        // ========================================================================
        case AlgoType::MIIC_THREE_OFF_TWO: {
          BNLearnerListener listener(this, miic_3off2__);
          // create the mixedGraph and the corrected mutual information
          MixedGraph mgraph = this->prepare_miic_3off2__();

          return miic_3off2__.learnStructure(*mutual_info__, mgraph);
        }

        // ========================================================================
        case AlgoType::GREEDY_HILL_CLIMBING: {
          BNLearnerListener listener(this, greedy_hill_climbing__);
          StructuralConstraintSetStatic< StructuralConstraintMandatoryArcs,
                                         StructuralConstraintForbiddenArcs,
                                         StructuralConstraintPossibleEdges,
                                         StructuralConstraintSliceOrder >
             gen_constraint;
          static_cast< StructuralConstraintMandatoryArcs& >(gen_constraint) =
             constraint_MandatoryArcs__;
          static_cast< StructuralConstraintForbiddenArcs& >(gen_constraint) =
             constraint_ForbiddenArcs__;
          static_cast< StructuralConstraintPossibleEdges& >(gen_constraint) =
             constraint_PossibleEdges__;
          static_cast< StructuralConstraintSliceOrder& >(gen_constraint) =
             constraint_SliceOrder__;

          GraphChangesGenerator4DiGraph< decltype(gen_constraint) > op_set(
             gen_constraint);

          StructuralConstraintSetStatic< StructuralConstraintIndegree,
                                         StructuralConstraintDAG >
             sel_constraint;
          static_cast< StructuralConstraintIndegree& >(sel_constraint) =
             constraint_Indegree__;

          GraphChangesSelector4DiGraph< decltype(sel_constraint),
                                        decltype(op_set) >
             selector(*score__, sel_constraint, op_set);

          return greedy_hill_climbing__.learnStructure(selector, init_graph);
        }

        // ========================================================================
        case AlgoType::LOCAL_SEARCH_WITH_TABU_LIST: {
          BNLearnerListener listener(this, local_search_with_tabu_list__);
          StructuralConstraintSetStatic< StructuralConstraintMandatoryArcs,
                                         StructuralConstraintForbiddenArcs,
                                         StructuralConstraintPossibleEdges,
                                         StructuralConstraintSliceOrder >
             gen_constraint;
          static_cast< StructuralConstraintMandatoryArcs& >(gen_constraint) =
             constraint_MandatoryArcs__;
          static_cast< StructuralConstraintForbiddenArcs& >(gen_constraint) =
             constraint_ForbiddenArcs__;
          static_cast< StructuralConstraintPossibleEdges& >(gen_constraint) =
             constraint_PossibleEdges__;
          static_cast< StructuralConstraintSliceOrder& >(gen_constraint) =
             constraint_SliceOrder__;

          GraphChangesGenerator4DiGraph< decltype(gen_constraint) > op_set(
             gen_constraint);

          StructuralConstraintSetStatic< StructuralConstraintTabuList,
                                         StructuralConstraintIndegree,
                                         StructuralConstraintDAG >
             sel_constraint;
          static_cast< StructuralConstraintTabuList& >(sel_constraint) =
             constraint_TabuList__;
          static_cast< StructuralConstraintIndegree& >(sel_constraint) =
             constraint_Indegree__;

          GraphChangesSelector4DiGraph< decltype(sel_constraint),
                                        decltype(op_set) >
             selector(*score__, sel_constraint, op_set);

          return local_search_with_tabu_list__.learnStructure(selector,
                                                              init_graph);
        }

        // ========================================================================
        case AlgoType::K2: {
          BNLearnerListener listener(this, K2__.approximationScheme());
          StructuralConstraintSetStatic< StructuralConstraintMandatoryArcs,
                                         StructuralConstraintForbiddenArcs,
                                         StructuralConstraintPossibleEdges >
             gen_constraint;
          static_cast< StructuralConstraintMandatoryArcs& >(gen_constraint) =
             constraint_MandatoryArcs__;
          static_cast< StructuralConstraintForbiddenArcs& >(gen_constraint) =
             constraint_ForbiddenArcs__;
          static_cast< StructuralConstraintPossibleEdges& >(gen_constraint) =
             constraint_PossibleEdges__;

          GraphChangesGenerator4K2< decltype(gen_constraint) > op_set(
             gen_constraint);

          // if some mandatory arcs are incompatible with the order, use a DAG
          // constraint instead of a DiGraph constraint to avoid cycles
          const ArcSet& mandatory_arcs =
             static_cast< StructuralConstraintMandatoryArcs& >(gen_constraint)
                .arcs();
          const Sequence< NodeId >& order = K2__.order();
          bool                      order_compatible = true;

          for (const auto& arc: mandatory_arcs) {
            if (order.pos(arc.tail()) >= order.pos(arc.head())) {
              order_compatible = false;
              break;
            }
          }

          if (order_compatible) {
            StructuralConstraintSetStatic< StructuralConstraintIndegree,
                                           StructuralConstraintDiGraph >
               sel_constraint;
            static_cast< StructuralConstraintIndegree& >(sel_constraint) =
               constraint_Indegree__;

            GraphChangesSelector4DiGraph< decltype(sel_constraint),
                                          decltype(op_set) >
               selector(*score__, sel_constraint, op_set);

            return K2__.learnStructure(selector, init_graph);
          } else {
            StructuralConstraintSetStatic< StructuralConstraintIndegree,
                                           StructuralConstraintDAG >
               sel_constraint;
            static_cast< StructuralConstraintIndegree& >(sel_constraint) =
               constraint_Indegree__;

            GraphChangesSelector4DiGraph< decltype(sel_constraint),
                                          decltype(op_set) >
               selector(*score__, sel_constraint, op_set);

            return K2__.learnStructure(selector, init_graph);
          }
        }

        // ========================================================================
        default:
          GUM_ERROR(OperationNotAllowed,
                    "the learnDAG method has not been implemented for this "
                    "learning algorithm");
      }
    }

    std::string genericBNLearner::checkScoreAprioriCompatibility() {
      const std::string& apriori = getAprioriType__();

      switch (score_type__) {
        case ScoreType::AIC:
          return ScoreAIC<>::isAprioriCompatible(apriori, apriori_weight__);

        case ScoreType::BD:
          return ScoreBD<>::isAprioriCompatible(apriori, apriori_weight__);

        case ScoreType::BDeu:
          return ScoreBDeu<>::isAprioriCompatible(apriori, apriori_weight__);

        case ScoreType::BIC:
          return ScoreBIC<>::isAprioriCompatible(apriori, apriori_weight__);

        case ScoreType::K2:
          return ScoreK2<>::isAprioriCompatible(apriori, apriori_weight__);

        case ScoreType::LOG2LIKELIHOOD:
          return ScoreLog2Likelihood<>::isAprioriCompatible(apriori,
                                                            apriori_weight__);

        default: return "genericBNLearner does not support yet this score";
      }
    }


    /// sets the ranges of rows to be used for cross-validation learning
    std::pair< std::size_t, std::size_t >
       genericBNLearner::useCrossValidationFold(const std::size_t learning_fold,
                                                const std::size_t k_fold) {
      if (k_fold == 0) {
        GUM_ERROR(OutOfBounds, "K-fold cross validation with k=0 is forbidden");
      }

      if (learning_fold >= k_fold) {
        GUM_ERROR(OutOfBounds,
                  "In " << k_fold << "-fold cross validation, the learning "
                        << "fold should be strictly lower than " << k_fold
                        << " but, here, it is equal to " << learning_fold);
      }

      const std::size_t db_size = score_database__.databaseTable().nbRows();
      if (k_fold >= db_size) {
        GUM_ERROR(OutOfBounds,
                  "In " << k_fold << "-fold cross validation, the database's "
                        << "size should be strictly greater than " << k_fold
                        << " but, here, the database has only " << db_size
                        << "rows");
      }

      // create the ranges of rows of the test database
      const std::size_t foldSize = db_size / k_fold;
      const std::size_t unfold_deb = learning_fold * foldSize;
      const std::size_t unfold_end = unfold_deb + foldSize;

      ranges__.clear();
      if (learning_fold == std::size_t(0)) {
        ranges__.push_back(
           std::pair< std::size_t, std::size_t >(unfold_end, db_size));
      } else {
        ranges__.push_back(
           std::pair< std::size_t, std::size_t >(std::size_t(0), unfold_deb));

        if (learning_fold != k_fold - 1) {
          ranges__.push_back(
             std::pair< std::size_t, std::size_t >(unfold_end, db_size));
        }
      }

      return std::pair< std::size_t, std::size_t >(unfold_deb, unfold_end);
    }


    std::pair< double, double > genericBNLearner::chi2(
       const NodeId id1, const NodeId id2, const std::vector< NodeId >& knowing) {
      createApriori__();
      gum::learning::IndepTestChi2<> chi2score(
         score_database__.parser(), *apriori__, databaseRanges());

      return chi2score.statistics(id1, id2, knowing);
    }

    std::pair< double, double >
       genericBNLearner::chi2(const std::string&                name1,
                              const std::string&                name2,
                              const std::vector< std::string >& knowing) {
      std::vector< NodeId > knowingIds;
      std::transform(
         knowing.begin(),
         knowing.end(),
         std::back_inserter(knowingIds),
         [this](const std::string& c) -> NodeId { return this->idFromName(c); });
      return chi2(idFromName(name1), idFromName(name2), knowingIds);
    }

    std::pair< double, double > genericBNLearner::G2(
       const NodeId id1, const NodeId id2, const std::vector< NodeId >& knowing) {
      createApriori__();
      gum::learning::IndepTestG2<> g2score(
         score_database__.parser(), *apriori__, databaseRanges());
      return g2score.statistics(id1, id2, knowing);
    }

    std::pair< double, double >
       genericBNLearner::G2(const std::string&                name1,
                            const std::string&                name2,
                            const std::vector< std::string >& knowing) {
      std::vector< NodeId > knowingIds;
      std::transform(
         knowing.begin(),
         knowing.end(),
         std::back_inserter(knowingIds),
         [this](const std::string& c) -> NodeId { return this->idFromName(c); });
      return G2(idFromName(name1), idFromName(name2), knowingIds);
    }

    double genericBNLearner::logLikelihood(const std::vector< NodeId >& vars,
                                           const std::vector< NodeId >& knowing) {
      createApriori__();
      gum::learning::ScoreLog2Likelihood<> ll2score(
         score_database__.parser(), *apriori__, databaseRanges());

      std::vector< NodeId > total(vars);
      total.insert(total.end(), knowing.begin(), knowing.end());
      double LLtotal = ll2score.score(IdCondSet<>(total, false, true));
      if (knowing.size() == (Size)0) {
        return LLtotal;
      } else {
        double LLknw = ll2score.score(IdCondSet<>(knowing, false, true));
        return LLtotal - LLknw;
      }
    }

    double
       genericBNLearner::logLikelihood(const std::vector< std::string >& vars,
                                       const std::vector< std::string >& knowing) {
      std::vector< NodeId > ids;
      std::vector< NodeId > knowingIds;

      auto mapper = [this](const std::string& c) -> NodeId {
        return this->idFromName(c);
      };

      std::transform(vars.begin(), vars.end(), std::back_inserter(ids), mapper);
      std::transform(
         knowing.begin(), knowing.end(), std::back_inserter(knowingIds), mapper);

      return logLikelihood(ids, knowingIds);
    }

    std::vector< double >
       genericBNLearner::rawPseudoCount(const std::vector< NodeId >& vars) {
      Potential< double > res;

      createApriori__();
      gum::learning::PseudoCount<> count(
         score_database__.parser(), *apriori__, databaseRanges());
      return count.get(vars);
    }


    std::vector< double >
       genericBNLearner::rawPseudoCount(const std::vector< std::string >& vars) {
      std::vector< NodeId > ids;

      auto mapper = [this](const std::string& c) -> NodeId {
        return this->idFromName(c);
      };

      std::transform(vars.begin(), vars.end(), std::back_inserter(ids), mapper);

      return rawPseudoCount(ids);
    }

  } /* namespace learning */

} /* namespace gum */
