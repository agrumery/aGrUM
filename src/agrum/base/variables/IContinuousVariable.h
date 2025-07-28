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
 * @brief Header of IContinuousVariable.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_I_CONTINUOUS_VARIABLE_H
#define GUM_I_CONTINUOUS_VARIABLE_H

#include <iostream>
#include <string>

#include <agrum/agrum.h>

#include <agrum/base/variables/variable.h>

namespace gum {

  /** @class IContinuousVariable
   * @headerfile IContinuousVariable.h <agrum/base/variables/IContinuousVariable.h>
   * @ingroup multidim_group
   * @brief A base class for continuous variables, independent of the
   * GUM_SCALAR type
   */
  class IContinuousVariable: public Variable {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// Default constructor
    IContinuousVariable(const std::string& aName, const std::string& aDesc);

    /** Copy Constructor.
     *
     * If from has any listener, it will not be copied.
     *
     * @param from the variable we copy
     */
    IContinuousVariable(const IContinuousVariable& from);

    /// move constructor
    IContinuousVariable(IContinuousVariable&& from);

    /// destructor
    virtual ~IContinuousVariable();

    /// Copy Factory
    virtual IContinuousVariable* clone() const = 0;

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// returns the lower bound of the domain of the variable as a double
    virtual double lowerBoundAsDouble() const = 0;

    /// returns the upper bound of the domain of the variable as a double
    virtual double upperBoundAsDouble() const = 0;

    /// updates the lower bound of the domain of the variable
    /** @throw OutOfBounds is raised if the new bound is higher than the
     * current upper bound. */
    virtual void setLowerBoundFromDouble(const double new_bound) = 0;

    /// updates the lower bound of the domain of the variable
    /** @throw OutOfBounds is raised if the new bound is lower than the
     * current lower bound */
    virtual void setUpperBoundFromDouble(const double new_bound) = 0;

    /// @}


    protected:
    /// copy operator
    IContinuousVariable& operator=(const IContinuousVariable& from);

    /// move operator
    IContinuousVariable& operator=(IContinuousVariable&& from);
  };


} /* namespace gum */


#ifndef GUM_NO_INLINE
#  include <agrum/base/variables/IContinuousVariable_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_I_CONTINUOUS_VARIABLE_H */
