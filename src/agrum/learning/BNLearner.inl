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

namespace gum {


  namespace learning {


    /// sets an initial DAG structure
    INLINE void BNLearner::setInitialDAG ( const DAG& dag ) {
      __initial_dag = dag;
    }


    /// indicate that we wish to use an AIC score
    INLINE void BNLearner::useScoreAIC () noexcept {
      __score_type = ScoreType::AIC;
    }


    /*
    /// indicate that we wish to use a BD score
    INLINE void BNLearner::useScoreBD () noexcept {
       __score_type = ScoreType::BD;
    }


    /// indicate that we wish to use a BDeu score
    INLINE void BNLearner::useScoreBDeu () noexcept;
      __score_type = ScoreType::BDeu;
    }
    */


    /// indicate that we wish to use a BIC score
    INLINE void BNLearner::useScoreBIC () noexcept {
      __score_type = ScoreType::BIC;
    }


    /// indicate that we wish to use a K2 score
    INLINE void BNLearner::useScoreK2 () noexcept {
      __score_type = ScoreType::K2Score;
    }


    /// indicate that we wish to use a Log2Likelihood score
    INLINE void BNLearner::useScoreLog2Likelihood () noexcept {
      __score_type = ScoreType::LOG2LIKELIHOOD;
    }


    /// sets the max indegree
    INLINE void
    BNLearner::setMaxIndegree ( unsigned int max_indegree ) {
      __constraint_Indegree.setMaxIndegree ( max_indegree );
    }


    /// indicate that we wish to use a greedy hill climbing algorithm
    INLINE void BNLearner::useK2 () noexcept {
      __selected_algo = AlgoType::K2;
    }


    /// indicate that we wish to use a greedy hill climbing algorithm
    INLINE void BNLearner::useGreedyHillClimbing () noexcept {
      __selected_algo = AlgoType::GREEDY_HILL_CLIMBING;
    }


    /// indicate that we wish to use a local search with tabu list
    INLINE void BNLearner::useLocalSearchWithTabuList () noexcept {
      __selected_algo = AlgoType::LOCAL_SEARCH_WITH_TABU_LIST;
    }


    /// set the max number of changes decreasing the score that we allow to apply
    INLINE void BNLearner::setMaxNbDecreasingChanges ( unsigned int nb ) {
      __local_search_with_tabu_list.setMaxNbDecreasingChanges ( nb );
    }


    /// set an ordering for K2 (by default, nodes are ordered by increasing id)
    INLINE void BNLearner::setOrder ( const Sequence<NodeId>& order ) {
      __order = order;
    }


    /// set an ordering for K2 (by default, nodes are ordered by increasing id)
    INLINE void BNLearner::setOrder ( const std::vector<NodeId>& order ) {
      __order.clear ();
      for ( const auto node : order ) {
        __order.insert ( node );
      }
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


    /// sets a partial order on the nodes
    INLINE void BNLearner::setPartialOrder
    ( const NodeProperty<unsigned int>& partial_order ) {
      __constraint_PartialOrder = partial_order;
    }


    /// sets a kTBN constraint (alias for setPartialOrder)
    INLINE void BNLearner::setkTBN
    ( const NodeProperty<unsigned int>& partial_order ) {
      __constraint_PartialOrder = partial_order;
    }


  } /* namespace learning */


} /* namespace gum */

