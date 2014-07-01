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
#ifndef GUM_LEARNING_ALGO_PACK_H
#define GUM_LEARNING_ALGO_PACK_H


#include <agrum/graphs/DAG.h>
#include <agrum/BN/BayesNet.h>

#include <agrum/learning/database/databaseFromCSV.h>
#include <agrum/learning/database/DBCellTranslators/cellTranslatorUniversal.h>
#include <agrum/learning/database/DBCellTranslators/cellTranslatorCompactIntId.h>
#include <agrum/learning/database/DBRowTranslatorSetDynamic.h>
#include <agrum/learning/database/DBTransformCompactInt.h>
#include <agrum/learning/database/filteredRowGenerators/rowGeneratorIdentity.h>

#include <agrum/learning/scores_and_tests/scoreBIC.h>
#include <agrum/learning/scores_and_tests/scoreBDeu.h>

#include <agrum/learning/constraints/structuralConstraintDAG.h>
#include <agrum/learning/constraints/structuralConstraintIndegree.h>
#include <agrum/learning/constraints/structuralConstraint2TimeSlice.h>
#include <agrum/learning/constraints/structuralConstraintSetStatic.h>

#include <agrum/learning/structureUtils/graphChange.h>
#include <agrum/learning/structureUtils/graphChangesSelector4DiGraph.h>
#include <agrum/learning/structureUtils/graphChangesGenerator4DiGraph.h>
#include <agrum/learning/paramUtils/paramEstimatorMLwithUniformApriori.h>

#include <agrum/learning/greedyHillClimbing.h>
#include <agrum/learning/localSearchWithTabuList.h>


namespace gum {

  
  namespace learning {


    /** @class LearningAlgoPack
     * @brief A pack of learning algorithms that can easily be used
     *
     * The pack currently contains K2, GreedyHillClimbing and
     * LocalSearchWithTabuList
     * @ingroup learning_group
     */
    class LearningAlgoPack {
    public:
      
      /// an enumeration enabling to select easily the score we wish to use
      enum class ScoreType {
        SCORE_AIC,
        SCORE_BD,
        SCORE_BDEU,
        SCORE_BIC,
        SCORE_K2,
        SCORE_LOG2LIKELIHOOD };

      /// an enumeration to sealect easily the learning algorithm to use
      enum class AlgoType {
        GREEDY_HILL_CLIMBING,
        LOCAL_SEARCH_WITH_TABU_LIST
      };
      
      
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      LearningAlgoPack ();

      /// copy constructor
      LearningAlgoPack ( const LearningAlgoPack& );

      /// move constructor
      LearningAlgoPack ( LearningAlgoPack&& );

      /// destructor
      ~LearningAlgoPack ();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      LearningAlgoPack& operator= ( const LearningAlgoPack& );

      /// move operator
      LearningAlgoPack& operator= ( LearningAlgoPack&& );

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
      /// @name Accessors / Modifiers specific for LocalSearchWithTabuList
      // ##########################################################################
      /// @{

      /// set the max number of changes decreasing the score that we allow to apply
      void setMaxNbDecreasingChanges ( unsigned int nb );

      /// @}
      
 
    private:
      /// the greedy hill climbing algorith
      GreedyHillClimbing __greedy_hill_climbing;

      /// the local search with tabu list algorithm
      LocalSearchWithTabuList __local_search_with_tabu_list;

      /// an initial DAG given to learners
      DAG __initial_dag;

      /// the score selected for learning
      ScoreType __score_type { ScoreType::SCORE_BDEU };

      /// the score used
      Score<>* __score { nullptr };

      /// the constraint for 2TBNs
      StructuralConstraint2TimeSlice __constraint_2TimeSlice;

      /// the constraint for indegrees
      StructuralConstraintIndegree __constraint_Indegree;

      /// the selected learning algorithm
      AlgoType __selected_algo { AlgoType::GREEDY_HILL_CLIMBING };


      /// reads a file and returns a databaseVectInRam
      DatabaseVectInRAM __readFile ( const std::string& filename );

      /// create the score used for learning
      template <typename FILTER>
      void __createScore ( FILTER& filter,
                           std::vector<unsigned int>& modalities );

      /// returns the DAG learnt
      template <typename FILTER>
      DAG __learnDAG ( FILTER& filter, std::vector<unsigned int>& modal );

    };
    
      
  } /* namespace learning */
  
  
} /* namespace gum */


/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/learningAlgoPack.inl>
#endif /* GUM_NO_INLINE */


/// always include templated methods
#include <agrum/learning/learningAlgoPack.tcc>


#endif /* GUM_LEARNING_ALGO_PACK_H */
