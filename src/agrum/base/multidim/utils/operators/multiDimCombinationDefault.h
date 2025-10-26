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

#ifndef GUM_MULTI_DIM_COMBINATION_DEFAULT_H
#define GUM_MULTI_DIM_COMBINATION_DEFAULT_H

#include <agrum/agrum.h>

#include <agrum/base/multidim/utils/operators/multiDimCombination.h>

namespace gum {

  // clang-format off
  /**
   * @class MultiDimCombinationDefault
   * @headerfile multiDimCombinationDefault.h <agrum/base/multidim/operators/multiDimCombinationDefault.h>
   * @brief A class to combine efficiently several MultiDim tables
   * @ingroup multidim_op_group
   *
   * MultiDimCombinationDefault is a class designed to combine efficiently
   * several multidimensional objects, that is, to compute expressions like T1
   * op T2 op T3 op .... op Tn, where the Ti's are the multidimensional objects
   * and op is an operator or a function taking in argument two such objects
   * and producing a new (combined) Ti object. Note that the
   * MultiDimCombinationDefault determines itself in which order the objects
   * should be combined. As such, the combination operation to perform should
   * thus be COMMUTATIVE and ASSOCIATIVE. Note also that MultiDimCombinationDefault
   * always produce a new freshly allocated table as the result of the combinations.
   *
   * By multidimensional objects, we mean of course MultiDimImplementations,
   * but also more complex objects such as, for instance, pairs of
   * MultiDimImplementations the first one of which being a utility function
   * and the second one being a table of instantiations (useful, e.g., for
   * computing MPE's) but this can also be a pair (Utility,Tensor) for the
   * inference in an Influence Diagram. Actually, the important point for a
   * multidimensional object to be eligible to be combined by the
   * MultiDimCombinationDefault is:
   * # that the object contains a method variablesSequence that returns a
   * sequence of Discrete variables that represent the dimensions of the
   * multidimensional object
   * # that there exists a function taking in arguments two such
   * multidimensional objects and producing a new object of the same type,
   * which is the so-called combined result of these two objects.
   *
   * To be quite generic, the MultiDimCombinationDefault takes in argument the
   * function that produces the result of the combination of two
   * multidimensional objects. The following code gives an example of the usage
   * of MultiDimCombinations:
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
   *
   * Tensor<float> t1, t2, t3;
   * Set< const Tensor< float >* > set;
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
  // clang-format on

  template < class TABLE >
  class MultiDimCombinationDefault final: public MultiDimCombination< TABLE > {
    public:
    // ========================================================================
    /// @name Constructors / Destructors
    // ========================================================================
    /// @{

    /**
     * @brief Default constructor.
     *
     * @param combine a function that takes two tables in input and produces a
     * new table which is the result of the combination of the two tables
     * passed in argument.
     */
    MultiDimCombinationDefault(TABLE (*combine)(const TABLE&, const TABLE&));

    /// Copy constructor
    MultiDimCombinationDefault(const MultiDimCombinationDefault< TABLE >&);

    /// Destructor
    virtual ~MultiDimCombinationDefault();

    /**
     * @brief virtual constructor
     *
     * @return A new fresh MultiDimCombinator with the same combination
     * function.
     */
    virtual MultiDimCombinationDefault< TABLE >* clone() const;

    /// @}
    // ========================================================================
    /// @name Accessors/Modifiers
    // ========================================================================
    /// @{

    /**
     * @brief Creates and returns the result of the combination of the tables
     * within set.
     *
     * @return a new freshly created TABLE which is the result of the
     * combination of all the TABLES passed in argument
     *
     * @throws InvalidArgumentsNumber exception is thrown if the set passed in
     * argument contains less than two elements.
     */
    TABLE* execute(const Set< const TABLE* >& set) const final;
    void   execute(TABLE& container, const Set< const TABLE* >& set) const final;

    /// returns the set of operations to perform to make the combination
    /** Executing sequentially the set of operations returned is guaranteed to
     * produce the right result.
     * @warning MultiDimCombinations always produce a new freshly allocated resulting table
     */
    std::pair< std::vector< ScheduleOperator* >, const IScheduleMultiDim* >
        operations(const std::vector< const IScheduleMultiDim* >& set,
                   const bool is_result_persistent = false) const final;
    std::pair< std::vector< ScheduleOperator* >, const IScheduleMultiDim* >
        operations(const Set< const IScheduleMultiDim* >& set,
                   const bool                             is_result_persistent = false) const final;

    /// add to a given schedule the set of operations needed to perform the combination
    using MultiDimCombination< TABLE >::schedule;

    /// Changes the function used for combining two TABLES.
    void setCombinationFunction(TABLE (*combine)(const TABLE&, const TABLE&)) final;

    /// Returns the combination function currently used by the combinator.
    TABLE (*combinationFunction())(const TABLE&, const TABLE&) final;

    /**
     * @brief returns a rough estimate of the number of operations that will be
     * performed to compute the combination.
     */
    double nbOperations(const Set< const TABLE* >& set) const final;
    double nbOperations(const Set< const Sequence< const DiscreteVariable* >* >& set) const final;

    /**
     * @brief Returns the additional memory consumption used during the
     * combination.
     *
     * Actually, this function does not return a precise account of the memory
     * used by the multidimCombination but a rough estimate based on the sizes
     * of the tables involved in the combination.  @return a pair of memory
     * consumption: the first one is the maximum amount of memory used during
     * the combination and the second one is the amount of memory still used at
     * the end of the function ( the memory used by the resulting table ).
     */
    std::pair< double, double > memoryUsage(const Set< const TABLE* >& set) const final;
    std::pair< double, double >
        memoryUsage(const Set< const Sequence< const DiscreteVariable* >* >& set) const final;

    /// @}

    private:
    /// The function used to combine two tables.
    TABLE (*_combine_)(const TABLE& t1, const TABLE& t2);

    /**
     * @brief returns the domain size of the Cartesian product of the union of
     * all the variables in seq1 and seq2.
     */
    double _combinedSize_(const IScheduleMultiDim& table1, const IScheduleMultiDim& table2) const;

    /// free the scheduling memory
    void _freeData_(std::vector< const IScheduleMultiDim* >& tables,
                    std::vector< ScheduleOperator* >&        operations) const;
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/base/multidim/utils/operators/multiDimCombinationDefault_tpl.h>

#endif /* GUM_MULTI_DIM_COMBINATION_DEFAULT_H */
