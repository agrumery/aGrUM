/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <algorithm>
#include <iterator>

#include <agrum/agrum.h>
#include <agrum/BN/learning/BNLearnUtils/BNLearnerListener.h>
#include <agrum/BN/learning/BNLearnUtils/IBNLearner.h>
#include <agrum/tools/stattests/indepTestChi2.h>
#include <agrum/tools/stattests/indepTestG2.h>
#include <agrum/BN/learning/scores_and_tests/scoreLog2Likelihood.h>
#include <agrum/tools/stattests/pseudoCount.h>

// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#  include <agrum/BN/learning/BNLearnUtils/IBNLearner_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum::learning {


  IBNLearner::Database::Database(const DatabaseTable& db) : _database_(db) {
    // get the variables names
    const auto&       var_names = _database_.variableNames();
    const std::size_t nb_vars   = var_names.size();
    for (auto dom: _database_.domainSizes())
      _domain_sizes_.push_back(dom);
    for (std::size_t i = 0; i < nb_vars; ++i) {
      _nodeId2cols_.insert(NodeId(i), i);
    }

    // create the parser
    _parser_ = new DBRowGeneratorParser(_database_.handler(), DBRowGeneratorSet());
  }


  IBNLearner::Database::Database(const std::string&                filename,
                                 const std::vector< std::string >& missing_symbols,
                                 const bool                        induceTypes) :
      Database(IBNLearner::readFile_(filename, missing_symbols)) {
    // if the usr wants the best translators to be inferred, just do it
    if (induceTypes) {
      for (const auto& [first, second]: _database_.betterTranslators()) {
        // change the translator
        _database_.changeTranslator(*second, first);
        // recompute the domain size
        _domain_sizes_[first] = second->domainSize();
      }
    }
  }


  IBNLearner::Database::Database(const std::string&                CSV_filename,
                                 const Database&                   score_database,
                                 const std::vector< std::string >& missing_symbols) {
    // assign to each column name in the CSV file its column
    IBNLearner::isCSVFileName_(CSV_filename);
    DBInitializerFromCSV                  initializer(CSV_filename);
    const auto&                           prior_names   = initializer.variableNames();
    std::size_t                           prior_nb_vars = prior_names.size();
    HashTable< std::string, std::size_t > prior_names2col(prior_nb_vars);
    for (auto i = std::size_t(0); i < prior_nb_vars; ++i)
      prior_names2col.insert(prior_names[i], i);

    // check that there are at least as many variables in the a priori
    // database as those in the score_database
    if (prior_nb_vars < score_database._database_.nbVariables()) {
      GUM_ERROR(InvalidArgument,
                "the a prior database has fewer variables "
                "than the observed database")
    }

    // get the mapping from the columns of score_database to those of
    // the CSV file
    const std::vector< std::string >& score_names = score_database.databaseTable().variableNames();
    const std::size_t                 score_nb_vars = score_names.size();
    HashTable< std::size_t, std::size_t > mapping(score_nb_vars);
    for (auto i = std::size_t(0); i < score_nb_vars; ++i) {
      try {
        mapping.insert(i, prior_names2col[score_names[i]]);
      } catch (Exception const&) {
        GUM_ERROR(MissingVariableInDatabase,
                  "Variable " << score_names[i]
                              << " of the observed database does not belong to the "
                              << "prior database")
      }
    }

    // create the translators for CSV database
    for (auto i = std::size_t(0); i < score_nb_vars; ++i) {
      const Variable& var = score_database.databaseTable().variable(i);
      _database_.insertTranslator(var, mapping[i], missing_symbols);
    }

    // fill the database
    initializer.fillDatabase(_database_);

    // get the domain sizes of the variables
    for (auto dom: _database_.domainSizes())
      _domain_sizes_.push_back(dom);

    // compute the mapping from node ids to column indices
    _nodeId2cols_ = score_database.nodeId2Columns();

    // create the parser
    _parser_ = new DBRowGeneratorParser(_database_.handler(), DBRowGeneratorSet());
  }


  IBNLearner::Database::Database(const Database& from) :
      _database_(from._database_), _domain_sizes_(from._domain_sizes_),
      _nodeId2cols_(from._nodeId2cols_) {
    // create the parser
    _parser_ = new DBRowGeneratorParser(_database_.handler(), DBRowGeneratorSet());
  }


  IBNLearner::Database::Database(Database&& from) :
      _database_(std::move(from._database_)), _domain_sizes_(std::move(from._domain_sizes_)),
      _nodeId2cols_(std::move(from._nodeId2cols_)) {
    // create the parser
    _parser_ = new DBRowGeneratorParser(_database_.handler(), DBRowGeneratorSet());
  }


  IBNLearner::Database::~Database() { delete _parser_; }

  IBNLearner::Database& IBNLearner::Database::operator=(const Database& from) {
    if (this != &from) {
      delete _parser_;
      _database_     = from._database_;
      _domain_sizes_ = from._domain_sizes_;
      _nodeId2cols_  = from._nodeId2cols_;

      // create the parser
      _parser_ = new DBRowGeneratorParser(_database_.handler(), DBRowGeneratorSet());
    }

    return *this;
  }

  IBNLearner::Database& IBNLearner::Database::operator=(Database&& from) {
    if (this != &from) {
      delete _parser_;
      _database_     = std::move(from._database_);
      _domain_sizes_ = std::move(from._domain_sizes_);
      _nodeId2cols_  = std::move(from._nodeId2cols_);

      // create the parser
      _parser_ = new DBRowGeneratorParser(_database_.handler(), DBRowGeneratorSet());
    }

    return *this;
  }


  // ===========================================================================

  IBNLearner::IBNLearner(const std::string&                filename,
                         const std::vector< std::string >& missing_symbols,
                         const bool                        induceTypes) :
      inducedTypes_(induceTypes),
      scoreDatabase_(filename, missing_symbols, induceTypes), filename_(filename) {
    noPrior_ = new NoPrior(scoreDatabase_.databaseTable());

    GUM_CONSTRUCTOR(IBNLearner)
  }


  IBNLearner::IBNLearner(const DatabaseTable& db) : scoreDatabase_(db) {
    noPrior_ = new NoPrior(scoreDatabase_.databaseTable());
    GUM_CONSTRUCTOR(IBNLearner)
  }


  IBNLearner::IBNLearner(const IBNLearner& from) :
      ThreadNumberManager(from), inducedTypes_(from.inducedTypes_), scoreType_(from.scoreType_),
      paramEstimatorType_(from.paramEstimatorType_), epsilonEM_(from.epsilonEM_),
      priorType_(from.priorType_), priorWeight_(from.priorWeight_),
      constraintSliceOrder_(from.constraintSliceOrder_),
      constraintIndegree_(from.constraintIndegree_), constraintTabuList_(from.constraintTabuList_),
      constraintForbiddenArcs_(from.constraintForbiddenArcs_),
      constraintMandatoryArcs_(from.constraintMandatoryArcs_), selectedAlgo_(from.selectedAlgo_),
      algoK2_(from.algoK2_), algoMiic3off2_(from.algoMiic3off2_), kmode3Off2_(from.kmode3Off2_),
      greedyHillClimbing_(from.greedyHillClimbing_),
      localSearchWithTabuList_(from.localSearchWithTabuList_), scoreDatabase_(from.scoreDatabase_),
      ranges_(from.ranges_), priorDbname_(from.priorDbname_), initialDag_(from.initialDag_),
      filename_(from.filename_), nbDecreasingChanges_(from.nbDecreasingChanges_) {
    noPrior_ = new NoPrior(scoreDatabase_.databaseTable());

    GUM_CONS_CPY(IBNLearner)
  }

  IBNLearner::IBNLearner(IBNLearner&& from) :
      ThreadNumberManager(std::move(from)), inducedTypes_(from.inducedTypes_),
      scoreType_(from.scoreType_), paramEstimatorType_(from.paramEstimatorType_),
      epsilonEM_(from.epsilonEM_), priorType_(from.priorType_), priorWeight_(from.priorWeight_),
      constraintSliceOrder_(std::move(from.constraintSliceOrder_)),
      constraintIndegree_(std::move(from.constraintIndegree_)),
      constraintTabuList_(std::move(from.constraintTabuList_)),
      constraintForbiddenArcs_(std::move(from.constraintForbiddenArcs_)),
      constraintMandatoryArcs_(std::move(from.constraintMandatoryArcs_)),
      selectedAlgo_(from.selectedAlgo_), algoK2_(std::move(from.algoK2_)),
      algoMiic3off2_(std::move(from.algoMiic3off2_)), kmode3Off2_(from.kmode3Off2_),
      greedyHillClimbing_(std::move(from.greedyHillClimbing_)),
      localSearchWithTabuList_(std::move(from.localSearchWithTabuList_)),
      scoreDatabase_(std::move(from.scoreDatabase_)), ranges_(std::move(from.ranges_)),
      priorDbname_(std::move(from.priorDbname_)), initialDag_(std::move(from.initialDag_)),
      filename_(std::move(from.filename_)),
      nbDecreasingChanges_(std::move(from.nbDecreasingChanges_)) {
    noPrior_ = new NoPrior(scoreDatabase_.databaseTable());

    GUM_CONS_MOV(IBNLearner)
  }

  IBNLearner::~IBNLearner() {
    if (score_) delete score_;

    if (prior_) delete prior_;

    if (noPrior_) delete noPrior_;

    if (priorDatabase_) delete priorDatabase_;

    if (mutualInfo_) delete mutualInfo_;

    GUM_DESTRUCTOR(IBNLearner)
  }

  IBNLearner& IBNLearner::operator=(const IBNLearner& from) {
    if (this != &from) {
      if (score_) {
        delete score_;
        score_ = nullptr;
      }

      if (prior_) {
        delete prior_;
        prior_ = nullptr;
      }

      if (priorDatabase_) {
        delete priorDatabase_;
        priorDatabase_ = nullptr;
      }

      if (mutualInfo_) {
        delete mutualInfo_;
        mutualInfo_ = nullptr;
      }

      ThreadNumberManager::operator=(from);
      scoreType_               = from.scoreType_;
      paramEstimatorType_      = from.paramEstimatorType_;
      epsilonEM_               = from.epsilonEM_;
      priorType_               = from.priorType_;
      priorWeight_             = from.priorWeight_;
      constraintSliceOrder_    = from.constraintSliceOrder_;
      constraintIndegree_      = from.constraintIndegree_;
      constraintTabuList_      = from.constraintTabuList_;
      constraintForbiddenArcs_ = from.constraintForbiddenArcs_;
      constraintMandatoryArcs_ = from.constraintMandatoryArcs_;
      selectedAlgo_            = from.selectedAlgo_;
      algoK2_                  = from.algoK2_;
      algoMiic3off2_           = from.algoMiic3off2_;
      kmode3Off2_              = from.kmode3Off2_;
      greedyHillClimbing_      = from.greedyHillClimbing_;
      localSearchWithTabuList_ = from.localSearchWithTabuList_;
      scoreDatabase_           = from.scoreDatabase_;
      ranges_                  = from.ranges_;
      priorDbname_             = from.priorDbname_;
      initialDag_              = from.initialDag_;
      filename_                = from.filename_;
      nbDecreasingChanges_     = from.nbDecreasingChanges_;
      currentAlgorithm_        = nullptr;
    }

    return *this;
  }

  IBNLearner& IBNLearner::operator=(IBNLearner&& from) {
    if (this != &from) {
      if (score_) {
        delete score_;
        score_ = nullptr;
      }

      if (prior_) {
        delete prior_;
        prior_ = nullptr;
      }

      if (priorDatabase_) {
        delete priorDatabase_;
        priorDatabase_ = nullptr;
      }

      if (mutualInfo_) {
        delete mutualInfo_;
        mutualInfo_ = nullptr;
      }

      ThreadNumberManager::operator=(std::move(from));
      scoreType_               = from.scoreType_;
      paramEstimatorType_      = from.paramEstimatorType_;
      epsilonEM_               = from.epsilonEM_;
      priorType_               = from.priorType_;
      priorWeight_             = from.priorWeight_;
      constraintSliceOrder_    = std::move(from.constraintSliceOrder_);
      constraintIndegree_      = std::move(from.constraintIndegree_);
      constraintTabuList_      = std::move(from.constraintTabuList_);
      constraintForbiddenArcs_ = std::move(from.constraintForbiddenArcs_);
      constraintMandatoryArcs_ = std::move(from.constraintMandatoryArcs_);
      selectedAlgo_            = from.selectedAlgo_;
      algoK2_                  = from.algoK2_;
      algoMiic3off2_           = std::move(from.algoMiic3off2_);
      kmode3Off2_              = from.kmode3Off2_;
      greedyHillClimbing_      = std::move(from.greedyHillClimbing_);
      localSearchWithTabuList_ = std::move(from.localSearchWithTabuList_);
      scoreDatabase_           = std::move(from.scoreDatabase_);
      ranges_                  = std::move(from.ranges_);
      priorDbname_             = std::move(from.priorDbname_);
      filename_                = std::move(from.filename_);
      initialDag_              = std::move(from.initialDag_);
      nbDecreasingChanges_     = std::move(from.nbDecreasingChanges_);
      currentAlgorithm_        = nullptr;
    }

    return *this;
  }


  DatabaseTable readFile(const std::string& filename) {
    // get the extension of the file
    if (auto filename_size = Size(filename.size()); filename_size < 4) {
      GUM_ERROR(FormatNotFound,
                "IBNLearner could not determine the "
                "file type of the database '"
                   << filename << "'")
    }

    std::string extension = filename.substr(filename.size() - 4);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    if (extension != ".csv") {
      GUM_ERROR(OperationNotAllowed,
                "IBNLearner does not support yet this type ('" << extension
                                                               << "')"
                                                                  "of database file")
    }

    DBInitializerFromCSV initializer(filename);

    const auto&       var_names = initializer.variableNames();
    const std::size_t nb_vars   = var_names.size();

    DBTranslatorSet                translator_set;
    DBTranslator4LabelizedVariable translator;
    for (std::size_t i = 0; i < nb_vars; ++i) {
      translator_set.insertTranslator(translator, i);
    }

    DatabaseTable database(translator_set);
    database.setVariableNames(initializer.variableNames());
    initializer.fillDatabase(database);

    return database;
  }


  void IBNLearner::isCSVFileName_(const std::string& filename) {
    // get the extension of the file

    if (auto filename_size = Size(filename.size()); filename_size < 4) {
      GUM_ERROR(FormatNotFound,
                "IBNLearner could not determine the "
                "file type of the database")
    }

    auto extension = filename.substr(filename.size() - 4);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    if (extension != ".csv") {
      GUM_ERROR(OperationNotAllowed, "IBNLearner does not support yet this type of database file")
    }
  }


  DatabaseTable IBNLearner::readFile_(const std::string&                filename,
                                      const std::vector< std::string >& missing_symbols) {
    // get the extension of the file
    isCSVFileName_(filename);

    DBInitializerFromCSV initializer(filename);

    const auto&       var_names = initializer.variableNames();
    const std::size_t nb_vars   = var_names.size();

    DBTranslatorSet                translator_set;
    DBTranslator4LabelizedVariable translator(missing_symbols);
    for (std::size_t i = 0; i < nb_vars; ++i) {
      translator_set.insertTranslator(translator, i);
    }

    DatabaseTable database(missing_symbols, translator_set);
    database.setVariableNames(initializer.variableNames());
    initializer.fillDatabase(database);

    database.reorder();

    return database;
  }

  void IBNLearner::createScore_() {
    // first, save the old score, to be delete if everything is ok
    Score* old_score = score_;

    // create the new scoring function
    switch (scoreType_) {
      case ScoreType::AIC:
        score_ = new ScoreAIC(scoreDatabase_.parser(),
                              *prior_,
                              ranges_,
                              scoreDatabase_.nodeId2Columns());
        break;

      case ScoreType::BD:
        score_ = new ScoreBD(scoreDatabase_.parser(),
                             *prior_,
                             ranges_,
                             scoreDatabase_.nodeId2Columns());
        break;

      case ScoreType::BDeu:
        score_ = new ScoreBDeu(scoreDatabase_.parser(),
                               *prior_,
                               ranges_,
                               scoreDatabase_.nodeId2Columns());
        break;

      case ScoreType::BIC:
        score_ = new ScoreBIC(scoreDatabase_.parser(),
                              *prior_,
                              ranges_,
                              scoreDatabase_.nodeId2Columns());
        break;

      case ScoreType::K2:
        score_ = new ScoreK2(scoreDatabase_.parser(),
                             *prior_,
                             ranges_,
                             scoreDatabase_.nodeId2Columns());
        break;

      case ScoreType::LOG2LIKELIHOOD:
        score_ = new ScoreLog2Likelihood(scoreDatabase_.parser(),
                                         *prior_,
                                         ranges_,
                                         scoreDatabase_.nodeId2Columns());
        break;

      default: GUM_ERROR(OperationNotAllowed, "IBNLearner does not support yet this score")
    }

    // remove the old score, if any
    if (old_score != nullptr) delete old_score;

    // assign the number of threads
    score_->setNumberOfThreads(this->isGumNumberOfThreadsOverriden() ? this->getNumberOfThreads()
                                                                     : 0);
  }

  ParamEstimator* IBNLearner::createParamEstimator_(const DBRowGeneratorParser& parser,
                                                    bool take_into_account_score) {
    ParamEstimator* param_estimator = nullptr;

    // create the new estimator
    switch (paramEstimatorType_) {
      case ParamEstimatorType::ML:
        if (take_into_account_score && (score_ != nullptr)) {
          param_estimator = new ParamEstimatorML(parser,
                                                 *prior_,
                                                 score_->internalPrior(),
                                                 ranges_,
                                                 scoreDatabase_.nodeId2Columns());
        } else {
          param_estimator = new ParamEstimatorML(parser,
                                                 *prior_,
                                                 *noPrior_,
                                                 ranges_,
                                                 scoreDatabase_.nodeId2Columns());
        }

        break;

      default:
        GUM_ERROR(OperationNotAllowed,
                  "IBNLearner does not support "
                     << "yet this parameter estimator")
    }

    // assign the number of threads
    param_estimator->setNumberOfThreads(
       this->isGumNumberOfThreadsOverriden() ? this->getNumberOfThreads() : 0);

    // assign the set of ranges
    param_estimator->setRanges(ranges_);

    return param_estimator;
  }

  /// prepares the initial graph for 3off2 or miic
  MixedGraph IBNLearner::prepareMiic3Off2_() {
    // Initialize the mixed graph to the fully connected graph
    MixedGraph mgraph;
    for (Size i = 0; i < scoreDatabase_.databaseTable().nbVariables(); ++i) {
      mgraph.addNodeWithId(i);
      for (Size j = 0; j < i; ++j) {
        mgraph.addEdge(j, i);
      }
    }

    // translating the constraints for 3off2 or miic
    HashTable< std::pair< NodeId, NodeId >, char > initial_marks;
    for (const auto& arc: constraintMandatoryArcs_.arcs()) {
      initial_marks.insert({arc.tail(), arc.head()}, '>');
    }

    for (const auto& arc: constraintForbiddenArcs_.arcs()) {
      initial_marks.insert({arc.tail(), arc.head()}, '-');
    }
    algoMiic3off2_.addConstraints(initial_marks);

    // create the mutual entropy object
    createCorrectedMutualInformation_();

    return mgraph;
  }

  MixedGraph IBNLearner::learnMixedStructure() {
    if (selectedAlgo_ != AlgoType::MIIC && selectedAlgo_ != AlgoType::THREE_OFF_TWO) {
      GUM_ERROR(OperationNotAllowed, "Must be using the miic/3off2 algorithm")
    }
    // check that the database does not contain any missing value
    if (scoreDatabase_.databaseTable().hasMissingValues()) {
      GUM_ERROR(MissingValueInDatabase,
                "For the moment, the BNLearner is unable to learn "
                   << "structures with missing values in databases")
    }
    BNLearnerListener listener(this, algoMiic3off2_);

    // create the mixedGraph_constraint_MandatoryArcs.arcs
    MixedGraph mgraph = this->prepareMiic3Off2_();

    return algoMiic3off2_.learnMixedStructure(*mutualInfo_, mgraph);
  }

  DAG IBNLearner::learnDAG() {
    // create the score and the prior
    createPrior_();
    createScore_();

    return learnDag_();
  }

  void IBNLearner::createCorrectedMutualInformation_() {
    if (mutualInfo_ != nullptr) delete mutualInfo_;

    mutualInfo_ = new CorrectedMutualInformation(scoreDatabase_.parser(),
                                                 *noPrior_,
                                                 ranges_,
                                                 scoreDatabase_.nodeId2Columns());
    switch (kmode3Off2_) {
      using enum CorrectedMutualInformation::KModeTypes;
      case MDL: mutualInfo_->useMDL(); break;
      case NML: mutualInfo_->useNML(); break;
      case NoCorr: mutualInfo_->useNoCorr(); break;
      default:
        GUM_ERROR(NotImplementedYet,
                  "The BNLearner's corrected mutual information class does "
                     << "not implement yet this correction : " << int(kmode3Off2_))
    }
  }

  DAG IBNLearner::learnDag_() {
    // check that the database does not contain any missing value
    if (scoreDatabase_.databaseTable().hasMissingValues()
        || ((priorDatabase_ != nullptr)
            && (priorType_ == BNLearnerPriorType::DIRICHLET_FROM_DATABASE)
            && priorDatabase_->databaseTable().hasMissingValues())) {
      GUM_ERROR(MissingValueInDatabase,
                "For the moment, the BNLearner is unable to cope "
                "with missing values in databases")
    }
    // add the mandatory arcs to the initial dag and remove the forbidden ones
    // from the initial graph
    DAG init_graph = initialDag_;

    for (const auto& arc: constraintMandatoryArcs_.arcs()) {
      if (!init_graph.exists(arc.tail())) init_graph.addNodeWithId(arc.tail());

      if (!init_graph.exists(arc.head())) init_graph.addNodeWithId(arc.head());

      init_graph.addArc(arc.tail(), arc.head());
    }

    for (const auto& arc: constraintForbiddenArcs_.arcs())
      init_graph.eraseArc(arc);


    switch (selectedAlgo_) {
      // ========================================================================
      case AlgoType::MIIC:
      case AlgoType::THREE_OFF_TWO: {
        BNLearnerListener listener(this, algoMiic3off2_);
        // create the mixedGraph and the corrected mutual information
        MixedGraph mgraph = this->prepareMiic3Off2_();

        return algoMiic3off2_.learnStructure(*mutualInfo_, mgraph);
      }

      // ========================================================================
      case AlgoType::GREEDY_HILL_CLIMBING: {
        BNLearnerListener listener(this, greedyHillClimbing_);
        StructuralConstraintSetStatic< StructuralConstraintMandatoryArcs,
                                       StructuralConstraintForbiddenArcs,
                                       StructuralConstraintPossibleEdges,
                                       StructuralConstraintSliceOrder >
           gen_constraint;
        static_cast< StructuralConstraintMandatoryArcs& >(gen_constraint)
           = constraintMandatoryArcs_;
        static_cast< StructuralConstraintForbiddenArcs& >(gen_constraint)
           = constraintForbiddenArcs_;
        static_cast< StructuralConstraintPossibleEdges& >(gen_constraint)
           = constraintPossibleEdges_;
        static_cast< StructuralConstraintSliceOrder& >(gen_constraint) = constraintSliceOrder_;

        GraphChangesGenerator4DiGraph op_set(gen_constraint);

        StructuralConstraintSetStatic< StructuralConstraintIndegree, StructuralConstraintDAG >
           sel_constraint;
        static_cast< StructuralConstraintIndegree& >(sel_constraint) = constraintIndegree_;

        GraphChangesSelector4DiGraph selector(*score_, sel_constraint, op_set);

        return greedyHillClimbing_.learnStructure(selector, init_graph);
      }

      // ========================================================================
      case AlgoType::LOCAL_SEARCH_WITH_TABU_LIST: {
        BNLearnerListener listener(this, localSearchWithTabuList_);
        StructuralConstraintSetStatic< StructuralConstraintMandatoryArcs,
                                       StructuralConstraintForbiddenArcs,
                                       StructuralConstraintPossibleEdges,
                                       StructuralConstraintSliceOrder >
           gen_constraint;
        static_cast< StructuralConstraintMandatoryArcs& >(gen_constraint)
           = constraintMandatoryArcs_;
        static_cast< StructuralConstraintForbiddenArcs& >(gen_constraint)
           = constraintForbiddenArcs_;
        static_cast< StructuralConstraintPossibleEdges& >(gen_constraint)
           = constraintPossibleEdges_;
        static_cast< StructuralConstraintSliceOrder& >(gen_constraint) = constraintSliceOrder_;

        GraphChangesGenerator4DiGraph op_set(gen_constraint);

        StructuralConstraintSetStatic< StructuralConstraintTabuList,
                                       StructuralConstraintIndegree,
                                       StructuralConstraintDAG >
           sel_constraint;
        static_cast< StructuralConstraintTabuList& >(sel_constraint) = constraintTabuList_;
        static_cast< StructuralConstraintIndegree& >(sel_constraint) = constraintIndegree_;

        GraphChangesSelector4DiGraph selector(*score_, sel_constraint, op_set);

        return localSearchWithTabuList_.learnStructure(selector, init_graph);
      }

      // ========================================================================
      case AlgoType::K2: {
        BNLearnerListener listener(this, algoK2_.approximationScheme());
        StructuralConstraintSetStatic< StructuralConstraintMandatoryArcs,
                                       StructuralConstraintForbiddenArcs,
                                       StructuralConstraintPossibleEdges >
           gen_constraint;
        static_cast< StructuralConstraintMandatoryArcs& >(gen_constraint)
           = constraintMandatoryArcs_;
        static_cast< StructuralConstraintForbiddenArcs& >(gen_constraint)
           = constraintForbiddenArcs_;
        static_cast< StructuralConstraintPossibleEdges& >(gen_constraint)
           = constraintPossibleEdges_;

        GraphChangesGenerator4K2 op_set(gen_constraint);

        // if some mandatory arcs are incompatible with the order, use a DAG
        // constraint instead of a DiGraph constraint to avoid cycles
        const ArcSet& mandatory_arcs
           = static_cast< StructuralConstraintMandatoryArcs& >(gen_constraint).arcs();
        const Sequence< NodeId >& order            = algoK2_.order();
        bool                      order_compatible = true;

        for (const auto& arc: mandatory_arcs) {
          if (order.pos(arc.tail()) >= order.pos(arc.head())) {
            order_compatible = false;
            break;
          }
        }

        if (order_compatible) {
          StructuralConstraintSetStatic< StructuralConstraintIndegree, StructuralConstraintDiGraph >
             sel_constraint;
          static_cast< StructuralConstraintIndegree& >(sel_constraint) = constraintIndegree_;

          GraphChangesSelector4DiGraph selector(*score_, sel_constraint, op_set);

          return algoK2_.learnStructure(selector, init_graph);
        } else {
          StructuralConstraintSetStatic< StructuralConstraintIndegree, StructuralConstraintDAG >
             sel_constraint;
          static_cast< StructuralConstraintIndegree& >(sel_constraint) = constraintIndegree_;

          GraphChangesSelector4DiGraph selector(*score_, sel_constraint, op_set);

          return algoK2_.learnStructure(selector, init_graph);
        }
      }

      // ========================================================================
      default:
        GUM_ERROR(OperationNotAllowed,
                  "the learnDAG method has not been implemented for this "
                  "learning algorithm")
    }
  }

  std::string IBNLearner::checkScorePriorCompatibility() const {
    const auto prior = getPriorType_();

    switch (scoreType_) {
      using enum gum::learning::IBNLearner::ScoreType;
      case AIC: return ScoreAIC::isPriorCompatible(prior, priorWeight_);

      case BD: return ScoreBD::isPriorCompatible(prior, priorWeight_);

      case BDeu: return ScoreBDeu::isPriorCompatible(prior, priorWeight_);

      case BIC: return ScoreBIC::isPriorCompatible(prior, priorWeight_);

      case K2: return ScoreK2::isPriorCompatible(prior, priorWeight_);

      case LOG2LIKELIHOOD: return ScoreLog2Likelihood::isPriorCompatible(prior, priorWeight_);

      default: return "IBNLearner does not support yet this score";
    }
  }


  /// sets the ranges of rows to be used for cross-validation learning
  std::pair< std::size_t, std::size_t >
     IBNLearner::useCrossValidationFold(const std::size_t learning_fold, const std::size_t k_fold) {
    if (k_fold == 0) { GUM_ERROR(OutOfBounds, "K-fold cross validation with k=0 is forbidden") }

    if (learning_fold >= k_fold) {
      GUM_ERROR(OutOfBounds,
                "In " << k_fold << "-fold cross validation, the learning "
                      << "fold should be strictly lower than " << k_fold
                      << " but, here, it is equal to " << learning_fold)
    }

    const std::size_t db_size = scoreDatabase_.databaseTable().nbRows();
    if (k_fold >= db_size) {
      GUM_ERROR(OutOfBounds,
                "In " << k_fold << "-fold cross validation, the database's "
                      << "size should be strictly greater than " << k_fold
                      << " but, here, the database has only " << db_size << "rows")
    }

    // create the ranges of rows of the test database
    const std::size_t foldSize   = db_size / k_fold;
    const std::size_t unfold_deb = learning_fold * foldSize;
    const std::size_t unfold_end = unfold_deb + foldSize;

    ranges_.clear();
    if (learning_fold == std::size_t(0)) {
      ranges_.push_back(std::pair< std::size_t, std::size_t >(unfold_end, db_size));
    } else {
      ranges_.push_back(std::pair< std::size_t, std::size_t >(std::size_t(0), unfold_deb));

      if (learning_fold != k_fold - 1) {
        ranges_.push_back(std::pair< std::size_t, std::size_t >(unfold_end, db_size));
      }
    }

    return std::pair< std::size_t, std::size_t >(unfold_deb, unfold_end);
  }


  std::pair< double, double >
     IBNLearner::chi2(const NodeId id1, const NodeId id2, const std::vector< NodeId >& knowing) {
    createPrior_();
    gum::learning::IndepTestChi2 chi2score(scoreDatabase_.parser(), *prior_, databaseRanges());

    return chi2score.statistics(id1, id2, knowing);
  }

  std::pair< double, double > IBNLearner::chi2(const std::string&                name1,
                                               const std::string&                name2,
                                               const std::vector< std::string >& knowing) {
    std::vector< NodeId > knowingIds;
    std::transform(knowing.begin(),
                   knowing.end(),
                   std::back_inserter(knowingIds),
                   [this](const std::string& c) { return this->idFromName(c); });
    return chi2(idFromName(name1), idFromName(name2), knowingIds);
  }

  std::pair< double, double >
     IBNLearner::G2(const NodeId id1, const NodeId id2, const std::vector< NodeId >& knowing) {
    createPrior_();
    gum::learning::IndepTestG2 g2score(scoreDatabase_.parser(), *prior_, databaseRanges());
    return g2score.statistics(id1, id2, knowing);
  }

  std::pair< double, double > IBNLearner::G2(const std::string&                name1,
                                             const std::string&                name2,
                                             const std::vector< std::string >& knowing) {
    std::vector< NodeId > knowingIds;
    std::transform(knowing.begin(),
                   knowing.end(),
                   std::back_inserter(knowingIds),
                   [this](const std::string& c) { return this->idFromName(c); });
    return G2(idFromName(name1), idFromName(name2), knowingIds);
  }

  double IBNLearner::logLikelihood(const std::vector< NodeId >& vars,
                                   const std::vector< NodeId >& knowing) {
    createPrior_();
    gum::learning::ScoreLog2Likelihood ll2score(scoreDatabase_.parser(), *prior_, databaseRanges());

    std::vector< NodeId > total(vars);
    total.insert(total.end(), knowing.begin(), knowing.end());
    double LLtotal = ll2score.score(IdCondSet(total, false, true));
    if (knowing.size() == (Size)0) {
      return LLtotal;
    } else {
      double LLknw = ll2score.score(IdCondSet(knowing, false, true));
      return LLtotal - LLknw;
    }
  }

  double IBNLearner::logLikelihood(const std::vector< std::string >& vars,
                                   const std::vector< std::string >& knowing) {
    std::vector< NodeId > ids;
    std::vector< NodeId > knowingIds;

    auto mapper = [this](const std::string& c) { return this->idFromName(c); };

    std::transform(vars.begin(), vars.end(), std::back_inserter(ids), mapper);
    std::transform(knowing.begin(), knowing.end(), std::back_inserter(knowingIds), mapper);

    return logLikelihood(ids, knowingIds);
  }

  double IBNLearner::correctedMutualInformation(const NodeId                 id1,
                                                const NodeId                 id2,
                                                const std::vector< NodeId >& knowing) {
    createPrior_();
    gum::learning::CorrectedMutualInformation cmi(scoreDatabase_.parser(),
                                                  *prior_,
                                                  databaseRanges());

    switch (kmode3Off2_) {
      using enum gum::learning::CorrectedMutualInformation::KModeTypes;
      case MDL: cmi.useMDL(); break;

      case NML: cmi.useNML(); break;

      case NoCorr: cmi.useNoCorr(); break;

      default:
        GUM_ERROR(NotImplementedYet,
                  "The BNLearner's corrected mutual information class does "
                     << "not implement yet this correction : " << int(kmode3Off2_))
    }

    if (knowing.size() == (Size)0) return cmi.score(id1, id2) / (double)this->nbRows();
    else return cmi.score(id1, id2, knowing) / (double)this->nbRows();
  }

  double IBNLearner::correctedMutualInformation(const std::string&                var1,
                                                const std::string&                var2,
                                                const std::vector< std::string >& knowing) {
    std::vector< NodeId > knowingIds;

    auto mapper = [this](const std::string& c) { return this->idFromName(c); };

    std::transform(knowing.begin(), knowing.end(), std::back_inserter(knowingIds), mapper);

    return correctedMutualInformation(this->idFromName(var1), this->idFromName(var2), knowingIds);
  }


  double IBNLearner::mutualInformation(const NodeId                 id1,
                                       const NodeId                 id2,
                                       const std::vector< NodeId >& knowing) {
    const auto prior = NoPrior(scoreDatabase_.databaseTable(), scoreDatabase_.nodeId2Columns());
    gum::learning::CorrectedMutualInformation cmi(scoreDatabase_.parser(), prior, databaseRanges());
    cmi.useNoCorr();

    if (knowing.size() == (Size)0) return cmi.score(id1, id2) / (double)this->nbRows();
    else return cmi.score(id1, id2, knowing) / (double)this->nbRows();
  }

  double IBNLearner::mutualInformation(const std::string&                var1,
                                       const std::string&                var2,
                                       const std::vector< std::string >& knowing) {
    std::vector< NodeId > knowingIds;

    auto mapper = [this](const std::string& c) { return this->idFromName(c); };

    std::transform(knowing.begin(), knowing.end(), std::back_inserter(knowingIds), mapper);

    return mutualInformation(this->idFromName(var1), this->idFromName(var2), knowingIds);
  }

  double IBNLearner::score(const NodeId var, const std::vector< NodeId >& knowing) {
    createPrior_();
    createScore_();

    return score_->score(var, knowing);
  }

  double IBNLearner::score(const std::string& var, const std::vector< std::string >& knowing) {
    auto mapper = [this](const std::string& c) { return this->idFromName(c); };

    const NodeId          id = this->idFromName(var);
    std::vector< NodeId > knowingIds;
    knowingIds.reserve(knowing.size());
    std::transform(knowing.begin(), knowing.end(), std::back_inserter(knowingIds), mapper);

    return score(id, knowingIds);
  }


  std::vector< double > IBNLearner::rawPseudoCount(const std::vector< NodeId >& vars) {
    Potential< double > res;

    createPrior_();
    gum::learning::PseudoCount count(scoreDatabase_.parser(), *prior_, databaseRanges());
    return count.get(vars);
  }


  std::vector< double > IBNLearner::rawPseudoCount(const std::vector< std::string >& vars) {
    std::vector< NodeId > ids;

    auto mapper = [this](const std::string& c) { return this->idFromName(c); };

    std::transform(vars.begin(), vars.end(), std::back_inserter(ids), mapper);
    return rawPseudoCount(ids);
  }


  /// use a new set of database rows' ranges to perform learning
  void IBNLearner::useDatabaseRanges(
     const std::vector< std::pair< std::size_t, std::size_t > >& new_ranges) {
    // use a score to detect whether the ranges are ok
    ScoreLog2Likelihood score(scoreDatabase_.parser(), *noPrior_);
    score.setRanges(new_ranges);
    ranges_ = score.ranges();
  }

}   // namespace gum::learning
