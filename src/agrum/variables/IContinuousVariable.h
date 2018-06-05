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
 * @brief Header of IContinuousVariable.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_I_CONTINUOUS_VARIABLE_H
#define GUM_I_CONTINUOUS_VARIABLE_H

#include <iostream>
#include <string>

#include <agrum/agrum.h>
#include <agrum/variables/variable.h>

namespace gum {

  /** @class IContinuousVariable
   * @headerfile IContinuousVariable.h <agrum/variables/IContinuousVariable.h>
   * @ingroup multidim_group
   * @brief A base class for continuous variables, independent of the
   * GUM_SCALAR type
   */
  class IContinuousVariable : public Variable {
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
#  include <agrum/variables/IContinuousVariable_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_I_CONTINUOUS_VARIABLE_H */
