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
/** @file
 * @brief A generic class to instantiate a subset of variables of a MultiDim table
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_MULTI_DIM_PARTIAL_INSTANTIATION_H
#define GUM_MULTI_DIM_PARTIAL_INSTANTIATION_H

#include <utility>
#include <agrum/core/set.h>
#include <agrum/variables/discreteVariable.h>
#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/multidim/multiDimDecorator.h>
#include <agrum/multidim/partialInstantiationRegister4MultiDim.h>

namespace gum {

  template <typename GUM_SCALAR, template <typename> class TABLE>
  class MultiDimPartialInstantiation {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    MultiDimPartialInstantiation();

    /// copy constructor
    MultiDimPartialInstantiation(
        const MultiDimPartialInstantiation<GUM_SCALAR, TABLE> &);

    /// destructor
    virtual ~MultiDimPartialInstantiation();

    /// virtual constructor
    /** @return a new fresh MultiDimPartialInstantiation for the same kind of
     * table and data*/
    virtual MultiDimPartialInstantiation<GUM_SCALAR, TABLE> *newFactory() const;

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    MultiDimPartialInstantiation<GUM_SCALAR, TABLE> &
    operator=(const MultiDimPartialInstantiation<GUM_SCALAR, TABLE> &);

    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /** @brief creates and returns the partial instantiation of the table over a
     * subset of its vars
     *
     * @return a new freshly created TABLE which is the result of the instantiation
     * of a subset of the variables of the TABLE passed in argument
     * @warning If inst_vars is precisely equal to the variables of table, the
     * result is an empty table. */
    TABLE<GUM_SCALAR> *
    instantiate(const TABLE<GUM_SCALAR> &table,
                const HashTable<const DiscreteVariable *, Idx> &inst_vars);
    void instantiate(TABLE<GUM_SCALAR> &container, const TABLE<GUM_SCALAR> &table,
                     const HashTable<const DiscreteVariable *, Idx> &inst_vars);

    /// @}

    private:
    /// the function that is used to actually perform the instantiation
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/multidim/multiDimPartialInstantiation.tcc>

#endif /* GUM_MULTI_DIM_PARTIAL_INSTANTIATION_H */
