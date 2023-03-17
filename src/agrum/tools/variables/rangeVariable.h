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


/**
 * @file
 * @brief Header of gumRangeVariable.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#ifndef GUM_RANGE_VARIABLE_H
#define GUM_RANGE_VARIABLE_H

#include <ostream>
#include <sstream>
#include <string>

#include <agrum/agrum.h>

#include <agrum/tools/variables/discreteVariable.h>

namespace gum {

  /**
   * @class RangeVariable
   * @brief Defines a discrete random variable over an integer interval.
   *
   * It is technically possible to create RangeVariable with minVal > maxVal
   * (or modify in that way an already created RangeVariable). The result is
   * an empty variable (i.e. empty() returns true).
   * If maxVal - minVal < 0, then domainsize() = 0.
   */

  class RangeVariable: public DiscreteVariable {
    public:
    /// @name Constructors / Destructors

    /// @{
    ///  constructors
    RangeVariable(const std::string& aName, const std::string& aDesc, long minVal, long maxVal);

    /// by de default min=0, max=1
    RangeVariable(const std::string& aName, const std::string& aDesc);

    /** Copy Constructor.
     *
     * If aDRV haves any listener, it will not be copied.
     *
     * @param aDRV the variable we copy
     */
    RangeVariable(const RangeVariable& aDRV);

    /// destructor
    ~RangeVariable() final;

    /// Copy Factory.
    /// @return Returns a pointer on a new copy of this.
    RangeVariable* clone() const final;

    /// @}

    /// @name Accessors / Modifiers

    /// @{
    /// returns the size of the random discrete variable domain
    /// @warning :  It is technically possible to create RangeVariable with minVal
    /// > maxVal
    ///   (or modify in that way an already created RangeVariable). The result is
    ///   an empty variable (i.e. empty() returns true).
    ///   If maxVal - minVal < 0, then domainsize() = 0.
    Size domainSize() const final;

    /// returns the type of variable

    VarType varType() const final;

    // returns the variable in fast syntax
    std::string toFast() const final;
    ///
    /**
     * Get the index-th label.
     * @param index the index of the label we wish to return
     * @throw OutOfBound
     */
    std::string label(Idx index) const final;

    /// get a numerical representation of the index-the value.
    double numerical(Idx index) const final;

    /**
     * Returns the lower bound.
     */
    long minVal() const;

    /**
     * Set a new value for the lower bound.
     */
    void setMinVal(long minVal);

    /**
     * Returns the upper bound.
     */
    long maxVal() const;

    /**
     * Set a new value of the upper bound.
     */
    void setMaxVal(long maxVal);

    /**
     * Returns true if the param belongs to the variable's interval.
     */
    bool belongs(long val) const;

    /**
     * @return the modality index from the label
     * @throw NotFound
     */
    Idx index(const std::string&) const final;

    /// @}

    /// @name Operators
    /// @{
    /**
     * Copy operator
     * @param aRV to be copied
     * @return a ref to *this */
    RangeVariable& operator=(const RangeVariable& aRV);

    /// @}

    std::string domain() const final;

    /// string represent the type of the variable
    std::string stype() const final { return "Range"; };

    private:
    /// @name Private Members.

    /// @{
    /// The lower bound.
    long _minBound_;

    /// The upper bound.
    long _maxBound_;

    /// @}
  };

} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/tools/variables/rangeVariable_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_RANGE_VARIABLE_H */
