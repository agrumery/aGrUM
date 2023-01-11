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
 * The pack currently contains K2, GreedyHillClimbing and
 *LocalSearchWithTabuList
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <fstream>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to help IDE parser
#  include <agrum/BN/learning/BNLearner.h>

#  include <agrum/BN/learning/BNLearnUtils/BNLearnerListener.h>

namespace gum {

  namespace learning {
    template < typename GUM_SCALAR >
    BNLearner< GUM_SCALAR >::BNLearner(const std::string&                filename,
                                       const std::vector< std::string >& missingSymbols,
                                       const bool                        induceTypes) :
        IBNLearner(filename, missingSymbols, induceTypes) {
      GUM_CONSTRUCTOR(BNLearner);
    }

    template < typename GUM_SCALAR >
    BNLearner< GUM_SCALAR >::BNLearner(const DatabaseTable& db) : IBNLearner(db) {
      GUM_CONSTRUCTOR(BNLearner);
    }

    template < typename GUM_SCALAR >
    BNLearner< GUM_SCALAR >::BNLearner(const std::string&                 filename,
                                       const gum::BayesNet< GUM_SCALAR >& bn,
                                       const std::vector< std::string >&  missing_symbols) :
        IBNLearner(filename, bn, missing_symbols) {
      GUM_CONSTRUCTOR(BNLearner);
    }

    /// copy constructor
    template < typename GUM_SCALAR >
    BNLearner< GUM_SCALAR >::BNLearner(const BNLearner< GUM_SCALAR >& src) : IBNLearner(src) {
      GUM_CONSTRUCTOR(BNLearner);
    }

    /// move constructor
    template < typename GUM_SCALAR >
    BNLearner< GUM_SCALAR >::BNLearner(BNLearner< GUM_SCALAR >&& src) : IBNLearner(src) {
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
    BNLearner< GUM_SCALAR >&
       BNLearner< GUM_SCALAR >::operator=(const BNLearner< GUM_SCALAR >& src) {
      IBNLearner::operator=(src);
      return *this;
    }

    /// move operator
    template < typename GUM_SCALAR >
    BNLearner< GUM_SCALAR >& BNLearner< GUM_SCALAR >::operator=(BNLearner< GUM_SCALAR >&& src) {
      IBNLearner::operator=(std::move(src));
      return *this;
    }

    /// learn a Bayes Net from a file
    template < typename GUM_SCALAR >
    BayesNet< GUM_SCALAR > BNLearner< GUM_SCALAR >::learnBN() {
      // create the score, the prior and the estimator
      auto notification = checkScorePriorCompatibility();
      if (notification != "") { std::cout << "[aGrUM notification] " << notification << std::endl; }
      createPrior_();
      createScore_();

      std::unique_ptr< ParamEstimator > param_estimator(
         createParamEstimator_(scoreDatabase_.parser(), true));

      return Dag2BN_.createBN< GUM_SCALAR >(*(param_estimator.get()), learnDag_());
    }

    /// learns a BN (its parameters) when its structure is known
    template < typename GUM_SCALAR >
    BayesNet< GUM_SCALAR > BNLearner< GUM_SCALAR >::learnParameters(const DAG& dag,
                                                                    bool takeIntoAccountScore) {
      // if the dag contains no node, return an empty BN
      if (dag.size() == 0) return BayesNet< GUM_SCALAR >();

      // check that the dag corresponds to the database
      std::vector< NodeId > ids;
      ids.reserve(dag.sizeNodes());
      for (const auto node: dag)
        ids.push_back(node);
      std::sort(ids.begin(), ids.end());

      if (ids.back() >= scoreDatabase_.names().size()) {
        std::stringstream str;
        str << "Learning parameters corresponding to the dag is impossible "
            << "because the database does not contain the following nodeID";
        std::vector< NodeId > bad_ids;
        for (const auto node: ids) {
          if (node >= scoreDatabase_.names().size()) bad_ids.push_back(node);
        }
        if (bad_ids.size() > 1) str << 's';
        str << ": ";
        bool deja = false;
        for (const auto node: bad_ids) {
          if (deja) str << ", ";
          else deja = true;
          str << node;
        }
        GUM_ERROR(MissingVariableInDatabase, str.str())
      }

      // create the prior
      createPrior_();

      if (epsilonEM_ == 0.0) {
        // check that the database does not contain any missing value
        if (scoreDatabase_.databaseTable().hasMissingValues()
            || ((priorDatabase_ != nullptr)
                && (priorType_ == BNLearnerPriorType::DIRICHLET_FROM_DATABASE)
                && priorDatabase_->databaseTable().hasMissingValues())) {
          GUM_ERROR(MissingValueInDatabase,
                    "In general, the BNLearner is unable to cope with "
                       << "missing values in databases. To learn parameters in "
                       << "such situations, you should first use method "
                       << "useEM()");
        }

        // create the usual estimator
        DBRowGeneratorParser parser(scoreDatabase_.databaseTable().handler(), DBRowGeneratorSet());
        std::unique_ptr< ParamEstimator > param_estimator(
           createParamEstimator_(parser, takeIntoAccountScore));

        return Dag2BN_.createBN< GUM_SCALAR >(*(param_estimator.get()), dag);
      } else {
        // EM !
        BNLearnerListener listener(this, Dag2BN_);

        // get the column types
        const auto&       database = scoreDatabase_.databaseTable();
        const std::size_t nb_vars  = database.nbVariables();
        const std::vector< gum::learning::DBTranslatedValueType > col_types(
           nb_vars,
           gum::learning::DBTranslatedValueType::DISCRETE);

        // create the bootstrap estimator
        DBRowGenerator4CompleteRows generator_bootstrap(col_types);
        DBRowGeneratorSet           genset_bootstrap;
        genset_bootstrap.insertGenerator(generator_bootstrap);
        DBRowGeneratorParser              parser_bootstrap(database.handler(), genset_bootstrap);
        std::unique_ptr< ParamEstimator > param_estimator_bootstrap(
           createParamEstimator_(parser_bootstrap, takeIntoAccountScore));

        // create the EM estimator
        BayesNet< GUM_SCALAR >         dummy_bn;
        DBRowGeneratorEM< GUM_SCALAR > generator_EM(col_types, dummy_bn);
        DBRowGenerator&                gen_EM = generator_EM;   // fix for g++-4.8
        DBRowGeneratorSet              genset_EM;
        genset_EM.insertGenerator(gen_EM);
        DBRowGeneratorParser              parser_EM(database.handler(), genset_EM);
        std::unique_ptr< ParamEstimator > param_estimator_EM(
           createParamEstimator_(parser_EM, takeIntoAccountScore));

        Dag2BN_.setEpsilon(epsilonEM_);
        return Dag2BN_.createBN< GUM_SCALAR >(*(param_estimator_bootstrap.get()),
                                              *(param_estimator_EM.get()),
                                              dag);
      }
    }


    /// learns a BN (its parameters) when its structure is known
    template < typename GUM_SCALAR >
    BayesNet< GUM_SCALAR > BNLearner< GUM_SCALAR >::learnParameters(bool take_into_account_score) {
      return learnParameters(initialDag_, take_into_account_score);
    }


    template < typename GUM_SCALAR >
    NodeProperty< Sequence< std::string > >
       BNLearner< GUM_SCALAR >::_labelsFromBN_(const std::string&            filename,
                                               const BayesNet< GUM_SCALAR >& src) {
      std::ifstream in(filename, std::ifstream::in);

      if ((in.rdstate() & std::ifstream::failbit) != 0) {
        GUM_ERROR(gum::IOError, "File " << filename << " not found")
      }

      CSVParser parser(in, filename);
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
          // no problem : a column not in the BN...
        }
      }

      return modals;
    }


    template < typename GUM_SCALAR >
    std::string BNLearner< GUM_SCALAR >::toString() const {
      const auto st = state();

      Size maxkey = 0;
      for (const auto& tuple: st)
        if (std::get< 0 >(tuple).length() > maxkey) maxkey = std::get< 0 >(tuple).length();

      std::stringstream s;
      for (const auto& tuple: st) {
        s << std::setiosflags(std::ios::left) << std::setw(maxkey) << std::get< 0 >(tuple) << " : "
          << std::get< 1 >(tuple);
        if (std::get< 2 >(tuple) != "") s << "  (" << std::get< 2 >(tuple) << ")";
        s << std::endl;
      }
      return s.str();
    }

    template < typename GUM_SCALAR >
    std::vector< std::tuple< std::string, std::string, std::string > >
       BNLearner< GUM_SCALAR >::state() const {
      std::vector< std::tuple< std::string, std::string, std::string > > vals;

      std::string key;
      std::string comment;
      const auto& db = database();

      vals.emplace_back("Filename", filename_, "");
      vals.emplace_back("Size",
                        "(" + std::to_string(nbRows()) + "," + std::to_string(nbCols()) + ")",
                        "");

      std::string vars = "";
      for (NodeId i = 0; i < db.nbVariables(); i++) {
        if (i > 0) vars += ", ";
        vars += nameFromId(i) + "[" + std::to_string(db.domainSize(i)) + "]";
      }
      vals.emplace_back("Variables", vars, "");
      vals.emplace_back("Induced types", inducedTypes_ ? "True" : "False", "");
      vals.emplace_back("Missing values", hasMissingValues() ? "True" : "False", "");

      key = "Algorithm";
      switch (selectedAlgo_) {
        case AlgoType::GREEDY_HILL_CLIMBING:
          vals.emplace_back(key, "Greedy Hill Climbing", "");
          break;
        case AlgoType::K2: {
          vals.emplace_back(key, "K2", "");
          const auto& k2order = algoK2_.order();
          vars                = "";
          for (NodeId i = 0; i < k2order.size(); i++) {
            if (i > 0) vars += ", ";
            vars += nameFromId(k2order.atPos(i));
          }
          vals.emplace_back("K2 order", vars, "");
        } break;
        case AlgoType::LOCAL_SEARCH_WITH_TABU_LIST:
          vals.emplace_back(key, "Local Search with Tabu List", "");
          vals.emplace_back("Tabu list size", std::to_string(nbDecreasingChanges_), "");
          break;
        case AlgoType::THREE_OFF_TWO: vals.emplace_back(key, "3off2", ""); break;
        case AlgoType::MIIC: vals.emplace_back(key, "MIIC", ""); break;
        default: vals.emplace_back(key, "(unknown)", "?"); break;
      }


      key = "Score";
      switch (scoreType_) {
        case ScoreType::K2: vals.emplace_back(key, "K2", ""); break;
        case ScoreType::AIC: vals.emplace_back(key, "AIC", ""); break;
        case ScoreType::BIC: vals.emplace_back(key, "BIC", ""); break;
        case ScoreType::BD: vals.emplace_back(key, "BD", ""); break;
        case ScoreType::BDeu: vals.emplace_back(key, "BDeu", ""); break;
        case ScoreType::LOG2LIKELIHOOD: vals.emplace_back(key, "Log2Likelihood", ""); break;
        default: vals.emplace_back(key, "(unknown)", "?"); break;
      }

      key                                        = "Correction";
      const std::string NotUsedForScoreBasedAlgo = "Not used for score-based algorithms";
      switch (kmode3Off2_) {
        case CorrectedMutualInformation::KModeTypes::MDL:
          vals.emplace_back(key, "MDL", NotUsedForScoreBasedAlgo);
          break;
        case CorrectedMutualInformation::KModeTypes::NML:
          vals.emplace_back(key, "NML", NotUsedForScoreBasedAlgo);
          break;
        case CorrectedMutualInformation::KModeTypes::NoCorr:
          vals.emplace_back(key, "No correction", "");
          break;
        default: vals.emplace_back(key, "(unknown)", "?"); break;
      }


      key     = "Prior";
      comment = checkScorePriorCompatibility();
      switch (priorType_) {
        case BNLearnerPriorType::NO_prior: vals.emplace_back(key, "-", comment); break;
        case BNLearnerPriorType::DIRICHLET_FROM_DATABASE:
          vals.emplace_back(key, "Dirichlet", comment);
          vals.emplace_back("Dirichlet from database", priorDbname_, "");
          break;
        case BNLearnerPriorType::DIRICHLET_FROM_BAYESNET:
          vals.emplace_back(key, "Dirichlet", comment);
          vals.emplace_back("Dirichlet from Bayesian network : ", _prior_bn_.toString(), "");
          break;
        case BNLearnerPriorType::BDEU: vals.emplace_back(key, "BDEU", comment); break;
        case BNLearnerPriorType::SMOOTHING: vals.emplace_back(key, "Smoothing", comment); break;
        default: vals.emplace_back(key, "(unknown)", "?"); break;
      }

      if (priorType_ != BNLearnerPriorType::NO_prior)
        vals.emplace_back("Prior weight", std::to_string(priorWeight_), "");

      if (databaseWeight() != double(nbRows())) {
        vals.emplace_back("Database weight", std::to_string(databaseWeight()), "");
      }

      if (epsilonEM_ > 0.0) {
        comment = "";
        if (!hasMissingValues()) comment = "But no missing values in this database";
        vals.emplace_back("EM", "True", "");
        vals.emplace_back("EM epsilon", std::to_string(epsilonEM_), comment);
      }

      std::string res;
      bool        nofirst;
      if (constraintIndegree_.maxIndegree() < std::numeric_limits< Size >::max()) {
        vals.emplace_back("Constraint Max InDegree",
                          std::to_string(constraintIndegree_.maxIndegree()),
                          "Used only for score-based algorithms.");
      }
      if (!constraintForbiddenArcs_.arcs().empty()) {
        res     = "{";
        nofirst = false;
        for (const auto& arc: constraintForbiddenArcs_.arcs()) {
          if (nofirst) res += ", ";
          else nofirst = true;
          res += nameFromId(arc.tail()) + "->" + nameFromId(arc.head());
        }
        res += "}";
        vals.emplace_back("Constraint Forbidden Arcs", res, "");
      }
      if (!constraintMandatoryArcs_.arcs().empty()) {
        res     = "{";
        nofirst = false;
        for (const auto& arc: constraintMandatoryArcs_.arcs()) {
          if (nofirst) res += ", ";
          else nofirst = true;
          res += nameFromId(arc.tail()) + "->" + nameFromId(arc.head());
        }
        res += "}";
        vals.emplace_back("Constraint Mandatory Arcs", res, "");
      }
      if (!constraintPossibleEdges_.edges().empty()) {
        res     = "{";
        nofirst = false;
        for (const auto& edge: constraintPossibleEdges_.edges()) {
          if (nofirst) res += ", ";
          else nofirst = true;
          res += nameFromId(edge.first()) + "--" + nameFromId(edge.second());
        }
        res += "}";
        vals.emplace_back("Constraint Possible Edges",
                          res,
                          "Used only for score-based algorithms.");
      }
      if (!constraintSliceOrder_.sliceOrder().empty()) {
        res               = "{";
        nofirst           = false;
        const auto& order = constraintSliceOrder_.sliceOrder();
        for (const auto& p: order) {
          if (nofirst) res += ", ";
          else nofirst = true;
          res += nameFromId(p.first) + ":" + std::to_string(p.second);
        }
        res += "}";
        vals.emplace_back("Constraint Slice Order", res, "Used only for score-based algorithms.");
      }
      if (initialDag_.size() != 0) {
        vals.emplace_back("Initial DAG", "True", initialDag_.toDot());
      }

      return vals;
    }

    template < typename GUM_SCALAR >
    void BNLearner< GUM_SCALAR >::createPrior_() {
      // first, save the old prior, to be delete if everything is ok
      Prior* old_prior = prior_;

      // create the new prior
      switch (priorType_) {
        case BNLearnerPriorType::NO_prior:
          prior_ = new NoPrior(scoreDatabase_.databaseTable(), scoreDatabase_.nodeId2Columns());
          break;

        case BNLearnerPriorType::SMOOTHING:
          prior_
             = new SmoothingPrior(scoreDatabase_.databaseTable(), scoreDatabase_.nodeId2Columns());
          break;

        case BNLearnerPriorType::DIRICHLET_FROM_DATABASE:
          if (priorDatabase_ != nullptr) {
            delete priorDatabase_;
            priorDatabase_ = nullptr;
          }

          priorDatabase_
             = new Database(priorDbname_, scoreDatabase_, scoreDatabase_.missingSymbols());

          prior_ = new DirichletPriorFromDatabase(scoreDatabase_.databaseTable(),
                                                  priorDatabase_->parser(),
                                                  priorDatabase_->nodeId2Columns());
          break;

        case BNLearnerPriorType::DIRICHLET_FROM_BAYESNET:
          prior_
             = new DirichletPriorFromBN< GUM_SCALAR >(scoreDatabase_.databaseTable(), &_prior_bn_);
          break;

        case BNLearnerPriorType::BDEU:
          prior_ = new BDeuPrior(scoreDatabase_.databaseTable(), scoreDatabase_.nodeId2Columns());
          break;

        default: GUM_ERROR(OperationNotAllowed, "The BNLearner does not support yet this prior")
      }

      // do not forget to assign a weight to the prior
      prior_->setWeight(priorWeight_);

      // remove the old prior, if any
      if (old_prior != nullptr) delete old_prior;
    }


    template < typename GUM_SCALAR >
    INLINE std::ostream& operator<<(std::ostream& output, const BNLearner< GUM_SCALAR >& learner) {
      output << learner.toString();
      return output;
    }
  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
