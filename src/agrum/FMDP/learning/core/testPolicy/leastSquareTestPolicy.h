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
 * @brief Headers of the LeastSquareTestPolicy
 *
 * @author Jean-Christophe Magnan
 *
 */
#ifndef GUM_MULTI_DIM_DECISION_GRAPH_LEAST_SQUARE_TEST_POLICY_H
#define GUM_MULTI_DIM_DECISION_GRAPH_LEAST_SQUARE_TEST_POLICY_H

// ============================================================================
#include <agrum/FMDP/learning/core/contingencyTable.h>
#include <agrum/FMDP/learning/core/testPolicy/ITestPolicy.h>
// ============================================================================
#include <agrum/multidim/decisionGraphUtilities/link.h>
// ============================================================================

namespace gum {

  /**
   * @class LeastSquareTestPolicy leastSquareTestPolicy.h <agrum/multidim/core/testPolicy/leastSquareTestPolicy.h>
   *
   * @brief LeastSquareTestPolicy implements a test policy that follows the Least Square statistic
   *
   * @ingroup fmdp_group
   */
  class LeastSquareTestPolicy : public ITestPolicy { //<Idx> {

    public:

      LeastSquareTestPolicy( ) : __nbObs(0), __sumO(0.0){}

      // ############################################################################
      /// @name Observation insertion
      // ############################################################################
      /// @{

//      HashTable<Idx,Idx> __nbObsTable;
//      HashTable<Idx,LinkedList<double>*> __obsTable;

        // ============================================================================
        /// Comptabilizes the new observation
        // ============================================================================
        void addObservation( Idx attr, double value ) {
          __sumO += value;
          __nbObs++;

          if(__sumAttrTable.exists(attr))
            __sumAttrTable[attr] += value;
          else
            __sumAttrTable.insert(attr, value);

          if(__nbObsTable.exists(attr))
            __nbObsTable[attr]++;
          else
            __nbObsTable.insert(attr, 1);

          if(!__obsTable.exists(attr))
            __obsTable.insert(attr, new LinkedList<double>());
          __obsTable[attr]->addLink(value);
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
          double mean = __sumO / (double)__nbObs;
          double errorO = 0.0;
          double sumErrorAttr = 0.0;
          for ( auto attrIter = __sumAttrTable.cbeginSafe(); attrIter != __sumAttrTable.cendSafe(); ++attrIter ){
              Idx key = attrIter.key();
              double meanAttr = __sumAttrTable[key] / (double)__nbObsTable[key];
              double errorAttr = 0.0;

              const Link<double>* linky = __obsTable[key]->list();
              while( linky ){
                  errorAttr += std::pow( linky->element() - meanAttr, 2 );
                  errorO += std::pow( linky->element() - mean, 2 );
                  linky = linky->nextLink();
              }

              sumErrorAttr += ( (double)__nbObsTable[key] / (double)__nbObs ) * errorAttr;
          }
          return errorO - sumErrorAttr;
        }

      /// @}

    private :

      ///
      Idx __nbObs;
      double __sumO;

      HashTable<Idx,Idx> __nbObsTable;
      HashTable<Idx, double> __sumAttrTable;
      HashTable<Idx,LinkedList<double>*> __obsTable;
  };

} // End of namespace gum

#endif /* GUM_MULTI_DIM_DECISION_GRAPH_LEAST_SQUARE_TEST_POLICY_H */
