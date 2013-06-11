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
 * @brief the base class for all the scores used for learning (BIC, BDeu, etc)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_LEARNING_SCORE_H
#define GUM_LEARNING_SCORE_H


#include <vector>
#include <agrum/config.h>
#include <agrum/learning/database.h>
#include <agrum/learning/countingTree.h>


namespace gum {

  
  namespace learning {
    
    /* ========================================================================= */
    /* ========================================================================= */
    /* ===                            SCORE CLASS                            === */
    /* ========================================================================= */
    /* ========================================================================= */
    /** @class Score The base class for all the scores used for learning
     * (BIC, BDeu, etc)
     *
     * The class provides a common interface for all the scores that treat
     * nodes in an asymmetric manner. For instance, in the log likelihood score,
     * the formula to be computed involves N_ijk * log ( N_ijk / N_ij ), where
     * N_ijk crresponds to the number of occurrences of the jth value of variable
     * i and the kth value of the parents of variable i, and N_ij is the same
     * except that we do not care the value of the parents. As such, variable
     * i receives a treatment that is different from its parents and this thus
     * leads to an asymmetric score. */
    /* ========================================================================= */
    class Score {
    public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      Score ( const Database& database );

      /// destructor
      ~Score ();

      /// @}

      
      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{
 
      /// computes the "unconditional" scores of a set of single targets
      /** The method computes the scores of a set of single target nodes
       * unconditionally, i.e., the target nodes have no conditioning node.
       *
       * The nodes are identified by their id in the database. Their scores can
       * be retrieved individually using method score.
       * @warning the method uses a reference to db_single_ids and does
       * not copy it in order to speed-up computations. Be careful not to
       * delete this vector from memory before calling the score method. */
      virtual void
      computeScores ( const std::vector<unsigned int>& db_single_ids ) = 0;

      /// compute the scores of the set of targets conditioned on some nodes
      /** The method comptes the scores of some individual target nodes
       * conditioned to a given set of nodes.
       *
       * The nodes are identified by their id in the database. Their scores can
       * be retrieved individually using method score.
       * @warning the method uses a reference to db_single_ids and
       * db_conditioning_ids, and it does not copy these vectors in order to
       * speed-up computations. So, be careful not to delete these vectors from
       * memory before calling the score method.*/
      virtual void
      computeScores ( const std::vector<unsigned int>& db_conditioning_ids,
                      const std::vector<unsigned int>& db_single_ids ) = 0;

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
      virtual void
      computeScores
      ( const std::vector< std::pair<unsigned int,
                                     unsigned int> >& db_pair_ids ) = 0;
      
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
      virtual void
      computeScores
      ( const std::vector<unsigned int>& db_conditioning_ids,
        const std::vector< std::pair<unsigned int,
                                     unsigned int> >& db_pair_ids  ) = 0;

      /// returns the score of a given single target Y given the conditioning nodes
      /** This method returns the score of Y conditionally to the set of
       * conditioning nodes specified in the last computeScores execution.
       * @warning This function should be called only after a
       * computeScores ( db_single_ids ) or a
       * computeScores ( db_conditioning_ids, db_single_ids ) has
       * been completed, else its result is undetermined and is most probably
       * an exception raised. */
      virtual float
      score ( unsigned int Y ) const = 0;
      
      /// returns the score of a given pair X,Y given the conditioning nodes
      /** This method returns the score of Y conditionally to X plus the set of
       * conditioning nodes specified in the last computeScores execution.
       * @warning This function should be called only after a
       * computeScores ( db_pair_ids ) or a
       * computeScores ( db_conditioning_ids, db_pair_ids ) 
       * has been completed, else its result is undetermined and is most probably
       * an exception raised. */
      virtual float
      score ( const std::pair<unsigned int,unsigned int>& XY_pair ) const = 0;

      /// @}


    protected:
      /// a pointer to the database
      const Database* _database;
      
      /// the counting tree used for the computations
      CountingTree _tree;

      

      /// prevent copy constructor
      Score ( const Score& );

      /// prevent copy operator
      Score& operator= ( const Score& );
      
    };


  } /* namespace learning */
  
  
} /* namespace gum */


#endif /* GUM_LEARNING_SCORE_H */
