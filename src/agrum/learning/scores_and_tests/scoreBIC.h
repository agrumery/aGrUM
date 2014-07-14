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
 * The class should be used as follows: first, to speed-up computations, you
 * should consider computing all the scores you need in one pass. To do so, use
 * the appropriate addNodeSets methods. These will compute everything you need.
 * Use methods score to retrieve the scores computed. See the Score class for
 * details.
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_LEARNING_SCORE_BIC_H
#define GUM_LEARNING_SCORE_BIC_H


#include <agrum/learning/scores_and_tests/score.h>


namespace gum {

  
  namespace learning {

    
    /* ========================================================================= */
    /* ===                         SCORE BIC CLASS                           === */
    /* ========================================================================= */
    /** @class ScoreBIC
     * @ingroup learning_group
     * @brief The class for computing BIC scores
     *
     * The class should be used as follows: first, to speed-up computations, you
     * should consider computing all the scores you need in one pass. To do so, use
     * the appropriate addNodeSets methods. These will compute everything you need.
     * Use methods score to retrieve the scores computed. See the Score class for
     * details.
     */
    template <typename IdSetAlloc = std::allocator<unsigned int>,
              typename CountAlloc = std::allocator<float> >
    class ScoreBIC : public Score<IdSetAlloc,CountAlloc> {
    public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param filter the row filter that will be used to read the database
       * @param var_modalities the domain sizes of the variables in the database */
      template <typename RowFilter>
      ScoreBIC ( const RowFilter& filter,
                 const std::vector<unsigned int>& var_modalities,
                 Apriori<IdSetAlloc,CountAlloc>& apriori );

      /// copy constructor
      ScoreBIC ( const ScoreBIC<IdSetAlloc,CountAlloc>& );

      /// move constructor
      ScoreBIC ( ScoreBIC<IdSetAlloc,CountAlloc>&& );

      /// virtual copy factory
      virtual ScoreBIC<IdSetAlloc,CountAlloc>* copyFactory () const;

      /// destructor
      virtual ~ScoreBIC ();

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// returns the score corresponding to a given nodeset
      float score ( unsigned int nodeset_index );
      
      /// indicates whether the apriori is compatible (meaningful) with the score
      /** The combination of some scaores and aprioris can be meaningless. For
       * instance, adding a Dirichlet apriori to the K2 score is not very
       * meaningful since K2 corresonds to a BD score with a 1-smoothing apriori.
       * aGrUM allows you to perform such combination, but yuou can check with
       * method isAprioriCompatible () whether the result the score will give
       * you is meaningful or not. */
      virtual bool isAprioriCompatible () const final;

      /// @}

    };
    

  } /* namespace learning */
  
  
} /* namespace gum */


// always include the template implementation
#include <agrum/learning/scores_and_tests/scoreBIC.tcc>


#endif /* GUM_LEARNING_SCORE_BIC_H */
