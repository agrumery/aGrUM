
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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

#include <agrum/variables/IDiscretizedVariable.h>

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
   * std::numeric_limits<T_TICKS>::infinity() and
   * std::numeric_limits<T_TICKS>::infinity() as tick value.
   * @warning If a variable contains less than 3 ticks, it is considered as
   * empty(). A variable with 3 ticks is binary and contains 2 ranges :
   * [T1,T2[ (modality 0) and [T2,T3] (modality 1).
   * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
   */
  template < typename T_TICKS >
  class DiscretizedVariable : public IDiscretizedVariable {
    private:
    std::vector< T_TICKS > __ticks;   // Array from 0 to domainSize-2
    Size                   __ticks_size;

    protected:
    /**
     * make a copy
     * TODO since we removed T_OtherData maybe some changes are
     * needed in this method?
     * @param aDRV  the copied object
     */
    void _copy(const DiscretizedVariable< T_TICKS >& aDRV);

    /**
     * perform a dichotomy on ticks
     * @param target T_TICKS value
     * @param min first index
     * @param max last index
     * @return either the index of target, either the index of the predecessor
     of
     target in ticks
    */
    Idx _dichotomy(const T_TICKS& target, Idx min, Idx max) const;

    /**
     * seach the class of target (internally use _dichotomy)
     * @param target
     * @return the class of target
     */
    Idx _pos(const T_TICKS& target) const;

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
     */
    DiscretizedVariable(const std::string&            aName,
                        const std::string&            aDesc,
                        const std::vector< T_TICKS >& ticks);

    /**
     * Copy constructor
     * @param aDRV
     */
    DiscretizedVariable(const DiscretizedVariable< T_TICKS >& aDRV);

    /**
     * Destructor.
     */
    virtual ~DiscretizedVariable();

    /// @}

    /// a virtual clone
    virtual DiscretizedVariable< T_TICKS >* clone() const;

    /// returns the type of variable

    virtual VarType varType() const;

    /**
     * operator =
     * @param aDRV a labelized discrete random variable
     * @return a reference to *this
     **/
    DiscretizedVariable< T_TICKS >&
       operator=(const DiscretizedVariable< T_TICKS >& aDRV);

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
    virtual std::string label(Idx i) const;

    /// get a numerical representation of he indice-the value.
    virtual double numerical(Idx indice) const;

    /// from the label to its index in var.
    /// @throws NotFound
    virtual Idx index(const std::string& label) const;

    /**
     *
     * @return the size of the random discrete variable domain
     */
    virtual Size              domainSize() const;
    virtual const std::string domain() const;

    /// from the index to the tick.
    /// @throws NotFound
    const T_TICKS& tick(Idx i) const;

    /// Return the list of ticks
    const std::vector< T_TICKS >& ticks() const;

    /// return the list of ticks as a vector of doubles
    virtual std::vector< double > ticksAsDoubles() const;
  };

} /* namespace gum */

/// always include the implementation of the templates
#include <agrum/variables/discretizedVariable_tpl.h>

#endif /* GUM_DISCRETIZED_VARIABLE_H */
