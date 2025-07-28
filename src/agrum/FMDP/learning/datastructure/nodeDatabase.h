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
 * @brief Headers of the NodeDatabase class.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#ifndef GUM_NODE_DATABASE_H
#define GUM_NODE_DATABASE_H
// =========================================================================
#include <agrum/base/core/sequence.h>
// =========================================================================
#include <agrum/FMDP/learning/core/templateStrategy.h>
#include <agrum/FMDP/learning/core/testPolicy/Chi2TestPolicy.h>
#include <agrum/FMDP/learning/core/testPolicy/GTestPolicy.h>
#include <agrum/FMDP/learning/core/testPolicy/leastSquareTestPolicy.h>

// =========================================================================
// =========================================================================

namespace gum {

  /**
   * @class NodeDatabase nodeDatabase.h
   * <agrum/FMDP/learning/datastructure/nodeDatabase.h>
   * @brief
   * @ingroup fmdp_group
   *
   */


  template < TESTNAME AttributeSelection, bool isScalar >
  class NodeDatabase {
    using ValueType = typename ValueSelect< isScalar, double, Idx >::type;

    template < typename GUM_SCALAR >
    using TestPolicy = typename TestSelect< AttributeSelection,
                                            GTestPolicy< GUM_SCALAR >,
                                            Chi2TestPolicy< GUM_SCALAR >,
                                            LeastSquareTestPolicy< GUM_SCALAR > >::type;

    public:
    // ==========================================================================
    /// @name Constructor & destructor.
    // ==========================================================================
    /// @{

    // ###################################################################
    /// Default constructor
    // ###################################################################
    NodeDatabase(const gum::VariableSet*, const DiscreteVariable* = nullptr);

    // ###################################################################
    /// Default destructor
    // ###################################################################
    ~NodeDatabase();

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new(size_t s) { return SmallObjectAllocator::instance().allocate(s); }

    void operator delete(void* p) {
      SmallObjectAllocator::instance().deallocate(p, sizeof(NodeDatabase));
    }

    /// @}

    // ==========================================================================
    /// @name Observation handling methods
    // ==========================================================================
    /// @{

    // ###################################################################
    /** Updates database with new observation
     *
     * Calls either @fn  _addObservation_( const Observation*, Int2Type<true>)
     * or @fn  _addObservation_( const Observation*, Int2Type<false>)
     * depending on if we're learning reward function or transition probability
     **/
    // ###################################################################
    void addObservation(const Observation*);

    private:
    void _addObservation_(const Observation*, Int2Type< true >);
    void _addObservation_(const Observation*, Int2Type< false >);

    public:
    // ###################################################################
    /// Nb observation taken into account by this instance
    // ###################################################################
    INLINE Idx nbObservation() const { return _nbObservation_; }

    /// @}

    // ==========================================================================
    /// @name Variable Test Methods
    // ==========================================================================
    /// @{

    // ###################################################################
    /// Indicates wether or not, node has sufficient observation so that
    /// any statistic is relevant
    // ###################################################################
    INLINE bool isTestRelevant(const DiscreteVariable* var) const {
      return _attrTable_[var]->isTestRelevant();
    }

    // ###################################################################
    /// Returns the performance of given variables according to selection
    /// criterion
    // ###################################################################
    INLINE double testValue(const DiscreteVariable* var) const { return _attrTable_[var]->score(); }

    // ###################################################################
    /// Returns the performance of given variables according to selection
    /// secondary criterion (to break ties)
    // ###################################################################
    INLINE double testOtherCriterion(const DiscreteVariable* var) const {
      return _attrTable_[var]->secondaryscore();
    }

    /// @}

    // ==========================================================================
    /// @name Aggregation Methods
    // ==========================================================================
    /// @{

    // ###################################################################
    /// Merges given NodeDatabase informations into current nDB.
    // ###################################################################
    NodeDatabase< AttributeSelection, isScalar >&
        operator+=(const NodeDatabase< AttributeSelection, isScalar >& src);

    // ###################################################################
    /// Returns a reference to nDB test policy for given variable
    /// (so that test policy information can be merged too)
    // ###################################################################
    const TestPolicy< ValueType >* testPolicy(const DiscreteVariable* var) const {
      return _attrTable_[var];
    }

    // ###################################################################
    /// Iterators on value count to recopy correctly its content
    // ###################################################################
    const HashTableConstIteratorSafe< ValueType, Idx > cbeginValues() const {
      return _valueCount_.cbeginSafe();
    }

    const HashTableConstIteratorSafe< ValueType, Idx > cendValues() const {
      return _valueCount_.cendSafe();
    }

    /// @}

    // ###################################################################
    ///
    // ###################################################################

    Idx effectif(Idx moda) const {
      return _valueCount_.exists(ValueType(moda)) ? _valueCount_[ValueType(moda)] : 0;
    }

    Idx valueDomain() const { return _valueDomain_(Int2Type< isScalar >()); }

    private:
    Idx _valueDomain_(Int2Type< true >) const { return _valueCount_.size(); }

    Idx _valueDomain_(Int2Type< false >) const { return _value_->domainSize(); }

    std::string toString() const;

    private:
    /// Table giving for every variables its instantiation
    HashTable< const DiscreteVariable*, TestPolicy< ValueType >* > _attrTable_;

    /// So does this reference on the value observed
    const DiscreteVariable* _value_;

    ///
    Idx _nbObservation_;

    ///
    HashTable< ValueType, Idx > _valueCount_;
  };


} /* namespace gum */

#include <agrum/FMDP/learning/datastructure/nodeDatabase_tpl.h>

#endif   // GUM_NODE_DATABASE_H
