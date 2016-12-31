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
 * @brief Headers of MultiDimPartialInstantiation.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_MULTI_DIM_PARTIAL_INSTANTIATION_H
#define GUM_MULTI_DIM_PARTIAL_INSTANTIATION_H

#include <agrum/core/set.h>
#include <agrum/multidim/multiDimDecorator.h>
#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/multidim/partialInstantiationRegister4MultiDim.h>
#include <agrum/variables/discreteVariable.h>
#include <utility>

namespace gum {

  /**
   * @class MultiDimPartialInstantiation
   * @ingroup multidim_group
   *
   * @warning Doxygen does not like spanning command on multiple line, so we
   * could not configure it with the correct include directive. Use the
   * following code snippet to include this file.
   * @code
   * #include <agrum/multidim/multiDimPartialInstantiation.h>
   * @endcode
   *
   * @brief A generic class to instantiate a subset of variables of a
   * multidimensional table.
   *
   * @tparam GUM_SCALAR The type of scalars stored in this multidimensional
   * table.
   * @tparam Table The class of multidimensional table to use.
   */
  template <typename GUM_SCALAR, template <typename> class TABLE>
  class MultiDimPartialInstantiation {
    public:
    // =========================================================================
    /// @name Constructors, Destructors and Copy
    // =========================================================================
    /// @{

    /**
     * @brief Default constructor.
     */
    MultiDimPartialInstantiation();

    /**
     * @brief Copy constructor.
     * @param src The MultiDimPartialInstantiation to copy.
     */
    MultiDimPartialInstantiation(
        const MultiDimPartialInstantiation<GUM_SCALAR, TABLE>& src );

    /**
     * @brief Class destructor.
     */
    virtual ~MultiDimPartialInstantiation();

    /**
     * @brief Copy operator.
     * @param src The MultiDimPartialInstantiation to copy.
     * @return Returns this MultiDimPartialInstantiation.
     */
    MultiDimPartialInstantiation<GUM_SCALAR, TABLE>&
    operator=( const MultiDimPartialInstantiation<GUM_SCALAR, TABLE>& src );

    /// @}
    // =========================================================================
    /// @name inherited methdods
    // =========================================================================
    /// @{

    virtual MultiDimPartialInstantiation<GUM_SCALAR, TABLE>* newFactory() const;

    /// @}
    // =========================================================================
    /// @name Accessors/Modifiers
    // =========================================================================
    /// @{

    /**
     * @brief Creates and returns the partial instantiation of the table over a
     * subset of its variables.
     *
     * @warning If inst_vars is precisely equal to the variables of table, the
     * result is an empty table.
     *
     * @param table The table to parially instantiate.
     * @param inst_vars A mapping between variables and their instantiation.
     *
     * @return A new freshly created TABLE which is the result of the
     * instantiation of a subset of the variables of the TABLE passed in
     * argument.
     */
    TABLE<GUM_SCALAR>*
    instantiate( const TABLE<GUM_SCALAR>& table,
                 const HashTable<const DiscreteVariable*, Idx>& inst_vars );

    /**
     * @brief Creates and returns the partial instantiation of the table over a
     * subset of its variables.
     *
     * @warning If inst_vars is precisely equal to the variables of table, the
     * result is an empty table.
     *
     * @param container Placeholder of the instantiation of table given
     * inst_vars.
     * @param table The table to parially instantiate.
     * @param inst_vars A mapping between variables and their instantiation.
     */
    void instantiate( TABLE<GUM_SCALAR>&       container,
                      const TABLE<GUM_SCALAR>& table,
                      const HashTable<const DiscreteVariable*, Idx>& inst_vars );

    /// @}
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/multidim/multiDimPartialInstantiation_tpl.h>

#endif /* GUM_MULTI_DIM_PARTIAL_INSTANTIATION_H */
