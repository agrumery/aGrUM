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
  class ITestPolicy {

    public:
      // ############################################################################
      /// @name Terminal Node Creation and Destruction
      // ############################################################################
      /// @{

        // ============================================================================
        /// Insert a new terminal node with given value
        // ============================================================================
        virtual void addTest( const NodeId& n, const GUM_SCALAR& v) = 0;

        // ============================================================================
        /// Remove node matching given id
        // ============================================================================
        virtual void eraseTest( const NodeId& n ) = 0;

        // ============================================================================
        /// Erase all terminal nodes
        // ============================================================================
        virtual void clearAllTests() = 0;

      /// @}


      // ############################################################################
      /// @name Terminal Nodes Existence
      // ############################################################################
      /// @{

        // ============================================================================
        /// Returns true if a terminal node matching this id exists
        // ============================================================================
        virtual bool existsTest( const NodeId& n ) const = 0;

        // ============================================================================
        /// Returns true if a terminal node matching this value exists
        // ============================================================================
        virtual bool existsTest( const GUM_SCALAR& v ) const = 0;

      /// @}


      // ############################################################################
      /// @name Terminal Nodes value and id access
      // ############################################################################
      /// @{

        // ============================================================================
        /// Returns the value of the terminal node that has the given id
        // ============================================================================
        virtual const GUM_SCALAR& testValue( const NodeId& n ) const  = 0;

        // ============================================================================
        /// Returns the id of the terminal node that has the given value
        // ============================================================================
        virtual const NodeId& testId( const GUM_SCALAR& v ) const = 0;

      /// @}


      // ############################################################################
      /// @name Iterator on Terminal Nodes
      // ############################################################################
      /// @{

        // ============================================================================
        /// Initializes the constant safe iterator on terminal nodes
        // ============================================================================
        virtual void beginValues() const = 0;
        // ============================================================================
        /// Indicates if constant safe iterator has reach end of terminal nodes list
        // ============================================================================
        virtual bool hasValue() const = 0;

        // ============================================================================
        /// Increments the constant safe iterator
        // ============================================================================
        virtual void nextValue() const = 0;
        // ============================================================================
        /// Returns the value of the current terminal nodes pointed by the constant safe iterator
        // ============================================================================
        virtual const GUM_SCALAR& value() const = 0;

        // ============================================================================
        /// Returns the id of the current terminal nodes pointed by the constant safe iterator
        // ============================================================================
        virtual const NodeId& id() const = 0;

    /// @}
  };

} // End of namespace gum

#endif /* GUM_MULTI_DIM_DECISION_GRAPH_INTERFACE_TEST_POLICY_H */
