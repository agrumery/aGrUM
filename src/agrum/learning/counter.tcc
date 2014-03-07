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
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    Counter<RowFilter,IdSetAlloc,CountAlloc>::Counter
    ( const RowFilter& filter,
      const std::vector<unsigned int>& var_modalities) :
      _modalities ( var_modalities ), 
      __record_counter ( filter, var_modalities ) {
      // for debugging purposes
      GUM_CONSTRUCTOR ( Counter );
    }

    
    /// destructor
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    Counter<RowFilter,IdSetAlloc,CountAlloc>::~Counter () {
      // for debugging purposes
      GUM_DESTRUCTOR ( Counter );
      clear ();
    }

    
    /// add a new single variable to be counted
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Counter<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
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
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Counter<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
    ( unsigned int var,
      const std::vector<unsigned int>& conditioning_ids ) {
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
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Counter<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
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
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Counter<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
    ( const std::pair<unsigned int,unsigned int>& vars ) {
      return addNodeSet ( vars.first, vars.second );
    }

      
    /// add a new pair of target conditioned variables to be counted
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Counter<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
    ( unsigned int var1,
      unsigned int var2,
      const std::vector<unsigned int>& conditioning_ids ) {
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
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Counter<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
    ( const std::pair<unsigned int,unsigned int>& vars,
      const std::vector<unsigned int>& conditioning_ids ) {
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
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Counter<RowFilter,IdSetAlloc,CountAlloc>::addNodeSets
    ( const std::vector<unsigned int>& single_vars ) {
      unsigned int index;
      for ( auto var : single_vars ) {
        index = addNodeSets ( var );
      }
      return index - single_vars.size () + 1;
    }


    /// add new set of "conditioned" single targets
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int
    Counter<RowFilter,IdSetAlloc,CountAlloc>::addNodeSets
    ( const std::vector<unsigned int>& single_vars,
      const std::vector<unsigned int>& conditioning_ids ) {
      unsigned int index;
      for ( auto var : single_vars ) {
        index = addNodeSets ( var, conditioning_ids );
      }
      return index - single_vars.size () + 1;
    }


    /// add new set of "unconditioned" pairs of targets
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Counter<RowFilter,IdSetAlloc,CountAlloc>::addNodeSets
    ( const std::vector< std::pair<unsigned int,unsigned int> >& vars ) {
      unsigned int index;
      for ( const auto& var : vars ) {
        index = addNodeSets ( var );
      }
      return index - vars.size () + 1;
    }

    
    /// add new set of "conditioned" pairs of targets
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Counter<RowFilter,IdSetAlloc,CountAlloc>::addNodeSets
    ( const std::vector< std::pair<unsigned int,unsigned int> >& vars,
      const std::vector<unsigned int>& conditioning_ids ) {
      unsigned int index;
      for ( const auto& var : vars ) {
        index = addNodeSets ( var, conditioning_ids );
      }
      return index - vars.size () + 1;
    }


    /// clears all the data structures from memory
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    void Counter<RowFilter,IdSetAlloc,CountAlloc>::clear () {
      __record_counter.clearNodeSets ();

      for ( auto set : __target_nodesets ) delete set;
      for ( auto set : __conditioning_nodesets ) if ( set ) delete set;
      __target_nodesets.clear ();
      __conditioning_nodesets.clear ();
      __counts_computed = false;
    }


    /// perform the computation of the countings
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    void Counter<RowFilter,IdSetAlloc,CountAlloc>::__count () {
      __record_counter.count ();
      __counts_computed = true;
    }

    
    /// returns the counting vector for a given (conditioned) target set
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    const std::vector<float,CountAlloc>&
    Counter<RowFilter,IdSetAlloc,CountAlloc>::_getAllCounts
    ( unsigned int index ) {
      if ( ! __counts_computed ) __count ();
      return __record_counter.getCounts ( __target_nodesets[index]->second );
    }

    
    /// returns the counting vector for a conditioning set
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    const std::vector<float,CountAlloc>&
    Counter<RowFilter,IdSetAlloc,CountAlloc>::_getConditioningCounts
    ( unsigned int index ) {
      if ( ! __counts_computed ) __count ();
      return __record_counter.getCounts ( __conditioning_nodesets[index]->second );
    }

    
    /// returns the set of target + conditioning nodes
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    const std::vector<unsigned int,IdSetAlloc>&
    Counter<RowFilter,IdSetAlloc,CountAlloc>::_getAllNodes
    ( unsigned int index ) const noexcept {
      return __target_nodesets[index]->first;
    }

    /// returns the set of conditioning nodes
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    const std::vector<unsigned int,IdSetAlloc>*
    Counter<RowFilter,IdSetAlloc,CountAlloc>::_getConditioningNodes
    ( unsigned int index ) const noexcept {
      if ( __conditioning_nodesets[index] != nullptr ) {
        return &( __conditioning_nodesets[index]->first );
      }
      else {
        return nullptr;
      }
    }

    
    /// returns the modalities of the variables
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    const std::vector<unsigned int>&
    Counter<RowFilter,IdSetAlloc,CountAlloc>::modalities () const noexcept {
      return _modalities;
    }

 
  } /* namespace learning */
  
  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

