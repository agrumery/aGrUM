/**
 *
 *   Copyright (c) 2005-2022 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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

#include <agrum/BN/BayesNet.h>
#include <agrum/agrum.h>
#include <agrum/tools/core/bijection.h>
#include <agrum/tools/core/sequence.h>
#include <agrum/tools/graphs/DAG.h>

#include <agrum/BN/learning/BNLearnUtils/genericBNLearner.h>

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
    class BNLearner: public GenericBNLearner {
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
       * be better totag them as corresponding to integer, range or continuous
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
       * to learn the structure has an implicit apriori (like K2 which has a
       * 1-smoothing apriori), it is important to also take into account this
       * implicit apriori for parameter learning. By default, if a score
       * exists, we will learn parameters by taking into account the apriori
       * specified by methods useAprioriXXX () + the implicit apriori of the
       * score, else we just take into account the apriori specified by
       * useAprioriXXX () */
      BayesNet< GUM_SCALAR > learnParameters(const DAG& dag, bool takeIntoAccountScore = true);

      // learns a BN (its parameters) when its structure is known
      /** @param take_into_account_score The dag of the BN which was passed in
       * argument to the BNLearner may have
       * been learnt from a structure learning. In this case, if the score used
       * to learn the structure has an implicit apriori (like K2 which has a
       * 1-smoothing apriori), it is important to also take into account this
       * implicit apriori for parameter learning. By default, if a score
       * exists, we will learn parameters by taking into account the apriori
       * specified by methods useAprioriXXX () + the implicit apriori of the
       * score, else we just take into account the apriori specified by
       * useAprioriXXX ()
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
        GenericBNLearner::setInitialDAG(dag);
        return *this;
      }
      BNLearner< GUM_SCALAR >& useEM(const double epsilon) {
        GenericBNLearner::useEM(epsilon);
        return *this;
      }
      BNLearner< GUM_SCALAR >& useScoreAIC() {
        GenericBNLearner::useScoreAIC();
        return *this;
      }
      BNLearner< GUM_SCALAR >& useScoreBD() {
        GenericBNLearner::useScoreBD();
        return *this;
      }
      BNLearner< GUM_SCALAR >& useScoreBDeu() {
        GenericBNLearner::useScoreBDeu();
        return *this;
      }
      BNLearner< GUM_SCALAR >& useScoreBIC() {
        GenericBNLearner::useScoreBIC();
        return *this;
      }
      BNLearner< GUM_SCALAR >& useScoreK2() {
        GenericBNLearner::useScoreK2();
        return *this;
      }
      BNLearner< GUM_SCALAR >& useScoreLog2Likelihood() {
        GenericBNLearner::useScoreLog2Likelihood();
        return *this;
      }
      BNLearner< GUM_SCALAR >& useNoApriori() {
        GenericBNLearner::useNoApriori();
        return *this;
      }
      BNLearner< GUM_SCALAR >& useAprioriBDeu(double weight = 1.0) {
        GenericBNLearner::useAprioriBDeu(weight);
        return *this;
      }
      BNLearner< GUM_SCALAR >& useAprioriSmoothing(double weight = 1) {
        GenericBNLearner::useAprioriSmoothing(weight);
        return *this;
      }
      BNLearner< GUM_SCALAR >& useAprioriDirichlet(const std::string& filename, double weight = 1) {
        GenericBNLearner::useAprioriDirichlet(filename, weight);
        return *this;
      }
      BNLearner< GUM_SCALAR >& useGreedyHillClimbing() {
        GenericBNLearner::useGreedyHillClimbing();
        return *this;
      }
      BNLearner< GUM_SCALAR >& useLocalSearchWithTabuList(Size tabu_size   = 100,
                                                          Size nb_decrease = 2) {
        GenericBNLearner::useLocalSearchWithTabuList(tabu_size, nb_decrease);
        return *this;
      }
      BNLearner< GUM_SCALAR >& useK2(const Sequence< NodeId >& order) {
        GenericBNLearner::useK2(order);
        return *this;
      }
      BNLearner< GUM_SCALAR >& useK2(const std::vector< NodeId >& order) {
        GenericBNLearner::useK2(order);
        return *this;
      }
      BNLearner< GUM_SCALAR >& use3off2() {
        GenericBNLearner::use3off2();
        return *this;
      }
      BNLearner< GUM_SCALAR >& useMIIC() {
        GenericBNLearner::useMIIC();
        return *this;
      }
      BNLearner< GUM_SCALAR >& useNMLCorrection() {
        GenericBNLearner::useNMLCorrection();
        return *this;
      }
      BNLearner< GUM_SCALAR >& useMDLCorrection() {
        GenericBNLearner::useMDLCorrection();
        return *this;
      }
      BNLearner< GUM_SCALAR >& useNoCorrection() {
        GenericBNLearner::useNoCorrection();
        return *this;
      }
      BNLearner< GUM_SCALAR >& setMaxIndegree(Size max_indegree) {
        GenericBNLearner::setMaxIndegree(max_indegree);
        return *this;
      }
      BNLearner< GUM_SCALAR >& setSliceOrder(const NodeProperty< NodeId >& slice_order) {
        GenericBNLearner::setSliceOrder(slice_order);
        return *this;
      }
      BNLearner< GUM_SCALAR >&
         setSliceOrder(const std::vector< std::vector< std::string > >& slices) {
        GenericBNLearner::setSliceOrder(slices);
        return *this;
      }
      BNLearner< GUM_SCALAR >& setForbiddenArcs(const ArcSet& set) {
        GenericBNLearner::setForbiddenArcs(set);
        return *this;
      }
      BNLearner< GUM_SCALAR >& addForbiddenArc(const Arc& arc) {
        GenericBNLearner::addForbiddenArc(arc);
        return *this;
      }
      BNLearner< GUM_SCALAR >& addForbiddenArc(const NodeId tail, const NodeId head) {
        GenericBNLearner::addForbiddenArc(tail, head);
        return *this;
      }
      BNLearner< GUM_SCALAR >& addForbiddenArc(const std::string& tail, const std::string& head) {
        GenericBNLearner::addForbiddenArc(tail, head);
        return *this;
      }
      BNLearner< GUM_SCALAR >& eraseForbiddenArc(const Arc& arc) {
        GenericBNLearner::eraseForbiddenArc(arc);
        return *this;
      }
      BNLearner< GUM_SCALAR >& eraseForbiddenArc(const NodeId tail, const NodeId head) {
        GenericBNLearner::eraseForbiddenArc(tail, head);
        return *this;
      }
      BNLearner< GUM_SCALAR >& eraseForbiddenArc(const std::string& tail, const std::string& head) {
        GenericBNLearner::eraseForbiddenArc(tail, head);
        return *this;
      }
      BNLearner< GUM_SCALAR >& addMandatoryArc(const Arc& arc) {
        GenericBNLearner::addMandatoryArc(arc);
        return *this;
      }
      BNLearner< GUM_SCALAR >& addMandatoryArc(const NodeId tail, const NodeId head) {
        GenericBNLearner::addMandatoryArc(tail, head);
        return *this;
      }
      BNLearner< GUM_SCALAR >& addMandatoryArc(const std::string& tail, const std::string& head) {
        GenericBNLearner::addMandatoryArc(tail, head);
        return *this;
      }
      BNLearner< GUM_SCALAR >& eraseMandatoryArc(const Arc& arc) {
        GenericBNLearner::eraseMandatoryArc(arc);
        return *this;
      }
      BNLearner< GUM_SCALAR >& eraseMandatoryArc(const NodeId tail, const NodeId head) {
        GenericBNLearner::eraseMandatoryArc(tail, head);
        return *this;
      }
      BNLearner< GUM_SCALAR >& eraseMandatoryArc(const std::string& tail, const std::string& head) {
        GenericBNLearner::eraseMandatoryArc(tail, head);
        return *this;
      }
      BNLearner< GUM_SCALAR >& addPossibleEdge(const Edge& edge) {
        GenericBNLearner::addPossibleEdge(edge);
        return *this;
      }
      BNLearner< GUM_SCALAR >& addPossibleEdge(const NodeId tail, const NodeId head) {
        GenericBNLearner::addPossibleEdge(tail, head);
        return *this;
      }
      BNLearner< GUM_SCALAR >& addPossibleEdge(const std::string& tail, const std::string& head) {
        GenericBNLearner::addPossibleEdge(tail, head);
        return *this;
      }
      BNLearner< GUM_SCALAR >& erasePossibleEdge(const Edge& edge) {
        GenericBNLearner::erasePossibleEdge(edge);
        return *this;
      }
      BNLearner< GUM_SCALAR >& erasePossibleEdge(const NodeId tail, const NodeId head) {
        GenericBNLearner::erasePossibleEdge(tail, head);
        return *this;
      }
      BNLearner< GUM_SCALAR >& erasePossibleEdge(const std::string& tail, const std::string& head) {
        GenericBNLearner::erasePossibleEdge(tail, head);
        return *this;
      }
      BNLearner< GUM_SCALAR >& setMandatoryArcs(const ArcSet& set) {
        GenericBNLearner::setMandatoryArcs(set);
        return *this;
      }
      BNLearner< GUM_SCALAR >& setPossibleEdges(const EdgeSet& set) {
        GenericBNLearner::setPossibleEdges(set);
        return *this;
      }
      BNLearner< GUM_SCALAR >& setPossibleSkeleton(const UndiGraph& skeleton) {
        GenericBNLearner::setPossibleSkeleton(skeleton);
        return *this;
      }


      private:
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
