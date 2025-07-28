/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Headers of the LeastSquareTestPolicy
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 *
 */
#ifndef GUM_MULTI_DIM_FUNCTION_GRAPH_LEAST_SQUARE_TEST_POLICY_H
#define GUM_MULTI_DIM_FUNCTION_GRAPH_LEAST_SQUARE_TEST_POLICY_H

// ============================================================================
#include <agrum/FMDP/learning/core/contingencyTable.h>
#include <agrum/FMDP/learning/core/testPolicy/ITestPolicy.h>
// ============================================================================
#include <agrum/base/multidim/utils/FunctionGraphUtilities/link.h>

// ============================================================================

namespace gum {

  /**
   * @class LeastSquareTestPolicy leastSquareTestPolicy.h
   * <agrum/base/multidim/core/testPolicy/leastSquareTestPolicy.h>
   *
   * @brief LeastSquareTestPolicy implements a test policy that follows the
   * Least Square statistic
   *
   * @ingroup fmdp_group
   */
  template < typename GUM_SCALAR >
  class LeastSquareTestPolicy: public ITestPolicy< GUM_SCALAR > {
    public:
    // ############################################################################
    /// @name Constructor/Destrcutor
    // ############################################################################
    /// @{

    // ============================================================================
    /// Constructor
    // ============================================================================
    LeastSquareTestPolicy() : ITestPolicy< GUM_SCALAR >(), _sumO_(0.0), _score_(0) {
      GUM_CONSTRUCTOR(LeastSquareTestPolicy);
    }

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new(size_t s) { return SmallObjectAllocator::instance().allocate(s); }

    void operator delete(void* p) {
      SmallObjectAllocator::instance().deallocate(p, sizeof(LeastSquareTestPolicy));
    }

    // ============================================================================
    /// Destructor
    // ============================================================================
    virtual ~LeastSquareTestPolicy();

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
    bool isTestRelevant() const { return (this->nbObservation() > 20); }

    /// @}


    // ############################################################################
    /// @name Test result
    // ############################################################################
    /// @{

    // ============================================================================
    /// Returns the performance of current variable according to the test
    // ============================================================================
    void computeScore();

    // ============================================================================
    /// Returns the performance of current variable according to the test
    // ============================================================================
    double score();

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
    /// Performs the merging of current LeastSquareTestPolicy instance with
    /// given instance
    // ============================================================================
    void add(const LeastSquareTestPolicy< GUM_SCALAR >& src);

    // ============================================================================
    /// Returns global sum (needed for the merging)
    // ============================================================================
    double sumValue() const { return _sumO_; }

    // ============================================================================
    /// Returns nbobs per modality table (needed for the merging)
    // ============================================================================
    const HashTable< Idx, Idx >& nbObsTable() const { return _nbObsTable_; }

    // ============================================================================
    /// Returns sum per modality table (needed for the merging)
    // ============================================================================
    const HashTable< Idx, double >& sumAttrTable() const { return _sumAttrTable_; }

    // ============================================================================
    /// Returns global sum (needed for the merging)
    // ============================================================================
    const HashTable< Idx, LinkedList< double >* >& obsTable() const { return _obsTable_; }

    private:
    /// Global sum
    double _sumO_;

    /// Nb Observation for each modality assumed by variable
    HashTable< Idx, Idx > _nbObsTable_;

    /// Sum for each modality assumed by variable
    HashTable< Idx, double > _sumAttrTable_;

    /// Not sure if needed
    HashTable< Idx, LinkedList< double >* > _obsTable_;

    /// Keeping computed score
    double _score_;
  };

}   // End of namespace gum

#include <agrum/FMDP/learning/core/testPolicy/leastSquareTestPolicy_tpl.h>

#endif /* GUM_MULTI_DIM_FUNCTION_GRAPH_LEAST_SQUARE_TEST_POLICY_H */
