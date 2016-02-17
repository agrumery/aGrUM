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
 * @brief Header of the Potential class.
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 */
#ifndef GUM_POTENTIAL_H
#define GUM_POTENTIAL_H

#include <vector>

#include <agrum/config.h>
#include <agrum/core/set.h>
#include <agrum/multidim/multiDimDecorator.h>
#include <agrum/multidim/multiDimImplementation.h>

namespace gum {

  // ==========================================================================
  // ===                          POTENTIAL                                 ===
  // ==========================================================================

  /**
   * @class Potential potential.h <agrum/multidim/potential.h>
   * @ingroup multidim_group
   *
   * @brief Class representing a potential.
   *
   * Using the decorator pattern, this representation is independant from the
   * implementation of the multidimensional matrix.
   *
   * @tparam GUM_SCALAR The type of the scalar stored in this multidimensional
   * matrix.
   */
  template <typename GUM_SCALAR>
  class Potential : public MultiDimDecorator<GUM_SCALAR> {

    public:
    // =========================================================================
    /// @name Constructors, Destructors and Copy
    // =========================================================================
    /// @{

    /**
     * @brief Default constructor.
     *
     * Creates an empty null dimensional matrix with a MultiDimArray as
     * its implementation.
     */
    Potential();

    /**
     * @brief Creates an potential around aContent.
     * @param aContent The implementation of this Potential.
     */
    explicit Potential( MultiDimImplementation<GUM_SCALAR>* aContent );

    /**
     * @brief Copy constructor.
     * @param src The Potential to copy.
     */
    explicit Potential( const Potential<GUM_SCALAR>& src );

    /**
     * move constructeur
     **/
    Potential( const Potential<GUM_SCALAR>&& from );

    /**
     * @brief Copy constructor.
     *
     * The newly created Potential share the variables and the values
     * from src, but no instantiation is associated to it.
     *
     * @param aContent The implementation to use in this Potential.
     * @param src The MultiDimContainer to copy.
     */
    explicit Potential( MultiDimImplementation<GUM_SCALAR>* aContent,
                        const MultiDimContainer<GUM_SCALAR>& src );

    /**
     * @brief Copy operator.
     *
     * @param src The Potential to copy.
     */
    Potential<GUM_SCALAR>& operator=( const Potential<GUM_SCALAR>& src );

    /**
     * @brief Destructor.
     */
    ~Potential();

    /// @}
    // ========================================================================
    /// @name MultiDimContainer implementation
    // ========================================================================
    /// @{

    virtual Potential<GUM_SCALAR>* newFactory() const;

    virtual const std::string toString() const;

    /// @}
    // ========================================================================
    /// @name Class operation for Potential instances
    // ========================================================================
    ///@{

    /// normalisation of this do nothing if sum is 0
    Potential<GUM_SCALAR>& normalize() const;

    /// sum of all elements in this
    const GUM_SCALAR sum() const;
    ///@}

    protected:
    virtual void _swap( const DiscreteVariable* x, const DiscreteVariable* y );
  };


  /// the function to be used to add two Potentials
  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR> operator+( const Potential<GUM_SCALAR>& p1,
                                   const Potential<GUM_SCALAR>& p2 ) {
    return std::move( Potential<GUM_SCALAR>( *p1.content() + *p2.content() ) );
  }

  /// the function to be used to subtract two Potentials
  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR> operator-( const Potential<GUM_SCALAR>& p1,
                                   const Potential<GUM_SCALAR>& p2 ) {
    return std::move( Potential<GUM_SCALAR>( *p1.content() - *p2.content() ) );
  }

  /// the function to be used to multiply two Potentials
  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR> operator*( const Potential<GUM_SCALAR>& p1,
                                   const Potential<GUM_SCALAR>& p2 ) {
    return std::move( Potential<GUM_SCALAR>( *p1.content() * *p2.content() ) );
  }

  /// the function to be used to divide two Potentials
  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR> operator/( const Potential<GUM_SCALAR>& p1,
                                   const Potential<GUM_SCALAR>& p2 ) {
    return std::move( Potential<GUM_SCALAR>( *p1.content() / *p2.content() ) );
  }
} /* namespace gum */

#include <agrum/multidim/potential.tcc>
#endif /* GUM_POTENTIAL_H */









