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
 * @brief A generic class to combine efficiently several MultiDim tables
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


#include <agrum/core/debug.h>
#include <agrum/core/exceptions.h>
#include <agrum/core/priorityQueue.h>


namespace gum {

  
  /// constructor
  template< typename T_DATA, template<typename> class TABLE >
  MultiDimCombination<T_DATA,TABLE>::MultiDimCombination
  (TABLE<T_DATA>* (*combine) ( const TABLE<T_DATA>&,const TABLE<T_DATA>& ) ) :
    __combine ( combine ) {
    /// for debugging purposes
    GUM_CONSTRUCTOR ( MultiDimCombination );
  }

  
  /// copy constructor
  template< typename T_DATA, template<typename> class TABLE >
  MultiDimCombination<T_DATA,TABLE>::MultiDimCombination
  ( const MultiDimCombination<T_DATA,TABLE>& from ) :
    __combine ( from.__combine ) {
    /// for debugging purposes
    GUM_CONS_CPY ( MultiDimCombination );
  }

  
  /// destructor
  template< typename T_DATA, template<typename> class TABLE >
  MultiDimCombination<T_DATA,TABLE>::~MultiDimCombination () {
    /// for debugging purposes
    GUM_DESTRUCTOR ( MultiDimCombination );
  }

  
  /// virtual constructor
  template< typename T_DATA, template<typename> class TABLE >
  MultiDimCombination<T_DATA,TABLE>*
  MultiDimCombination<T_DATA,TABLE>::newFactory () const {
    return new MultiDimCombination<T_DATA,TABLE> ( __combine );
  }

  
  /// changes the function used for combining two TABLES
  template< typename T_DATA, template<typename> class TABLE >
  void MultiDimCombination<T_DATA,TABLE>::setCombinator
  ( TABLE<T_DATA>*
    (*combine) ( const TABLE<T_DATA>&,const TABLE<T_DATA>& ) ) {
    __combine = combine;
  }

  
  /// returns the domain size of the Cartesian product of the union of all the
  /// variables in seq1 and seq2
  template< typename T_DATA, template<typename> class TABLE >
  Size MultiDimCombination<T_DATA,TABLE>::__combined_size
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

   /// returns the result of the combination
   /// @todo This implementation is very improvable : we need a way for decorators to swap contents instead of copying it...
  template< typename T_DATA, template<typename> class TABLE >
  INLINE void
  MultiDimCombination<T_DATA,TABLE>::combine
  ( TABLE<T_DATA>& container, const Set<const TABLE<T_DATA>*>& set ) {
    TABLE<T_DATA>* res=combine(set);
    container=*res;
    delete(res);
  }

  /// returns the result of the combination
  template< typename T_DATA, template<typename> class TABLE >
  TABLE<T_DATA>*
  MultiDimCombination<T_DATA,TABLE>::combine
  ( const Set<const TABLE<T_DATA>*>& set ) {
    // check if the set passed in argument is empty. If so, raise an exception
    if ( set.size() < 2 )
      GUM_ERROR ( InvalidArgumentsNumber, "the set passed to a MultiDimCombination"
                  " should at least contain two elements" );
    
    // create a vector with all the tables to combine
    std::vector< const TABLE<T_DATA>* > tables ( set.size() );
    {
      unsigned int i = 0;
      for ( typename Set<const TABLE<T_DATA>*>::const_iterator iter =
              set.begin(); iter != set.end(); ++iter, ++i ) {
        tables[i] = *iter;
      }
    }
    
    // create a vector indicating wether the elements in tables are freshly
    // created TABLE<T_DATA>* due to the combination of some TABLEs or if they
    // were added by the user into the combination container
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
          ( __combined_size ( seq1, tables[j]->variablesSequence() ), pair );
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

      TABLE<T_DATA>* result = __combine ( *(tables[ti]), *(tables[tj]) );
    
      // substitute tables[pair.first] by the result
      if ( tables[ti] && is_t_new[ti] ) delete tables[ti];
      if ( tables[tj] && is_t_new[tj] ) delete tables[tj];
      tables[ti] = result;
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
            newsize = __combined_size ( seq1, tables[ind]->variablesSequence () ); 
            queue.setPriorityByVal ( pair, newsize );
          }
        }
        pair.first = ti;
        for ( unsigned int ind = ti+1; ind < tables.size(); ++ind ) {
          if ( tables[ind] ) {
            pair.second = ind;
            newsize = __combined_size ( seq1, tables[ind]->variablesSequence () ); 
            queue.setPriorityByVal ( pair, newsize );
          }
        }
      }
    }

    // here, there remains only one nonzero pointer in tables:
    // the result of our combination
    unsigned int k = 0;
    while ( ! tables[k] ) ++k;
    return const_cast<TABLE<T_DATA>*>( tables[k] );
  }


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
