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


#ifndef GUM_LEARNING_SCORE_BIC_H
#define GUM_LEARNING_SCORE_BIC_H


#include <agrum/learning/asymmetricScore.h>


namespace gum {

  
  namespace learning {

    
    /* ========================================================================= */
    /* ========================================================================= */
    /* ===                         SCORE BIC CLASS                           === */
    /* ========================================================================= */
    /* ========================================================================= */
    /** @class ScoreBIC */
    /* ========================================================================= */
    class ScoreBIC : public AsymmetricScore {
    public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      ScoreBIC ( const Database& database,
                 unsigned int max_tree_size = 0 );

      /// destructor
      ~ScoreBIC ();

      /// @}


    protected:
      /// computes the BIC score of a set of targets
      /** @warning The function assumes that the counting tree has already been
       * constructed */
      void _computeScores ( const std::vector<unsigned int>& db_single_ids );

      /// computes the BIC score of a set of targets
      /** @warning The function assumes that the counting tree has already been
       * constructed */
      void _computeScores
      ( const std::vector< std::pair<unsigned int,
                                     unsigned int> >& db_pair_ids );
      
    };
    

  } /* namespace learning */
  
  
} /* namespace gum */


#endif /* GUM_LEARNING_SCORE_BIC_H */
