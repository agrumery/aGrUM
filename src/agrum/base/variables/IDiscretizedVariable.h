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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#ifndef GUM_I_DISCRETIZED_VARIABLE_H
#define GUM_I_DISCRETIZED_VARIABLE_H

#include <iostream>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/variables/discreteVariable.h>

namespace gum {

  /** @class IDiscretizedVariable
   * @headerfile IDiscretizedVariable.h <agrum/base/variables/IDiscretizedVariable.h>
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
    bool isEmpirical() const final;

    /**
     *
     *  @param state : change the variabl to empirical or not
     */
    void setEmpirical(bool state);

    /// return the list of ticks as a vector of doubles
    virtual std::vector< double > ticksAsDoubles() const = 0;

    /// get a (double) random value in the intervall indice
    virtual double draw(Idx indice) const = 0;
    /// @}


    protected:
    bool _is_empirical;
    /// copy operator
    IDiscretizedVariable& operator=(const IDiscretizedVariable& from);
  };


} /* namespace gum */


#ifndef GUM_NO_INLINE
#  include <agrum/base/variables/IDiscretizedVariable_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_I_DISCRETIZED_VARIABLE_H */
