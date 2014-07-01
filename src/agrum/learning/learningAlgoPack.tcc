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
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  
  namespace learning {

    /// create the score used for learning
    template <typename FILTER>
    void
    LearningAlgoPack::__createScore ( FILTER& filter,
                                      std::vector<unsigned int>& modalities ) {
      __score = new ScoreBIC<> ( filter, modalities );
    }
    
      
    /// learn a structure from a file
    template <typename FILTER>
    DAG LearningAlgoPack::__learnDAG ( FILTER& filter,
                                       std::vector<unsigned int>& modal ) {
      switch ( __selected_algo ) {
      case AlgoType::GREEDY_HILL_CLIMBING:
        {
          StructuralConstraintSetStatic<StructuralConstraint2TimeSlice,
                                        StructuralConstraintIndegree,
                                        StructuralConstraintDAG> constraint;
          static_cast<StructuralConstraint2TimeSlice> ( constraint ) =
            __constraint_2TimeSlice;
          static_cast<StructuralConstraintIndegree> ( constraint ) =
            __constraint_Indegree;
                         
          GraphChangesGenerator4DiGraph< decltype ( constraint ) >
            op_set ( constraint );
        
          GraphChangesSelector4DiGraph< Score<>,
                                        decltype ( constraint ),
                                        decltype ( op_set ) >
            selector ( *__score, constraint, op_set );

          return __greedy_hill_climbing.learnStructure ( selector, modal,
                                                         __initial_dag );
        }
        
      case AlgoType::LOCAL_SEARCH_WITH_TABU_LIST:
        {
          StructuralConstraintSetStatic<StructuralConstraint2TimeSlice,
                                        StructuralConstraintIndegree,
                                        StructuralConstraintDAG> constraint;
          static_cast<StructuralConstraint2TimeSlice> ( constraint ) =
            __constraint_2TimeSlice;
          static_cast<StructuralConstraintIndegree> ( constraint ) =
            __constraint_Indegree;
          
          GraphChangesGenerator4DiGraph< decltype ( constraint ) >
            op_set ( constraint );
          
          GraphChangesSelector4DiGraph< Score<>,
                                        decltype ( constraint ),
                                        decltype ( op_set ) >
            selector ( *__score, constraint, op_set );
          
          return __local_search_with_tabu_list.learnStructure ( selector, modal,
                                                                __initial_dag );
        }

      default:
        GUM_ERROR ( OperationNotAllowed,
                    "the learnDAG method has not been implemented for this "
                    "learning algorithm" );
      }
    }
      
    /// learn a Bayes Net from a file
    template <typename GUM_SCALAR>
    BayesNet<GUM_SCALAR> LearningAlgoPack::learnBN ( std::string filename ) {
      return BayesNet<GUM_SCALAR> ();
    }


  } /* namespace learning */
  
  
} /* namespace gum */

#endif /*  */

