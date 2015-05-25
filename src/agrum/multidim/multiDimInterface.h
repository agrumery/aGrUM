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
/**
 * @file
 * @brief Interface for all classes addressing in a multidimensional fashion.
 *
 * This class is a base interface for Instantiation and for all classes
 * addressed by a Instantiation (mainly: MultiDimContainer and hence for all
 * multidimensional containers). Its purpose is to deal with the dimensions
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES <{prenom.nom}_at_lip6.fr>
 */

/*!
  @ingroup multidim_group
  \page multidim_page How to use the MultiDim hierarchy.

  \section multidim_sec_1 Why the MultiDim hierarchy?
*/

#ifndef GUM_MULTIDIM_INTERFACE_H
#define GUM_MULTIDIM_INTERFACE_H

#include <iostream>
#include <vector>

#include <agrum/config.h>

#include <agrum/core/list.h>
#include <agrum/core/sequence.h>

#include <agrum/variables/discreteVariable.h>

namespace gum {

  class Instantiation;

  /**
   * @class MultiDimInterface multiDimInterface.h agrum/multidim/multiDimInterface.h
   *
   * @brief interface for all classes addressing in a multiDim fashion
   * @ingroup multidim_group
   *
   * This class is a base interface for Instantiation and for all classes
   * addressed by a Instantiation (mainly: MultiDimContainer and hence for all
   * dimensionnal containers). Its purpose is to deal with the dimensions.
   *
   * @ingroup multidim_group
   */

  class MultiDimInterface {
    public:
    /// Alias for Sequence<DiscreteVariable*>::iterator.
    typedef Sequence<const DiscreteVariable *>::iterator iterator;

    /// No constructor : interface only.

    virtual ~MultiDimInterface(){};

    // ##############################################################################
    /// @name Accessors / Modifiers
    // ##############################################################################
    /// @{

    /// Returns the number of vars in the multidimensional container.

    virtual Idx nbrDim() const = 0;

    /**
     * @brief Returns the product of the size of the domains of the variables
     *        belonging to the matrix.
     */

    virtual Size domainSize() const = 0;

    /**
     * @brief Adds a new var to the variables of the multidimensional matrix.
     * @see operator<<(MultiDimInterface& c, const DiscreteVariable& v)
     *
     * @param v The new var.
     * @warning Note that the variable passed in argument is not duplicated, that
     *          is, only a pointer toward the variable is kept by the
     *          MultiDimInterface.
     * @throw DuplicateElement is thrown if the variable already belongs to the
     *                         sequence of variables.
     * @throw OperationNotAllowed if this object is non mutable.
     */

    virtual void add(const DiscreteVariable &v) = 0;

    /**
     * @brief Removes a var from the variables of the multidimensional matrix.
     * @see operator>>(MultiDimInterface& c, const DiscreteVariable& v)
     *
     * If the variable does not belong to the MultiDimBase, then this method,
     * NotFound is thrown.
     * @throw OperationNotAllowed if this object is non mutable.
     * @throw NotFound is v does not belong to this
     */

    virtual void erase(const DiscreteVariable &v) = 0;

    /// Returns a const ref to the sequence of DiscreteVariable*.

    virtual const Sequence<const DiscreteVariable *> &variablesSequence() const = 0;

    /**
     * Returns a const ref to the ith var.
     * @throw NotFound
     */

    virtual const DiscreteVariable &variable(Idx i) const = 0;

    /**
     * Returns the index of a var.
     * @throw NotFound
     */

    virtual Idx pos(const DiscreteVariable &v) const = 0;

    /// Returns true if var is in *this.

    virtual bool contains(const DiscreteVariable &v) const = 0;

    /// Returns true if no var is in *this.

    virtual bool empty(void) const = 0;

    /**
     * @brief Swap two variables in this multidim.
     * If x is in this MultiDim and y has the same domain size,
     * then x will be replace by y in this MultiDim.
     *
     * @param x The variable in this which will be replaced.
     * @param y The variable replacing y.
     * @throw NotFound Raised if x does not belong to this MultiDim.
     * @throw OperationNotAllowed If y and x are not interchangeable.
     * @throw DuplicateElement If y is already in this MultiDim.
     */
    void swap(const DiscreteVariable &x, const DiscreteVariable &y);

    /// @}

    // ############################################################################
    /// @name Iterators
    // ############################################################################
    /// @{

    /// Iterator pointing at the beginning of the Sequence of variables.

    iterator begin() const;

    /// Iterator pointing at the rbeginning of the Sequence of variables.

    iterator rbegin() const;

    /// Constant reference on the iterator pointing at the end of the Sequence
    /// of variables.

    const iterator &end() const;

    /// Constant reference on the iterator pointing at the rend of the Sequence
    /// of variables.

    const iterator &rend() const;

    /// @}
    protected:
    /**
     * @brief This is called by MultiDimContainer::swap() to proceed with the
     *        swapping between x and y.
     * This is called only when everything have been checked.
     */
    virtual void _swap(const DiscreteVariable *x, const DiscreteVariable *y) = 0;
  };

  /**
   * @brief Adds a new var to the sequence of vars.
   *
   * This function is not VIRTUAL (call of add which IS virtual).
   * @warning Note that the variable passed in argument is not duplicated, that is,
   *          only a pointer toward the variable is kept by the MultiDimInterface
   *
   * @code mat << var1 << var2 @endcode will insert @code var1 @endcode first.
   *
   * @param c the multidim container
   * @param v the new var
   * @return a reference to *this
   *
   * @throw DuplicateElement is thrown if the variable already belongs to the
   *                         sequence of variables.
   * @throw OperationNotAllowed if *this is non mutable
   *
   * @ingroup multidim_group
   */

  MultiDimInterface &operator<<(MultiDimInterface &c, const DiscreteVariable &v);

  /**
   * @brief Removes a var from the variables of the MutliDimAdressing.
   *
   * @throw OperationNotAllowed if this object is non mutable.
   * @throw NotFound is v does not belong to this
   *
   * @ingroup multidim_group
   */

  MultiDimInterface &operator>>(MultiDimInterface &c, const DiscreteVariable &v);

} /* namespace gum */

#ifndef GUM_NO_INLINE
#include <agrum/multidim/multiDimInterface.inl>
#endif // GUM_NO_INLINE

#endif /* GUM_MULTIDIM_INTERFACE_H */
