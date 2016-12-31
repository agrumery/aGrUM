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
 * @brief Header of the MultiDimReadOnly class.
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 */

#ifndef GUM_MULTI_DIM_READONLY_H
#define GUM_MULTI_DIM_READONLY_H

#include <agrum/multidim/multiDimImplementation.h>

namespace gum {

  /**
   * @class MultiDimReadOnly
   * @headerfile multiDimReadOnly.h <agrum/multidim/multiDimReadOnly.h>
   * @ingroup multidim_group
   *
   * @brief Abstract base class for all multi dimensionnal read only structure.
   * @tparam GUM_SCALAR The type of scalars stored in the multidimensional
   * table.
   */
  template <typename GUM_SCALAR>
  class MultiDimReadOnly : public MultiDimImplementation<GUM_SCALAR> {
    public:
    // =========================================================================
    /// @name Constructors / Destructors
    // =========================================================================
    /// @{

    /**
     * @brief Default constructor.
     */
    MultiDimReadOnly();

    /**
     * @brief Copy constructor.
     * @param from The MultiDimReadOnly to copy.
     */
    MultiDimReadOnly( const MultiDimReadOnly<GUM_SCALAR>& from );

    /**
     * @brief Class destructor.
     */
    virtual ~MultiDimReadOnly();

    virtual MultiDimContainer<GUM_SCALAR>* newFactory() const = 0;

    /// @}
    // =========================================================================
    /// @name Accessors / Modifiers
    // =========================================================================
    /// @{

    /**
     * @warning An OperationNotAllowed will be raised because this is a read
     * only table.
     */
    virtual void set( const Instantiation& i, const GUM_SCALAR& value ) const;

    /**
     * @warning An OperationNotAllowed will be raised because this is a read
     * only table.
     */
    virtual void fill( const GUM_SCALAR& ) const;

    /// @}

    protected:
    /**
     * @warning An OperationNotAllowed will be raised because this is a read
     * only table.
     */
    virtual GUM_SCALAR& _get( const Instantiation& i ) const;
  };
} /* namespace gum */

#include <agrum/multidim/multiDimReadOnly_tpl.h>

#endif /* GUM_MULTI_DIM_READONLY_H */
