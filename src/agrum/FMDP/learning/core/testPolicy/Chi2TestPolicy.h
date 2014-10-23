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
/**
 * @file
 * @brief Headers of the Chi2TestPolicy
 *
 * @author Jean-Christophe Magnan
 *
 */
#ifndef GUM_MULTI_DIM_DECISION_GRAPH_CHI2_TEST_POLICY_H
#define GUM_MULTI_DIM_DECISION_GRAPH_CHI2_TEST_POLICY_H

// ============================================================================
#include <agrum/FMDP/learning/core/contingencyTable.h>
#include <agrum/FMDP/learning/core/chiSquare.h>
#include <agrum/FMDP/learning/core/testPolicy/ITestPolicy.h>
// ============================================================================

namespace gum {

  /**
   * @class Chi2TestPolicy Chi2TestPolicy.h <agrum/multidim/core/testPolicy/Chi2TestPolicy.h>
   *
   * @brief Chi2TestPolicy implements a test policy that follows the Chi2 statistic
   *
   * @ingroup fmdp_group
   */
  template < typename GUM_SCALAR >
  class Chi2TestPolicy : public ITestPolicy<GUM_SCALAR> {

    public:

      Chi2TestPolicy( ) : __conTab(), __nbObs(0){}

      // ############################################################################
      /// @name Observation insertion
      // ############################################################################
      /// @{

        // ============================================================================
        /// Comptabilizes the new observation
        // ============================================================================
        void addObservation( Idx attr, GUM_SCALAR value ) {
          __conTab.add( attr, value );
          __nbObs++;
        }

      /// @}


      // ############################################################################
      /// @name Test relevance
      // ############################################################################
      /// @{

        // ============================================================================
        /// Returns true if enough observation were made so that the test can be relevant
        // ============================================================================
        bool isTestRelevant(){ return ( __nbObs > 20 && __nbObs > __conTab.attrSize() * 5 ); }

      /// @}


      // ############################################################################
      /// @name Test result
      // ############################################################################
      /// @{

        // ============================================================================
        /// Returns the performance of current variable according to the test
        // ============================================================================
        double score(){
          double Chi2Stat = 0;
          for ( auto attrIter = __conTab.aBeginSafe(); attrIter != __conTab.aEndSafe(); ++attrIter ){
            double semiExpected = (double)(attrIter.val())/(double)__nbObs;
            for ( auto valIter = __conTab.vBeginSafe(); valIter != __conTab.vEndSafe(); ++valIter ) {
              double cell = (double)__conTab.joint(attrIter.val(),valIter.val());
              if( cell < 5 )
                continue;
              double expected = semiExpected*(double)(valIter.val());

              Chi2Stat += std::pow(cell - expected, 2.0)/expected;
            }
          }
          return 1 - ChiSquare::probaChi2(Chi2Stat, __conTab.attrSize()*__conTab.valueSize());
        }

      /// @}

    private :

      /// The contingency table used to keeps records of all observation
      ContingencyTable<Idx, GUM_SCALAR> __conTab;

      ///
      Idx __nbObs;
  };

} // End of namespace gum

#endif /* GUM_MULTI_DIM_DECISION_GRAPH_CHI2_TEST_POLICY_H */
