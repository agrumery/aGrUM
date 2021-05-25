/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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


/**
 * @file
 * @brief A class for generic framework of learning algorithms that can easily
 * be used.
 *
 * The pack currently contains K2, GreedyHillClimbing, miic, 3off2 and
 * LocalSearchWithTabuList
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#ifndef GUM_LEARNING_GENERIC_BN_LEARNER_H
#define GUM_LEARNING_GENERIC_BN_LEARNER_H

#include <sstream>
#include <memory>

#include <agrum/BN/BayesNet.h>
#include <agrum/agrum.h>
#include <agrum/tools/core/bijection.h>
#include <agrum/tools/core/sequence.h>
#include <agrum/tools/graphs/DAG.h>

#include <agrum/tools/database/DBTranslator4LabelizedVariable.h>
#include <agrum/tools/database/DBRowGeneratorParser.h>
#include <agrum/tools/database/DBInitializerFromCSV.h>
#include <agrum/tools/database/databaseTable.h>
#include <agrum/tools/database/DBRowGeneratorParser.h>
#include <agrum/tools/database/DBRowGenerator4CompleteRows.h>
#include <agrum/tools/database/DBRowGeneratorEM.h>
#include <agrum/tools/database/DBRowGeneratorSet.h>

#include <agrum/BN/learning/scores_and_tests/scoreAIC.h>
#include <agrum/BN/learning/scores_and_tests/scoreBD.h>
#include <agrum/BN/learning/scores_and_tests/scoreBDeu.h>
#include <agrum/BN/learning/scores_and_tests/scoreBIC.h>
#include <agrum/BN/learning/scores_and_tests/scoreK2.h>
#include <agrum/BN/learning/scores_and_tests/scoreLog2Likelihood.h>

#include <agrum/BN/learning/aprioris/aprioriDirichletFromDatabase.h>
#include <agrum/BN/learning/aprioris/aprioriNoApriori.h>
#include <agrum/BN/learning/aprioris/aprioriSmoothing.h>
#include <agrum/BN/learning/aprioris/aprioriBDeu.h>

#include <agrum/BN/learning/constraints/structuralConstraintDAG.h>
#include <agrum/BN/learning/constraints/structuralConstraintDiGraph.h>
#include <agrum/BN/learning/constraints/structuralConstraintForbiddenArcs.h>
#include <agrum/BN/learning/constraints/structuralConstraintPossibleEdges.h>
#include <agrum/BN/learning/constraints/structuralConstraintIndegree.h>
#include <agrum/BN/learning/constraints/structuralConstraintMandatoryArcs.h>
#include <agrum/BN/learning/constraints/structuralConstraintSetStatic.h>
#include <agrum/BN/learning/constraints/structuralConstraintSliceOrder.h>
#include <agrum/BN/learning/constraints/structuralConstraintTabuList.h>

#include <agrum/BN/learning/structureUtils/graphChange.h>
#include <agrum/BN/learning/structureUtils/graphChangesGenerator4DiGraph.h>
#include <agrum/BN/learning/structureUtils/graphChangesGenerator4K2.h>
#include <agrum/BN/learning/structureUtils/graphChangesSelector4DiGraph.h>

#include <agrum/BN/learning/paramUtils/DAG2BNLearner.h>
#include <agrum/BN/learning/paramUtils/paramEstimatorML.h>

#include <agrum/tools/core/approximations/IApproximationSchemeConfiguration.h>
#include <agrum/tools/core/approximations/approximationSchemeListener.h>

#include <agrum/BN/learning/K2.h>
#include <agrum/BN/learning/Miic.h>
#include <agrum/BN/learning/greedyHillClimbing.h>
#include <agrum/BN/learning/localSearchWithTabuList.h>

#include <agrum/tools/core/signal/signaler.h>

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
    class genericBNLearner: public gum::IApproximationSchemeConfiguration {
      // private:
      public:
      /// an enumeration enabling to select easily the score we wish to use
      enum class ScoreType
      {
        AIC,
        BD,
        BDeu,
        BIC,
        K2,
        LOG2LIKELIHOOD
      };

      /// an enumeration to select the type of parameter estimation we shall
      /// apply
      enum class ParamEstimatorType
      { ML };

      /// an enumeration to select the apriori
      enum class AprioriType
      {
        NO_APRIORI,
        SMOOTHING,
        DIRICHLET_FROM_DATABASE,
        BDEU
      };

      /// an enumeration to select easily the learning algorithm to use
      enum class AlgoType
      {
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
        DatabaseTable<>  _database_;

        /// the parser used for reading the database
        DBRowGeneratorParser<>*  _parser_{nullptr};

        /// the domain sizes of the variables (useful to speed-up computations)
        std::vector< std::size_t >  _domain_sizes_;

        /// a bijection assigning to each variable name its NodeId
        Bijection< NodeId, std::size_t >  _nodeId2cols_;

/// the max number of threads authorized
#if defined(_OPENMP) && !defined(GUM_DEBUG_MODE)
        Size  _max_threads_number_{getMaxNumberOfThreads()};
#else
        Size  _max_threads_number_{1};
#endif /* GUM_DEBUG_MODE */

        /// the minimal number of rows to parse (on average) by thread
        Size  _min_nb_rows_per_thread_{100};

        private:
        // returns the set of variables as a BN. This is convenient for
        // the constructors of apriori Databases
        template < typename GUM_SCALAR >
        BayesNet< GUM_SCALAR >  _BNVars_() const;
      };

      /// sets the apriori weight
      void  _setAprioriWeight_(double weight);

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
      Size                              domainSize(NodeId var) const;
      Size                              domainSize(const std::string& var) const;

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
      std::pair< double, double > chi2(const std::string&                name1,
                                       const std::string&                name2,
                                       const std::vector< std::string >& knowing
                                       = {});

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
      std::pair< double, double > G2(const std::string&                name1,
                                     const std::string&                name2,
                                     const std::vector< std::string >& knowing
                                     = {});

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
       * Return the pseudoconts ofNodeIds vars in the base in a raw array
       * @param vars a vector of
       * @return a a std::vector<double> containing the contingency table
       */
      std::vector< double > rawPseudoCount(const std::vector< NodeId >& vars);

      /**
       * Return the pseudoconts of vars in the base in a raw array
       * @param vars a vector of name
       * @return a std::vector<double> containing the contingency table
       */
      std::vector< double > rawPseudoCount(const std::vector< std::string >& vars);
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
      ScoreType  _score_type_{ScoreType::BDeu};

      /// the score used
      Score<>*  _score_{nullptr};

      /// the type of the parameter estimator
      ParamEstimatorType  _param_estimator_type_{ParamEstimatorType::ML};

      /// epsilon for EM. if espilon=0.0 : no EM
      double  _EMepsilon_{0.0};

      /// the selected correction for 3off2 and miic
      CorrectedMutualInformation<>*  _mutual_info_{nullptr};

      /// the a priori selected for the score and parameters
      AprioriType  _apriori_type_{AprioriType::NO_APRIORI};

      /// the apriori used
      Apriori<>*  _apriori_{nullptr};

      AprioriNoApriori<>*  _no_apriori_{nullptr};

      /// the weight of the apriori
      double  _apriori_weight_{1.0f};

      /// the constraint for 2TBNs
      StructuralConstraintSliceOrder  _constraint_SliceOrder_;

      /// the constraint for indegrees
      StructuralConstraintIndegree  _constraint_Indegree_;

      /// the constraint for tabu lists
      StructuralConstraintTabuList  _constraint_TabuList_;

      /// the constraint on forbidden arcs
      StructuralConstraintForbiddenArcs  _constraint_ForbiddenArcs_;

      /// the constraint on possible Edges
      StructuralConstraintPossibleEdges  _constraint_PossibleEdges_;

      /// the constraint on forbidden arcs
      StructuralConstraintMandatoryArcs  _constraint_MandatoryArcs_;

      /// the selected learning algorithm
      AlgoType  _selected_algo_{AlgoType::GREEDY_HILL_CLIMBING};

      /// the K2 algorithm
      K2  _K2_;

      /// the 3off2 algorithm
      Miic  _miic_3off2_;

      /// the penalty used in 3off2
      typename CorrectedMutualInformation<>::KModeTypes  _kmode_3off2_{
         CorrectedMutualInformation<>::KModeTypes::MDL};

      /// the parametric EM
      DAG2BNLearner<>  _Dag2BN_;

      /// the greedy hill climbing algorithm
      GreedyHillClimbing  _greedy_hill_climbing_;

      /// the local search with tabu list algorithm
      LocalSearchWithTabuList  _local_search_with_tabu_list_;

      /// the database to be used by the scores and parameter estimators
      Database  _score_database_;

      /// the set of rows' ranges within the database in which learning is done
      std::vector< std::pair< std::size_t, std::size_t > >  _ranges_;

      /// the database used by the Dirichlet a priori
      Database*  _apriori_database_{nullptr};

      /// the filename for the Dirichlet a priori, if any
      std::string  _apriori_dbname_;

      /// an initial DAG given to learners
      DAG  _initial_dag_;

      // the current algorithm as an approximationScheme
      const ApproximationScheme*  _current_algorithm_{nullptr};

      /// reads a file and returns a databaseVectInRam
      static DatabaseTable<>
          _readFile_(const std::string&                filename,
                    const std::vector< std::string >& missing_symbols);

      /// checks whether the extension of a CSV filename is correct
      static void  _checkFileName_(const std::string& filename);

      /// create the apriori used for learning
      void  _createApriori_();

      /// create the score used for learning
      void  _createScore_();

      /// create the parameter estimator used for learning
      ParamEstimator<>*  _createParamEstimator_(DBRowGeneratorParser<>& parser,
                                               bool take_into_account_score
                                               = true);

      /// returns the DAG learnt
      DAG  _learnDAG_();

      /// prepares the initial graph for 3off2 or miic
      MixedGraph  _prepare_miic_3off2_();

      /// returns the type (as a string) of a given apriori
      const std::string&  _getAprioriType_() const;

      /// create the Corrected Mutual Information instance for Miic/3off2
      void  _createCorrectedMutualInformation_();


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
         _current_algorithm_ = approximationScheme;
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
         _K2_.approximationScheme().setEpsilon(eps);
         _greedy_hill_climbing_.setEpsilon(eps);
         _local_search_with_tabu_list_.setEpsilon(eps);
         _Dag2BN_.setEpsilon(eps);
      };

      /// Get the value of epsilon
      double epsilon() const {
        if ( _current_algorithm_ != nullptr)
          return  _current_algorithm_->epsilon();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning")
      }

      /// Disable stopping criterion on epsilon
      void disableEpsilon() {
         _K2_.approximationScheme().disableEpsilon();
         _greedy_hill_climbing_.disableEpsilon();
         _local_search_with_tabu_list_.disableEpsilon();
         _Dag2BN_.disableEpsilon();
      };

      /// Enable stopping criterion on epsilon
      void enableEpsilon() {
         _K2_.approximationScheme().enableEpsilon();
         _greedy_hill_climbing_.enableEpsilon();
         _local_search_with_tabu_list_.enableEpsilon();
         _Dag2BN_.enableEpsilon();
      };

      /// @return true if stopping criterion on epsilon is enabled, false
      /// otherwise
      bool isEnabledEpsilon() const {
        if ( _current_algorithm_ != nullptr)
          return  _current_algorithm_->isEnabledEpsilon();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning")
      }
      /// @}

      /// Given that we approximate f(t), stopping criterion on
      /// d/dt(|f(t+1)-f(t)|)
      /// If the criterion was disabled it will be enabled
      /// @{
      /// @throw OutOfLowerBound if rate<0
      void setMinEpsilonRate(double rate) {
         _K2_.approximationScheme().setMinEpsilonRate(rate);
         _greedy_hill_climbing_.setMinEpsilonRate(rate);
         _local_search_with_tabu_list_.setMinEpsilonRate(rate);
         _Dag2BN_.setMinEpsilonRate(rate);
      };

      /// Get the value of the minimal epsilon rate
      double minEpsilonRate() const {
        if ( _current_algorithm_ != nullptr)
          return  _current_algorithm_->minEpsilonRate();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning")
      }

      /// Disable stopping criterion on epsilon rate
      void disableMinEpsilonRate() {
         _K2_.approximationScheme().disableMinEpsilonRate();
         _greedy_hill_climbing_.disableMinEpsilonRate();
         _local_search_with_tabu_list_.disableMinEpsilonRate();
         _Dag2BN_.disableMinEpsilonRate();
      };
      /// Enable stopping criterion on epsilon rate
      void enableMinEpsilonRate() {
         _K2_.approximationScheme().enableMinEpsilonRate();
         _greedy_hill_climbing_.enableMinEpsilonRate();
         _local_search_with_tabu_list_.enableMinEpsilonRate();
         _Dag2BN_.enableMinEpsilonRate();
      };
      /// @return true if stopping criterion on epsilon rate is enabled, false
      /// otherwise
      bool isEnabledMinEpsilonRate() const {
        if ( _current_algorithm_ != nullptr)
          return  _current_algorithm_->isEnabledMinEpsilonRate();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning")
      }
      /// @}

      /// stopping criterion on number of iterations
      /// @{
      /// If the criterion was disabled it will be enabled
      /// @param max The maximum number of iterations
      /// @throw OutOfLowerBound if max<=1
      void setMaxIter(Size max) {
         _K2_.approximationScheme().setMaxIter(max);
         _greedy_hill_climbing_.setMaxIter(max);
         _local_search_with_tabu_list_.setMaxIter(max);
         _Dag2BN_.setMaxIter(max);
      };

      /// @return the criterion on number of iterations
      Size maxIter() const {
        if ( _current_algorithm_ != nullptr)
          return  _current_algorithm_->maxIter();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning")
      }

      /// Disable stopping criterion on max iterations
      void disableMaxIter() {
         _K2_.approximationScheme().disableMaxIter();
         _greedy_hill_climbing_.disableMaxIter();
         _local_search_with_tabu_list_.disableMaxIter();
         _Dag2BN_.disableMaxIter();
      };
      /// Enable stopping criterion on max iterations
      void enableMaxIter() {
         _K2_.approximationScheme().enableMaxIter();
         _greedy_hill_climbing_.enableMaxIter();
         _local_search_with_tabu_list_.enableMaxIter();
         _Dag2BN_.enableMaxIter();
      };
      /// @return true if stopping criterion on max iterations is enabled, false
      /// otherwise
      bool isEnabledMaxIter() const {
        if ( _current_algorithm_ != nullptr)
          return  _current_algorithm_->isEnabledMaxIter();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning")
      }
      /// @}

      /// stopping criterion on timeout
      /// If the criterion was disabled it will be enabled
      /// @{
      /// @throw OutOfLowerBound if timeout<=0.0
      /** timeout is time in second (double).
       */
      void setMaxTime(double timeout) {
         _K2_.approximationScheme().setMaxTime(timeout);
         _greedy_hill_climbing_.setMaxTime(timeout);
         _local_search_with_tabu_list_.setMaxTime(timeout);
         _Dag2BN_.setMaxTime(timeout);
      }

      /// returns the timeout (in seconds)
      double maxTime() const {
        if ( _current_algorithm_ != nullptr)
          return  _current_algorithm_->maxTime();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning")
      }

      /// get the current running time in second (double)
      double currentTime() const {
        if ( _current_algorithm_ != nullptr)
          return  _current_algorithm_->currentTime();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning")
      }

      /// Disable stopping criterion on timeout
      void disableMaxTime() {
         _K2_.approximationScheme().disableMaxTime();
         _greedy_hill_climbing_.disableMaxTime();
         _local_search_with_tabu_list_.disableMaxTime();
         _Dag2BN_.disableMaxTime();
      };
      void enableMaxTime() {
         _K2_.approximationScheme().enableMaxTime();
         _greedy_hill_climbing_.enableMaxTime();
         _local_search_with_tabu_list_.enableMaxTime();
         _Dag2BN_.enableMaxTime();
      };
      /// @return true if stopping criterion on timeout is enabled, false
      /// otherwise
      bool isEnabledMaxTime() const {
        if ( _current_algorithm_ != nullptr)
          return  _current_algorithm_->isEnabledMaxTime();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning")
      }
      /// @}

      /// how many samples between 2 stopping isEnableds
      /// @{
      /// @throw OutOfLowerBound if p<1
      void setPeriodSize(Size p) {
         _K2_.approximationScheme().setPeriodSize(p);
         _greedy_hill_climbing_.setPeriodSize(p);
         _local_search_with_tabu_list_.setPeriodSize(p);
         _Dag2BN_.setPeriodSize(p);
      };

      Size periodSize() const {
        if ( _current_algorithm_ != nullptr)
          return  _current_algorithm_->periodSize();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning")
      }
      /// @}

      /// verbosity
      /// @{
      void setVerbosity(bool v) {
         _K2_.approximationScheme().setVerbosity(v);
         _greedy_hill_climbing_.setVerbosity(v);
         _local_search_with_tabu_list_.setVerbosity(v);
         _Dag2BN_.setVerbosity(v);
      };

      bool verbosity() const {
        if ( _current_algorithm_ != nullptr)
          return  _current_algorithm_->verbosity();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning")
      }
      /// @}

      /// history
      /// @{

      ApproximationSchemeSTATE stateApproximationScheme() const {
        if ( _current_algorithm_ != nullptr)
          return  _current_algorithm_->stateApproximationScheme();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning")
      }

      /// @throw OperationNotAllowed if scheme not performed
      Size nbrIterations() const {
        if ( _current_algorithm_ != nullptr)
          return  _current_algorithm_->nbrIterations();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning")
      }

      /// @throw OperationNotAllowed if scheme not performed or verbosity=false
      const std::vector< double >& history() const {
        if ( _current_algorithm_ != nullptr)
          return  _current_algorithm_->history();
        else
          GUM_ERROR(FatalError, "No chosen algorithm for learning")
      }
      /// @}
    };

  } /* namespace learning */

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/BN/learning/BNLearnUtils/genericBNLearner_inl.h>
#endif /* GUM_NO_INLINE */

#include <agrum/BN/learning/BNLearnUtils/genericBNLearner_tpl.h>

#endif /* GUM_LEARNING_GENERIC_BN_LEARNER_H */
