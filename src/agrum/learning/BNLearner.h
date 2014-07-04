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
 * @brief A basic pack of learning algorithms that can easily be used
 *
 * The pack currently contains K2, GreedyHillClimbing and LocalSearchWithTabuList
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_BN_LEARNER_H
#define GUM_LEARNING_BN_LEARNER_H


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
#include <agrum/learning/paramUtils/paramEstimatorMLwithUniformApriori.h>
#include <agrum/learning/paramUtils/paramEstimatorML.h>

#include <agrum/BN/algorithms/approximationSchemeListener.h>
#include <agrum/BN/algorithms/approximationScheme.h>

#include <agrum/learning/K2.h>
#include <agrum/learning/greedyHillClimbing.h>
#include <agrum/learning/localSearchWithTabuList.h>

#include <agrum/core/signal/signaler.h>


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
    class BNLearner: public ApproximationScheme {
      private:

        /// an enumeration enabling to select easily the score we wish to use
        enum class ScoreType {
          AIC,
          BD,
          BDEU,
          BIC,
          K2,
          LOG2LIKELIHOOD
        };

        /// an enumeration to select the type of parameter estimation we shall apply
        enum ParamEstimatorType {
          ML,
          MLwithUniformApriori
        };

        /// an enumeration to select easily the learning algorithm to use
        enum class AlgoType {
          K2,
          GREEDY_HILL_CLIMBING,
          LOCAL_SEARCH_WITH_TABU_LIST
        };


      public:


        // ##########################################################################
        /// @name Constructors / Destructors
        // ##########################################################################
        /// @{

        /// default constructor
        BNLearner ();

        /// copy constructor
        BNLearner ( const BNLearner& );

        /// move constructor
        BNLearner ( BNLearner&& );

        /// destructor
        ~BNLearner ();

        /// @}


        // ##########################################################################
        /// @name Operators
        // ##########################################################################
        /// @{

        /// copy operator
        BNLearner& operator= ( const BNLearner& );

        /// move operator
        BNLearner& operator= ( BNLearner&& );

        /// @}


        // ##########################################################################
        /// @name Accessors / Modifiers
        // ##########################################################################
        /// @{

        /// learn a structure from a file
        DAG learnDAG ( std::string filename );

        /// learn a Bayes Net from a file
        template <typename GUM_SCALAR = float>
        BayesNet<GUM_SCALAR> learnBN ( std::string filename );

        /// sets an initial DAG structure
        void setInitialDAG ( const DAG& );

        /// @}


        // ##########################################################################
        /// @name Score selection
        // ##########################################################################
        /// @{

        /// indicate that we wish to use an AIC score
        void useScoreAIC () noexcept;

        /*
        /// indicate that we wish to use a BD score
        void useScoreBD () noexcept;

        /// indicate that we wish to use a BDeu score
        void useScoreBDeu () noexcept;
        */

        /// indicate that we wish to use a BIC score
        void useScoreBIC () noexcept;

        /// indicate that we wish to use a K2 score
        void useScoreK2 () noexcept;

        /// indicate that we wish to use a Log2Likelihood score
        void useScoreLog2Likelihood () noexcept;

        /// @}


        // ##########################################################################
        /// @name Learning algorithm selection
        // ##########################################################################
        /// @{

        /// indicate that we wish to use a greedy hill climbing algorithm
        void useGreedyHillClimbing () noexcept;

        /// indicate that we wish to use a local search with tabu list
        /** @param tabu_size indicate the size of the tabu list
         * @param nb_decrease indicate the max number of changes decreasing the
         * score consecutively that we allow to apply */
        void useLocalSearchWithTabuList ( unsigned int tabu_size = 100,
                                          unsigned int nb_decrease = 2 ) noexcept;

        /// indicate that we wish to use K2
        void useK2 ( const Sequence<NodeId>& order ) noexcept;

        /// indicate that we wish to use K2
        void useK2 ( const std::vector<NodeId>& order ) noexcept;

        /// @}


        // ##########################################################################
        /// @name Accessors / Modifiers for adding constraints on learning
        // ##########################################################################
        /// @{

        /// sets the max indegree
        void setMaxIndegree ( unsigned int max_indegree );

        /// sets a partial order on the nodes
        void setSliceOrder ( const NodeProperty<unsigned int>& slice_order );

        /// assign a set of forbidden arcs
        void setForbiddenArcs ( const ArcSet& set );

        /// assign a new forbidden arc
        void addForbiddenArc ( const Arc& arc );

        /// remove a forbidden arc
        void eraseForbiddenArc ( const Arc& arc );

        /// assign a set of forbidden arcs
        void setMandatoryArcs ( const ArcSet& set );

        /// assign a new forbidden arc
        void addMandatoryArc ( const Arc& arc );

        /// remove a forbidden arc
        void eraseMandatoryArc ( const Arc& arc );

        /// @}

        /// @name redistribute signals
        /// {@
        void distributeProgress ( Size pourcent, double error, double time );
        void distributeStop ( std::string message );
        /// @}

      private:

        /// the score selected for learning
        ScoreType __score_type { ScoreType::BIC };

        /// the score used

        Score<>* __score { nullptr };

        /// the type of the parameter estimator
        ParamEstimatorType
        __param_estimator_type { ParamEstimatorType::MLwithUniformApriori };

        /// the parameter estimator to use
        ParamEstimator<>* __param_estimator { nullptr };

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
        AlgoType __selected_algo { AlgoType::GREEDY_HILL_CLIMBING };

        /// the K2 algorithm
        K2 __K2;

        /// the greedy hill climbing algorithm
        GreedyHillClimbing __greedy_hill_climbing;

        /// the local search with tabu list algorithm
        LocalSearchWithTabuList __local_search_with_tabu_list;

        /// an initial DAG given to learners
        DAG __initial_dag;




        /// reads a file and returns a databaseVectInRam
        DatabaseVectInRAM __readFile ( const std::string& filename );

        /// create the score used for learning
        template <typename FILTER>
        void __createScore ( FILTER& filter,
                             std::vector<unsigned int>& modalities );

        /// create the parameter estimator used for learning
        template <typename FILTER>
        void __createParamEstimator ( FILTER& filter,
                                      std::vector<unsigned int>& modalities );

        /// returns the DAG learnt
        template <typename FILTER>
        DAG __learnDAG ( FILTER& filter, std::vector<unsigned int>& modal );

        BNLearnerListener* __forK2;
        BNLearnerListener* __forGreedyHillClimbing;
        BNLearnerListener* __forLocalSearch;
    };
  } /* namespace learning */
} /* namespace gum */


/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/BNLearner.inl>
#endif /* GUM_NO_INLINE */


/// always include templated methods
#include <agrum/learning/BNLearner.tcc>


#endif /* GUM_LEARNING_BN_LEARNER_H */
