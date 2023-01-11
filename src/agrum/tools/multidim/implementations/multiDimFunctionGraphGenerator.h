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
#include <agrum/tools/multidim/implementations/multiDimFunctionGraph.h>

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
   * <agrum/tools/multidim/implementations/multiDimFunctionGraphGenerator.h>
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
