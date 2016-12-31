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
 * @brief Headers of MultiDimInterface.
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
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

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  // Forward declaration.
  class Instantiation;
#endif

  // ==========================================================================
  // ===                    MULTIDIM INTERFACE                              ===
  // ==========================================================================
  /**
   * @class MultiDimInterface
   * @headerfile multiDimInterface.h <agrum/multidim/multiDimInterface.h>
   * @ingroup multidim_group
   *
   * @brief Interface for all classes addressing in a multiDim fashion.
   *
   * This class is a base interface for Instantiation and for all classes
   * addressed by a Instantiation (mainly: MultiDimContainer and hence for all
   * dimensionnal containers). Its purpose is to deal with the dimensions.
   *
   */
  class MultiDimInterface {
    public:
    /// Alias for Sequence<DiscreteVariable*>::iterator.
    typedef Sequence<const DiscreteVariable*>::iterator iterator;

    /**
     * @brief Class destructor.
     * @warning No constructor : interface only.
     */
    virtual ~MultiDimInterface(){};

    // =========================================================================
    /// @name Accessors / Modifiers
    // =========================================================================
    /// @{

    /**
     * @brief Returns the number of vars in the multidimensional container.
     * @return Returns the number of vars in the multidimensional container.
     */
    virtual Idx nbrDim() const = 0;

    /**
     * @brief Returns the product of the variables domain size.
     * @return Returns the product of the variables domain size.
     */
    virtual Size domainSize() const = 0;

    /**
     * @brief Adds a new var to the variables of the multidimensional matrix.
     * @see operator<<(MultiDimInterface& c, const DiscreteVariable& v)
     *
     * @warning Note that the variable passed in argument is not duplicated,
     * that is, only a pointer toward the variable is kept by the
     * MultiDimInterface.
     *
     * @param v The new var.
     *
     * @throw DuplicateElement Raised if the variable already belongs to the
     * sequence of variables.
     * @throw OperationNotAllowed Raised if this object is non mutable.
     */
    virtual void add( const DiscreteVariable& v ) = 0;

    /**
     * @brief Removes a var from the variables of the multidimensional matrix.
     * @see operator>>(MultiDimInterface& c, const DiscreteVariable& v)
     *
     * @throw OperationNotAllowed Raised if this object is non mutable.
     * @throw NotFound Raised if v does not belong to this.
     */
    virtual void erase( const DiscreteVariable& v ) = 0;

    /**
     * @brief Returns a const ref to the sequence of DiscreteVariable*.
     * @return Returns a const ref to the sequence of DiscreteVariable*.
     */
    virtual const Sequence<const DiscreteVariable*>& variablesSequence() const = 0;

    /**
     * @brief Returns a const ref to the ith var.
     * param i An index of this multidimensional matrix.
     * @return Returns a const ref to the ith var.
     * @throw NotFound Raised if i does not reference a variable in this
     * multidimensional matrix.
     */
    virtual const DiscreteVariable& variable( Idx i ) const = 0;

    /**
     * @brief Returns the index of a variable.
     * @param v The variable for which the index is returned.
     * @return Returns the index of a variable.
     * @throw NotFound Raised if v is not in this multidimensional matrix.
     */
    virtual Idx pos( const DiscreteVariable& v ) const = 0;

    /**
     * @brief Returns true if var is in *this.
     * @param v A DiscreteVariable.
     * @return Returns true if var is in *this.
     */
    virtual bool contains( const DiscreteVariable& v ) const = 0;

    /**
     * @brief Returns true if no var is in *this.
     * @return Returns true if no var is in *this.
     */
    virtual bool empty( void ) const = 0;

    /**
     * @brief Swap two variables in this multidim.
     *
     * If x is in this MultiDim and y has the same domain size,
     * then x will be replace by y in this MultiDim.
     *
     * @param x The variable in this which will be replaced.
     * @param y The variable replacing y.
     * @throw NotFound Raised if x does not belong to this MultiDim.
     * @throw OperationNotAllowed If y and x are not interchangeable.
     * @throw DuplicateElement If y is already in this MultiDim.
     */
    void swap( const DiscreteVariable& x, const DiscreteVariable& y );

    /// @}
    // =========================================================================
    /// @name Iterators
    // =========================================================================
    /// @{

    /**
     * @brief Returns an iterator pointing at the beginning of the Sequence of
     * variables.
     * @return Returns an iterator pointing at the beginning of the Sequence of
     * variables.
     */
    iterator begin() const;

    /**
     * @brief Returns an iterator pointing at the rbeginning of the Sequence
     * of variables.
     * @return Returns an iterator pointing at the rbeginning of the Sequence
     * of variables.
     */
    iterator rbegin() const;

    /**
     * @brief Returns a constant reference on the iterator pointing at the end
     * of the Sequence of variables.
     * @return Returns a constant reference on the iterator pointing at the end
     * of the Sequence of variables.
     */
    const iterator& end() const;

    /**
     * @brief Constant reference on the iterator pointing at the rend of the
     * Sequence of variables.
     * @return Constant reference on the iterator pointing at the rend of the
     * Sequence of variables.
     */
    const iterator& rend() const;

    /// @}
    protected:
    /**
     * @brief This is called by MultiDimContainer::swap() to proceed with the
     * swapping between x and y.
     *
     * This is called only when everything have been checked.
     *
     * @param x The first variable to swap.
     * @param y The second variable to swap.
     */
    virtual void _swap( const DiscreteVariable* x, const DiscreteVariable* y ) = 0;
  };

  /**
   * @brief Adds a new var to the sequence of vars.
   * @ingroup multidim_group
   *
   * This function is not VIRTUAL (call of add which IS virtual).  @warning
   * Note that the variable passed in argument is not duplicated, that is, only
   * a pointer toward the variable is kept by the MultiDimInterface
   *
   * @code mat << var1 << var2; @endcode will insert @code var1 @endcode first.
   *
   * @param c the multidim container
   * @param v the new var
   * @return a reference to *this
   *
   * @throw DuplicateElement Raised if the variable already belongs to the
   * sequence of variables.
   * @throw OperationNotAllowed Raised if *this is non mutable.
   */
  MultiDimInterface& operator<<( MultiDimInterface& c, const DiscreteVariable& v );

  /**
   * @brief Removes a var from the variables of the MutliDimAdressing.
   * @ingroup multidim_group
   *
   * @throw OperationNotAllowed Raised if this object is non mutable.
   * @throw NotFound Raised if v does not belong to this
   */
  MultiDimInterface& operator>>( MultiDimInterface& c, const DiscreteVariable& v );

} /* namespace gum */

#ifndef GUM_NO_INLINE
#include <agrum/multidim/multiDimInterface_inl.h>
#endif  // GUM_NO_INLINE

#endif /* GUM_MULTIDIM_INTERFACE_H */
