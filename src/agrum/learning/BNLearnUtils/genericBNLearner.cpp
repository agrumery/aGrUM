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
 * The pack currently contains K2, GreedyHillClimbing, miic, 3off2 and
 *LocalSearchWithTabuList
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <algorithm>

#include <agrum/agrum.h>
#include <agrum/learning/BNLearnUtils/BNLearnerListener.h>
#include <agrum/learning/BNLearnUtils/genericBNLearner.h>

// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#  include <agrum/learning/BNLearnUtils/genericBNLearner_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {


    genericBNLearner::Database::Database(const DatabaseTable<>& db) :
        __database(db) {
      // get the variables names
      const auto&       var_names = __database.variableNames();
      const std::size_t nb_vars = var_names.size();
      __domain_sizes.resize(nb_vars);
      const auto domainSizes = __database.domainSizes();
      for (std::size_t i = 0; i < nb_vars; ++i) {
        __nodeId2cols.insert(NodeId(i),i);
        __domain_sizes[i] = Size(domainSizes[i]);
      }

      // create the parser
      __parser =
        new DBRowGeneratorParser<>(__database.handler(), DBRowGeneratorSet<>());
    }


    genericBNLearner::Database::Database(
      const std::string&                filename,
      const std::vector< std::string >& missing_symbols) :
        Database(genericBNLearner::__readFile(filename, missing_symbols)) {}


    /*
    genericBNLearner::Database::Database(
      std::string                                    filename,
      const NodeProperty< Sequence< std::string > >& modalities,
      bool                                           check_database)
        : __database(genericBNLearner::__readFile(filename)) {

      // #### TODO: change the domain sizes of the variables

      // get the variables names
      const auto& var_names = __database.variableNames ();
      const std::size_t nb_vars = var_names.size ();
      for ( std::size_t i = 0; i < nb_vars; ++i )
        __name2nodeId.insert ( var_names[i], i );

      // get the domain sizes of the variables
      __domain_sizes.resize ( nb_vars );
      for ( std::size_t i = 0; i < nb_vars; ++i ) {
        const DiscreteVariable& var =
          static_cast<const DiscreteVariable&> __database.variable ( i );
        __domain_sizes[i] = var.domainSize ();
      }

      // create the parser
      __parser = new DBRowGeneratorParser<> ( __database.handler (),
                                              DBRowGeneratorSet<> () );
    }
    */


    genericBNLearner::Database::Database(
      const std::string&                filename,
      Database&                         score_database,
      const std::vector< std::string >& missing_symbols) :
        __database(genericBNLearner::__readFile(filename, missing_symbols)) {
      // check that there are at least as many variables in the a priori
      // database as those in the score_database
      if (__database.nbVariables() < score_database.__database.nbVariables()) {
        GUM_ERROR(InvalidArgument,
                  "the a apriori database has fewer variables "
                  "than the observed database");
      }

      const auto& score_nodeId2cols = score_database.nodeId2Columns();
      const std::vector< std::string >& apriori_vars = __database.variableNames();
      
      Size size = Size(apriori_vars.size());
      __domain_sizes.resize(size);
      const auto domainSizes = __database.domainSizes();
      for (Idx i = 0; i < size; ++i) {
        std::size_t col = std::size_t(0);
        try {
          const auto cols =
            score_database.__database.columnsFromVariableName(apriori_vars[i]);
          col = cols[0];
        }
        catch (...) {
           GUM_ERROR(InvalidArgument,
                     "A priori Variable "
                     << apriori_vars[i]
                     << " does not belong to the observed database");
        }
        
        // check that the domain size of the variable is lower than or
        // equal to that of the original variable
        if (score_database.__database.domainSize(col) < __database.domainSize(i)) {
          GUM_ERROR(InvalidArgument,
                    "A priori Variable "
                    << apriori_vars[i] << " has a domain size of "
                    << __database.domainSize(i)
                    << ", which is higher than that of the same variable in "
                    << "the observed database, which is equal to "
                    << score_database.__database.domainSize(col));
        }

        const NodeId id = score_nodeId2cols.first(col);
        __nodeId2cols.insert(id,i);
        __domain_sizes[i] = Size(domainSizes[i]);
      }

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

    genericBNLearner::Database& genericBNLearner::Database::
                                operator=(const Database& from) {
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

    genericBNLearner::Database& genericBNLearner::Database::
                                operator=(Database&& from) {
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
    

    /// returns the mapping between node ids and their columns in the database
    const Bijection< NodeId, std::size_t >&
    genericBNLearner::Database::nodeId2Columns() const {
      return __nodeId2cols;
    }
    


    // ===========================================================================

    genericBNLearner::genericBNLearner(
      const std::string&                filename,
      const std::vector< std::string >& missing_symbols) :
      __score_database(filename, missing_symbols) {
      __no_apriori = new AprioriNoApriori2<> (__score_database.databaseTable());

      // for debugging purposes
      GUM_CONSTRUCTOR(genericBNLearner);
    }


    genericBNLearner::genericBNLearner(const DatabaseTable<>& db) :
      __score_database(db) {
      __no_apriori = new AprioriNoApriori2<> (__score_database.databaseTable());
      
      // for debugging purposes
      GUM_CONSTRUCTOR(genericBNLearner);
    }


    /*

    genericBNLearner::genericBNLearner(
      const std::string&                             filename,
      const NodeProperty< Sequence< std::string > >& modalities,
      bool                                           parse_database)
        : __score_database(filename, modalities, parse_database)
        , __user_modalities(modalities)
        , __domain_sizes_parse_db(parse_database) {
      // for debugging purposes
      GUM_CONSTRUCTOR(genericBNLearner);
    }

    */


    genericBNLearner::genericBNLearner(const genericBNLearner& from) :
        __score_type(from.__score_type),
        __param_estimator_type(from.__param_estimator_type),
        __apriori_type(from.__apriori_type),
        __apriori_weight(from.__apriori_weight),
        __constraint_SliceOrder(from.__constraint_SliceOrder),
        __constraint_Indegree(from.__constraint_Indegree),
        __constraint_TabuList(from.__constraint_TabuList),
        __constraint_ForbiddenArcs(from.__constraint_ForbiddenArcs),
        __constraint_MandatoryArcs(from.__constraint_MandatoryArcs),
        __selected_algo(from.__selected_algo), __K2(from.__K2),
        __miic_3off2(from.__miic_3off2),
        __greedy_hill_climbing(from.__greedy_hill_climbing),
        __local_search_with_tabu_list(from.__local_search_with_tabu_list),
        __score_database(from.__score_database),
        __apriori_dbname(from.__apriori_dbname),
        __initial_dag(from.__initial_dag) {
      __no_apriori = new AprioriNoApriori2<> (__score_database.databaseTable());

      // for debugging purposes
      GUM_CONS_CPY(genericBNLearner);
    }

    genericBNLearner::genericBNLearner(genericBNLearner&& from) :
        __score_type(from.__score_type),
        __param_estimator_type(from.__param_estimator_type),
        __apriori_type(from.__apriori_type),
        __apriori_weight(from.__apriori_weight),
        __constraint_SliceOrder(std::move(from.__constraint_SliceOrder)),
        __constraint_Indegree(std::move(from.__constraint_Indegree)),
        __constraint_TabuList(std::move(from.__constraint_TabuList)),
        __constraint_ForbiddenArcs(std::move(from.__constraint_ForbiddenArcs)),
        __constraint_MandatoryArcs(std::move(from.__constraint_MandatoryArcs)),
        __selected_algo(from.__selected_algo), __K2(std::move(from.__K2)),
        __miic_3off2(std::move(from.__miic_3off2)),
        __greedy_hill_climbing(std::move(from.__greedy_hill_climbing)),
        __local_search_with_tabu_list(
          std::move(from.__local_search_with_tabu_list)),
        __score_database(std::move(from.__score_database)),
        __apriori_dbname(std::move(from.__apriori_dbname)),
        __initial_dag(std::move(from.__initial_dag)) {
      __no_apriori = new AprioriNoApriori2<> (__score_database.databaseTable());

      // for debugging purposes
      GUM_CONS_MOV(genericBNLearner);
    }

    genericBNLearner::~genericBNLearner() {
      if (__score) delete __score;

      if (__param_estimator) delete __param_estimator;

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

        if (__mutual_info) {
          delete __mutual_info;
          __mutual_info = nullptr;
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
        __miic_3off2 = from.__miic_3off2;
        __greedy_hill_climbing = from.__greedy_hill_climbing;
        __local_search_with_tabu_list = from.__local_search_with_tabu_list;
        __score_database = from.__score_database;
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

        if (__mutual_info) {
          delete __mutual_info;
          __mutual_info = nullptr;
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
        __miic_3off2 = std::move(from.__miic_3off2);
        __greedy_hill_climbing = std::move(from.__greedy_hill_climbing);
        __local_search_with_tabu_list =
          std::move(from.__local_search_with_tabu_list);
        __score_database = std::move(from.__score_database);
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

      // check that the database does not contain any missing value
      if (database.hasMissingValues())
        GUM_ERROR(MissingValueInDatabase,
                  "For the moment, the BNLearaner is unable to cope "
                  "with missing values in databases");

      database.reorder();

      return database;
    }


    void genericBNLearner::__createApriori() {
      // first, save the old apriori, to be delete if everything is ok
      Apriori2<>* old_apriori = __apriori;

      // create the new apriori
      switch (__apriori_type) {
        case AprioriType::NO_APRIORI:
          __apriori = new AprioriNoApriori2<> (__score_database.databaseTable());
          break;

        case AprioriType::SMOOTHING:
          __apriori = new AprioriSmoothing2<> (__score_database.databaseTable());
          break;

        case AprioriType::DIRICHLET_FROM_DATABASE:
          if (__apriori_database != nullptr) {
            delete __apriori_database;
            __apriori_database = nullptr;
          }

          __apriori_database = new Database(__apriori_dbname,
                                            __score_database,
                                            __score_database.missingSymbols());
         
          __apriori = new AprioriDirichletFromDatabase2<>(
                    __score_database.databaseTable(),
                    __apriori_database->parser(),
                    __apriori_database->nodeId2Columns() );
          break;

        default:
          GUM_ERROR(OperationNotAllowed,
                    "genericBNLearner does not support yet this apriori");
      }

      // do not forget to assign a weight to the apriori
      __apriori->setWeight(__apriori_weight);

      // remove the old apriori, if any
      if (old_apriori != nullptr) delete old_apriori;
    }

    void genericBNLearner::__createScore() {
      // first, save the old score, to be delete if everything is ok
      Score2<>* old_score = __score;

      // create the new scoring function
      switch (__score_type) {
        case ScoreType::AIC:
          __score = new ScoreAIC2<>(__score_database.parser(), *__apriori);
          break;

        case ScoreType::BD:
          __score = new ScoreBD2<>( __score_database.parser(), *__apriori);
          break;

        case ScoreType::BDeu:
          __score = new ScoreBDeu2<>( __score_database.parser(), *__apriori);
          break;

        case ScoreType::BIC:
          __score = new ScoreBIC2<>( __score_database.parser(), *__apriori);
          break;

        case ScoreType::K2:
          __score = new ScoreK22<>( __score_database.parser(), *__apriori);
          break;

        case ScoreType::LOG2LIKELIHOOD:
          __score = new ScoreLog2Likelihood2<>( __score_database.parser(), *__apriori);
          break;

        default:
          GUM_ERROR(OperationNotAllowed,
                    "genericBNLearner does not support yet this score");
      }

      // remove the old score, if any
      if (old_score != nullptr) delete old_score;
    }

    void genericBNLearner::__createParamEstimator(bool take_into_account_score) {
      // first, save the old estimator, to be delete if everything is ok
      ParamEstimator2<>* old_estimator = __param_estimator;

      // create the new estimator
      switch (__param_estimator_type) {
        case ParamEstimatorType::ML:
          if (take_into_account_score && (__score != nullptr)) {
            __param_estimator =
              new ParamEstimatorML2<>(__score_database.parser(),
                                      *__apriori,
                                     __score->internalApriori());
          } else {
            __param_estimator =
              new ParamEstimatorML2<>(__score_database.parser(),
                                      *__apriori,
                                      *__no_apriori);
          }

          break;

        default:
          GUM_ERROR(OperationNotAllowed,
                    "genericBNLearner does not support "
                    "yet this parameter estimator");
      }

      // remove the old estimator, if any
      if (old_estimator != nullptr) delete old_estimator;
    }

    /// prepares the initial graph for 3off2 or miic
    MixedGraph genericBNLearner::__prepare_miic_3off2() {
      // Initialize the mixed graph to the fully connected graph
      MixedGraph mgraph;
      for (Size i = 0; i < __score_database.domainSizes().size(); ++i) {
        mgraph.addNodeWithId(i);
        for (Size j = 0; j < i; ++j) {
          mgraph.addEdge(j, i);
        }
      }

      // translating the constraints for 3off2 or miic
      HashTable< std::pair< NodeId, NodeId >, char > initial_marks;
      const ArcSet& mandatory_arcs = __constraint_MandatoryArcs.arcs();
      for (const auto& arc : mandatory_arcs) {
        initial_marks.insert({arc.tail(), arc.head()}, '>');
      }

      const ArcSet& forbidden_arcs = __constraint_ForbiddenArcs.arcs();
      for (const auto& arc : forbidden_arcs) {
        initial_marks.insert({arc.tail(), arc.head()}, '-');
      }
      __miic_3off2.addConstraints(initial_marks);
      // create the mutual entropy object
      if (__mutual_info == nullptr) { this->useNML(); }

      return mgraph;
    }

    MixedGraph genericBNLearner::learnMixedStructure() {
      if (__selected_algo != AlgoType::MIIC_THREE_OFF_TWO) {
        GUM_ERROR(OperationNotAllowed, "Must be using the miic/3off2 algorithm");
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

    DAG genericBNLearner::__learnDAG() {
      // add the mandatory arcs to the initial dag and remove the forbidden ones
      // from the initial graph
      DAG init_graph = __initial_dag;

      const ArcSet& mandatory_arcs = __constraint_MandatoryArcs.arcs();

      for (const auto& arc : mandatory_arcs) {
        if (!init_graph.exists(arc.tail())) init_graph.addNodeWithId(arc.tail());

        if (!init_graph.exists(arc.head())) init_graph.addNodeWithId(arc.head());

        init_graph.addArc(arc.tail(), arc.head());
      }

      const ArcSet& forbidden_arcs = __constraint_ForbiddenArcs.arcs();

      for (const auto& arc : forbidden_arcs) {
        init_graph.eraseArc(arc);
      }

      switch (__selected_algo) {
        // ========================================================================
        case AlgoType::MIIC_THREE_OFF_TWO: {
          BNLearnerListener listener(this, __miic_3off2);
          // create the mixedGraph
          MixedGraph mgraph = this->__prepare_miic_3off2();

          return __miic_3off2.learnStructure(*__mutual_info, mgraph);
        }
        // ========================================================================
        case AlgoType::GREEDY_HILL_CLIMBING: {
          BNLearnerListener listener(this, __greedy_hill_climbing);
          StructuralConstraintSetStatic< StructuralConstraintMandatoryArcs,
                                         StructuralConstraintForbiddenArcs,
                                         StructuralConstraintSliceOrder >
            gen_constraint;
          static_cast< StructuralConstraintMandatoryArcs& >(gen_constraint) =
            __constraint_MandatoryArcs;
          static_cast< StructuralConstraintForbiddenArcs& >(gen_constraint) =
            __constraint_ForbiddenArcs;
          static_cast< StructuralConstraintSliceOrder& >(gen_constraint) =
            __constraint_SliceOrder;

          GraphChangesGenerator4DiGraph< decltype(gen_constraint) > op_set(
            gen_constraint);

          StructuralConstraintSetStatic< StructuralConstraintIndegree,
                                         StructuralConstraintDAG >
            sel_constraint;
          static_cast< StructuralConstraintIndegree& >(sel_constraint) =
            __constraint_Indegree;

          GraphChangesSelector4DiGraph2< decltype(sel_constraint),
                                         decltype(op_set) >
            selector(*__score, sel_constraint, op_set);

          return __greedy_hill_climbing.learnStructure(
            selector, init_graph);
        }

        // ========================================================================
        case AlgoType::LOCAL_SEARCH_WITH_TABU_LIST: {
          BNLearnerListener listener(this, __local_search_with_tabu_list);
          StructuralConstraintSetStatic< StructuralConstraintMandatoryArcs,
                                         StructuralConstraintForbiddenArcs,
                                         StructuralConstraintSliceOrder >
            gen_constraint;
          static_cast< StructuralConstraintMandatoryArcs& >(gen_constraint) =
            __constraint_MandatoryArcs;
          static_cast< StructuralConstraintForbiddenArcs& >(gen_constraint) =
            __constraint_ForbiddenArcs;
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

          GraphChangesSelector4DiGraph2< decltype(sel_constraint),
                                         decltype(op_set) >
            selector(*__score, sel_constraint, op_set);

          return __local_search_with_tabu_list.learnStructure(
            selector, init_graph);
        }

        // ========================================================================
        case AlgoType::K2: {
          BNLearnerListener listener(this, __K2.approximationScheme());
          StructuralConstraintSetStatic< StructuralConstraintMandatoryArcs,
                                         StructuralConstraintForbiddenArcs >
            gen_constraint;
          static_cast< StructuralConstraintMandatoryArcs& >(gen_constraint) =
            __constraint_MandatoryArcs;
          static_cast< StructuralConstraintForbiddenArcs& >(gen_constraint) =
            __constraint_ForbiddenArcs;

          GraphChangesGenerator4K2< decltype(gen_constraint) > op_set(
            gen_constraint);

          // if some mandatory arcs are incompatible with the order, use a DAG
          // constraint instead of a DiGraph constraint to avoid cycles
          const ArcSet& mandatory_arcs =
            static_cast< StructuralConstraintMandatoryArcs& >(gen_constraint)
              .arcs();
          const Sequence< NodeId >& order = __K2.order();
          bool                      order_compatible = true;

          for (const auto& arc : mandatory_arcs) {
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

            GraphChangesSelector4DiGraph2< decltype(sel_constraint),
                                           decltype(op_set) >
              selector(*__score, sel_constraint, op_set);

            return __K2.learnStructure(
              selector, init_graph);
          } else {
            StructuralConstraintSetStatic< StructuralConstraintIndegree,
                                           StructuralConstraintDAG >
              sel_constraint;
            static_cast< StructuralConstraintIndegree& >(sel_constraint) =
              __constraint_Indegree;

            GraphChangesSelector4DiGraph2< decltype(sel_constraint),
                                           decltype(op_set) >
              selector(*__score, sel_constraint, op_set);

            return __K2.learnStructure(
              selector, init_graph);
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
          return ScoreAIC2<>::isAprioriCompatible(apriori, __apriori_weight);

        case ScoreType::BD:
          return ScoreBD2<>::isAprioriCompatible(apriori, __apriori_weight);

        case ScoreType::BDeu:
          return ScoreBDeu2<>::isAprioriCompatible(apriori, __apriori_weight);

        case ScoreType::BIC:
          return ScoreBIC2<>::isAprioriCompatible(apriori, __apriori_weight);

        case ScoreType::K2:
          return ScoreK22<>::isAprioriCompatible(apriori, __apriori_weight);

        case ScoreType::LOG2LIKELIHOOD:
          return ScoreLog2Likelihood2<>::isAprioriCompatible(apriori,
                                                             __apriori_weight);

        default: return "genericBNLearner does not support yet this score";
      }
    }

  } /* namespace learning */

} /* namespace gum */
