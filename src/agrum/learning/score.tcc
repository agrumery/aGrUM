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

 

namespace gum {

  
  namespace learning {


    /// default constructor
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    Score<RowFilter,IdSetAlloc,CountAlloc>::Score
    ( const RowFilter& filter,
      const std::vector<unsigned int>& var_modalities ) :
      Counter<RowFilter,IdSetAlloc,CountAlloc> ( filter, var_modalities ) {
      GUM_CONSTRUCTOR ( Score );
    }

    
    /// destructor
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    Score<RowFilter,IdSetAlloc,CountAlloc>::~Score () {
      GUM_DESTRUCTOR ( Score );
    }


    /// add a new single variable to be counted
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int Score<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
    ( unsigned int var ) {
      return Counter<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet ( var );
    }


    /// add a new target variable plus some conditioning vars
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int
    Score<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
    ( unsigned int var,
      const std::vector<unsigned int>& conditioning_ids ) {
      return Counter<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
        ( var,  conditioning_ids );
    }

    
    /// add new set of "unconditioned" single targets
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int
    Score<RowFilter,IdSetAlloc,CountAlloc>::addNodeSets
    ( const std::vector<unsigned int>& single_vars ) {
      return Counter<RowFilter,IdSetAlloc,CountAlloc>::addNodeSets
        ( single_vars );
    }

    
    /// add new set of "conditioned" single targets
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int
    Score<RowFilter,IdSetAlloc,CountAlloc>::addNodeSets
    ( const std::vector<unsigned int>& single_vars,
      const std::vector<unsigned int>& conditioning_ids ) {
      return Counter<RowFilter,IdSetAlloc,CountAlloc>::addNodeSets
        ( single_vars, conditioning_ids );
    }
    
     
  } /* namespace learning */
  
  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

