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
 * @brief the class for all the independence tests with symmetric scores
 *
 * The class should be used as follows: first, to speed-up computations, you
 * should consider computing all the scores conditioned to a given set of
 * nodes in one pass. To do so, use the appropriate computeScores method. This
 * one will compute everything you need. The computeScores methods where you
 * do not specify a set of conditioning nodes assume that this set is empty.
 * If available memory is limited, use the setMaxSize method to constrain the
 * memory that will be used for these computations. Once the computations
 * have been performed, use methods score to retrieve the scores computed.
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_LEARNING_SYMMETRIC_INDEPENDENCE_TEST_H
#define GUM_LEARNING_SYMMETRIC_INDEPENDENCE_TEST_H


#include <agrum/learning/independenceTest.h>


namespace gum {

  
  namespace learning {

    
    /* ========================================================================= */
    /* ========================================================================= */
    /* ===                 SYMMETRIC INDEPENDENCE TEST CLASS                 === */
    /* ========================================================================= */
    /* ========================================================================= */
    /** @class SymmetricIndependenceTest the class for all the independence tests
     * with symmetric scores
     *
     * The class should be used as follows: first, to speed-up computations, you
     * should consider computing all the scores conditioned to a given set of
     * nodes in one pass. To do so, use the appropriate computeScores method. This
     * one will compute everything you need. The computeScores methods where you
     * do not specify a set of conditioning nodes assume that this set is empty.
     * If available memory is limited, use the setMaxSize method to constrain the
     * memory that will be used for these computations. Once the computations
     * have been performed, use methods score to retrieve the scores computed. */
    /* ========================================================================= */
    class SymmetricIndependenceTest : public IndependenceTest {
    public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param database the database from which the scores will be computed
       * @param max_tree_size the scores are computed using a CountingTree.
       * Parameter max_tree_size indicates which maximal size in bytes the tree
       * should have. This number is used approximately, i.e., we do not count
       * precisely the number of bytes used but we count them roughly. */
      SymmetricIndependenceTest ( const Database& database,
                                  unsigned int max_tree_size = 0  );

      /// destructor
      virtual ~SymmetricIndependenceTest ();

      /// @}

      
    protected:
      /// a function that determines the db single ids needed for the score
      virtual void _computeInducedSingleIds
      ( const std::vector< std::pair<unsigned int, unsigned int> >& db_pair_ids );
     
    };
    

  } /* namespace learning */
  
  
} /* namespace gum */


/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/symmetricIndependenceTest.inl>
#endif /* GUM_NO_INLINE */


#endif /* GUM_LEARNING_SYMMETRIC_INDEPENDENCE_TEST_H */
