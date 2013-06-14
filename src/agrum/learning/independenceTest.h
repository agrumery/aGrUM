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

#ifndef GUM_LEARNING_INDEPENDENCE_TEST_H
#define GUM_LEARNING_INDEPENDENCE_TEST_H


#include <agrum/learning/score.h>


namespace gum {

  
  namespace learning {

    
    /* ========================================================================= */
    /* ========================================================================= */
    /* ===                      INDEPENDENCE TEST CLASS                      === */
    /* ========================================================================= */
    /* ========================================================================= */
    /** @class IndependenceTest the abstract class for all the independence tests
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
    class IndependenceTest : protected Score {
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
      IndependenceTest ( const Database& database,
                         unsigned int max_tree_size = 0 );
      
      /// destructor
      virtual ~IndependenceTest ();

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// modifies the max size of the counting trees
      /** This function sets the maximal size in bytes that the CountingTree
       * used to compute score should have. This number is used approximately,
       * i.e., we do not count precisely the number of bytes used but we count
       * them roughly. */
      void setMaxSize ( unsigned int new_size );
 
      /// computes the "unconditional" scores of a set of pairs of targets
      /** The method computes the scores of a set of pairs of target nodes
       * unconditionally, i.e., the target nodes have no conditioning node. The
       * score of a pair (X,Y) of target nodes corresponds to the score of Y 
       * conditioned to X. As such, it could be computed by putting X into
       * the set of conditioning nodes and Y into that of target nodes. However,
       * in order to avoid parsing too many times the database, it is more
       * efficient to specify this score as a pair of target node because it
       * enables to factorize the parsing of the database among several scores.
       * For instance, assume that we wish to test the scores of setting a node
       * X_1 or X_2 or ... or X_n as the parent of a node Y, then we simply 
       * need to specify the set of target nodes {(X_1,Y),...,(X_n,Y)}.
       *
       * The nodes are identified by their id in the database. Their scores can
       * be retrieved individually using method score.
       * @warning the method uses a reference to db_pair_ids and does
       * not copy it in order to speed-up computations. Be careful not to
       * delete this vector from memory before calling the score method. */
      void computeScores
      ( const std::vector< std::pair<unsigned int,
                                     unsigned int> >& db_pair_ids );
      
      /// compute the scores of the set of targets conditioned on some nodes
      /** The method computes the scores of a set of pairs of target nodes
       * conditionally to some other nodes. The score of a pair (X,Y) of target
       * nodes conditionally to a set Z corresponds to the score of Y 
       * conditioned to Z U {X}. As such, it could be computed by putting X into
       * the set of conditioning nodes and Y into that of target nodes. However,
       * in order to avoid parsing too many times the database, it is more
       * efficient to specify this score as a pair of target node because it
       * enables to factorize the parsing of the database among several scores.
       * For instance, assume that we wish to test the scores of setting a node
       * X_1 or X_2 or ... or X_n as the parent of a node Y, then we simply 
       * need to specify the set of target nodes {(X_1,Y),...,(X_n,Y)}.
       *
       * The nodes are identified by their id in the database. Their scores can
       * be retrieved individually using method score.
       * @warning the method uses a reference to db_pair_ids and
       * db_conditioning_ids, and it does not copy these vectors in order to
       * speed-up computations. So, be careful not to delete these vectors from
       * memory before calling the score method.*/
      void computeScores
      ( const std::vector<unsigned int>& db_conditioning_ids,
        const std::vector< std::pair<unsigned int,
                                     unsigned int> >& db_pair_ids );

      /// returns the score of a given pair X,Y given the conditioning nodes
      /** This method returns the score of Y conditionally to X plus the set of
       * conditioning nodes specified in the last computeScores execution.
       * @warning This function should be called only after a
       * computeScores ( db_pair_ids ) or a
       * computeScores ( db_conditioning_ids, db_pair_ids ) 
       * has been completed, else its result is undetermined and is most probably
       * an exception raised. */
      float score ( const std::pair<unsigned int,unsigned int>& XY_pair ) const;

      /// @}


    protected:
      // ##########################################################################
      // the real computations of the score given an already constructed tree
      // ##########################################################################

      /// a function that determines the db single ids needed for the score
      virtual void _computeInducedSingleIds
      ( const std::vector< std::pair<unsigned int,
                                     unsigned int> >& db_pair_ids ) = 0;


    private:
      /// prevent the use of the function computing the scores for single targets
      void _computeScores ( const std::vector<unsigned int>& );

      /// prevent copy constructor
      IndependenceTest ( const IndependenceTest& );

      /// prevent copy operator
      IndependenceTest& operator= ( const IndependenceTest& );

    };


  } /* namespace learning */
  
  
} /* namespace gum */


/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/independenceTest.inl>
#endif /* GUM_NO_INLINE */


#endif /* GUM_LEARNING_INDEPENDENCE_TEST_H */
