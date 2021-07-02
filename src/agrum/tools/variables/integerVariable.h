/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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


/** @file
 * @brief Base class for integer discrete random variables
 *
 * @author Christophe GONZALES(@AMU) & Pierre-Henri WUILLEMIN(@LIP6)
 */
#ifndef GUM_INTEGER_DISCRETE_VARIABLE_H
#define GUM_INTEGER_DISCRETE_VARIABLE_H

#include <iostream>
#include <sstream>
#include <string>

#include <agrum/agrum.h>

#include <agrum/tools/core/sequence.h>
#include <agrum/tools/variables/discreteVariable.h>

namespace gum {
  
  /** class IntegerVariable
   * @brief The class representing discrete integer random variables
   * @ingroup multidim_group
   */
  /* =========================================================================*/

  class IntegerVariable: public DiscreteVariable {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /** @brief constructor
     * @param aName the name of the variable
     * @param aDesc the Description of the variable, if any
     */
    IntegerVariable(const std::string& aName,
                    const std::string& aDesc = "");

    /** @brief constructor assigning a domain to the variable
     * @param aName the name of the variable
     * @param aDesc the Description of the variable, if any
     * @param domain the domain (set of values) of the variable
     */
    IntegerVariable(const std::string&          aName,
                    const std::string&        aDesc,
                    const std::vector< int >& domain);

    /// Copy constructor
    /**
     * @param from the variable we copy
     */
    IntegerVariable(const IntegerVariable& from);

    /// move constructor
    IntegerVariable(IntegerVariable&& from);

    /// virtual copy constructor
    virtual IntegerVariable* clone() const;

    /// destructor
    virtual ~IntegerVariable();

    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    /** @param from the integer discrete random variable we copy */
    IntegerVariable& operator=(const IntegerVariable& from);

    /// move operator
    /** @param from the integer discrete random variable we copy */
    IntegerVariable& operator=(IntegerVariable&& from);

    /// equality operator
    virtual bool operator==(const Variable& var) const;

    /// inequality operator
    virtual bool operator!=(const Variable& var) const;

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################

    /// @{
    /// returns the domain size of the discrete random variable
    virtual Size domainSize() const;

    /// returns the type of variable
    virtual VarType varType() const;

    /// returns the index of a given label
    /** @param label searched label
     * @return the index of this label
     * @throw NotFound */
    virtual Idx index(const std::string& label) const;

    /// returns a string corresponding to the ith value of the domain
    virtual std::string label(Idx index) const;

    /// get a integer representation of the value at a given index
    virtual double numerical(Idx index) const;

    /// Returns the domain as a string
    virtual const std::string domain() const;

    /// returns the domain as a sequence of values
    const Sequence< int >& integerDomain() const;

    /// add a new value to the domain size
    /** @throw DuplicateElement is raised if the variable already contains the value
     * @return *this which allows : v.addValue(1).addValue(2)...;
     */
    IntegerVariable& addValue(int value);

    /// substitute a value by another one
    void changeValue(int old_value, int new_value);

    /// erase a value from the domain of the variable
    void eraseValue(int value);

    /// clear the domain of the variable
    void eraseValues();

    /// @}

    private:
    /// the domain of the variable
    Sequence< int > _domain_;
  };

} /* namespace gum */


#ifndef GUM_NO_INLINE
#  include <agrum/tools/variables/integerVariable_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_INTEGER_DISCRETE_VARIABLE_H */
