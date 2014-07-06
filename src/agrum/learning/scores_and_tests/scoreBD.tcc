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
    template <typename IdSetAlloc, typename CountAlloc>
    template <typename RowFilter> INLINE
    ScoreBD<IdSetAlloc,CountAlloc>::ScoreBD
    ( const RowFilter& filter,
      const std::vector<unsigned int>& var_modalities,
      Apriori<IdSetAlloc,CountAlloc>& apriori ) :
      Score<IdSetAlloc,CountAlloc> ( filter, var_modalities, apriori ) {
      // check that the apriori is compatible with the score
      if ( ! apriori.isOfType ( AprioriDirichletType::type ) &&
           ! apriori.isOfType ( AprioriSmoothingType::type ) &&
           ! apriori.isOfType ( AprioriNoAprioriType::type ) ) {
        GUM_ERROR ( InvalidArgument, "The apriori is incompatible with the BD "
                    "score: shall be Dirichlet, smoothing or no apriori" );
      }
      
      // for debugging purposes
      GUM_CONSTRUCTOR ( ScoreBD );
    }
    

    /// copy constructor
    template <typename IdSetAlloc, typename CountAlloc>
    ScoreBD<IdSetAlloc,CountAlloc>::ScoreBD
    ( const ScoreBD<IdSetAlloc,CountAlloc>& from ) :
      Score<IdSetAlloc,CountAlloc> ( from ),
      __gammalog2 ( from.__gammalog2 ) {
      // for debugging purposes
      GUM_CONS_CPY ( ScoreBD );
    }
      

    /// move constructor
    template <typename IdSetAlloc, typename CountAlloc>
    ScoreBD<IdSetAlloc,CountAlloc>::ScoreBD
    ( ScoreBD<IdSetAlloc,CountAlloc>&& from ) :
      Score<IdSetAlloc,CountAlloc> ( std::move ( from ) ),
      __gammalog2 ( std::move ( from.__gammalog2 ) ) {
      // for debugging purposes
      GUM_CONS_MOV ( ScoreBD );
    }
      

    /// virtual copy factory
    template <typename IdSetAlloc, typename CountAlloc>
    ScoreBD<IdSetAlloc,CountAlloc>*
    ScoreBD<IdSetAlloc,CountAlloc>::copyFactory () const {
      return new ScoreBD<IdSetAlloc,CountAlloc> ( *this );
    }

    
    /// destructor
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    ScoreBD<IdSetAlloc,CountAlloc>::~ScoreBD () {
      // for debugging purposes
      GUM_DESTRUCTOR ( ScoreBD );
    }
    
    
    /// returns the score corresponding to a given nodeset
    template <typename IdSetAlloc, typename CountAlloc>
    float ScoreBD<IdSetAlloc,CountAlloc>::score
    ( unsigned int nodeset_index ) {
      // if the score has already been computed, get its value
      if ( this->_isInCache ( nodeset_index ) ) {
        return this->_cachedScore ( nodeset_index );
      }

      // get the counts for all the targets and the conditioning nodes
      const std::vector<float,CountAlloc>& N_ijk = 
        this->_getAllCounts ( nodeset_index );
      unsigned int targets_modal = N_ijk.size ();
      float score = 0;

      // here, we distinguish nodesets with conditioning nodes from those
      // without conditioning nodes
      if ( this->_getConditioningNodes ( nodeset_index ) ) {
        // get the count of the conditioning nodes
        const std::vector<float,CountAlloc>& N_ij = 
          this->_getConditioningCounts ( nodeset_index );
        unsigned int conditioning_modal = N_ij.size ();
        
        if ( this->_apriori->weight () ) {
          const std::vector<float,CountAlloc>& N_prime_ijk = 
            this->_getAllApriori ( nodeset_index );
          const std::vector<float,CountAlloc>& N_prime_ij = 
            this->_getConditioningApriori ( nodeset_index );
          
          // the BD score can be computed as follows:
          // sum_j=1^qi [ gammalog2 ( N'_ij ) - gammalog2 ( N_ij + N'_ij )
          //              + sum_k=1^ri { gammlog2 ( N_ijk + N'_ijk ) -
          //                             gammalog2 ( N'_ijk ) } ]

          for ( unsigned int j = 0; j < conditioning_modal; ++j ) {
            if ( N_prime_ij[j] ) {
              score += __gammalog2 ( N_prime_ij[j] ) -
                __gammalog2 ( N_ij[j] + N_prime_ij[j] );
            }
            else if ( N_ij[j] ) {
              score += __gammalog2 ( N_ij[j] );
            }
          }
          for ( unsigned int k = 0; k < targets_modal; ++k ) {
            if ( N_prime_ijk[k] ) {
              score += __gammalog2 ( N_ijk[k] + N_prime_ijk[k] ) -
                __gammalog2 ( N_prime_ijk[k] );
            }
            else if ( N_ijk[k] ) {
              score += __gammalog2 ( N_ijk[k] );
            }
          }
        }
        else {
          // the BD score can be computed as follows:
          // sum_j=1^qi [ - gammalog2 ( N_ij )
          //              + sum_k=1^ri { gammlog2 ( N_ijk ) } ]

          for ( unsigned int j = 0; j < conditioning_modal; ++j ) {
            if ( N_ij[j] ) score -= __gammalog2 ( N_ij[j] );
          }
          for ( unsigned int k = 0; k < targets_modal; ++k ) {
            if ( N_ijk[k] ) score += __gammalog2 ( N_ijk[k] );
          }
        }
          
        // shall we put the score into the cache?
        if ( this->_isUsingCache () ) {
          this->_insertIntoCache ( nodeset_index, score );
        }
        
        return score;
      }
      else {
        if ( this->_apriori->weight () ) {
          const std::vector<float,CountAlloc>& N_prime_ijk = 
            this->_getAllApriori ( nodeset_index );

          // the BD score can be computed as follows:
          // gammalog2 ( N' ) - gammalog2 ( N + N' )
          // + sum_k=1^ri { gammlog2 ( N_i + N'_i ) - gammalog2 ( N'_i ) }
     
          // precompute N'
          float N_prime = 0;
          for ( const auto elt : N_prime_ijk ) N_prime += elt;
        
          score = __gammalog2 ( N_prime );
          float N = 0;
          for ( unsigned int k = 0; k < targets_modal; ++k ) {
            if ( N_prime_ijk[k] ) {
              score += __gammalog2 ( N_ijk[k] + N_prime_ijk[k] ) -
                __gammalog2 ( N_prime_ijk[k] );
            }
            else if ( N_ijk[k] ) {
              score += __gammalog2 ( N_ijk[k] );
            }
            N += N_ijk[k];
          }
          score -= __gammalog2 ( N + N_prime );
        }
        else {
          // the BD score can be computed as follows:
          // - gammalog2 ( N ) + sum_k=1^ri { gammlog2 ( N_i ) }
          
          float N = 0;
          for ( unsigned int k = 0; k < targets_modal; ++k ) {
            if ( N_ijk[k] ) {
              score += __gammalog2 ( N_ijk[k] );
              N += N_ijk[k];
            }
          }
          score -= __gammalog2 ( N );
        }
        
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
