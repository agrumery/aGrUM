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
 * @brief base class for all encapsulation of MultiDimImplementation
 *
 * A MultiDimDecorator is a virtual class for all encapsulation of
 * MultiDimImplementation * (for instance probability, utility, etc.).
 * It implements a decorator design pattern in order to have a array/tree/sparse
 * matrix/... implementation for multiDim
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES <{prenom.nom}_at_lip6.fr>
 */
#ifndef GUM_MULTI_DIM_DECORATOR_H
#define GUM_MULTI_DIM_DECORATOR_H

#include <agrum/config.h>

#include <agrum/multidim/multiDimArray.h>


namespace gum {


  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                         GUM_ALGEBRAIC_ELEMENT                       === */
  /* =========================================================================== */
  /* =========================================================================== */
  /** @class MultiDimDecorator
   * @brief decorator design pattern in order to separate implementations
   * (array,tree,etc.) for MultiDim concept (potential, CPF, utility Table, etc.)
   * @ingroup multidim_group
   *
   * A MultiDimDecorator is a virtual class for all encapsulation of
   * MultiDimImplementation * (for instance probability, utility, etc.).
   * It implements a decorator design pattern in order to have a array/tree/sparse
   * matrix/... implementation for multiDim<*T_DATA*>
   */
  /* =========================================================================== */
  template<typename T_DATA>

  class MultiDimDecorator : public MultiDimContainer<T_DATA> {
  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{
    // ============================================================================
    /// Constructor.
    // ============================================================================
    MultiDimDecorator( MultiDimImplementation<T_DATA> *aContent );

    // ============================================================================
    /// Destructor.
    // ============================================================================
    ~MultiDimDecorator();

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{
    // ============================================================================
    /// Get the size of domains - final method.
    // ============================================================================
    Size domainSize() const ;


    // ============================================================================
    /**
     * Add a new var to the sequence of vars.
     * @param v The new var.
     * @throw DuplicateElement
     */
    // ============================================================================
    virtual void add( const DiscreteVariable& v );

    // ============================================================================
    /// Give a const ref to the sequence of DiscreteVariable*. final method.
    // ============================================================================
    const Sequence<const DiscreteVariable *>& variablesSequence() const ;

    // ============================================================================
    /// Get the nbr of vars in the sequence. final method.
    // ============================================================================
    Idx nbrDim() const ;

    // ============================================================================
    /**
     *  Removes a dimension.
     *  @param var The variable wich is removed.
     *  @throws NotFound Raised if var isn't in this.
     *  @throws OperationNotAllowed Raised if var can't be removed.
     */
    // ============================================================================
    virtual void erase( const DiscreteVariable& var );

    // ============================================================================
    /**
     * Returns the corresponding variable.
     * @throws NotFound Raised if the index doesn't exist.
     */
    // ============================================================================
    virtual const DiscreteVariable& variable( Idx ) const;

    // ============================================================================
    /**
     *  Returns the index of the given variable.
     *  @throws NotFound Raised if the variable does not belong to this
     */
    // ============================================================================
    virtual Idx pos( const DiscreteVariable& ) const;

    // ============================================================================
    /// Returns true if the variable does not belong to this.
    // ============================================================================
    virtual bool contains( const DiscreteVariable& ) const;

    // ============================================================================
    /// Returns true if this is empty.
    // ============================================================================
    virtual bool empty() const;

    // ============================================================================
    /// Unregister a slave.
    // ============================================================================
    virtual bool unregisterSlave( Instantiation& );

    // ============================================================================
    /// Register a slave.
    // ============================================================================
    virtual bool registerSlave( Instantiation& i );

    // ============================================================================
    /// Fills this with the content of d.
    // ============================================================================
    virtual void fill( const T_DATA& d ) const;

    // ============================================================================
    /// Protected access to content.
    // ============================================================================
    const MultiDimImplementation<T_DATA>* content() const;

    // ============================================================================
    /// Access to content.
    // ============================================================================
    MultiDimImplementation<T_DATA>* content();

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
    virtual MultiDimDecorator<T_DATA>* newFactory() const =0;


    // ############################################################################
    /// @name Notification methods
    // ############################################################################
    /// @{
    // ============================================================================
    /**
     * listen to change in each recorded Instantiation. final method
     * @param i the Instantiation
     * @param var the changed dim.
     * @param oldval the old value
     * @param newval the new value
     */
    // ============================================================================
    void changeNotification( Instantiation& i,
                             const DiscreteVariable* const var,
                             const Idx& oldval,const Idx& newval );

    // ============================================================================
    /// listen to an assignment of a value in a Instantiation
    // ============================================================================
    void setChangeNotification( Instantiation& i );

    // ============================================================================
    /**
     * listen to setFirst in each recorded Instantiation. final method.
     * @param i the Instantiation
     */
    // ============================================================================
    void setFirstNotification( Instantiation& i );

    // ============================================================================
    /**
     * listen to setLast in each recorded Instantiation. final method.
     * @param i the Instantiation
     */
    // ============================================================================
    void setLastNotification( Instantiation& i );

    // ============================================================================
    /**
     * listen to increment in each recorded Instantiation. final method.
     * @param i the Instantiation
     */
    // ============================================================================
    void setIncNotification( Instantiation& i );

    // ============================================================================
    /**
     * listen to increment in each recorded Instantiation. final method.
     * @param i the Instantiation
     */
    // ============================================================================
    void setDecNotification( Instantiation& i );

    // ============================================================================
    /** @brief notification modification on vars to all Instantiation listeners.
     * final method. */
    // ============================================================================
    virtual void notifyChange() const;

    // ============================================================================
    /**
     * Returns the Master reference of this.
     * A Instantiation who wants to know his *true* master should ask for
     * master->getMasterRef()
     */
    // ============================================================================
    virtual MultiDimImplementation<T_DATA>& getMasterRef( void );

    // ============================================================================
    /**
     * Returns the Master reference of this.
     * A Instantiation who wants to know his *true* master should ask for
     * master->getMasterRef()
     */
    // ============================================================================
    virtual const MultiDimImplementation<T_DATA>& getMasterRef( void ) const;

    /// @}


    // ############################################################################
    /// @name Fast large modifications in structures.
    // ############################################################################
    /// @{
    // ============================================================================
    virtual void beginMultipleChanges( void );
    virtual void endMultipleChanges( void );
    virtual void endMultipleChanges( const T_DATA& );

    /// @}


    // ############################################################################
    /// @name Various methods.
    // ############################################################################
    /// @{
    // ============================================================================
    /// string representation of internal data about i in this.
    // ============================================================================
    virtual const std::string toString( const Instantiation *i ) const ;

    /// string representation of this.
    virtual const std::string toString( ) const ;

    // ============================================================================
    /**
     * Perform an homothety on a multiDim container.
     * @param alpha The ratio.
     * @param mul The chosen mult operation.
     */
    // ============================================================================
    virtual void
    homothetic( T_DATA alpha,T_DATA( *mul )( const T_DATA,const T_DATA ) );

    // ============================================================================
    /**
     * Iterate add on each element of a multiDim container.
     * @param add The chosen folded operation.
     * @return Returns the sum of values contains in the multiDim.
     */
    // ============================================================================
    virtual T_DATA fold( T_DATA( *add )( const T_DATA,const T_DATA ) ) const;

    /// @}

    /// by default, set just calls _get as a r-value
    virtual void set( const Instantiation& i,const T_DATA& value ) const;

    /// by default, get just calls _get as a l-value
    virtual T_DATA get( const Instantiation& i ) const ;
  protected:
    /// protecte method to swap the implementation behind the Potential
    /// @warning unsafe method for slave Instantiations !
    void _swapContent(MultiDimImplementation<T_DATA> *aContent) const;

    // ============================================================================
    /// The true container.
    // ============================================================================
    mutable MultiDimImplementation<T_DATA> *_content;


    // ============================================================================
    /**
     * Return a data, given a Insantiation - final method.
     * @param i The instantiation.
     * @throw NullElement
     * @throw NotFound
     */
    // ============================================================================
    T_DATA& _get( const Instantiation& i ) const;

  };


} /* namespace gum */


#include <agrum/multidim/multiDimDecorator.tcc>


#endif /* GUM_MULTI_DIM_DECORATOR_H */

