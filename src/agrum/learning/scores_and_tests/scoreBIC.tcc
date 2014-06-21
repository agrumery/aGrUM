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
 * @brief the class for computing BIC scores
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


#include <agrum/learning/scores_and_tests/scoreBIC.h>


namespace gum {

  
  namespace learning {

    
    /// default constructor
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    ScoreBIC<RowFilter,IdSetAlloc,CountAlloc>::ScoreBIC
    ( const RowFilter& filter,
      const std::vector<unsigned int>& var_modalities ) :
      Score<RowFilter,IdSetAlloc,CountAlloc> ( filter, var_modalities ) {
      // for debugging purposes
      GUM_CONSTRUCTOR ( ScoreBIC );
    }
 

    /// destructor
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    ScoreBIC<RowFilter,IdSetAlloc,CountAlloc>::ScoreBIC::~ScoreBIC () {
      // for debugging purposes
      GUM_DESTRUCTOR ( ScoreBIC );
    }


    /// returns the score corresponding to a given nodeset
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    float ScoreBIC<RowFilter,IdSetAlloc,CountAlloc>::score
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
        // initialize the score: this should be the penalty of the BIC score, i.e.,
        // -(ri-1 ) * qi
        float penalty = 1;
        for ( unsigned int i = 0; i < conditioning_nodes->size(); ++i ) {
          penalty *= modalities[conditioning_nodes->operator[] ( i )];
        }
        for ( unsigned int i = conditioning_nodes->size();
              i < all_nodes.size (); ++i ) {
          penalty *= modalities[all_nodes[i]] - 1;
        }

        // get the counts for all the targets and for the conditioning nodes
        const std::vector<float,CountAlloc>& N_ijk = 
          this->_getAllCounts ( nodeset_index );
        const std::vector<float,CountAlloc>& N_ij = 
          this->_getConditioningCounts ( nodeset_index );
        unsigned int conditioning_modal = N_ij.size ();
        unsigned int targets_modal = N_ijk.size ();

        // compute the score: it remains to compute the log likelihood, i.e.,
        // sum_k=1^r_i sum_j=1^q_i N_ijk log (N_ijk / N_ij), which is also
        // equivalent to:
        // sum_j=1^q_i sum_k=1^r_i N_ijk log N_ijk - sum_j=1^q_i N_ij log N_ij
        float score = 0;
        for ( unsigned int k = 0; k < targets_modal; ++k ) {
          if ( N_ijk[k] ) {
            score += N_ijk[k] * logf ( N_ijk[k] );
          }
        }
        float N = 0;
        for ( unsigned int j = 0; j < conditioning_modal; ++j ) {
          if ( N_ij[j] ) {
            score -= N_ij[j] * logf ( N_ij[j] );
            N += N_ij[j];
          }
        }
  
        // finally, remove the penalty
        score -= penalty * logf ( N ) * 0.5f;
 
        // divide by log(2), since the log likelihood uses log_2
        score *= this->_1log2;

        // shall we put the score into the cache?
        if ( this->_isUsingCache () ) {
          this->_insertIntoCache ( nodeset_index, score );
        }

        return score;
      }
      else {
        // here, there are no conditioning nodes

        // initialize the score: this should be the penalty of the AIC score, i.e.,
        // -(ri-1 )
        float penalty = 1;
        for ( unsigned int i = 0; i < all_nodes.size (); ++i ) {
          penalty *= modalities[all_nodes[i]] - 1;
        }

        // get the counts for all the targets and for the conditioning nodes
        const std::vector<float,CountAlloc>& N_ijk =
          this->_getAllCounts ( nodeset_index );
        unsigned int targets_modal = N_ijk.size ();
 
        // compute the score: it remains to compute the log likelihood, i.e.,
        // sum_k=1^r_i N_ijk log (N_ijk / N), which is also
        // equivalent to:
        // sum_j=1^q_i sum_k=1^r_i N_ijk log N_ijk - N log N
        float score = 0;
        float N = 0;
        for ( unsigned int k = 0; k < targets_modal; ++k ) {
          if ( N_ijk[k] ) {
            score += N_ijk[k] * logf ( N_ijk[k] );
            N += N_ijk[k];
          }
        }
        score -= N * logf ( N );
        
        // finally, remove the penalty
        score -= penalty * logf ( N ) * 0.5f;

        // divide by log(2), since the log likelihood uses log_2
        score *= this->_1log2;

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
