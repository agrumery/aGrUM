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
 * @brief the class for computing BDeu scores (actually their log2 value)
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
    ScoreBDeu<IdSetAlloc,CountAlloc>::ScoreBDeu
    ( const RowFilter& filter,
      const std::vector<unsigned int>& var_modalities ) :
      Score<IdSetAlloc,CountAlloc> ( filter, var_modalities ) {
      // for debugging purposes
      GUM_CONSTRUCTOR ( ScoreBDeu );
    }
    

    /// destructor
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    ScoreBDeu<IdSetAlloc,CountAlloc>::~ScoreBDeu () {
      // for debugging purposes
      GUM_DESTRUCTOR ( ScoreBDeu );
    }


    /// save an equivalent sample size
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    void ScoreBDeu<IdSetAlloc,CountAlloc>::__insertESS
    ( float equivalent_sample_size,
      unsigned int index ) {
      if ( __ess.size () <= index ) __ess.resize ( index + 1 );
      __ess[index] = equivalent_sample_size <= 0 ? 1 : equivalent_sample_size;
    }
    

    /// add a new single variable to be counted
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int
    ScoreBDeu<IdSetAlloc,CountAlloc>::addNodeSet
    ( unsigned int var,
      float equivalent_sample_size ) {
      unsigned int index =
        Score<IdSetAlloc,CountAlloc>::addNodeSet ( var );
      __insertESS ( equivalent_sample_size, index );
      return index;
    }

      
    /// add a new target variable plus some conditioning vars
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int
    ScoreBDeu<IdSetAlloc,CountAlloc>::addNodeSet
    ( unsigned int var,
      const std::vector<unsigned int>& conditioning_ids,
      float equivalent_sample_size ) {
      unsigned int index = Score<IdSetAlloc,CountAlloc>::addNodeSet
        ( var, conditioning_ids );
      __insertESS ( equivalent_sample_size, index );
      return index;
    }


    /// clears all the data structures from memory
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    void ScoreBDeu<IdSetAlloc,CountAlloc>::clear () {
      Score<IdSetAlloc,CountAlloc>::clear ();
      __ess.clear ();
    }
      
    
    /// returns the score corresponding to a given nodeset
    template <typename IdSetAlloc, typename CountAlloc>
    float ScoreBDeu<IdSetAlloc,CountAlloc>::score
    ( unsigned int nodeset_index ) {
      // if the score has already been computed, get its value
      if ( this->_isInCache ( nodeset_index ) ) {
        return this->_cachedScore ( nodeset_index );
      }

       // get the nodes involved in the score as well as their modalities
      const std::vector<unsigned int,IdSetAlloc>& all_nodes =
        this->_getAllNodes ( nodeset_index );
      const std::vector<unsigned int,IdSetAlloc>* conditioning_nodes =
        this->_getConditioningNodes ( nodeset_index );
      const std::vector<unsigned int>& modalities = this->modalities ();

      // here, we distinguish nodesets with conditioning nodes from those
      // without conditioning nodes
      if ( conditioning_nodes ) {
        // get the counts for all the targets and for the conditioning nodes
        const std::vector<float,CountAlloc>& N_ijk = 
          this->_getAllCounts ( nodeset_index );
        const std::vector<float,CountAlloc>& N_ij = 
          this->_getConditioningCounts ( nodeset_index );
        unsigned int conditioning_modal = N_ij.size ();
        unsigned int targets_modal = N_ijk.size ();
 
        // the BDeu score can be computed as follows:
        // qi * gammalog2 (ess / qi) - ri * qi * gammalog2 (ess / (ri * qi) )
        // - sum_j=1^qi [ gammalog2 ( N_ij + ess / qi ) ]
        // + sum_j=1^qi sum_k=1^ri log [ gammalog2 ( N_ijk + ess / (ri * qi) ) ]

        // precompute qi and ri
        unsigned int qi = 1;
        for ( unsigned int i = 0; i < conditioning_nodes->size(); ++i ) {
          qi *= modalities[conditioning_nodes->operator[] ( i )];
        }
        unsigned int ri = 1;
        for ( unsigned int i = conditioning_nodes->size ();
              i < all_nodes.size (); ++i ) {
          ri *= modalities[all_nodes[i]];
        }
        
        const float ess_qi = __ess[nodeset_index] / qi;
        const float ess_qi_ri = __ess[nodeset_index] / ( qi * ri );
        float score = qi * __gammalog2 ( ess_qi ) -
          ri * qi * __gammalog2 ( ess_qi_ri );
       
        for ( unsigned int j = 0; j < conditioning_modal; ++j ) {
          score -= __gammalog2 ( N_ij[j] + ess_qi );
        }
        for ( unsigned int k = 0; k < targets_modal; ++k ) {
          score += __gammalog2 ( N_ijk[k] + ess_qi_ri );
        }

        // shall we put the score into the cache?
        if ( this->_isUsingCache () ) {
          this->_insertIntoCache ( nodeset_index, score );
        }

        return score;
      }
      else {
        // here, there are no conditioning nodes

        // get the counts for all the targets and for the conditioning nodes
        const std::vector<float,CountAlloc>& N_ijk = 
          this->_getAllCounts ( nodeset_index );
        unsigned int targets_modal = N_ijk.size ();

        // the BDeu score can be computed as follows:
        // gammalog2 ( ess ) - ri * gammalog2 (ess / ri ) - gammalog2 ( N + ess )
        // + sum_k=1^ri log [ gammalog2 ( N_ijk + ess / ri ) ]
     
        // precompute ri
        unsigned int ri = 1;
        for ( unsigned int i = 0; i < all_nodes.size (); ++i ) {
          ri *= modalities[all_nodes[i]];
        }
        const float ess = __ess[nodeset_index];
        const float ess_ri = __ess[nodeset_index] / ri;
        float score = __gammalog2 ( ess ) - ri * __gammalog2 ( ess_ri );
        float N = 0;
        for ( unsigned int k = 0; k < targets_modal; ++k ) {
          score += __gammalog2 ( N_ijk[k] + ess_ri );
          N += N_ijk[k];
        }
        score -= __gammalog2 ( N + ess );

        // shall we put the score into the cache?
        if ( this->_isUsingCache () ) {
          this->_insertIntoCache ( nodeset_index, score );
        }

        return score;
      }
    }

  } /* namespace learning */
  
  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
