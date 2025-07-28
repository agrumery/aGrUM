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


/** @file
 * @brief Base class for labelized discrete random variables
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_LABELIZED_DISCRETE_VARIABLE_H
#define GUM_LABELIZED_DISCRETE_VARIABLE_H

#include <iostream>
#include <sstream>
#include <string>

#include <agrum/agrum.h>

#include <agrum/base/core/sequence.h>
#include <agrum/base/variables/discreteVariable.h>

namespace gum {
  /* =========================================================================*/
  /* =========================================================================*/
  /* ===                        GUM_LABELIZED_VARIABLE === */
  /* =========================================================================*/
  /* =========================================================================*/
  /** class LabelizedVariable
   * @brief Base class for labelized discrete random variables
   * @ingroup multidim_group
   */
  /* =========================================================================*/

  class LabelizedVariable final: public DiscreteVariable {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /** @brief constructor
     * @param aName the name
     * @param aDesc the Description
     * @param nbrLabel the domainSize (2 by default)
     */

    LabelizedVariable(const std::string& aName,
                      const std::string& aDesc    = "",
                      const Size         nbrLabel = 2);

    /** @brief constructor
     * @param aName the name
     * @param aDesc the Description
     * @param labels the labels
     */

    LabelizedVariable(const std::string&                aName,
                      const std::string&                aDesc,
                      const std::vector< std::string >& labels);

    /// Copy constructor
    /**
     * @param aLDRV the variable we copy
     */

    LabelizedVariable(const LabelizedVariable& aLDRV);

    /// destructor

    ~LabelizedVariable() final;

    /// a virtual clone ...
    LabelizedVariable* clone() const final;

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    /** @param aLDRV the labelized discrete random variable we copy */
    LabelizedVariable& operator=(const LabelizedVariable& aLDRV);


    /// returns the index of a given label
    /** @param label searched label
     * @return the index of this label
     * @throw NotFound */
    Idx index(const std::string& label) const final;

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /**
     * @brief indicates whether the variable already has the label passed in
     * argument
     * @param aLabel The label
     * @return true if the label already exists */
    bool isLabel(const std::string& aLabel) const;

    /// add a label with a new index (we assume that we will NEVER remove a
    /// label)
    /** @param aLabel the label to be added to the labelized variable
     * @throw DuplicateElement is raised if the variable already contains
     * the label
     * @return *this which allows : v.addLabel("1").addLabel("2")...;
     */
    LabelizedVariable& addLabel(const std::string& aLabel);

    /// change a label for this index
    /** @param pos the index of the label to be changed
     * @param aLabel the label to be added to the labelized variable
     * @throw DuplicateElement is raised if the variable already contains
     * @throw OutOfBounds
     * @warning Since this change does not affect the variable domain but only
     * the
     * variable representation,
     * this method is allowed on const LabelizedVariable.
     */
    void changeLabel(Idx pos, const std::string& aLabel) const;

    /// erase all the labels
    void eraseLabels();

    /// returns the ith label
    /** @param i
     * @return the ith label */
    std::string label(Idx i) const final;

    /// return the pos from label
    Idx posLabel(const std::string& label) const;

    /// get a numerical representation of the index-the value.
    double numerical(Idx index) const final;

    ///  returns the closest index of the value
    Idx closestIndex(double val) const final;

    /// returns the size of the random discrete variable domain
    Size domainSize() const final;

    /// returns the type of variable
    VarType varType() const final;

    /// Returns the domain
    std::string domain() const final;

    /// string represent the type of the variable
    std::string stype() const final { return "Labelized"; };

    // returns the variable in fast syntax
    std::string toFast() const final;

    /// @}


    private:
    /// check the domain
    /**
     * this function use the assumption that the concrete type of the variable is the same as *this
     */
    bool _checkSameDomain_(const Variable& aRV) const final;

    protected:
    /// (protected) Default constructor
    LabelizedVariable() { GUM_CONSTRUCTOR(LabelizedVariable) }

    /// copies the content of aLDRV

    void copy_(const LabelizedVariable& aLDRV);

    private:
    /// the set of labels contained in the variable
    mutable Sequence< std::string > _labels_;
  };

} /* namespace gum */

/* ==============================================================================
 */
/* ==============================================================================
 */
/* ===                    LabelizedVariable IMPLEMENTATION === */
/* ==============================================================================
 */
/* ==============================================================================
 */
#ifndef GUM_NO_INLINE
#  include <agrum/base/variables/labelizedVariable_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LABELIZED_DISCRETE_VARIABLE_H */
