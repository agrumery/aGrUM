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
 * @brief Headers of the ITestPolicy
 *
 * @author Jean-Christophe Magnan
 *
 */
#ifndef GUM_MULTI_DIM_DECISION_GRAPH_INTERFACE_TEST_POLICY_H
#define GUM_MULTI_DIM_DECISION_GRAPH_INTERFACE_TEST_POLICY_H

// ============================================================================
#include <agrum/graphs/nodeGraphPart.h>
// ============================================================================

namespace gum {

  /**
   * @class ITestPolicy ITestPolicy.h <agrum/multidim/core/testPolicies/ITestPolicy.h>
   *
   * @brief Interface specifying the methods to be implemented by any TestPolicy
   *
   * @ingroup fmdp_group
   */
//  template < typename GUM_SCALAR >
  class ITestPolicy {

    public:
      // ############################################################################
      /// @name Observation insertion
      // ############################################################################
      /// @{

        // ============================================================================
        /// Comptabilizes the new observation
        // ============================================================================
        virtual void addObservation( Idx iattr, double ivalue ) = 0;

      /// @}


      // ############################################################################
      /// @name Test relevance
      // ############################################################################
      /// @{

        // ============================================================================
        /// Returns true if enough observation were added so that the test can be relevant
        // ============================================================================
        virtual bool isTestRelevant() = 0;

      /// @}


      // ############################################################################
      /// @name Test result
      // ############################################################################
      /// @{

        // ============================================================================
        /// Returns the performance of current variable according to the test
        // ============================================================================
        virtual double score() = 0;

      /// @}
  };

} // End of namespace gum

#endif /* GUM_MULTI_DIM_DECISION_GRAPH_INTERFACE_TEST_POLICY_H */
