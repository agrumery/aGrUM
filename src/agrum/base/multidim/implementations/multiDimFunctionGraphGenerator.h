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


/**
 * @file
 * @brief Headers of gum::MultiDimFunctionGraphGenerator.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 *
 */
#ifndef GUM_MULTI_DIM_FUNCTION_GRAPH_GENERATOR_H
#define GUM_MULTI_DIM_FUNCTION_GRAPH_GENERATOR_H

#include <agrum/agrum.h>

#include <agrum/base/multidim/implementations/multiDimFunctionGraph.h>

namespace gum {
  /**
   * @class MultiDimFunctionGraphGenerator
   * @ingroup multidim_group
   *
   * @brief Class implementing a function graph generator with template type
   * double.
   *
   * @warning Doxygen does not like spanning command on multiple line, so we
   * could not configure it with the correct include directive. Use the
   * following code snippet to include this file.
   * @code
   * #include
   * <agrum/base/multidim/implementations/multiDimFunctionGraphGenerator.h>
   * @endcode
   *
   */
  class MultiDimFunctionGraphGenerator {
    public:
    // =========================================================================
    /// @name Constructors / Destructors
    // =========================================================================

    /// @{
    /**
     * @brief Default constructor.
     */
    MultiDimFunctionGraphGenerator(Idx                                        maxVar,
                                   Idx                                        minVar,
                                   const Sequence< const DiscreteVariable* >& varSeq);

    /**
     * @brief Class destructor.
     */
    ~MultiDimFunctionGraphGenerator();

    /// @}

    /**
     * @brief Generates a MultiDimFunctionGraph.
     */
    MultiDimFunctionGraph< double >* generate();


    private:
    /**
     * @brief Creates a leaf.
     */
    bool _createLeaf_(NodeId currentNodeId, HashTable< NodeId, Idx >& node2MinVar);

    /**
     * @brief Generate a variable position.
     */
    Idx _generateVarPos_(Idx offset, Idx span);

    /// The variables
    const Sequence< const DiscreteVariable* > _varSeq_;

    /// The total number of variables
    Idx _nbTotalVar_;

    /// The seed for random numbers
    static Idx _genSeed_;
  };
}   // namespace gum
#endif /* GUM_MULTI_DIM_FUNCTION_GRAPH_GENERATOR_H */
