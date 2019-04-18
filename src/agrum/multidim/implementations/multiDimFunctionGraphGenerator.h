
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
 * @brief Headers of gum::MultiDimFunctionGraphGenerator.
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 *
 */
#ifndef GUM_MULTI_DIM_FUNCTION_GRAPH_GENERATOR_H
#define GUM_MULTI_DIM_FUNCTION_GRAPH_GENERATOR_H

#include <agrum/agrum.h>
#include <agrum/multidim/implementations/multiDimFunctionGraph.h>

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
   * #include <agrum/multidim/implementations/multiDimFunctionGraphGenerator.h>
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
    MultiDimFunctionGraphGenerator(
       Idx maxVar, Idx minVar, const Sequence< const DiscreteVariable* >& varSeq);

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
    bool __createLeaf(NodeId currentNodeId, HashTable< NodeId, Idx >& node2MinVar);

    /**
     * @brief Generate a variable position.
     */
    Idx __generateVarPos(Idx offset, Idx span);

    /// The variables
    const Sequence< const DiscreteVariable* > __varSeq;

    /// The total number of variables
    Idx __nbTotalVar;

    /// The seed for random numbers
    static Idx __genSeed;
  };
}   // namespace gum
#endif /* GUM_MULTI_DIM_FUNCTION_GRAPH_GENERATOR_H */
