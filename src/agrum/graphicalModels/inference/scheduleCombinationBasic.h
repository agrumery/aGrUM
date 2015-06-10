/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
/** @file
 * @brief A class to combine efficiently several ScheduleMultiDims
 *
 * MultiDimCombinationBasic is a class designed to combine efficiently
 * several ScheduleMultiDims, that is, to compute expressions like
 * T1 op T2 op T3 op .... op Tn, where the Ti's are either the ScheduleMultiDims
 * or just MultiDimImplementations that will be later wrapped into
 * ScheduleMultiDims and op is an operator or a function taking in argument two
 * such objects and producing a new (combined) Ti object. By default, the
 * combination operation "op" is assumed to be COMMUTATIVE and ASSOCIATIVE.
 *
 * To be quite generic, the MultiDimCombinationBasic takes in argument the
 * function that produces the result of the combination of two multidimensional
 * objects. The following code gives an example of the usage of
 * MultiDimCombinationBasics:
 * @code
 * // a function used to combine two MultiDimImplementation<float>'s:
 * MultiDimImplementation<float>* add ( const MultiDimImplementation<float>& t1,
 *                                      const MultiDimImplementation<float>& t2 ) {
 *   return new MultiDimImplementation<float> (t1 + t2);
 * }
 *
 * // another function used to combine two MultiDimImplementation<float>'s:
 * MultiDimImplementation<float>*
 * mult ( const MultiDimImplementation<float>& t1,
 *        const MultiDimImplementation<float>& t2 ) {
 *   return new MultiDimImplementation<float> (t1 * t2);
 * }
 *
 *
 * MultiDimImplementation<float> t1, t2, t3;
 * Set<const MultiDimImplementation<float>*> set;
 * set << &t1 << &t2 << &t3;
 * ScheduleCombinationBasic<float> Comb ( add );
 * ScheduleMultiDim<float> combined_table = Comb.combine ( set );
 *
 * // do the same thing only with ScheduleMultiDims
 * MultiDimImplementation<float> tt4, tt5;
 * ScheduleMultiDim<float> t4 ( &tt4 ), t5 ( &tt5 );
 * Set<const ScheduleMultiDim<float>*> set;
 * set << &t4 << &t5;
 * ScheduleCombinationBasic<float> Comb ( add );
 * ScheduleMultiDim<float> combined_table2 = Comb.combine ( set );
 *
 * // change the operator to apply
 * Comb.setCombineFunction ( mult );
 * ScheduleMultiDim<float> combined_table3 = Comb.combine ( set );
 *
 * @endcode
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_SCHEDULE_COMBINATION_BASIC_H
#define GUM_SCHEDULE_COMBINATION_BASIC_H

#include <agrum/core/sequence.h>
#include <agrum/variables/discreteVariable.h>
#include <agrum/graphicalModels/inference/scheduleCombination.h>

namespace gum {

  template <typename GUM_SCALAR>
  class ScheduleCombinationBasic : public ScheduleCombination<GUM_SCALAR> {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    ScheduleCombinationBasic(MultiDimImplementation<GUM_SCALAR> *(*combine)(
        const MultiDimImplementation<GUM_SCALAR> &,
        const MultiDimImplementation<GUM_SCALAR> &));

    /// copy constructor
    ScheduleCombinationBasic(const ScheduleCombinationBasic<GUM_SCALAR> &);

    /// destructor
    virtual ~ScheduleCombinationBasic();

    /// virtual constructor
    /** @return a new fresh ScheduleCombinator with the same combination
     * function. */
    virtual ScheduleCombinationBasic<GUM_SCALAR> *newFactory() const;

    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    // adds to a given schedule the operations necessary to perform a combination
    /** @returns the ScheduleMultiDim resulting from the combination
     * @throws InvalidArgumentsNumber exception is thrown if the set passed in
     * argument contains less than two elements */
    virtual ScheduleMultiDim<GUM_SCALAR>
    combine(const Set<const ScheduleMultiDim<GUM_SCALAR> *> &set,
            Schedule<GUM_SCALAR> &);
    ScheduleMultiDim<GUM_SCALAR>
    combine(const Set<const MultiDimImplementation<GUM_SCALAR> *> &set,
            Schedule<GUM_SCALAR> &);
    template <template <typename> class TABLE>
    ScheduleMultiDim<GUM_SCALAR> combine(const Set<const TABLE<GUM_SCALAR> *> &set,
                                         Schedule<GUM_SCALAR> &);

    /// changes the function used for combining two TABLES
    virtual void setCombineFunction(MultiDimImplementation<GUM_SCALAR> *(*combine)(
        const MultiDimImplementation<GUM_SCALAR> &,
        const MultiDimImplementation<GUM_SCALAR> &));

    /// returns the combination function currently used by the combinator
    virtual MultiDimImplementation<GUM_SCALAR> *(*combineFunction())(
        const MultiDimImplementation<GUM_SCALAR> &,
        const MultiDimImplementation<GUM_SCALAR> &);

    /** @brief returns a rough estimate of the number of operations that will be
     * performed to compute the combination */
    virtual float nbOperations(const Set<const ScheduleMultiDim<GUM_SCALAR> *> &set,
                               const Schedule<GUM_SCALAR> &schedule);
    float nbOperations(const Set<const MultiDimImplementation<GUM_SCALAR> *> &set,
                       const Schedule<GUM_SCALAR> &schedule);
    template <template <typename> class TABLE>
    float nbOperations(const Set<const TABLE<GUM_SCALAR> *> &set,
                       const Schedule<GUM_SCALAR> &schedule);

    /// returns the memory consumption used during the combination
    /** Actually, this function does not return a precise account of the memory
     * used by the multidimCombination but a rough estimate based on the sizes
     * of the tables involved in the combination.
     * @return a pair of memory consumption: the first one is the maximum
     * amount of memory used during the combination and the second one is the
     * amount of memory still used at the end of the function ( the memory used by
     * the resulting table ) */
    virtual std::pair<long, long>
    memoryUsage(const Set<const ScheduleMultiDim<GUM_SCALAR> *> &set,
                const Schedule<GUM_SCALAR> &schedule);
    std::pair<long, long>
    memoryUsage(const Set<const MultiDimImplementation<GUM_SCALAR> *> &set,
                const Schedule<GUM_SCALAR> &schedule);
    template <template <typename> class TABLE>
    std::pair<long, long> memoryUsage(const Set<const TABLE<GUM_SCALAR> *> &set,
                                      const Schedule<GUM_SCALAR> &schedule);

    /// @}

    protected:
    /// the function used to combine two tables
    MultiDimImplementation<GUM_SCALAR> *(*_combine)(
        const MultiDimImplementation<GUM_SCALAR> &t1,
        const MultiDimImplementation<GUM_SCALAR> &t2);

    /** @brief returns the domain size of the Cartesian product of the union of
     * all the variables in seq1 and seq2 */
    Size _combinedSize(const Sequence<const DiscreteVariable *> &seq1,
                       const Sequence<const DiscreteVariable *> &seq2) const;
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/graphicalModels/inference/scheduleCombinationBasic.tcc>

#endif /* GUM_SCHEDULE_COMBINATION_BASIC_H */
