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


/**
 * @file
 * @brief Header of the MultiDimReadOnly class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#ifndef GUM_MULTI_DIM_READONLY_H
#define GUM_MULTI_DIM_READONLY_H

#include <agrum/tools/multidim/implementations/multiDimImplementation.h>

namespace gum {

  /**
   * @class MultiDimReadOnly
   * @headerfile multiDimReadOnly.h <agrum/tools/multidim/multiDimReadOnly.h>
   * @ingroup multidim_group
   *
   * @brief Abstract base class for all multi dimensionnal read only structure.
   * @tparam GUM_SCALAR The type of scalars stored in the multidimensional
   * table.
   */
  template < typename GUM_SCALAR >
  class MultiDimReadOnly: public MultiDimImplementation< GUM_SCALAR > {
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
    MultiDimReadOnly(const MultiDimReadOnly< GUM_SCALAR >& from);

    /**
     * @brief Class destructor.
     */
    virtual ~MultiDimReadOnly();

    virtual MultiDimContainer< GUM_SCALAR >* newFactory() const = 0;

    /// @}
    // =========================================================================
    /// @name Accessors / Modifiers
    // =========================================================================
    /// @{

    /**
     * @warning An OperationNotAllowed will be raised because this is a read
     * only table.
     */
    virtual void set(const Instantiation& i, const GUM_SCALAR& value) const;

    /**
     * @warning An OperationNotAllowed will be raised because this is a read
     * only table.
     */
    virtual void fill(const GUM_SCALAR&) const;

    /// @}

    protected:
    /**
     * @warning An OperationNotAllowed will be raised because this is a read
     * only table.
     */
    virtual GUM_SCALAR& get_(const Instantiation& i) const;
  };
} /* namespace gum */

#include <agrum/tools/multidim/implementations/multiDimReadOnly_tpl.h>

#endif /* GUM_MULTI_DIM_READONLY_H */
