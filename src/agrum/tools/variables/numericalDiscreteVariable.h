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


/** @file
 * @brief Base class for numerical discrete random variables
 *
 * @author Christophe GONZALES(_at_AMU) & Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_NUMERICAL_DISCRETE_VARIABLE_H
#define GUM_NUMERICAL_DISCRETE_VARIABLE_H

#include <iostream>
#include <sstream>
#include <string>

#include <agrum/agrum.h>

#include <agrum/tools/core/sequence.h>
#include <agrum/tools/variables/discreteVariable.h>

namespace gum {

  /** class NumericalDiscreteVariable
   * @brief The class representing discrete numerical random variables
   * @ingroup multidim_group
   */
  /* =========================================================================*/

  class NumericalDiscreteVariable: public DiscreteVariable {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /** @brief constructor
     * @param aName the name of the variable
     * @param aDesc the Description of the variable, if any
     */
    NumericalDiscreteVariable(const std::string& aName, const std::string& aDesc = "");

    /** @brief constructor assigning a domain to the variable
     * @param aName the name of the variable
     * @param aDesc the Description of the variable, if any
     * @param domain the domain (set of values) of the variable
     */
    NumericalDiscreteVariable(const std::string&           aName,
                              const std::string&           aDesc,
                              const std::vector< double >& domain);

    /** @brief constructor assigning a domain to the variable
     * @param aName the name of the variable
     * @param aDesc the Description of the variable, if any
     * @param first the first value
     * @param last the last value
     * @param nb the number of values
     */
    NumericalDiscreteVariable(const std::string& aName,
                              const std::string& aDesc,
                              double             first,
                              double             last,
                              Size               nb);

    /// Copy constructor
    /**
     * @param from the variable we copy
     */
    NumericalDiscreteVariable(const NumericalDiscreteVariable& from);

    /// move constructor
    NumericalDiscreteVariable(NumericalDiscreteVariable&& from);

    /// virtual copy constructor
    NumericalDiscreteVariable* clone() const final;

    /// destructor
    virtual ~NumericalDiscreteVariable();

    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    /** @param from the numerical discrete random variable we copy */
    NumericalDiscreteVariable& operator=(const NumericalDiscreteVariable& from);

    /// move operator
    /** @param from the numerical discrete random variable we copy */
    NumericalDiscreteVariable& operator=(NumericalDiscreteVariable&& from);

    /// equality operator
    bool operator==(const NumericalDiscreteVariable& var) const;
    bool operator==(const Variable& var) const final;

    /// inequality operator
    bool operator!=(const NumericalDiscreteVariable& var) const;
    bool operator!=(const Variable& var) const final;

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################

    /// @{
    /// returns the domain size of the discrete random variable
    Size domainSize() const final;

    /// returns the type of variable
    VarType varType() const final;

    // returns the variable in fast syntax
    std::string toFast() const final;

    /// returns the index of a given label
    /** @param label searched label
     * @return the index of this label
     * @throw NotFound */
    Idx index(const std::string& label) const final;

    /// returns a string corresponding to the ith value of the domain
    std::string label(Idx index) const final;

    /// get a numerical representation of the value at a given index
    double numerical(Idx index) const final;

    /// Returns the domain as a string
    std::string domain() const final;

    /// string represent the type of the variable
    std::string stype() const final { return "NumericalDiscrete"; };

    /// returns the domain as a sequence of values
    const Sequence< double >& numericalDomain() const;

    /// add a new value to the domain size
    /** @throw DuplicateElement is raised if the variable already contains the value
     */
    void addValue(double value);

    /// does this value exist in the domain ?
    bool isValue(double value) const;

    /// substitute a value by another one
    void changeValue(double old_value, double new_value);

    /// erase a value from the domain of the variable
    void eraseValue(double value);

    /// clear the domain of the variable
    void eraseValues();

    /**
     * gives the index of the value closest to val
     *
     * @param val the desired double
     * @return the index
     */
    Idx closestIndex(double val) const;

    /**
     * gives the value closets to val
     *
     * @param val the desired value
     * @return the value
     */
    std::string closestLabel(double val) const;
    /// @}

    private:
    /// the domain of the variable
    Sequence< double > _domain_;

    Idx         dichotomy_(double target, Idx min, Idx max) const;
    std::string _generateLabel(double f) const;
  };

} /* namespace gum */


#ifndef GUM_NO_INLINE
#  include <agrum/tools/variables/numericalDiscreteVariable_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_NUMERICAL_DISCRETE_VARIABLE_H */
