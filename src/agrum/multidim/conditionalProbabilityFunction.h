/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
 * @brief Headers of CPF.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_CONDITIONAL_PROBABILITY_FUNCTION_H
#define GUM_CONDITIONAL_PROBABILITY_FUNCTION_H

#include <ostream>
#include <string>
#include <sstream>

#include <agrum/config.h>

#include <agrum/core/hashTable.h>
#include <agrum/variables/discreteVariable.h>
#include <agrum/multidim/instantiation.h>
#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/multidim/multiDimDecorator.h>

namespace gum {

  /**
   * @class CPF
   * @brief Abstract class representing a Conditionnal Probability Function.
   * @ingroup multidim_group
   *
   * A CPF in it's simplest form is a table (which gives a conditionnale
   * probability table). However since we want to exploit micro factors
   * and determinism more complex structures can be uses.
   * This class gives an abstraction on accessing the value P(X|Pa(X)).
   *
   * Variables in a CPF must all have different names.
   */
  template <typename GUM_SCALAR>

  class CPF : public MultiDimDecorator<GUM_SCALAR> {
    public:
    /// @name Constructors & Destructor

    /// @{
    /**
     * Default constructor.
     *
     * A MultiDimArray is used for representing the CPF.
     */
    CPF();

    /**
     * Constructor with user defined content.
     * The content will be deleted with this class.
     * @param content The implementation used for representing the CPF.
     */
    CPF(MultiDimImplementation<GUM_SCALAR> *content);

    /**
     * Copy constructor.
     */
    CPF(const CPF<GUM_SCALAR> &from);

    /**
     * Destructor.
     */
    virtual ~CPF();

    /// @}

    /**
     * This method creates a clone of this object, withouth its content
     * (including variable), you must use this method if you want to ensure
     * that the generated object has the same type than the object containing
     * the called newFactory()
     * For example :
     *   MultiDimArray<double> y;
     *   MultiDimContainer<double>* x = y.newFactory();
     * Then x is a MultiDimArray<double>*
     *
     * @warning you must desallocate by yourself the memory
     * @return an empty clone of this object with the same type
     */
    virtual CPF<GUM_SCALAR> *newFactory() const;

    /// Getters & setters

    /// @{
    /**
     * Add a new conditioned variable.
     * @param var The new conditioned variable added.
     * @throw DuplicateElement
     */
    virtual void add(const DiscreteVariable &var);

    /**
     *  Removes a dimension.
     *  @param var The variable which is removed.
     *  @throws NotFound If var isn't in this.
     *  @throws OperationNotAllowed If var can't be removed.
     */
    virtual void erase(const DiscreteVariable &var);

    /**
     * Adds a conditioning variable.
     * @param var The variable added as a conditioning variable.
    * @param isConditioned If true then var is added as a conditioned variable,
     *               otherwise it is added as a conditioning variable.
     * @throw DuplicateElement
     */
    void add(const DiscreteVariable &var, bool isConditioned);

    /**
    * Returns true if the variable is a conditioned variable.
    * @param var The variable that is tested.
     */
    bool isConditioned(const DiscreteVariable &var) const;

    /**
     * Returns a Instantiation initialized
     * over the conditioned variables.
     *
     * The methods parse all the sequence of variables to build the
     * Instantiation (i.e. it's not fast).
     *
     * To speed your code call this method this way:
     * @code
     * Instantiation inst = myCPF.conditionedVars();
     * @endcode
     */
    Instantiation conditionedVars();

    /**
     * Returns a constant Instantiation initialized
     * over the conditioned variables.
     *
     * The methods parse all the sequence of variables to build the
     * Instantiation (i.e. it's not fast).
     *
     * To speed your code call this method this way:
     * @code
     * const Instantiation inst = myCPF.conditionedVars();
     * @endcode
     */
    const Instantiation conditionedVars() const;

    /**
     * Returns a Instantiation initialized
     * over the conditioning variables.
     *
     * The methods parse all the sequence of variables to build the
     * Instantiation (i.e. it's not fast).
     *
     * To speed your code call this method this way:
     * @code
     * Instantiation inst = myCPF.conditioningVars();
     * @endcode
     */
    Instantiation conditioningVars();

    /**
     * Returns a constant Instantiation initialized
     * over the conditioning variables.
     *
     * The methods parse all the sequence of variables to build the
     * Instantiation (i.e. it's not fast).
     *
     * To speed your code call this method this way:
     * @code
     * const Instantiation inst = myCPF.conditioningVars();
     * @endcode
     */
    const Instantiation conditioningVars() const;

    /// @}

    /// @name Various methods

    /// @{
    /**
     * Used by operator<< for displaying the content of CPF.
     *
     * The result differs from the one of a MultiDimDecorator since
     * conditioned variables will be places at the left and conditioning variables
     * at the right.
     *
     * @return Returns a string with the content of this CPF.
     */
    virtual const std::string toString(const Instantiation *i) const;

    /// @}

    protected:
    virtual void _swap(const DiscreteVariable *x, const DiscreteVariable *y);

    private:
    // Used to no if a given is conditioned or not (if _cond[var] == true then
    // var is a conditioned variable).
    HashTable<const DiscreteVariable *, bool> *__condMap;
  };

} /* namespace gum */

#include <agrum/multidim/conditionalProbabilityFunction.tcc>

#endif /* GUM_CONDITIONAL_PROBABILITY_FUNCTION_H */
