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
 * @brief A class for generic framework of learning algorithms that can easily be
 *used
 *
 * The pack currently contains K2, GreedyHillClimbing and LocalSearchWithTabuList
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_GENERIC_BN_LEARNER_H
#define GUM_LEARNING_GENERIC_BN_LEARNER_H

#include <sstream>

#include <agrum/config.h>
#include <agrum/core/bijection.h>
#include <agrum/core/sequence.h>
#include <agrum/graphs/DAG.h>
#include <agrum/BN/BayesNet.h>

#include <agrum/learning/database/databaseFromCSV.h>
#include <agrum/learning/database/DBCellTranslators/cellTranslatorUniversal.h>
#include <agrum/learning/database/DBCellTranslators/cellTranslatorCompactIntId.h>
#include <agrum/learning/database/DBRowTranslatorSetDynamic.h>
#include <agrum/learning/database/DBTransformCompactInt.h>
#include <agrum/learning/database/filteredRowGenerators/rowGeneratorIdentity.h>

#include <agrum/learning/scores_and_tests/scoreAIC.h>
#include <agrum/learning/scores_and_tests/scoreBD.h>
#include <agrum/learning/scores_and_tests/scoreBDeu.h>
#include <agrum/learning/scores_and_tests/scoreBIC.h>
#include <agrum/learning/scores_and_tests/scoreK2.h>
#include <agrum/learning/scores_and_tests/scoreLog2Likelihood.h>

#include <agrum/learning/aprioris/aprioriNoApriori.h>
#include <agrum/learning/aprioris/aprioriSmoothing.h>
#include <agrum/learning/aprioris/aprioriDirichletFromDatabase.h>

#include <agrum/learning/constraints/structuralConstraintDiGraph.h>
#include <agrum/learning/constraints/structuralConstraintDAG.h>
#include <agrum/learning/constraints/structuralConstraintIndegree.h>
#include <agrum/learning/constraints/structuralConstraintSliceOrder.h>
#include <agrum/learning/constraints/structuralConstraintTabuList.h>
#include <agrum/learning/constraints/structuralConstraintForbiddenArcs.h>
#include <agrum/learning/constraints/structuralConstraintMandatoryArcs.h>
#include <agrum/learning/constraints/structuralConstraintSetStatic.h>

#include <agrum/learning/structureUtils/graphChange.h>
#include <agrum/learning/structureUtils/graphChangesSelector4DiGraph.h>
#include <agrum/learning/structureUtils/graphChangesGenerator4DiGraph.h>
#include <agrum/learning/structureUtils/graphChangesGenerator4K2.h>

#include <agrum/learning/paramUtils/DAG2BNLearner.h>
#include <agrum/learning/paramUtils/paramEstimatorML.h>

#include <agrum/core/algorithms/approximationScheme/approximationSchemeListener.h>
#include <agrum/core/algorithms/approximationScheme/IApproximationSchemeConfiguration.h>

#include <agrum/learning/K2.h>
#include <agrum/learning/greedyHillClimbing.h>
#include <agrum/learning/localSearchWithTabuList.h>

#include <agrum/core/signal/signaler.h>

namespace gum {

  namespace learning {

    class BNLearnerListener;

    /** @class genericBNLearner
     * @brief A pack of learning algorithms that can easily be used
     *
     * The pack currently contains K2, GreedyHillClimbing and
     * LocalSearchWithTabuList
     * @ingroup learning_group
     */
    class genericBNLearner : public gum::IApproximationSchemeConfiguration {
      // private:
      public:
      /// an enumeration enabling to select easily the score we wish to use
      enum class ScoreType { AIC, BD, BDeu, BIC, K2, LOG2LIKELIHOOD };

      /// an enumeration to select the type of parameter estimation we shall apply
      enum class ParamEstimatorType { ML };

      /// an enumeration to select the apriori
      enum class AprioriType { NO_APRIORI, SMOOTHING, DIRICHLET_FROM_DATABASE };

      /// an enumeration to select easily the learning algorithm to use
      enum class AlgoType { K2, GREEDY_HILL_CLIMBING, LOCAL_SEARCH_WITH_TABU_LIST };

#ifndef DOXYGEN_SHOULD_SKIP_THIS

      /// a helper to easily read databases
      class Database {
        public:
        // ########################################################################
        /// @name Constructors / Destructors
        // ########################################################################
        /// @{

        /// default constructor
        Database(std::string filename);

        /// default constructor with defined modalities for some variables
        /** @param modalities indicate for some nodes (not necessarily all the
         * nodes of the BN) which modalities they should have and in which
         * order these modalities should be stored into the nodes. For instance,
         * if modalities = { 1 -> {True, False, Big} }, then the node of id 1
         * in the BN will have 3 modalities, the first one being True, the
         * second one being False, and the third bein Big. */
        Database(std::string filename,
                 const NodeProperty<Sequence<std::string>> &modalities,
                 bool check_database = false);

        /// default constructor for the aprioris
        /** We must ensure that, when reading the apriori database, if the
         * "apriori" rowFilter says that a given variable has value i
         * (given by its fast translator), the corresponding "raw" value in the
         * apriori database is the same as in the score/parameter database
         * read before creating the apriori. This is compulsory to have
         * aprioris that make sense. */
        Database(std::string filename, Database &score_database);

        /// default constructor for the aprioris
        /** We must ensure that, when reading the apriori database, if the
         * "apriori" rowFilter says that a given variable has value i
         * (given by its fast translator), the corresponding "raw" value in the
         * apriori database is the same as in the score/parameter database
         * read before creating the apriori. This is compulsory to have
         * aprioris that make sense.
         * @param modalities indicate for some nodes (not necessarily all the
         * nodes of the BN) which modalities they should have and in which
         * order these modalities should be stored into the nodes. For instance,
         * if modalities = { 1 -> {True, False, Big} }, then the node of id 1
         * in the BN will have 3 modalities, the first one being True, the
         * second one being False, and the third bein Big. */
        Database(std::string filename, Database &score_database,
                 const NodeProperty<Sequence<std::string>> &modalities);

        /// copy constructor
        Database(const Database &from);

        /// move constructor
        Database(Database &&from);

        /// destructor
        ~Database();

        /// @}

        // ########################################################################
        /// @name Operators
        // ########################################################################
        /// @{

        /// copy operator
        Database &operator=(const Database &from);

        /// move operator
        Database &operator=(Database &&from);

        /// @}

        // ########################################################################
        /// @name Accessors / Modifiers
        // ########################################################################
        /// @{

        /// returns the row filter
        DBRowFilter<DatabaseVectInRAM::Handler,
                    DBRowTranslatorSetDynamic<CellTranslatorCompactIntId>,
                    FilteredRowGeneratorSet<RowGeneratorIdentity>> &
        rowFilter();

        /// returns the modalities of the variables
        std::vector<unsigned int> &modalities() noexcept;

        /// returns the names of the variables in the database
        const std::vector<std::string> &names() const noexcept;

        /// returns the node id corresponding to a variable name
        NodeId idFromName(const std::string &var_name) const;

        /// returns the variable name corresponding to a given node id
        const std::string &nameFromId(NodeId id) const;

        /// returns the "raw" translators (needed for the aprioris)
        /** We must ensure that, when reading the apriori database, if the
         * "apriori" rowFilter says that a given variable has value i
         * (given by its fast translator), the corresponding "raw" value in the
         * apriori database is the same as in the score/parameter database
         * read before creating the apriori. This is compulsory to have
         * aprioris that make sense. */
        DBRowTranslatorSetDynamic<CellTranslatorUniversal> &rawTranslators();

        /// @}

        protected:
        /// the database itself
        DatabaseVectInRAM __database;

        /// the rwo translators
        DBRowTranslatorSetDynamic<CellTranslatorUniversal> __raw_translators;

        /// the translators used for reading the database
        DBRowTranslatorSetDynamic<CellTranslatorCompactIntId> __translators;

        /// the generators used for reading the database
        FilteredRowGeneratorSet<RowGeneratorIdentity> __generators;

        /// the filtered row that reads the database
        DBRowFilter<DatabaseVectInRAM::Handler,
                    DBRowTranslatorSetDynamic<CellTranslatorCompactIntId>,
                    FilteredRowGeneratorSet<RowGeneratorIdentity>> *__row_filter{
            nullptr};

        /// the modalities of the variables
        std::vector<unsigned int> __modalities;

        /// a hashtable assigning to each variable name its NodeId
        Bijection<std::string, NodeId> __name2nodeId;

/// the max number of threads authorized
#if defined(_OPENMP) && defined(NDEBUG)
        unsigned int __max_threads_number{getMaxNumberOfThreads()};
#else
        unsigned int __max_threads_number{1};
#endif /* NDEBUG */

        /// the minimal number of rows to parse (on average) by thread
        unsigned int __min_nb_rows_per_thread{100};
      };

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /**
       * read the database file for the score / parameter estimation and var names
       */
      genericBNLearner(const std::string &filename);

      /**
       * read the database file for the score / parameter estimation and var names
       * @param modalities indicate for some nodes (not necessarily all the
       * nodes of the BN) which modalities they should have and in which
       * order these modalities should be stored into the nodes. For instance,
       * if modalities = { 1 -> {True, False, Big} }, then the node of id 1
       * in the BN will have 3 modalities, the first one being True, the
       * second one being False, and the third bein Big.
       * @param parse_database if true, the modalities specified by the user
       * will be considered as a superset of the modalities of the variables. A
       * parsing of the database will allow to determine which ones are really
       * necessary and will keep them in the order specified by the user
       * (NodeProperty modalities). If parse_database is set to false (the
       * default), then the modalities specified by the user will be considered as
       * being exactly those of the variables of the BN (as a consequence, if we
       * find other values in the database, an exception will be raised
       * during learning). */
      genericBNLearner(const std::string &filename,
                       const NodeProperty<Sequence<std::string>> &modalities,
                       bool parse_database = false);

      /// copy constructor
      genericBNLearner(const genericBNLearner &);

      /// move constructor
      genericBNLearner(genericBNLearner &&);

      /// destructor
      virtual ~genericBNLearner();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      genericBNLearner &operator=(const genericBNLearner &);

      /// move operator
      genericBNLearner &operator=(genericBNLearner &&);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// learn a structure from a file (must have read the db before)
      DAG learnDAG();

      /// sets an initial DAG structure
      void setInitialDAG(const DAG &);

      /// returns the names of the variables in the database
      const std::vector<std::string> &names() const;

      /// returns the names of the variables in the database
      const std::vector<unsigned int> &modalities() noexcept;

      /// returns the node id corresponding to a variable name
      /**
       * @throw MissingVariableInDatabase if a variable of the BN is not found
       * in the database.
       */
      NodeId idFromName(const std::string &var_name) const;

      /// returns the variable name corresponding to a given node id
      const std::string &nameFromId(NodeId id) const;

      /// @}

      // ##########################################################################
      /// @name Score selection
      // ##########################################################################
      /// @{

      /// indicate that we wish to use an AIC score
      void useScoreAIC();

      /// indicate that we wish to use a BD score
      void useScoreBD();

      /// indicate that we wish to use a BDeu score
      void useScoreBDeu();

      /// indicate that we wish to use a BIC score
      void useScoreBIC();

      /// indicate that we wish to use a K2 score
      void useScoreK2();

      /// indicate that we wish to use a Log2Likelihood score
      void useScoreLog2Likelihood();

      /// @}

      // ##########################################################################
      /// @name A priori selection / parameterization
      // ##########################################################################
      /// @{

      /// sets the apriori weight
      void setAprioriWeight(float weight);

      /// use no apriori
      void useNoApriori();

      /// use the apriori smoothing
      /** @param weight pass in argument a weight if you wish to assign a weight
       * to the smoothing, else the current weight of the genericBNLearner will
       * be used. */
      void useAprioriSmoothing(float weight = -1);

      /// use the Dirichlet apriori
      void useAprioriDirichlet(const std::string &filename);

      /// @}

      // ##########################################################################
      /// @name Learning algorithm selection
      // ##########################################################################
      /// @{

      /// indicate that we wish to use a greedy hill climbing algorithm
      void useGreedyHillClimbing() noexcept;

      /// indicate that we wish to use a local search with tabu list
      /** @param tabu_size indicate the size of the tabu list
       * @param nb_decrease indicate the max number of changes decreasing the
       * score consecutively that we allow to apply */
      void useLocalSearchWithTabuList(unsigned int tabu_size = 100,
                                      unsigned int nb_decrease = 2) noexcept;

      /// indicate that we wish to use K2
      void useK2(const Sequence<NodeId> &order) noexcept;

      /// indicate that we wish to use K2
      void useK2(const std::vector<NodeId> &order) noexcept;

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers for adding constraints on learning
      // ##########################################################################
      /// @{

      /// sets the max indegree
      void setMaxIndegree(unsigned int max_indegree);

      /// sets a partial order on the nodes
      void setSliceOrder(const NodeProperty<unsigned int> &slice_order);

      /// assign a set of forbidden arcs
      void setForbiddenArcs(const ArcSet &set);

      /// @name assign a new forbidden arc
      /// @{
      void addForbiddenArc(const Arc &arc);
      void addForbiddenArc(const NodeId tail, const NodeId head);
      void addForbiddenArc(const std::string &tail, const std::string &head);
      /// @}

      /// @name remove a forbidden arc
      /// @{
      void eraseForbiddenArc(const Arc &arc);
      void eraseForbiddenArc(const NodeId tail, const NodeId head);
      void eraseForbiddenArc(const std::string &tail, const std::string &head);
      ///@}

      /// assign a set of forbidden arcs
      void setMandatoryArcs(const ArcSet &set);

      /// @name assign a new forbidden arc
      ///@{
      void addMandatoryArc(const Arc &arc);
      void addMandatoryArc(const NodeId tail, const NodeId head);
      void addMandatoryArc(const std::string &tail, const std::string &head);
      ///@}

      /// @name remove a forbidden arc
      ///@{
      void eraseMandatoryArc(const Arc &arc);
      void eraseMandatoryArc(const NodeId tail, const NodeId head);
      void eraseMandatoryArc(const std::string &tail, const std::string &head);
      /// @}

      ///@}

      protected:
      /// the score selected for learning
      ScoreType __score_type{ScoreType::BDeu};

      /// the score used
      Score<> *__score{nullptr};

      /// the type of the parameter estimator
      ParamEstimatorType __param_estimator_type{ParamEstimatorType::ML};

      /// the parameter estimator to use
      ParamEstimator<> *__param_estimator{nullptr};

      /// the a priori selected for the score and parameters
      AprioriType __apriori_type{AprioriType::NO_APRIORI};

      /// the apriori used
      Apriori<> *__apriori{nullptr};

      /// the weight of the apriori
      float __apriori_weight{1.0f};

      /// the constraint for 2TBNs
      StructuralConstraintSliceOrder __constraint_SliceOrder;

      /// the constraint for indegrees
      StructuralConstraintIndegree __constraint_Indegree;

      /// the constraint for tabu lists
      StructuralConstraintTabuList __constraint_TabuList;

      /// the constraint on forbidden arcs
      StructuralConstraintForbiddenArcs __constraint_ForbiddenArcs;

      /// the constraint on forbidden arcs
      StructuralConstraintMandatoryArcs __constraint_MandatoryArcs;

      /// the selected learning algorithm
      AlgoType __selected_algo{AlgoType::GREEDY_HILL_CLIMBING};

      /// the K2 algorithm
      K2 __K2;

      /// the greedy hill climbing algorithm
      GreedyHillClimbing __greedy_hill_climbing;

      /// the local search with tabu list algorithm
      LocalSearchWithTabuList __local_search_with_tabu_list;

      /// the database to be used by the scores and parameter estimators
      Database __score_database;

      /// indicates the values the user specified for the translators
      NodeProperty<Sequence<std::string>> __user_modalities;

      /// indicates whether we shall parse the database to update __user_modalities
      bool __modalities_parse_db{false};

      /// the database used by the Dirichlet a priori
      Database *__apriori_database{nullptr};

      /// the filename for the Dirichlet a priori, if any
      std::string __apriori_dbname;

      /// an initial DAG given to learners
      DAG __initial_dag;

      // the current algorithm as an approximationScheme
      const ApproximationScheme *__current_algorithm{nullptr};

      /// reads a file and returns a databaseVectInRam
      static DatabaseVectInRAM __readFile(const std::string &filename);

      /// create the apriori used for learning
      void __createApriori();

      /// create the score used for learning
      void __createScore();

      /// create the parameter estimator used for learning
      void __createParamEstimator(bool take_into_account_score = true);

      /// returns the DAG learnt
      DAG __learnDAG();

      /// checks whether the current score and apriori are compatible
      /** @returns true if the apriori is compatible with the score.
       * @throws IncompatibleScoreApriori is raised if the apriori is known to
       * be incompatible with the score. Such a case usually arises because the
       * score already implicitly contains an apriori which should not be combined
       * with the apriori passed in argument. aGrUM will nevertheless allow you to
       * use this apriori with the score, but you should be warned that the result
       * of learning will most probably be meaningless.
       * @throws PossiblyIncompatibleScoreApriori is raised if, in general, the
       * apriori is incompatible with the score but, with its current weight, it
       * becomes compatible (e.g., a Dirichlet apriori with a 0-weight is the
       * same as a NoApriori). In such a case, you should not modify the weight.
       * aGrUM will allow you to do so but the result of learning will most
       * probably be meaningless.
       * @throws InvalidArgument is raised if the apriori is not handled yet by
       * method isAprioriCompatible (the method needs be updated to take it into
       * account). */
      bool __checkScoreAprioriCompatibility();

      /// returns the type (as a string) of a given apriori
      const std::string &__getAprioriType() const;

      public:
      // ##########################################################################
      /// @name redistribute signals AND implemenation of interface
      /// IApproximationSchemeConfiguration
      // ##########################################################################
      // in order to not pollute the proper code of genericBNLearner, we directly
      // implement those
      // very simples methods here.
      /// {@    /// distribute signals
      INLINE void
      setCurrentApproximationScheme(const ApproximationScheme *approximationScheme) {
        __current_algorithm = approximationScheme;
      }

      INLINE void distributeProgress(const ApproximationScheme *approximationScheme,
                                     Size pourcent, double error, double time) {
        setCurrentApproximationScheme(approximationScheme);

        if (onProgress.hasListener())
          GUM_EMIT3(onProgress, pourcent, error, time);
      };

      /// distribute signals
      INLINE void distributeStop(const ApproximationScheme *approximationScheme,
                                 std::string message) {
        setCurrentApproximationScheme(approximationScheme);

        if (onStop.hasListener())
          GUM_EMIT1(onStop, message);
      };
      /// @}

      /// Given that we approximate f(t), stopping criterion on |f(t+1)-f(t)|
      /// If the criterion was disabled it will be enabled
      /// @{
      /// @throw OutOfLowerBound if eps<0
      void setEpsilon(double eps) {
        __K2.approximationScheme().setEpsilon(eps);
        __greedy_hill_climbing.setEpsilon(eps);
        __local_search_with_tabu_list.setEpsilon(eps);
      };

      /// Get the value of epsilon
      double epsilon(void) const {
        if (__current_algorithm != nullptr)
          return __current_algorithm->epsilon();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning");
      };

      /// Disable stopping criterion on epsilon
      void disableEpsilon() {
        __K2.approximationScheme().disableEpsilon();
        __greedy_hill_climbing.disableEpsilon();
        __local_search_with_tabu_list.disableEpsilon();
      };

      /// Enable stopping criterion on epsilon
      void enableEpsilon() {
        __K2.approximationScheme().enableEpsilon();
        __greedy_hill_climbing.enableEpsilon();
        __local_search_with_tabu_list.enableEpsilon();
      };

      /// @return true if stopping criterion on epsilon is enabled, false otherwise
      bool isEnabledEpsilon() const {
        if (__current_algorithm != nullptr)
          return __current_algorithm->isEnabledEpsilon();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning");
      };
      /// @}

      /// Given that we approximate f(t), stopping criterion on d/dt(|f(t+1)-f(t)|)
      /// If the criterion was disabled it will be enabled
      /// @{
      /// @throw OutOfLowerBound if rate<0
      void setMinEpsilonRate(double rate) {
        __K2.approximationScheme().setMinEpsilonRate(rate);
        __greedy_hill_climbing.setMinEpsilonRate(rate);
        __local_search_with_tabu_list.setMinEpsilonRate(rate);
      };

      /// Get the value of the minimal epsilon rate
      double minEpsilonRate(void) const {
        if (__current_algorithm != nullptr)
          return __current_algorithm->minEpsilonRate();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning");
      };

      /// Disable stopping criterion on epsilon rate
      void disableMinEpsilonRate() {
        __K2.approximationScheme().disableMinEpsilonRate();
        __greedy_hill_climbing.disableMinEpsilonRate();
        __local_search_with_tabu_list.disableMinEpsilonRate();
      };
      /// Enable stopping criterion on epsilon rate
      void enableMinEpsilonRate() {
        __K2.approximationScheme().enableMinEpsilonRate();
        __greedy_hill_climbing.enableMinEpsilonRate();
        __local_search_with_tabu_list.enableMinEpsilonRate();
      };
      /// @return true if stopping criterion on epsilon rate is enabled, false
      /// otherwise
      bool isEnabledMinEpsilonRate() const {
        if (__current_algorithm != nullptr)
          return __current_algorithm->isEnabledMinEpsilonRate();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning");
      };
      /// @}

      /// stopping criterion on number of iterations
      /// @{
      /// If the criterion was disabled it will be enabled
      /// @param max The maximum number of iterations
      /// @throw OutOfLowerBound if max<=1
      void setMaxIter(Size max) {
        __K2.approximationScheme().setMaxIter(max);
        __greedy_hill_climbing.setMaxIter(max);
        __local_search_with_tabu_list.setMaxIter(max);
      };

      /// @return the criterion on number of iterations
      Size maxIter(void) const {
        if (__current_algorithm != nullptr)
          return __current_algorithm->maxIter();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning");
      };

      /// Disable stopping criterion on max iterations
      void disableMaxIter() {
        __K2.approximationScheme().disableMaxIter();
        __greedy_hill_climbing.disableMaxIter();
        __local_search_with_tabu_list.disableMaxIter();
      };
      /// Enable stopping criterion on max iterations
      void enableMaxIter() {
        __K2.approximationScheme().enableMaxIter();
        __greedy_hill_climbing.enableMaxIter();
        __local_search_with_tabu_list.enableMaxIter();
      };
      /// @return true if stopping criterion on max iterations is enabled, false
      /// otherwise
      bool isEnabledMaxIter() const {
        if (__current_algorithm != nullptr)
          return __current_algorithm->isEnabledMaxIter();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning");
      };
      /// @}

      /// stopping criterion on timeout
      /// If the criterion was disabled it will be enabled
      /// @{
      /// @throw OutOfLowerBound if timeout<=0.0
      /** timeout is time in second (double).
       */
      void setMaxTime(double timeout) {
        __K2.approximationScheme().setMaxTime(timeout);
        __greedy_hill_climbing.setMaxTime(timeout);
        __local_search_with_tabu_list.setMaxTime(timeout);
      }

      /// returns the timeout (in seconds)
      double maxTime(void) const {
        if (__current_algorithm != nullptr)
          return __current_algorithm->maxTime();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning");
      };

      /// get the current running time in second (double)
      double currentTime(void) const {
        if (__current_algorithm != nullptr)
          return __current_algorithm->currentTime();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning");
      };

      /// Disable stopping criterion on timeout
      void disableMaxTime() {
        __K2.approximationScheme().disableMaxTime();
        __greedy_hill_climbing.disableMaxTime();
        __local_search_with_tabu_list.disableMaxTime();
      };
      void enableMaxTime() {
        __K2.approximationScheme().enableMaxTime();
        __greedy_hill_climbing.enableMaxTime();
        __local_search_with_tabu_list.enableMaxTime();
      };
      /// @return true if stopping criterion on timeout is enabled, false otherwise
      bool isEnabledMaxTime() const {
        if (__current_algorithm != nullptr)
          return __current_algorithm->isEnabledMaxTime();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning");
      };
      /// @}

      /// how many samples between 2 stopping isEnableds
      /// @{
      /// @throw OutOfLowerBound if p<1
      void setPeriodSize(Size p) {
        __K2.approximationScheme().setPeriodSize(p);
        __greedy_hill_climbing.setPeriodSize(p);
        __local_search_with_tabu_list.setPeriodSize(p);
      };

      Size periodSize(void) const {
        if (__current_algorithm != nullptr)
          return __current_algorithm->periodSize();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning");
      };
      /// @}

      /// size of burn in on number of iteration
      /// @{

      /// @throw OutOfLowerBound if b<1
      void setBurnIn(Size b) {
        __K2.approximationScheme().setBurnIn(b);
        __greedy_hill_climbing.setBurnIn(b);
        __local_search_with_tabu_list.setBurnIn(b);
      };

      Size burnIn(void) const {
        if (__current_algorithm != nullptr)
          return __current_algorithm->burnIn();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning");
      };
      /// @}

      /// verbosity
      /// @{
      void setVerbosity(bool v) {
        __K2.approximationScheme().setVerbosity(v);
        __greedy_hill_climbing.setVerbosity(v);
        __local_search_with_tabu_list.setVerbosity(v);
      };

      bool verbosity(void) const {
        if (__current_algorithm != nullptr)
          return __current_algorithm->verbosity();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning");
      };
      /// @}

      /// history
      /// @{

      ApproximationSchemeSTATE stateApproximationScheme() const {
        if (__current_algorithm != nullptr)
          return __current_algorithm->stateApproximationScheme();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning");
      };

      /// @throw OperationNotAllowed if scheme not performed
      Size nbrIterations() const {
        if (__current_algorithm != nullptr)
          return __current_algorithm->nbrIterations();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning");
      };

      /// @throw OperationNotAllowed if scheme not performed or verbosity=false
      const std::vector<double> &history() const {
        if (__current_algorithm != nullptr)
          return __current_algorithm->history();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning");
      };
      /// @}
    };

  } /* namespace learning */

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/BNLearnUtils/genericBNLearner.inl>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_GENERIC_BN_LEARNER_H */
