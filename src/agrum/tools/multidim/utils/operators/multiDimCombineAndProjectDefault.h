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
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_MULTI_DIM_COMBINE_AND_PROJECT_DEFAULT_H
#define GUM_MULTI_DIM_COMBINE_AND_PROJECT_DEFAULT_H

#include <agrum/tools/multidim/utils/operators/multiDimCombineAndProject.h>

namespace gum {

  // clang-format off
  /**
   * @class MultiDimCombineAndProjectDefault
   * @headerfile multiDimCombineAndProjectDefault.h <agrum/tools/multidim/operators/multiDimCombineAndProjectDefault.h>
   * @ingroup multidim_op_group
   *
   * @brief An efficient class for combining and projecting MultiDim tables
   */
  // clang-format on
  template < class TABLE >
  class MultiDimCombineAndProjectDefault: public MultiDimCombineAndProject< TABLE > {
    public:
    using value_type = typename MultiDimCombineAndProject< TABLE >::value_type;

    // ========================================================================
    /// @name Constructors / Destructors
    // ========================================================================
    /// @{

    /// Default constructor
    MultiDimCombineAndProjectDefault(TABLE (*combine)(const TABLE&, const TABLE&),
                                     TABLE (*project)(const TABLE&,
                                                      const Set< const DiscreteVariable* >&));

    /// Copy constructor
    MultiDimCombineAndProjectDefault(const MultiDimCombineAndProjectDefault< TABLE >&);

    /// Destructor
    virtual ~MultiDimCombineAndProjectDefault();

    /**
     * @brief virtual constructor
     *
     * @return a new fresh MultiDimCombineAndProjectDefault with the same
     * combination and projection functions.
     */
    virtual MultiDimCombineAndProjectDefault< TABLE >* clone() const;

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
     * argument contains less than two elements.
     */
    Set< const TABLE* > execute(const Set< const TABLE* >&            set,
                                const Set< const DiscreteVariable* >& del_vars) final;


    /** @brief returns the set of operations to perform to make all the combinations
     * and projections
     */
    std::pair< std::vector< ScheduleOperator* >, Set< const IScheduleMultiDim* > >
       operations(const Set< const IScheduleMultiDim* >& original_tables,
                  const Set< const DiscreteVariable* >&  del_vars,
                  const bool                             is_result_persistent = false) const final;
    std::pair< std::vector< ScheduleOperator* >, Set< const IScheduleMultiDim* > >
       operations(const std::vector< const IScheduleMultiDim* >& original_tables,
                  const Set< const DiscreteVariable* >&          del_vars,
                  const bool is_result_persistent = false) const final;

    /** @brief add to a given schedule the set of operations needed to perform all
     * the combinations and projections
     */
    using MultiDimCombineAndProject< TABLE >::schedule;

    /// changes the function used for combining two TABLES
    void setCombinationFunction(TABLE (*combine)(const TABLE&, const TABLE&)) final;

    /**
     * @brief changes the class that performs the combinations
     *
     * Combinations are performed by pair, the CombineFunction being the
     * function used to combine 2 tables, thus producing a new one. Of course,
     * when we have to combine, say T1 * T2 * T3 * T4, there are several ways
     * to do this computation. For instance, we might first compute T1 * T2,
     * then T3 * T4, and finally compute the product of the resulting two
     * tables. Or we might compute T1 * T2, then combine the result with T3,
     * and then combine the last result with T4. The CombinationClass is
     * responsible for determining which of these possibilities will actually
     * be used. Function setCombinationClass allows to change the
     * combinationClass and, thus, the way all those tables will be combined.
     */
    void setCombinationClass(const MultiDimCombination< TABLE >& comb_class) final;

    /// Returns the current combination function
    TABLE (*combinationFunction())(const TABLE&, const TABLE&) final;

    /// Changes the function used for projecting TABLES
    void setProjectionFunction(TABLE (*proj)(const TABLE&,
                                             const Set< const DiscreteVariable* >&)) final;

    /**
     * @brief Changes the class that performs the projections.
     *
     * As for the combination class, it is possible to change the projection
     * class, thus defining a new projection behaviour.
     */
    void setProjectionClass(const MultiDimProjection< TABLE >& proj_class) final;

    /// returns the current projection function
    TABLE (*projectionFunction())(const TABLE&, const Set< const DiscreteVariable* >&) final;

    /**
     * @brief returns a rough estimate of the number of operations that will be
     * performed to compute the combination.
     */
    double nbOperations(const Set< const TABLE* >&            set,
                        const Set< const DiscreteVariable* >& del_vars) const final;

    /**
     * @brief returns a rough estimate of the number of operations that will be
     * performed to compute the combination.
     */
    double nbOperations(const Set< const Sequence< const DiscreteVariable* >* >& set,
                        const Set< const DiscreteVariable* >& del_vars) const final;

    /**
     * @brief returns the memory consumption used during the combinations and
     * projections
     *
     * Actually, this function does not return a precise account of the memory
     * used by the MultiDimCombineAndProject but a rough estimate based on the
     * size of the tables involved in the combinations and projections.
     *
     * @return a pair of memory consumption: the first one is the maximum
     * amount of memory used during the set of combinations and projections
     * performed, and the second one is the amount of memory still used at the
     * end of the function ( the memory used by the resulting tables )
     */
    std::pair< double, double >
       memoryUsage(const Set< const TABLE* >&            set,
                   const Set< const DiscreteVariable* >& del_vars) const final;

    /**
     * @brief returns the memory consumption used during the combinations and
     * projections
     *
     * Actually, this function does not return a precise account of the memory
     * used by the MultiDimCombineAndProject but a rough estimate based on the
     * size of the tables involved in the combinations and projections.
     *
     * @return a pair of memory consumption: the first one is the maximum
     * amount of memory used during the set of combinations and projections
     * performed, and the second one is the amount of memory still used at the
     * end of the function ( the memory used by the resulting tables )
     */
    std::pair< double, double >
       memoryUsage(const Set< const Sequence< const DiscreteVariable* >* >& set,
                   const Set< const DiscreteVariable* >&                    del_vars) const final;

    /// @}

    private:
    /// the class used for the combinations
    MultiDimCombination< TABLE >* _combination_;

    /// the class used for the projections
    MultiDimProjection< TABLE >* _projection_;


    /// free the scheduling memory
    void _freeData_(std::vector< const IScheduleMultiDim* >& tables,
                    std::vector< ScheduleOperator* >&        operations) const;
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/tools/multidim/utils/operators/multiDimCombineAndProjectDefault_tpl.h>

#endif /* GUM_MULTI_DIM_COMBINE_AND_PROJECT_DEFAULT_H */
