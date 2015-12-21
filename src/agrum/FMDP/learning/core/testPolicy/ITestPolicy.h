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
#ifndef GUM_MULTI_DIM_FUNCTION_GRAPH_INTERFACE_TEST_POLICY_H
#define GUM_MULTI_DIM_FUNCTION_GRAPH_INTERFACE_TEST_POLICY_H

// ============================================================================
#include <agrum/core/smallobjectallocator/smallObjectAllocator.h>
// ============================================================================
#include <agrum/graphs/nodeGraphPart.h>
// ============================================================================

namespace gum {

  /**
   * @class ITestPolicy ITestPolicy.h
   * <agrum/multidim/core/testPolicies/ITestPolicy.h>
   *
   * @brief Interface specifying the methods to be implemented by any TestPolicy
   *
   * @ingroup fmdp_group
   */
  template <typename GUM_SCALAR>
  class ITestPolicy {

    public:
    // ############################################################################
    /// @name Constructor/Destructor
    // ############################################################################
    /// @{

    // ============================================================================
    ///
    // ============================================================================
    ITestPolicy()
        : __isModified( false )
        , __nbObs( 0 ) {
      GUM_CONSTRUCTOR( ITestPolicy )
    }

    // ============================================================================
    ///
    // ============================================================================
    virtual ~ITestPolicy() { GUM_DESTRUCTOR( ITestPolicy ) }

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new( size_t s ) {
      return SmallObjectAllocator::instance().allocate( s );
    }
    void operator delete( void* p ) {
      SmallObjectAllocator::instance().deallocate( p, sizeof( ITestPolicy ) );
    }

    /// @}


    // ############################################################################
    /// @name Observation methods
    // ############################################################################
    /// @{

    // ============================================================================
    /// Comptabilizes the new observation
    // ============================================================================
    virtual void addObservation( Idx attr, GUM_SCALAR value ) {
      __isModified = true;
      __nbObs++;
    }

    // ============================================================================
    /// Comptabilizes the new observation
    // ============================================================================
    Idx nbObservation() const { return __nbObs; }

    /// @}


    // ############################################################################
    /// @name Test methods
    // ############################################################################
    /// @{

    // ============================================================================
    /// Returns true if enough observation were added so that the test can be
    /// relevant
    // ============================================================================
    virtual bool isTestRelevant() const = 0;

    // ============================================================================
    /// Recomputes the statistic from the beginning
    // ============================================================================
    virtual void computeScore() const { __isModified = false; }

    // ============================================================================
    /// Returns the performance of current variable according to the test
    // ============================================================================
    virtual double score() const = 0;

    // ============================================================================
    /// Returns a second criterion to severe ties
    // ============================================================================
    virtual double secondaryscore() const = 0;

    /// @}


    // ############################################################################
    /// @name Fusion Methods
    // ############################################################################
    /// @{

    // ============================================================================
    ///
    // ============================================================================
    void add( const ITestPolicy<GUM_SCALAR>& src ) {
      __isModified = true;
      __nbObs += src.nbObservation();
    }

    /// @}


    // ############################################################################
    /// @name Miscelleanous Methods
    // ############################################################################
    /// @{

    // ============================================================================
    ///
    // ============================================================================
    std::string toString() const {
      std::stringstream ss;
      ss << "\t\t\tNb Obs : " << __nbObs << std::endl;
      return ss.str();
    }

    /// @}

    protected:
    bool _isModified() const { return __isModified; }

    private:
    ///  Booleans indicating if we have to re eval test
    mutable bool __isModified;

    ///
    Idx __nbObs;
  };

}  // End of namespace gum

#endif /* GUM_MULTI_DIM_FUNCTION_GRAPH_INTERFACE_TEST_POLICY_H */
