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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#ifndef GUM_DISCRETIZED_VARIABLE_H
#define GUM_DISCRETIZED_VARIABLE_H

#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/variables/IDiscretizedVariable.h>

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
  class DiscretizedVariable final: public IDiscretizedVariable {
    private:
    std::vector< T_TICKS > _ticks_;   // Array from 0 to domainSize-2

    protected:
    /**
     * make a copy
     * @param aDRV  the copied object
     */
    void copy_(const DiscretizedVariable< T_TICKS >& aDRV);

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
    DiscretizedVariable< T_TICKS >* clone() const override;

    /// returns the type of variable

    VarType varType() const override;

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
    std::string label(Idx i) const override;

    /// get a (deterministic) numerical representation of he indice-the value.
    double numerical(Idx indice) const override;

    /// get a random value in the intervall indice
    double draw(Idx indice) const override;

    /// from the label to its index in var.
    /// @throws NotFound
    Idx index(const std::string& label) const override;
    /// from the T+TICKS to its index in var.
    /// @throws NotFound
    Idx index(const T_TICKS tick) const;

    /**
     *
     * @return the size of the random discrete variable domain
     */
    Size domainSize() const override;

    std::string domain() const override;

    /// string represent the type of the variable
    std::string stype() const final { return "Discretized"; };

    /// from the index to the tick.
    /// @throws NotFound
    const T_TICKS& tick(Idx i) const;

    /// Return the list of ticks
    const std::vector< T_TICKS >& ticks() const;

    /// return the list of ticks as a vector of doubles
    std::vector< double > ticksAsDoubles() const override;

    ///  returns the closest index of the value
    Idx closestIndex(double val) const override;

    // returns the variable in fast syntax
    std::string toFast() const override;

    private:
    /// check the domain
    /**
     * this function use the assumption that the concrete type of the variable is the same as *this
     */
    bool _checkSameDomain_(const Variable& aRV) const override;
  };

} /* namespace gum */

/// always include the implementation of the templates
#include <agrum/base/variables/discretizedVariable_tpl.h>

#endif /* GUM_DISCRETIZED_VARIABLE_H */
