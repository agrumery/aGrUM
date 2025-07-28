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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_MULTI_DIM_COMBINATION_H
#define GUM_MULTI_DIM_COMBINATION_H

#include <utility>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/graphicalModels/inference/scheduler/schedule.h>

namespace gum {

  /**
   * @class MultiDimCombination
   * @headerfile multiDimCombination.h <agrum/base/multidim/multiDimCombination.h>
   * @ingroup multidim_op_group
   *
   * @brief A generic interface to combine efficiently several MultiDim tables
   *
   * MultiDimCombination is a generic interface designed to combine efficiently
   * several multidimensional objects, that is, to compute expressions like T1
   * op T2 op T3 op .... op Tn, where the Ti's are the multidimensional objects
   * and op is an operator or a function taking in argument two such objects
   * and producing a new (combined) Ti object. By default, the combination
   * operation "op" is assumed to be COMMUTATIVE and ASSOCIATIVE. Note that
   * MultiDimCombinations always produce a new freshly allocated table as the
   * result of the combinations.
   *
   * By multidimensional objects, we mean of course MultiDimImplementations,
   * but also more complex objects such as, for instance, pairs of
   * MultiDimImplementation the first one of which being a utility function and
   * the second one being a table of instantiations (useful, e.g., for
   * computing MPE's) but this can also be a pair (Utility,Tensor) for the
   * inference in an Influence Diagram. Actually, the important point for a
   * multidimensional object to be eligible to be combined by the
   * MultiDimCombination is:
   *
   * # that there exists a function taking in arguments two such
   * multidimensional objects and producing a new object of the same type,
   * which is the so-called combined result of these two objects.
   *
   * To be quite generic, the MultiDimCombination takes in argument the
   * function that produces the result of the combination of two
   * multidimensional objects.  The following code gives an example of the
   * usage of MultiDimCombinations:
   *
   * @code
   * // a function used to combine two Tensor<float>'s:
   * Tensor<float> addTensor ( const Tensor<float>& t1,
   *                                 const Tensor<float>& t2 ) {
   *   return t1 + t2;
   * }
   *
   * // another function used to combine two Tensor<float>'s:
   * Tensor<float> multTensor ( const Tensor<float>& t1,
   *                                  const Tensor<float>& t2 ) {
   *   return t1 * t2;
   * }
   *
   * Tensor<float> t1, t2, t3;
   * Set<const Tensor<float>*> set;
   * set << &table1 << &table2 << &table3;
   * MultiDimCombinationDefault< Tensor< float > > Comb ( addTensor );
   * Tensor<float>* combined_table = Comb.execute ( set );
   *
   * // change the operator to apply
   * Comb.setCombinationFunction ( multTensor );
   * Tensor<float>* combined_table2 = Comb.execute ( set );
   *
   * @endcode
   */
  template < class TABLE >
  class MultiDimCombination {
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

    // =========================================================================
    /// @name Constructors / Destructors
    // =========================================================================
    /// @{

    /// default constructor
    MultiDimCombination();

    /// copy constructor
    MultiDimCombination(const MultiDimCombination< TABLE >&);

    /// destructor
    virtual ~MultiDimCombination();

    /**
     * @brief virtual constructor
     *
     * @return a new fresh MultiDimCombinator with the same combination
     * function.
     */
    virtual MultiDimCombination< TABLE >* clone() const = 0;

    /// @}
    // =========================================================================
    /// @name Accessors/Modifiers
    // =========================================================================
    /// @{

    /**
     * @brief creates and returns the result of the combination of the tables
     * within set
     *
     * @return a new freshly created TABLE which is the result of the
     * combination of all the TABLES passed in argument
     *
     * @throws InvalidArgumentsNumber exception is thrown if the set passed in
     * argument contains less than two elements.
     */
    virtual TABLE* execute(const Set< const TABLE* >& set) const                   = 0;
    virtual void   execute(TABLE& container, const Set< const TABLE* >& set) const = 0;

    /// returns the set of operations to perform as well as the result of the combination
    /** Executing sequentially the set of operations returned is guaranteed to
     * produce the right result. The latter is contained within the IScheduleMultiDim.
     * @warning MultiDimCombinations always produce a new freshly allocated resulting table
     */
    virtual std::pair< std::vector< ScheduleOperator* >, const IScheduleMultiDim* >
        operations(const std::vector< const IScheduleMultiDim* >& set,
                   const bool is_result_persistent = false) const
        = 0;
    virtual std::pair< std::vector< ScheduleOperator* >, const IScheduleMultiDim* >
        operations(const Set< const IScheduleMultiDim* >& set,
                   const bool                             is_result_persistent = false) const
        = 0;

    /// add to a given schedule the set of operations needed to perform the combination
    /** @warning whenever this method is executed, it is assumed that the
     * ScheduleMultiDim already belong to the schedule.
     * @warning MultiDimCombinations always produce a new freshly allocated resulting table
     */
    const IScheduleMultiDim* schedule(Schedule&                              schedule,
                                      const Set< const IScheduleMultiDim* >& set,
                                      const bool is_result_persistent = false) const;
    const IScheduleMultiDim* schedule(Schedule&                                      schedule,
                                      const std::vector< const IScheduleMultiDim* >& set,
                                      const bool is_result_persistent = false) const;

    /// changes the function used for combining two TABLES
    virtual void setCombinationFunction(TABLE (*combine)(const TABLE&, const TABLE&)) = 0;

    /// returns the combination function currently used by the combinator
    virtual TABLE (*combinationFunction())(const TABLE&, const TABLE&) = 0;

    /**
     * @brief returns a rough estimate of the number of operations that will be
     * performed to compute the combination.
     */
    virtual double nbOperations(const Set< const TABLE* >& set) const = 0;
    virtual double nbOperations(const Set< const Sequence< const DiscreteVariable* >* >& set) const
        = 0;

    /**
     * @brief Returns the memory consumption used during the combination.
     *
     * Actually, this function does not return a precise account of the memory
     * used by the multidimCombination but a rough estimate based on the sizes
     * of the tables involved in the combination.
     *
     * @return a pair of memory consumption: the first one is the maximum
     * amount of memory used during the combination and the second one is the
     * amount of memory still used at the end of the function ( the memory used
     * by the resulting table ).
     */
    virtual std::pair< double, double > memoryUsage(const Set< const TABLE* >& set) const = 0;
    virtual std::pair< double, double >
        memoryUsage(const Set< const Sequence< const DiscreteVariable* >* >& set) const = 0;

    private:
    /// forbid copy operators
    MultiDimCombination< TABLE >& operator=(const MultiDimCombination< TABLE >&);

    /// @}
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/base/multidim/utils/operators/multiDimCombination_tpl.h>

#endif /* GUM_MULTI_DIM_COMBINATION_H */
