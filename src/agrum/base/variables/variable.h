/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
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
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief Base class for random variable.
 *
 * Basically wrapper for a string name and description.
 * This class is used as an interface. So the constructor/destructor is
 *protected.
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_VARIABLE_H
#define GUM_VARIABLE_H

#include <iostream>
#include <string>

#include <agrum/agrum.h>

namespace gum {

  enum class VarType : char { DISCRETIZED, LABELIZED, INTEGER, NUMERICAL, RANGE, CONTINUOUS };

  /* ===========================================================================
   */
  /* ===========================================================================
   */
  /* ===                            GUM_VARIABLE                             ===
   */
  /* ===========================================================================
   */
  /* ===========================================================================
   */
  /** @class Variable
   * @brief Base class for every random variable.
   * @ingroup multidim_group
   */
  /* ===========================================================================
   */

  class Variable {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// destructor

    virtual ~Variable();

    /// Copy Factory.
    /// @return Returns a pointer on a new copy of this.
    virtual Variable* clone() const = 0;

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// Copy operator
    /** @param aRV to be copied
     * @return a const ref to *this */

    Variable& operator=(const Variable& aRV);

    /// equality operator
    bool operator==(const Variable& aRV) const;


    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// sets the name of the variable
    /** @param theValue */
    void setName(const std::string& theValue);

    /// returns the name of the variable
    const std::string& name() const;

    /// sets the description of the variable
    /// @warning since  _description_ is mutable, setDescription() is const
    /** @param theValue */
    void setDescription(const std::string& theValue) const;

    /// returns the description of the variable
    const std::string& description() const;

    /// returns the type of variable
    virtual VarType varType() const = 0;

    /// string represent the domain of the variable
    virtual std::string domain() const = 0;

    /// @}

    protected:
    /// (protected) Default constructor
    Variable() { GUM_CONSTRUCTOR(Variable); }

    /// protected copy
    /** @param aRV to be copied */
    void copy_(const Variable& aRV);

    /// constructor
    /** @param aName name of the variable
     * @param aDesc description of the variable */
    Variable(const std::string& aName, const std::string& aDesc);

    /// copy constructor
    /** @param aRV the variable we copy */
    Variable(const Variable& aRV);

    private:
    /// the name of the variable
    std::string _name_;

    /// the description of the variable
    /// since description is not a characteristic of a variable, we allow the
    /// description to be changed even in a const reference.
    mutable std::string _description_;

    /// check the domain
    /**
     * this function use the assumption that the concrete type of the variable is the same as *this
     */
    virtual bool _checkSameDomain_(const Variable& aRV) const = 0;
  };

} /* namespace gum */

/// for friendly displaying the content of the variable
std::ostream& operator<<(std::ostream& s, const gum::Variable& LDRV);

#ifndef GUM_NO_INLINE
#  include <agrum/base/variables/variable_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_VARIABLE_H */
