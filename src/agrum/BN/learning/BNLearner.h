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
 * @brief A basic pack of learning algorithms that can easily be used
 *
 * The pack currently contains K2, GreedyHillClimbing and
 *LocalSearchWithTabuList
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_BN_LEARNER_H
#define GUM_LEARNING_BN_LEARNER_H

#include <sstream>
#include <algorithm>
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
      BNLearner& operator=(BNLearner&&);

      /// @}

      /// learn a Bayes Net from a file (must have read the db before)
      BayesNet< GUM_SCALAR > learnBN();

      /// learns a BN (its parameters) when its structure is known
      /**
       * @param dag the structure of the Bayesian network
       * @param takeIntoAccountScore The dag passed in argument may have
       * been learnt from a structure learning. In this case, if the score used
       * to learn the structure has an implicit prior (like K2 which has a
       * 1-smoothing prior), it is important to also take into account this
       * implicit prior for parameter learning. By default, if a score
       * exists, we will learn parameters by taking into account the prior
       * specified by methods usePriorXXX () + the implicit prior of the
       * score, else we just take into account the prior specified by
       * usePriorXXX () */
      BayesNet< GUM_SCALAR > learnParameters(const DAG& dag, bool takeIntoAccountScore = true);

      // learns a BN (its parameters) when its structure is known
      /** @param take_into_account_score The dag of the BN which was passed in
       * argument to the BNLearner may have
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
       * @throw UnknownLabelInDatabase if a label is found in the databast that
       * do not correpond to the variable.
       */
      BayesNet< GUM_SCALAR > learnParameters(bool take_into_account_score = true);

      /// @return Returns a string representation of this BNLearner's current features.
      std::string toString() const;

      /// @return a representation of the state of the learner in the form vector<key,value,comment>
      std::vector< std::tuple< std::string, std::string, std::string > > state() const;

      //=== === add return to certain methods in order to chain command
      BNLearner< GUM_SCALAR >& setInitialDAG(const DAG& dag) {
        IBNLearner::setInitialDAG(dag);
        return *this;
      }
      BNLearner< GUM_SCALAR >& useEM(const double epsilon) {
        IBNLearner::useEM(epsilon);
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
      BNLearner< GUM_SCALAR >& use3off2() {
        IBNLearner::use3off2();
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
      BNLearner< GUM_SCALAR >& addForbiddenArc(const NodeId tail, const NodeId head) {
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
      BNLearner< GUM_SCALAR >& eraseForbiddenArc(const NodeId tail, const NodeId head) {
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
      BNLearner< GUM_SCALAR >& addMandatoryArc(const NodeId tail, const NodeId head) {
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
      BNLearner< GUM_SCALAR >& eraseMandatoryArc(const NodeId tail, const NodeId head) {
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
      BNLearner< GUM_SCALAR >& addPossibleEdge(const NodeId tail, const NodeId head) {
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
      BNLearner< GUM_SCALAR >& erasePossibleEdge(const NodeId tail, const NodeId head) {
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

      protected:
      /// create the prior used for learning
      void createPrior_() final;

      private:
      BayesNet< GUM_SCALAR > _prior_bn_;

      /// read the first line of a file to find column names
      NodeProperty< Sequence< std::string > > _labelsFromBN_(const std::string&            filename,
                                                             const BayesNet< GUM_SCALAR >& src);
    };

    /// Prints BNLearner's current features
    template < typename GUM_SCALAR >
    std::ostream& operator<<(std::ostream& output, const BNLearner< GUM_SCALAR >& learner);
  } /* namespace learning */

} /* namespace gum */

/// always include templated methods
#include <agrum/BN/learning/BNLearner_tpl.h>

#endif /* GUM_LEARNING_BN_LEARNER_H */
