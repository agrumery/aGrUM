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
 * @brief Header of ContinuousVariable.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_CONTINUOUS_VARIABLE_H
#define GUM_CONTINUOUS_VARIABLE_H

#include <iostream>
#include <limits>
#include <sstream>
#include <string>

#include <agrum/agrum.h>
#include <agrum/variables/variable.h>

namespace gum {

  /**
   * @class ContinuousVariable
   * @brief Defines a continuous random variable.
   */
  template < typename GUM_SCALAR = float >
  class ContinuousVariable : public Variable {
    public:
    // ##########################################################################
    /// @name Constructors / Destructors
    // ##########################################################################

    /// @{

    /// Default constructor
    /** It is possible to create ContinuousVariable with lower_bound > upper_bound.
     * In this case, the min and the max are swapped.
     * By default, the range of the variable is (-inf,+inf). */
    ContinuousVariable(
      const std::string& aName,
      const std::string& aDesc,
      GUM_SCALAR lower_bound = -std::numeric_limits< GUM_SCALAR >::infinity(),
      GUM_SCALAR upper_bound = std::numeric_limits< GUM_SCALAR >::infinity());

    /// Copy Constructor.
    ContinuousVariable(const ContinuousVariable< GUM_SCALAR >& from);

    /// generalized copy constructor
    template < typename TX_VAL >
    ContinuousVariable(const ContinuousVariable< TX_VAL >& from);

    /// move constructor
    ContinuousVariable(ContinuousVariable< GUM_SCALAR >&& from);

    /// destructor
    virtual ~ContinuousVariable();

    /// Copy Factory.
    /// @return Returns a pointer on a new copy of this.
    virtual ContinuousVariable< GUM_SCALAR >* clone() const;

    /// @}


    // ##########################################################################
    /// @name Operators
    // ##########################################################################

    /// @{

    /// copy operator
    ContinuousVariable< GUM_SCALAR >&
    operator=(const ContinuousVariable< GUM_SCALAR >& from);

    /// generalized copy operator
    template < typename TX_VAL >
    ContinuousVariable< GUM_SCALAR >&
    operator=(const ContinuousVariable< TX_VAL >& from);

    /// move operator
    ContinuousVariable< GUM_SCALAR >&
    operator=(ContinuousVariable< GUM_SCALAR >&& from);

    /// returns the T_VAL corresponding to a string
    /** @throw OutOfBounds is raised if the value does not belong to the
     * domain of the variable
     * @throw TypeError if the string cannot be converted into a T_VAL */
    GUM_SCALAR operator[](const std::string& str) const;

    /// @}


    // ##########################################################################
    /// @name Accessors / Modifiers
    // ##########################################################################

    /// @{

    /// returns the lower bound of the domain of the variable
    GUM_SCALAR lowerBound() const;

    /// returns the upper bound of the domain of the variable
    GUM_SCALAR upperBound() const;

    /// updates the lower bound of the domain of the variable
    /** @throw OutOfBounds is raised if the new bound is higher than the
     * current upper bound. */
    void setLowerBound(const GUM_SCALAR& new_bound);

    /// updates the lower bound of the domain of the variable
    /** @throw OutOfBounds is raised if the new bound is lower than the
     * current lower bound */
    void setUpperBound(const GUM_SCALAR& new_bound);

    /// returns a string containing the value of the variable passed in argument
    /**
     * @param value the value of the variable we wish to return
     * @throw OutOfBounds is raised if the value does not belong to the
     * domain of the variable
     */
    virtual std::string label(const GUM_SCALAR& value) const;

    /// Returns true if the param belongs to the domain of the variable
    bool belongs(const GUM_SCALAR& value) const;

    /// returns the domain of the variable as a string
    std::string domain() const;

    /// returns the type of the variable
    virtual VarType varType() const;

    /// string version of *this
    const std::string toString() const;

    /// string version of *this using description attribute instead of name.
    const std::string toStringWithDescription() const;

    /// @}


    private:
    // the lower bound.
    GUM_SCALAR __lower_bound;

    // the upper bound.
    GUM_SCALAR __upper_bound;

    template < typename TX_VAL >
    friend class ContinuousVariable;
  };


  /// for friendly displaying the content of the variable
  template < typename T_VAL >
  std::ostream& operator<<(std::ostream&, const ContinuousVariable< T_VAL >&);


  // specialized operator[] for real numbers
  template <>
  float ContinuousVariable< float >::operator[](const std::string& str) const;

  template <>
  double ContinuousVariable< double >::operator[](const std::string& str) const;


} /* namespace gum */

// always include the template implementation
#include <agrum/variables/continuousVariable_tpl.h>

#ifndef GUM_NO_INLINE
#include <agrum/variables/continuousVariable_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_CONTINUOUS_VARIABLE_H */
