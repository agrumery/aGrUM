/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
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
 * @brief This files contains several function objects that are not (yet) defined
 *        in the STL
 *
 * Generically, function objects are instances of a class with member function
 * operator() defined.
 * This member function allows the object to be used with the same syntax as a
 * function call.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#ifndef GUM_ARG_MAX_SET_H
#define GUM_ARG_MAX_SET_H
// =========================================================================
#include <cstdlib>
// =========================================================================
#include <agrum/base/core/sequence.h>
#include <agrum/base/core/smallobjectallocator/smallObjectAllocator.h>

// =========================================================================

namespace gum {

  /**
   * @struct ArgMaxSet argMaxSet.h <agrum/base/multidim/patterns/argMaxSet.h>
   * @brief A class containing the set of object assoicated to a maximum
   * @ingroup core
   *
   * This set contains the ids of the obj
   *
   */
  /// Class to handle efficiently argMaxSet
  template < GUM_Numeric GUM_SCALAR_VAL, GUM_Numeric GUM_SCALAR_SEQ >
  class ArgMaxSet {
    public:
    // ###########################################################################
    /// @name CNL
    // ###########################################################################
    /// @{

    // ============================================================================
    /// Constructor
    // ============================================================================
    ArgMaxSet();

    // ============================================================================
    /// Constructor
    // ============================================================================
    ArgMaxSet(const GUM_SCALAR_VAL& val, const GUM_SCALAR_SEQ& elem);

    // ============================================================================
    /// Copy Constructor
    // ============================================================================
    ArgMaxSet(const ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >& src);

    ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >&
        operator=(const ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >& src);

    // ============================================================================
    /// Destructor
    // ============================================================================
    ~ArgMaxSet();

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new(size_t s);

    void operator delete(void* p);

    /// @}

    // ###########################################################################
    /// @name Iterators
    // ###########################################################################
    /// @{

    // ============================================================================
    /// Iterator beginning
    // ============================================================================
    SequenceIteratorSafe< GUM_SCALAR_SEQ > beginSafe() const;

    // ============================================================================
    /// Iterator end
    // ============================================================================
    SequenceIteratorSafe< GUM_SCALAR_SEQ > endSafe() const;

    /// @}

    // ###########################################################################
    /// @name Operators
    // ###########################################################################
    /// @{

    // ============================================================================
    /// Ajout d'un élément
    // ============================================================================
    ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >& operator+=(const GUM_SCALAR_SEQ& elem);

    // ============================================================================
    /// Use to insert the content of another set inside this one
    // ============================================================================
    ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >&
        operator+=(const ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >& src);

    // ============================================================================
    /// Gives the ith element
    // ============================================================================
    const GUM_SCALAR_SEQ& operator[](const Idx i) const;

    // ============================================================================
    /// Compares two ArgMaxSet to check if they are equals
    // ============================================================================
    bool operator==(const ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >& compared) const;

    // ============================================================================
    /// Ordering comparisons based on val (C++20 spaceship operator)
    // ============================================================================
    auto operator<=>(const ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >& compared) const;

    /// @}

    // ============================================================================
    /// Gives the size
    // ============================================================================
    Idx size() const;

    // ============================================================================
    /// Returns the value on which comparison are made
    // ============================================================================
    const GUM_SCALAR_VAL& value() const;

    bool exists(const GUM_SCALAR_SEQ& elem) const;

    private:
    /// The very bone of the ArgMaxSet
    Sequence< GUM_SCALAR_SEQ >* _argMaxSeq_;
    GUM_SCALAR_VAL              _val_;

    public:
    friend std::ostream& operator<<(std::ostream& streamy, const ArgMaxSet& objy) {
      streamy << "Value : " << objy.value() << " - Set : " << objy._argMaxSeq_->toString();
      return streamy;
    }
  };

}   // End of namespace gum

#include <agrum/base/core/argMaxSet_tpl.h>

#endif /* GUM_ARG_MAX_SET_H */
