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


#ifndef GUM_LEARNING_INDEP_TEST_CHI2_H
#define GUM_LEARNING_INDEP_TEST_CHI2_H


#include <agrum/learning/chi2.h>
#include <agrum/learning/symmetricIndependenceTest.h>


namespace gum {

  
  namespace learning {

    
    /* ========================================================================= */
    /* ========================================================================= */
    /* ===                         SCORE CHI2 CLASS                          === */
    /* ========================================================================= */
    /* ========================================================================= */
    /** @class ScoreChi2 */
    /* ========================================================================= */
    class IndepTestChi2 : public SymmetricIndependenceTest {
    public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      IndepTestChi2 ( const Database& database,
                      unsigned int max_tree_size = 0 );

      /// destructor
      ~IndepTestChi2 ();

      /// @}


    protected:
      /// computes the Chi2 of (X,Y) given conditioning set Z
      /** This method computes sum_X sum_Y sum_Z ( #XYZ - #XZ * #YZ / #Z )^2 /
       * (#XZ * #YZ / #Z ), where #XYZ and #XZ correspond to the number of
       * occurences of (X,Y,Z) and (X,Z) respectively in the database. Those
       * numbers are stored in the target set boxes of the counting tree of the
       * base class Score. #Z is directly accessible from the box using method
       * nbParentRecords of the targetSetBox.
       * @warning The function assumes that the counting tree has already been
       * constructed */
      void _computeScores ( const std::vector<unsigned int>& db_single_ids );


      /// the function that computes the chi2 score of pairs of targets
      /** @warning The function assumes that the counting tree has already been
       * constructed */
      void _computeScores
      ( const std::vector< std::pair<unsigned int,
                                     unsigned int> >& db_pair_ids );
 
    };
    

  } /* namespace learning */
  
  
} /* namespace gum */


#endif /* GUM_LEARNING_INDEP_TEST_CHI2_H */
