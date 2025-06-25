/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief A basic pack of learning algorithms that can easily be used
 *
 * The pack currently contains K2, GreedyHillClimbing and
 *LocalSearchWithTabuList
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_BN_LEARNER_H
#define GUM_LEARNING_BN_LEARNER_H

#include <algorithm>
#include <sstream>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/BN/learning/BNLearnUtils/IBNLearner.h>
#include <agrum/BN/learning/priors/DirichletPriorFromBN.h>

namespace gum {
  namespace learning {
    class BNLearnerListener;

    /** @class BNLearner
     * @brief A pack of learning algorithms that can easily be used
     *
     * The pack currently contains K2, GreedyHillClimbing and
     * LocalSearchWithTabuList
     * @ingroup learning_group
     */
    template < typename GUM_SCALAR >
    class BNLearner: public IBNLearner {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /**
       * read the database file for the score / parameter estimation and var
       * names
       * @param filename the name of a CSV file containing the dataset
       * @param missingSymbols the set of symbols in the CSV that should
       * be interpreted as missing values
       * @param induceTypes By default, all the values in the dataset are
       * interpreted as "labels", i.e., as categorical values. But if some
       * columns of the dataset have only numerical values, it would certainly
       * be better to tag them as corresponding to integer, range or continuous
       * variables. By setting induceTypes to true, this is precisely what the
       * BNLearner will do.
       */
      BNLearner(const std::string&                filename,
                const std::vector< std::string >& missingSymbols = {"?"},
                const bool                        induceTypes    = true);

      /// default constructor
      /**
       * read the database file for the score / parameter estimation and var
       * names
       * @param db an already initialized database table that is used to
       * fill the Database
       */
      BNLearner(const DatabaseTable& db);

      /**
       * Wrapper for BNLearner (filename,modalities,parse_database) using a bn
       * to find those modalities and nodeids.
       **/
      BNLearner(const std::string&                 filename,
                const gum::BayesNet< GUM_SCALAR >& src,
                const std::vector< std::string >&  missing_symbols = {"?"});

      /// copy constructor
      BNLearner(const BNLearner&);

      /// move constructor
      BNLearner(BNLearner&&);

      /// destructor
      virtual ~BNLearner();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      BNLearner& operator=(const BNLearner&);

      /// move operator
      BNLearner& operator=(BNLearner&&) noexcept;

      /// @}

      /// learn a Bayes Net from a file (must have read the db before)
      BayesNet< GUM_SCALAR > learnBN();

      /**
       * @brief learns a BN (its parameters) with the structure passed in argument
       * @param dag the structure of the Bayesian network
       * @param takeIntoAccountScore The dag passed in argument may have
       * been learnt from a structure learning. In this case, if the score used
       * to learn the structure has an implicit prior (like K2 which has a
       * 1-smoothing prior), it is important to also take into account this
       * implicit prior for parameter learning. By default, if a score
       * exists, we will learn parameters by taking into account the prior
       * specified by methods usePriorXXX () + the implicit prior of the
       * score, else we just take into account the prior specified by
       * usePriorXXX ()
       * @throw MissingVariableInDatabase if a variable of the BN is not found
       * in the database.
       * @throw MissingValueInDatabase if the database contains some missing
       * values and EM is not used for the learning.
       * @throw OperationNotAllowed if EM is used but no EM stopping criterion
       * has been selected.
       * @warning if method useEM() has been executed, then the learning is
       * performed using EM, else this is a standard (e.g. ML/MAP) learning
       */
      BayesNet< GUM_SCALAR > learnParameters(const DAG& dag, bool takeIntoAccountScore = true);

      /**
       * @brief learns a BN (its parameters) with the BN structure passed in
       * argument, EM being initialized by this argument
       * @param bn the Bayesian network used to specify the graphical structure
       * of the returned Bayes net and to initialize its CPTs before running EM.
       * When a CPT is filled exclusively with only zeroes, then this one is
       * initialized by the BNLearner using a specific estimator that does not
       * take into account the missing values in the database
       * @param takeIntoAccountScore The dag passed in argument may have
       * been learnt from a structure learning. In this case, if the score used
       * to learn the structure has an implicit prior (like K2 which has a
       * 1-smoothing prior), it is important to also take into account this
       * implicit prior for parameter learning. By default, if a score
       * exists, we will learn parameters by taking into account the prior
       * specified by methods usePriorXXX () + the implicit prior of the
       * score, else we just take into account the prior specified by
       * usePriorXXX ()
       * @throw MissingVariableInDatabase if a variable of the BN is not found
       * in the database.
       * @throw OperationNotAllowed if EM is used but no EM stopping criterion
       * has been selected.
       * @warning if method useEM() has been executed, then the learning is
       * performed using EM, else this is a standard (e.g. ML/MAP) learning
       * @warning the EM algorithm initializes the parameters of the CPTs using
       * those of the Bayes net passed in argument whenever those are not fully
       * filled with zeroes, else the BNLearner initializes them using an
       * estimator that does not take into account the missing values.
       * If you wish the BNLearner to automatically initialize all the CPTs, use
       * the method in which a DAG is passed in argument rather than a Bayes net.
       */
      BayesNet< GUM_SCALAR > learnParameters(const BayesNet< GUM_SCALAR >& bn,
                                             bool takeIntoAccountScore = true);

      /**
       * @brief learns a BN (its parameters) when its structure is known
       * @param take_into_account_score The dag of the BN which was passed in
       * argument to the BNLearner may have
       * been learnt from a structure learning. In this case, if the score used
       * to learn the structure has an implicit prior (like K2 which has a
       * 1-smoothing prior), it is important to also take into account this
       * implicit prior for parameter learning. By default, if a score
       * exists, we will learn parameters by taking into account the prior
       * specified by methods usePriorXXX () + the implicit prior of the
       * score, else we just take into account the prior specified by
       * usePriorXXX ()
       * @warning if method useEM() has been executed, then the learning is
       * performed using EM, else this is a standard (e.g. ML/MAP) learning
       * @throw MissingVariableInDatabase if a variable of the BN is not found
       * in the database.
       * @throw UnknownLabelInDatabase if a label is found in the databast that
       * do not correpond to the variable.
       * @throw OperationNotAllowed if EM is used but neither the min log-likelihood
       * difference nor the min log-likelihood evolution rate have been
       * selected as stopping criteria
       */
      BayesNet< GUM_SCALAR > learnParameters(bool take_into_account_score = true);

      /// @return Returns a string representation of this BNLearner's current features.
      std::string toString() const;

      /// @return a representation of the state of the learner in the form vector<key,value,comment>
      std::vector< std::tuple< std::string, std::string, std::string > > state() const;

      /**
       * @brief copy the states of the BNLearner
       * @warning except the database ! just the configurations of the learner
       */
      void copyState(const BNLearner< GUM_SCALAR >& learner);

      //=== === add return to certain methods in order to chain command
      BNLearner< GUM_SCALAR >& setInitialDAG(const DAG& dag) {
        IBNLearner::setInitialDAG(dag);
        return *this;
      }

      /**
       * @brief use The EM algorithm to learn parameters
       *
       * This is essentially an alias for Method useEMWithRateCriterion().
       * @param epsilon sets the approximation stopping criterion: EM stops
       * whenever the absolute value of the relative difference between two
       * consecutive log-likelihoods drops below epsilon.
       * Note that epsilon=0 is considered as a directive to not use EM.
       * However, if you wish to forbid the use of EM, prefer executing Method
       * forbidEM() rather than useEM(0) as it is more unequivocal.
       * @param noise When EM starts, it initializes all the CPTs of the Bayes
       * net. EM adds a noise to these CPTs by mixing their values with some
       * random noise. The formula used is, up to some normalizing constant:
       * new_cpt = (1-noise) * cpt + noise * random_cpt(). Of course, noise must
       * belong to interval [0,1].
       * @return the BNLearner (so that we can chains the calls to useXXX() methods)
       * @warning if epsilon=0, EM is not used
       * @throws OutOfBounds is raised if epsilon is strictly negative or if
       * noise does not belong to interval [0,1].
       */
      BNLearner< GUM_SCALAR >& useEM(const double epsilon, const double noise = default_EM_noise) {
        IBNLearner::useEM(epsilon, noise);
        return *this;
      }

      /**
       * @brief use The EM algorithm to learn parameters with the rate stopping criterion
       * @param epsilon epsilon sets the approximation stopping criterion: EM stops
       * whenever the absolute value of the relative difference between two
       * consecutive log-likelihoods drops below epsilon. Note that, for using EM,
       * epsilon should be strictly positive.
       * @param max_nb_iter the maximum number of EM iterations allowed. If equal
       * to 0, this stopping criterion is unused.
       * @param noise When EM starts, it initializes all the CPTs of the Bayes
       * net. EM adds a noise to these CPTs by mixing their values with some
       * random noise. The formula used is, up to some normalizing constant:
       * new_cpt = (1-noise) * cpt + noise * random_cpt(). Of course, noise must
       * belong to interval [0,1].
       * @return the BNLearner (so that we can chains the calls to useXXX() methods)
       * @throws OutOfBounds is raised if epsilon is not strictly positive or if
       * noise does not belong to interval [0,1].
       */
      BNLearner< GUM_SCALAR >& useEMWithRateCriterion(const double epsilon,
                                                      const double noise = default_EM_noise) {
        IBNLearner::useEMWithRateCriterion(epsilon, noise);
        return *this;
      }

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
       * @return the BNLearner (so that we can chains the calls to useXXX() methods)
       * @throws OutOfBounds is raised if epsilon is not strictly positive or if
       * noise does not belong to interval [0,1].
       */
      BNLearner< GUM_SCALAR >& useEMWithDiffCriterion(const double epsilon,
                                                      const double noise = default_EM_noise) {
        IBNLearner::useEMWithDiffCriterion(epsilon, noise);
        return *this;
      }

      /// prevent using the EM algorithm for parameter learning
      BNLearner< GUM_SCALAR >& forbidEM() {
        IBNLearner::forbidEM();
        return *this;
      }

      /**
       * @brief sets the stopping criterion of EM as being the minimal difference between two
       * consecutive log-likelihoods
       * @param eps the log-likelihood difference below which EM stops its iterations
       * @warning setting this stopping criterion disables the min rate criterion (if it was
       * enabled)
       * @throw OutOfBounds if eps <= 0
       */
      BNLearner< GUM_SCALAR >& EMsetEpsilon(const double eps) {
        IBNLearner::EMsetEpsilon(eps);
        return *this;
      }

      /// Disable the min log-likelihood diff stopping criterion
      BNLearner< GUM_SCALAR >& EMdisableEpsilon() {
        IBNLearner::EMdisableEpsilon();
        return *this;
      }

      /**
       * @brief Enable the log-likelihood min diff stopping criterion in EM
       * @warning setting this stopping criterion disables the min rate criterion (if it was
       * enabled)
       */
      BNLearner< GUM_SCALAR >& EMenableEpsilon() {
        IBNLearner::EMenableEpsilon();
        return *this;
      }

      /**
       * @brief sets the stopping criterion of EM as being the minimal log-likelihood's evolution
       * rate
       * @param rate the log-likelihood evolution rate below which EM stops its iterations
       * @warning setting this stopping criterion disables the min diff criterion (if it was
       * enabled)
       * @throw OutOfBounds if rate<=0
       */
      BNLearner< GUM_SCALAR >& EMsetMinEpsilonRate(const double rate) {
        IBNLearner::EMsetMinEpsilonRate(rate);
        return *this;
      }

      /// Disable the log-likelihood evolution rate stopping criterion
      BNLearner< GUM_SCALAR >& EMdisableMinEpsilonRate() {
        IBNLearner::EMdisableMinEpsilonRate();
        return *this;
      }

      /**
       * @brief Enable the log-likelihood evolution rate stopping criterion
       * @warning setting this stopping criterion disables the min diff criterion (if it was
       * enabled)
       */
      BNLearner< GUM_SCALAR >& EMenableMinEpsilonRate() {
        IBNLearner::EMenableMinEpsilonRate();
        return *this;
      }

      /**
       * @brief add a max iteration stopping criterion
       * @param max the max number of iterations that EM is allowed to perform
       * @throw OutOfBounds if max<=1
       */
      BNLearner< GUM_SCALAR >& EMsetMaxIter(const Size max) {
        IBNLearner::EMsetMaxIter(max);
        return *this;
      }

      /// Disable stopping criterion on max iterations
      BNLearner< GUM_SCALAR >& EMdisableMaxIter() {
        IBNLearner::EMdisableMaxIter();
        return *this;
      }

      /// Enable stopping criterion on max iterations
      BNLearner< GUM_SCALAR >& EMenableMaxIter() {
        IBNLearner::EMenableMaxIter();
        return *this;
      }

      /**
       * @brief add a stopping criterion on timeout
       * @param timeout the timeout in milliseconds
       * @throw OutOfBounds if timeout<=0.0
       */
      BNLearner< GUM_SCALAR >& EMsetMaxTime(const double timeout) {
        IBNLearner::EMsetMaxTime(timeout);
        return *this;
      }

      /// Disable EM's timeout stopping criterion
      BNLearner< GUM_SCALAR >& EMdisableMaxTime() {
        IBNLearner::EMdisableMaxTime();
        return *this;
      }

      /// enable EM's timeout stopping criterion
      BNLearner< GUM_SCALAR >& EMenableMaxTime() {
        IBNLearner::EMenableMaxTime();
        return *this;
      };

      /**
       * @brief how many samples between 2 stoppings isEnabled
       * @throw OutOfBounds if p<1
       */
      BNLearner< GUM_SCALAR >& EMsetPeriodSize(const Size p) {
        IBNLearner::EMsetPeriodSize(p);
        return *this;
      }

      // for pyagrum support
      using IBNLearner::EMPeriodSize;

      /// sets or unsets EM's verbosity
      BNLearner< GUM_SCALAR >& EMsetVerbosity(const bool v) {
        IBNLearner::EMsetVerbosity(v);
        return *this;
      }

      BNLearner< GUM_SCALAR >& useScoreAIC() {
        IBNLearner::useScoreAIC();
        return *this;
      }

      BNLearner< GUM_SCALAR >& useScoreBD() {
        IBNLearner::useScoreBD();
        return *this;
      }

      BNLearner< GUM_SCALAR >& useScoreBDeu() {
        IBNLearner::useScoreBDeu();
        return *this;
      }

      BNLearner< GUM_SCALAR >& useScoreBIC() {
        IBNLearner::useScoreBIC();
        return *this;
      }

      BNLearner< GUM_SCALAR >& useScoreK2() {
        IBNLearner::useScoreK2();
        return *this;
      }

      BNLearner< GUM_SCALAR >& useScoreLog2Likelihood() {
        IBNLearner::useScoreLog2Likelihood();
        return *this;
      }

      BNLearner< GUM_SCALAR >& useNoPrior() {
        IBNLearner::useNoPrior();
        return *this;
      }

      BNLearner< GUM_SCALAR >& useBDeuPrior(double weight = 1.0) {
        IBNLearner::useBDeuPrior(weight);
        return *this;
      }

      BNLearner< GUM_SCALAR >& useSmoothingPrior(double weight = 1) {
        IBNLearner::useSmoothingPrior(weight);
        return *this;
      }

      BNLearner< GUM_SCALAR >& useDirichletPrior(const std::string& filename, double weight = 1) {
        IBNLearner::useDirichletPrior(filename, weight);
        return *this;
      }

      BNLearner< GUM_SCALAR >& useDirichletPrior(const gum::BayesNet< GUM_SCALAR >& bn,
                                                 double                             weight = 1) {
        if (weight < 0) { GUM_ERROR(OutOfBounds, "the weight of the prior must be positive") }

        _prior_bn_ = gum::BayesNet< GUM_SCALAR >(bn);
        priorType_ = BNLearnerPriorType::DIRICHLET_FROM_BAYESNET;
        _setPriorWeight_(weight);

        checkScorePriorCompatibility();
        return *this;
      }

      BNLearner< GUM_SCALAR >& useGreedyHillClimbing() {
        IBNLearner::useGreedyHillClimbing();
        return *this;
      }

      BNLearner< GUM_SCALAR >& useLocalSearchWithTabuList(Size tabu_size   = 100,
                                                          Size nb_decrease = 2) {
        IBNLearner::useLocalSearchWithTabuList(tabu_size, nb_decrease);
        return *this;
      }

      BNLearner< GUM_SCALAR >& useK2(const Sequence< NodeId >& order) {
        IBNLearner::useK2(order);
        return *this;
      }

      BNLearner< GUM_SCALAR >& useK2(const std::vector< NodeId >& order) {
        IBNLearner::useK2(order);
        return *this;
      }

      BNLearner< GUM_SCALAR >& useMIIC() {
        IBNLearner::useMIIC();
        return *this;
      }

      BNLearner< GUM_SCALAR >& useNMLCorrection() {
        IBNLearner::useNMLCorrection();
        return *this;
      }

      BNLearner< GUM_SCALAR >& useMDLCorrection() {
        IBNLearner::useMDLCorrection();
        return *this;
      }

      BNLearner< GUM_SCALAR >& useNoCorrection() {
        IBNLearner::useNoCorrection();
        return *this;
      }

      BNLearner< GUM_SCALAR >& setMaxIndegree(Size max_indegree) {
        IBNLearner::setMaxIndegree(max_indegree);
        return *this;
      }

      BNLearner< GUM_SCALAR >& setSliceOrder(const NodeProperty< NodeId >& slice_order) {
        IBNLearner::setSliceOrder(slice_order);
        return *this;
      }

      BNLearner< GUM_SCALAR >&
          setSliceOrder(const std::vector< std::vector< std::string > >& slices) {
        IBNLearner::setSliceOrder(slices);
        return *this;
      }

      BNLearner< GUM_SCALAR >& setForbiddenArcs(const ArcSet& set) {
        IBNLearner::setForbiddenArcs(set);
        return *this;
      }

      BNLearner< GUM_SCALAR >& addForbiddenArc(const Arc& arc) {
        IBNLearner::addForbiddenArc(arc);
        return *this;
      }

      BNLearner< GUM_SCALAR >& addForbiddenArc(NodeId tail, NodeId head) {
        IBNLearner::addForbiddenArc(tail, head);
        return *this;
      }

      BNLearner< GUM_SCALAR >& addForbiddenArc(const std::string& tail, const std::string& head) {
        IBNLearner::addForbiddenArc(tail, head);
        return *this;
      }

      BNLearner< GUM_SCALAR >& eraseForbiddenArc(const Arc& arc) {
        IBNLearner::eraseForbiddenArc(arc);
        return *this;
      }

      BNLearner< GUM_SCALAR >& eraseForbiddenArc(NodeId tail, NodeId head) {
        IBNLearner::eraseForbiddenArc(tail, head);
        return *this;
      }

      BNLearner< GUM_SCALAR >& eraseForbiddenArc(const std::string& tail, const std::string& head) {
        IBNLearner::eraseForbiddenArc(tail, head);
        return *this;
      }

      BNLearner< GUM_SCALAR >& addMandatoryArc(const Arc& arc) {
        IBNLearner::addMandatoryArc(arc);
        return *this;
      }

      BNLearner< GUM_SCALAR >& addMandatoryArc(NodeId tail, NodeId head) {
        IBNLearner::addMandatoryArc(tail, head);
        return *this;
      }

      BNLearner< GUM_SCALAR >& addMandatoryArc(const std::string& tail, const std::string& head) {
        IBNLearner::addMandatoryArc(tail, head);
        return *this;
      }

      BNLearner< GUM_SCALAR >& eraseMandatoryArc(const Arc& arc) {
        IBNLearner::eraseMandatoryArc(arc);
        return *this;
      }

      BNLearner< GUM_SCALAR >& eraseMandatoryArc(NodeId tail, NodeId head) {
        IBNLearner::eraseMandatoryArc(tail, head);
        return *this;
      }

      BNLearner< GUM_SCALAR >& eraseMandatoryArc(const std::string& tail, const std::string& head) {
        IBNLearner::eraseMandatoryArc(tail, head);
        return *this;
      }

      BNLearner< GUM_SCALAR >& addPossibleEdge(const Edge& edge) {
        IBNLearner::addPossibleEdge(edge);
        return *this;
      }

      BNLearner< GUM_SCALAR >& addPossibleEdge(NodeId tail, NodeId head) {
        IBNLearner::addPossibleEdge(tail, head);
        return *this;
      }

      BNLearner< GUM_SCALAR >& addPossibleEdge(const std::string& tail, const std::string& head) {
        IBNLearner::addPossibleEdge(tail, head);
        return *this;
      }

      BNLearner< GUM_SCALAR >& erasePossibleEdge(const Edge& edge) {
        IBNLearner::erasePossibleEdge(edge);
        return *this;
      }

      BNLearner< GUM_SCALAR >& erasePossibleEdge(NodeId tail, NodeId head) {
        IBNLearner::erasePossibleEdge(tail, head);
        return *this;
      }

      BNLearner< GUM_SCALAR >& erasePossibleEdge(const std::string& tail, const std::string& head) {
        IBNLearner::erasePossibleEdge(tail, head);
        return *this;
      }

      BNLearner< GUM_SCALAR >& setMandatoryArcs(const ArcSet& set) {
        IBNLearner::setMandatoryArcs(set);
        return *this;
      }

      BNLearner< GUM_SCALAR >& setPossibleEdges(const EdgeSet& set) {
        IBNLearner::setPossibleEdges(set);
        return *this;
      }

      BNLearner< GUM_SCALAR >& setPossibleSkeleton(const UndiGraph& skeleton) {
        IBNLearner::setPossibleSkeleton(skeleton);
        return *this;
      }

      BNLearner< GUM_SCALAR >& addNoParentNode(NodeId node) {
        IBNLearner::addNoParentNode(node);
        return *this;
      }

      BNLearner< GUM_SCALAR >& addNoParentNode(const std::string& name) {
        IBNLearner::addNoParentNode(name);
        return *this;
      }

      BNLearner< GUM_SCALAR >& eraseNoParentNode(NodeId node) {
        IBNLearner::eraseNoParentNode(node);
        return *this;
      }

      BNLearner< GUM_SCALAR >& eraseNoParentNode(const std::string& name) {
        IBNLearner::eraseNoParentNode(name);
        return *this;
      }

      BNLearner< GUM_SCALAR >& addNoChildrenNode(NodeId node) {
        IBNLearner::addNoChildrenNode(node);
        return *this;
      }

      BNLearner< GUM_SCALAR >& addNoChildrenNode(const std::string& name) {
        IBNLearner::addNoChildrenNode(name);
        return *this;
      }

      BNLearner< GUM_SCALAR >& eraseNoChildrenNode(NodeId node) {
        IBNLearner::eraseNoChildrenNode(node);
        return *this;
      }

      BNLearner< GUM_SCALAR >& eraseNoChildrenNode(const std::string& name) {
        IBNLearner::eraseNoChildrenNode(name);
        return *this;
      }

      bool isConstraintBased() const { return IBNLearner::isConstraintBased(); }

      bool isScoreBased() const { return IBNLearner::isScoreBased(); }

      protected:
      /// create the prior used for learning
      void createPrior_() final;

      private:
      BayesNet< GUM_SCALAR > _prior_bn_;

      /// read the first line of a file to find column names
      NodeProperty< Sequence< std::string > > _labelsFromBN_(const std::string&            filename,
                                                             const BayesNet< GUM_SCALAR >& src);

      /**
       * @brief check that the database contains the nodes of the dag, else raise an exception
       * @throw MissingVariableInDatabase if a variable of the BN is not found
       * in the database.
       * @return nothing if all the nodes of the dag belong to the dataset
       */
      void _checkDAGCompatibility_(const DAG& dag);

      /**
       * @brief learns a BN (its parameters) with the structure passed in argument
       * using a single pass estimation (not EM)
       * @param dag the structure of the Bayesian network
       * @param takeIntoAccountScore The dag passed in argument may have
       * been learnt from a structure learning. In this case, if the score used
       * to learn the structure has an implicit prior (like K2 which has a
       * 1-smoothing prior), it is important to also take into account this
       * implicit prior for parameter learning. By default, if a score
       * exists, we will learn parameters by taking into account the prior
       * specified by methods usePriorXXX () + the implicit prior of the
       * score, else we just take into account the prior specified by
       * usePriorXXX ()
       * @throw MissingVariableInDatabase if a variable of the BN is not found
       * in the database.
       * @throw MissingValueInDatabase if the database contains some missing
       * values
       */
      BayesNet< GUM_SCALAR > _learnParameters_(const DAG& dag, bool takeIntoAccountScore);

      /**
       * @brief initializes EM and returns a pair containing, first, a bootstrap estimator
       * and, second, the EM estimator
       * @param dag the graphical structure of the BN learnt by EM
       * @param takeIntoAccountScore The dag passed in argument may have
       * been learnt from a structure learning. In this case, if the score used
       * to learn the structure has an implicit prior (like K2 which has a
       * 1-smoothing prior), it is important to also take into account this
       * implicit prior for parameter learning. By default, if a score
       * exists, we will learn parameters by taking into account the prior
       * specified by methods usePriorXXX () + the implicit prior of the
       * score, else we just take into account the prior specified by
       * usePriorXXX ()
       * @return a pair of estimators. The first one is used to fill the CPTs
       * of the returned BN in order to initialize EM. It fills them using
       * the database records without taking into account the missing values.
       * The second estimator is the one used by all the subsequent iterations of
       * the EM algorithm.
       */
      std::pair< std::shared_ptr< ParamEstimator >, std::shared_ptr< ParamEstimator > >
          _initializeEMParameterLearning_(const DAG& dag, bool takeIntoAccountScore);

      /**
       * @brief learns a BN (its parameters) with the structure passed in argument
       * using the EM algorithm initialized by the BNLearner
       * @param dag the structure of the Bayesian network
       * @param takeIntoAccountScore The dag passed in argument may have
       * been learnt from a structure learning. In this case, if the score used
       * to learn the structure has an implicit prior (like K2 which has a
       * 1-smoothing prior), it is important to also take into account this
       * implicit prior for parameter learning. By default, if a score
       * exists, we will learn parameters by taking into account the prior
       * specified by methods usePriorXXX () + the implicit prior of the
       * score, else we just take into account the prior specified by
       * usePriorXXX ()
       * @throw MissingVariableInDatabase if a variable of the BN is not found
       * in the database.
       * @warning the EM algorithm initializes the parameters of the CPTs by
       * running a first estimator that does not take into account missing values.
       * If you wish to initialize them yourself, use the method in which a
       * Bayes net is passed in argument rather than a DAG.
       */
      BayesNet< GUM_SCALAR > _learnParametersWithEM_(const DAG& dag, bool takeIntoAccountScore);

      /**
       * @brief learns a BN (its parameters) with the structure passed in argument
       * using the EM algorithm initialized by the Bayes net passed in argument
       * @param bn the Bayesian network used to specify the graphical structure
       * of the returned Bayes net and to initialize its CPTs before running EM.
       * When a CPT is filled exclusively with only zeroes, then this one is
       * initialized by the BNLearner using a specific estimator that does not
       * take into account the missing values in the database
       * @param takeIntoAccountScore The dag passed in argument may have
       * been learnt from a structure learning. In this case, if the score used
       * to learn the structure has an implicit prior (like K2 which has a
       * 1-smoothing prior), it is important to also take into account this
       * implicit prior for parameter learning. By default, if a score
       * exists, we will learn parameters by taking into account the prior
       * specified by methods usePriorXXX () + the implicit prior of the
       * score, else we just take into account the prior specified by
       * usePriorXXX ()
       * @throw MissingVariableInDatabase if a variable of the BN is not found
       * in the database.
       * @warning the EM algorithm initializes the parameters of the CPTs using
       * those of the Bayes net passed in argument whenever those are not fully
       * filled with zeroes, else the BNLearner initializes them using an
       * estimator that does not take into account the missing values.
       * If you wish the BNLearner to automatically initialize all the CPTs, use
       * the method in which a DAG is passed in argument rather than a Bayes net.
       */
      BayesNet< GUM_SCALAR > _learnParametersWithEM_(const BayesNet< GUM_SCALAR >& bn,
                                                     bool takeIntoAccountScore);
    };

    /// Prints BNLearner's current features
    template < typename GUM_SCALAR >
    std::ostream& operator<<(std::ostream& output, const BNLearner< GUM_SCALAR >& learner);
  } /* namespace learning */
} /* namespace gum */

/// always include templated methods
#include <agrum/BN/learning/BNLearner_tpl.h>

#endif /* GUM_LEARNING_BN_LEARNER_H */
