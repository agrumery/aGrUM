/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Headers of the GTestPolicy
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 *
 */
#ifndef GUM_MULTI_DIM_FUNCTION_GRAPH_G_TEST_POLICY_H
#define GUM_MULTI_DIM_FUNCTION_GRAPH_G_TEST_POLICY_H
// ============================================================================
#include <agrum/FMDP/learning/core/chiSquare.h>
#include <agrum/FMDP/learning/core/contingencyTable.h>
#include <agrum/FMDP/learning/core/testPolicy/ITestPolicy.h>
#include <agrum/FMDP/learning/core/tupleHashFunc.h>
// ============================================================================

namespace gum {

  //  template <typename GUM_SCALAR>
  //  using ConTab = ContingencyTable<Idx, GUM_SCALAR>;

  /**
   * @class GTestPolicy GTestPolicy.h
   * <agrum/tools/multidim/core/testPolicies/GTestPolicy.h>
   *
   * @brief G implements a test policy that follows the G statistic
   *
   * @ingroup fmdp_group
   */
  template < typename GUM_SCALAR >
  class GTestPolicy: public ITestPolicy< GUM_SCALAR > {
    public:
    // ############################################################################
    /// @name Constructor/Destrcutor
    // ############################################################################
    /// @{

    // ============================================================================
    /// Constructor
    // ============================================================================
    GTestPolicy() : ITestPolicy< GUM_SCALAR >(), _conTab_(), _GStat_(0) {
      GUM_CONSTRUCTOR(GTestPolicy);
    }

    // ============================================================================
    /// Destructor
    // ============================================================================
    virtual ~GTestPolicy() {
      GUM_DESTRUCTOR(GTestPolicy);
      ;
    }

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new(size_t s) { return SmallObjectAllocator::instance().allocate(s); }

    void operator delete(void* p) {
      SmallObjectAllocator::instance().deallocate(p, sizeof(GTestPolicy));
    }

    /// @}

    // ############################################################################
    /// @name Observation insertion
    // ############################################################################
    /// @{

    // ============================================================================
    /// Comptabilizes the new observation
    // ============================================================================
    void addObservation(Idx iattr, GUM_SCALAR ivalue);

    /// @}


    // ############################################################################
    /// @name Test methods
    // ############################################################################
    /// @{

    // ============================================================================
    /// Returns true if enough observation were made so that the test can be
    /// relevant
    // ============================================================================
    bool isTestRelevant() const {
      return (this->nbObservation() > 20 && this->nbObservation() > _conTab_.attrASize() * 5);
    }

    // ============================================================================
    /// Computes the GStat of current variable according to the test
    // ============================================================================
    void computeScore() const;

    // ============================================================================
    /// Returns the performance of current variable according to the test
    // ============================================================================
    double score() const;

    // ============================================================================
    /// Returns a second criterion to severe ties
    // ============================================================================
    double secondaryscore() const;


    /// @}


    // ############################################################################
    /// @name Fusion Methods
    // ############################################################################
    /// @{

    // ============================================================================
    /// Performs the merging of current GTestPolicy instance with given instance
    // ============================================================================
    void add(const GTestPolicy< GUM_SCALAR >& src);

    // ============================================================================
    /// Returns contingency table (needed for the merging of GTestPolicy
    /// instances)
    // ============================================================================
    const ContingencyTable< Idx, GUM_SCALAR >& ct() const { return _conTab_; }


    /// @}


    // ############################################################################
    /// @name Miscelleanous Methods
    // ############################################################################
    /// @{

    std::string toString() const {
      std::stringstream ss;
      ss << ITestPolicy< GUM_SCALAR >::toString() << "\t\t\tContingency Table : " << std::endl
         << _conTab_.toString() << std::endl
         << "\t\t\tGStat : " << _GStat_ << std::endl
         << "\t\t\tGStat : " << this->secondaryscore() << std::endl;
      return ss.str();
    }

    /// @}

    private:
    /// The contingency table used to keeps records of all observation
    ContingencyTable< Idx, GUM_SCALAR > _conTab_;
    mutable double                      _GStat_;
  };

}   // End of namespace gum

#include <agrum/FMDP/learning/core/testPolicy/GTestPolicy_tpl.h>

#endif /* GUM_MULTI_DIM_FUNCTION_GRAPH_G_TEST_POLICY_H */
