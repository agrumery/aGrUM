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

#ifndef GUM_MULTI_DIM_COMBINE_AND_PROJECT_H
#define GUM_MULTI_DIM_COMBINE_AND_PROJECT_H

#include <utility>

#include <agrum/tools/multidim/utils/operators/multiDimCombinationDefault.h>
#include <agrum/tools/multidim/utils/operators/multiDimProjection.h>


namespace gum {

  // clang-format off
  /**
   * @class MultiDimCombineAndProject
   * @headerfile multiDimCombineAndProject.h <agrum/tools/multidim/operators/multiDimCombineAndProject.h>
   * @ingroup multidim_op_group
   *
   * @brief A generic interface to combine and project efficiently MultiDim tables.
   */
  // clang-format on
  template < class TABLE >
  class MultiDimCombineAndProject {
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    // class to get the types of the TABLE's values using metaprogramming
    template < typename T >
    struct TableType {
      using value_type = T;
    };

    template < template < typename, typename... > class CONTAINER, typename T, typename... Args >
    struct TableType< CONTAINER< T, Args... > > {
      using value_type = T;
    };
#endif   // DOXYGEN_SHOULD_SKIP_THIS

    public:
    /// the type of the values contained into TABLE
    using value_type = typename TableType< TABLE >::value_type;

    // ========================================================================
    /// @name Constructors / Destructors
    // ========================================================================
    /// @{

    /// default constructor
    explicit MultiDimCombineAndProject();

    /// copy constructor
    MultiDimCombineAndProject(const MultiDimCombineAndProject< TABLE >&);

    /// destructor
    virtual ~MultiDimCombineAndProject();

    /**
     * @brief virtual constructor
     *
     * @return a new fresh MultiDimCombineAndProject with the same combination
     * and projection functions.
     */
    virtual MultiDimCombineAndProject< TABLE >* clone() const = 0;

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
    virtual Set< const TABLE* > execute(const Set< const TABLE* >&            set,
                                        const Set< const DiscreteVariable* >& del_vars)
       = 0;

    /** @brief returns the set of operations to perform to make all the combinations
     * and projections
     */
    virtual std::pair< std::vector< ScheduleOperator* >, Set< const IScheduleMultiDim* > >
       operations(const Set< const IScheduleMultiDim* >& original_tables,
                  const Set< const DiscreteVariable* >&  del_vars,
                  const bool                             is_result_persistent = false) const
       = 0;
    virtual std::pair< std::vector< ScheduleOperator* >, Set< const IScheduleMultiDim* > >
       operations(const std::vector< const IScheduleMultiDim* >& original_tables,
                  const Set< const DiscreteVariable* >&          del_vars,
                  const bool                                     is_result_persistent = false) const
       = 0;

    /** @brief add to a given schedule the set of operations needed to perform all
     * the combinations and projections
     */
    Set< const IScheduleMultiDim* > schedule(Schedule&                              schedule,
                                             const Set< const IScheduleMultiDim* >& original_tables,
                                             const Set< const DiscreteVariable* >&  del_vars,
                                             const bool is_result_persistent = false) const;

    /// changes the function used for combining two TABLES
    virtual void setCombinationFunction(TABLE (*combine)(const TABLE&, const TABLE&)) = 0;

    /**f
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
    virtual void setCombinationClass(const MultiDimCombination< TABLE >& comb_class) = 0;

    /// returns the current combination function
    virtual TABLE (*combinationFunction())(const TABLE&, const TABLE&) = 0;

    /// changes the function used for projecting TABLES
    virtual void setProjectionFunction(TABLE (*proj)(const TABLE&,
                                                     const Set< const DiscreteVariable* >&))
       = 0;

    /// returns the current projection function
    virtual TABLE (*projectionFunction())(const TABLE&, const Set< const DiscreteVariable* >&) = 0;

    /**
     * @brief Changes the class that performs the projections.
     *
     * As for the combination class, it is possible to change the projection
     * class, thus defining a new projection behaviour.
     */
    virtual void setProjectionClass(const MultiDimProjection< TABLE >& proj_class) = 0;

    /**
     * @brief returns a rough estimate of the number of operations that will be
     * performed to compute the combination.
     */
    virtual double nbOperations(const Set< const TABLE* >&            set,
                                const Set< const DiscreteVariable* >& del_vars) const
       = 0;

    /**
     * @brief returns a rough estimate of the number of operations that will be
     * performed to compute the combination.
     */
    virtual double nbOperations(const Set< const Sequence< const DiscreteVariable* >* >& set,
                                const Set< const DiscreteVariable* >& del_vars) const
       = 0;

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
    virtual std::pair< double, double >
       memoryUsage(const Set< const TABLE* >&            set,
                   const Set< const DiscreteVariable* >& del_vars) const
       = 0;

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
    virtual std::pair< double, double >
       memoryUsage(const Set< const Sequence< const DiscreteVariable* >* >& set,
                   const Set< const DiscreteVariable* >&                    del_vars) const
       = 0;

    /// @}

    private:
    /// forbid copy operators
    MultiDimCombineAndProject< TABLE >& operator=(const MultiDimCombineAndProject< TABLE >&);
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/tools/multidim/utils/operators/multiDimCombineAndProject_tpl.h>

#endif /* GUM_MULTI_DIM_COMBINE_AND_PROJECT_H */
