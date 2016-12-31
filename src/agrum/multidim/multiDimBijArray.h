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
/**
 * @file
 * @brief Headers of the MultiDimBijArray class.
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 * @author Lionel TORTI
 */

#ifndef GUM_MULTIDIMBIJARRAY_H
#define GUM_MULTIDIMBIJARRAY_H

#include <initializer_list>

#include <agrum/core/bijection.h>

#include <agrum/multidim/multiDimArray.h>
#include <agrum/multidim/multiDimWithOffset.h>

namespace gum {

  /**
   * @class MultiDimBijArray
   * @headerfile multiDimBijArray.h <agrum/multidim/multiDimBijArray.h>
   * @ingroup multidim_group
   *
   * @brief Decorator of a MultiDimArray, using a bijection over the variables.
   *
   * @tparam GUM_SCALAR The type of scaler stored in this multidimensional
   * table.
   */
  template <typename GUM_SCALAR>
  class MultiDimBijArray : public MultiDimWithOffset<GUM_SCALAR> {
    public:
    using VarBijection =
        Bijection<const DiscreteVariable*, const DiscreteVariable*>;

    // =========================================================================
    /// @name Constructors / Destructors
    // =========================================================================
    /// @{

    /**
     * @brief Class constructor.
     *
     * @param bijection The bijection between variables in array and variable
     * in this.
     * @param array The MultiDimArray decorated by this MultiDimBijArray.
     */
    MultiDimBijArray( const VarBijection&              bijection,
                      const MultiDimArray<GUM_SCALAR>& array );

    /**
     * @brief Class constructor.
     *
     * @param bijection The bijection between variables in array and variable
     * in this.
     * @param array The MultiDimBijArray decorated by this MultiDimBijArray.
     */
    MultiDimBijArray( const VarBijection&                 bijection,
                      const MultiDimBijArray<GUM_SCALAR>& array );

    /**
     * @brief Copy constructor.
     *
     * The newly created matrix contains the same variables and the same values
     * as from, but no instantiation is associated to it.
     *
     * @param from The MultiDimBijArray to copy.
     */
    MultiDimBijArray( const MultiDimBijArray<GUM_SCALAR>& from );

    /**
     * @brief Class destructor.
     */
    virtual ~MultiDimBijArray();

    /// @}
    // ========================================================================
    /// @name Modifiers -- will raise OperationNotAllowed exceptions
    // ========================================================================
    /// @{
    /**
     * @warning This will raise an exception: read only structure.
     * @throw OperationNotAllowed Raised since you can't change a readonly
     * structure.
     */
    MultiDimBijArray<GUM_SCALAR>&
    operator=( const MultiDimBijArray<GUM_SCALAR>& from );

    /**
     * @warning This will raise an exception: read only structure.
     * @throw OperationNotAllowed Raised since you can't change a readonly
     * structure.
     */
    virtual void set( const Instantiation& i, const GUM_SCALAR& value ) const;

    /**
     * This will raise an exception: you can't change the variables in a
     * MultiDimBijArray.
     * @param v The variable not added.
     * @throw OperationNotAllowed You can't add variable in a MultiDimBijArray.
     */
    virtual void add( const DiscreteVariable& v );

    /**
     * This will raise an exception: you can't change the variables in a
     * MultiDimBijArray.
     * @param v The variable not added.
     * @throw OperationNotAllowed You can't add variable in a MultiDimBijArray.
     */
    virtual void erase( const DiscreteVariable& v );

    /**
     * This will raise an exception: you can't change the data
     * @param d the value changed
     * @throw OperationNotAllowed You can't change data.
     */
    virtual void fill( const GUM_SCALAR& d ) const;

    /**
     * This will raise an exception: you can't change the variables in a
     * MultiDimBijArray.
     * @param v The variable not added.
     * @throw OperationNotAllowed You can't add variable in a MultiDimBijArray.
     */
    virtual void populate( const std::vector<GUM_SCALAR>& v ) const;

    /**
     * This will raise an exception: you can't change the variables in a
     * MultiDimBijArray.
     * @param l The variable not added.
     * @throw OperationNotAllowed You can't add variable in a MultiDimBijArray.
     */
    virtual void populate( std::initializer_list<GUM_SCALAR> l ) const;

    /// @}
    // ========================================================================
    /// @name Inherited methods
    // ========================================================================
    /// @{

    virtual const std::string& name() const;

    virtual GUM_SCALAR get( const Instantiation& i ) const;

    virtual Size realSize() const;

    virtual MultiDimBijArray<GUM_SCALAR>* newFactory() const;

    /// @}

    protected:
    virtual GUM_SCALAR& _get( const Instantiation& i ) const;

    virtual void _commitMultipleChanges( void );

    virtual void _swap( const DiscreteVariable* x, const DiscreteVariable* y );

    private:
    /// The true data.
    const MultiDimArray<GUM_SCALAR>& __array;

    /// The class name.
    std::string __name;
  };


  extern template class MultiDimBijArray<float>;
  extern template class MultiDimBijArray<double>;

}  // namespace gum

#include <agrum/multidim/multiDimBijArray_tpl.h>

#endif  // GUM_MULTIDIMBIJARRAY_H
