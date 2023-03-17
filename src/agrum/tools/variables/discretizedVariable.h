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


#ifndef GUM_DISCRETIZED_VARIABLE_H
#define GUM_DISCRETIZED_VARIABLE_H

#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/tools/variables/IDiscretizedVariable.h>

namespace gum {
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
   * -std::numeric_limits<T_TICKS>::infinity() and
   * std::numeric_limits<T_TICKS>::infinity() as tick value.
   * @warning If a variable contains less than 3 ticks, it is considered as
   * empty(). A variable with 3 ticks is binary and contains 2 ranges :
   * [T1,T2[ (modality 0) and [T2,T3] (modality 1).
   * @warning For convenience, if the variable `_is_empirical`,  a value smaller
   * than the first tick or bigger than the last tick is not considered as an
   * error but is treated as a correct value.
   * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
   */
  template < typename T_TICKS >
  class DiscretizedVariable: public IDiscretizedVariable {
    private:
    std::vector< T_TICKS > _ticks_;   // Array from 0 to domainSize-2
    Size                   _ticks_size_;

    protected:
    /**
     * make a copy
     * @param aDRV  the copied object
     */
    void copy_(const DiscretizedVariable< T_TICKS >& aDRV);

    /**
     * perform a dichotomy on ticks
     * @param target T_TICKS value
     * @param min first index
     * @param max last index
     * @return either the index of target, either the index of the predecessor
     of
     target in ticks
    */
    Idx dichotomy_(const T_TICKS& target, Idx min, Idx max) const;

    /**
     * seach the class of target (internally use dichotomy_)
     * @param target
     * @return the class of target
     */
    Idx pos_(const T_TICKS& target) const;

    public:
    /** @name constructors & destructors
     * @{
     **/

    /**
     * Constructor
     * @param aName the name
     * @param aDesc the description
     */
    DiscretizedVariable(const std::string& aName, const std::string& aDesc);

    /**
     * Constructor
     * @param aName the name
     * @param aDesc the description
     * @param ticks list of ticks (limits of the intervals of the discretization)
     * @param is_empirical determine the behavior of the variable for value below or above the
     * limits. If false, it is treated as an error.
     */
    DiscretizedVariable(const std::string&            aName,
                        const std::string&            aDesc,
                        const std::vector< T_TICKS >& ticks,
                        bool                          is_empirical = false);

    /**
     * Copy constructor
     * @param aDRV
     */
    DiscretizedVariable(const DiscretizedVariable< T_TICKS >& aDRV);

    /**
     * Destructor.
     */
    ~DiscretizedVariable() final;

    /// @}

    /// a virtual clone
    DiscretizedVariable< T_TICKS >* clone() const final;

    /// returns the type of variable

    VarType varType() const final;

    /**
     * operator =
     * @param aDRV a labelized discrete random variable
     * @return a reference to *this
     **/
    DiscretizedVariable< T_TICKS >& operator=(const DiscretizedVariable< T_TICKS >& aDRV);

    /**
     *
     * @param aTick
     * @return true if the tick already exists
     */
    bool isTick(const T_TICKS& aTick) const;

    /**
     * add a tick.
     * @param aTick
     * @throw DefaultInLabel
     */
    DiscretizedVariable& addTick(const T_TICKS& aTick);

    /**
     * erase all the Ticks
     */
    void eraseTicks();

    /**
     * @param i
     * @return the ith label
     * @throw OutOfBound
     */
    std::string label(Idx i) const final;

    /// get a numerical representation of he indice-the value.
    double numerical(Idx indice) const final;

    /// from the label to its index in var.
    /// @throws NotFound
    Idx index(const std::string& label) const final;

    /**
     *
     * @return the size of the random discrete variable domain
     */
    Size domainSize() const final;

    std::string domain() const final;

    /// string represent the type of the variable
    std::string stype() const final { return "Discretized"; };

    /// from the index to the tick.
    /// @throws NotFound
    const T_TICKS& tick(Idx i) const;

    /// Return the list of ticks
    const std::vector< T_TICKS >& ticks() const;

    /// return the list of ticks as a vector of doubles
    std::vector< double > ticksAsDoubles() const final;

    // returns the variable in fast syntax
    std::string toFast() const final;
  };

} /* namespace gum */

/// always include the implementation of the templates
#include <agrum/tools/variables/discretizedVariable_tpl.h>

#endif /* GUM_DISCRETIZED_VARIABLE_H */
