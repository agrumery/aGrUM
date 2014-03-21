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
 * @brief the class for computing Bayesian Dirichlet (BD) log2 scores
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


#include <cmath>


namespace gum {

  
  namespace learning {

    
    /// default constructor
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    ScoreBD<RowFilter,IdSetAlloc,CountAlloc>::ScoreBD
    ( const RowFilter& filter,
      const std::vector<unsigned int>& var_modalities ) :
      Score<RowFilter,IdSetAlloc,CountAlloc> ( filter, var_modalities ) {
      // for debugging purposes
      GUM_CONSTRUCTOR ( ScoreBD );
    }
    

    /// destructor
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    ScoreBD<RowFilter,IdSetAlloc,CountAlloc>::~ScoreBD () {
      // for debugging purposes
      GUM_DESTRUCTOR ( ScoreBD );
    }


    /// save an equivalent sample size
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    void ScoreBD<RowFilter,IdSetAlloc,CountAlloc>::__insertHyperParam
    ( const std::vector<float>& N_prime_ijk,
      unsigned int index ) {
      if ( __N_prime_ijk.size () <= index ) __N_prime_ijk.resize ( index + 1 );
      __N_prime_ijk[index] = N_prime_ijk;
    }


    /// checks that the size of a given N_prime_ijk is correct
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    void ScoreBD<RowFilter,IdSetAlloc,CountAlloc>::__checkHyperParam
    ( unsigned int var,
      const std::vector<unsigned int>& conditioning_ids,
      const std::vector<float>& N_prime_ijk ) const {
      const std::vector<unsigned int>& modal = modalities ();
      unsigned int size = modal[var];
      for ( const auto id : conditioning_ids ) {
        size *= modal[id];
      }
      if ( size != N_prime_ijk.size () )
        GUM_ERROR ( SizeError, "N_prime_ijk does not have the correct size" );
    }
      

    /// add a new single variable to be counted
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int
    ScoreBD<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
    ( unsigned int var,
      const std::vector<float>& N_prime_ijk ) {
      __checkHyperParam ( var, __empty_vect, N_prime_ijk );
      unsigned int index =
        Score<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet ( var );
      __insertHyperParam ( N_prime_ijk, index );
      return index;
    }

      
    /// add a new target variable plus some conditioning vars
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int
    ScoreBD<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
    ( unsigned int var,
      const std::vector<unsigned int>& conditioning_ids,
      const std::vector<float>& N_prime_ijk ) {
      __checkHyperParam ( var, conditioning_ids, N_prime_ijk );
      unsigned int index =
        Score<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
        ( var, conditioning_ids );
      __insertHyperParam ( N_prime_ijk, index );
      return index;
    }


    /// clears all the data structures from memory
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    void ScoreBD<RowFilter,IdSetAlloc,CountAlloc>::clear () {
      Score<RowFilter,IdSetAlloc,CountAlloc>::clear ();
      __N_prime_ijk.clear ();
    }

    
    /// returns the score corresponding to a given nodeset
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    float ScoreBD<RowFilter,IdSetAlloc,CountAlloc>::score
    ( unsigned int nodeset_index ) {
      // get the nodes involved in the score as well as their modalities
      const std::vector<unsigned int,IdSetAlloc>* conditioning_nodes =
        this->_getConditioningNodes ( nodeset_index );

      // here, we distinguish nodesets with conditioning nodes from those
      // without conditioning nodes
      if ( conditioning_nodes ) {
        // get the counts for all the targets and for the conditioning nodes
        const std::vector<float,CountAlloc>& N_ijk = 
          this->_getAllCounts ( nodeset_index );
        const std::vector<float,CountAlloc>& N_ij = 
          this->_getConditioningCounts ( nodeset_index );
        const std::vector<float>& N_prime_ijk =
          __N_prime_ijk[nodeset_index];
        unsigned int conditioning_modal = N_ij.size ();
        unsigned int targets_modal = N_ijk.size ();
 
        // the BD score can be computed as follows:
        // sum_j=1^qi [ gammalog2 ( N'_ij ) - gammalog2 ( N_ij + N'_ij )
        //              + sum_k=1^ri { gammlog2 ( N_ijk + N'_ijk ) -
        //                             gammalog2 ( N'_ijk ) } ]

        // precompute N'_ij
        std::vector<float> N_prime_ij ( conditioning_modal, 0.0f );
        for ( unsigned int i = 0, j = 0; i < targets_modal;
              ++i, j = ( ++j == conditioning_modal ) ? 0 : j ) {
          N_prime_ij[j] += N_prime_ijk[i];
        }

        float score = 0;
        for ( unsigned int j = 0; j < conditioning_modal; ++j ) {
          score += __gammalog2 ( N_prime_ij[j] ) -
            __gammalog2 ( N_ij[j] + N_prime_ij[j] );
        }
        for ( unsigned int k = 0; k < targets_modal; ++k ) {
          score += __gammalog2 ( N_ijk[k] + N_prime_ijk[k] ) -
            __gammalog2 ( N_prime_ijk[k] );
        }

        return score;
      }
      else {
        // here, there are no conditioning nodes

        // get the counts for all the targets and for the conditioning nodes
        const std::vector<float,CountAlloc>& N_ijk = 
          this->_getAllCounts ( nodeset_index );
        const std::vector<float>& N_prime_ijk =
          __N_prime_ijk[nodeset_index];
        unsigned int targets_modal = N_ijk.size ();

        // the BD score can be computed as follows:
        // gammalog2 ( N' ) - gammalog2 ( N + N' )
        // + sum_k=1^ri { gammlog2 ( N_i + N'_i ) - gammalog2 ( N'_i ) }
     
        // precompute N'
        float N_prime = 0;
        for ( const auto elt : N_prime_ijk ) N_prime += elt;
        
        float score = __gammalog2 ( N_prime );
        float N = 0;
        for ( unsigned int k = 0; k < targets_modal; ++k ) {
          score += __gammalog2 ( N_ijk[k] + N_prime_ijk[k] ) -
            __gammalog2 ( N_prime_ijk[k] );
          N += N_ijk[k];
        }
        score -= __gammalog2 ( N + N_prime );

        return score;
      }
    }

  } /* namespace learning */
  
  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
