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
 * @brief the class for computing Chi2 scores
 *
 * The class is only composed of an inline static method score that actually
 * computes the Chi2 score. To do so, it parses the number of occurrences
 * stored into a CountingTargetSetBox (see file countingTreeBoxes.h for
 * more details about such a box).
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */


#ifndef GUM_LEARNING_SCORE_CHI2_H
#define GUM_LEARNING_SCORE_CHI2_H


#include <agrum/learning/countingTreeBoxes.h>


namespace gum {

  
  namespace learning {

    
    /* ========================================================================= */
    /* ========================================================================= */
    /* ===                         SCORE CHI2 CLASS                          === */
    /* ========================================================================= */
    /* ========================================================================= */
    /** @class ScoreChi2 */
    /* ========================================================================= */
    class ScoreChi2 {
    public:
      /// computes the Chi2 of (X,Y) given conditioning set Z (stored in the box)
      /** This method computes sum_X sum_Y sum_Z ( #XYZ - #XZ * #YZ / #Z )^2 /
       * (#XZ * #YZ / #Z ), where #XYZ and #XZ correspond to the number of
       * occurences of (X,Y,Z) and (X,Z) respectively in the database. Those
       * numbers are stored in the target set box passed in argument and the
       * parameters x, y, xy indicate the indices in the target set box of the
       * elements corresponding to #XZ, #YZ and #XYZ respectively. #Z is
       * directly accessible from the box using method nbParentRecords.
       * @param box the target set box that contains all the countings required,
       * i.e., those of x, y, xy and z
       * @param x the index in 'box' of the targetBox containing the #XZ numbers
       * @param y the index in 'box' of the targetBox containing the #YZ numbers
       * @param xy the index in 'box' of the targetBox containing the #XYZ numbers
       */
      static float score ( const CountingTreeTargetSetBox& box,
                           unsigned int x,
                           unsigned int y,
                           unsigned int xy );
    };
    

  } /* namespace learning */
  
  
} /* namespace gum */


/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/scoreChi2.inl>
#endif /* GUM_NO_INLINE */


#endif /* GUM_LEARNING_SCORE_CHI2_H */
