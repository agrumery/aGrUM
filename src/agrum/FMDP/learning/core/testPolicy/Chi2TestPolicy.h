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
 * @brief Headers of the Chi2TestPolicy
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 *
 */
#ifndef GUM_MULTI_DIM_FUNCTION_GRAPH_CHI2_TEST_POLICY_H
#define GUM_MULTI_DIM_FUNCTION_GRAPH_CHI2_TEST_POLICY_H

// ============================================================================
#include <agrum/FMDP/learning/core/chiSquare.h>
#include <agrum/FMDP/learning/core/contingencyTable.h>
#include <agrum/FMDP/learning/core/testPolicy/ITestPolicy.h>
// ============================================================================

namespace gum {

  /**
   * @class Chi2TestPolicy Chi2TestPolicy.h
   * <agrum/tools/multidim/core/testPolicy/Chi2TestPolicy.h>
   *
   * @brief Chi2TestPolicy implements a test policy that follows the Chi2
   * statistic
   *
   * @ingroup fmdp_group
   */
  template < typename GUM_SCALAR >
  class Chi2TestPolicy: public ITestPolicy< GUM_SCALAR > {
    public:
    Chi2TestPolicy() : ITestPolicy< GUM_SCALAR >(), _conTab_(), _chi2Score_(0) {
      GUM_CONSTRUCTOR(Chi2TestPolicy);
    }

    virtual ~Chi2TestPolicy() {
      GUM_DESTRUCTOR(Chi2TestPolicy);
      ;
    }

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new(size_t s) { return SmallObjectAllocator::instance().allocate(s); }
    void  operator delete(void* p) {
      SmallObjectAllocator::instance().deallocate(p, sizeof(Chi2TestPolicy));
    }

    // ############################################################################
    /// @name Observation insertion
    // ############################################################################
    /// @{

    // ============================================================================
    /// Comptabilizes the new observation
    // ============================================================================
    void addObservation(Idx attr, GUM_SCALAR value);

    /// @}


    // ############################################################################
    /// @name Test relevance
    // ############################################################################
    /// @{

    // ============================================================================
    /// Returns true if enough observation were made so that the test can be
    /// relevant
    // ============================================================================
    bool isTestRelevant() const {
      return (this->nbObservation() > 20 && this->nbObservation() > _conTab_.attrASize() * 5);
    }

    /// @}


    // ############################################################################
    /// @name Test result
    // ############################################################################
    /// @{

    // ============================================================================
    /// Recomputes the statistic from the beginning
    // ============================================================================
    void computeScore() const;

    // ============================================================================
    /// Returns the performance of current variable according to the test
    // ============================================================================
    double score() const;

    // ============================================================================
    /// Returns a second criterion to severe ties
    // ============================================================================
    virtual double secondaryscore() const;

    /// @}

    const ContingencyTable< Idx, GUM_SCALAR >& ct() const { return _conTab_; }

    void add(const Chi2TestPolicy< GUM_SCALAR >& src);

    std::string toString() const {
      std::stringstream ss;
      ss << ITestPolicy< GUM_SCALAR >::toString() << "\t\t\tContingency Table : " << std::endl
         << _conTab_.toString() << std::endl
         << "\t\t\tGStat : " << this->score() << std::endl
         << "\t\t\tGStat : " << this->secondaryscore() << std::endl;
      return ss.str();
    }

    private:
    /// The contingency table used to keeps records of all observation
    ContingencyTable< Idx, GUM_SCALAR > _conTab_;

    mutable double _chi2Score_;
  };

}   // End of namespace gum

#include <agrum/FMDP/learning/core/testPolicy/Chi2TestPolicy_tpl.h>

#endif /* GUM_MULTI_DIM_FUNCTION_GRAPH_CHI2_TEST_POLICY_H */
