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
 * @brief An efficient class for combining and projecting MultiDim tables
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_MULTI_DIM_COMBINE_AND_PROJECT_DEFAULT_H
#define GUM_MULTI_DIM_COMBINE_AND_PROJECT_DEFAULT_H

#include <agrum/multidim/operators/multiDimCombinationDefault.h>
#include <agrum/multidim/operators/multiDimProjection.h>
#include <agrum/multidim/operators/multiDimCombineAndProject.h>

namespace gum {

  template <typename GUM_SCALAR, template <typename> class TABLE>
  class MultiDimCombineAndProjectDefault
      : public MultiDimCombineAndProject<GUM_SCALAR, TABLE> {

    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    MultiDimCombineAndProjectDefault(
        TABLE<GUM_SCALAR> *(*combine)(const TABLE<GUM_SCALAR> &,
                                      const TABLE<GUM_SCALAR> &),
        TABLE<GUM_SCALAR> *(*project)(const TABLE<GUM_SCALAR> &,
                                      const Set<const DiscreteVariable *> &));

    /// copy constructor
    MultiDimCombineAndProjectDefault(
        const MultiDimCombineAndProjectDefault<GUM_SCALAR, TABLE> &);

    /// destructor
    virtual ~MultiDimCombineAndProjectDefault();

    /// virtual constructor
    /** @return a new fresh MultiDimCombineAndProjectDefault with the same
     * combination and projection functions. */
    virtual MultiDimCombineAndProjectDefault<GUM_SCALAR, TABLE> *newFactory() const;

    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /** @brief creates and returns the result of the projection over the variables
     * not in del_vars of the combination of the tables within set
     *
     * @return a new freshly created TABLE which is the result of the projection
     * of the combination of all the TABLES passed in argument
     * @throws InvalidArgumentsNumber exception is thrown if the set passed in
     * argument contains less than two elements */
    virtual Set<const TABLE<GUM_SCALAR> *>
    combineAndProject(Set<const TABLE<GUM_SCALAR> *> set,
                      Set<const DiscreteVariable *> del_vars);

    /// changes the function used for combining two TABLES
    virtual void
    setCombineFunction(TABLE<GUM_SCALAR> *(*combine)(const TABLE<GUM_SCALAR> &,
                                                     const TABLE<GUM_SCALAR> &));

    /// changes the class that performs the combinations
    /** Combinations are performed by pair, the CombineFunction being the function
     * used to combine 2 tables, thus producing a new one. Of course, when we have
     * to combine, say T1 * T2 * T3 * T4, there are several ways to do this
     * computation. For instance, we might first compute T1 * T2, then T3 * T4,
     * and finally compute the product of the resulting two tables. Or we might
     * compute T1 * T2, then combine the result with T3, and then combine the last
     * result with T4. The CombinationClass is responsible for determining which
     * of these possibilities will actually be used. Function setCombinationClass
     * allows to change the combinationClass and, thus, the way all those tables
     * will be combined. */
    virtual void
    setCombinationClass(const MultiDimCombination<GUM_SCALAR, TABLE> &comb_class);

    /// returns the current combination function
    virtual TABLE<GUM_SCALAR> *(*combineFunction())(const TABLE<GUM_SCALAR> &,
                                                    const TABLE<GUM_SCALAR> &);

    /// changes the function used for projecting TABLES
    virtual void setProjectFunction(TABLE<GUM_SCALAR> *(*proj)(
        const TABLE<GUM_SCALAR> &, const Set<const DiscreteVariable *> &));

    /// changes the class that performs the projections
    /** As for the combination class, it is possible to change the projection
     * class, thus defining a new projection behaviour. */
    virtual void
    setProjectionClass(const MultiDimProjection<GUM_SCALAR, TABLE> &proj_class);

    /// returns the current projection function
    virtual TABLE<GUM_SCALAR> *(*projectFunction())(
        const TABLE<GUM_SCALAR> &, const Set<const DiscreteVariable *> &);

    /** @brief returns a rough estimate of the number of operations that will be
     * performed to compute the combination */
    virtual float nbOperations(const Set<const TABLE<GUM_SCALAR> *> &set,
                               const Set<const DiscreteVariable *> &del_vars) const;
    virtual float
    nbOperations(const Set<const Sequence<const DiscreteVariable *> *> &set,
                 Set<const DiscreteVariable *> del_vars) const;

    /// returns the memory consumption used during the combinations and projections
    /** Actually, this function does not return a precise account of the memory
     * used by the MultiDimCombineAndProject but a rough estimate based on the size
     * of the tables involved in the combinations and projections.
     * @return a pair of memory consumption: the first one is the maximum
     * amount of memory used during the set of combinations and projections
     * performed, and the second one is the amount of memory still used at the end
     * of the function ( the memory used by the resulting tables ) */
    virtual std::pair<long, long>
    memoryUsage(const Set<const TABLE<GUM_SCALAR> *> &set,
                const Set<const DiscreteVariable *> &del_vars) const;
    virtual std::pair<long, long>
    memoryUsage(const Set<const Sequence<const DiscreteVariable *> *> &set,
                Set<const DiscreteVariable *> del_vars) const;

    /// @}

    private:
    /// the class used for the combinations
    MultiDimCombination<GUM_SCALAR, TABLE> *__combination;

    /// the class used for the projections
    MultiDimProjection<GUM_SCALAR, TABLE> *__projection;
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/multidim/operators/multiDimCombineAndProjectDefault.tcc>

#endif /* GUM_MULTI_DIM_COMBINE_AND_PROJECT_DEFAULT_H */
