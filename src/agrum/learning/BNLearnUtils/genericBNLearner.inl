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
 * The pack currently contains K2, GreedyHillClimbing and LocalSearchWithTabuList
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

// to help IDE parser
#include <agrum/learning/BNLearnUtils/genericBNLearner.h>

namespace gum {

  namespace learning {

    // returns the row filter
    INLINE DBRowFilter<DatabaseVectInRAM::Handler,
                       DBRowTranslatorSetDynamic<CellTranslatorCompactIntId>,
                       FilteredRowGeneratorSet<RowGeneratorIdentity>> &
    genericBNLearner::Database::rowFilter() {
      return *__row_filter;
    }

    // returns the modalities of the variables
    INLINE std::vector<unsigned int> &
    genericBNLearner::Database::modalities() noexcept {
      return __modalities;
    }

    // returns the names of the variables in the database
    INLINE const std::vector<std::string> &genericBNLearner::Database::names() const
        noexcept {
      return __database.variableNames();
    }

    // returns the node id corresponding to a variable name
    INLINE NodeId
        genericBNLearner::Database::idFromName(const std::string &var_name) const {
      try {
        return __name2nodeId.second(const_cast<std::string &>(var_name));
      } catch (gum::NotFound) {
        GUM_ERROR(MissingVariableInDatabase, "for variable " << var_name);
      }
    }

    // returns the variable name corresponding to a given node id
    INLINE const std::string &
    genericBNLearner::Database::nameFromId(NodeId id) const {
      return __name2nodeId.first(id);
    }

    // returns the "raw" translators (needed for the aprioris)
    INLINE DBRowTranslatorSetDynamic<CellTranslatorUniversal> &
    genericBNLearner::Database::rawTranslators() {
      return __raw_translators;
    }

    // ===========================================================================

    // returns the node id corresponding to a variable name
    INLINE NodeId genericBNLearner::idFromName(const std::string &var_name) const {
      return __score_database.idFromName(var_name);
    }

    // returns the variable name corresponding to a given node id
    INLINE const std::string &genericBNLearner::nameFromId(NodeId id) const {
      return __score_database.nameFromId(id);
    }

    // sets an initial DAG structure
    INLINE void genericBNLearner::setInitialDAG(const DAG &dag) {
      __initial_dag = dag;
    }

    // indicate that we wish to use an AIC score
    INLINE void genericBNLearner::useScoreAIC() {
      __score_type = ScoreType::AIC;
      __checkScoreAprioriCompatibility();
    }

    // indicate that we wish to use a BD score
    INLINE void genericBNLearner::useScoreBD() {
      __score_type = ScoreType::BD;
      __checkScoreAprioriCompatibility();
    }

    // indicate that we wish to use a BDeu score
    INLINE void genericBNLearner::useScoreBDeu() {
      __score_type = ScoreType::BDeu;
      __checkScoreAprioriCompatibility();
    }

    // indicate that we wish to use a BIC score
    INLINE void genericBNLearner::useScoreBIC() {
      __score_type = ScoreType::BIC;
      __checkScoreAprioriCompatibility();
    }

    // indicate that we wish to use a K2 score
    INLINE void genericBNLearner::useScoreK2() {
      __score_type = ScoreType::K2;
      __checkScoreAprioriCompatibility();
    }

    // indicate that we wish to use a Log2Likelihood score
    INLINE void genericBNLearner::useScoreLog2Likelihood() {
      __score_type = ScoreType::LOG2LIKELIHOOD;
      __checkScoreAprioriCompatibility();
    }

    // sets the max indegree
    INLINE void genericBNLearner::setMaxIndegree(unsigned int max_indegree) {
      __constraint_Indegree.setMaxIndegree(max_indegree);
    }

    // indicate that we wish to use a K2 algorithm
    INLINE void genericBNLearner::useK2(const Sequence<NodeId> &order) noexcept {
      __selected_algo = AlgoType::K2;
      __K2.setOrder(order);
    }

    // indicate that we wish to use a K2 algorithm
    INLINE void genericBNLearner::useK2(const std::vector<NodeId> &order) noexcept {
      __selected_algo = AlgoType::K2;
      __K2.setOrder(order);
    }

    // indicate that we wish to use a greedy hill climbing algorithm
    INLINE void genericBNLearner::useGreedyHillClimbing() noexcept {
      __selected_algo = AlgoType::GREEDY_HILL_CLIMBING;
    }

    // indicate that we wish to use a local search with tabu list
    INLINE void
    genericBNLearner::useLocalSearchWithTabuList(unsigned int tabu_size,
                                                 unsigned int nb_decrease) noexcept {
      __selected_algo = AlgoType::LOCAL_SEARCH_WITH_TABU_LIST;
      __constraint_TabuList.setTabuListSize(tabu_size);
      __local_search_with_tabu_list.setMaxNbDecreasingChanges(nb_decrease);
    }

    // assign a set of forbidden arcs
    INLINE void genericBNLearner::setForbiddenArcs(const ArcSet &set) {
      __constraint_ForbiddenArcs.setArcs(set);
    }

    // assign a new forbidden arc
    INLINE void genericBNLearner::addForbiddenArc(const Arc &arc) {
      __constraint_ForbiddenArcs.addArc(arc);
    }

    // remove a forbidden arc
    INLINE void genericBNLearner::eraseForbiddenArc(const Arc &arc) {
      __constraint_ForbiddenArcs.eraseArc(arc);
    }

    // assign a new forbidden arc
    INLINE void genericBNLearner::addForbiddenArc(const NodeId tail,
                                                  const NodeId head) {
      addForbiddenArc(Arc(tail, head));
    }

    // remove a forbidden arc
    INLINE void genericBNLearner::eraseForbiddenArc(const NodeId tail,
                                                    const NodeId head) {
      eraseForbiddenArc(Arc(tail, head));
    }

    // assign a new forbidden arc
    INLINE void genericBNLearner::addForbiddenArc(const std::string &tail,
                                                  const std::string &head) {
      addForbiddenArc(Arc(idFromName(tail), idFromName(head)));
    }

    // remove a forbidden arc
    INLINE void genericBNLearner::eraseForbiddenArc(const std::string &tail,
                                                    const std::string &head) {
      eraseForbiddenArc(Arc(idFromName(tail), idFromName(head)));
    }

    // assign a set of forbidden arcs
    INLINE void genericBNLearner::setMandatoryArcs(const ArcSet &set) {
      __constraint_MandatoryArcs.setArcs(set);
    }

    // assign a new forbidden arc
    INLINE void genericBNLearner::addMandatoryArc(const Arc &arc) {
      __constraint_MandatoryArcs.addArc(arc);
    }

    // remove a forbidden arc
    INLINE void genericBNLearner::eraseMandatoryArc(const Arc &arc) {
      __constraint_MandatoryArcs.eraseArc(arc);
    }

    // assign a new forbidden arc
    INLINE void genericBNLearner::addMandatoryArc(const std::string &tail,
                                                  const std::string &head) {
      addMandatoryArc(Arc(idFromName(tail), idFromName(head)));
    }

    // remove a forbidden arc
    INLINE void genericBNLearner::eraseMandatoryArc(const std::string &tail,
                                                    const std::string &head) {
      eraseMandatoryArc(Arc(idFromName(tail), idFromName(head)));
    }

    // assign a new forbidden arc
    INLINE void genericBNLearner::addMandatoryArc(const NodeId tail,
                                                  const NodeId head) {
      addMandatoryArc(Arc(tail, head));
    }

    // remove a forbidden arc
    INLINE void genericBNLearner::eraseMandatoryArc(const NodeId tail,
                                                    const NodeId head) {
      eraseMandatoryArc(Arc(tail, head));
    }

    // sets a partial order on the nodes
    INLINE void
    genericBNLearner::setSliceOrder(const NodeProperty<unsigned int> &slice_order) {
      __constraint_SliceOrder = slice_order;
    }

    // sets the apriori weight
    INLINE void genericBNLearner::setAprioriWeight(float weight) {
      if (weight < 0) {
        GUM_ERROR(OutOfBounds, "the weight of the apriori must be positive");
      }

      __apriori_weight = weight;
      __checkScoreAprioriCompatibility();
    }

    // use the apriori smoothing
    INLINE void genericBNLearner::useNoApriori() {
      __apriori_type = AprioriType::NO_APRIORI;
      __checkScoreAprioriCompatibility();
    }

    // use the apriori smoothing
    INLINE void genericBNLearner::useAprioriSmoothing(float weight) {
      __apriori_type = AprioriType::SMOOTHING;

      if (weight >= 0) {
        setAprioriWeight(weight);
      }

      __checkScoreAprioriCompatibility();
    }

    // use the Dirichlet apriori
    INLINE void genericBNLearner::useAprioriDirichlet(const std::string &filename) {
      __apriori_dbname = filename;
      __apriori_type = AprioriType::DIRICHLET_FROM_DATABASE;
      __checkScoreAprioriCompatibility();
    }

    // returns the type (as a string) of a given apriori
    INLINE const std::string &genericBNLearner::__getAprioriType() const {
      switch (__apriori_type) {
        case AprioriType::NO_APRIORI:
          return AprioriNoApriori<>::type::type;

        case AprioriType::SMOOTHING:
          return AprioriSmoothing<>::type::type;

        case AprioriType::DIRICHLET_FROM_DATABASE:
          return AprioriDirichletFromDatabase<>::type::type;

        default:
          GUM_ERROR(
              OperationNotAllowed,
              "genericBNLearner getAprioriType does not support yet this apriori");
      }
    }

    // returns the names of the variables in the database
    INLINE const std::vector<std::string> &genericBNLearner::names() const {
      return __score_database.names();
    }

    // returns the modalities  of the variables in the database
    INLINE const std::vector<unsigned int> &genericBNLearner::modalities() noexcept {
      return __score_database.modalities();
    }

  } /* namespace learning */

} /* namespace gum */
