/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Headers of the ITestPolicy
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(@AMU)
 *
 */
#ifndef GUM_MULTI_DIM_FUNCTION_GRAPH_INTERFACE_TEST_POLICY_H
#define GUM_MULTI_DIM_FUNCTION_GRAPH_INTERFACE_TEST_POLICY_H

// ============================================================================
#include <agrum/tools/core/smallobjectallocator/smallObjectAllocator.h>
// ============================================================================
#include <agrum/tools/graphs/parts/nodeGraphPart.h>
// ============================================================================

namespace gum {

  /**
   * @class ITestPolicy ITestPolicy.h
   * <agrum/tools/multidim/core/testPolicies/ITestPolicy.h>
   *
   * @brief Interface specifying the methods to be implemented by any TestPolicy
   *
   * @ingroup fmdp_group
   */
  template < typename GUM_SCALAR >
  class ITestPolicy {
    public:
    // ############################################################################
    /// @name Constructor/Destructor
    // ############################################################################
    /// @{

    // ============================================================================
    ///
    // ============================================================================
    ITestPolicy() : isModified__(false), nbObs__(0) {
      GUM_CONSTRUCTOR(ITestPolicy);
    }

    // ============================================================================
    ///
    // ============================================================================
    virtual ~ITestPolicy() {
      GUM_DESTRUCTOR(ITestPolicy);
      ;
    }

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new(size_t s) {
      return SmallObjectAllocator::instance().allocate(s);
    }
    void operator delete(void* p) {
      SmallObjectAllocator::instance().deallocate(p, sizeof(ITestPolicy));
    }

    /// @}


    // ############################################################################
    /// @name Observation methods
    // ############################################################################
    /// @{

    // ============================================================================
    /// Comptabilizes the new observation
    // ============================================================================
    virtual void addObservation(Idx attr, GUM_SCALAR value) {
      isModified__ = true;
      nbObs__++;
    }

    // ============================================================================
    /// Comptabilizes the new observation
    // ============================================================================
    Idx nbObservation() const { return nbObs__; }

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
    virtual void computeScore() const { isModified__ = false; }

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
    void add(const ITestPolicy< GUM_SCALAR >& src) {
      isModified__ = true;
      nbObs__ += src.nbObservation();
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
      ss << "\t\t\tNb Obs : " << nbObs__ << std::endl;
      return ss.str();
    }

    /// @}

    protected:
    bool isModified_() const { return isModified__; }

    private:
    ///  Booleans indicating if we have to re eval test
    mutable bool isModified__;

    ///
    Idx nbObs__;
  };

}   // End of namespace gum

#endif /* GUM_MULTI_DIM_FUNCTION_GRAPH_INTERFACE_TEST_POLICY_H */
