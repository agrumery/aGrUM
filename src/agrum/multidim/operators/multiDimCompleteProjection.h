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
 * @brief A generic class to project efficiently a MultiDim table over all
 * of its variables
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_MULTI_DIM_COMPLETE_PROJECTION_H
#define GUM_MULTI_DIM_COMPLETE_PROJECTION_H

#include <utility>
#include <agrum/core/set.h>
#include <agrum/variables/discreteVariable.h>
#include <agrum/multidim/instantiation.h>

namespace gum {

  template <typename GUM_SCALAR, template <typename> class TABLE>
  class MultiDimCompleteProjection {

    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    MultiDimCompleteProjection(GUM_SCALAR (*proj)(const TABLE<GUM_SCALAR> &,
                                                  Instantiation *));

    /// copy constructor
    MultiDimCompleteProjection(
        const MultiDimCompleteProjection<GUM_SCALAR, TABLE> &);

    /// destructor
    virtual ~MultiDimCompleteProjection();

    /// virtual constructor
    /** @return a new fresh MultiDimCombinator with the same projection
     * function. */
    virtual MultiDimCompleteProjection<GUM_SCALAR, TABLE> *newFactory() const;

    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// creates and returns the projection of the table over all of its vars
    GUM_SCALAR project(const TABLE<GUM_SCALAR> &table,
                       Instantiation *instantiation = 0);

    /// changes the function used for projecting TABLES
    void setProjectFunction(GUM_SCALAR (*proj)(const TABLE<GUM_SCALAR> &,
                                               Instantiation *));

    /// returns the projection function currently used by the projector
    GUM_SCALAR(*projectFunction())
    (const TABLE<GUM_SCALAR> &, Instantiation *);

    /// @}

    protected:
    /// the projection function actually used
    GUM_SCALAR (*_proj)(const TABLE<GUM_SCALAR> &, Instantiation *);

    private:
    /// forbid copy operators
    MultiDimCompleteProjection<GUM_SCALAR, TABLE> &
    operator=(const MultiDimCompleteProjection<GUM_SCALAR, TABLE> &);
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/multidim/operators/multiDimCompleteProjection.tcc>

#endif /* GUM_MULTI_DIM_COMPLETE_PROJECTION_H */
