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
 * @brief Headers of the Observation class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */

// #define  TRACE_ALGO
// =========================================================================
#ifndef GUM_OBSERVATION_H
#define GUM_OBSERVATION_H
// =========================================================================
#include <agrum/base/core/smallobjectallocator/smallObjectAllocator.h>
// =========================================================================
#include <agrum/base/variables/discreteVariable.h>

// =========================================================================

namespace gum {

  /**
   * @class Observation
   * @headerfile observation.h <agrum/FMDP/learning/observation.h>
   * @brief
   * @ingroup fmdp_group
   *
   *
   *
   */

  class Observation {
    public:
    // ==========================================================================
    /// @name Constructor & destructor.
    // ==========================================================================
    /// @{

    // ###################################################################
    /// Default constructor
    // ###################################################################
    Observation() {
      GUM_CONSTRUCTOR(Observation);
      ;
    }

    // ###################################################################
    /// Default destructor
    // ###################################################################
    ~Observation() {
      GUM_DESTRUCTOR(Observation);
      ;
    }

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new(size_t s) { return SmallObjectAllocator::instance().allocate(s); }

    void operator delete(void* p) {
      SmallObjectAllocator::instance().deallocate(p, sizeof(Observation));
    }

    /// @}

    // ==========================================================================
    /// @name Observation Handlers
    // ==========================================================================
    /// @{

    // ###################################################################
    /**
     * Returns the modality assumed by the given variable in this observation
     *
     * @throws NotFound if variable is not in this observation
     */
    // ###################################################################
    INLINE Idx modality(const DiscreteVariable* var) const { return _varInst_[var]; }

    INLINE Idx rModality(const DiscreteVariable* var) const { return _rInst_[var]; }

    // ###################################################################
    /**
     * Sets the modality assumed by the given variable in this observation
     *
     * @throws DuplicateElement if a value has already be assigned to
     * this variable
     */
    // ###################################################################
    INLINE void setModality(const DiscreteVariable* var, Idx modality) {
      _varInst_.insert(var, modality);
    }

    INLINE void setRModality(const DiscreteVariable* var, Idx modality) {
      _rInst_.insert(var, modality);
    }

    // ###################################################################
    // Returns the reward obtained during this observation
    // ###################################################################
    double reward() const { return _reward_; }

    // ###################################################################
    // Sets the reward obtained during this observation
    // ###################################################################
    void setReward(double reward) { _reward_ = reward; }

    /// @}
    ///
    std::string toString() const;

    // ==========================================================================
    /// @name Iterators on Variables
    // ==========================================================================
    /// @{

    // ###################################################################
    /// Returns an const safe iterator on the beginning of the list of
    /// variables in this observation
    // ###################################################################
    HashTableConstIteratorSafe< const DiscreteVariable*, Idx > cbeginVariablesSafe() const {
      return _varInst_.cbeginSafe();
    }

    // ###################################################################
    /// Returns an const safe iterator on the end of the list of
    /// variables in this observation
    // ###################################################################
    HashTableConstIteratorSafe< const DiscreteVariable*, Idx > cendVariablesSafe() const {
      return _varInst_.cendSafe();
    }

    /// @}

    private:
    /// Table giving for every variables its instantiation
    HashTable< const DiscreteVariable*, Idx > _varInst_;
    HashTable< const DiscreteVariable*, Idx > _rInst_;

    /// The reward associated to this transition
    double _reward_;
  };

} /* namespace gum */


#endif   // GUM_OBSERVATION_H
