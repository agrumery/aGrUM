
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


/**
 * @file
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_MULTI_DIM_COMPLETE_PROJECTION_H
#define GUM_MULTI_DIM_COMPLETE_PROJECTION_H

#include <agrum/core/set.h>
#include <agrum/multidim/instantiation.h>
#include <agrum/variables/discreteVariable.h>
#include <utility>

namespace gum {

  // clang-format off
  /** @class MultiDimCompleteProjection
   * @headerfile multiDimCompleteProjection.h <agrum/multidim/operators/multiDimCompleteProjection.h>
   * @ingroup multidim_op_group
   *
   * @brief A generic class to project efficiently a MultiDim table over all
   * of its variables
   */
  // clang-format on
  template < typename GUM_SCALAR, template < typename > class TABLE >
  class MultiDimCompleteProjection {
    public:
    // ========================================================================
    /// @name Constructors / Destructors
    // ========================================================================
    /// @{

    /// Default constructor
    MultiDimCompleteProjection(GUM_SCALAR (*proj)(const TABLE< GUM_SCALAR >&,
                                                  Instantiation*));

    /// Copy constructor
    MultiDimCompleteProjection(
       const MultiDimCompleteProjection< GUM_SCALAR, TABLE >&);

    /// Destructor
    virtual ~MultiDimCompleteProjection();

    /**
     * @brief Virtual constructor
     *
     * @return a new fresh MultiDimCombinator with the same projection
     * function.
     */
    virtual MultiDimCompleteProjection< GUM_SCALAR, TABLE >* newFactory() const;

    /// @}
    // ========================================================================
    /// @name Accessors/Modifiers
    // ========================================================================
    /// @{

    /// Creates and returns the projection of the table over all of its vars
    GUM_SCALAR project(const TABLE< GUM_SCALAR >& table,
                       Instantiation*             instantiation = 0);

    /// Changes the function used for projecting TABLES
    void setProjectFunction(GUM_SCALAR (*proj)(const TABLE< GUM_SCALAR >&,
                                               Instantiation*));

    /// Returns the projection function currently used by the projector
    GUM_SCALAR(*projectFunction())
    (const TABLE< GUM_SCALAR >&, Instantiation*);

    /// @}

    protected:
    /// The projection function actually used
    GUM_SCALAR (*_proj)(const TABLE< GUM_SCALAR >&, Instantiation*);

    private:
    /// Forbid copy operators
    MultiDimCompleteProjection< GUM_SCALAR, TABLE >&
       operator=(const MultiDimCompleteProjection< GUM_SCALAR, TABLE >&);
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/multidim/utils/operators/multiDimCompleteProjection_tpl.h>

#endif /* GUM_MULTI_DIM_COMPLETE_PROJECTION_H */
