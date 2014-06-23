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
 * @brief the class for estimating parameters of CPTs using Maximum Likelihood
 * + a uniform apriori
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  
  namespace learning {

    
    /// default constructor
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    ParamEstimatorMLwithUniformApriori<RowFilter,IdSetAlloc,CountAlloc>::
    ParamEstimatorMLwithUniformApriori
    ( const RowFilter& filter,
      const std::vector<unsigned int>& var_modalities,
      float apriori ) :
      ParamEstimator<RowFilter,IdSetAlloc,CountAlloc> ( filter, var_modalities ),
      __apriori ( apriori ) {
      if ( apriori < 0 ) {
        GUM_ERROR ( OutOfBounds, "the apriori must be positive" );
      }
      
      // for debugging purposes
      GUM_CONSTRUCTOR ( ParamEstimatorMLwithUniformApriori );
    }

    
    /// destructor
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    ParamEstimatorMLwithUniformApriori<RowFilter,IdSetAlloc,CountAlloc>::
    ~ParamEstimatorMLwithUniformApriori () {
      // for debugging purposes
      GUM_DESTRUCTOR ( ParamEstimatorMLwithUniformApriori );
    }

    
    /// sets the apriori
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    void
    ParamEstimatorMLwithUniformApriori<RowFilter,IdSetAlloc,CountAlloc>::
    setApriori ( float new_apriori ) {
      if ( new_apriori < 0 ) {
        GUM_ERROR ( OutOfBounds, "the apriori must be positive" );
      }
      __apriori = new_apriori;
    }

    
    /// returns the CPT's parameters corresponding to a given nodeset
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    const std::vector<float,CountAlloc>&
    ParamEstimatorMLwithUniformApriori<RowFilter,IdSetAlloc,CountAlloc>::parameters
    ( unsigned int nodeset_index ) {
      // if all_counts is already normalized, just return it
      if ( this->_is_normalized[nodeset_index] ) {
        return this->_getAllCounts ( nodeset_index );
      }
      
      // now, normalize all_counts

      // get the nodes involved in the score as well as their modalities
      const std::vector<unsigned int,IdSetAlloc>& all_nodes =
        this->_getAllNodes ( nodeset_index );
      const std::vector<unsigned int,IdSetAlloc>* conditioning_nodes =
        this->_getConditioningNodes ( nodeset_index );
      unsigned int target_modal =
        this->modalities () [all_nodes[all_nodes.size ()-1]];
      std::vector<float,CountAlloc>& N_ijk = 
        const_cast<std::vector<float,CountAlloc>&>
        ( this->_getAllCounts ( nodeset_index ) );

      // add the apriori
      if ( __apriori ) {
        for ( auto& elt : N_ijk ) {
          elt += __apriori;
        }
      }

      // here, we distinguish nodesets with conditioning nodes from those
      // without conditioning nodes
      if ( conditioning_nodes ) {
        // get the counts for all the targets and for the conditioning nodes
        std::vector<float,CountAlloc>& N_ij =
          const_cast<std::vector<float,CountAlloc>&>
          ( this->_getConditioningCounts ( nodeset_index ) );
        unsigned int conditioning_size = N_ij.size ();

        // if there exists a strictly positive apriori, add it to N_ij
        if ( __apriori ) {
          for ( auto& elt : N_ij ) {
            elt += __apriori;
          }
        }
        else {
          // check that all conditioning nodes have strictly positive counts
          for ( unsigned int j = 0; j < conditioning_size; ++j ) {
            if ( ! N_ij[j] ) {
              GUM_ERROR ( CPTError, "A conditioning set has a value that never "
                          "appears in the database" );
            }
          }
        }

        // normalize the counts
        for ( unsigned int i = 0, k = 0; i < target_modal; ++i ) {
          for ( unsigned int j = 0; j < conditioning_size; ++k, ++j ) {
            N_ijk[k] /= N_ij[j];
          }
        }
      }
      else {
        // here, there are no conditioning nodes
        float sum = 0;
        for ( unsigned int k = 0; k < target_modal; ++k ) {
          sum += N_ijk[k];
        }
        if ( sum ) {
          for ( unsigned int k = 0; k < target_modal; ++k ) {
            N_ijk[k] /= sum;
          }
        }
      }
        

      // counts are normalized
      this->_is_normalized[nodeset_index] = true;
      return N_ijk;
    }
    

  } /* namespace learning */
  
  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
