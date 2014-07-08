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
#include <agrum/learning/BNLearner.h>

namespace gum {


  namespace learning {


    /// returns the row filter
    INLINE DBRowFilter< DatabaseVectInRAM::Handler,
           DBRowTranslatorSetDynamic<CellTranslatorCompactIntId>,
           FilteredRowGeneratorSet<RowGeneratorIdentity> >&
    BNLearner::Database::rowFilter () {
      return *__row_filter;
    }


    /// returns the modalities of the variables
    INLINE std::vector<unsigned int>&
    BNLearner::Database::modalities () noexcept {
      return __modalities;
    }


    /// returns the names of the variables in the database
    INLINE const std::vector<std::string>&
    BNLearner::Database::names () const noexcept {
      return __database.variableNames ();
    }

    /// returns the node id corresponding to a variable name
    INLINE NodeId
    BNLearner::Database::idFromName ( const std::string& var_name ) const {
      return __name2nodeId.second ( const_cast<std::string&> ( var_name ) );
    }


    /// returns the variable name corresponding to a given node id
    INLINE const std::string&
    BNLearner::Database::nameFromId ( NodeId id ) const {
      return __name2nodeId.first ( id );
    }


    /// returns the "raw" translators (needed for the aprioris)
    INLINE DBRowTranslatorSetDynamic<CellTranslatorUniversal>&
    BNLearner::Database::rawTranslators () {
      return __raw_translators;
    }


    // ===========================================================================


    /// returns the node id corresponding to a variable name
    INLINE NodeId
    BNLearner::idFromName ( const std::string& var_name ) const {
      return __score_database.idFromName ( var_name );
    }


    /// returns the variable name corresponding to a given node id
    INLINE const std::string&
    BNLearner::nameFromId ( NodeId id ) const {
      return __score_database.nameFromId ( id );
    }


    /// sets an initial DAG structure
    INLINE void BNLearner::setInitialDAG ( const DAG& dag ) {
      __initial_dag = dag;
    }


    /// indicate that we wish to use an AIC score
    INLINE void BNLearner::useScoreAIC() noexcept {
      __score_type = ScoreType::AIC;
    }


    /// indicate that we wish to use a BD score
    INLINE void BNLearner::useScoreBD () noexcept {
      __score_type = ScoreType::BD;
    }


    /// indicate that we wish to use a BDeu score
    INLINE void BNLearner::useScoreBDeu () noexcept {
      __score_type = ScoreType::BDeu;
    }


    /// indicate that we wish to use a BIC score
    INLINE void BNLearner::useScoreBIC() noexcept {
      __score_type = ScoreType::BIC;
    }


    /// indicate that we wish to use a K2 score
    INLINE void BNLearner::useScoreK2() noexcept {
      __score_type = ScoreType::K2;
    }


    /// indicate that we wish to use a Log2Likelihood score
    INLINE void BNLearner::useScoreLog2Likelihood() noexcept {
      __score_type = ScoreType::LOG2LIKELIHOOD;
    }


    /// sets the max indegree
    INLINE void
    BNLearner::setMaxIndegree ( unsigned int max_indegree ) {
      __constraint_Indegree.setMaxIndegree ( max_indegree );
    }


    /// indicate that we wish to use a K2 algorithm
    INLINE void BNLearner::useK2 ( const Sequence<NodeId>& order ) noexcept {
      __selected_algo = AlgoType::K2;
      __K2.setOrder ( order );
    }


    /// indicate that we wish to use a K2 algorithm
    INLINE void BNLearner::useK2 ( const std::vector<NodeId>& order ) noexcept {
      __selected_algo = AlgoType::K2;
      __K2.setOrder ( order );
    }


    /// indicate that we wish to use a greedy hill climbing algorithm
    INLINE void BNLearner::useGreedyHillClimbing() noexcept {
      __selected_algo = AlgoType::GREEDY_HILL_CLIMBING;
    }


    /// indicate that we wish to use a local search with tabu list
    INLINE void
    BNLearner::useLocalSearchWithTabuList ( unsigned int tabu_size,
    unsigned int nb_decrease ) noexcept {
      __selected_algo = AlgoType::LOCAL_SEARCH_WITH_TABU_LIST;
      __constraint_TabuList.setTabuListSize ( tabu_size );
      __local_search_with_tabu_list.setMaxNbDecreasingChanges ( nb_decrease );
    }


    /// assign a set of forbidden arcs
    INLINE void BNLearner::setForbiddenArcs ( const ArcSet& set ) {
      __constraint_ForbiddenArcs.setArcs ( set );
    }


    /// assign a new forbidden arc
    INLINE void BNLearner::addForbiddenArc ( const Arc& arc ) {
      __constraint_ForbiddenArcs.addArc ( arc );
    }


    /// remove a forbidden arc
    INLINE void BNLearner::eraseForbiddenArc ( const Arc& arc ) {
      __constraint_ForbiddenArcs.eraseArc ( arc );
    }


    /// assign a new forbidden arc
    INLINE void BNLearner::addForbiddenArc ( const NodeId tail, const NodeId head ) {
      addForbiddenArc ( Arc ( tail,head ) );
    }


    /// remove a forbidden arc
    INLINE void BNLearner::eraseForbiddenArc ( const NodeId tail, const NodeId head ) {
      eraseForbiddenArc ( Arc ( tail,head ) );
    }

    /// assign a new forbidden arc
    INLINE void BNLearner::addForbiddenArc ( const std::string& tail, const std::string& head ) {
      addForbiddenArc ( Arc ( idFromName ( tail ), idFromName ( head ) ) );
    }


    /// remove a forbidden arc
    INLINE void BNLearner::eraseForbiddenArc ( const std::string& tail, const std::string& head ) {
      eraseForbiddenArc ( Arc ( idFromName ( tail ), idFromName ( head ) ) );
    }


    /// assign a set of forbidden arcs
    INLINE void BNLearner::setMandatoryArcs ( const ArcSet& set ) {
      __constraint_MandatoryArcs.setArcs ( set );
    }


    /// assign a new forbidden arc
    INLINE void BNLearner::addMandatoryArc ( const Arc& arc ) {
      __constraint_MandatoryArcs.addArc ( arc );
    }


    /// remove a forbidden arc
    INLINE void BNLearner::eraseMandatoryArc ( const Arc& arc ) {
      __constraint_MandatoryArcs.eraseArc ( arc );
    }


    /// assign a new forbidden arc
    INLINE void BNLearner::addMandatoryArc ( const std::string& tail, const std::string& head ) {
      addMandatoryArc ( Arc ( idFromName ( tail ), idFromName ( head ) ) );
    }


    /// remove a forbidden arc
    INLINE void BNLearner::eraseMandatoryArc ( const std::string& tail, const std::string& head ) {
      eraseMandatoryArc ( Arc ( idFromName ( tail ), idFromName ( head ) ) );
    }


    /// assign a new forbidden arc
    INLINE void BNLearner::addMandatoryArc ( const NodeId tail, const NodeId head ) {
      addMandatoryArc ( Arc ( tail , head ) );
    }


    /// remove a forbidden arc
    INLINE void BNLearner::eraseMandatoryArc ( const NodeId tail, const NodeId head ) {
      eraseMandatoryArc ( Arc ( tail , head ) );
    }


    /// sets a partial order on the nodes
    INLINE void BNLearner::setSliceOrder
    ( const NodeProperty<unsigned int>& slice_order ) {
      __constraint_SliceOrder = slice_order;
    }


    /// use the apriori smoothing
    INLINE void BNLearner::useAprioriSmoothing() noexcept {
      __apriori_type = AprioriType::SMOOTHING;
    }


    /// sets the apriori weight
    INLINE void BNLearner::setAprioriWeight ( float weight ) noexcept {
      if ( weight < 0 ) {
        GUM_ERROR ( OutOfBounds, "the weight of the apriori must be positive" );
      }

      __apriori_weight = weight;
    }


    /// use the Dirichlet apriori
    INLINE void
    BNLearner::useAprioriDirichlet ( const std::string& filename ) noexcept {
      __apriori_dbname = filename;
      __apriori_type = AprioriType::DIRICHLET_FROM_DATABASE;
    }


    /// returns the names of the variables in the database
    INLINE const std::vector<std::string>& BNLearner::names () const {
      return __score_database.names ();
    }


  } /* namespace learning */


} /* namespace gum */

