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
 * @brief the base class for all the scores that are like BIC, etc, whose
 * formula is asymmetric w.r.t. the nodes it contains.
 *
 * The class provides a common interface for all the scores that treat
 * nodes in an asymmetric manner. For instance, in the log likelihood score,
 * the formula to be computed involves N_ijk * log ( N_ijk / N_ij ), where
 * N_ijk corresponds to the number of occurrences of the kth value of variable
 * i and the jth value of the parents of variable i, and N_ij is the same
 * except that we do not care the value of the parents. As such, variable
 * i receives a treatment that is different from its parents and this thus
 * leads to an asymmetric score.
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_LEARNING_ASYMMETRIC_SCORE_H
#define GUM_LEARNING_ASYMMETRIC_SCORE_H


#include <vector>
#include <agrum/config.h>
#include <agrum/learning/score.h>


namespace gum {

  
  namespace learning {

    
    /* ========================================================================= */
    /* ========================================================================= */
    /* ===                       ASYMMETRIC SCORE CLASS                      === */
    /* ========================================================================= */
    /* ========================================================================= */
    /** @class AsymmetricScore The base class for all the scores used for learning
     * whose formula is asymmetric w.r.t. the nodes it contains (BIC, BDeu, etc)
     *
     * The class provides a common interface for all the scores that treat
     * nodes in an asymmetric manner. For instance, in the log likelihood score,
     * the formula to be computed involves N_ijk * log ( N_ijk / N_ij ), where
     * N_ijk corresponds to the number of occurrences of the kth value of variable
     * i and the jth value of the parents of variable i, and N_ij is the same
     * except that we do not care the value of the parents. As such, variable
     * i receives a treatment that is different from its parents and this thus
     * leads to an asymmetric score.
     *
     * The class should be used as follows: first, to speed-up computations, you
     * should consider computing all the scores conditioned to a given set of
     * nodes in one pass. To do so, use the appropriate computeScores method. This
     * one will compute everything you need. The computeScores methods where you
     * do not specify a set of conditioning nodes assume that this set is empty.
     * If available memory is limited, use the setMaxSize method to constrain the
     * memory that will be used for these computations. Once the computations
     * have been performed, use methods score to retrieve the scores computed.
     * See the Score class for details. */
    /* ========================================================================= */
    class AsymmetricScore : public Score {
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
      AsymmetricScore ( const Database& database,
                        unsigned int max_tree_size = 0 );

      /// destructor
      virtual ~AsymmetricScore ();

      /// @}

      
    protected:
      /// a function that determines the db single ids needed for the score
      /** By definition, asymmetric scores treat nodes differently. For some
       * algorithms that compute the score of a given pair, we also need to
       * count some single nodes occurrences. This method should determine
       * these nodes. */
      virtual void _computeInducedSingleIds
      ( const std::vector< std::pair<unsigned int, unsigned int> >& db_pair_ids );
     
    };


  } /* namespace learning */
  
  
} /* namespace gum */


/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/asymmetricScore.inl>
#endif /* GUM_NO_INLINE */


#endif /* GUM_LEARNING_ASYMMETRIC_SCORE_H */
