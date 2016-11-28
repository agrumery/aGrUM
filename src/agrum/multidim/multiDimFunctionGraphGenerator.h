/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
 * @brief Headers of gum::MultiDimFunctionGraphGenerator.
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 * @author Jean-Christophe Magnan
 *
 */
#ifndef GUM_MULTI_DIM_FUNCTION_GRAPH_GENERATOR_H
#define GUM_MULTI_DIM_FUNCTION_GRAPH_GENERATOR_H

#include <agrum/config.h>
#include <agrum/multidim/multiDimFunctionGraph.h>

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
   * #include <agrum/multidim/multiDimFunctionGraphGenerator.h>
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
        Idx maxVar, Idx minVar, const Sequence<const DiscreteVariable*>& varSeq );

    /**
     * @brief Class destructor.
     */
    ~MultiDimFunctionGraphGenerator();

    /// @}

    /**
     * @brief Generates a MultiDimFunctionGraph.
     */
    MultiDimFunctionGraph<double>* generate();


    private:
    /**
     * @brief Creates a leaf.
     */
    bool __createLeaf( NodeId currentNodeId, HashTable<NodeId, Idx>& node2MinVar );

    /**
     * @brief Generate a variable position.
     */
    Idx __generateVarPos( Idx offset, Idx span );

    /// The variables
    const Sequence<const DiscreteVariable*> __varSeq;

    /// The total number of variables
    Idx __nbTotalVar;

    /// The seed for random numbers
    static Idx __genSeed;
  };
} /* end of namespace */
#endif /* GUM_MULTI_DIM_FUNCTION_GRAPH_GENERATOR_H */
