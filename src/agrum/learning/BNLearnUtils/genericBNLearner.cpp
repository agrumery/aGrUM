/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief A pack of learning algorithms that can easily be used
 *
 * The pack currently contains K2, GreedyHillClimbing and LocalSearchWithTabuList
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <algorithm>

#include <agrum/config.h>
#include <agrum/learning/BNLearnUtils/genericBNLearner.h>
#include <agrum/learning/BNLearnUtils/BNLearnerListener.h>

#include <agrum/learning/scores_and_tests/scoreInternalNoApriori.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#include <agrum/learning/BNLearnUtils/genericBNLearner.inl>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// Database default constructor
    genericBNLearner::Database::Database(std::string filename)
        : __database(genericBNLearner::__readFile(filename)),
          __generators(RowGeneratorIdentity()) {
      // create the RowFilter used for learning: we first generate a universal
      // filter that can parse any database. Then, we parse once the DB to
      // convert it into a compact int (an interval 0..N-1) so that we can
      // parse it very quickly
      __raw_translators.insertTranslator(Col<0>(), __database.nbVariables());

      auto raw_filter =
          make_DB_row_filter(__database, __raw_translators, __generators);

      __raw_translators = raw_filter.translatorSet();
      __modalities = raw_filter.modalities();

      // create the fast translators
      DBTransformCompactInt raw2fast_transfo;
      raw2fast_transfo.transform(raw_filter);

      __translators.insertTranslator(CellTranslatorCompactIntId(false), Col<0>(),
                                     __database.nbVariables());

      // create the row filter using the fast translators
      __row_filter =
          new DBRowFilter<DatabaseVectInRAM::Handler,
                          DBRowTranslatorSetDynamic<CellTranslatorCompactIntId>,
                          FilteredRowGeneratorSet<RowGeneratorIdentity>>(
              __database.handler(), __translators, __generators);
      __translators = __row_filter->translatorSet();

      // fill the variable name -> nodeid hashtable
      const std::vector<std::string> &var_names = __database.variableNames();
      unsigned int id = 0;

      for (const auto &name : var_names) {
        __name2nodeId.insert(const_cast<std::string &>(name), id);
        ++id;
      }
    }

    /// Database default constructor
    genericBNLearner::Database::Database(
        std::string filename, const NodeProperty<Sequence<std::string>> &modalities,
        bool check_database)
        : __database(genericBNLearner::__readFile(filename)),
          __generators(RowGeneratorIdentity()) {
      // create the RowFilter used for learning: we first generate a universal
      // filter that can parse any database. Then, we parse once the DB to
      // convert it into a compact int (an interval 0..N-1) so that we can
      // parse it very quickly
      CellTranslatorUniversal dummy_translator(
          Sequence<std::string>(),
          true); // by default, check the database
      __raw_translators.insertTranslator(dummy_translator, Col<0>(),
                                         __database.nbVariables());

      // assign the user values to the raw translators
      for (auto iter = modalities.cbegin(); iter != modalities.cend(); ++iter) {
        __raw_translators[iter.key()].setUserValues(iter.val(), check_database);
      }

      auto raw_filter =
          make_DB_row_filter(__database, __raw_translators, __generators);
      __raw_translators = raw_filter.translatorSet();

      // check that the database complies with the modalities specified by the
      // user. Notably, if the db contains numbers that correspond to strings
      // specified by the user, map them into strings
      {
        DBHandler &handler = raw_filter.handler();
        const unsigned long db_size = handler.DBSize();

        // determine the number of threads to use for the parsing
        unsigned int max_nb_threads =
            std::max(1UL, std::min(db_size / __min_nb_rows_per_thread,
                                   (unsigned long)__max_threads_number));

        const unsigned long max_size_per_thread =
            (db_size + max_nb_threads - 1) / max_nb_threads;

        max_nb_threads = db_size / max_size_per_thread;

        std::vector<DatabaseVectInRAM::Handler> handlers(max_nb_threads,
                                                         __database.handler());

        // as we shall not raise exception inside OMP threads, we shall keep
        // track of the errors and raise exceptions after OMP threads have
        // completed their job
        std::vector<std::pair<int, std::string>> errors(
            max_nb_threads, std::pair<int, std::string>(-1, ""));

#pragma omp parallel num_threads(max_nb_threads)
        {
          // use the ith handler
          const unsigned int num_threads = getNumberOfRunningThreads();
          const int this_thread = getThreadNumber();
          DBHandler &the_handler = handlers[this_thread];

          // indicate to the filter which part of the database it must parse
          const unsigned long size_per_thread =
              (db_size + num_threads - 1) / num_threads;
          const unsigned long min_range = size_per_thread * this_thread;
          const unsigned long max_range =
              std::min(min_range + size_per_thread, db_size);

          if (min_range < max_range) {
            bool has_errors = false;

            for (the_handler.setRange(min_range, max_range);
                 the_handler.hasRows() && !has_errors; the_handler.nextRow()) {
              DBRow &row = the_handler.row();

              for (auto iter = modalities.cbegin(); iter != modalities.cend();
                   ++iter) {
                const unsigned int i = iter.key();

                switch (row[i].type()) {
                  case DBCell::EltType::STRING:
                    if (!iter.val().exists(row[i].getString())) {
                      std::stringstream str;
                      str << "Column " << 1 + iter.key() << " contains modality '"
                          << row[i].getString()
                          << "' which has not been specified by the user in line "
                          << the_handler.numRow();
                      errors[this_thread].first = i;
                      errors[this_thread].second = str.str();
                      has_errors = true;
                    }

                    break;

                  case DBCell::EltType::MISSING:
                    break;

                  case DBCell::EltType::FLOAT: {
                    std::stringstream str;
                    str << row[i].getFloat();

                    if (!iter.val().exists(str.str())) {
                      std::stringstream str2;
                      str2 << "Column " << 1 + iter.key() << " contains modality '"
                           << str.str()
                           << "' which has not been specified by the user in line "
                           << the_handler.numRow();
                      errors[this_thread].first = i;
                      errors[this_thread].second = str2.str();
                      has_errors = true;
                    } else {
                      row[i].setStringSafe(str.str());
                    }
                  } break;

                  default:
                    GUM_ERROR(TypeError, "type not supported by DBCell convertType");
                }
              }
            }
          }
        }

        // raise an exception if needed
        for (const auto &error : errors) {
          if (error.first != -1) {
            GUM_ERROR(UnknownLabelInDatabase, error.second);
          }
        }
      }

      // get the modalities of the filters
      __modalities = raw_filter.modalities();

      // create the fast translators
      DBTransformCompactInt raw2fast_transfo;
      raw2fast_transfo.transform(raw_filter);

      __translators.insertTranslator(CellTranslatorCompactIntId(false), Col<0>(),
                                     __database.nbVariables());

      // create the row filter using the fast translators
      __row_filter =
          new DBRowFilter<DatabaseVectInRAM::Handler,
                          DBRowTranslatorSetDynamic<CellTranslatorCompactIntId>,
                          FilteredRowGeneratorSet<RowGeneratorIdentity>>(
              __database.handler(), __translators, __generators);
      __translators = __row_filter->translatorSet();

      // fill the variable name -> nodeid hashtable
      const std::vector<std::string> &var_names = __database.variableNames();
      unsigned int id = 0;

      for (const auto &name : var_names) {
        __name2nodeId.insert(const_cast<std::string &>(name), id);
        ++id;
      }
    }

    /// Database default constructor
    genericBNLearner::Database::Database(std::string filename,
                                         Database &score_database)
        : __database(genericBNLearner::__readFile(filename)),
          __generators(RowGeneratorIdentity()) {
      // check that there are at least as many variables in the a priori
      // database as those in the score_database
      if (__database.nbVariables() < score_database.__database.nbVariables()) {
        GUM_ERROR(InvalidArgument, "the a priori seems to have fewer variables "
                                   "than the observed database");
      }

      const std::vector<std::string> &score_vars =
          score_database.__database.variableNames();
      const std::vector<std::string> &apriori_vars = __database.variableNames();

      for (unsigned int i = 0, size = score_vars.size(); i < size; ++i) {
        if (score_vars[i] != apriori_vars[i]) {
          GUM_ERROR(InvalidArgument, "some a priori variables do not match "
                                     "their counterpart in the score database");
        }
      }

      // create the RowFilter used for learning: we first generate a universal
      // filter that can parse any database. Then, we parse once the DB to
      // convert it into a compact int (an interval 0..N-1) so that we can
      // parse it very quickly. We first copy the raw translators of the score
      // database so that we take into account the values observed in this
      // database. Then we copy back these raw translators so that the score
      // filter knows about these values.
      __raw_translators = score_database.__raw_translators;
      auto raw_filter =
          make_DB_row_filter(__database, __raw_translators, __generators);
      __raw_translators = raw_filter.translatorSet();
      score_database.__raw_translators = raw_filter.translatorSet();

      // update the modalities of the two databases
      __modalities = raw_filter.modalities();
      score_database.__modalities = __modalities;

      // create the fast translators
      DBTransformCompactInt raw2fast_transfo;
      raw2fast_transfo.transform(raw_filter);

      __translators.insertTranslator(CellTranslatorCompactIntId(false), Col<0>(),
                                     score_database.__database.nbVariables());

      __row_filter =
          new DBRowFilter<DatabaseVectInRAM::Handler,
                          DBRowTranslatorSetDynamic<CellTranslatorCompactIntId>,
                          FilteredRowGeneratorSet<RowGeneratorIdentity>>(
              __database.handler(), __translators, __generators);
      __translators = __row_filter->translatorSet();

      __name2nodeId = score_database.__name2nodeId;
    }

    /// Database default constructor
    genericBNLearner::Database::Database(
        std::string filename, Database &score_database,
        const NodeProperty<Sequence<std::string>> &modalities)
        : __database(genericBNLearner::__readFile(filename)),
          __generators(RowGeneratorIdentity()) {
      GUM_ERROR(OperationNotAllowed,
                "Learners with both Dirichlet apriori and "
                "variables' modalities specified are not implemented yet");
    }

    /// prevent copy constructor
    genericBNLearner::Database::Database(const Database &from)
        : __database(from.__database), __raw_translators(from.__raw_translators),
          __translators(from.__translators), __generators(from.__generators),
          __modalities(from.__modalities), __name2nodeId(from.__name2nodeId) {
      // create the row filter for the __database
      __row_filter =
          new DBRowFilter<DatabaseVectInRAM::Handler,
                          DBRowTranslatorSetDynamic<CellTranslatorCompactIntId>,
                          FilteredRowGeneratorSet<RowGeneratorIdentity>>(
              __database.handler(), __translators, __generators);
    }

    /// prevent move constructor
    genericBNLearner::Database::Database(Database &&from)
        : __database(std::move(from.__database)),
          __raw_translators(std::move(from.__raw_translators)),
          __translators(std::move(from.__translators)),
          __generators(std::move(from.__generators)),
          __modalities(std::move(from.__modalities)),
          __name2nodeId(std::move(from.__name2nodeId)) {
      // create the row filter for the __database
      __row_filter =
          new DBRowFilter<DatabaseVectInRAM::Handler,
                          DBRowTranslatorSetDynamic<CellTranslatorCompactIntId>,
                          FilteredRowGeneratorSet<RowGeneratorIdentity>>(
              __database.handler(), __translators, __generators);
    }

    /// destructor
    genericBNLearner::Database::~Database() { delete __row_filter; }

    /// copy operator
    genericBNLearner::Database &genericBNLearner::Database::
    operator=(const Database &from) {
      if (this != &from) {
        delete __row_filter;
        __row_filter = nullptr;
        __database = from.__database;
        __raw_translators = from.__raw_translators;
        __translators = from.__translators;
        __generators = from.__generators;
        __modalities = from.__modalities;
        __name2nodeId = from.__name2nodeId;

        // create the row filter for the __database
        __row_filter =
            new DBRowFilter<DatabaseVectInRAM::Handler,
                            DBRowTranslatorSetDynamic<CellTranslatorCompactIntId>,
                            FilteredRowGeneratorSet<RowGeneratorIdentity>>(
                __database.handler(), __translators, __generators);
      }

      return *this;
    }

    /// move operator
    genericBNLearner::Database &genericBNLearner::Database::
    operator=(Database &&from) {
      if (this != &from) {
        delete __row_filter;
        __row_filter = nullptr;
        __database = std::move(from.__database);
        __raw_translators = std::move(from.__raw_translators);
        __translators = std::move(from.__translators);
        __generators = std::move(from.__generators);
        __modalities = std::move(from.__modalities);
        __name2nodeId = std::move(from.__name2nodeId);

        // create the row filter for the __database
        __row_filter =
            new DBRowFilter<DatabaseVectInRAM::Handler,
                            DBRowTranslatorSetDynamic<CellTranslatorCompactIntId>,
                            FilteredRowGeneratorSet<RowGeneratorIdentity>>(
                __database.handler(), __translators, __generators);
      }

      return *this;
    }

    // ===========================================================================

    /// default constructor
    genericBNLearner::genericBNLearner(const std::string &filename)
        : __score_database(filename) {
      // for debugging purposes
      GUM_CONSTRUCTOR(genericBNLearner);
    }

    /// default constructor
    genericBNLearner::genericBNLearner(
        const std::string &filename,
        const NodeProperty<Sequence<std::string>> &modalities, bool parse_database)
        : __score_database(filename, modalities, parse_database),
          __user_modalities(modalities), __modalities_parse_db(parse_database) {
      // for debugging purposes
      GUM_CONSTRUCTOR(genericBNLearner);
    }

    /// copy constructor
    genericBNLearner::genericBNLearner(const genericBNLearner &from)
        : __score_type(from.__score_type),
          __param_estimator_type(from.__param_estimator_type),
          __apriori_type(from.__apriori_type),
          __apriori_weight(from.__apriori_weight),
          __constraint_SliceOrder(from.__constraint_SliceOrder),
          __constraint_Indegree(from.__constraint_Indegree),
          __constraint_TabuList(from.__constraint_TabuList),
          __constraint_ForbiddenArcs(from.__constraint_ForbiddenArcs),
          __constraint_MandatoryArcs(from.__constraint_MandatoryArcs),
          __selected_algo(from.__selected_algo), __K2(from.__K2),
          __greedy_hill_climbing(from.__greedy_hill_climbing),
          __local_search_with_tabu_list(from.__local_search_with_tabu_list),
          __score_database(from.__score_database),
          __user_modalities(from.__user_modalities),
          __modalities_parse_db(from.__modalities_parse_db),
          __apriori_dbname(from.__apriori_dbname),
          __initial_dag(from.__initial_dag) {
      // for debugging purposes
      GUM_CONS_CPY(genericBNLearner);
    }

    /// move constructor
    genericBNLearner::genericBNLearner(genericBNLearner &&from)
        : __score_type(from.__score_type),
          __param_estimator_type(from.__param_estimator_type),
          __apriori_type(from.__apriori_type),
          __apriori_weight(from.__apriori_weight),
          __constraint_SliceOrder(std::move(from.__constraint_SliceOrder)),
          __constraint_Indegree(std::move(from.__constraint_Indegree)),
          __constraint_TabuList(std::move(from.__constraint_TabuList)),
          __constraint_ForbiddenArcs(std::move(from.__constraint_ForbiddenArcs)),
          __constraint_MandatoryArcs(std::move(from.__constraint_MandatoryArcs)),
          __selected_algo(from.__selected_algo), __K2(std::move(from.__K2)),
          __greedy_hill_climbing(std::move(from.__greedy_hill_climbing)),
          __local_search_with_tabu_list(
              std::move(from.__local_search_with_tabu_list)),
          __score_database(std::move(from.__score_database)),
          __user_modalities(std::move(from.__user_modalities)),
          __modalities_parse_db(from.__modalities_parse_db),
          __apriori_dbname(std::move(from.__apriori_dbname)),
          __initial_dag(std::move(from.__initial_dag)) {
      // for debugging purposes
      GUM_CONS_MOV(genericBNLearner);
    }

    /// destructor
    genericBNLearner::~genericBNLearner() {
      if (__score)
        delete __score;

      if (__param_estimator)
        delete __param_estimator;

      if (__apriori)
        delete __apriori;

      if (__apriori_database)
        delete __apriori_database;

      GUM_DESTRUCTOR(genericBNLearner);
    }

    /// copy operator
    genericBNLearner &genericBNLearner::operator=(const genericBNLearner &from) {
      if (this != &from) {
        if (__score) {
          delete __score;
          __score = nullptr;
        }

        if (__param_estimator) {
          delete __param_estimator;
          __param_estimator = nullptr;
        }

        if (__apriori) {
          delete __apriori;
          __apriori = nullptr;
        }

        if (__apriori_database) {
          delete __apriori_database;
          __apriori_database = nullptr;
        }

        __score_type = from.__score_type;
        __param_estimator_type = from.__param_estimator_type;
        __apriori_type = from.__apriori_type;
        __apriori_weight = from.__apriori_weight;
        __constraint_SliceOrder = from.__constraint_SliceOrder;
        __constraint_Indegree = from.__constraint_Indegree;
        __constraint_TabuList = from.__constraint_TabuList;
        __constraint_ForbiddenArcs = from.__constraint_ForbiddenArcs;
        __constraint_MandatoryArcs = from.__constraint_MandatoryArcs;
        __selected_algo = from.__selected_algo;
        __K2 = from.__K2;
        __greedy_hill_climbing = from.__greedy_hill_climbing;
        __local_search_with_tabu_list = from.__local_search_with_tabu_list;
        __score_database = from.__score_database;
        __user_modalities = from.__user_modalities;
        __modalities_parse_db = from.__modalities_parse_db;
        __apriori_dbname = from.__apriori_dbname;
        __initial_dag = from.__initial_dag;
        __current_algorithm = nullptr;
      }

      return *this;
    }

    /// move operator
    genericBNLearner &genericBNLearner::operator=(genericBNLearner &&from) {
      if (this != &from) {
        if (__score) {
          delete __score;
          __score = nullptr;
        }

        if (__param_estimator) {
          delete __param_estimator;
          __param_estimator = nullptr;
        }

        if (__apriori) {
          delete __apriori;
          __apriori = nullptr;
        }

        if (__apriori_database) {
          delete __apriori_database;
          __apriori_database = nullptr;
        }

        __score_type = from.__score_type;
        __param_estimator_type = from.__param_estimator_type;
        __apriori_type = from.__apriori_type;
        __apriori_weight = from.__apriori_weight;
        __constraint_SliceOrder = std::move(from.__constraint_SliceOrder);
        __constraint_Indegree = std::move(from.__constraint_Indegree);
        __constraint_TabuList = std::move(from.__constraint_TabuList);
        __constraint_ForbiddenArcs = std::move(from.__constraint_ForbiddenArcs);
        __constraint_MandatoryArcs = std::move(from.__constraint_MandatoryArcs);
        __selected_algo = from.__selected_algo;
        __K2 = from.__K2;
        __greedy_hill_climbing = std::move(from.__greedy_hill_climbing);
        __local_search_with_tabu_list =
            std::move(from.__local_search_with_tabu_list);
        __score_database = std::move(from.__score_database);
        __user_modalities = std::move(from.__user_modalities);
        __modalities_parse_db = from.__modalities_parse_db;
        __apriori_dbname = std::move(from.__apriori_dbname);
        __initial_dag = std::move(from.__initial_dag);
        __current_algorithm = nullptr;
      }

      return *this;
    }

    /// reads a file and returns a databaseVectInRam
    DatabaseVectInRAM genericBNLearner::__readFile(const std::string &filename) {
      // get the extension of the file
      int filename_size = filename.size();

      if (filename_size < 4) {
        GUM_ERROR(FormatNotFound, "genericBNLearner could not determine the "
                                  "file type of the database");
      }

      std::string extension = filename.substr(filename.size() - 4);
      std::transform(extension.begin(), extension.end(), extension.begin(),
                     ::tolower);

      if (extension == ".csv") {
        return DatabaseFromCSV(filename);
      }

      GUM_ERROR(OperationNotAllowed,
                "genericBNLearner does not support yet this type of database file");
    }

    /// create the apriori used for learning
    void genericBNLearner::__createApriori() {
      // first, save the old apriori, to be delete if everything is ok
      Apriori<> *old_apriori = __apriori;

      // create the new apriori
      switch (__apriori_type) {
        case AprioriType::NO_APRIORI:
          __apriori = new AprioriNoApriori<>;
          break;

        case AprioriType::SMOOTHING:
          __apriori = new AprioriSmoothing<>;
          break;

        case AprioriType::DIRICHLET_FROM_DATABASE:
          if (__apriori_database != nullptr) {
            delete __apriori_database;
            __apriori_database = nullptr;
          }

          if (__user_modalities.empty()) {
            __apriori_database = new Database(__apriori_dbname, __score_database);
          } else {
            __apriori_database =
                new Database(__apriori_dbname, __score_database, __user_modalities);
          }

          __apriori = new AprioriDirichletFromDatabase<>(
              __apriori_database->rowFilter(), __apriori_database->modalities());
          break;

        default:
          GUM_ERROR(OperationNotAllowed,
                    "genericBNLearner does not support yet this apriori");
      }

      // do not forget to assign a weight to the apriori
      __apriori->setWeight(__apriori_weight);

      // remove the old apriori, if any
      if (old_apriori != nullptr)
        delete old_apriori;
    }

    /// create the score used for learning
    void genericBNLearner::__createScore() {
      // first, save the old score, to be delete if everything is ok
      Score<> *old_score = __score;

      // create the new scoring function
      switch (__score_type) {
        case ScoreType::AIC:
          __score = new ScoreAIC<>(__score_database.rowFilter(),
                                   __score_database.modalities(), *__apriori);
          break;

        case ScoreType::BD:
          __score = new ScoreBD<>(__score_database.rowFilter(),
                                  __score_database.modalities(), *__apriori);
          break;

        case ScoreType::BDeu:
          __score = new ScoreBDeu<>(__score_database.rowFilter(),
                                    __score_database.modalities(), *__apriori);
          break;

        case ScoreType::BIC:
          __score = new ScoreBIC<>(__score_database.rowFilter(),
                                   __score_database.modalities(), *__apriori);
          break;

        case ScoreType::K2:
          __score = new ScoreK2<>(__score_database.rowFilter(),
                                  __score_database.modalities(), *__apriori);
          break;

        case ScoreType::LOG2LIKELIHOOD:
          __score =
              new ScoreLog2Likelihood<>(__score_database.rowFilter(),
                                        __score_database.modalities(), *__apriori);
          break;

        default:
          GUM_ERROR(OperationNotAllowed,
                    "genericBNLearner does not support yet this score");
      }

      // remove the old score, if any
      if (old_score != nullptr)
        delete old_score;
    }

    /// create the parameter estimator used for learning
    void genericBNLearner::__createParamEstimator(bool take_into_account_score) {
      // first, save the old estimator, to be delete if everything is ok
      ParamEstimator<> *old_estimator = __param_estimator;

      // create the new estimator
      switch (__param_estimator_type) {
        case ParamEstimatorType::ML:
          if (take_into_account_score && (__score != nullptr)) {
            __param_estimator = new ParamEstimatorML<>(
                __score_database.rowFilter(), __score_database.modalities(),
                *__apriori, __score->internalApriori());
          } else {
            __param_estimator =
                new ParamEstimatorML<>(__score_database.rowFilter(),
                                       __score_database.modalities(), *__apriori);
          }

          break;

        default:
          GUM_ERROR(
              OperationNotAllowed,
              "genericBNLearner does not support yet this parameter estimator");
      }

      // remove the old estimator, if any
      if (old_estimator != nullptr)
        delete old_estimator;
    }

    /// learn a structure from a file
    DAG genericBNLearner::learnDAG() {
      // create the score and the apriori
      __createApriori();
      __createScore();

      return __learnDAG();
    }

    /// learn a structure from a file
    DAG genericBNLearner::__learnDAG() {
      // add the mandatory arcs to the initial dag and remove the forbidden ones
      // from the initial graph
      DAG init_graph = __initial_dag;

      const ArcSet &mandatory_arcs = __constraint_MandatoryArcs.arcs();

      for (const auto &arc : mandatory_arcs) {
        if (!init_graph.exists(arc.tail()))
          init_graph.addNode(arc.tail());

        if (!init_graph.exists(arc.head()))
          init_graph.addNode(arc.head());

        init_graph.addArc(arc.tail(), arc.head());
      }

      const ArcSet &forbidden_arcs = __constraint_ForbiddenArcs.arcs();

      for (const auto &arc : forbidden_arcs) {
        init_graph.eraseArc(arc);
      }

      switch (__selected_algo) {
        // ========================================================================
        case AlgoType::GREEDY_HILL_CLIMBING: {
          BNLearnerListener listener(this, __greedy_hill_climbing);
          StructuralConstraintSetStatic<
              StructuralConstraintMandatoryArcs, StructuralConstraintForbiddenArcs,
              StructuralConstraintSliceOrder> gen_constraint;
          static_cast<StructuralConstraintMandatoryArcs &>(gen_constraint) =
              __constraint_MandatoryArcs;
          static_cast<StructuralConstraintForbiddenArcs &>(gen_constraint) =
              __constraint_ForbiddenArcs;
          static_cast<StructuralConstraintSliceOrder &>(gen_constraint) =
              __constraint_SliceOrder;

          GraphChangesGenerator4DiGraph<decltype(gen_constraint)> op_set(
              gen_constraint);

          StructuralConstraintSetStatic<StructuralConstraintIndegree,
                                        StructuralConstraintDAG> sel_constraint;
          static_cast<StructuralConstraintIndegree &>(sel_constraint) =
              __constraint_Indegree;

          GraphChangesSelector4DiGraph<Score<>, decltype(sel_constraint),
                                       decltype(op_set)> selector(*__score,
                                                                  sel_constraint,
                                                                  op_set);

          return __greedy_hill_climbing.learnStructure(
              selector, __score_database.modalities(), init_graph);
        }

        // ========================================================================
        case AlgoType::LOCAL_SEARCH_WITH_TABU_LIST: {
          BNLearnerListener listener(this, __local_search_with_tabu_list);
          StructuralConstraintSetStatic<
              StructuralConstraintMandatoryArcs, StructuralConstraintForbiddenArcs,
              StructuralConstraintSliceOrder> gen_constraint;
          static_cast<StructuralConstraintMandatoryArcs &>(gen_constraint) =
              __constraint_MandatoryArcs;
          static_cast<StructuralConstraintForbiddenArcs &>(gen_constraint) =
              __constraint_ForbiddenArcs;
          static_cast<StructuralConstraintSliceOrder &>(gen_constraint) =
              __constraint_SliceOrder;

          GraphChangesGenerator4DiGraph<decltype(gen_constraint)> op_set(
              gen_constraint);

          StructuralConstraintSetStatic<StructuralConstraintTabuList,
                                        StructuralConstraintIndegree,
                                        StructuralConstraintDAG> sel_constraint;
          static_cast<StructuralConstraintTabuList &>(sel_constraint) =
              __constraint_TabuList;
          static_cast<StructuralConstraintIndegree &>(sel_constraint) =
              __constraint_Indegree;

          GraphChangesSelector4DiGraph<Score<>, decltype(sel_constraint),
                                       decltype(op_set)> selector(*__score,
                                                                  sel_constraint,
                                                                  op_set);

          return __local_search_with_tabu_list.learnStructure(
              selector, __score_database.modalities(), init_graph);
        }

        // ========================================================================
        case AlgoType::K2: {
          BNLearnerListener listener(this, __K2.approximationScheme());
          StructuralConstraintSetStatic<StructuralConstraintMandatoryArcs,
                                        StructuralConstraintForbiddenArcs>
              gen_constraint;
          static_cast<StructuralConstraintMandatoryArcs &>(gen_constraint) =
              __constraint_MandatoryArcs;
          static_cast<StructuralConstraintForbiddenArcs &>(gen_constraint) =
              __constraint_ForbiddenArcs;

          GraphChangesGenerator4K2<decltype(gen_constraint)> op_set(gen_constraint);

          // if some mandatory arcs are incompatible with the order, use a DAG
          // constraint instead of a DiGraph constraint to avoid cycles
          const ArcSet &mandatory_arcs =
              static_cast<StructuralConstraintMandatoryArcs &>(gen_constraint)
                  .arcs();
          const Sequence<NodeId> &order = __K2.order();
          bool order_compatible = true;

          for (const auto &arc : mandatory_arcs) {
            if (order.pos(arc.tail()) >= order.pos(arc.head())) {
              order_compatible = false;
              break;
            }
          }

          if (order_compatible) {
            StructuralConstraintSetStatic<StructuralConstraintIndegree,
                                          StructuralConstraintDiGraph>
                sel_constraint;
            static_cast<StructuralConstraintIndegree &>(sel_constraint) =
                __constraint_Indegree;

            GraphChangesSelector4DiGraph<Score<>, decltype(sel_constraint),
                                         decltype(op_set)> selector(*__score,
                                                                    sel_constraint,
                                                                    op_set);

            return __K2.learnStructure(selector, __score_database.modalities(),
                                       init_graph);
          } else {
            StructuralConstraintSetStatic<StructuralConstraintIndegree,
                                          StructuralConstraintDAG> sel_constraint;
            static_cast<StructuralConstraintIndegree &>(sel_constraint) =
                __constraint_Indegree;

            GraphChangesSelector4DiGraph<Score<>, decltype(sel_constraint),
                                         decltype(op_set)> selector(*__score,
                                                                    sel_constraint,
                                                                    op_set);

            return __K2.learnStructure(selector, __score_database.modalities(),
                                       init_graph);
          }
        }

        // ========================================================================
        default:
          GUM_ERROR(OperationNotAllowed,
                    "the learnDAG method has not been implemented for this "
                    "learning algorithm");
      }
    }

    /// checks whether the current score and apriori are compatible
    bool genericBNLearner::__checkScoreAprioriCompatibility() {
      const std::string &apriori = __getAprioriType();

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

        default:
          GUM_ERROR(OperationNotAllowed,
                    "genericBNLearner does not support yet this score");
      }
    }

  } /* namespace learning */

} /* namespace gum */
