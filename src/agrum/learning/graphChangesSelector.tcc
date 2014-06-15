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
 * @brief The mecanism to compute the next available graph changes for learning
 * search algorithms.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS


#include <limits>


namespace gum {

  
  namespace learning {

    
    /// default constructor
    template <typename SCORE,
              typename STRUCTURAL_CONSTRAINT,
              template <typename> class GRAPH_CHANGES_GENERATOR> INLINE
    GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,
                         GRAPH_CHANGES_GENERATOR>::GraphChangesSelector
    ( SCORE& score,
      STRUCTURAL_CONSTRAINT& constraint,
      GRAPH_CHANGES_GENERATOR<STRUCTURAL_CONSTRAINT>& changes_generator ) :
      __score ( &score ),
      __constraint ( &constraint ),
      __changes_generator ( &changes_generator ) {
      //GUM_CONSTRUCTOR ( learning::GraphChangesSelector );
    }


    /// copy constructor
    template <typename SCORE,
              typename STRUCTURAL_CONSTRAINT,
              template <typename> class GRAPH_CHANGES_GENERATOR>
    GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,
                         GRAPH_CHANGES_GENERATOR>::GraphChangesSelector
    ( const GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,
                                 GRAPH_CHANGES_GENERATOR>& from ) :
      __score                 ( from.__score ),
      __constraint            ( from.__constraint ),
      __changes_generator     ( from.__changes_generator ),
      __graph                 ( from.__graph ),
      __changes               ( from.__changes ),
      __change_scores         ( from.__change_scores ),
      __change_queue_per_node ( from.__change_queue_per_node ),
      __node_queue            ( from.__node_queue ),
      __illegal_changes       ( from.__illegal_changes ),
      __node_current_scores   ( from.__node_current_scores ),
      __parents               ( from.__parents ),
      __queues_valid          ( from.__queues_valid ) {
      // for debugging
      //GUM_CONS_CPY ( learning::GraphChangesSelector );
    }


    /// move constructor
    template <typename SCORE,
              typename STRUCTURAL_CONSTRAINT,
              template <typename> class GRAPH_CHANGES_GENERATOR>
    GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,
                         GRAPH_CHANGES_GENERATOR>::GraphChangesSelector
    ( GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,
                           GRAPH_CHANGES_GENERATOR>&& from ) :
      __score                 ( std::move ( from.__score ) ),
      //__constraint            ( std::move ( from.__constraint ) ),
      __constraint            ( from.__constraint ),
      __changes_generator     ( std::move ( from.__changes_generator ) ),
      __graph                 ( std::move ( from.__graph ) ),
      __changes               ( std::move ( from.__changes ) ),
      __change_scores         ( std::move ( from.__change_scores ) ),
      __change_queue_per_node ( std::move ( from.__change_queue_per_node ) ),
      __node_queue            ( std::move ( from.__node_queue ) ),
      __illegal_changes       ( std::move ( from.__illegal_changes ) ),
      __node_current_scores   ( std::move ( from.__node_current_scores ) ),
      __parents               ( std::move ( from.__parents ) ),
      __queues_valid          ( std::move ( from.__queues_valid ) ) {
      // for debugging
      //GUM_CONS_MOV ( learning::GraphChangesSelector );
    }


    /// destructor
    template <typename SCORE,
              typename STRUCTURAL_CONSTRAINT,
              template <typename> class GRAPH_CHANGES_GENERATOR> INLINE
    GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,
                         GRAPH_CHANGES_GENERATOR>::~GraphChangesSelector () {
      GUM_DESTRUCTOR ( learning::GraphChangesSelector );
    }


    /// copy operator
    template <typename SCORE,
              typename STRUCTURAL_CONSTRAINT,
              template <typename> class GRAPH_CHANGES_GENERATOR>
    GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,GRAPH_CHANGES_GENERATOR>&
    GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,GRAPH_CHANGES_GENERATOR>::
    operator=  ( const GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,
                                             GRAPH_CHANGES_GENERATOR>& from ) {
      if ( this != &from ) {
        __score                 = from.__score;
        __constraint            = from.__constraint;
        __changes_generator     = from.__changes_generator;
        __graph                 = from.__graph;
        __changes               = from.__changes;
        __change_scores         = from.__change_scores;
        __change_queue_per_node = from.__change_queue_per_node;
        __node_queue            = from.__node_queue;
        __illegal_changes       = from.__illegal_changes;
        __node_current_scores   = from.__node_current_scores;
        __parents               = from.__parents;
        __queues_valid          = from.__queues_valid;
      }

      return *this;
    }

    
    /// move operator
    template <typename SCORE,
              typename STRUCTURAL_CONSTRAINT,
              template <typename> class GRAPH_CHANGES_GENERATOR>
    GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,GRAPH_CHANGES_GENERATOR>&
    GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,GRAPH_CHANGES_GENERATOR>::
    operator= ( GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,
                                      GRAPH_CHANGES_GENERATOR>&& from ) {
      if ( this != &from ) {
        __score                 = std::move ( from.__score );
        //__constraint            = std::move ( from.__constraint );
        __constraint            = from.__constraint;
        __changes_generator     = std::move ( from.__changes_generator );
        __graph                 = std::move ( from.__graph );
        __changes               = std::move ( from.__changes );
        __change_scores         = std::move ( from.__change_scores );
        __change_queue_per_node = std::move ( from.__change_queue_per_node );
        __node_queue            = std::move ( from.__node_queue );
        __illegal_changes       = std::move ( from.__illegal_changes );
        __node_current_scores   = std::move ( from.__node_current_scores );
        __parents               = std::move ( from.__parents );
        __queues_valid          = std::move ( from.__queues_valid );
       }

      return *this;
    }


    /// prepare the next score computations
    template <typename SCORE,
              typename STRUCTURAL_CONSTRAINT,
              template <typename> class GRAPH_CHANGES_GENERATOR> INLINE
    void
    GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,GRAPH_CHANGES_GENERATOR>::
    __addScoreToCompute ( unsigned int change_index ) const {
      const GraphChange& change = __changes[change_index];
      
      switch ( change.type () ) {
      case GraphChangeType::ARC_ADDITION:
        {
          std::vector<unsigned int> parents = __parents[ change.node2 () ];
          parents.push_back ( change.node1 () );
          __score->addNodeSet ( change.node2 (), std::move ( parents ) );
        }
        break;
        
      case GraphChangeType::ARC_REVERSAL:
        // remove arc ( node1 -> node2 )
        {
          std::vector<unsigned int> parents = __parents[ change.node2 () ];
          for ( auto& par : parents ) {
            if ( par == change.node1 () ) {
              par = *( parents.rbegin () );
              parents.pop_back ();
              break;
            }
          }
          __score->addNodeSet ( change.node2 (), std::move ( parents ) );
        }
              
        // add narc node2 -> node1
        {
          std::vector<unsigned int> parents = __parents[ change.node1 () ];
          parents.push_back ( change.node2 () );
          __score->addNodeSet ( change.node1 (), std::move ( parents ) );
        }
        break;

      case GraphChangeType::ARC_DELETION:
        {
          std::vector<unsigned int> parents = __parents[ change.node2 () ];
          for ( auto& par : parents ) {
            if ( par == change.node1 () ) {
              par = *( parents.rbegin () );
              parents.pop_back ();
              break;
            }
          }
          __score->addNodeSet ( change.node2 (), std::move ( parents ) );
        }
        break;
            
      default:
        GUM_ERROR ( OperationNotAllowed, "edge modifications are not "
                    "supported by local search" );
      }
    }

    
    /// indicates whether a given change is valid or not
    template <typename SCORE,
              typename STRUCTURAL_CONSTRAINT,
              template <typename> class GRAPH_CHANGES_GENERATOR> INLINE
    bool
    GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,GRAPH_CHANGES_GENERATOR>::
    __isChangeValid ( unsigned int index ) const {
      return __constraint->checkModification ( __changes[index] );
    }


    /// indicates whether a given change is valid or not
    template <typename SCORE,
              typename STRUCTURAL_CONSTRAINT,
              template <typename> class GRAPH_CHANGES_GENERATOR> INLINE
    bool
    GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,GRAPH_CHANGES_GENERATOR>::
    isChangeValid ( const GraphChange& change ) const {
      return __constraint->checkModification ( change );
    }

    
    /// sets the graph from which scores are computed
    template <typename SCORE,
              typename STRUCTURAL_CONSTRAINT,
              template <typename> class GRAPH_CHANGES_GENERATOR>
    void
    GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,GRAPH_CHANGES_GENERATOR>::
    setGraph ( DiGraph& graph,
               const std::vector<unsigned int>& modal ) {
      // ok, assign the correct graph
      __graph = &graph;

      // fill the DAG with all the missing nodes
      unsigned int nb_nodes = modal.size ();
      for ( unsigned int i = 0; i < nb_nodes; ++i ) {
        if ( ! graph.existsNode ( i ) ) {
          graph.insertNode ( i );
        }
      }

      __constraint->setGraph ( graph );
      __changes_generator->setGraph ( graph );

      // save the set of parents of each node (this will speed-up the
      // computations of the scores)
      __parents.clear ();
      __parents.resize ( nb_nodes );
      for ( unsigned int i = 0; i < nb_nodes; ++i ) {
        const NodeSet& dag_parents = __graph->parents ( i );
        if ( dag_parents.size () ) {
          __parents[i].resize ( dag_parents.size () );
          unsigned int j = 0;
          for ( const auto par : dag_parents ) {
            __parents[i][j] = par;
            ++j;
          }
        }
      }
        
      // assign a score to each node given its parents in the current graph
      __score->clear ();
      __node_current_scores.resize ( nb_nodes );
      for ( unsigned int i = 0; i < nb_nodes; ++i ) {
        __score->addNodeSet ( i, __parents[i] );
      }
      for ( unsigned int i = 0; i < nb_nodes; ++i ) {
        __node_current_scores[i] = __score->score ( i );
      }

      // compute all the possible changes
      __changes.clear ();
      __changes.resize ( nb_nodes );
      for ( const auto& change : *__changes_generator ) {
        __changes << change;
      }
      __changes_generator->notifyGetCompleted ();

      // determine the changes that are illegal and prepare the computation of
      // the scores of all the legal changes
      __illegal_changes.clear ();
      __score->clear ();
      for ( unsigned int i = 0; i < __changes.size (); ++i ) {
        if ( ! __isChangeValid ( i ) ) {
          __illegal_changes.insert ( i );
        }
        else {
          __addScoreToCompute ( i );
        }
      }

      // set the __change_scores and __change_queue_per_node for legal changes
      __change_scores.resize ( __changes.size (), std::pair<float,float>
                               ( std::numeric_limits<float>::min (),
                                 std::numeric_limits<float>::min () ) );
      __change_queue_per_node.clear ();
      __change_queue_per_node.resize ( nb_nodes );
      for ( unsigned int i = 0, j = 0; i < __changes.size (); ++i ) {
        if ( ! __illegal_changes.exists ( i ) ) {
          const GraphChange& change = __changes[i];

          if ( change.type () == GraphChangeType::ARC_REVERSAL ) {
            float delta2 =
              __score->score ( j ) - __node_current_scores[ change.node2() ];
            ++j;
            float delta1 =
              __score->score ( j ) - __node_current_scores[ change.node1() ];
            float delta  = delta1 + delta2;
            __change_scores[i].first  = delta1;
            __change_scores[i].second = delta2;
            __change_queue_per_node[change.node1 ()].insert ( i, delta );
            __change_queue_per_node[change.node2 ()].insert ( i, delta );
          }
          else {
            float delta =
              __score->score ( j ) - __node_current_scores[ change.node2() ];
            __change_scores[i].second = delta;
            __change_queue_per_node[change.node2 ()].insert ( i, delta );
          }

          ++j;
        }
      }
      __score->clear ();

      // update the global queue
      __node_queue.clear ();
      for ( unsigned int i = 0; i < nb_nodes; ++i ) {
        __node_queue.insert ( i, __change_queue_per_node[i].empty () ?
                              std::numeric_limits<float>::min () :
                              __change_queue_per_node[i].topPriority () );
      }
      __queues_valid = true;
    }


    /// put a change into the illegal set
    template <typename SCORE,
              typename STRUCTURAL_CONSTRAINT,
              template <typename> class GRAPH_CHANGES_GENERATOR> INLINE
    void
    GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,GRAPH_CHANGES_GENERATOR>::
    __invalidateChange ( unsigned int change_index ) {
      const GraphChange& change = __changes[change_index];
      if ( change.type () == GraphChangeType::ARC_REVERSAL ) {
        // remove the tail change from its priority queue
        PriorityQueue<unsigned int,float,std::greater<float> >&
          queue1 = __change_queue_per_node[change.node1 ()];
        queue1.erase ( change_index );

        // recompute the top priority for the changes of the head
        const float new_priority = queue1.empty () ?
          std::numeric_limits<float>::min () :
          queue1.topPriority ();
        __node_queue.setPriority ( change.node1 (), new_priority );
      }

      // remove the head change from its priority queue
      PriorityQueue<unsigned int,float,std::greater<float> >&
        queue2 = __change_queue_per_node[change.node2 ()];
      queue2.erase ( change_index );

      // recompute the top priority for the changes of the head
      const float new_priority = queue2.empty () ?
        std::numeric_limits<float>::min () :
        queue2.topPriority ();
      __node_queue.setPriority ( change.node2 (), new_priority );

      // put the change into the illegal set
      __illegal_changes.insert ( change_index );
    }
    
      
    /// indicates whether the selector still contain graph changes 
    template <typename SCORE,
              typename STRUCTURAL_CONSTRAINT,
              template <typename> class GRAPH_CHANGES_GENERATOR> INLINE
    bool
    GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,GRAPH_CHANGES_GENERATOR>::
    empty () {
      // put into the illegal change set all the top elements of the different
      // queues that are not valid anymore
      if ( ! __queues_valid ) {
        for ( auto& queue : __change_queue_per_node ) {
          while ( ! queue.empty () &&
                  ( queue.topPriority () !=
                    std::numeric_limits<float>::min () ) &&
                  ! __isChangeValid ( queue.top () ) ) {
            __invalidateChange ( queue.top() );
          }
        }
        __queues_valid = true;
      }
      
      return __node_queue.topPriority () == std::numeric_limits<float>::min ();
    }


    /// indicates whether the selector still contain graph changes in the ith queue
    template <typename SCORE,
              typename STRUCTURAL_CONSTRAINT,
              template <typename> class GRAPH_CHANGES_GENERATOR> INLINE
    bool
    GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,GRAPH_CHANGES_GENERATOR>::
    empty ( unsigned int i ) {
      // put into the illegal change set all the top elements of the different
      // queues that are not valid anymore
      if ( ! __queues_valid ) {
        for ( auto& queue : __change_queue_per_node ) {
          while ( ! queue.empty () &&
                  ( queue.topPriority () !=
                    std::numeric_limits<float>::min () ) &&
                  ! __isChangeValid ( queue.top () ) ) {
            __invalidateChange ( queue.top() );
          }
        }
        __queues_valid = true;
      }
      
      return __change_queue_per_node[i].empty ();
    }


    /// returns the best graph change to examine
    template <typename SCORE,
              typename STRUCTURAL_CONSTRAINT,
              template <typename> class GRAPH_CHANGES_GENERATOR> INLINE
    const GraphChange&
    GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,GRAPH_CHANGES_GENERATOR>::
    bestChange () {
      if ( ! empty () )
        return __changes[__change_queue_per_node[__node_queue.top ()].top ()];
      else
        GUM_ERROR ( NotFound, "there exists no graph change applicable" );
    }
    

    /// returns the best graph change to examine in the ith queue
    template <typename SCORE,
              typename STRUCTURAL_CONSTRAINT,
              template <typename> class GRAPH_CHANGES_GENERATOR> INLINE
    const GraphChange&
    GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,GRAPH_CHANGES_GENERATOR>::
    bestChange ( unsigned int i ) {
      GUM_ASSERT ( i < __change_queue_per_node.size () );
      if ( ! empty ( i ) )
        return __changes[ __change_queue_per_node[i].top () ];
      else
        GUM_ERROR ( NotFound, "there exists no graph change applicable" );
    }
    

    /// return the score of the best graph change
    template <typename SCORE,
              typename STRUCTURAL_CONSTRAINT,
              template <typename> class GRAPH_CHANGES_GENERATOR> INLINE
    float
    GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,GRAPH_CHANGES_GENERATOR>::
    bestScore () {
      if ( ! empty () ) 
        return __change_queue_per_node[__node_queue.top ()].topPriority ();
      else
        GUM_ERROR ( NotFound, "there exists no graph change applicable" );
    }


    /// return the score of the best graph change in the ith queue
    template <typename SCORE,
              typename STRUCTURAL_CONSTRAINT,
              template <typename> class GRAPH_CHANGES_GENERATOR> INLINE
    float
    GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,GRAPH_CHANGES_GENERATOR>::
    bestScore ( unsigned int i ) {
      GUM_ASSERT ( i < __change_queue_per_node.size () );
      if ( ! empty ( i ) ) 
        return __change_queue_per_node[i].topPriority ();
      else
        GUM_ERROR ( NotFound, "there exists no graph change applicable" );
    }
    

    /// remove the now legal changes from the illegal set
    template <typename SCORE,
              typename STRUCTURAL_CONSTRAINT,
              template <typename> class GRAPH_CHANGES_GENERATOR> INLINE
    void
    GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,GRAPH_CHANGES_GENERATOR>::
    __illegal2LegalChanges ( Set<unsigned int>& changes_to_recompute ) {
      for ( auto iter = __illegal_changes.beginSafe ();
            iter != __illegal_changes.endSafe (); ++iter ) {
        if ( __isChangeValid ( *iter ) ) {
          const GraphChange& change = __changes[*iter];
          if ( change.type () == GraphChangeType::ARC_REVERSAL ) {
            __change_queue_per_node[change.node1()].insert
              ( *iter, std::numeric_limits<float>::min () );
          }
          __change_queue_per_node[change.node2()].insert
            ( *iter, std::numeric_limits<float>::min () );

          changes_to_recompute.insert ( *iter );
          __illegal_changes.erase ( iter );
        }
      }
    }


    /// finds the changes that are affected by a given node modification
    template <typename SCORE,
              typename STRUCTURAL_CONSTRAINT,
              template <typename> class GRAPH_CHANGES_GENERATOR> INLINE
    void
    GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,GRAPH_CHANGES_GENERATOR>::
    __findLegalChangesNeedingUpdate ( Set<unsigned int>& changes_to_recompute,
                                      NodeId target_node ) {
      const HashTable<unsigned int,Size>&
        changes = __change_queue_per_node[ target_node ].allValues ();
      for ( auto iter = changes.cbegin (); iter != changes.cend (); ++iter ) {
        if ( !  changes_to_recompute.exists ( iter.key () ) ) {
          if ( __isChangeValid ( iter.key () ) ) {
            changes_to_recompute.insert ( iter.key () );
          }
          else {
            __invalidateChange ( iter.key () );
          }
        }
      }
    }


    /// perform the necessary updates of the scores
    template <typename SCORE,
              typename STRUCTURAL_CONSTRAINT,
              template <typename> class GRAPH_CHANGES_GENERATOR> INLINE
    void
    GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,GRAPH_CHANGES_GENERATOR>::
    __updateScores ( const Set<unsigned int>& changes_to_recompute ) {
      // prepare the computations of the cores
      __score->clear ();
      for ( const auto change_index : changes_to_recompute ) {
        __addScoreToCompute ( change_index );
      }

      // update the scores
      unsigned int j = 0;
      Set<unsigned int> modified_nodes ( changes_to_recompute.size () );
      for ( auto change_index : changes_to_recompute ) {
        const GraphChange& change = __changes[change_index];
        if ( change.type () == GraphChangeType::ARC_REVERSAL ) {
           // get the scores of both the tail and the head
          const float delta2 =
            __score->score ( j ) - __node_current_scores[ change.node2() ];
          ++j;
          const float delta1 =
            __score->score ( j ) - __node_current_scores[ change.node1() ];
          const float delta  = delta1 + delta2;

          // update the scores 
          __change_scores[change_index].first  = delta1;
          __change_scores[change_index].second = delta2;

          //update the queues
          __change_queue_per_node[change.node1 ()].setPriority
            ( change_index, delta );
          __change_queue_per_node[change.node2 ()].setPriority
            ( change_index, delta );

          // indicate which queues were modified
          modified_nodes.insert ( change.node1 () );
          modified_nodes.insert ( change.node2 () );
        }
        else {
          // get the score of the head
          const float delta =
            __score->score ( j ) - __node_current_scores[ change.node2() ];

          // update the score
          __change_scores[change_index].second = delta;
 
          // update the head queue
          __change_queue_per_node[change.node2 ()].setPriority
            ( change_index, delta );

          // indicate which queue was modified
          modified_nodes.insert ( change.node2 () );
       }

        ++j;
      }
 
      // update the node queue
      for ( auto node : modified_nodes ) {
        __node_queue.setPriority
          ( node, __change_queue_per_node[node].empty () ?
            std::numeric_limits<float>::min () :
            __change_queue_per_node[node].topPriority () );
      }
    }


    /// get from the graph change generator a new set of changes
    template <typename SCORE,
              typename STRUCTURAL_CONSTRAINT,
              template <typename> class GRAPH_CHANGES_GENERATOR> INLINE
    void
    GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,GRAPH_CHANGES_GENERATOR>::
    __getNewChanges () {
      // ask the graph change generator for all its available changes
      for ( const auto& change : *__changes_generator ) {
        // check that the change does not already exist
        if ( ! __changes.exists ( change ) ) {
          // add the new change. To make the addition simple, we put the new
          // change into the illegal changes set. Afterwards, the applyChange
          // function will put the legal changes again in the queues
          __illegal_changes.insert ( __changes.size () );
          __changes << change;
          __change_scores.push_back ( std::pair<float,float>
                                      ( std::numeric_limits<float>::min (),
                                        std::numeric_limits<float>::min () ) );
        }
      }

      // indicate to the generator that we have finished retrieving its changes
      __changes_generator->notifyGetCompleted ();
    }

    
    /// indicate to the selector that its best score has been applied
    template <typename SCORE,
              typename STRUCTURAL_CONSTRAINT,
              template <typename> class GRAPH_CHANGES_GENERATOR> INLINE
    void
    GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,GRAPH_CHANGES_GENERATOR>::
    applyChange ( const GraphChange& change ) {
      // first, we get the index of the change
      unsigned change_index = __changes.pos ( change );

      // perform the change
      Set<unsigned int> changes_to_recompute;
      switch ( change.type () ) {
      case GraphChangeType::ARC_ADDITION:
        {
          // update the current score
          __node_current_scores[change.node2 ()] +=
            __change_scores[change_index].second;
          __parents[change.node2 ()].push_back ( change.node1 () );

          // inform the constraint that the graph has been modified
          __constraint->modifyGraph ( static_cast<const ArcAddition&> ( change ) );

          // get new possible changes from the graph change generator
          // warning: put the next 3 lines before calling __illegal2LegalChanges
          __changes_generator->modifyGraph
            ( static_cast<const ArcAddition&> ( change ) );
          __getNewChanges ();
          
          // check whether some illegal changes can be put into the valid queues
          __illegal2LegalChanges ( changes_to_recompute );
          __invalidateChange ( change_index );
          __findLegalChangesNeedingUpdate ( changes_to_recompute, change.node2() );
          __updateScores ( changes_to_recompute );
        }
        break;

      case GraphChangeType::ARC_DELETION:
        {
          // update the current score
          __node_current_scores[change.node2 ()] +=
            __change_scores[change_index].second;
          std::vector<unsigned int>& parents = __parents[ change.node2 () ];
          for ( auto& par : parents ) {
            if ( par == change.node1 () ) {
              par = *( parents.rbegin () );
              parents.pop_back ();
              break;
            }
          }
          
          // inform the constraint that the graph has been modified
          __constraint->modifyGraph ( static_cast<const ArcDeletion&> ( change ) );

          // get new possible changes from the graph change generator
          // warning: put the next 3 lines before calling __illegal2LegalChanges
          __changes_generator->modifyGraph
            ( static_cast<const ArcDeletion&> ( change ) );
          __getNewChanges ();
          
          // check whether some illegal changes can be put into the valid queues
          __illegal2LegalChanges ( changes_to_recompute );
          __invalidateChange ( change_index );
          __findLegalChangesNeedingUpdate ( changes_to_recompute, change.node2() );
          __updateScores ( changes_to_recompute );
        }
        break;

      case GraphChangeType::ARC_REVERSAL:
        {
          // update the current score
          __node_current_scores[change.node1 ()] +=
            __change_scores[change_index].first;
          __node_current_scores[change.node2 ()] +=
            __change_scores[change_index].second;
          __parents[change.node1 ()].push_back ( change.node2 () );
          std::vector<unsigned int>& parents = __parents[ change.node2 () ];
          for ( auto& par : parents ) {
            if ( par == change.node1 () ) {
              par = *( parents.rbegin () );
              parents.pop_back ();
              break;
            }
          }

          // inform the constraint that the graph has been modified
          __constraint->modifyGraph ( static_cast<const ArcReversal&> ( change ) );

          // get new possible changes from the graph change generator
          // warning: put the next 3 lines before calling __illegal2LegalChanges
          __changes_generator->modifyGraph
            ( static_cast<const ArcReversal&> ( change ) );
          __getNewChanges ();
          
          // check whether some illegal changes can be put into the valid queues
          __illegal2LegalChanges ( changes_to_recompute );
          __invalidateChange ( change_index );
          __findLegalChangesNeedingUpdate ( changes_to_recompute, change.node1() );
          __findLegalChangesNeedingUpdate ( changes_to_recompute, change.node2() );
          __updateScores ( changes_to_recompute );
        }
        break;

      default:
        GUM_ERROR ( NotFound, "not implemented" );
      }

      __queues_valid = false;
    }

    
    /// applies several changes at a time
    template <typename SCORE,
              typename STRUCTURAL_CONSTRAINT,
              template <typename> class GRAPH_CHANGES_GENERATOR> INLINE
    void
    GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,GRAPH_CHANGES_GENERATOR>::
    applyChangeWithoutScoreUpdate ( const GraphChange& change ) {
      // create a set that will help determining which queues need be updated
      //Set<NodeId> queues_to_update ( 2 * changes.size () );

      // first, we get the index of the change
      unsigned change_index = __changes.pos ( change );

      // perform the change
      switch ( change.type () ) {
      case GraphChangeType::ARC_ADDITION:
        {
          // update the current score
          __node_current_scores[change.node2 ()] +=
            __change_scores[change_index].second;
          __parents[change.node2 ()].push_back ( change.node1 () );

          // inform the constraint that the graph has been modified
          __constraint->modifyGraph ( static_cast<const ArcAddition&> ( change ) );

          // get new possible changes from the graph change generator
          // warning: put the next 3 lines before calling __illegal2LegalChanges
          __changes_generator->modifyGraph
            ( static_cast<const ArcAddition&> ( change ) );
          __getNewChanges ();

          // indicate that we have just applied the change
          __invalidateChange ( change_index );

          // indicate that the queue to which the change belongs needs be updated
          __queues_to_update.insert ( change.node2 () );
        }
        break;

      case GraphChangeType::ARC_DELETION:
        {
          // update the current score
          __node_current_scores[change.node2 ()] +=
            __change_scores[change_index].second;
          std::vector<unsigned int>& parents = __parents[ change.node2 () ];
          for ( auto& par : parents ) {
            if ( par == change.node1 () ) {
              par = *( parents.rbegin () );
              parents.pop_back ();
              break;
            }
          }
          
          // inform the constraint that the graph has been modified
          __constraint->modifyGraph
            ( static_cast<const ArcDeletion&> ( change ) );
          
          // get new possible changes from the graph change generator
          // warning: put the next 3 lines before calling __illegal2LegalChanges
          __changes_generator->modifyGraph
            ( static_cast<const ArcDeletion&> ( change ) );
          __getNewChanges ();
          
          // indicate that we have just applied the change
          __invalidateChange ( change_index );
          
          // indicate that the queue to which the change belongs needs be updated
          __queues_to_update.insert ( change.node2 () );
        }
        break;

      case GraphChangeType::ARC_REVERSAL:
        {
          // update the current score
          __node_current_scores[change.node1 ()] +=
            __change_scores[change_index].first;
          __node_current_scores[change.node2 ()] +=
            __change_scores[change_index].second;
          __parents[change.node1 ()].push_back ( change.node2 () );
          std::vector<unsigned int>& parents = __parents[ change.node2 () ];
          for ( auto& par : parents ) {
            if ( par == change.node1 () ) {
              par = *( parents.rbegin () );
              parents.pop_back ();
              break;
            }
          }

          // inform the constraint that the graph has been modified
          __constraint->modifyGraph ( static_cast<const ArcReversal&> ( change ) );

          // get new possible changes from the graph change generator
          // warning: put the next 3 lines before calling __illegal2LegalChanges
          __changes_generator->modifyGraph
            ( static_cast<const ArcReversal&> ( change ) );
          __getNewChanges ();

          // indicate that we have just applied the change
          __invalidateChange ( change_index );

          // indicate that the queue to which the change belongs needs be updated
          __queues_to_update.insert ( change.node1 () );
          __queues_to_update.insert ( change.node2 () );
        }
        break;

      default:
        GUM_ERROR ( NotFound, "not implemented" );
      }
    }


    /// applies several changes at a time
    template <typename SCORE,
              typename STRUCTURAL_CONSTRAINT,
              template <typename> class GRAPH_CHANGES_GENERATOR> INLINE
    void
    GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,GRAPH_CHANGES_GENERATOR>::
    updateScoresAfterAppliedChanges () {
      // update the scores that need be updated
      Set<unsigned int> changes_to_recompute;

      __illegal2LegalChanges ( changes_to_recompute );
      for ( const auto& node : __queues_to_update ) {
        __findLegalChangesNeedingUpdate ( changes_to_recompute, node );
      }
      __queues_to_update.clear ();
      __updateScores ( changes_to_recompute );
 
      __queues_valid = false;
    }


    /// returns the set of queues sorted by decreasing top priority
    template <typename SCORE,
              typename STRUCTURAL_CONSTRAINT,
              template <typename> class GRAPH_CHANGES_GENERATOR> INLINE
    std::vector< std::pair<unsigned int,float> >
    GraphChangesSelector<SCORE,STRUCTURAL_CONSTRAINT,GRAPH_CHANGES_GENERATOR>::
    nodesSortedByBestScore () const {
      std::vector< std::pair<unsigned int,float> > result ( __node_queue.size () );
       for ( unsigned int i = 0; i < __node_queue.size (); ++i ) {
        result[i].first  = __node_queue[i];
        result[i].second = __node_queue.priorityByPos ( i );
      }

      std::sort ( result.begin (), result.end (),
                  [] ( const std::pair<unsigned int,float>& a,
                       const std::pair<unsigned int,float>& b) -> bool { 
                    return a.second > b.second; 
                  } );
      
     return result;
    }

  } /* namespace learning */
  
  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

