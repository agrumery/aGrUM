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
#ifndef GUM_DISCRETIZED_VARIABLE_H
#define GUM_DISCRETIZED_VARIABLE_H

#include <iostream>
#include <sstream>
#include <vector>
#include <limits>

#include <agrum/config.h>

#include <agrum/variables/discreteVariable.h>

namespace gum {

#define INC_MARKS_ARRAY 4

  /**
   * @class DiscretizedVariable
   * @brief Class for discretized random variable.
   * @ingroup multidim_group
   *
   * a DiscretizedVariable contains a vector of T_TICKS, assuming that the
   * modalities of the variable arc [T1,T2[,[T2,T3[,...,[Tn-1,Tn]. T can be
   * plus_infinity or minus_infinity.
   *
   * operator [] has a T_TICKS argument.
   * @warning DiscretizedVariable is a template but it is assumed that there is
   * a complete order on T_TICKS.
   * @warning Note that, if T_TICKS allows it, you may use
   * std::numeric_limits<T_TICKS>::infinity() and
   * std::numeric_limits<T_TICKS>::infinity() as tick value.
   * @warning If a variable contains less than 3 ticks, it is considered as
   * empty(). A variable with 3 ticks is binary and contains 2 ranges :
   * [T1,T2[ (modality 0) and [T2,T3] (modality 1).
   * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
   */
  template <typename T_TICKS>

  class DiscretizedVariable : public DiscreteVariable {
    private:
    std::vector<T_TICKS> __ticks; // Array from 0 to domainSize-2
    Size __ticks_size;

    protected:
    /**
     * make a copy
     * TODO since we removed T_OtherData maybe some changes are
     * needed in this method?
     * @param aDRV  the copied object
     */
    void _copy(const DiscretizedVariable<T_TICKS> &aDRV);

    /**
     * perform a dichotomy on ticks
     * @param target T_TICKS value
     * @param min first index
     * @param max last index
     * @return either the index of target, either the index of the predecessor of
     target in ticks
    */
    Idx _dichotomy(const T_TICKS &target, Idx min, Idx max) const;

    /**
     * seach the class of target (internally use _dichotomy)
     * @param target
     * @return the class of target
     */
    Idx _pos(const T_TICKS &target) const;

    public:
    /** @name constructors & destructors
     * @{
     **/

    /**
    * Constructor
    * @param aName the name
    * @param aDesc the description
    */
    DiscretizedVariable(const std::string &aName, const std::string &aDesc);

    /**
     * Copy constructor
     * @param aDRV
     */
    DiscretizedVariable(const DiscretizedVariable<T_TICKS> &aDRV);

    /**
     * Destructor.
    */
    virtual ~DiscretizedVariable();

    /// @}

    /// a virtual clone
    virtual DiscreteVariable *clone() const;

    /// returns the type of variable

    virtual VarType varType(void) const;

    /**
     * operator =
     * @param aDRV a labelized discrete random variable
     * @return a reference to *this
     **/
    const DiscretizedVariable<T_TICKS> &
    operator=(DiscretizedVariable<T_TICKS> &aDRV);

    /**
     *
     * @param aTick
     * @return true if the tick already exists
     */
    bool isTick(const T_TICKS &aTick) const;

    /**
     * add a tick.
     * @param aTick
     * @throw DefaultInLabel
     */
    DiscretizedVariable &addTick(const T_TICKS &aTick);

    /**
     * erase all the Ticks
     */
    void eraseTicks(void);

    /**
     * @param i
     * @return the ith label
     * @throw OutOfBound
     */
    virtual const std::string label(Idx i) const;

    /// get a numerical representation of he indice-the value.
    virtual double numerical(Idx indice) const;

    /// from the label to its index in var.
    /// @throws NotFound
    virtual Idx operator[](const std::string &label) const;
    virtual Idx index(const std::string &label) const;

    /**
     *
     * @param aTarget searched label
     * @return index of this labelized
     * @throw OutOfBound, OutOfLowerBound, OutOfUpperBound
     */
    Idx operator[](const T_TICKS &aTarget) const;
    Idx index(const T_TICKS &aTarget) const;

    /**
     *
     * @return the size of the random discrete variable domain
     */
    virtual Size domainSize() const;

    /// from the index to the tick.
    /// @throws NotFound
    const T_TICKS &tick(Idx i) const;
  };

} /* namespace gum */

/// always include the implementation of the templates
#include <agrum/variables/discretizedVariable.tcc>

#endif /* GUM_DISCRETIZED_VARIABLE_H */
