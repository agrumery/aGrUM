/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 * @file
 * @brief Header of gumRangeVariable.
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 */

#ifndef GUM_RANGE_VARIABLE_H
#define GUM_RANGE_VARIABLE_H

#include <string>
#include <sstream>
#include <ostream>

#include <agrum/config.h>

#include <agrum/core/hashFunc.h>
#include <agrum/variables/discreteVariable.h>

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

  class RangeVariable : public DiscreteVariable {

    public:
    /// @name Constructors / Destructors

    /// @{
    /// Default constructor
    RangeVariable(const std::string &aName, const std::string &aDesc, Idx minVal = 0,
                  Idx maxVal = 1);

    /** Copy Constructor.
     *
     * If aDRV haves any listener, it will not be copied.
     *
     * @param aDRV the variable we copy
     */
    RangeVariable(const RangeVariable &aDRV);

    /// destructor
    virtual ~RangeVariable();

    /// Copy Factory.
    /// @return Returns a pointer on a new copy of this.
    virtual DiscreteVariable *clone() const;

    /// @}

    /// @name Accessors / Modifiers

    /// @{
    /// returns the size of the random discrete variable domain
    virtual Size domainSize() const;

    /// returns the type of variable

    virtual VarType varType(void) const;

    ///
    /**
     * Get the indice-th label.
     * @param indice the index of the label we wish to return
     * @throw OutOfBound
     */
    virtual const std::string label(Idx indice) const;

    /// get a numerical representation of he indice-the value.
    virtual double numerical(Idx indice) const;

    /**
     * Returns the lower bound.
     */
    Idx minVal() const;

    /**
     * Set a new value for the lower bound.
     */
    void setMinVal(Idx minVal);

    /**
     * Returns the upper bound.
     */
    Idx maxVal() const;

    /**
     * Set a new value of the upper bound.
     */
    void setMaxVal(Idx maxVal);

    /**
     * Returns true if the param belongs to the variable's interval.
     */
    bool belongs(Idx indice) const;

    /**
     * @return the modality index from the label
     * @throw NotFound
     */
    virtual Idx operator[](const std::string &) const;
    virtual Idx index(const std::string &) const;

    /// @}

    /// @name Operators
    /// @{
    /**
     * Copy operator
     * @param aRV to be copied
     * @return a ref to *this */
    RangeVariable &operator=(const RangeVariable &aRV);

    /// @}

    virtual const std::string toString() const;

    private:
    /// @name Private Members.

    /// @{
    /// The lower bound.
    Idx __minBound;

    /// The upper bound.
    Idx __maxBound;

    /// @}
  };

} /* namespace gum */

#ifndef GUM_NO_INLINE
#include <agrum/variables/rangeVariable.inl>
#endif /* GUM_NO_INLINE */

#endif /* GUM_RANGE_VARIABLE_H */
