
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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


/** @file
 * @brief Base class for random variable.
 *
 * Basically wrapper for a string name and description.
 * This class is used as an interface. So the constructor/destructor is
 *protected.
 * @author Pierre-Henri WUILLEMIN
 */
#ifndef GUM_VARIABLE_H
#define GUM_VARIABLE_H

#include <iostream>
#include <string>

#include <agrum/agrum.h>

namespace gum {

  enum class VarType : char { Discretized, Labelized, Range, Continuous };

  class Variable;

  /// for friendly displaying the content of the variable

  std::ostream& operator<<(std::ostream& s, const Variable& LDRV);

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

    virtual bool operator==(const Variable& aRV) const;

    /// inequality operator

    virtual bool operator!=(const Variable& aRV) const;

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
    /// @warning since __description is mutable, setDescription() is const
    /** @param theValue */

    void setDescription(const std::string& theValue) const;

    /// returns the description of the variable

    const std::string& description() const;

    /// returns the type of variable

    virtual VarType varType() const = 0;

    /// string represent the domain of the variable
    virtual const std::string domain() const = 0;

    /// @}

    protected:
    /// (protected) Default constructor
    Variable() { GUM_CONSTRUCTOR(Variable); };

    /// protected copy
    /** @param aRV to be copied */

    void _copy(const Variable& aRV);

    /// constructor
    /** @param aName name of the variable
     * @param aDesc description of the variable */

    Variable(const std::string& aName, const std::string& aDesc);

    /// copy constructor
    /** @param aRV the variable we copy */

    Variable(const Variable& aRV);

    private:
    /// the name of the variable
    std::string __name;

    /// the description of the variable
    /// since description is not a characteristic of a variable, we allow the
    /// description to be changed even in a const reference.
    mutable std::string __description;
  };

} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/variables/variable_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_VARIABLE_H */
