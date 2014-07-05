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
 * @brief  the smooth a priori: adds a weight w to all the countings
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {

  
  namespace learning {

     
    /// default constructor
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    AprioriSmoothing<IdSetAlloc,CountAlloc>::AprioriSmoothing () {
      GUM_CONSTRUCTOR ( AprioriSmoothing );
    }


    /// copy constructor
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    AprioriSmoothing<IdSetAlloc,CountAlloc>::AprioriSmoothing
    ( const AprioriSmoothing<IdSetAlloc,CountAlloc>& from ) :
      Apriori<IdSetAlloc,CountAlloc> ( from ) {
      GUM_CONS_CPY ( AprioriSmoothing );
    }
      

    /// move constructor
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    AprioriSmoothing<IdSetAlloc,CountAlloc>::AprioriSmoothing
    ( AprioriSmoothing<IdSetAlloc,CountAlloc>&& from ) :
      Apriori<IdSetAlloc,CountAlloc> ( std::move ( from ) ) {
      GUM_CONS_MOV ( AprioriSmoothing );
    }


    /// virtual copy constructor
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    AprioriSmoothing<IdSetAlloc,CountAlloc>*
    AprioriSmoothing<IdSetAlloc,CountAlloc>::copyFactory () const {
      return new AprioriSmoothing<IdSetAlloc,CountAlloc> ( *this );
    }


    /// destructor
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    AprioriSmoothing<IdSetAlloc,CountAlloc>::~AprioriSmoothing () {
      GUM_DESTRUCTOR ( AprioriSmoothing );
    }

    
    /// include the apriori into a given set of counts
    template <typename IdSetAlloc, typename CountAlloc> INLINE
    void AprioriSmoothing<IdSetAlloc,CountAlloc>::addApriori
    ( const std::vector<unsigned int>& modalities,
      std::vector< std::vector<float,CountAlloc> >& counts,
      const std::vector< std::pair<std::vector<unsigned int,IdSetAlloc>,
                                   unsigned int>* >& target_nodesets,
      const std::vector< std::pair<std::vector<unsigned int,IdSetAlloc>,
                                   unsigned int>* >& conditioning_nodesets ) {
      if ( this->_weight != 0 ) {
        if ( this->_weight != 1 ) {
          for ( auto& countings : counts ) {
            for ( auto& count : countings ) {
              count += this->_weight;
            }
          }
        }
        else {
          for ( auto& countings : counts ) {
            for ( auto& count : countings ) {
              ++count;
            }
          }
        }
      }
    }


  } /* namespace learning */
  
  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

