
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


/**
 * @file
 * @brief A class for generic framework of learning algorithms that can easily
 * be used.
 *
 * The pack currently contains K2, GreedyHillClimbing, miic, 3off2 and
 * LocalSearchWithTabuList
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_GENERIC_BN_LEARNER_H
#define GUM_LEARNING_GENERIC_BN_LEARNER_H

#include <sstream>
#include <memory>

#include <agrum/BN/BayesNet.h>
#include <agrum/agrum.h>
#include <agrum/core/bijection.h>
#include <agrum/core/sequence.h>
#include <agrum/graphs/DAG.h>

#include <agrum/learning/database/DBTranslator4LabelizedVariable.h>
#include <agrum/learning/database/DBRowGeneratorParser.h>
#include <agrum/learning/database/DBInitializerFromCSV.h>
#include <agrum/learning/database/databaseTable.h>
#include <agrum/learning/database/DBRowGeneratorParser.h>
#include <agrum/learning/database/DBRowGenerator4CompleteRows.h>
#include <agrum/learning/database/DBRowGeneratorEM.h>
#include <agrum/learning/database/DBRowGeneratorSet.h>

#include <agrum/learning/scores_and_tests/scoreAIC.h>
#include <agrum/learning/scores_and_tests/scoreBD.h>
#include <agrum/learning/scores_and_tests/scoreBDeu.h>
#include <agrum/learning/scores_and_tests/scoreBIC.h>
#include <agrum/learning/scores_and_tests/scoreK2.h>
#include <agrum/learning/scores_and_tests/scoreLog2Likelihood.h>

#include <agrum/learning/aprioris/aprioriDirichletFromDatabase.h>
#include <agrum/learning/aprioris/aprioriNoApriori.h>
#include <agrum/learning/aprioris/aprioriSmoothing.h>
#include <agrum/learning/aprioris/aprioriBDeu.h>

#include <agrum/learning/constraints/structuralConstraintDAG.h>
#include <agrum/learning/constraints/structuralConstraintDiGraph.h>
#include <agrum/learning/constraints/structuralConstraintForbiddenArcs.h>
#include <agrum/learning/constraints/structuralConstraintPossibleEdges.h>
#include <agrum/learning/constraints/structuralConstraintIndegree.h>
#include <agrum/learning/constraints/structuralConstraintMandatoryArcs.h>
#include <agrum/learning/constraints/structuralConstraintSetStatic.h>
#include <agrum/learning/constraints/structuralConstraintSliceOrder.h>
#include <agrum/learning/constraints/structuralConstraintTabuList.h>

#include <agrum/learning/structureUtils/graphChange.h>
#include <agrum/learning/structureUtils/graphChangesGenerator4DiGraph.h>
#include <agrum/learning/structureUtils/graphChangesGenerator4K2.h>
#include <agrum/learning/structureUtils/graphChangesSelector4DiGraph.h>

#include <agrum/learning/paramUtils/DAG2BNLearner.h>
#include <agrum/learning/paramUtils/paramEstimatorML.h>

#include <agrum/core/approximations/IApproximationSchemeConfiguration.h>
#include <agrum/core/approximations/approximationSchemeListener.h>

#include <agrum/learning/K2.h>
#include <agrum/learning/Miic.h>
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
     * LocalSearchWithTabuList also 3off2/miic
     * @ingroup learning_group
     */
    class genericBNLearner : public gum::IApproximationSchemeConfiguration {
      // private:
      public:
      /// an enumeration enabling to select easily the score we wish to use
      enum class ScoreType { AIC, BD, BDeu, BIC, K2, LOG2LIKELIHOOD };

      /// an enumeration to select the type of parameter estimation we shall
      /// apply
      enum class ParamEstimatorType { ML };

      /// an enumeration to select the apriori
      enum class AprioriType {
        NO_APRIORI,
        SMOOTHING,
        DIRICHLET_FROM_DATABASE,
        BDEU
      };

      /// an enumeration to select easily the learning algorithm to use
      enum class AlgoType {
        K2,
        GREEDY_HILL_CLIMBING,
        LOCAL_SEARCH_WITH_TABU_LIST,
        MIIC_THREE_OFF_TWO
      };


      /// a helper to easily read databases
      class Database {
        public:
        // ########################################################################
        /// @name Constructors / Destructors
        // ########################################################################
        /// @{

        /// default constructor
        /** @param file the name of the CSV file containing the data
         * @param missing_symbols the set of symbols in the CSV file that
         * correspond to missing data */
        explicit Database(const std::string&                file,
                          const std::vector< std::string >& missing_symbols);

        /// default constructor
        /** @param db an already initialized database table that is used to
         * fill the Database */
        explicit Database(const DatabaseTable<>& db);

        /// constructor for the aprioris
        /** We must ensure that the variables of the Database are identical to
         * those of the score database (else the countings used by the
         * scores might be erroneous). However, we allow the variables to be
         * ordered differently in the two databases: variables with the same
         * name in both databases are supposed to be the same.
         * @param file the name of the CSV file containing the data
         * @param score_database the main database used for the learning
         * @param missing_symbols the set of symbols in the CSV file that
         * correspond to missing data
         */
        Database(const std::string&                filename,
                 Database&                         score_database,
                 const std::vector< std::string >& missing_symbols);

        /// constructor with a BN providing the variables of interest
        /** @param file the name of the CSV file containing the data
         * @param bn a Bayesian network indicating which variables of the CSV
         * file are used for learning
         * @param missing_symbols the set of symbols in the CSV file that
         * correspond to missing data
         */
        template < typename GUM_SCALAR >
        Database(const std::string&                 filename,
                 const gum::BayesNet< GUM_SCALAR >& bn,
                 const std::vector< std::string >&  missing_symbols);

        /// copy constructor
        Database(const Database& from);

        /// move constructor
        Database(Database&& from);

        /// destructor
        ~Database();

        /// @}

        // ########################################################################
        /// @name Operators
        // ########################################################################
        /// @{

        /// copy operator
        Database& operator=(const Database& from);

        /// move operator
        Database& operator=(Database&& from);

        /// @}

        // ########################################################################
        /// @name Accessors / Modifiers
        // ########################################################################
        /// @{

        /// returns the parser for the database
        DBRowGeneratorParser<>& parser();

        /// returns the domain sizes of the variables
        const std::vector< std::size_t >& domainSizes() const;

        /// returns the names of the variables in the database
        const std::vector< std::string >& names() const;

        /// returns the node id corresponding to a variable name
        NodeId idFromName(const std::string& var_name) const;

        /// returns the variable name corresponding to a given node id
        const std::string& nameFromId(NodeId id) const;

        /// returns the internal database table
        const DatabaseTable<>& databaseTable() const;

        /** @brief assign a weight to all the rows of the database so
         * that the sum of their weights is equal to new_weight */
        void setDatabaseWeight(const double new_weight);

        /// returns the mapping between node ids and their columns in the database
        const Bijection< NodeId, std::size_t >& nodeId2Columns() const;

        /// returns the set of missing symbols taken into account
        const std::vector< std::string >& missingSymbols() const;

        /// returns the number of records in the database
        std::size_t nbRows() const;

        /// returns the number of records in the database
        std::size_t size() const;

        /// sets the weight of the ith record
        /** @throws OutOfBounds if i is outside the set of indices of the
         * records or if the weight is negative
         */
        void setWeight(const std::size_t i, const double weight);

        /// returns the weight of the ith record
        /** @throws OutOfBounds if i is outside the set of indices of the
         * records */
        double weight(const std::size_t i) const;

        /// returns the weight of the whole database
        double weight() const;


        /// @}

        protected:
        /// the database itself
        DatabaseTable<> __database;

        /// the parser used for reading the database
        DBRowGeneratorParser<>* __parser{nullptr};

        /// the domain sizes of the variables (useful to speed-up computations)
        std::vector< std::size_t > __domain_sizes;

        /// a bijection assigning to each variable name its NodeId
        Bijection< NodeId, std::size_t > __nodeId2cols;

/// the max number of threads authorized
#if defined(_OPENMP) && !defined(GUM_DEBUG_MODE)
        Size __max_threads_number{getMaxNumberOfThreads()};
#else
        Size __max_threads_number{1};
#endif /* GUM_DEBUG_MODE */

        /// the minimal number of rows to parse (on average) by thread
        Size __min_nb_rows_per_thread{100};

        private:
        // returns the set of variables as a BN. This is convenient for
        // the constructors of apriori Databases
        template < typename GUM_SCALAR >
        BayesNet< GUM_SCALAR > __BNVars() const;
      };

      /// sets the apriori weight
      void __setAprioriWeight(double weight);

      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /**
       * read the database file for the score / parameter estimation and var
       * names
       */
      genericBNLearner(const std::string&                filename,
                       const std::vector< std::string >& missing_symbols);
      genericBNLearner(const DatabaseTable<>& db);

      /**
       * read the database file for the score / parameter estimation and var
       * names
       * @param filename The file to learn from.
       * @param modalities indicate for some nodes (not necessarily all the
       * nodes of the BN) which modalities they should have and in which order
       * these modalities should be stored into the nodes. For instance, if
       * modalities = { 1 -> {True, False, Big} }, then the node of id 1 in the
       * BN will have 3 modalities, the first one being True, the second one
       * being False, and the third bein Big.
       * @param parse_database if true, the modalities specified by the user
       * will be considered as a superset of the modalities of the variables. A
       * parsing of the database will allow to determine which ones are really
       * necessary and will keep them in the order specified by the user
       * (NodeProperty modalities). If parse_database is set to false (the
       * default), then the modalities specified by the user will be considered
       * as being exactly those of the variables of the BN (as a consequence,
       * if we find other values in the database, an exception will be raised
       * during learning). */
      template < typename GUM_SCALAR >
      genericBNLearner(const std::string&                 filename,
                       const gum::BayesNet< GUM_SCALAR >& src,
                       const std::vector< std::string >&  missing_symbols);

      /// copy constructor
      genericBNLearner(const genericBNLearner&);

      /// move constructor
      genericBNLearner(genericBNLearner&&);

      /// destructor
      virtual ~genericBNLearner();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      genericBNLearner& operator=(const genericBNLearner&);

      /// move operator
      genericBNLearner& operator=(genericBNLearner&&);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// learn a structure from a file (must have read the db before)
      DAG learnDAG();

      /// learn a partial structure from a file (must have read the db before and
      /// must have selected miic or 3off2)
      MixedGraph learnMixedStructure();

      /// sets an initial DAG structure
      void setInitialDAG(const DAG&);

      /// returns the names of the variables in the database
      const std::vector< std::string >& names() const;

      /// returns the domain sizes of the variables in the database
      const std::vector< std::size_t >& domainSizes() const;

      /// returns the node id corresponding to a variable name
      /**
       * @throw MissingVariableInDatabase if a variable of the BN is not found
       * in the database.
       */
      NodeId idFromName(const std::string& var_name) const;

      /// returns the database used by the BNLearner
      const DatabaseTable<>& database() const;

      /** @brief assign a weight to all the rows of the learning database so
       * that the sum of their weights is equal to new_weight */
      void setDatabaseWeight(const double new_weight);

      /// sets the weight of the ith record of the database
      /** @throws OutOfBounds if i is outside the set of indices of the
       * records or if the weight is negative
       */
      void setRecordWeight(const std::size_t i, const double weight);

      /// returns the weight of the ith record
      /** @throws OutOfBounds if i is outside the set of indices of the
       * records */
      double recordWeight(const std::size_t i) const;

      /// returns the weight of the whole database
      double databaseWeight() const;

      /// returns the variable name corresponding to a given node id
      const std::string& nameFromId(NodeId id) const;

      /// use a new set of database rows' ranges to perform learning
      /** @param ranges a set of pairs {(X1,Y1),...,(Xn,Yn)} of database's rows
       * indices. The subsequent learnings are then performed only on the union
       * of the rows [Xi,Yi), i in {1,...,n}. This is useful, e.g, when
       * performing cross validation tasks, in which part of the database should
       * be ignored. An empty set of ranges is equivalent to an interval [X,Y)
       * ranging over the whole database. */
      template < template < typename > class XALLOC >
      void useDatabaseRanges(
         const std::vector< std::pair< std::size_t, std::size_t >,
                            XALLOC< std::pair< std::size_t, std::size_t > > >&
            new_ranges);

      /// reset the ranges to the one range corresponding to the whole database
      void clearDatabaseRanges();

      /// returns the current database rows' ranges used for learning
      /** @return The method returns a vector of pairs [Xi,Yi) of indices of
       * rows in the database. The learning is performed on these set of rows.
       * @warning an empty set of ranges means the whole database. */
      const std::vector< std::pair< std::size_t, std::size_t > >&
         databaseRanges() const;

      /// sets the ranges of rows to be used for cross-validation learning
      /** When applied on (x,k), the method indicates to the subsequent learnings
       * that they should be performed on the xth fold in a k-fold
       * cross-validation context. For instance, if a database has 1000 rows,
       * and if we perform a 10-fold cross-validation, then, the first learning
       * fold (learning_fold=0) corresponds to rows interval [100,1000) and the
       * test dataset corresponds to [0,100). The second learning fold
       * (learning_fold=1) is [0,100) U [200,1000) and the corresponding test
       * dataset is [100,200).
       * @param learning_fold a number indicating the set of rows used for
       * learning. If N denotes the size of the database, and k_fold represents
       * the number of folds in the cross validation, then the set of rows
       * used for testing is [learning_fold * N / k_fold,
       * (learning_fold+1) * N / k_fold) and the learning database is the
       * complement in the database
       * @param k_fold the value of "k" in k-fold cross validation
       * @return a pair [x,y) of rows' indices that corresponds to the indices
       * of rows in the original database that constitute the test dataset
       * @throws OutOfBounds is raised if k_fold is equal to 0 or learning_fold
       * is greater than or eqal to k_fold, or if k_fold is greater than
       * or equal to the size of the database. */
      std::pair< std::size_t, std::size_t >
         useCrossValidationFold(const std::size_t learning_fold,
                                const std::size_t k_fold);


      /**
       * Return the <statistic,pvalue> pair for chi2 test in the database
       * @param id1 first variable
       * @param id2 second variable
       * @param knowing list of observed variables
       * @return a std::pair<double,double>
       */
      std::pair< double, double > chi2(const NodeId                 id1,
                                       const NodeId                 id2,
                                       const std::vector< NodeId >& knowing = {});
      /**
       * Return the <statistic,pvalue> pair for the BNLearner
       * @param id1 first variable
       * @param id2 second variable
       * @param knowing list of observed variables
       * @return a std::pair<double,double>
       */
      std::pair< double, double >
         chi2(const std::string&                name1,
              const std::string&                name2,
              const std::vector< std::string >& knowing = {});

      /**
       * Return the <statistic,pvalue> pair for for G2 test in the database
       * @param id1 first variable
       * @param id2 second variable
       * @param knowing list of observed variables
       * @return a std::pair<double,double>
       */
      std::pair< double, double > G2(const NodeId                 id1,
                                     const NodeId                 id2,
                                     const std::vector< NodeId >& knowing = {});
      /**
       * Return the <statistic,pvalue> pair for for G2 test in the database
       * @param id1 first variable
       * @param id2 second variable
       * @param knowing list of observed variables
       * @return a std::pair<double,double>
       */
      std::pair< double, double >
         G2(const std::string&                name1,
            const std::string&                name2,
            const std::vector< std::string >& knowing = {});

      /**
       * Return the loglikelihood of vars in the base, conditioned by knowing for
       * the BNLearner
       * @param vars a vector of NodeIds
       * @param knowing an optional vector of conditioning NodeIds
       * @return a std::pair<double,double>
       */
      double logLikelihood(const std::vector< NodeId >& vars,
                           const std::vector< NodeId >& knowing = {});

      /**
       * Return the loglikelihood of vars in the base, conditioned by knowing for
       * the BNLearner
       * @param vars a vector of name of rows
       * @param knowing an optional vector of conditioning rows
       * @return a std::pair<double,double>
       */
      double logLikelihood(const std::vector< std::string >& vars,
                           const std::vector< std::string >& knowing = {});


      /**
       *
       * @return the number of cols in the database
       */
      Size nbCols() const;

      /**
       *
       * @return the number of rows in the database
       */
      Size nbRows() const;

      /** use The EM algorithm to learn paramters
       *
       * if epsilon=0, EM is not used
       */
      void useEM(const double epsilon);

      /// returns true if the learner's database has missing values
      bool hasMissingValues() const;

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

      /// use no apriori
      void useNoApriori();

      /// use the BDeu apriori
      /** The BDeu apriori adds weight to all the cells of the countings
       * tables. In other words, it adds weight rows in the database with
       * equally probable values. */
      void useAprioriBDeu(double weight = 1);

      /// use the apriori smoothing
      /** @param weight pass in argument a weight if you wish to assign a weight
       * to the smoothing, else the current weight of the genericBNLearner will
       * be used. */
      void useAprioriSmoothing(double weight = 1);

      /// use the Dirichlet apriori
      void useAprioriDirichlet(const std::string& filename, double weight = 1);


      /// checks whether the current score and apriori are compatible
      /** @returns a non empty string if the apriori is somehow compatible with the
       * score.*/
      std::string checkScoreAprioriCompatibility();
      /// @}

      // ##########################################################################
      /// @name Learning algorithm selection
      // ##########################################################################
      /// @{

      /// indicate that we wish to use a greedy hill climbing algorithm
      void useGreedyHillClimbing();

      /// indicate that we wish to use a local search with tabu list
      /** @param tabu_size indicate the size of the tabu list
       * @param nb_decrease indicate the max number of changes decreasing the
       * score consecutively that we allow to apply */
      void useLocalSearchWithTabuList(Size tabu_size = 100, Size nb_decrease = 2);

      /// indicate that we wish to use K2
      void useK2(const Sequence< NodeId >& order);

      /// indicate that we wish to use K2
      void useK2(const std::vector< NodeId >& order);

      /// indicate that we wish to use 3off2
      void use3off2();

      /// indicate that we wish to use MIIC
      void useMIIC();

      /// @}

      // ##########################################################################
      /// @name 3off2/MIIC parameterization and specific results
      // ##########################################################################
      /// @{
      /// indicate that we wish to use the NML correction for 3off2
      /// @throws OperationNotAllowed when 3off2 is not the selected algorithm
      void useNML();
      /// indicate that we wish to use the MDL correction for 3off2
      /// @throws OperationNotAllowed when 3off2 is not the selected algorithm
      void useMDL();
      /// indicate that we wish to use the NoCorr correction for 3off2
      /// @throws OperationNotAllowed when 3off2 is not the selected algorithm
      void useNoCorr();

      /// get the list of arcs hiding latent variables
      /// @throws OperationNotAllowed when 3off2 is not the selected algorithm
      const std::vector< Arc > latentVariables() const;

      /// @}
      // ##########################################################################
      /// @name Accessors / Modifiers for adding constraints on learning
      // ##########################################################################
      /// @{

      /// sets the max indegree
      void setMaxIndegree(Size max_indegree);

      /**
       * sets a partial order on the nodes
       * @param slice_order a NodeProperty given the rank (priority) of nodes in
       * the partial order
       */
      void setSliceOrder(const NodeProperty< NodeId >& slice_order);

      /**
       * sets a partial order on the nodes
       * @param slices the list of list of variable names
       */
      void setSliceOrder(const std::vector< std::vector< std::string > >& slices);

      /// assign a set of forbidden arcs
      void setForbiddenArcs(const ArcSet& set);

      /// @name assign a new forbidden arc
      /// @{
      void addForbiddenArc(const Arc& arc);
      void addForbiddenArc(const NodeId tail, const NodeId head);
      void addForbiddenArc(const std::string& tail, const std::string& head);
      /// @}

      /// @name remove a forbidden arc
      /// @{
      void eraseForbiddenArc(const Arc& arc);
      void eraseForbiddenArc(const NodeId tail, const NodeId head);
      void eraseForbiddenArc(const std::string& tail, const std::string& head);
      ///@}

      /// assign a set of forbidden arcs
      void setMandatoryArcs(const ArcSet& set);

      /// @name assign a new forbidden arc
      ///@{
      void addMandatoryArc(const Arc& arc);
      void addMandatoryArc(const NodeId tail, const NodeId head);
      void addMandatoryArc(const std::string& tail, const std::string& head);
      ///@}

      /// @name remove a forbidden arc
      ///@{
      void eraseMandatoryArc(const Arc& arc);
      void eraseMandatoryArc(const NodeId tail, const NodeId head);
      void eraseMandatoryArc(const std::string& tail, const std::string& head);
      /// @}

      /// assign a set of forbidden edges
      /// @warning Once at least one possible edge is defined, all other edges are
      /// not possible anymore
      /// @{
      void setPossibleEdges(const EdgeSet& set);
      void setPossibleSkeleton(const UndiGraph& skeleton);
      /// @}

      /// @name assign a new possible edge
      /// @warning Once at least one possible edge is defined, all other edges are
      /// not possible anymore
      /// @{
      void addPossibleEdge(const Edge& edge);
      void addPossibleEdge(const NodeId tail, const NodeId head);
      void addPossibleEdge(const std::string& tail, const std::string& head);
      /// @}

      /// @name remove a possible edge
      /// @{
      void erasePossibleEdge(const Edge& edge);
      void erasePossibleEdge(const NodeId tail, const NodeId head);
      void erasePossibleEdge(const std::string& tail, const std::string& head);
      ///@}

      ///@}

      protected:
      /// the score selected for learning
      ScoreType __score_type{ScoreType::BDeu};

      /// the score used
      Score<>* __score{nullptr};

      /// the type of the parameter estimator
      ParamEstimatorType __param_estimator_type{ParamEstimatorType::ML};

      /// epsilon for EM. if espilon=0.0 : no EM
      double __EMepsilon{0.0};

      /// the selected correction for 3off2 and miic
      CorrectedMutualInformation<>* __mutual_info{nullptr};

      /// the a priori selected for the score and parameters
      AprioriType __apriori_type{AprioriType::NO_APRIORI};

      /// the apriori used
      Apriori<>* __apriori{nullptr};

      AprioriNoApriori<>* __no_apriori{nullptr};

      /// the weight of the apriori
      double __apriori_weight{1.0f};

      /// the constraint for 2TBNs
      StructuralConstraintSliceOrder __constraint_SliceOrder;

      /// the constraint for indegrees
      StructuralConstraintIndegree __constraint_Indegree;

      /// the constraint for tabu lists
      StructuralConstraintTabuList __constraint_TabuList;

      /// the constraint on forbidden arcs
      StructuralConstraintForbiddenArcs __constraint_ForbiddenArcs;

      /// the constraint on possible Edges
      StructuralConstraintPossibleEdges __constraint_PossibleEdges;

      /// the constraint on forbidden arcs
      StructuralConstraintMandatoryArcs __constraint_MandatoryArcs;

      /// the selected learning algorithm
      AlgoType __selected_algo{AlgoType::GREEDY_HILL_CLIMBING};

      /// the K2 algorithm
      K2 __K2;

      /// the 3off2 algorithm
      Miic __miic_3off2;

      /// the penalty used in 3off2
      typename CorrectedMutualInformation<>::KModeTypes __3off2_kmode{
         CorrectedMutualInformation<>::KModeTypes::MDL};

      /// the parametric EM
      DAG2BNLearner<> __Dag2BN;

      /// the greedy hill climbing algorithm
      GreedyHillClimbing __greedy_hill_climbing;

      /// the local search with tabu list algorithm
      LocalSearchWithTabuList __local_search_with_tabu_list;

      /// the database to be used by the scores and parameter estimators
      Database __score_database;

      /// the set of rows' ranges within the database in which learning is done
      std::vector< std::pair< std::size_t, std::size_t > > __ranges;

      /// the database used by the Dirichlet a priori
      Database* __apriori_database{nullptr};

      /// the filename for the Dirichlet a priori, if any
      std::string __apriori_dbname;

      /// an initial DAG given to learners
      DAG __initial_dag;

      // the current algorithm as an approximationScheme
      const ApproximationScheme* __current_algorithm{nullptr};

      /// reads a file and returns a databaseVectInRam
      static DatabaseTable<>
         __readFile(const std::string&                filename,
                    const std::vector< std::string >& missing_symbols);

      /// checks whether the extension of a CSV filename is correct
      static void __checkFileName(const std::string& filename);

      /// create the apriori used for learning
      void __createApriori();

      /// create the score used for learning
      void __createScore();

      /// create the parameter estimator used for learning
      ParamEstimator<>*
         __createParamEstimator(DBRowGeneratorParser<>& parser,
                                bool take_into_account_score = true);

      /// returns the DAG learnt
      DAG __learnDAG();

      /// prepares the initial graph for 3off2 or miic
      MixedGraph __prepare_miic_3off2();

      /// returns the type (as a string) of a given apriori
      const std::string& __getAprioriType() const;

      /// create the Corrected Mutual Information instance for Miic/3off2
      void __createCorrectedMutualInformation();


      public:
      // ##########################################################################
      /// @name redistribute signals AND implemenation of interface
      /// IApproximationSchemeConfiguration
      // ##########################################################################
      // in order to not pollute the proper code of genericBNLearner, we
      // directly
      // implement those
      // very simples methods here.
      /// {@    /// distribute signals
      INLINE void setCurrentApproximationScheme(
         const ApproximationScheme* approximationScheme) {
        __current_algorithm = approximationScheme;
      }

      INLINE void
         distributeProgress(const ApproximationScheme* approximationScheme,
                            Size                       pourcent,
                            double                     error,
                            double                     time) {
        setCurrentApproximationScheme(approximationScheme);

        if (onProgress.hasListener()) GUM_EMIT3(onProgress, pourcent, error, time);
      };

      /// distribute signals
      INLINE void distributeStop(const ApproximationScheme* approximationScheme,
                                 std::string                message) {
        setCurrentApproximationScheme(approximationScheme);

        if (onStop.hasListener()) GUM_EMIT1(onStop, message);
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
        __Dag2BN.setEpsilon(eps);
      };

      /// Get the value of epsilon
      double epsilon() const {
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
        __Dag2BN.disableEpsilon();
      };

      /// Enable stopping criterion on epsilon
      void enableEpsilon() {
        __K2.approximationScheme().enableEpsilon();
        __greedy_hill_climbing.enableEpsilon();
        __local_search_with_tabu_list.enableEpsilon();
        __Dag2BN.enableEpsilon();
      };

      /// @return true if stopping criterion on epsilon is enabled, false
      /// otherwise
      bool isEnabledEpsilon() const {
        if (__current_algorithm != nullptr)
          return __current_algorithm->isEnabledEpsilon();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning");
      };
      /// @}

      /// Given that we approximate f(t), stopping criterion on
      /// d/dt(|f(t+1)-f(t)|)
      /// If the criterion was disabled it will be enabled
      /// @{
      /// @throw OutOfLowerBound if rate<0
      void setMinEpsilonRate(double rate) {
        __K2.approximationScheme().setMinEpsilonRate(rate);
        __greedy_hill_climbing.setMinEpsilonRate(rate);
        __local_search_with_tabu_list.setMinEpsilonRate(rate);
        __Dag2BN.setMinEpsilonRate(rate);
      };

      /// Get the value of the minimal epsilon rate
      double minEpsilonRate() const {
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
        __Dag2BN.disableMinEpsilonRate();
      };
      /// Enable stopping criterion on epsilon rate
      void enableMinEpsilonRate() {
        __K2.approximationScheme().enableMinEpsilonRate();
        __greedy_hill_climbing.enableMinEpsilonRate();
        __local_search_with_tabu_list.enableMinEpsilonRate();
        __Dag2BN.enableMinEpsilonRate();
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
        __Dag2BN.setMaxIter(max);
      };

      /// @return the criterion on number of iterations
      Size maxIter() const {
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
        __Dag2BN.disableMaxIter();
      };
      /// Enable stopping criterion on max iterations
      void enableMaxIter() {
        __K2.approximationScheme().enableMaxIter();
        __greedy_hill_climbing.enableMaxIter();
        __local_search_with_tabu_list.enableMaxIter();
        __Dag2BN.enableMaxIter();
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
        __Dag2BN.setMaxTime(timeout);
      }

      /// returns the timeout (in seconds)
      double maxTime() const {
        if (__current_algorithm != nullptr)
          return __current_algorithm->maxTime();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning");
      };

      /// get the current running time in second (double)
      double currentTime() const {
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
        __Dag2BN.disableMaxTime();
      };
      void enableMaxTime() {
        __K2.approximationScheme().enableMaxTime();
        __greedy_hill_climbing.enableMaxTime();
        __local_search_with_tabu_list.enableMaxTime();
        __Dag2BN.enableMaxTime();
      };
      /// @return true if stopping criterion on timeout is enabled, false
      /// otherwise
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
        __Dag2BN.setPeriodSize(p);
      };

      Size periodSize() const {
        if (__current_algorithm != nullptr)
          return __current_algorithm->periodSize();
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
        __Dag2BN.setVerbosity(v);
      };

      bool verbosity() const {
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
      const std::vector< double >& history() const {
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
#  include <agrum/learning/BNLearnUtils/genericBNLearner_inl.h>
#endif /* GUM_NO_INLINE */

#include <agrum/learning/BNLearnUtils/genericBNLearner_tpl.h>

#endif /* GUM_LEARNING_GENERIC_BN_LEARNER_H */
