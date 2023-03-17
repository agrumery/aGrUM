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


#ifndef GUM_I_DISCRETIZED_VARIABLE_H
#define GUM_I_DISCRETIZED_VARIABLE_H

#include <iostream>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/tools/variables/discreteVariable.h>

namespace gum {

  /** @class IDiscretizedVariable
   * @headerfile IDiscretizedVariable.h <agrum/tools/variables/IDiscretizedVariable.h>
   * @ingroup multidim_group
   * @brief A base class for discretized variables, independent of the ticks type
   */
  class IDiscretizedVariable: public DiscreteVariable {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// Default constructor
    IDiscretizedVariable(const std::string& aName, const std::string& aDesc);

    /** Copy Constructor.
     *
     * If from has any listener, it will not be copied.
     *
     * @param from the variable we copy
     */
    IDiscretizedVariable(const IDiscretizedVariable& from);

    /// destructor
    virtual ~IDiscretizedVariable();

    /// Copy Factory
    IDiscretizedVariable* clone() const = 0;

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /**
     *
     * @return true if the tick is considered as empirical (relaxeds bounds)
     */
    bool isEmpirical() const;

    /**
     *
     *  @param state : change the variabl to empirical or not
     */
    void setEmpirical(bool state);

    /// return the list of ticks as a vector of doubles
    virtual std::vector< double > ticksAsDoubles() const = 0;
    /// @}


    protected:
    bool _is_empirical;
    /// copy operator
    IDiscretizedVariable& operator=(const IDiscretizedVariable& from);
  };


} /* namespace gum */


#ifndef GUM_NO_INLINE
#  include <agrum/tools/variables/IDiscretizedVariable_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_I_DISCRETIZED_VARIABLE_H */
