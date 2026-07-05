/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief A class for generic framework of learning algorithms that can easily
 * be used.
 *
 * The pack currently contains K2, GreedyHillClimbing, miic and
 * LocalSearchWithTabuList
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_GENERIC_BN_LEARNER_H
#define GUM_LEARNING_GENERIC_BN_LEARNER_H

#include <memory>
#include <sstream>

#include <agrum/agrum.h>

#include <agrum/base/database/DBInitializerFromCSV.h>
#include <agrum/base/database/DBRowGenerator4CompleteRows.h>
#include <agrum/base/database/DBRowGeneratorEM.h>
#include <agrum/base/stattests/indepTestChi2.h>
#include <agrum/base/stattests/indepTestG2.h>
#include <agrum/BN/algorithms/essentialGraph.h>
#include <agrum/BN/learning/constraints/structuralConstraintDAG.h>
#include <agrum/BN/learning/constraints/structuralConstraintForbiddenArcs.h>
#include <agrum/BN/learning/constraints/structuralConstraintIndegree.h>
#include <agrum/BN/learning/constraints/structuralConstraintMandatoryArcs.h>
#include <agrum/BN/learning/constraints/structuralConstraintNoChildrenNodes.h>
#include <agrum/BN/learning/constraints/structuralConstraintNoParentNodes.h>
#include <agrum/BN/learning/constraints/structuralConstraintPossibleEdges.h>
#include <agrum/BN/learning/constraints/structuralConstraintSliceOrder.h>
#include <agrum/BN/learning/constraints/structuralConstraintTabuList.h>
#include <agrum/BN/learning/constraints/structuralConstraintTotalOrder.h>
#include <agrum/BN/learning/FCI.h>
#include <agrum/BN/learning/greedyThickThinning.h>
#include <agrum/BN/learning/K2.h>
#include <agrum/BN/learning/localSearchWithTabuList.h>
#include <agrum/BN/learning/paramUtils/DAG2BNLearner.h>
#include <agrum/BN/learning/paramUtils/paramEstimatorML.h>
#include <agrum/BN/learning/PC.h>
#include <agrum/BN/learning/priors/DirichletPriorFromDatabase.h>
#include <agrum/BN/learning/scores/scoreAIC.h>
#include <agrum/BN/learning/scores/scoreBD.h>
#include <agrum/BN/learning/scores/scoreBDeu.h>
#include <agrum/BN/learning/scores/scorefNML.h>
#include <agrum/BN/learning/scores/scoreK2.h>
#include <agrum/BN/learning/SimpleMiic.h>
#include <agrum/BN/learning/structureUtils/graphChangesGenerator4DiGraph.h>
#include <agrum/BN/learning/structureUtils/graphChangesGenerator4K2.h>
#include <agrum/BN/learning/structureUtils/graphChangesSelector4DiGraph.h>

namespace gum::learning {
  class BNLearnerListener;

  /** @class IBNLearner
   * @brief A pack of learning algorithms that can easily be used
   *
   * The pack currently contains K2, GreedyHillClimbing and
   * LocalSearchWithTabuList also miic
   * @ingroup learning_group
   */
  class IBNLearner: public IApproximationSchemeConfiguration, public ThreadNumberManager {
    public:
    /// an enumeration enabling to select easily the score we wish to use
    enum class ScoreType { AIC, BD, BDeu, BIC, fNML, K2, LOG2LIKELIHOOD, MDL };

    /// an enumeration to select the type of parameter estimation we shall
    /// apply
    enum class ParamEstimatorType { ML };

    /// an enumeration to select the prior
    enum class BNLearnerPriorType {
      NO_prior,
      SMOOTHING,
      DIRICHLET_FROM_DATABASE,
      DIRICHLET_FROM_BAYESNET,
      BDEU
    };

    /// an enumeration to select easily the learning algorithm to use
    enum class AlgoType {
      K2,
      GREEDY_HILL_CLIMBING,
      LOCAL_SEARCH_WITH_TABU_LIST,
      MIIC,
      PC,
      FCI,
      EXTENDED_GREEDY_HILL_CLIMBING,
      GREEDY_THICK_THINNING
    };

    /// the default noise amount added to CPTs during EM's initialization (see method useEM())
    static constexpr double default_EM_noise{0.1};

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
       * correspond to missing data
       * @param induceTypes By default, all the values in the dataset are
       * interpreted as "labels", i.e., as categorical values. But if some
       * columns of the dataset have only numerical values, it would certainly
       * be better totag them as corresponding to integer, range or continuous
       * variables. By setting induceTypes to true, this is precisely what the
       * BNLearner will do.
       */
      explicit Database(std::string_view                  file,
                        const std::vector< std::string >& missing_symbols,
                        const bool                        induceTypes = false);

      /// default constructor
      /** @param db an already initialized database table that is used to
       * fill the Database
       */
      explicit Database(const DatabaseTable& db);

      /// constructor for the priors
      /** We must ensure that the variables of the Database are identical to
       * those of the score database (else the counts used by the
       * scores might be erroneous). However, we allow the variables to be
       * ordered differently in the two databases: variables with the same
       * name in both databases are supposed to be the same.
       * @param file the name of the CSV file containing the data
       * @param score_database the main database used for the learning
       * @param missing_symbols the set of symbols in the CSV file that
       * correspond to missing data
       */
      Database(std::string_view                  filename,
               const Database&                   score_database,
               const std::vector< std::string >& missing_symbols);

      /// constructor with a BN providing the variables of interest
      /** @param file the name of the CSV file containing the data
       * @param bn a Bayesian network indicating which variables of the CSV
       * file are used for learning
       * @param missing_symbols the set of symbols in the CSV file that
       * correspond to missing data
       */
      template < GUM_Numeric GUM_SCALAR >
      Database(std::string_view                   filename,
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
      DBRowGeneratorParser& parser();

      /// returns the domain sizes of the variables
      const std::vector< std::size_t >& domainSizes() const;

      /// returns the names of the variables in the database
      const std::vector< std::string >& names() const;

      /// returns the node id corresponding to a variable name
      NodeId idFromName(std::string_view var_name) const;

      /// returns the variable name corresponding to a given node id
      const std::string& nameFromId(NodeId id) const;

      /// returns the internal database table
      const DatabaseTable& databaseTable() const;

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
      DatabaseTable _database_;

      /// the parser used for reading the database
      DBRowGeneratorParser* _parser_{nullptr};

      /// the domain sizes of the variables (useful to speed-up computations)
      std::vector< std::size_t > _domain_sizes_;

      /// a bijection assigning to each variable name its NodeId
      Bijection< NodeId, std::size_t > _nodeId2cols_;

      /// the max number of threads authorized
      Size _max_threads_number_{gum::getNumberOfThreads()};

      /// the minimal number of rows to parse (on average) by thread
      Size _min_nb_rows_per_thread_{100};

      private:
      // returns the set of variables as a BN. This is convenient for
      // the constructors of prior Databases
      template < GUM_Numeric GUM_SCALAR >
      BayesNet< GUM_SCALAR > _BNVars_() const;
    };

    // ##########################################################################
    /// @name Constructors / Destructors
    // ##########################################################################
    /// @{

    /**
     * read the database file for the score / parameter estimation and var
     * names
     * @param filename the name of a CSV file containing the dataset
     * @param missing_symbols the set of symbols in the CSV that should
     * be interpreted as missing values
     * @param induceTypes when some  columns of the dataset have only numerical
     * values, it is certainly be better to tag them as corresponding to integer,
     * range or continuous variables. By setting induceTypes to true (default), this is
     * precisely what the BNLearner will do. If inducedTypes is false, all the values in
     * the dataset are interpreted as "labels", i.e., as categorical values.
     */
    IBNLearner(std::string_view                  filename,
               const std::vector< std::string >& missingSymbols,
               bool                              induceTypes = true);

    explicit IBNLearner(const DatabaseTable& db);

    /**
     * read the database file for the score / parameter estimation and var
     * names
     * @param filename The file to learn from.
     * @param src indicate for some nodes (not necessarily all the
     * nodes of the BN) which modalities they should have and in which order
     * these modalities should be stored into the nodes. For instance, if
     * modalities = { 1 -> {True, False, Big} }, then the node of id 1 in the
     * BN will have 3 modalities, the first one being True, the second one
     * being False, and the third bein Big.
     * The modalities specified by the user will be considered
     * as being exactly those of the variables of the BN (as a consequence,
     * if we find other values in the database, an exception will be raised
     * during learning).
     * @param missing_symbols the set of symbols in the CSV that should
     * be interpreted as missing values
     */
    template < GUM_Numeric GUM_SCALAR >
    IBNLearner(std::string_view                   filename,
               const gum::BayesNet< GUM_SCALAR >& src,
               const std::vector< std::string >&  missing_symbols);

    /// copy constructor
    IBNLearner(const IBNLearner&);

    /// move constructor
    IBNLearner(IBNLearner&&);

    /// destructor
    virtual ~IBNLearner();

    /// @}

    // ##########################################################################
    /// @name Operators
    // ##########################################################################
    /// @{

    /// copy operator
    IBNLearner& operator=(const IBNLearner&);

    /// move operator
    IBNLearner& operator=(IBNLearner&&);

    /// @}

    // ##########################################################################
    /// @name Accessors / Modifiers
    // ##########################################################################
    /// @{

    /// learn a structure from a file (must have read the db before)
    DAG learnDAG();

    /// learn a partial structure from a file (must have read the db before and
    /// must have selected miic)
    PDAG learnPDAG();

    /// learn a PAG — only valid when useFCI() has been called
    /// @throws OperationNotAllowed when FCI is not the selected algorithm
    PAG learnPAG();

    /// sets an initial DAG structure
    void setInitialDAG(const DAG&);

    /// returns the initial DAG structure
    DAG initialDAG();

    /// returns the names of the variables in the database
    const std::vector< std::string >& names() const;

    /// returns the domain sizes of the variables in the database
    const std::vector< std::size_t >& domainSizes() const;
    Size                              domainSize(NodeId var) const;
    Size                              domainSize(std::string_view var) const;

    /// returns the node id corresponding to a variable name
    /**
     * @throw MissingVariableInDatabase if a variable of the BN is not found
     * in the database.
     */
    NodeId idFromName(std::string_view var_name) const;

    /// returns the database used by the BNLearner
    const DatabaseTable& database() const;

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
    void useDatabaseRanges(const std::vector< std::pair< std::size_t, std::size_t > >& new_ranges);

    /// reset the ranges to the one range corresponding to the whole database
    void clearDatabaseRanges();

    /// returns the current database rows' ranges used for learning
    /** @return The method returns a vector of pairs [Xi,Yi) of indices of
     * rows in the database. The learning is performed on these set of rows.
     * @warning an empty set of ranges means the whole database. */
    const std::vector< std::pair< std::size_t, std::size_t > >& databaseRanges() const;

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
    std::pair< std::size_t, std::size_t > useCrossValidationFold(const std::size_t learning_fold,
                                                                 const std::size_t k_fold);


    /**
     * Return the <statistic,pvalue> pair for chi2 test in the database
     * @param id1 first variable
     * @param id2 second variable
     * @param knowing list of observed variables
     * @return a std::pair<double,double>
     */
    std::pair< double, double >
        chi2(NodeId id1, NodeId id2, const std::vector< NodeId >& knowing = {});
    /**
     * Return the <statistic,pvalue> pair for the BNLearner
     * @param id1 first variable
     * @param id2 second variable
     * @param knowing list of observed variables
     * @return a std::pair<double,double>
     */
    std::pair< double, double > chi2(std::string_view                  name1,
                                     std::string_view                  name2,
                                     const std::vector< std::string >& knowing = {});

    /**
     * Return the <statistic,pvalue> pair for for G2 test in the database
     * @param id1 first variable
     * @param id2 second variable
     * @param knowing list of observed variables
     * @return a std::pair<double,double>
     */
    std::pair< double, double >
        G2(NodeId id1, NodeId id2, const std::vector< NodeId >& knowing = {});
    /**
     * Return the <statistic,pvalue> pair for for G2 test in the database
     * @param id1 first variable
     * @param id2 second variable
     * @param knowing list of observed variables
     * @return a std::pair<double,double>
     */
    std::pair< double, double > G2(std::string_view                  name1,
                                   std::string_view                  name2,
                                   const std::vector< std::string >& knowing = {});

    /**
     * Return the loglikelihood of vars in the base, conditioned by knowing for
     * the BNLearner
     * @param vars a vector of NodeIds
     * @param knowing an optional vector of conditioning NodeIds
     * @return a double
     */
    double logLikelihood(const std::vector< NodeId >& vars,
                         const std::vector< NodeId >& knowing = {});

    /**
     * Return the loglikelihood of vars in the base, conditioned by knowing for
     * the BNLearner
     * @param vars a vector of name of rows
     * @param knowing an optional vector of conditioning rows
     * @return a double
     */
    double logLikelihood(const std::vector< std::string >& vars,
                         const std::vector< std::string >& knowing = {});

    /**
     * Return the mutual information of id1 and id2 in the base, conditioned by knowing for
     * the BNLearner
     *
     * @warning This function returns the bare mutual information (without prior or correction)
     *
     * @param id1 nodeId
     * @param id2 nodeId
     * @param knowing an optional vector of conditioning NodeIds
     * @return a double
     */
    double mutualInformation(NodeId id1, NodeId id2, const std::vector< NodeId >& knowing = {});

    /**
     * Return the mutual information of var1 and var2 in the base, conditioned by knowing for
     * the BNLearner
     *
     * @warning This function returns the bare mutual information (without prior or correction)
     *
     * @param var1 name of a row
     * @param var2 name of a row
     * @param knowing an optional vector of conditioning rows
     * @return a double
     */
    double mutualInformation(std::string_view                  var1,
                             std::string_view                  var2,
                             const std::vector< std::string >& knowing = {});


    /**
     * Return the mutual information of id1 and id2 in the base, conditioned by knowing for
     * the BNLearner
     *
     * @warning This function returns the mutual information corrected using defined correction
     * (useNML, useMDL) and prior
     *
     * @param id1 nodeId
     * @param id2 nodeId
     * @param knowing an optional vector of conditioning NodeIds
     * @return a double
     */
    double correctedMutualInformation(NodeId                       id1,
                                      NodeId                       id2,
                                      const std::vector< NodeId >& knowing = {});

    /**
     * Return the mutual information of var1 and var2 in the base, conditioned by knowing for
     * the BNLearner
     *
     * @warning This function returns the mutual information corrected using defined correction
     * (useNML, useMDL) and prior
     *
     * @warning This function returns the bare mutual information (without prior or correction)
     *
     * @param var1 name of a row
     * @param var2 name of a row
     * @param knowing an optional vector of conditioning rows
     * @return a double
     */
    double correctedMutualInformation(std::string_view                  var1,
                                      std::string_view                  var2,
                                      const std::vector< std::string >& knowing = {});

    /**
     * Return the value of the score currently in use by the BNLearner of a
     * variable given a set of other variables
     * @param var the NodeId of the LHS variable
     * @param knowing an optional vector of conditioning variables, specified
     * by their NodeIds
     * @return a double corresponding to the value of the score
     */
    double score(NodeId vars, const std::vector< NodeId >& knowing = {});

    /**
     * Return the value of the score currently in use by the BNLearner of a
     * variable given a set of other variables
     * The score used is the one currently selected in the BNLearner
     * @param var the name of the variable
     * @param knowing an optional vector of conditioning variables, specified
     * by their names
     * @return a double corresponding to the value of the score
     */
    double score(std::string_view vars, const std::vector< std::string >& knowing = {});

    /**
     * Return the pseudo-counts of NodeIds vars in the base in a raw array
     * @param vars a vector of
     * @return a std::vector<double> containing the contingency table
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

    /**
     * use The EM algorithm to learn parameters
     * @param epsilon sets the approximation stopping criterion: EM stops
     * whenever the absolute value of the relative difference between two
     * consecutive log-likelihoods drops below epsilon. Note that epsilon=0
     * is considered as a directive to not use EM. However, if you wish to
     * forbid the use of EM, prefer executing Method forbidEM() rather than
     * useEM(0) as it is more unequivocal.
     * @param noise When EM starts, it initializes all the CPTs of the Bayes
     * net. EM adds a noise to these CPTs by mixing their values with some
     * random noise. The formula used is, up to some normalizing constant:
     * new_cpt = (1-noise) * cpt + noise * random_cpt(). Of course, noise must
     * belong to interval [0,1].
     * @warning if epsilon=0, EM is not used
     * @throws OutOfBounds is raised if epsilon is strictly negative or if
     * noise does not belong to interval [0,1].
     */
    void useEM(const double epsilon, const double noise = default_EM_noise);

    /**
     * @brief use The EM algorithm to learn parameters with the rate stopping criterion
     * @param epsilon epsilon sets the approximation stopping criterion: EM stops
     * whenever the absolute value of the relative difference between two
     * consecutive log-likelihoods drops below epsilon. Note that, for using EM,
     * epsilon should be strictly positive.
     * @param noise When EM starts, it initializes all the CPTs of the Bayes
     * net. EM adds a noise to these CPTs by mixing their values with some
     * random noise. The formula used is, up to some normalizing constant:
     * new_cpt = (1-noise) * cpt + noise * random_cpt(). Of course, noise must
     * belong to interval [0,1].
     * @throws OutOfBounds is raised if epsilon is not strictly positive or if
     * noise does not belong to interval [0,1].
     */
    void useEMWithRateCriterion(const double epsilon, const double noise = default_EM_noise);

    /**
     * @brief use The EM algorithm to learn parameters with the diff stopping criterion
     * @param epsilon epsilon sets the approximation stopping criterion: EM stops
     * whenever the difference between two consecutive log-likelihoods drops below
     * epsilon. Note that, for using EM, epsilon should be strictly positive.
     * @param noise When EM starts, it initializes all the CPTs of the Bayes
     * net. EM adds a noise to these CPTs by mixing their values with some
     * random noise. The formula used is, up to some normalizing constant:
     * new_cpt = (1-noise) * cpt + noise * random_cpt(). Of course, noise must
     * belong to interval [0,1].
     * @throws OutOfBounds is raised if epsilon is not strictly positive or if
     * noise does not belong to interval [0,1].
     */
    void useEMWithDiffCriterion(const double epsilon, const double noise = default_EM_noise);

    /// prevent using the EM algorithm for parameter learning
    void forbidEM();

    /// indicates whether we use EM for parameter learning
    bool isUsingEM() const;

    /**
     * @brief returns the EM parameter learning approximation scheme if EM is enabled
     * @return returns the EM parameter learning approximation scheme if EM is enabled,
     * else if raises a NotFound exception. The scheme can then be subsequently used
     * to fine-tune the EM algorithm, e.g., by setting its max time or max number of
     * iterations.
     * @throw NotFound is raised if EM() is called while the EM algorithm has not been
     * enabled by useEM(), useEMWithDiffCriterion() or useEMWithRateCriterion().
     */
    EMApproximationScheme& EM();

    /// returns the state of the last EM algorithm executed
    ApproximationSchemeSTATE EMState() const;

    /// returns the state of the EM algorithm
    std::string EMStateMessage() const;

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

    /// indicate that we wish to use a fNML score
    void useScorefNML();

    /// indicate that we wish to use a K2 score
    void useScoreK2();

    /// indicate that we wish to use a Log2Likelihood score
    void useScoreLog2Likelihood();

    /// indicate that we wish to use a MDL score
    void useScoreMDL();

    /// @}

    // ##########################################################################
    /// @name a prior selection / parameterization
    // ##########################################################################
    /// @{

    /// use no prior
    void useNoPrior();

    /// use the BDeu prior
    /** The BDeu prior adds weight to all the cells of the counts
     * tables. In other words, it adds weight rows in the database with
     * equally probable values. */
    void useBDeuPrior(double weight = 1.0);

    /// use the prior smoothing
    /** @param weight pass in argument a weight if you wish to assign a weight
     * to the smoothing, else the current weight of the IBNLearner will
     * be used. */
    void useSmoothingPrior(double weight = 1);

    /// use the Dirichlet prior from a database
    void useDirichletPrior(std::string_view filename, double weight = 1);

    /// checks whether the current score and prior are compatible
    /** @returns a non empty string if the prior is somehow compatible with the
     * score.*/
    std::string checkScorePriorCompatibility() const;
    /// @}

    // ##########################################################################
    /// @name Learning algorithm selection
    // ##########################################################################
    /// @{

    /// indicate that we wish to use a greedy hill climbing algorithm
    void useGreedyHillClimbing();

    /// indicate that we wish to use the extended greedy hill climbing algorithm
    /**
     * The extended greedy hill climbing adds to the classical operations
     * (arc addition, arc reversal, arc deletion) two additional operations:
     * arc triangle deletion1 and arc triangle deletion2. The former modifies
     * a structure X1->X2->X3 + X1->X3 into a v-structure centered on X1, i.e.,
     * X2->X1<-X3. Arc triangle deletion2 modifies structure X1->X2->X3 + X1->X3
     * into a v-structure centered on X2, i.e., X1->X2<-X3.
     *
     * In addition, the extended greedy hill climbing allows the user to select
     * which operations among arc additions, arc reversals, arc deletions, arc
     * triangle deletions, are allowed.
     */
    void useExtendedGreedyHillClimbing();

    /// indicate that we wish to use a local search with tabu list
    /** @param tabu_size indicate the size of the tabu list
     * @param nb_decrease indicate the max number of changes decreasing the
     * score consecutively that we allow to apply */
    void useLocalSearchWithTabuList(Size tabu_size = 100, Size nb_decrease = 2);

    /// indicate that we wish to use K2
    void useK2(const Sequence< NodeId >& order);

    /// indicate that we wish to use K2
    void useK2(const std::vector< NodeId >& order);

    /// indicate that we wish to use MIIC
    void useMIIC();

    /// indicate that we wish to use PC (Chi2 test by default)
    void usePC();

    /// indicate that we wish to use FCI (Chi2 test by default)
    void useFCI();

    /// indicate that we wish to use greedy thick-thinning
    void useGreedyThickThinning();

    /// enable or disable arc reversals in the thin phase of greedy thick-thinning
    void setGreedyThickThinningReversals(bool allow);

    /// returns whether arc reversals are allowed in the thin phase of greedy thick-thinning
    bool greedyThickThinningReversals() const;

    /// indicate if the selected algorithm is constraint-based
    bool isConstraintBased() const;

    /// indicate if the selected algorithm is score-based
    bool isScoreBased() const;

    // ##########################################################################
    /// @name allow/forbid the graph operations used during learning
    // ##########################################################################
    /// @{
    /** @brief allow (true)/forbid (false) to add arcs during learning.
     *
     * This affects only Extended Greedy Hill Climbing and Local Search with
     * Tabu List. Greedy Hill Climbing and K2 always allow arc additions.
     */
    void allowArcAdditions(bool allow = true);

    /** @brief allow (true)/forbid (false) to delete arcs during learning.
     *
     * This affects only Extended Greedy Hill Climbing and Local Search with
     * Tabu List. Greedy Hill Climbing always allows arc deletions while
     * K2 always forbid them. If you wish a K2-like algorithm that enables
     * arc deletions, use and extended greedy hill climbing combined with
     * a TotalOrder constraint.
     */
    void allowArcDeletions(bool allow = true);

    /** @brief allow (true)/forbid (false) to reverse arcs during learning.
     *
     * This affects only Extended Greedy Hill Climbing and Local Search with
     * Tabu List. Greedy Hill Climbing always allows arc reversals while
     * K2 always forbid them (due to its total ordering constraint).
     */
    void allowArcReversals(bool allow = true);

    /** @brief allow (true)/forbid (false) to delete arc triangles during learning.
     *
     * This affects only Extended Greedy Hill Climbing and Local Search with
     * Tabu List. Greedy Hill Climbing and K2 always forbid arc triangle deletions.
     */
    void allowArcTriangleDeletions(bool allow = true);

    /// @}

    // ##########################################################################
    /// @name MIIC parameterization and specific results
    // ##########################################################################
    /// @{
    /// indicate that we wish to use the NML correction for and MIIC
    /// @throws OperationNotAllowed when MIIC is not the selected algorithm
    void useNMLCorrection();
    /// indicate that we wish to use the MDL correction for  MIIC
    /// @throws OperationNotAllowed when MIIC is not the selected algorithm
    void useMDLCorrection();
    /// indicate that we wish to use the NoCorr correction for MIIC
    /// @throws OperationNotAllowed when MIIC is not the selected algorithm
    void useNoCorrection();

    /// get the list of arcs hiding latent variables
    /// @throws OperationNotAllowed when MIIC is not the selected algorithm
    std::vector< Arc > latentVariables() const;

    /// @}

    // ##########################################################################
    /// @name PC parameterization
    // ##########################################################################
    /// @{

    /// indicate that we wish to use Chi2 independence test for PC
    /// @throws OperationNotAllowed when PC is not the selected algorithm
    void useChi2Test();

    /// indicate that we wish to use G2 independence test for PC
    /// @throws OperationNotAllowed when PC is not the selected algorithm
    void useG2Test();

    /// set the significance threshold alpha for PC (default 0.05)
    /// @throws OperationNotAllowed when PC is not the selected algorithm
    void setPCAlpha(double alpha);

    /// set stable mode for PC — defer removals to end of each depth level (default true)
    /// @throws OperationNotAllowed when PC is not the selected algorithm
    void setPCStable(bool stable);

    /// set maximum conditioning set size for PC (default Size(-1) = unlimited)
    /// @throws OperationNotAllowed when PC is not the selected algorithm
    void setPCMaxCondSetSize(Size max_k);

    /// set unshielded-collider ordering for PC: sorted=true uses descending p-value order
    /// (strongest evidence first); sorted=false uses natural traversal order (default false)
    /// @throws OperationNotAllowed when PC is not the selected algorithm
    void setPCUnshieldedColliderSorted(bool sorted);

    /// @}

    // ##########################################################################
    /// @name FCI parameterization
    // ##########################################################################
    /// @{

    /// indicate that we wish to use Chi2 independence test for FCI
    /// @throws OperationNotAllowed when FCI is not the selected algorithm
    void useFCIChi2Test();

    /// indicate that we wish to use G2 independence test for FCI
    /// @throws OperationNotAllowed when FCI is not the selected algorithm
    void useFCIG2Test();

    /// set the significance threshold alpha for FCI (default 0.05)
    /// @throws OperationNotAllowed when FCI is not the selected algorithm
    void setFCIAlpha(double alpha);

    /// set maximum discriminating-path length for FCI R4 (default Size(-1) = unlimited)
    /// @throws OperationNotAllowed when FCI is not the selected algorithm
    void setFCIMaxPathLength(Size max_len);

    /// enable exhaustive sepset mode for FCI skeleton learning (default false)
    /// @throws OperationNotAllowed when FCI is not the selected algorithm
    void setFCIExhaustiveSepSet(bool exhaustive);

    /// return true when FCI uses exhaustive sepset mode
    /// @throws OperationNotAllowed when FCI is not the selected algorithm
    bool fciExhaustiveSepSet() const;

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
     * @param slices the list of list of variable names. If it is empty, this is
     * equivalent to removing the constraint (useful if you had previously added
     * a slice order constraint and you do not want to use it anymore)
     */
    void setSliceOrder(const std::vector< std::vector< std::string > >& slices);

    /** @brief removes the slice order constraint
     *
     * The methd is useful if you had previously added
     * a slice order constraint and you do not want to use it anymore
     */
    void unsetSliceOrder();

    /** @brief sets a total order over some nodes
     *
     * This defines a total ordering constraint on the nodes belonging to the
     * sequence. If you plan to use K2, do not use this constraint, prefer using
     * method useK2(order).
     *
     * Note that all the nodes of the graph to be learned need not
     * belong to order: if a node does not belong to it, then it just means that
     * there is no constraint on this node in the total ordering.
     *
     * Note also that you can both exploit a total ordering and slice ordering. For
     * instance, we may wish that the topological ordering of the graph we learn
     * satisfies that 1 < 3 < 4 < 5 and 2 < 3, 4 and 5, then use a total ordering
     * 1 < 3 < 4 < 5 combined with a slice ordering {2} < {3,4,5}.
     * @param order
     */
    /// @{
    void setTotalOrder(const Sequence< NodeId >& order);
    void setTotalOrder(const std::vector< std::string >& order);
    /// @}

    /// removes the current total ordering constraint, if any
    void unsetTotalOrder();

    /// removes a total

    /// assign a set of forbidden arcs
    void setForbiddenArcs(const ArcSet& set);

    /// @name assign a new forbidden arc
    /// @{
    void addForbiddenArc(const Arc& arc);
    void addForbiddenArc(NodeId tail, NodeId head);
    void addForbiddenArc(std::string_view tail, std::string_view head);
    /// @}

    /// @name remove a forbidden arc
    /// @{
    void eraseForbiddenArc(const Arc& arc);
    void eraseForbiddenArc(NodeId tail, NodeId head);
    void eraseForbiddenArc(std::string_view tail, std::string_view head);
    ///@}

    /// assign a set of mandatory arcs
    void setMandatoryArcs(const ArcSet& set);

    /// @name assign a new mandatory arc
    ///@{
    void addMandatoryArc(const Arc& arc);
    void addMandatoryArc(NodeId tail, NodeId head);
    void addMandatoryArc(std::string_view tail, std::string_view head);
    ///@}

    /// @name remove a mandatory arc
    ///@{
    void eraseMandatoryArc(const Arc& arc);
    void eraseMandatoryArc(NodeId tail, NodeId head);
    void eraseMandatoryArc(std::string_view tail, std::string_view head);
    /// @}

    /// @name add a node with no parent
    ///@{
    void addNoParentNode(NodeId node);
    void addNoParentNode(std::string_view node);
    /// @}

    /// @name remove a node with no parent
    ///@{
    void eraseNoParentNode(NodeId node);
    void eraseNoParentNode(std::string_view node);

    /// @name add a node with no children
    ///@{
    void addNoChildrenNode(NodeId node);
    void addNoChildrenNode(std::string_view node);
    /// @}

    /// @name remove a node with no children
    ///@{
    void eraseNoChildrenNode(NodeId node);
    void eraseNoChildrenNode(std::string_view node);
    /// @}

    /// assign a set of possible edges
    /// @warning Once at least one possible edge is defined, all other edges are
    /// not possible anymore
    /// @{
    void setPossibleEdges(const EdgeSet& set);
    void setPossibleSkeleton(const UndiGraph& skeleton);
    /// @}

    /// @name assign a new possible edge
    /// @warning By default, any edge is possible. However, once at least one possible edge
    /// is defined, all the other edges not declared possible are considered as impossible.
    /// @{
    void addPossibleEdge(const Edge& edge);
    void addPossibleEdge(NodeId tail, NodeId head);
    void addPossibleEdge(std::string_view tail, std::string_view head);
    /// @}

    /// @name remove a possible edge
    /// @{
    void erasePossibleEdge(const Edge& edge);
    void erasePossibleEdge(NodeId tail, NodeId head);
    void erasePossibleEdge(std::string_view tail, std::string_view head);
    ///@}

    ///@}

    // ##########################################################################
    /// @name Multithreading
    // ##########################################################################
    /// @{

    /// sets the number max of threads that can be used
    /**
     * @param nb the number max of threads to be used. If this number is set to 0, then
     * it is defaulted to aGrUM's max number of threads
     */
    void setNumberOfThreads(Size nb) override;

    /// @}

    protected:
    PAG  learnPAG_();
    PDAG learnPDAG_();

    /// sets the prior weight
    void _setPriorWeight_(double weight);

    /// the policy for typing variables
    bool inducedTypes_{false};

    /// the score selected for learning
    ScoreType scoreType_{ScoreType::BDeu};

    /// the score used
    Score* score_{nullptr};

    /// the type of the parameter estimator
    ParamEstimatorType paramEstimatorType_{ParamEstimatorType::ML};

    /// a Boolean indicating whether we should use EM for parameter learning or not
    bool useEM_{false};

    /// the noise factor (in (0,1)) used by EM for perturbing the CPT during init
    double noiseEM_{0.1};

    /// the selected correction for miic
    CorrectedMutualInformation* mutualInfo_{nullptr};

    /// the a priorselected for the score and parameters
    BNLearnerPriorType priorType_{BNLearnerPriorType::NO_prior};

    /// the prior used
    Prior* prior_{nullptr};

    NoPrior* noPrior_{nullptr};

    /// the weight of the prior
    double priorWeight_{1.0f};

    /// the constraint for 2TBNs
    StructuralConstraintSliceOrder constraintSliceOrder_;

    /// the total order ing constraint
    StructuralConstraintTotalOrder constraintTotalOrder_;

    /// the constraint for indegrees
    StructuralConstraintIndegree constraintIndegree_;

    /// the constraint for tabu lists
    StructuralConstraintTabuList constraintTabuList_;

    /// the constraint on forbidden arcs
    StructuralConstraintForbiddenArcs constraintForbiddenArcs_;

    /// the constraint on possible Edges
    StructuralConstraintPossibleEdges constraintPossibleEdges_;

    /// the constraint on mandatory arcs
    StructuralConstraintMandatoryArcs constraintMandatoryArcs_;

    /// the constraint on no parent nodes
    StructuralConstraintNoParentNodes constraintNoParentNodes_;

    /// the constraint on no children nodes
    StructuralConstraintNoChildrenNodes constraintNoChildrenNodes_;

    /// the selected learning algorithm
    AlgoType selectedAlgo_{AlgoType::MIIC};

    /// whether we allow or not arc additions during learning
    bool allowArcAdditions_{true};

    /// whether we allow or not arc deletions during learning
    bool allowArcDeletions_{true};

    /// whether we allow or not arc reversals during learning
    bool allowArcReversals_{true};

    /// whether we allow or not arc deletions during learning
    bool allowArcTriangleDeletions_{true};

    /// the K2 algorithm
    K2 algoK2_;

    /// the MIIC algorithm
    SimpleMiic algoSimpleMiic_;

    /// the Constraint MIIC algorithm
    Miic algoMiic_;

    /// the penalty used in MIIC
    typename CorrectedMutualInformation::KModeTypes kmodeMiic_{
        CorrectedMutualInformation::KModeTypes::MDL};

    /// the PC algorithm
    gum::learning::PC algoPC_;

    /// independence test type for PC
    enum class IndepTestType { Chi2, G2 };
    IndepTestType indepTestTypePC_{IndepTestType::Chi2};

    /// owned independence test object for PC (rebuilt before each learn call)
    IndependenceTest* indepTestPC_{nullptr};

    /// PC parameters
    double alphaPc_{0.05};
    bool   stablePc_{true};
    Size   maxCondSetSizePc_{Size(-1)};
    bool   sortedUCPc_{false};

    /// the FCI algorithm
    gum::learning::FCI algoFCI_;

    /// independence test type for FCI (reuses IndepTestType defined above)
    IndepTestType indepTestTypeFCI_{IndepTestType::Chi2};

    /// owned independence test object for FCI (rebuilt before each learn call)
    IndependenceTest* indepTestFCI_{nullptr};

    /// FCI parameters
    double alphaFci_{0.05};
    Size   maxPathLengthFci_{Size(-1)};
    bool   exhaustiveSepSetFci_{false};

    /// the parametric EM
    DAG2BNLearner dag2BN_;

    /// the greedy hill climbing algorithm
    GreedyHillClimbing greedyHillClimbing_;

    /// the extended greedy hill climbing
    GreedyHillClimbing extendedGreedyHillClimbing_;

    /// the greedy thick-thinning algorithm
    GreedyThickThinning greedyThickThinning_;

    /// the local search with tabu list algorithm
    LocalSearchWithTabuList localSearchWithTabuList_;

    /// the database to be used by the scores and parameter estimators
    Database scoreDatabase_;

    /// the set of rows' ranges within the database in which learning is done
    std::vector< std::pair< std::size_t, std::size_t > > ranges_;

    /// the database used by the Dirichlet a priori
    Database* priorDatabase_{nullptr};

    /// the filename for the Dirichlet a priori, if any
    std::string priorDbname_;

    /// an initial DAG given to learners
    DAG initialDag_;

    /// the filename database
    std::string filename_{"-"};

    // size of the tabu list
    Size nbDecreasingChanges_{2};

    // the current algorithm as an approximationScheme
    const ApproximationScheme* currentAlgorithm_{nullptr};

    /// reads a file and returns a databaseVectInRam
    static DatabaseTable readFile_(std::string_view                  filename,
                                   const std::vector< std::string >& missing_symbols);

    /// checks whether the extension of a CSV filename is correct
    static void isCSVFileName_(std::string_view filename);

    /// create the prior used for learning
    virtual void createPrior_() = 0;

    /// create the score used for learning
    void createScore_();

    /// create the parameter estimator used for learning
    ParamEstimator* createParamEstimator_(const DBRowGeneratorParser& parser,
                                          bool take_into_account_score = true);

    /// returns the DAG learnt
    DAG learnDag_();

    /// prepares the initial graph for Simple Miic
    MixedGraph prepareSimpleMiic_();

    /// prepares the initial graph for miic
    MixedGraph prepareMiic_();

    /// prepares the initial graph and independence test for PC
    MixedGraph preparePC_();

    /// prepares the initial graph and independence test for FCI
    MixedGraph prepareFCI_();

    /// returns the type (as a string) of a given prior
    PriorType getPriorType_() const;

    /// create the Corrected Mutual Information instance for Miic
    void createCorrectedMutualInformation_();

    public:
    // ##########################################################################
    /// @name redistribute signals AND implementation of interface
    /// IApproximationSchemeConfiguration
    // ##########################################################################
    // in order to not pollute the proper code of IBNLearner, we
    // directly
    // implement those
    // very simples methods here.
    /// {@    /// distribute signals
    void setCurrentApproximationScheme(const ApproximationScheme* approximationScheme);

    void distributeProgress(const ApproximationScheme* approximationScheme,
                            Size                       pourcent,
                            double                     error,
                            double                     time);

    /// distribute signals
    void distributeStop(const ApproximationScheme* approximationScheme, std::string_view message);

    /// @}

    /// Given that we approximate f(t), stopping criterion on |f(t+1)-f(t)|
    /// If the criterion was disabled it will be enabled
    /// @{
    /// @throw OutOfBounds if eps<0
    void setEpsilon(double eps) override;

    /// Get the value of epsilon
    double epsilon() const override;

    /// Disable stopping criterion on epsilon
    void disableEpsilon() override;

    /// Enable stopping criterion on epsilon
    void enableEpsilon() override;

    /// @return true if stopping criterion on epsilon is enabled, false
    /// otherwise
    bool isEnabledEpsilon() const override;

    /// @}

    /// Given that we approximate f(t), stopping criterion on
    /// d/dt(|f(t+1)-f(t)|)
    /// If the criterion was disabled it will be enabled
    /// @{
    /// @throw OutOfBounds if rate<0
    void setMinEpsilonRate(double rate) override;

    /// Get the value of the minimal epsilon rate
    double minEpsilonRate() const override;

    /// Disable stopping criterion on epsilon rate
    void disableMinEpsilonRate() override;

    /// Enable stopping criterion on epsilon rate
    void enableMinEpsilonRate() override;

    /// @return true if stopping criterion on epsilon rate is enabled, false
    /// otherwise
    bool isEnabledMinEpsilonRate() const override;

    /// @}

    /// stopping criterion on number of iterations
    /// @{
    /// If the criterion was disabled it will be enabled
    /// @param max The maximum number of iterations
    /// @throw OutOfBounds if max<=1
    void setMaxIter(Size max) override;

    /// @return the criterion on number of iterations
    Size maxIter() const override;

    /// Disable stopping criterion on max iterations
    void disableMaxIter() override;

    /// Enable stopping criterion on max iterations
    void enableMaxIter() override;

    /// @return true if stopping criterion on max iterations is enabled, false
    /// otherwise
    bool isEnabledMaxIter() const override;

    /// @}

    /// stopping criterion on timeout
    /// If the criterion was disabled it will be enabled
    /// @{
    /// @throw OutOfBounds if timeout<=0.0
    /** timeout is time in second (double).
     */
    void setMaxTime(double timeout) override;

    /// returns the timeout (in seconds)
    double maxTime() const override;

    /// get the current running time in second (double)
    double currentTime() const override;

    /// Disable stopping criterion on timeout
    void disableMaxTime() override;

    void enableMaxTime() override;

    /// @return true if stopping criterion on timeout is enabled, false
    /// otherwise
    bool isEnabledMaxTime() const override;

    /// @}

    /// how many samples between 2 stopping isEnableds
    /// @{
    /// @throw OutOfBounds if p<1
    void setPeriodSize(Size p) override;

    Size periodSize() const override;

    /// @}

    /// verbosity
    /// @{
    void setVerbosity(bool v) override;

    bool verbosity() const override;

    /// @}

    /// history
    /// @{

    ApproximationSchemeSTATE stateApproximationScheme() const override;

    /// @throw OperationNotAllowed if scheme not performed
    Size nbrIterations() const override;

    /// @throw OperationNotAllowed if scheme not performed or verbosity=false
    const std::vector< double >& history() const override;

    /// @}


    /// @name EM approximation scheme for parameter learning
    /// @{

    /**
     * @brief sets the stopping criterion of EM as being the minimal difference between two
     * consecutive log-likelihoods
     * @param eps the log-likelihood difference below which EM stops its iterations
     * @warning setting this stopping criterion disables the min rate criterion (if it was enabled)
     * @throw OutOfBounds if eps <= 0
     */
    void EMsetEpsilon(double eps);

    /// Get the value of EM's min diff epsilon
    /**
     * @brief Get the value of EM's min diff epsilon
     * @warning Note that this value is not taken into account unless the min diff criterion is
     * enabled
     */
    double EMEpsilon() const;

    /// Disable the min log-likelihood diff stopping criterion for EM
    void EMdisableEpsilon();

    /**
     * @brief Enable the log-likelihood min diff stopping criterion in EM
     * @warning setting this stopping criterion disables the min rate criterion (if it was enabled)
     */
    void EMenableEpsilon();

    /// return true if EM's stopping criterion is the log-likelihood min diff
    bool EMisEnabledEpsilon() const;

    /**
     * @brief sets the stopping criterion of EM as being the minimal log-likelihood's evolution rate
     * @param rate the log-likelihood evolution rate below which EM stops its iterations
     * @warning setting this stopping criterion disables the min diff criterion (if it was enabled)
     * @throw OutOfBounds if rate<=0
     */
    void EMsetMinEpsilonRate(double rate);

    /**
     * @brief Get the value of the minimal log-likelihood evolution rate of EM
     * @warning Note that this value is not taken into account unless the min rate criterion is
     * enabled
     */
    double EMMinEpsilonRate() const;

    /// Disable the log-likelihood evolution rate stopping criterion
    void EMdisableMinEpsilonRate();

    /**
     * @brief Enable the log-likelihood evolution rate stopping criterion
     * @warning setting this stopping criterion disables the min diff criterion (if it was enabled)
     */
    void EMenableMinEpsilonRate();

    /// @return true if EM's stopping criterion is the log-likelihood evolution rate
    bool EMisEnabledMinEpsilonRate() const;

    /**
     * @brief add a max iteration stopping criterion
     * @param max the max number of iterations that EM is allowed to perform
     * @throw OutOfBounds if max<=1
     */
    void EMsetMaxIter(Size max);

    /**
     * @brief return the max number of iterations criterion
     * @warning Note that this value is not taken into account unless the max iter criterion is
     * enabled
     */
    Size EMMaxIter() const;

    /// Disable stopping criterion on max iterations
    void EMdisableMaxIter();

    /// Enable stopping criterion on max iterations
    void EMenableMaxIter();

    /// @return true if stopping criterion on max iterations is enabled, false
    /// otherwise
    bool EMisEnabledMaxIter() const;

    /**
     * @brief add a stopping criterion on timeout
     * @param timeout the timeout in milliseconds
     * @throw OutOfBounds if timeout<=0.0
     */
    void EMsetMaxTime(double timeout);

    /**
     * @@brief returns EM's timeout (in milliseconds)
     * @warning Note that this value is not taken into account unless the max time criterion is
     * enabled
     */
    double EMMaxTime() const;

    /// get the current running time in second (double)
    double EMCurrentTime() const;

    /// Disable EM's timeout stopping criterion
    void EMdisableMaxTime();

    void EMenableMaxTime();

    /// @return true if stopping criterion on timeout is enabled, false otherwise
    bool EMisEnabledMaxTime() const;

    /**
     * @brief how many samples between 2 stoppings isEnabled
     * @throw OutOfBounds if p<1
     */
    void EMsetPeriodSize(Size p);

    Size EMPeriodSize() const;

    /// sets or unsets EM's verbosity
    void EMsetVerbosity(bool v);

    /// returns the EM's verbosity status
    bool EMVerbosity() const;

    /// get the current state of EM
    ApproximationSchemeSTATE EMStateApproximationScheme() const;

    /// returns the number of iterations performed by the last EM execution
    Size EMnbrIterations() const;

    /**
     * @brief returns the history of the last EM execution
     * @warning to activate the history recording, EM's verbosity must be set to true
     */
    const std::vector< double >& EMHistory() const;

    /// @}
  };

  /* namespace learning */
}   // namespace gum::learning

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/BN/learning/BNLearnUtils/IBNLearner_inl.h>
#endif /* GUM_NO_INLINE */

#include <agrum/BN/learning/BNLearnUtils/IBNLearner_tpl.h>

#endif /* GUM_LEARNING_GENERIC_BN_LEARNER_H */
