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
 * @brief Header of the gum::MultiDimReadOnly class.
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES <{prenom.nom}_at_lip6.fr>
 */

#ifndef GUM_MULTI_DIM_READONLY_H
#define GUM_MULTI_DIM_READONLY_H

#include <agrum/multidim/multiDimImplementation.h>

namespace gum {
  /**
   * @class MultiDimReadOnly
   * @brief Abstract base class for all multi dimensionnal read only structure.
   *
   * @ingroup multidim_group
   */
  template <typename GUM_SCALAR>
  class MultiDimReadOnly : public MultiDimImplementation<GUM_SCALAR> {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// Default constructor.

    MultiDimReadOnly();

    /// Copy constructor.

    MultiDimReadOnly(const MultiDimReadOnly<GUM_SCALAR> &from);

    /// Destructor.

    virtual ~MultiDimReadOnly();

    /// @}

    /**
     * This method creates a clone of this object, withouth its content
     * (including variable), you must use this method if you want to ensure
     * that the generated object has the same type than the object containing
     * the called newFactory()
     * For example :
     *   MultiDimArray<double> y;
     *   MultiDimContainer<double>* x = y.newFactory();
     * Then x is a MultiDimArray<double>*
     *
     * @warning you must desallocate by yourself the memory
     * @return an empty clone of this object with the same type
     */
    virtual MultiDimContainer<GUM_SCALAR> *newFactory() const = 0;

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// @throw OperationNotAllowed Raised because this is a read only table.

    virtual void set(const Instantiation &i, const GUM_SCALAR &value) const;

    /// @throw OperationNotAllowed Raised because this is a read only table.

    virtual void fill(const GUM_SCALAR &) const;

    /// @}

    protected:
    /// @throw OperationNotAllowed Raised because this is a read only table.

    virtual GUM_SCALAR &_get(const Instantiation &i) const;
  };
} /* namespace gum */

#include <agrum/multidim/multiDimReadOnly.tcc>

#endif /* GUM_MULTI_DIM_READONLY_H */
