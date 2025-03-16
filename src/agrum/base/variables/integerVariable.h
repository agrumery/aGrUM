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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/





/** @file
 * @brief Base class for integer discrete random variables
 *
 * @author Christophe GONZALES(_at_AMU) & Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_INTEGER_DISCRETE_VARIABLE_H
#define GUM_INTEGER_DISCRETE_VARIABLE_H

#include <agrum/agrum.h>

#include <agrum/base/variables/discreteVariable.h>

namespace gum {

  /** class IntegerVariable
   * @brief The class representing discrete integer random variables
   * @ingroup multidim_group
   */
  /* =========================================================================*/

  class IntegerVariable final: public DiscreteVariable {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /** @brief constructor
     * @param aName the name of the variable
     * @param aDesc the Description of the variable, if any
     */
    IntegerVariable(const std::string& aName, const std::string& aDesc = "");

    /** @brief constructor assigning a domain to the variable
     * @param aName the name of the variable
     * @param aDesc the Description of the variable, if any
     * @param domain the domain (set of values) of the variable
     */
    IntegerVariable(const std::string&        aName,
                    const std::string&        aDesc,
                    const std::vector< int >& domain);

    /** @brief constructor assigning a domain to the variable
     * @param aName the name of the variable
     * @param aDesc the Description of the variable, if any
     * @param first the first value
     * @param last the last value
     * @param nb the number of values
     */
    IntegerVariable(const std::string& aName,
                    const std::string& aDesc,
                    int                first,
                    int                last,
                    Size               nb);

    /// Copy constructor
    /**
     * @param from the variable we copy
     */
    IntegerVariable(const IntegerVariable& from);

    /// move constructor
    IntegerVariable(IntegerVariable&& from) noexcept;

    /// virtual copy constructor
    IntegerVariable* clone() const final;

    /// destructor
    ~IntegerVariable() final;

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

    ///  returns the closest index of the value
    Idx closestIndex(double val) const final;

    /// returns a string corresponding to the ith value of the domain
    std::string label(Idx index) const final;

    /// get a integer representation of the value at a given index
    double numerical(Idx index) const final;

    /// Returns the domain as a string
    std::string domain() const final;

    /// string represent the type of the variable
    std::string stype() const final { return "Integer"; };

    /// returns the domain as a sequence of values
    const std::vector< int >& integerDomain() const;

    /// add a new value to the domain size
    /** @throw DuplicateElement is raised if the variable already contains the value
     */
    void addValue(int value);

    /// does this value exist in the domain ?
    bool isValue(int value) const;

    /// substitute a value by another one
    void changeValue(int old_value, int new_value);

    /// erase a value from the domain of the variable
    void eraseValue(int value);

    /// clear the domain of the variable
    void eraseValues();

    /**
     * gives the value closets to val
     *
     * @param val the desired value
     * @return the value
     */
    std::string closestLabel(double val) const;

    /// @}

    private:
    /// check the domain
    /**
     * this function use the assumption that the concrete type of the variable is the same as *this
     */
    bool _checkSameDomain_(const Variable& aRV) const final;

    /// the domain of the variable
    std::vector< int > _domain_;
  };

} /* namespace gum */


#ifndef GUM_NO_INLINE
#  include <agrum/base/variables/integerVariable_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_INTEGER_DISCRETE_VARIABLE_H */
