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
 * @brief the class for computing K2 scores (actually their log2 value)
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
    ScoreK2<IdSetAlloc,CountAlloc>::ScoreK2
    ( const RowFilter& filter,
      const std::vector<unsigned int>& var_modalities ) :
      Score<IdSetAlloc,CountAlloc> ( filter, var_modalities ) {
      // for debugging purposes
      GUM_CONSTRUCTOR ( ScoreK2 );
    }
    

    /// destructor
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    ScoreK2<IdSetAlloc,CountAlloc>::~ScoreK2 () {
      // for debugging purposes
      GUM_DESTRUCTOR ( ScoreK2 );
    }

    
    /// returns the score corresponding to a given nodeset
    template <typename IdSetAlloc, typename CountAlloc>
    float ScoreK2<IdSetAlloc,CountAlloc>::score
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
 
        // the K2 score can be computed as follows:
        // qi log {(ri - 1)!} + sum_j=1^qi [ - log {(N_ij+ri-1)!} +
        //                                   sum_k=1^ri log { N_ijk! } ]

        unsigned int qi = 1;
        for ( unsigned int i = 0; i < conditioning_nodes->size(); ++i ) {
          qi *= modalities[conditioning_nodes->operator[] ( i )];
        }
    
        // precompute the log of (ri - 1)! and add the initial penalty to the score
        unsigned int ri_minus_1 = 1;
        for ( unsigned int i = conditioning_nodes->size ();
              i < all_nodes.size (); ++i ) {
          ri_minus_1 *= modalities[all_nodes[i]] - 1;
        }
        float score = qi * __gammalog2 ( ri_minus_1 + 1 );
       
        for ( unsigned int k = 0; k < targets_modal; ++k ) {
          if ( N_ijk[k] ) {
            score += __gammalog2 ( N_ijk[k] + 1 );           
          }
        }
        for ( unsigned int j = 0; j < conditioning_modal; ++j ) {
          score -= __gammalog2 ( N_ij[j] + ri_minus_1 + 1 );
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

        // the K2 score can be computed as follows:
        // log {(ri - 1)!} - log {(N + ri-1)!} + sum_k=1^ri log { N_ijk! } ]
     
        // precompute the log of (ri - 1)! and add the initial penalty to the score
        unsigned int ri_minus_1 = 1;
        for ( unsigned int i = 0; i < all_nodes.size (); ++i ) {
          ri_minus_1 *= modalities[all_nodes[i]] - 1;
        }
        float score = __gammalog2 ( ri_minus_1 + 1 );
        float N = 0;
        for ( unsigned int k = 0; k < targets_modal; ++k ) {
          if ( N_ijk[k] ) {
            score += __gammalog2 ( N_ijk[k] + 1 );
            N += N_ijk[k];
          }
        }
        score -= __gammalog2 ( N + ri_minus_1 + 1 );

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
