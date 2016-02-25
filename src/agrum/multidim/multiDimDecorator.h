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
 * @brief Headers for MultiDimDecorator.
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 */
#ifndef GUM_MULTI_DIM_DECORATOR_H
#define GUM_MULTI_DIM_DECORATOR_H

#include <agrum/config.h>

#include <agrum/multidim/multiDimArray.h>

namespace gum {

  // ===========================================================================
  // ===                         MULTIDIM DECORATOR                          ===
  // ===========================================================================

  /**
   * @class MultiDimDecorator
   * @headerfile multiDimDecorator.h <agrum/multidim/multiDimDecorator.h>
   * @ingroup multidim_group
   *
   * @brief Decorator design pattern in order to separate implementations from
   * multidimensional matrix concepts.
   *
   * A MultiDimDecorator is a virtual class for all encapsulation of
   * MultiDimImplementation, for example probability, utility, etc.  It
   * implements a decorator design pattern in order to have a array, tree,
   * sparse or matrix (...) implementation of MultiDimImplementation.
   *
   * @tparam GUM_SCALAR The type of the scalar stored in this multidimensional
   * matrix.
   */
  template <typename GUM_SCALAR>
  class MultiDimDecorator : public MultiDimContainer<GUM_SCALAR> {
    public:
    // =========================================================================
    /// @name Constructors / Destructors
    // =========================================================================
    /// @{

    /**
     * @brief Class constructor.
     * @param aContent The implementation used by this MultiDimDecorator.
     */
    MultiDimDecorator( MultiDimImplementation<GUM_SCALAR>* aContent = nullptr );

    /**
     * @brief copy constructor & assignment
     */
    MultiDimDecorator( const MultiDimDecorator<GUM_SCALAR>& from );
    MultiDimDecorator<GUM_SCALAR>& operator=( const MultiDimDecorator& from );

    /**
     * @brief Copy constructor.
     *
     * The newly created Potential share the variables and the values
     * from src, but no instantiation is associated to it. It allows to force
     * the chosen implementation and to copy the data from src.
     *
     * @param aContent The implementation to use in this Potential.
     * @param src The MultiDimContainer to copy.
     */
    /*MultiDimDecorator( MultiDimImplementation<GUM_SCALAR>* aContent,
                       const MultiDimContainer<GUM_SCALAR>& src );*/


    /**
     * @brief Class move constructor & assignment
     */
    MultiDimDecorator( MultiDimDecorator<GUM_SCALAR>&& );
    MultiDimDecorator<GUM_SCALAR>& operator=( MultiDimDecorator&& from );

    /**
     * @brief Class destructor.
     */
    ~MultiDimDecorator();

    /// @}
    // =========================================================================
    /// @name MultiDimInterface implementation
    // =========================================================================
    /// @{

    virtual Idx nbrDim() const /* final */;

    virtual Size domainSize() const;

    virtual void add( const DiscreteVariable& v );

    virtual void erase( const DiscreteVariable& var );

    virtual const Sequence<const DiscreteVariable*>& variablesSequence() const
        /* final */;

    virtual const DiscreteVariable& variable( Idx ) const;

    virtual Idx pos( const DiscreteVariable& var ) const;

    virtual bool contains( const DiscreteVariable& var ) const;

    virtual bool empty() const;

    /// @}
    // ========================================================================
    /// @name MultiDimAdressable implementation
    // ========================================================================
    /// @{

    virtual bool unregisterSlave( Instantiation& i );

    virtual bool registerSlave( Instantiation& i );

    virtual void changeNotification( Instantiation& i,
                                     const DiscreteVariable* const var,
                                     const Idx& oldval,
                                     const Idx& newval );

    virtual void setChangeNotification( Instantiation& i );

    virtual void setFirstNotification( Instantiation& i );

    virtual void setLastNotification( Instantiation& i );

    virtual void setIncNotification( Instantiation& i );

    virtual void setDecNotification( Instantiation& i );

    virtual void notifyChange() const /* final */;

    virtual MultiDimImplementation<GUM_SCALAR>& getMasterRef( void );

    virtual const MultiDimImplementation<GUM_SCALAR>&
    getMasterRef( void ) const;

    /// @}
    // =========================================================================
    /// @name MultiDimContainer implementation
    // =========================================================================
    /// @{

    /**
     * @brief Default implementation of MultiDimContainer::set().
     *
     * Calls _get as a r-value.
     */
    virtual void set( const Instantiation& i, const GUM_SCALAR& value ) const;

    /**
     * @brief Default implementation of MultiDimContainer::get().
     *
     * Calls _get as a l-value.
     */
    virtual GUM_SCALAR get( const Instantiation& i ) const;

    virtual void fill( const GUM_SCALAR& d ) const;

    virtual MultiDimDecorator<GUM_SCALAR>* newFactory() const = 0;

    virtual void beginMultipleChanges( void );

    virtual void endMultipleChanges( void );

    virtual void endMultipleChanges( const GUM_SCALAR& );

    virtual const std::string toString( const Instantiation* i ) const;

    virtual const std::string toString() const;

    /// @}
    // =========================================================================
    /// @name Various methods.
    // =========================================================================
    /// @{

    /**
     * @brief Perform an homothety on a multiDim container.
     * @param alpha The ratio.
     * @param mul The chosen mult operation.
     */
    virtual void homothetic( GUM_SCALAR alpha,
                             GUM_SCALAR ( *mul )( const GUM_SCALAR,
                                                  const GUM_SCALAR ) );

    /**
     * @brief Iterate add on each element of a multiDim container.
     * @param add The chosen folded operation.
     * @return Returns the sum of values contains in the multiDim.
     */
    virtual GUM_SCALAR fold( GUM_SCALAR ( *add )( const GUM_SCALAR,
                                                  const GUM_SCALAR ) ) const;

    /**
     * @brief Returns the implementation used by this MultiDimDecorator.
     * @return Returns the implementation used by this MultiDimDecorator.
     */
    const MultiDimImplementation<GUM_SCALAR>* content() const;

    /**
     * @brief Returns the implementation used by this MultiDimDecorator.
     * @return Returns the implementation used by this MultiDimDecorator.
     */
    MultiDimImplementation<GUM_SCALAR>* content();

    /// @}
    protected:
    /**
     * protecte method to swap the implementation behind the Potential
     * @warning unsafe method for slave Instantiations !
     */
    void _swapContent( MultiDimImplementation<GUM_SCALAR>* aContent ) const;

    /**
     * The true container.
     */
    mutable MultiDimImplementation<GUM_SCALAR>* _content;

    /**
     * Return a data, given a Insantiation - final method.
     * @param i The instantiation.
     * @throw NullElement
     * @throw NotFound
     */
    GUM_SCALAR& _get( const Instantiation& i ) const;
  };

} /* namespace gum */

#include <agrum/multidim/multiDimDecorator.tcc>

#endif /* GUM_MULTI_DIM_DECORATOR_H */
