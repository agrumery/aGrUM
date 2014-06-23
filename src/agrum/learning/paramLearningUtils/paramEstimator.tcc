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
 * @brief the base class for estimating parameters of CPTs
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {

  
  namespace learning {

    
    /// default constructor
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    ParamEstimator<RowFilter,IdSetAlloc,CountAlloc>::ParamEstimator
    ( const RowFilter& filter,
      const std::vector<unsigned int>& var_modalities ) :
      Counter<RowFilter,IdSetAlloc,CountAlloc> ( filter, var_modalities ) {
      GUM_CONSTRUCTOR ( ParamEstimator );
    }


    /// destructor
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    ParamEstimator<RowFilter,IdSetAlloc,CountAlloc>::~ParamEstimator () {
      GUM_DESTRUCTOR ( ParamEstimator );
    }


    /// add a new CPT with a single variable to be estimated 
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int
    ParamEstimator<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
    ( unsigned int var ) {
      _is_normalized.push_back ( false );
      return Counter<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet ( var );
    }


    /// add a new target variable plus some conditioning vars
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int
    ParamEstimator<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
    ( unsigned int var,
      const std::vector<unsigned int>& conditioning_ids ) {
      _is_normalized.push_back ( false );
      return Counter<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
        ( var,  conditioning_ids );
    }


    
    /// clears all the data structures from memory
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    void ParamEstimator<RowFilter,IdSetAlloc,CountAlloc>::clear () {
      Counter<RowFilter,IdSetAlloc,CountAlloc>::clear ();
      _is_normalized.clear ();
    }
    

    /// sets the CPT's parameters corresponding to a given nodeset
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    void ParamEstimator<RowFilter,IdSetAlloc,CountAlloc>::setParameters
    ( unsigned int nodeset_index,
      Potential<float>& pot ) {
      const std::vector<float,CountAlloc>& normalized_counts =
        parameters ( nodeset_index );
      pot.fillWith ( normalized_counts );
    }


  } /* namespace learning */
  
  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

