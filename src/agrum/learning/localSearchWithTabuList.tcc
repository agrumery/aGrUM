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
 * @brief The local search with tabu list learning algorithm (for directed graphs)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/core/set.h>
#include <agrum/core/priorityQueue.h>
#include <agrum/learning/DAG2BNLearner.h>


namespace gum {

  
  namespace learning {



    /// learns the structure of a Bayes net
    template <typename SCORE,
              typename STRUCT_CONSTRAINT,
              template <typename> class GRAPH_CHANGES_GENERATOR>
    DAG LocalSearchWithTabuList::learnStructure
    ( GraphChangesSelector<SCORE,STRUCT_CONSTRAINT,GRAPH_CHANGES_GENERATOR>&
      selector,
      const std::vector<unsigned int>& modal,
      unsigned int N,
      DAG dag ) {
      selector.setGraph ( dag, modal );
      
      unsigned int nb_changes_applied;
      bool are_changes_applied_yet;
      unsigned int applied_change_with_positive_score;
      unsigned int current_N = 0;

      // a vector that indicates which queues have valid scores, i.e., scores
      // that were not invalidated by previously applied changes
      std::vector<bool> impacted_queues ( dag.size (), false );

      while ( current_N < N ) {
        are_changes_applied_yet = false;
        applied_change_with_positive_score = 0;
        
        while ( ! are_changes_applied_yet || nb_changes_applied ) {
          nb_changes_applied = 0;

          std::vector< std::pair<unsigned int,float> > ordered_queues =
            selector.nodesSortedByBestScore ();
         
          for ( unsigned int j = 0; j < dag.size (); ++j ) {
            unsigned int i = ordered_queues[j].first;
            if ( ! selector.empty ( i ) &&
                 ( ! are_changes_applied_yet ||
                   ( selector.bestScore ( i ) > 0 ) ) ) {
              // pick up the best change
              const GraphChange& change = selector.bestChange ( i );

              // std::cout << "change = " << change.type () << " "
              //           << change.node1 () << " " << change.node2 ()
              //           << "  " << selector.bestScore ( i ) << std::endl;


              // perform the change
              switch ( change.type () ) {
              case GraphChangeType::ARC_ADDITION:
                if ( ! impacted_queues[ change.node2 () ] &&
                     selector.isChangeValid ( change ) ) {
                  if ( selector.bestScore ( i ) > 0 )
                    ++applied_change_with_positive_score;
                  dag.insertArc ( change.node1 (), change.node2 () );
                  impacted_queues[ change.node2 () ] = true;
                  selector.applyChangeWithoutScoreUpdate ( change );
                  ++nb_changes_applied;
                  are_changes_applied_yet = true;
                }
                else { std::cout << std::endl; }
                break;
            
              case GraphChangeType::ARC_DELETION:
                if ( ! impacted_queues[ change.node2 () ] &&
                     selector.isChangeValid ( change ) ) {
                  if ( selector.bestScore ( i ) > 0 )
                    ++applied_change_with_positive_score;
                  dag.eraseArc ( Arc ( change.node1 (), change.node2 () ) );
                  impacted_queues[ change.node2 () ] = true;
                  selector.applyChangeWithoutScoreUpdate ( change );
                  ++nb_changes_applied;
                  are_changes_applied_yet = true;
                }
                else { std::cout << std::endl; }
                break;
            
              case GraphChangeType::ARC_REVERSAL:
                if ( ( ! impacted_queues[ change.node1 () ] ) &&
                     ( ! impacted_queues[ change.node2 () ] ) &&
                     selector.isChangeValid ( change ) ) {
                  if ( selector.bestScore ( i ) > 0 )
                    ++applied_change_with_positive_score;
                  dag.eraseArc ( Arc ( change.node1 (), change.node2 () ) );
                  dag.insertArc ( change.node2 (), change.node1 () );
                  impacted_queues[ change.node1 () ] = true;
                  impacted_queues[ change.node2 () ] = true;
                  selector.applyChangeWithoutScoreUpdate ( change );
                  ++nb_changes_applied;
                  are_changes_applied_yet = true;
                }
                else { std::cout << std::endl; }
                break;
                
              default:
                GUM_ERROR ( OperationNotAllowed,
                          "edge modifications are not supported by local search" );
              }
            }
          }
          
          selector.updateScoresAfterAppliedChanges ();

          // reset the impacted queue and applied changes structures
          for ( auto iter = impacted_queues.begin ();
                iter != impacted_queues.end (); ++iter )
            *iter = false;

          // stop the loop if we have found no change to apply
          if ( ! nb_changes_applied )
            are_changes_applied_yet = true;
        }

        // update current_N
        if ( applied_change_with_positive_score ) {
          current_N = 0;
        }
        else {
          ++current_N;
        }

        //std::cout << "current N = " << current_N << std::endl;
      }
   
      return dag;
    }
      
    
    /// learns the structure and the parameters of a BN
    template <typename SCORE,
              typename STRUCT_CONSTRAINT,
              template <typename> class GRAPH_CHANGES_GENERATOR,
              typename PARAM_ESTIMATOR>
    BayesNet<float>
    LocalSearchWithTabuList::learnBN
    ( GraphChangesSelector<SCORE,STRUCT_CONSTRAINT,GRAPH_CHANGES_GENERATOR>&
      selector,
      PARAM_ESTIMATOR& estimator,
      const std::vector<std::string>& names,
      const std::vector<unsigned int>& modal,
      unsigned int N,
      DAG initial_dag ) {
      return DAG2BNLearner::createBN ( estimator,
                                       learnStructure ( selector, modal, N,
                                                        initial_dag ),
                                       names, modal );
    }
 

  } /* namespace learning */
  
  
} /* namespace gum */

