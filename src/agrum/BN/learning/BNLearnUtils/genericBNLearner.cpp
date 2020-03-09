
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
 * The pack currently contains K2, GreedyHillClimbing, miic, 3off2 and
 *LocalSearchWithTabuList
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <algorithm>

#include <agrum/agrum.h>
#include <agrum/BN/learning/BNLearnUtils/BNLearnerListener.h>
#include <agrum/BN/learning/BNLearnUtils/genericBNLearner.h>
#include <agrum/tools/stattests/indepTestChi2.h>
#include <agrum/tools/stattests/indepTestG2.h>
#include <agrum/BN/learning/scores_and_tests/scoreLog2Likelihood.h>

// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#  include <agrum/BN/learning/BNLearnUtils/genericBNLearner_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {


    genericBNLearner::Database::Database(const DatabaseTable<>& db) :
        __database(db) {
      // get the variables names
      const auto&       var_names = __database.variableNames();
      const std::size_t nb_vars = var_names.size();
      for (auto dom: __database.domainSizes())
        __domain_sizes.push_back(dom);
      for (std::size_t i = 0; i < nb_vars; ++i) {
        __nodeId2cols.insert(NodeId(i), i);
      }

      // create the parser
      __parser =
         new DBRowGeneratorParser<>(__database.handler(), DBRowGeneratorSet<>());
    }


    genericBNLearner::Database::Database(
       const std::string&                filename,
       const std::vector< std::string >& missing_symbols) :
        Database(genericBNLearner::__readFile(filename, missing_symbols)) {}


    genericBNLearner::Database::Database(
       const std::string&                CSV_filename,
       Database&                         score_database,
       const std::vector< std::string >& missing_symbols) {
      // assign to each column name in the CSV file its column
      genericBNLearner::__checkFileName(CSV_filename);
      DBInitializerFromCSV<> initializer(CSV_filename);
      const auto&            apriori_names = initializer.variableNames();
      std::size_t            apriori_nb_vars = apriori_names.size();
      HashTable< std::string, std::size_t > apriori_names2col(apriori_nb_vars);
      for (std::size_t i = std::size_t(0); i < apriori_nb_vars; ++i)
        apriori_names2col.insert(apriori_names[i], i);

      // check that there are at least as many variables in the a priori
      // database as those in the score_database
      if (apriori_nb_vars < score_database.__database.nbVariables()) {
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
        __database.insertTranslator(var, mapping[i], missing_symbols);
      }

      // fill the database
      initializer.fillDatabase(__database);

      // get the domain sizes of the variables
      for (auto dom: __database.domainSizes())
        __domain_sizes.push_back(dom);

      // compute the mapping from node ids to column indices
      __nodeId2cols = score_database.nodeId2Columns();

      // create the parser
      __parser =
         new DBRowGeneratorParser<>(__database.handler(), DBRowGeneratorSet<>());
    }


    genericBNLearner::Database::Database(const Database& from) :
        __database(from.__database), __domain_sizes(from.__domain_sizes),
        __nodeId2cols(from.__nodeId2cols) {
      // create the parser
      __parser =
         new DBRowGeneratorParser<>(__database.handler(), DBRowGeneratorSet<>());
    }


    genericBNLearner::Database::Database(Database&& from) :
        __database(std::move(from.__database)),
        __domain_sizes(std::move(from.__domain_sizes)),
        __nodeId2cols(std::move(from.__nodeId2cols)) {
      // create the parser
      __parser =
         new DBRowGeneratorParser<>(__database.handler(), DBRowGeneratorSet<>());
    }


    genericBNLearner::Database::~Database() { delete __parser; }

    genericBNLearner::Database&
       genericBNLearner::Database::operator=(const Database& from) {
      if (this != &from) {
        delete __parser;
        __database = from.__database;
        __domain_sizes = from.__domain_sizes;
        __nodeId2cols = from.__nodeId2cols;

        // create the parser
        __parser =
           new DBRowGeneratorParser<>(__database.handler(), DBRowGeneratorSet<>());
      }

      return *this;
    }

    genericBNLearner::Database&
       genericBNLearner::Database::operator=(Database&& from) {
      if (this != &from) {
        delete __parser;
        __database = std::move(from.__database);
        __domain_sizes = std::move(from.__domain_sizes);
        __nodeId2cols = std::move(from.__nodeId2cols);

        // create the parser
        __parser =
           new DBRowGeneratorParser<>(__database.handler(), DBRowGeneratorSet<>());
      }

      return *this;
    }


    // ===========================================================================

    genericBNLearner::genericBNLearner(
       const std::string&                filename,
       const std::vector< std::string >& missing_symbols) :
        __score_database(filename, missing_symbols) {
      __no_apriori = new AprioriNoApriori<>(__score_database.databaseTable());

      // for debugging purposes
      GUM_CONSTRUCTOR(genericBNLearner);
    }


    genericBNLearner::genericBNLearner(const DatabaseTable<>& db) :
        __score_database(db) {
      __no_apriori = new AprioriNoApriori<>(__score_database.databaseTable());

      // for debugging purposes
      GUM_CONSTRUCTOR(genericBNLearner);
    }


    genericBNLearner::genericBNLearner(const genericBNLearner& from) :
        __score_type(from.__score_type),
        __param_estimator_type(from.__param_estimator_type),
        __EMepsilon(from.__EMepsilon), __apriori_type(from.__apriori_type),
        __apriori_weight(from.__apriori_weight),
        __constraint_SliceOrder(from.__constraint_SliceOrder),
        __constraint_Indegree(from.__constraint_Indegree),
        __constraint_TabuList(from.__constraint_TabuList),
        __constraint_ForbiddenArcs(from.__constraint_ForbiddenArcs),
        __constraint_MandatoryArcs(from.__constraint_MandatoryArcs),
        __selected_algo(from.__selected_algo), __K2(from.__K2),
        __miic_3off2(from.__miic_3off2), __3off2_kmode(from.__3off2_kmode),
        __greedy_hill_climbing(from.__greedy_hill_climbing),
        __local_search_with_tabu_list(from.__local_search_with_tabu_list),
        __score_database(from.__score_database), __ranges(from.__ranges),
        __apriori_dbname(from.__apriori_dbname),
        __initial_dag(from.__initial_dag) {
      __no_apriori = new AprioriNoApriori<>(__score_database.databaseTable());

      // for debugging purposes
      GUM_CONS_CPY(genericBNLearner);
    }

    genericBNLearner::genericBNLearner(genericBNLearner&& from) :
        __score_type(from.__score_type),
        __param_estimator_type(from.__param_estimator_type),
        __EMepsilon(from.__EMepsilon), __apriori_type(from.__apriori_type),
        __apriori_weight(from.__apriori_weight),
        __constraint_SliceOrder(std::move(from.__constraint_SliceOrder)),
        __constraint_Indegree(std::move(from.__constraint_Indegree)),
        __constraint_TabuList(std::move(from.__constraint_TabuList)),
        __constraint_ForbiddenArcs(std::move(from.__constraint_ForbiddenArcs)),
        __constraint_MandatoryArcs(std::move(from.__constraint_MandatoryArcs)),
        __selected_algo(from.__selected_algo), __K2(std::move(from.__K2)),
        __miic_3off2(std::move(from.__miic_3off2)),
        __3off2_kmode(from.__3off2_kmode),
        __greedy_hill_climbing(std::move(from.__greedy_hill_climbing)),
        __local_search_with_tabu_list(
           std::move(from.__local_search_with_tabu_list)),
        __score_database(std::move(from.__score_database)),
        __ranges(std::move(from.__ranges)),
        __apriori_dbname(std::move(from.__apriori_dbname)),
        __initial_dag(std::move(from.__initial_dag)) {
      __no_apriori = new AprioriNoApriori<>(__score_database.databaseTable());

      // for debugging purposes
      GUM_CONS_MOV(genericBNLearner);
    }

    genericBNLearner::~genericBNLearner() {
      if (__score) delete __score;

      if (__apriori) delete __apriori;

      if (__no_apriori) delete __no_apriori;

      if (__apriori_database) delete __apriori_database;

      if (__mutual_info) delete __mutual_info;

      GUM_DESTRUCTOR(genericBNLearner);
    }

    genericBNLearner& genericBNLearner::operator=(const genericBNLearner& from) {
      if (this != &from) {
        if (__score) {
          delete __score;
          __score = nullptr;
        }

        if (__apriori) {
          delete __apriori;
          __apriori = nullptr;
        }

        if (__apriori_database) {
          delete __apriori_database;
          __apriori_database = nullptr;
        }

        if (__mutual_info) {
          delete __mutual_info;
          __mutual_info = nullptr;
        }

        __score_type = from.__score_type;
        __param_estimator_type = from.__param_estimator_type;
        __EMepsilon = from.__EMepsilon;
        __apriori_type = from.__apriori_type;
        __apriori_weight = from.__apriori_weight;
        __constraint_SliceOrder = from.__constraint_SliceOrder;
        __constraint_Indegree = from.__constraint_Indegree;
        __constraint_TabuList = from.__constraint_TabuList;
        __constraint_ForbiddenArcs = from.__constraint_ForbiddenArcs;
        __constraint_MandatoryArcs = from.__constraint_MandatoryArcs;
        __selected_algo = from.__selected_algo;
        __K2 = from.__K2;
        __miic_3off2 = from.__miic_3off2;
        __3off2_kmode = from.__3off2_kmode;
        __greedy_hill_climbing = from.__greedy_hill_climbing;
        __local_search_with_tabu_list = from.__local_search_with_tabu_list;
        __score_database = from.__score_database;
        __ranges = from.__ranges;
        __apriori_dbname = from.__apriori_dbname;
        __initial_dag = from.__initial_dag;
        __current_algorithm = nullptr;
      }

      return *this;
    }

    genericBNLearner& genericBNLearner::operator=(genericBNLearner&& from) {
      if (this != &from) {
        if (__score) {
          delete __score;
          __score = nullptr;
        }

        if (__apriori) {
          delete __apriori;
          __apriori = nullptr;
        }

        if (__apriori_database) {
          delete __apriori_database;
          __apriori_database = nullptr;
        }

        if (__mutual_info) {
          delete __mutual_info;
          __mutual_info = nullptr;
        }

        __score_type = from.__score_type;
        __param_estimator_type = from.__param_estimator_type;
        __EMepsilon = from.__EMepsilon;
        __apriori_type = from.__apriori_type;
        __apriori_weight = from.__apriori_weight;
        __constraint_SliceOrder = std::move(from.__constraint_SliceOrder);
        __constraint_Indegree = std::move(from.__constraint_Indegree);
        __constraint_TabuList = std::move(from.__constraint_TabuList);
        __constraint_ForbiddenArcs = std::move(from.__constraint_ForbiddenArcs);
        __constraint_MandatoryArcs = std::move(from.__constraint_MandatoryArcs);
        __selected_algo = from.__selected_algo;
        __K2 = from.__K2;
        __miic_3off2 = std::move(from.__miic_3off2);
        __3off2_kmode = from.__3off2_kmode;
        __greedy_hill_climbing = std::move(from.__greedy_hill_climbing);
        __local_search_with_tabu_list =
           std::move(from.__local_search_with_tabu_list);
        __score_database = std::move(from.__score_database);
        __ranges = std::move(from.__ranges);
        __apriori_dbname = std::move(from.__apriori_dbname);
        __initial_dag = std::move(from.__initial_dag);
        __current_algorithm = nullptr;
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


    void genericBNLearner::__checkFileName(const std::string& filename) {
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


    DatabaseTable<> genericBNLearner::__readFile(
       const std::string&                filename,
       const std::vector< std::string >& missing_symbols) {
      // get the extension of the file
      __checkFileName(filename);

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


    void genericBNLearner::__createApriori() {
      // first, save the old apriori, to be delete if everything is ok
      Apriori<>* old_apriori = __apriori;

      // create the new apriori
      switch (__apriori_type) {
        case AprioriType::NO_APRIORI:
          __apriori = new AprioriNoApriori<>(__score_database.databaseTable(),
                                             __score_database.nodeId2Columns());
          break;

        case AprioriType::SMOOTHING:
          __apriori = new AprioriSmoothing<>(__score_database.databaseTable(),
                                             __score_database.nodeId2Columns());
          break;

        case AprioriType::DIRICHLET_FROM_DATABASE:
          if (__apriori_database != nullptr) {
            delete __apriori_database;
            __apriori_database = nullptr;
          }

          __apriori_database = new Database(__apriori_dbname,
                                            __score_database,
                                            __score_database.missingSymbols());

          __apriori = new AprioriDirichletFromDatabase<>(
             __score_database.databaseTable(),
             __apriori_database->parser(),
             __apriori_database->nodeId2Columns());
          break;

        case AprioriType::BDEU:
          __apriori = new AprioriBDeu<>(__score_database.databaseTable(),
                                        __score_database.nodeId2Columns());
          break;

        default:
          GUM_ERROR(OperationNotAllowed,
                    "The BNLearner does not support yet this apriori");
      }

      // do not forget to assign a weight to the apriori
      __apriori->setWeight(__apriori_weight);

      // remove the old apriori, if any
      if (old_apriori != nullptr) delete old_apriori;
    }

    void genericBNLearner::__createScore() {
      // first, save the old score, to be delete if everything is ok
      Score<>* old_score = __score;

      // create the new scoring function
      switch (__score_type) {
        case ScoreType::AIC:
          __score = new ScoreAIC<>(__score_database.parser(),
                                   *__apriori,
                                   __ranges,
                                   __score_database.nodeId2Columns());
          break;

        case ScoreType::BD:
          __score = new ScoreBD<>(__score_database.parser(),
                                  *__apriori,
                                  __ranges,
                                  __score_database.nodeId2Columns());
          break;

        case ScoreType::BDeu:
          __score = new ScoreBDeu<>(__score_database.parser(),
                                    *__apriori,
                                    __ranges,
                                    __score_database.nodeId2Columns());
          break;

        case ScoreType::BIC:
          __score = new ScoreBIC<>(__score_database.parser(),
                                   *__apriori,
                                   __ranges,
                                   __score_database.nodeId2Columns());
          break;

        case ScoreType::K2:
          __score = new ScoreK2<>(__score_database.parser(),
                                  *__apriori,
                                  __ranges,
                                  __score_database.nodeId2Columns());
          break;

        case ScoreType::LOG2LIKELIHOOD:
          __score = new ScoreLog2Likelihood<>(__score_database.parser(),
                                              *__apriori,
                                              __ranges,
                                              __score_database.nodeId2Columns());
          break;

        default:
          GUM_ERROR(OperationNotAllowed,
                    "genericBNLearner does not support yet this score");
      }

      // remove the old score, if any
      if (old_score != nullptr) delete old_score;
    }

    ParamEstimator<>*
       genericBNLearner::__createParamEstimator(DBRowGeneratorParser<>& parser,
                                                bool take_into_account_score) {
      ParamEstimator<>* param_estimator = nullptr;

      // create the new estimator
      switch (__param_estimator_type) {
        case ParamEstimatorType::ML:
          if (take_into_account_score && (__score != nullptr)) {
            param_estimator =
               new ParamEstimatorML<>(parser,
                                      *__apriori,
                                      __score->internalApriori(),
                                      __ranges,
                                      __score_database.nodeId2Columns());
          } else {
            param_estimator =
               new ParamEstimatorML<>(parser,
                                      *__apriori,
                                      *__no_apriori,
                                      __ranges,
                                      __score_database.nodeId2Columns());
          }

          break;

        default:
          GUM_ERROR(OperationNotAllowed,
                    "genericBNLearner does not support "
                       << "yet this parameter estimator");
      }

      // assign the set of ranges
      param_estimator->setRanges(__ranges);

      return param_estimator;
    }

    /// prepares the initial graph for 3off2 or miic
    MixedGraph genericBNLearner::__prepare_miic_3off2() {
      // Initialize the mixed graph to the fully connected graph
      MixedGraph mgraph;
      for (Size i = 0; i < __score_database.databaseTable().nbVariables(); ++i) {
        mgraph.addNodeWithId(i);
        for (Size j = 0; j < i; ++j) {
          mgraph.addEdge(j, i);
        }
      }

      // translating the constraints for 3off2 or miic
      HashTable< std::pair< NodeId, NodeId >, char > initial_marks;
      const ArcSet& mandatory_arcs = __constraint_MandatoryArcs.arcs();
      for (const auto& arc: mandatory_arcs) {
        initial_marks.insert({arc.tail(), arc.head()}, '>');
      }

      const ArcSet& forbidden_arcs = __constraint_ForbiddenArcs.arcs();
      for (const auto& arc: forbidden_arcs) {
        initial_marks.insert({arc.tail(), arc.head()}, '-');
      }
      __miic_3off2.addConstraints(initial_marks);

      // create the mutual entropy object
      // if (__mutual_info == nullptr) { this->useNML(); }
      __createCorrectedMutualInformation();

      return mgraph;
    }

    MixedGraph genericBNLearner::learnMixedStructure() {
      if (__selected_algo != AlgoType::MIIC_THREE_OFF_TWO) {
        GUM_ERROR(OperationNotAllowed, "Must be using the miic/3off2 algorithm");
      }
      // check that the database does not contain any missing value
      if (__score_database.databaseTable().hasMissingValues()) {
        GUM_ERROR(MissingValueInDatabase,
                  "For the moment, the BNLearner is unable to learn "
                     << "structures with missing values in databases");
      }
      BNLearnerListener listener(this, __miic_3off2);

      // create the mixedGraph_constraint_MandatoryArcs.arcs();
      MixedGraph mgraph = this->__prepare_miic_3off2();

      return __miic_3off2.learnMixedStructure(*__mutual_info, mgraph);
    }

    DAG genericBNLearner::learnDAG() {
      // create the score and the apriori
      __createApriori();
      __createScore();

      return __learnDAG();
    }

    void genericBNLearner::__createCorrectedMutualInformation() {
      if (__mutual_info != nullptr) delete __mutual_info;

      __mutual_info =
         new CorrectedMutualInformation<>(__score_database.parser(),
                                          *__no_apriori,
                                          __ranges,
                                          __score_database.nodeId2Columns());
      switch (__3off2_kmode) {
        case CorrectedMutualInformation<>::KModeTypes::MDL:
          __mutual_info->useMDL();
          break;

        case CorrectedMutualInformation<>::KModeTypes::NML:
          __mutual_info->useNML();
          break;

        case CorrectedMutualInformation<>::KModeTypes::NoCorr:
          __mutual_info->useNoCorr();
          break;

        default:
          GUM_ERROR(NotImplementedYet,
                    "The BNLearner's corrected mutual information class does "
                       << "not support yet penalty mode " << int(__3off2_kmode));
      }
    }

    DAG genericBNLearner::__learnDAG() {
      // check that the database does not contain any missing value
      if (__score_database.databaseTable().hasMissingValues()
          || ((__apriori_database != nullptr)
              && (__apriori_type == AprioriType::DIRICHLET_FROM_DATABASE)
              && __apriori_database->databaseTable().hasMissingValues())) {
        GUM_ERROR(MissingValueInDatabase,
                  "For the moment, the BNLearner is unable to cope "
                  "with missing values in databases");
      }
      // add the mandatory arcs to the initial dag and remove the forbidden ones
      // from the initial graph
      DAG init_graph = __initial_dag;

      const ArcSet& mandatory_arcs = __constraint_MandatoryArcs.arcs();

      for (const auto& arc: mandatory_arcs) {
        if (!init_graph.exists(arc.tail())) init_graph.addNodeWithId(arc.tail());

        if (!init_graph.exists(arc.head())) init_graph.addNodeWithId(arc.head());

        init_graph.addArc(arc.tail(), arc.head());
      }

      const ArcSet& forbidden_arcs = __constraint_ForbiddenArcs.arcs();

      for (const auto& arc: forbidden_arcs) {
        init_graph.eraseArc(arc);
      }

      switch (__selected_algo) {
        // ========================================================================
        case AlgoType::MIIC_THREE_OFF_TWO: {
          BNLearnerListener listener(this, __miic_3off2);
          // create the mixedGraph and the corrected mutual information
          MixedGraph mgraph = this->__prepare_miic_3off2();

          return __miic_3off2.learnStructure(*__mutual_info, mgraph);
        }

        // ========================================================================
        case AlgoType::GREEDY_HILL_CLIMBING: {
          BNLearnerListener listener(this, __greedy_hill_climbing);
          StructuralConstraintSetStatic< StructuralConstraintMandatoryArcs,
                                         StructuralConstraintForbiddenArcs,
                                         StructuralConstraintPossibleEdges,
                                         StructuralConstraintSliceOrder >
             gen_constraint;
          static_cast< StructuralConstraintMandatoryArcs& >(gen_constraint) =
             __constraint_MandatoryArcs;
          static_cast< StructuralConstraintForbiddenArcs& >(gen_constraint) =
             __constraint_ForbiddenArcs;
          static_cast< StructuralConstraintPossibleEdges& >(gen_constraint) =
             __constraint_PossibleEdges;
          static_cast< StructuralConstraintSliceOrder& >(gen_constraint) =
             __constraint_SliceOrder;

          GraphChangesGenerator4DiGraph< decltype(gen_constraint) > op_set(
             gen_constraint);

          StructuralConstraintSetStatic< StructuralConstraintIndegree,
                                         StructuralConstraintDAG >
             sel_constraint;
          static_cast< StructuralConstraintIndegree& >(sel_constraint) =
             __constraint_Indegree;

          GraphChangesSelector4DiGraph< decltype(sel_constraint),
                                        decltype(op_set) >
             selector(*__score, sel_constraint, op_set);

          return __greedy_hill_climbing.learnStructure(selector, init_graph);
        }

        // ========================================================================
        case AlgoType::LOCAL_SEARCH_WITH_TABU_LIST: {
          BNLearnerListener listener(this, __local_search_with_tabu_list);
          StructuralConstraintSetStatic< StructuralConstraintMandatoryArcs,
                                         StructuralConstraintForbiddenArcs,
                                         StructuralConstraintPossibleEdges,
                                         StructuralConstraintSliceOrder >
             gen_constraint;
          static_cast< StructuralConstraintMandatoryArcs& >(gen_constraint) =
             __constraint_MandatoryArcs;
          static_cast< StructuralConstraintForbiddenArcs& >(gen_constraint) =
             __constraint_ForbiddenArcs;
          static_cast< StructuralConstraintPossibleEdges& >(gen_constraint) =
             __constraint_PossibleEdges;
          static_cast< StructuralConstraintSliceOrder& >(gen_constraint) =
             __constraint_SliceOrder;

          GraphChangesGenerator4DiGraph< decltype(gen_constraint) > op_set(
             gen_constraint);

          StructuralConstraintSetStatic< StructuralConstraintTabuList,
                                         StructuralConstraintIndegree,
                                         StructuralConstraintDAG >
             sel_constraint;
          static_cast< StructuralConstraintTabuList& >(sel_constraint) =
             __constraint_TabuList;
          static_cast< StructuralConstraintIndegree& >(sel_constraint) =
             __constraint_Indegree;

          GraphChangesSelector4DiGraph< decltype(sel_constraint),
                                        decltype(op_set) >
             selector(*__score, sel_constraint, op_set);

          return __local_search_with_tabu_list.learnStructure(selector,
                                                              init_graph);
        }

        // ========================================================================
        case AlgoType::K2: {
          BNLearnerListener listener(this, __K2.approximationScheme());
          StructuralConstraintSetStatic< StructuralConstraintMandatoryArcs,
                                         StructuralConstraintForbiddenArcs,
                                         StructuralConstraintPossibleEdges >
             gen_constraint;
          static_cast< StructuralConstraintMandatoryArcs& >(gen_constraint) =
             __constraint_MandatoryArcs;
          static_cast< StructuralConstraintForbiddenArcs& >(gen_constraint) =
             __constraint_ForbiddenArcs;
          static_cast< StructuralConstraintPossibleEdges& >(gen_constraint) =
             __constraint_PossibleEdges;

          GraphChangesGenerator4K2< decltype(gen_constraint) > op_set(
             gen_constraint);

          // if some mandatory arcs are incompatible with the order, use a DAG
          // constraint instead of a DiGraph constraint to avoid cycles
          const ArcSet& mandatory_arcs =
             static_cast< StructuralConstraintMandatoryArcs& >(gen_constraint)
                .arcs();
          const Sequence< NodeId >& order = __K2.order();
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
               __constraint_Indegree;

            GraphChangesSelector4DiGraph< decltype(sel_constraint),
                                          decltype(op_set) >
               selector(*__score, sel_constraint, op_set);

            return __K2.learnStructure(selector, init_graph);
          } else {
            StructuralConstraintSetStatic< StructuralConstraintIndegree,
                                           StructuralConstraintDAG >
               sel_constraint;
            static_cast< StructuralConstraintIndegree& >(sel_constraint) =
               __constraint_Indegree;

            GraphChangesSelector4DiGraph< decltype(sel_constraint),
                                          decltype(op_set) >
               selector(*__score, sel_constraint, op_set);

            return __K2.learnStructure(selector, init_graph);
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
      const std::string& apriori = __getAprioriType();

      switch (__score_type) {
        case ScoreType::AIC:
          return ScoreAIC<>::isAprioriCompatible(apriori, __apriori_weight);

        case ScoreType::BD:
          return ScoreBD<>::isAprioriCompatible(apriori, __apriori_weight);

        case ScoreType::BDeu:
          return ScoreBDeu<>::isAprioriCompatible(apriori, __apriori_weight);

        case ScoreType::BIC:
          return ScoreBIC<>::isAprioriCompatible(apriori, __apriori_weight);

        case ScoreType::K2:
          return ScoreK2<>::isAprioriCompatible(apriori, __apriori_weight);

        case ScoreType::LOG2LIKELIHOOD:
          return ScoreLog2Likelihood<>::isAprioriCompatible(apriori,
                                                            __apriori_weight);

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

      const std::size_t db_size = __score_database.databaseTable().nbRows();
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

      __ranges.clear();
      if (learning_fold == std::size_t(0)) {
        __ranges.push_back(
           std::pair< std::size_t, std::size_t >(unfold_end, db_size));
      } else {
        __ranges.push_back(
           std::pair< std::size_t, std::size_t >(std::size_t(0), unfold_deb));

        if (learning_fold != k_fold - 1) {
          __ranges.push_back(
             std::pair< std::size_t, std::size_t >(unfold_end, db_size));
        }
      }

      return std::pair< std::size_t, std::size_t >(unfold_deb, unfold_end);
    }


    std::pair< double, double > genericBNLearner::chi2(
       const NodeId id1, const NodeId id2, const std::vector< NodeId >& knowing) {
      __createApriori();
      gum::learning::IndepTestChi2<> chi2score(
         __score_database.parser(), *__apriori, databaseRanges());

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
      __createApriori();
      gum::learning::IndepTestG2<> g2score(
         __score_database.parser(), *__apriori, databaseRanges());
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
      __createApriori();
      gum::learning::ScoreLog2Likelihood<> ll2score(
         __score_database.parser(), *__apriori, databaseRanges());

      std::vector< NodeId > total(vars);
      total.insert(total.end(), knowing.begin(), knowing.end());
      double LLtotal = ll2score.score(IdSet<>(total, false, true));
      if (knowing.size() == (Size)0) {
        return LLtotal;
      } else {
        double LLknw = ll2score.score(IdSet<>(knowing, false, true));
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


  } /* namespace learning */

} /* namespace gum */
