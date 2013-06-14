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


#include <agrum/learning/independenceTest.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#include <agrum/learning/independenceTest.inl>
#endif /* GUM_NO_INLINE */


namespace gum {

  
  namespace learning {

    
    /// default constructor
    IndependenceTest::IndependenceTest ( const Database& database,
                                         unsigned int max_tree_size ) :
      Score ( database, max_tree_size ) {
      // for debugging purposes
      GUM_CONSTRUCTOR ( IndependenceTest );
    }
      

    /// destructor
    IndependenceTest::~IndependenceTest () {
      // for debugging purposes
      GUM_DESTRUCTOR ( IndependenceTest );
    }


    /// prevent the use of the function computing the scores for single targets
    void IndependenceTest::_computeScores ( const std::vector<unsigned int>& ) {
    }

    
  } /* namespace learning */
  
  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

