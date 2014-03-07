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
 * @brief the abstract class for all the independence tests
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {

  
  namespace learning {

    
    /// default constructor
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    IndependenceTest<RowFilter,IdSetAlloc,CountAlloc>::IndependenceTest
    ( const RowFilter& filter,
      const std::vector<unsigned int>& var_modalities ) :
      Counter<RowFilter,IdSetAlloc,CountAlloc> ( filter, var_modalities ) {
      GUM_CONSTRUCTOR ( IndependenceTest );
    }

    
    /// destructor
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    IndependenceTest<RowFilter,IdSetAlloc,CountAlloc>::~IndependenceTest () {
      GUM_DESTRUCTOR ( IndependenceTest );
    }


    /// add a new pair of target unconditioned variables to be counted
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int
    IndependenceTest<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
    ( unsigned int var1,
      unsigned int var2 ) {
      unsigned int index = Counter<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
        ( var1, var2 );
      Counter<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet ( var1 );
      return index;
    }

    
    /// add a new pair of target unconditioned variables to be counted
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int
    IndependenceTest<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
    ( const std::pair<unsigned int,unsigned int>& vars ) {
      unsigned int index = Counter<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
        ( vars );
      Counter<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet ( vars.first );
      return index;
    }


    /// add a new pair of target conditioned variables to be counted
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int
    IndependenceTest<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
    ( unsigned int var1,
      unsigned int var2,
      const std::vector<unsigned int>& conditioning_ids ) {
      unsigned int index = Counter<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
        ( var1, var2, conditioning_ids );
      Counter<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
        ( var1, conditioning_ids );
      return index;
    }

    
    /// add a new pair of target conditioned variables to be counted
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int
    IndependenceTest<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
    ( const std::pair<unsigned int,unsigned int>& vars,
      const std::vector<unsigned int>& conditioning_ids ) {
      unsigned int index = Counter<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
        ( vars, conditioning_ids );
      Counter<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
        ( vars.first, conditioning_ids );
      return index;
    }

    
    /// add new set of "unconditioned" pairs of targets
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int
    IndependenceTest<RowFilter,IdSetAlloc,CountAlloc>::addNodeSets
    ( const std::vector< std::pair<unsigned int,unsigned int> >& vars ) {
      unsigned int index = addNodeSets ( vars[0] );
      for ( unsigned int i = 1; i < vars.size (); ++i ) { 
        addNodeSets ( vars[i] );
      }
      return index;
    }

    
    /// add new set of "conditioned" pairs of targets
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    unsigned int
    IndependenceTest<RowFilter,IdSetAlloc,CountAlloc>::addNodeSets
    ( const std::vector< std::pair<unsigned int,unsigned int> >& vars,
      const std::vector<unsigned int>& conditioning_ids ) {
      unsigned int index = addNodeSets ( vars[0], conditioning_ids );
      for ( unsigned int i = 1; i < vars.size (); ++i ) { 
        addNodeSets ( vars[i], conditioning_ids );
      }
      return index;
    }

    
  } /* namespace learning */
  
  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

