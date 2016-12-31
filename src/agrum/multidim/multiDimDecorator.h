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
    MultiDimDecorator( MultiDimImplementation<GUM_SCALAR>* aContent = nullptr,
                       GUM_SCALAR empty_value = (GUM_SCALAR)0 );

    /**
     * @brief copy constructor & assignment
     */
    MultiDimDecorator( const MultiDimDecorator<GUM_SCALAR>& from );

    /**
     * @brief copy operator
     */
    MultiDimDecorator<GUM_SCALAR>& operator=( const MultiDimDecorator& from );

    /**
     * @brief Class move constructor & assignment
     */
    MultiDimDecorator( MultiDimDecorator<GUM_SCALAR>&& );

    /**
     * @brief Copy operator.
     */
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

    virtual Idx nbrDim() const final;

    virtual Size domainSize() const final;

    virtual void add( const DiscreteVariable& v ) final;

    virtual void erase( const DiscreteVariable& var ) final;

    virtual const Sequence<const DiscreteVariable*>&
                                    variablesSequence() const final;
    virtual const DiscreteVariable& variable( Idx ) const final;

    virtual Idx pos( const DiscreteVariable& var ) const final;

    virtual bool contains( const DiscreteVariable& var ) const final;

    virtual bool empty() const final;

    /// @}
    // ========================================================================
    /// @name MultiDimAdressable implementation
    // ========================================================================
    /// @{

    virtual bool unregisterSlave( Instantiation& i ) final;

    virtual bool registerSlave( Instantiation& i ) final;

    virtual void changeNotification( Instantiation&                i,
                                     const DiscreteVariable* const var,
                                     const Idx&                    oldval,
                                     const Idx&                    newval ) final;

    virtual void setChangeNotification( Instantiation& i ) final;

    virtual void setFirstNotification( Instantiation& i ) final;

    virtual void setLastNotification( Instantiation& i ) final;

    virtual void setIncNotification( Instantiation& i ) final;

    virtual void setDecNotification( Instantiation& i ) final;

    virtual void notifyChange() const final;

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
    virtual void set( const Instantiation& i,
                      const GUM_SCALAR&    value ) const final;

    /**
     * @brief Default implementation of MultiDimContainer::get().
     *
     * Calls _get as a l-value.
     */
    virtual GUM_SCALAR get( const Instantiation& i ) const final;

    virtual void fill( const GUM_SCALAR& d ) const final;

    /**
     * @brief Automatically fills this MultiDimContainer with the values in
     * v.
     *
     * The order used to fill this MultiDimContainer is the same as with an
     * instantiation over it.
     * @code
     * Size cpt = 0;
     * Instantiation i( *this );
     * for (i.setFirst(); !i.end(); ++i, ++cpt) {
     *   set(i, v[cpt]);
     * }
     * @endcode
     *
     * @param v Vector of values.
     * @throw SizeError Raised if v size's does not matches this
     * MultiDimContainer domain size.
     */
    virtual void populate( const std::vector<GUM_SCALAR>& v ) const final;

    /**
     * @brief Automatically fills this MultiDimContainer with the values in
     * l.
     *
     * The order used to fill this MultiDimContainer is the same as with an
     * instantiation over it.
     * @code
     * Size cpt = 0;
     * Instantiation i( *this );
     * for (i.setFirst(); !i.end(); ++i, ++cpt) {
     *   set(i, v[cpt]);
     * }
     * @endcode
     *
     * @param l contains the data.
     * @throw SizeError Raised if l size's does not matches this
     * MultiDimContainer domain size.
     */
    virtual void populate( std::initializer_list<GUM_SCALAR> l ) const final;

    /**
     * @brief Apply a function on every element of the container
     * @param f the function to apply
     */
    virtual void apply( std::function<GUM_SCALAR( GUM_SCALAR )> f ) const final;

    /**
     * @brief compute lfold for this container
     * @param f the function to apply
     * @param base the initial value
     */
    virtual GUM_SCALAR
    reduce( std::function<GUM_SCALAR( GUM_SCALAR, GUM_SCALAR )> f,
            GUM_SCALAR base ) const final;

    virtual MultiDimDecorator<GUM_SCALAR>* newFactory() const = 0;

    virtual void beginMultipleChanges( void ) final;

    virtual void endMultipleChanges( void ) final;

    virtual void endMultipleChanges( const GUM_SCALAR& ) final;

    virtual const std::string toString( const Instantiation* i ) const;

    virtual const std::string toString() const;

    /// @}
    // =========================================================================
    /// @name Various methods.
    // =========================================================================
    /// @{


    /**
     * @brief Returns the implementation for this object (may be *this).
     */
    virtual const MultiDimImplementation<GUM_SCALAR>* content() const final;

    /**
     * @brief Returns the implementation for this object (may be *this).
     */
    virtual MultiDimImplementation<GUM_SCALAR>* content() final;

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
    GUM_SCALAR& _get( const Instantiation& i ) const final;

    /**
     * value of the MultiDimDecorator if no dimension.
     *
     */
    mutable GUM_SCALAR _empty_value;
  };

} /* namespace gum */

#include <agrum/multidim/multiDimDecorator_tpl.h>

#endif /* GUM_MULTI_DIM_DECORATOR_H */
