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
 * @brief the class for computing AIC scores
 *
 * The class should be used as follows: first, to speed-up computations, you
 * should consider computing all the scores you need in one pass. To do so, use
 * the appropriate addNodeSets methods. These will compute everything you need.
 * Use methods score to retrieve the scores computed. See the Score class for
 * details.
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */


#ifndef GUM_LEARNING_SCORE_AIC_H
#define GUM_LEARNING_SCORE_AIC_H


#include <agrum/learning/scores_and_tests/score.h>


namespace gum {

  
  namespace learning {

    
    /* ========================================================================= */
    /* ===                         SCORE AIC CLASS                           === */
    /* ========================================================================= */
    /** @class ScoreAIC
     * @ingroup learning_group
     * @brief the class for computing AIC scores
     *
     * The class should be used as follows: first, to speed-up computations, you
     * should consider computing all the scores you need in one pass. To do so, use
     * the appropriate addNodeSets methods. These will compute everything you need.
     * Use methods score to retrieve the scores computed. See the Score class for
     * details.
     */
    template <typename IdSetAlloc = std::allocator<unsigned int>,
              typename CountAlloc = std::allocator<float> >
    class ScoreAIC : public Score<IdSetAlloc,CountAlloc> {
    public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param filter the row filter that will be used to read the database
       * @param var_modalities the domain sizes of the variables in the database */
      template <typename RowFilter>
      ScoreAIC ( const RowFilter& filter,
                 const std::vector<unsigned int>& var_modalities );

      /// destructor
      virtual ~ScoreAIC ();

      /// @}

      
      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// returns the score corresponding to a given nodeset
      float score ( unsigned int nodeset_index );

      /// @}

    };
    

  } /* namespace learning */
  
  
} /* namespace gum */


// always include the template implementation
#include <agrum/learning/scores_and_tests/scoreAIC.tcc>


#endif /* GUM_LEARNING_SCORE_AIC_H */
