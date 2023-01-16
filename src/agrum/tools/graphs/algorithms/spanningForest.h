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


/** @file
 * @brief Interface for computing min cost spanning trees or forests
 *
 * @author Jean-Philippe DUBUS & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_SPANNING_FOREST_H
#define GUM_SPANNING_FOREST_H
#include <agrum/agrum.h>

#include <agrum/tools/graphs/undiGraph.h>

namespace gum {

  /* ===========================================================================
   */
  /** @class SpanningForest
   * @brief Base class for computing min cost spanning trees or forests */
  /* ===========================================================================
   */
  class SpanningForest {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// Destructor
    virtual ~SpanningForest();

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// Returns the edges in a min cost spanning forest
    /** @returns edges in the spanning forest */
    virtual const EdgeSet& edgesInSpanningForest() = 0;

    /// Construct the spanning forest
    /** @return the spanning forest */
    virtual const UndiGraph& spanningForest() = 0;

    /// Returns the cost of the spanning forest
    /** @return cost of the spanning forest */
    virtual float costOfSpanningForest() = 0;

    /// @}

    protected:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** You should use SpanningForestPrim or SpanningForestKuskal to create
     * appropriate spanning trees algorithms. */
    SpanningForest();

    /// Copy constructor
    SpanningForest(const SpanningForest& toCopy);

    /// Copy operator
    /** avoid copying the interface from scratch: this would prevent, in
     * particular, that a Prim algorithm be initialized by a Kruskal algo. */
    SpanningForest& operator=(const SpanningForest& toCopy);

    /// @}
  };

} /* namespace gum */

#endif /* GUM_SPANNING_FOREST_H */
