/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
 * @brief the base class for all the scores used for learning (BIC, BDeu, etc)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


#include <cmath>


namespace gum {

  
  namespace learning {
    
    
    /// default constructor
    template <typename IdSetAlloc, typename CountAlloc>
    template <typename RowFilter> INLINE
    Counter<IdSetAlloc,CountAlloc>::Counter
    ( const RowFilter& filter,
      const std::vector<unsigned int>& var_modalities) :
      _modalities ( var_modalities ), 
      __record_counter ( filter, var_modalities ) {
      // for debugging purposes
      GUM_CONSTRUCTOR ( Counter );
    }

    
    /// copy constructor
    template <typename IdSetAlloc, typename CountAlloc>
    Counter<IdSetAlloc,CountAlloc>::Counter
    ( const Counter<IdSetAlloc,CountAlloc>& from ) :
      _modalities ( from._modalities ),
      __record_counter ( from.__record_counter ),
      __counts_computed ( from.__counts_computed ) {
      // copy the target nodesets
      __target_nodesets.reserve ( from.__target_nodesets.size () );
      for ( const auto set : from.__target_nodesets ) {
        __target_nodesets.push_back
          ( new std::pair<std::vector<unsigned int,IdSetAlloc>,
                          unsigned int> ( *set ) );
      }
      
      // copy the conditioning nodesets
      __conditioning_nodesets.reserve ( from.__conditioning_nodesets.size () );
      for ( const auto set : from.__conditioning_nodesets ) {
        __conditioning_nodesets.push_back
          ( new std::pair<std::vector<unsigned int,IdSetAlloc>,
                          unsigned int> ( *set ) );
      }

      // now update the __nodesets of the record counter, as this one still
      // points to the nodesets of from
      std::vector<const std::vector<unsigned int,IdSetAlloc>* >&
        nodesets = __record_counter.__nodesets;
      const std::vector<const std::vector<unsigned int,IdSetAlloc>* >&
        from_nodesets = from.__record_counter.__nodesets;
      for ( unsigned int i = 0, j = 0, size = from.__target_nodesets.size ();
            i < size; ++i ) {
        if ( from_nodesets[j] == &( from.__target_nodesets[i]->first ) ) {
          nodesets[j] = &( __target_nodesets[i]->first );
          ++j;
        }
      }
      for ( unsigned int i = 0, j = 0, size = from.__conditioning_nodesets.size ();
            i < size; ++i ) {
        if ( from_nodesets[j] == &( from.__conditioning_nodesets[i]->first ) ) {
          nodesets[j] = &( __conditioning_nodesets[i]->first );
          ++j;
        }
      }

      // for debugging purposes
      GUM_CONS_CPY ( Counter );
    }
      

    /// move constructor
    template <typename IdSetAlloc, typename CountAlloc>
    Counter<IdSetAlloc,CountAlloc>::Counter
    ( Counter<IdSetAlloc,CountAlloc>&& from ) :
      _modalities ( from._modalities ),
      __record_counter  ( std::move ( from.__record_counter ) ),
      __target_nodesets ( std::move ( from.__target_nodesets ) ),
      __conditioning_nodesets ( std::move ( from.__conditioning_nodesets ) ),
      __counts_computed ( std::move ( from.__counts_computed ) ) {
      from.__target_nodesets.clear ();
      from.__conditioning_nodesets.clear ();
      
      // for debugging purposes
      GUM_CONS_CPY ( Counter );
    }
    
    
    /// destructor
    template <typename IdSetAlloc, typename CountAlloc>
    Counter<IdSetAlloc,CountAlloc>::~Counter () {
      // for debugging purposes
      GUM_DESTRUCTOR ( Counter );
      clear ();
    }

    
    /// add an empty variable set to be counted
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Counter<IdSetAlloc,CountAlloc>::addEmptyNodeSet () {
      __conditioning_nodesets.push_back ( nullptr );
      __target_nodesets.push_back ( nullptr );
      __counts_computed = false;
      
      return __target_nodesets.size () - 1;
    }

    
    /// add a new single variable to be counted
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Counter<IdSetAlloc,CountAlloc>::addNodeSet
    ( unsigned int var ) {
      // create the target nodeset
      std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>* new_target =
        new std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>
        ( std::vector<unsigned int,IdSetAlloc> { var }, 0 );

      // assign to it its record counter index
      new_target->second = __record_counter.addNodeSet ( new_target->first );

      // save it in the score's nodeset
      __conditioning_nodesets.push_back ( nullptr );
      __target_nodesets.push_back ( new_target );

      __counts_computed = false;
      
      return __target_nodesets.size () - 1;
    }



    /// add a new target variable plus some conditioning vars
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Counter<IdSetAlloc,CountAlloc>::addNodeSet
    ( unsigned int var,
      const std::vector<unsigned int>& conditioning_ids ) {
      // if the conditioning set is empty, perform the unconditional addNodeSet
      if ( ! conditioning_ids.size () ) {
        return Counter<IdSetAlloc,CountAlloc>::addNodeSet ( var );
      }
          
      // create the target nodeset and assign to it its record counter index
      std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>* new_target =
        new std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>
        ( conditioning_ids, 0 );
      new_target->first.push_back ( var );
      new_target->second = __record_counter.addNodeSet ( new_target->first );

      // create the conditioning nodeset and assign to it its record counter index
      std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>* new_cond =
        new std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>
        ( conditioning_ids, 0 );
      new_cond->second = __record_counter.addNodeSet ( new_cond->first );
     
      // save it in the score's nodeset
      __conditioning_nodesets.push_back ( new_cond );
      __target_nodesets.push_back ( new_target );

      __counts_computed = false;
      
      return __target_nodesets.size () - 1;
    }


    /// add a new pair of target unconditioned variables to be counted
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Counter<IdSetAlloc,CountAlloc>::addNodeSet
    ( unsigned int var1,
      unsigned int var2 ) {
      // create the target nodeset
      std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>* new_target =
        new std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>
        ( std::vector<unsigned int,IdSetAlloc> { var2, var1 }, 0 );

      // assign to it its record counter index
      new_target->second = __record_counter.addNodeSet ( new_target->first );

      // create the conditioning nodeset and assign to it its record counter index
      std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>* new_cond =
        new std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>
        ( std::vector<unsigned int,IdSetAlloc> { var2 }, 0 );
      new_cond->second = __record_counter.addNodeSet ( new_cond->first );
  
      // save it in the score's nodeset
      __conditioning_nodesets.push_back ( new_cond );
      __target_nodesets.push_back ( new_target );

      __counts_computed = false;
      
      return __target_nodesets.size () - 1;
    }


    /// add a new pair of target unconditioned variables to be counted
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Counter<IdSetAlloc,CountAlloc>::addNodeSet
    ( const std::pair<unsigned int,unsigned int>& vars ) {
      return addNodeSet ( vars.first, vars.second );
    }

      
    /// add a new pair of target conditioned variables to be counted
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Counter<IdSetAlloc,CountAlloc>::addNodeSet
    ( unsigned int var1,
      unsigned int var2,
      const std::vector<unsigned int>& conditioning_ids ) {  
      // if the conditioning set is empty, perform the unconditional addNodeSet
      if ( ! conditioning_ids.size () ) {
        return Counter<IdSetAlloc,CountAlloc>::addNodeSet ( var1, var2 );
      }

      // create the target nodeset and assign to it its record counter index
      std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>* new_target =
        new std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>
        ( conditioning_ids, 0 );
      new_target->first.push_back ( var2 );
      new_target->first.push_back ( var1 );
      new_target->second = __record_counter.addNodeSet ( new_target->first );

      // create the conditioning nodeset and assign to it its record counter index
      std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>* new_cond =
        new std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>
        ( conditioning_ids, 0 );
      new_cond->first.push_back ( var2 );
      new_cond->second = __record_counter.addNodeSet ( new_cond->first );
     
      // save it in the score's nodeset
      __conditioning_nodesets.push_back ( new_cond );
      __target_nodesets.push_back ( new_target );

      __counts_computed = false;
      
      return __target_nodesets.size () - 1;
    }


    /// add a new pair of target conditioned variables to be counted
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Counter<IdSetAlloc,CountAlloc>::addNodeSet
    ( const std::pair<unsigned int,unsigned int>& vars,
      const std::vector<unsigned int>& conditioning_ids ) {
      // if the conditioning set is empty, perform the unconditional addNodeSet
      if ( ! conditioning_ids.size () ) {
        return Counter<IdSetAlloc,CountAlloc>::addNodeSet
          ( vars.first, vars.second );
      }
      
      // create the target nodeset and assign to it its record counter index
      std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>* new_target =
        new std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>
        ( conditioning_ids, 0 );
      new_target->first.push_back ( vars.second );
      new_target->first.push_back ( vars.first );
      new_target->second = __record_counter.addNodeSet ( new_target->first );

      // create the conditioning nodeset and assign to it its record counter index
      std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>* new_cond =
        new std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>
        ( conditioning_ids, 0 );
      new_cond->first.push_back ( vars.second );
      new_cond->second = __record_counter.addNodeSet ( new_cond->first );
     
      // save it in the score's nodeset
      __conditioning_nodesets.push_back ( new_cond );
      __target_nodesets.push_back ( new_target );

      __counts_computed = false;
      
      return __target_nodesets.size () - 1;
    }


    /// add new set of "unconditioned" single targets
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Counter<IdSetAlloc,CountAlloc>::addNodeSets
    ( const std::vector<unsigned int>& single_vars ) {
      unsigned int index;
      for ( auto var : single_vars ) {
        index = addNodeSets ( var );
      }
      return index - single_vars.size () + 1;
    }


    /// add new set of "conditioned" single targets
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int
    Counter<IdSetAlloc,CountAlloc>::addNodeSets
    ( const std::vector<unsigned int>& single_vars,
      const std::vector<unsigned int>& conditioning_ids ) {
      // if the conditioning set is empty, perform the unconditional addNodeSet
      if ( ! conditioning_ids.size () ) {
        return Counter<IdSetAlloc,CountAlloc>::addNodeSet
          ( single_vars );
      }

      unsigned int index;
      for ( auto var : single_vars ) {
        index = addNodeSets ( var, conditioning_ids );
      }
      return index - single_vars.size () + 1;
    }


    /// add new set of "unconditioned" pairs of targets
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Counter<IdSetAlloc,CountAlloc>::addNodeSets
    ( const std::vector< std::pair<unsigned int,unsigned int> >& vars ) {
      unsigned int index;
      for ( const auto& var : vars ) {
        index = addNodeSets ( var );
      }
      return index - vars.size () + 1;
    }

    
    /// add new set of "conditioned" pairs of targets
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Counter<IdSetAlloc,CountAlloc>::addNodeSets
    ( const std::vector< std::pair<unsigned int,unsigned int> >& vars,
      const std::vector<unsigned int>& conditioning_ids ) {
      // if the conditioning set is empty, perform the unconditional addNodeSet
      if ( ! conditioning_ids.size () ) {
        return Counter<IdSetAlloc,CountAlloc>::addNodeSet ( vars );
      }

      unsigned int index;
      for ( const auto& var : vars ) {
        index = addNodeSets ( var, conditioning_ids );
      }
      return index - vars.size () + 1;
    }


    /// clears all the data structures from memory
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    void Counter<IdSetAlloc,CountAlloc>::clear () {
      __record_counter.clearNodeSets ();

      for ( auto set : __target_nodesets ) delete set;
      for ( auto set : __conditioning_nodesets ) if ( set ) delete set;
      __target_nodesets.clear ();
      __conditioning_nodesets.clear ();
      __counts_computed = false;
    }


    /// perform the computation of the countings
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    void Counter<IdSetAlloc,CountAlloc>::__count () {
      __record_counter.count ();
      __counts_computed = true;
    }

    
    /// returns the counting vector for a given (conditioned) target set
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    const std::vector<float,CountAlloc>&
    Counter<IdSetAlloc,CountAlloc>::_getAllCounts
    ( unsigned int index ) {
      if ( ! __counts_computed ) __count ();
      return __record_counter.getCounts ( __target_nodesets[index]->second );
    }

    
    /// returns the counting vector for a conditioning set
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    const std::vector<float,CountAlloc>&
    Counter<IdSetAlloc,CountAlloc>::_getConditioningCounts
    ( unsigned int index ) {
      if ( ! __counts_computed ) __count ();
      return __record_counter.getCounts ( __conditioning_nodesets[index]->second );
    }

    
    /// returns the set of target + conditioning nodes
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    const std::vector<unsigned int,IdSetAlloc>&
    Counter<IdSetAlloc,CountAlloc>::_getAllNodes
    ( unsigned int index ) const noexcept {
      return __target_nodesets[index]->first;
    }

    /// returns the set of conditioning nodes
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    const std::vector<unsigned int,IdSetAlloc>*
    Counter<IdSetAlloc,CountAlloc>::_getConditioningNodes
    ( unsigned int index ) const noexcept {
      if ( __conditioning_nodesets[index] != nullptr ) {
        return &( __conditioning_nodesets[index]->first );
      }
      else {
        return nullptr;
      }
    }

    
    /// returns the modalities of the variables
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    const std::vector<unsigned int>&
    Counter<IdSetAlloc,CountAlloc>::modalities () const noexcept {
      return _modalities;
    }


    /// sets the maximum number of threads used to perform countings
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    void
    Counter<IdSetAlloc,CountAlloc>::setMaxNbThreads ( unsigned int nb )
      noexcept {
      __record_counter.setMaxNbThreads ( nb );
    }

 
  } /* namespace learning */
  
  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

