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
#ifndef GUM_I_DISCRETIZED_VARIABLE_H
#define GUM_I_DISCRETIZED_VARIABLE_H

#include <iostream>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/variables/discreteVariable.h>

namespace gum {

  /** @class IDiscretizedVariable
   * @headerfile IDiscretizedVariable.h <agrum/variables/IDiscretizedVariable.h>
   * @ingroup multidim_group
   * @brief A base class for discretized variables, independent of the ticks type
   */
  class IDiscretizedVariable : public DiscreteVariable {
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
    virtual IDiscretizedVariable* clone() const = 0;

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// return the list of ticks as a vector of doubles
    virtual std::vector< double > ticksAsDoubles() const = 0;

    /// @}


    protected:
    /// copy operator
    IDiscretizedVariable& operator=(const IDiscretizedVariable& from);
  };


} /* namespace gum */


#ifndef GUM_NO_INLINE
#  include <agrum/variables/IDiscretizedVariable_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_I_DISCRETIZED_VARIABLE_H */