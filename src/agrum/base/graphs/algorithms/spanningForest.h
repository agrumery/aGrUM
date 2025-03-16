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





/** @file
 * @brief Interface for computing min cost spanning trees or forests
 *
 * @author Jean-Philippe DUBUS & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_SPANNING_FOREST_H
#define GUM_SPANNING_FOREST_H
#include <agrum/agrum.h>

#include <agrum/base/graphs/undiGraph.h>

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
