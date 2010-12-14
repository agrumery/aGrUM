/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               * 
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
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
 * @brief A class to combine efficiently several ScheduleMultiDims
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */


#ifndef DOXYGEN_SHOULD_SKIP_THIS


#include <agrum/core/debug.h>
#include <agrum/core/exceptions.h>
#include <agrum/core/priorityQueue.h>


namespace gum {

  
  /// constructor
  template<typename T_DATA>
  ScheduleCombinationBasic<T_DATA>::ScheduleCombinationBasic
  (MultiDimImplementation<T_DATA>* (*combine)
   ( const MultiDimImplementation<T_DATA>&,
     const MultiDimImplementation<T_DATA>& ) ) :
    ScheduleCombination<T_DATA> (),
    _combine ( combine ) {
    /// for debugging purposes
    GUM_CONSTRUCTOR ( ScheduleCombinationBasic );
  }

  
  /// copy constructor
  template<typename T_DATA>
  ScheduleCombinationBasic<T_DATA>::ScheduleCombinationBasic
  ( const ScheduleCombinationBasic<T_DATA>& from ) :
    ScheduleCombination<T_DATA> ( from ),
    _combine ( from._combine ) {
    /// for debugging purposes
    GUM_CONS_CPY ( ScheduleCombinationBasic );
  }

  
  /// destructor
  template<typename T_DATA>
  ScheduleCombinationBasic<T_DATA>::~ScheduleCombinationBasic () {
    /// for debugging purposes
    GUM_DESTRUCTOR ( ScheduleCombinationBasic );
  }

  
  /// virtual constructor
  template<typename T_DATA>
  ScheduleCombinationBasic<T_DATA>*
  ScheduleCombinationBasic<T_DATA>::newFactory () const {
    return new ScheduleCombinationBasic<T_DATA> ( *this );
  }


  /// changes the function used for combining two MultiDimImplementations
  template<typename T_DATA>
  void ScheduleCombinationBasic<T_DATA>::setCombinator
  ( MultiDimImplementation<T_DATA>* (*combine)
    ( const MultiDimImplementation<T_DATA>&,
      const MultiDimImplementation<T_DATA>& ) ) {
    _combine = combine;
  }

  
  /// returns the domain size of the Cartesian product of the union of all the
  /// variables in seq1 and seq2
  template<typename T_DATA>
  Size ScheduleCombinationBasic<T_DATA>::_combined_size
  ( const Sequence<const DiscreteVariable *>& seq1,
    const Sequence<const DiscreteVariable *>& seq2 ) const {
    if ( seq1.empty() && seq2.empty() ) return 0;
    
    Size size = 1;
    for ( Sequence<const DiscreteVariable *>::const_iterator iter =
            seq1.begin(); iter != seq1.end(); ++iter ) {
      size *= (*iter)->domainSize();
    }
    for ( Sequence<const DiscreteVariable *>::const_iterator iter =
            seq2.begin(); iter != seq2.end(); ++iter ) {
      if ( ! seq1.exists ( *iter ) )
        size *= (*iter)->domainSize();
    }
    
    return size;
  }


  // adds operations to an already created schedule
  template< typename T_DATA >
  ScheduleMultiDim<T_DATA>
  ScheduleCombinationBasic<T_DATA>::combine
  ( const Set<const ScheduleMultiDim<T_DATA>*>& set,
    Schedule<T_DATA>& schedule ) {
    // check if the set passed in argument is empty. If so, raise an exception
    if ( set.size() < 2 )
      GUM_ERROR ( InvalidArgumentsNumber,
                  "the set passed to a ScheduleCombinationBasic"
                  " should at least contain two elements" );
    
    // create a vector with all the tables to combine
    std::vector<const ScheduleMultiDim<T_DATA>*> tables ( set.size() );
    {
      unsigned int i = 0;
      for ( typename Set<const ScheduleMultiDim<T_DATA>*>::const_iterator iter =
              set.begin(); iter != set.end(); ++iter, ++i ) {
        tables[i] = *iter;
      }
    }
    
    // create a vector indicating wether the elements in tables are freshly
    // created ScheduleMultiDim<T_DATA>* due to the combination of some
    // ScheduleMultiDims or if they were added by the user into the
    // combination container
    std::vector<bool> is_t_new ( tables.size(), false );  
    
    // for each pair of tables (i,j), compute the size of the table that would
    // result from the addition of tables i and j and store the result into a
    // priorityQueue
    std::pair<unsigned int,unsigned int> pair;
    PriorityQueue<std::pair<unsigned int,unsigned int>,Size> queue;
    for ( unsigned int i = 0; i < tables.size(); ++i ) {
      pair.first = i;
      const Sequence<const DiscreteVariable *>& seq1 =
        tables[i]->variablesSequence();
      for ( unsigned int j = i+1; j < tables.size(); ++j ) {
        pair.second = j;
        queue.insert
          ( _combined_size ( seq1, tables[j]->variablesSequence() ), pair );
      }
    }

    // now parse the priority queue: the top element (i,j) gives the combination
    // to perform. When the result R has been computed, substitute i by R, remove
    // table j and recompute all the priorities of all the pairs (R,k) still
    // available.
    //Timer timer;
    for ( unsigned int k = 1; k < tables.size(); ++k ) {
      // get the combination to perform and do it
      pair = queue.pop ();
      unsigned int ti = pair.first;
      unsigned int tj = pair.second;

      // create the combination that will be performed later on and put it into
      // the schedule
      ScheduleCombine<T_DATA> comb ( *( tables[ti] ), *( tables[tj] ), _combine );
      NodeId comb_id = schedule.insert ( comb );
      
      // substitute tables[pair.first] by the result and delete the temporary
      // multidim tables
      if ( tables[ti] && is_t_new[ti] ) {
        ScheduleDeleteMultiDim<T_DATA> del ( *( tables[ti] ) );
        NodeId del_id = schedule.insert ( del );
        const NodeSet& set_i = schedule.operationsInvolving ( *( tables[ti] ) );
        schedule.forceAfter ( del_id, set_i );
      }
      if ( tables[tj] && is_t_new[tj] ) {
        ScheduleDeleteMultiDim<T_DATA> del ( *( tables[tj] ) );
        NodeId del_id = schedule.insert ( del );
        const NodeSet& set_j = schedule.operationsInvolving ( *( tables[tj] ) );
        schedule.forceAfter ( del_id, set_j );
      }
      tables[ti] = &( static_cast<const ScheduleCombine<T_DATA>&>
                      ( schedule.operation ( comb_id ) ).result () );
      is_t_new[ti] = true;
      tables[tj] = 0;

     // remove all the pairs involving tj in the priority queue
      for ( unsigned int ind = 0; ind < tj; ++ind ) {
        if ( tables[ind] ) {
          pair.first = ind;
          queue.eraseByVal ( pair );
        }
      }
      pair.first = tj;
      for ( unsigned int ind = tj+1; ind < tables.size(); ++ind ) {
        if ( tables[ind] ) {
          pair.second = ind;
          queue.eraseByVal ( pair );
        }
      }

      // update the "combinated" size of all the pairs involving "result"
      {
        const Sequence<const DiscreteVariable *>& seq1 =
          tables[ti]->variablesSequence ();
        pair.second = ti;
        Size newsize;
        for ( unsigned int ind = 0; ind < ti; ++ind ) {
          if ( tables[ind] ) {
            pair.first = ind;
            newsize = _combined_size ( seq1, tables[ind]->variablesSequence () );
            queue.setPriorityByVal ( pair, newsize );
          }
        }
        pair.first = ti;
        for ( unsigned int ind = ti+1; ind < tables.size(); ++ind ) {
          if ( tables[ind] ) {
            pair.second = ind;
            newsize = _combined_size ( seq1, tables[ind]->variablesSequence () ); 
            queue.setPriorityByVal ( pair, newsize );
          }
        }
      }
    }

    // here, there remains only one nonzero pointer in tables:
    // the result of our combination
    unsigned int k = 0;
    while ( ! tables[k] ) ++k;
    return *( tables[k] );
  }


  // adds operations to an already created schedule
  template<typename T_DATA>
  INLINE ScheduleMultiDim<T_DATA>
  ScheduleCombinationBasic<T_DATA>::combine
  ( const Set<const MultiDimImplementation<T_DATA>*>& set,
    Schedule<T_DATA>& schedule ) {
    return ScheduleCombination<T_DATA>::combine ( set, schedule );
  }
  
  
  // adds operations to an already created schedule
  template<typename T_DATA>
  template <template<typename> class TABLE>
  INLINE ScheduleMultiDim<T_DATA>
  ScheduleCombinationBasic<T_DATA>:: combine
  ( const Set<const TABLE<T_DATA>*>& set,
    Schedule<T_DATA>& schedule ) {
    return ScheduleCombination<T_DATA>::combine ( set, schedule );
  }
  
  
  /// returns the result of the combination
  template<typename T_DATA>
  float ScheduleCombinationBasic<T_DATA>::nbOperations
  ( const Set<const ScheduleMultiDim<T_DATA>*>& set,
    const Schedule<T_DATA>& schedule ) {
    // check if the set passed in argument is empty.
    if ( set.size() < 2 ) return 0.0f;

    float result = 0.0f;
    
    // create a vector with all the tables to combine
    std::vector< const Sequence<const DiscreteVariable *>* > tables ( set.size() );
    {
      unsigned int i = 0;
      for ( typename Set<const ScheduleMultiDim<T_DATA>*>::const_iterator iter =
              set.begin(); iter != set.end(); ++iter, ++i ) {
        tables[i] = &( (*iter)->variablesSequence () );
      }
    }
    
    // create a vector indicating wether the elements in tables are freshly
    // created Sequence<const DiscreteVariable *>* due to the combination of some
    // ScheduleMultiDims or if they were added by the user into the combination
    // container
    std::vector<bool> is_t_new ( tables.size(), false );  
 
    
    // for each pair of tables (i,j), compute the size of the table that would
    // result from the addition of tables i and j and store the result into a
    // priorityQueue
    std::pair<unsigned int,unsigned int> pair;
    PriorityQueue<std::pair<unsigned int,unsigned int>,Size> queue;
    for ( unsigned int i = 0; i < tables.size(); ++i ) {
      pair.first = i;
      for ( unsigned int j = i+1; j < tables.size(); ++j ) {
        pair.second = j;
        queue.insert ( _combined_size ( *(tables[i]), *(tables[j]) ), pair );
      }
    }

    // now parse the priority queue: the top element (i,j) gives the combination
    // to perform. When the result R has been computed, substitute i by R, remove
    // table j and recompute all the priorities of all the pairs (R,k) still
    // available.
    for ( unsigned int k = 1; k < tables.size(); ++k ) {
      // get the combination to perform and do it
      pair = queue.pop ();
      unsigned int ti = pair.first;
      unsigned int tj = pair.second;

      // compute the result
      Sequence<const DiscreteVariable *>* new_seq =
        new Sequence<const DiscreteVariable *>;
      const Sequence<const DiscreteVariable *>& seq1 = *( tables[ti] );
      const Sequence<const DiscreteVariable *>& seq2 = *( tables[tj] );
    
      Size new_size = 1;
      for ( Sequence<const DiscreteVariable *>::const_iterator iter =
              seq1.begin(); iter != seq1.end(); ++iter ) {
        new_size *= (*iter)->domainSize();
        new_seq->insert ( *iter );
      }
      for ( Sequence<const DiscreteVariable *>::const_iterator iter =
              seq2.begin(); iter != seq2.end(); ++iter ) {
        if ( ! seq1.exists ( *iter ) ) {
          new_size *= (*iter)->domainSize();
          new_seq->insert ( *iter );
        }
      }
      result += new_size;     
    
      // substitute tables[pair.first] by the result
      if ( tables[ti] && is_t_new[ti] ) delete tables[ti];
      if ( tables[tj] && is_t_new[tj] ) delete tables[tj];
      tables[ti] = new_seq;
      is_t_new[ti] = true;
      tables[tj] = 0;

      // remove all the pairs involving tj in the priority queue
      for ( unsigned int ind = 0; ind < tj; ++ind ) {
        if ( tables[ind] ) {
          pair.first = ind;
          queue.eraseByVal ( pair );
        }
      }
      pair.first = tj;
      for ( unsigned int ind = tj+1; ind < tables.size(); ++ind ) {
        if ( tables[ind] ) {
          pair.second = ind;
          queue.eraseByVal ( pair );
        }
      }

      // update the "combined" size of all the pairs involving "new_seq"
      {
        pair.second = ti;
        Size newsize;
        for ( unsigned int ind = 0; ind < ti; ++ind ) {
          if ( tables[ind] ) {
            pair.first = ind;
            newsize = _combined_size ( *new_seq, * ( tables[ind] ) ); 
            queue.setPriorityByVal ( pair, newsize );
          }
        }
        pair.first = ti;
        for ( unsigned int ind = ti+1; ind < tables.size(); ++ind ) {
          if ( tables[ind] ) {
            pair.second = ind;
            newsize = _combined_size ( *new_seq, * ( tables[ind] ) ); 
            queue.setPriorityByVal ( pair, newsize );
          }
        }
      }
    }

    // here, there remains only one nonzero pointer in tables:
    // the result of our combination
    unsigned int k = 0;
    while ( ! tables[k] ) ++k;
    delete tables[k];
    return result;
  }

  
  /// returns the result of the combination
  template<typename T_DATA>
  INLINE float ScheduleCombinationBasic<T_DATA>::nbOperations
  ( const Set<const MultiDimImplementation<T_DATA>*>& set,
    const Schedule<T_DATA>& schedule ) {
    return ScheduleCombination<T_DATA>::nbOperations ( set, schedule );
  }

  
  /// returns the result of the combination
  template<typename T_DATA>
  template <template<typename> class TABLE>
  INLINE float ScheduleCombinationBasic<T_DATA>::nbOperations
  ( const Set<const TABLE<T_DATA>*>& set,
    const Schedule<T_DATA>& schedule ) {
    return ScheduleCombination<T_DATA>::nbOperations ( set, schedule );
  }
    

} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
