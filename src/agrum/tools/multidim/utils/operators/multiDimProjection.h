
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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
 *
 * @author Christophe GONZALES (@AMU) and Pierre-Henri WUILLEMIN (@LIP6)
 */

#ifndef GUM_MULTI_DIM_PROJECTION_H
#define GUM_MULTI_DIM_PROJECTION_H

#include <utility>

#include <agrum/tools/core/sequence.h>
#include <agrum/tools/core/set.h>
#include <agrum/tools/variables/discreteVariable.h>

namespace gum {

  // clang-format off
  /**
   * @class MultiDimProjection
   * @headerfile multiDimProjection.h <agrum/tools/multidim/operators/multiDimProjection.h>
   * @ingroup multidim_op_group
   *
   * @brief A generic class to project efficiently a MultiDim table over a subset
   * of its variables
   *
   * MultiDimProjection is a generic wrapper designed to project efficiently a
   * multidimensional object over a subset of its variables.
   *
   * By multidimensional objects, we mean of course MultiDimImplementations,
   * but also more complex objects such as, for instance, pairs of
   * MultiDimImplementations the first one of which being a utility function
   * and the second one being a table of instantiations (useful, e.g., for
   * computing MPE's) but this can also be a pair (Utility,Potential) for the
   * inference in an Influence Diagram.
   *
   * To be quite generic, the MultiDimProjection takes in argument the function
   * that produces the result of the projection of the multidimensional object.
   * The following code gives an example of the usage of MultiDimProjection:
   *
   * @code
   * // a function used to project a Potential<float>:
   * Potential<float>* MinPot ( const Potential<float>& table,
   *                            const Set<const DiscreteVariable*>& del_vars ) {
   *   return new Potential<float> (...);
   * }
   *
   * // another function used to project a Potential<float>:
   * Potential<float>* MaxPot ( const Potential<float>& table,
   *                            const Set<const DiscreteVariable*>& del_vars ) {
   *   return new Potential<float> (...);
   * }
   *
   *
   * Potential<float> t1, t2;
   * Set<const DiscreteVariable*> set1, set2;
   * MultiDimProjectionDefault<float,Potential> Proj ( MinPot );
   * Potential<float>* projected_table = Proj.project ( t1, set1 );
   *
   * // change the operator to apply
   * Proj.setProjectFunction ( MaxPot );
   * Potential<float>* projected_table2 = Proj.project ( t2, set2 );
   *
   * @endcode
   */
  // clang-format on
  template < typename GUM_SCALAR, template < typename > class TABLE >
  class MultiDimProjection {
    public:
    // ========================================================================
    /// @name Constructors / Destructors
    // ========================================================================
    /// @{

    /// Default constructor
    MultiDimProjection(TABLE< GUM_SCALAR >* (*proj)(
       const TABLE< GUM_SCALAR >&, const Set< const DiscreteVariable* >&));

    /// Copy constructor
    MultiDimProjection(const MultiDimProjection< GUM_SCALAR, TABLE >&);

    /// Destructor
    virtual ~MultiDimProjection();

    /**
     * @brief virtual constructor
     *
     * @return a new fresh MultiDimCombinator with the same projection
     * function.
     */
    virtual MultiDimProjection< GUM_SCALAR, TABLE >* newFactory() const;

    /// @}
    // ========================================================================
    /// @name Accessors/Modifiers
    // ========================================================================
    /// @{

    /**
     * @brief Creates and returns the projection of the table over a subset of
     * its vars.
     *
     * @return A new freshly created TABLE which is the result of the
     * projection of the TABLE passed in argument over the set of variables NOT
     * IN del_vars
     *
     * @warning If del_vars is precisely equal to the variables of table, the
     * result is an empty table.
     */
    TABLE< GUM_SCALAR >* project(const TABLE< GUM_SCALAR >&            table,
                                 const Set< const DiscreteVariable* >& del_vars);

    void project(TABLE< GUM_SCALAR >&                     container,
                 const TABLE< GUM_SCALAR >&               table,
                 const Set< const TABLE< GUM_SCALAR >* >& del_vars);

    /// Changes the function used for projecting TABLES
    void setProjectFunction(TABLE< GUM_SCALAR >* (*proj)(
       const TABLE< GUM_SCALAR >&, const Set< const DiscreteVariable* >&));

    /// Returns the projection function currently used by the projector
    TABLE< GUM_SCALAR >* (*projectFunction())(
       const TABLE< GUM_SCALAR >&, const Set< const DiscreteVariable* >&);

    /**
     * @brief returns a rough estimate of the number of operations that will be
     * performed to compute the projection.
     */
    float nbOperations(const TABLE< GUM_SCALAR >&            table,
                       const Set< const DiscreteVariable* >& del_vars) const;

    /**
     * @brief returns a rough estimate of the number of operations that will be
     * performed to compute the projection.
     */
    float nbOperations(const Sequence< const DiscreteVariable* >& vars,
                       const Set< const DiscreteVariable* >&      del_vars) const;

    /**
     * @brief Returns the memory consumption used during the projection.
     *
     * Actually, this function does not return a precise account of the memory
     * used by the multidimProjection but a rough estimate based on the size of
     * the table involved in the projection.
     *
     * @return A pair of memory consumption: the first one is the maximum
     * amount of memory used during the combination and the second one is the
     * amount of memory still used at the end of the function ( the memory used
     * by the resulting table )
     */
    std::pair< long, long >
       memoryUsage(const TABLE< GUM_SCALAR >&            table,
                   const Set< const DiscreteVariable* >& del_vars) const;

    /**
     * @brief Returns the memory consumption used during the projection.
     *
     * Actually, this function does not return a precise account of the memory
     * used by the multidimProjection but a rough estimate based on the size of
     * the table involved in the projection.
     *
     * @return A pair of memory consumption: the first one is the maximum
     * amount of memory used during the combination and the second one is the
     * amount of memory still used at the end of the function ( the memory used
     * by the resulting table )
     */
    std::pair< long, long >
       memoryUsage(const Sequence< const DiscreteVariable* >& vars,
                   const Set< const DiscreteVariable* >&      del_vars) const;

    /// @}

    protected:
    /// The projection function actually used
    TABLE< GUM_SCALAR >* (*_proj)(const TABLE< GUM_SCALAR >&,
                                  const Set< const DiscreteVariable* >&);

    private:
    /// Forbid copy operators
    MultiDimProjection< GUM_SCALAR, TABLE >&
       operator=(const MultiDimProjection< GUM_SCALAR, TABLE >&);
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/tools/multidim/utils/operators/multiDimProjection_tpl.h>

#endif /* GUM_MULTI_DIM_PROJECTION_H */