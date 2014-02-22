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
#include <agrum/learning/score.h>


#define MAX_LOG2F_SIZE 100000


namespace gum {

  
  namespace learning {
    
    
    /// default constructor
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    Score<RowFilter,IdSetAlloc,CountAlloc>::Score
    ( const RowFilter& filter,
      const std::vector<unsigned int>& var_modalities) :
      _1log2 { M_LOG2E },
      __record_counter ( filter, var_modalities ) {
      // for debugging purposes
      GUM_CONSTRUCTOR ( Score );
    }

    
    /// destructor
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    Score<RowFilter,IdSetAlloc,CountAlloc>::~Score () {
      // for debugging purposes
      GUM_DESTRUCTOR ( Score );
      clear ();
    }

    
    /// add a new single variable to be counted
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Score<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
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

      return __target_nodesets.size () - 1;
    }


    /// add a new target variable plus some conditioning vars
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Score<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
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

      return __target_nodesets.size () - 1;
    }


    /// add a new pair of target unconditioned variables to be counted
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Score<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
    ( unsigned int var1,
      unsigned int var2 ) {
      // create the target nodeset
      std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>* new_target =
        new std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>
        ( std::vector<unsigned int,IdSetAlloc> { var1, var2 }, 0 );

      // assign to it its record counter index
      new_target->second = __record_counter.addNodeSet ( new_target->first );

      // save it in the score's nodeset
      __conditioning_nodesets.push_back ( nullptr );
      __target_nodesets.push_back ( new_target );

      return __target_nodesets.size () - 1;
    }


    /// add a new pair of target unconditioned variables to be counted
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Score<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
    ( const std::pair<unsigned int,unsigned int>& vars ) {
      return addNodeSet ( vars.first, vars.second );
    }

      
    /// add a new pair of target conditioned variables to be counted
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Score<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
    ( unsigned int var1,
      unsigned int var2,
      const std::vector<unsigned int>& conditioning_ids ) {
      // create the target nodeset and assign to it its record counter index
      std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>* new_target =
        new std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>
        ( conditioning_ids, 0 );
      new_target->first.push_back ( var1 );
      new_target->first.push_back ( var2 );
      new_target->second = __record_counter.addNodeSet ( new_target->first );

      // create the conditioning nodeset and assign to it its record counter index
      std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>* new_cond =
        new std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>
        ( conditioning_ids, 0 );
      new_cond->second = __record_counter.addNodeSet ( new_cond->first );
     
      // save it in the score's nodeset
      __conditioning_nodesets.push_back ( new_cond );
      __target_nodesets.push_back ( new_target );

      return __target_nodesets.size () - 1;
    }


    /// add a new pair of target conditioned variables to be counted
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Score<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
    ( const std::pair<unsigned int,unsigned int>& vars,
      const std::vector<unsigned int>& conditioning_ids ) {
      // create the target nodeset and assign to it its record counter index
      std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>* new_target =
        new std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>
        ( conditioning_ids, 0 );
      new_target->first.push_back ( vars.first );
      new_target->first.push_back ( vars.second );
      new_target->second = __record_counter.addNodeSet ( new_target->first );

      // create the conditioning nodeset and assign to it its record counter index
      std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>* new_cond =
        new std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>
        ( conditioning_ids, 0 );
      new_cond->second = __record_counter.addNodeSet ( new_cond->first );
     
      // save it in the score's nodeset
      __conditioning_nodesets.push_back ( new_cond );
      __target_nodesets.push_back ( new_target );

      return __target_nodesets.size () - 1;
    }


    /// add new set of "unconditioned" single targets
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Score<RowFilter,IdSetAlloc,CountAlloc>::addNodeSets
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
    Score<RowFilter,IdSetAlloc,CountAlloc>::addNodeSets
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
    unsigned int Score<RowFilter,IdSetAlloc,CountAlloc>::addNodeSets
    ( const std::vector< std::pair<unsigned int,unsigned int> >& vars ) {
      unsigned int index;
      for ( const auto& var : vars ) {
        index = addNodeSets ( var );
      }
      return index - vars.size () + 1;
    }

    
    /// add new set of "conditioned" pairs of targets
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Score<RowFilter,IdSetAlloc,CountAlloc>::addNodeSets
    ( const std::vector< std::pair<unsigned int,unsigned int> >& vars,
      const std::vector<unsigned int>& conditioning_ids ) {
      unsigned int index;
      for ( const auto& var : vars ) {
        index = addNodeSets ( var, conditioning_ids );
      }
      return index - vars.size () + 1;
    }


    /// perform the computation of the countings
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    void Score<RowFilter,IdSetAlloc,CountAlloc>::count () {
      __record_counter.count ();
    }

    
    /// returns the counting vector for a given (conditioned) target set
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    const std::vector<float,CountAlloc>&
    Score<RowFilter,IdSetAlloc,CountAlloc>::getTargetCounts
    ( unsigned int index ) const noexcept {
      return __record_counter.getCounts ( __target_nodesets[index]->second );
    }

    
    /// returns the counting vector for a conditioning set
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    const std::vector<float,CountAlloc>&
    Score<RowFilter,IdSetAlloc,CountAlloc>::getConditioningCounts
    ( unsigned int index ) const noexcept {
      return __record_counter.getCounts ( __conditioning_nodesets[index]->second );
    }
    

    /// clears all the data structures from memory
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    void Score<RowFilter,IdSetAlloc,CountAlloc>::clear () {
      __record_counter.clearNodeSets ();

      for ( auto set : __target_nodesets ) delete set;
      for ( auto set : __conditioning_nodesets ) if ( set ) delete set;
      __target_nodesets.clear ();
      __conditioning_nodesets.clear ();
    }

    
  } /* namespace learning */
  
  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

