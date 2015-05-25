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
 * @brief A generic interface to combine efficiently several MultiDim tables
 *
 * MultiDimCombination is a generic interface designed to combine efficiently
 * several multidimensional objects, that is, to compute expressions like
 * T1 op T2 op T3 op .... op Tn, where the Ti's are the multidimensional objects
 * and op is an operator or a function taking in argument two such objects and
 * producing a new (combined) Ti object. By default, the combination operation "op"
 * is assumed to be COMMUTATIVE and ASSOCIATIVE.
 *
 * By multidimensional objects, we mean of course MultiDimDecorators,
 * MultiDimImplementations, but also more complex objects such as, for instance,
 * pairs of MultiDimDecorators the first one of which being a utility function and
 * the second one being a table of instantiations (useful, e.g., for computing
 * MPE's) but this can also be a pair (Utility,Potential) for the inference in
 * an Influence Diagram. Actually, the important point for a multidimensional
 * object to be eligible to be combined by the MultiDimCombination is:
 * # that there exists a function taking in arguments two such multidimensional
 *   objects and producing a new object of the same type, which is the so-called
 *   combined result of these two objects.
 *
 * To be quite generic, the MultiDimCombination takes in argument the function
 * that produces the result of the combination of two multidimensional objects.
 * The following code gives an example of the usage of MultiDimCombinations:
 * @code
 * // a function used to combine two Potential<float>'s:
 * Potential<float>* addPotential ( const Potential<float>& t1,
 *                                  const Potential<float>& t2 ) {
 *   return new Potential<float> (t1 + t2);
 * }
 *
 * // another function used to combine two Potential<float>'s:
 * Potential<float>* multPotential ( const Potential<float>& t1,
 *                                   const Potential<float>& t2 ) {
 *   return new Potential<float> (t1 * t2);
 * }
 *
 *
 * Potential<float> t1, t2, t3;
 * Set<const Potential<float>*> set;
 * set << &table1 << &table2 << &table3;
 * MultiDimCombinationDefault<float,Potential> Comb ( addPotential );
 * Potential<float>* combined_table = Comb.combine ( set );
 *
 * // change the operator to apply
 * Comb.setCombineFunction ( multPotential );
 * Potential<float>* combined_table2 = Comb.combine ( set );
 *
 * @endcode
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_MULTI_DIM_COMBINATION_H
#define GUM_MULTI_DIM_COMBINATION_H

#include <utility>
#include <agrum/core/set.h>
#include <agrum/core/sequence.h>
#include <agrum/variables/discreteVariable.h>

namespace gum {

  template <typename GUM_SCALAR, template <typename> class TABLE>
  class MultiDimCombination {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    MultiDimCombination();

    /// copy constructor
    MultiDimCombination(const MultiDimCombination<GUM_SCALAR, TABLE> &);

    /// destructor
    virtual ~MultiDimCombination();

    /// virtual constructor
    /** @return a new fresh MultiDimCombinator with the same combination
     * function. */
    virtual MultiDimCombination<GUM_SCALAR, TABLE> *newFactory() const = 0;

    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// creates and returns the result of the combination of the tables within set
    /** @return a new freshly created TABLE which is the result of the combination
     * of all the TABLES passed in argument
     * @throws InvalidArgumentsNumber exception is thrown if the set passed in
     * argument contains less than two elements */
    virtual TABLE<GUM_SCALAR> *
    combine(const Set<const TABLE<GUM_SCALAR> *> &set) = 0;
    virtual void combine(TABLE<GUM_SCALAR> &container,
                         const Set<const TABLE<GUM_SCALAR> *> &set) = 0;

    /// changes the function used for combining two TABLES
    virtual void
    setCombineFunction(TABLE<GUM_SCALAR> *(*combine)(const TABLE<GUM_SCALAR> &,
                                                     const TABLE<GUM_SCALAR> &)) = 0;

    /// returns the combination function currently used by the combinator
    virtual TABLE<GUM_SCALAR> *(*combineFunction())(const TABLE<GUM_SCALAR> &,
                                                    const TABLE<GUM_SCALAR> &) = 0;

    /** @brief returns a rough estimate of the number of operations that will be
     * performed to compute the combination */
    virtual float nbOperations(const Set<const TABLE<GUM_SCALAR> *> &set) const = 0;
    virtual float nbOperations(
        const Set<const Sequence<const DiscreteVariable *> *> &set) const = 0;

    /// returns the memory consumption used during the combination
    /** Actually, this function does not return a precise account of the memory
     * used by the multidimCombination but a rough estimate based on the sizes
     * of the tables involved in the combination.
     * @return a pair of memory consumption: the first one is the maximum
     * amount of memory used during the combination and the second one is the
     * amount of memory still used at the end of the function ( the memory used by
     * the resulting table ) */
    virtual std::pair<long, long>
    memoryUsage(const Set<const TABLE<GUM_SCALAR> *> &set) const = 0;
    virtual std::pair<long, long> memoryUsage(
        const Set<const Sequence<const DiscreteVariable *> *> &set) const = 0;

    /// @}

    private:
    /// forbid copy operators
    MultiDimCombination<GUM_SCALAR, TABLE> &
    operator=(const MultiDimCombination<GUM_SCALAR, TABLE> &);
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/multidim/operators/multiDimCombination.tcc>

#endif /* GUM_MULTI_DIM_COMBINATION_H */
