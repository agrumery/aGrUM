
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

#ifndef GUM_MULTI_DIM_COMBINE_AND_PROJECT_H
#define GUM_MULTI_DIM_COMBINE_AND_PROJECT_H

#include <utility>

#include <agrum/core/sequence.h>
#include <agrum/core/set.h>
#include <agrum/variables/discreteVariable.h>

namespace gum {

  // clang-format off
  /**
   * @class MultiDimCombineAndProject
   * @headerfile multiDimCombineAndProject.h <agrum/multidim/operators/multiDimCombineAndProject.h>
   * @ingroup multidim_op_group
   *
   * @brief A generic interface to combine and project efficiently MultiDim tables.
   */
  // clang-format on
  template < typename GUM_SCALAR, template < typename > class TABLE >
  class MultiDimCombineAndProject {
    public:
    // ========================================================================
    /// @name Constructors / Destructors
    // ========================================================================
    /// @{

    /// default constructor
    MultiDimCombineAndProject();

    /// copy constructor
    MultiDimCombineAndProject(
       const MultiDimCombineAndProject< GUM_SCALAR, TABLE >&);

    /// destructor
    virtual ~MultiDimCombineAndProject();

    /**
     * @brief virtual constructor
     *
     * @return a new fresh MultiDimCombineAndProject with the same combination
     * and projection functions.
     */
    virtual MultiDimCombineAndProject< GUM_SCALAR, TABLE >* newFactory() const = 0;

    /// @}
    // ========================================================================
    /// @name Accessors/Modifiers
    // ========================================================================
    /// @{

    /**
     * @brief creates and returns the result of the projection over the
     * variables not in del_vars of the combination of the tables within set
     *
     * @return a new freshly created TABLE which is the result of the
     * projection of the combination of all the TABLES passed in argument
     *
     * @throws InvalidArgumentsNumber exception is thrown if the set passed in
     * argument contains less than two elements
     */
    virtual Set< const TABLE< GUM_SCALAR >* >
       combineAndProject(Set< const TABLE< GUM_SCALAR >* > set,
                         Set< const DiscreteVariable* >    del_vars) = 0;

    /// changes the function used for combining two TABLES
    virtual void setCombineFunction(TABLE< GUM_SCALAR >* (*combine)(
       const TABLE< GUM_SCALAR >&, const TABLE< GUM_SCALAR >&)) = 0;

    /// returns the current combination function
    virtual TABLE< GUM_SCALAR >* (*combineFunction())(
       const TABLE< GUM_SCALAR >&, const TABLE< GUM_SCALAR >&) = 0;

    /// changes the function used for projecting TABLES
    virtual void setProjectFunction(TABLE< GUM_SCALAR >* (*proj)(
       const TABLE< GUM_SCALAR >&, const Set< const DiscreteVariable* >&)) = 0;

    /// returns the current projection function
    virtual TABLE< GUM_SCALAR >* (*projectFunction())(
       const TABLE< GUM_SCALAR >&, const Set< const DiscreteVariable* >&) = 0;

    /**
     * @brief returns a rough estimate of the number of operations that will be
     * performed to compute the combination.
     */
    virtual float
       nbOperations(const Set< const TABLE< GUM_SCALAR >* >& set,
                    const Set< const DiscreteVariable* >&    del_vars) const = 0;
    virtual std::pair< long, long >
       memoryUsage(const Set< const Sequence< const DiscreteVariable* >* >& set,
                   Set< const DiscreteVariable* > del_vars) const = 0;

    /// @}

    private:
    /// forbid copy operators
    MultiDimCombineAndProject< GUM_SCALAR, TABLE >&
       operator=(const MultiDimCombineAndProject< GUM_SCALAR, TABLE >&);
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/multidim/utils/operators/multiDimCombineAndProject_tpl.h>

#endif /* GUM_MULTI_DIM_COMBINE_AND_PROJECT_H */
