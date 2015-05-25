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
 * @brief A generic interface to combine efficiently several ScheduleMultiDims
 *
 * MultiDimCombination is a generic interface designed to combine efficiently
 * several ScheduleMultiDims, that is, to compute expressions like
 * T1 op T2 op T3 op .... op Tn, where the Ti's are either the ScheduleMultiDims
 * or just MultiDimImplementations that will be later wrapped into
 * ScheduleMultiDims and op is an operator or a function taking in argument two
 * such objects and producing a new (combined) Ti object. By default, the
 * combination operation "op" is assumed to be COMMUTATIVE and ASSOCIATIVE.
 *
 * To be quite generic, the MultiDimCombination takes in argument the function
 * that produces the result of the combination of two multidimensional objects.
 * The following code gives an example of the usage of MultiDimCombinations:
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

#ifndef GUM_SCHEDULE_COMBINATION_H
#define GUM_SCHEDULE_COMBINATION_H

#include <utility>
#include <agrum/core/set.h>
#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/graphicalModels/inference/scheduleMultiDim.h>
#include <agrum/graphicalModels/inference/schedule.h>

namespace gum {

  template <typename GUM_SCALAR> class ScheduleCombination {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    ScheduleCombination();

    /// copy constructor
    ScheduleCombination(const ScheduleCombination<GUM_SCALAR> &);

    /// destructor
    virtual ~ScheduleCombination();

    /// virtual constructor
    /** @return a new fresh ScheduleCombinator with the same combination
     * function. */
    virtual ScheduleCombination<GUM_SCALAR> *newFactory() const = 0;

    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    // adds operations to an already created schedule
    /**  @returns the ScheduleMultiDim resulting from the combination
     * @throws InvalidArgumentsNumber exception is thrown if the set passed in
     * argument contains less than two elements */
    virtual ScheduleMultiDim<GUM_SCALAR>
    combine(const Set<const ScheduleMultiDim<GUM_SCALAR> *> &set,
            Schedule<GUM_SCALAR> &schedule) = 0;
    ScheduleMultiDim<GUM_SCALAR>
    combine(const Set<const MultiDimImplementation<GUM_SCALAR> *> &set,
            Schedule<GUM_SCALAR> &schedule);
    template <template <typename> class TABLE>
    ScheduleMultiDim<GUM_SCALAR> combine(const Set<const TABLE<GUM_SCALAR> *> &set,
                                         Schedule<GUM_SCALAR> &schedule);

    /// changes the function used for combining two TABLES
    virtual void setCombineFunction(MultiDimImplementation<GUM_SCALAR> *(*combine)(
        const MultiDimImplementation<GUM_SCALAR> &,
        const MultiDimImplementation<GUM_SCALAR> &)) = 0;

    /// returns the combination function currently used by the combinator
    virtual MultiDimImplementation<GUM_SCALAR> *(*combineFunction())(
        const MultiDimImplementation<GUM_SCALAR> &,
        const MultiDimImplementation<GUM_SCALAR> &) = 0;

    /** @brief returns a rough estimate of the number of operations that will be
     * performed to compute the combination */
    virtual float nbOperations(const Set<const ScheduleMultiDim<GUM_SCALAR> *> &set,
                               const Schedule<GUM_SCALAR> &schedule) = 0;
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
                const Schedule<GUM_SCALAR> &schedule) = 0;
    std::pair<long, long>
    memoryUsage(const Set<const MultiDimImplementation<GUM_SCALAR> *> &set,
                const Schedule<GUM_SCALAR> &schedule);
    template <template <typename> class TABLE>
    std::pair<long, long> memoryUsage(const Set<const TABLE<GUM_SCALAR> *> &set,
                                      const Schedule<GUM_SCALAR> &schedule);

    /// @}

    private:
    /// forbid copy operators
    ScheduleCombination<GUM_SCALAR> &
    operator=(const ScheduleCombination<GUM_SCALAR> &);
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/graphicalModels/inference/scheduleCombination.tcc>

#endif /* GUM_SCHEDULE_COMBINATION_H */
