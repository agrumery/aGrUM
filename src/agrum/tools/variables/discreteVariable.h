/**
 *
 *   Copyright (c) 2005-2024  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Base class for discrete random variable.
 *
 * This class is used as an interface.
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_DISCRETE_VARIABLE_H
#define GUM_DISCRETE_VARIABLE_H

#include <ostream>
#include <string>

#include <agrum/agrum.h>

#include <agrum/tools/core/set.h>
#include <agrum/tools/variables/variable.h>

namespace gum {

  /* ===========================================================================
   */
  /* ===========================================================================
   */
  /* ===                         GUM_DISCRETE_VARIABLE                       ===
   */
  /* ===========================================================================
   */
  /* ===========================================================================
   */
  /** @class DiscreteVariable
   * @brief Base class for discrete random variable.
   * @ingroup multidim_group
   *
   * This class is used as an interface. */
  /* ===========================================================================
   */

  class DiscreteVariable: public Variable {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// Default constructor

    DiscreteVariable(const std::string& aName, const std::string& aDesc);

    /// Copy constructor
    /** Copy Constructor.
     *
     * If aDRV haves any listener, it will not be copied.
     *
     * @param aDRV the variable we copy
     */
    DiscreteVariable(const DiscreteVariable& aDRV);

    /// destructor
    ~DiscreteVariable() override;

    /// Copy Factory.
    /// @return Returns a pointer on a new copy of this.
    DiscreteVariable* clone() const override = 0;

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// @return true if the domainSize() < 2;
    bool empty() const;

    /// @return the number of modalities of the random discrete
    virtual Size domainSize() const = 0;

    /// vector of labels
    std::vector< std::string > labels() const;

    /// get a numerical representation of the indice-th value.
    virtual double numerical(Idx indice) const = 0;

    virtual bool isEmpirical() const { return false; };

    /// for numerical variables, returns the closest index for the value
    virtual Idx closestIndex(double val) const = 0;
    /// for numerical variables, returns the closest label for the value
    std::string closestLabel(double val) const;

    /// returns the varType of variable
    VarType varType() const override = 0;

    // returns the variable in fast syntax
    virtual std::string toFast() const = 0;

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// Copy operator
    /** @param aRV to be copied
     * @return a ref to *this */

    DiscreteVariable& operator=(const DiscreteVariable& aRV);

    /// @}

    /// from the label to its index in var.
    ///  @warning This operation may have different complexity in different
    /// subclasses.
    /// @throws NotFound
    Idx operator[](const std::string& label) const { return index(label); };

    virtual Idx index(const std::string& label) const = 0;

    /// get the indice-th label. This method is pure virtual.
    /** @param indice the index of the label we wish to return
     * @throw OutOfBound
     */
    virtual std::string label(Idx i) const = 0;

    /// string version of *this
    std::string toString() const;

    /// string version of *this using description attribute instead of name.
    std::string toStringWithDescription() const;

    /// string represent the domain of the variable
    std::string domain() const override = 0;

    /// string represent the type of the variable
    virtual std::string stype() const = 0;

    protected:
    /// (protected) Default constructor
    DiscreteVariable() { GUM_CONSTRUCTOR(DiscreteVariable) }
  };

  /// for friendly displaying the content of the variable

  std::ostream& operator<<(std::ostream&, const DiscreteVariable&);

  using VariableSet = Set< const DiscreteVariable* >;
} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/tools/variables/discreteVariable_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_DISCRETE_VARIABLE_H */
