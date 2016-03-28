//@beforeMerge working on setEvidence in pyAgrum

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
    Potential( MultiDimImplementation<GUM_SCALAR>* aContent );

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
    Potential( MultiDimImplementation<GUM_SCALAR>* aContent,
               const MultiDimContainer<GUM_SCALAR>& src );
    /**
     * @brief Copy constructor & assignment
     */
    Potential( const Potential<GUM_SCALAR>& src );
    Potential<GUM_SCALAR>& operator=( const Potential<GUM_SCALAR>& src );

    /**
     * move constructor & assignement
     **/
    Potential( Potential<GUM_SCALAR>&& from );
    Potential<GUM_SCALAR>& operator=( Potential<GUM_SCALAR>&& src );


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

    /**
     * Projection using sum as operation (and implementation-optimized
     * operations)
     * @param del_vars is the set of vars to eliminate
     * @param kept_vars is the set of vars to keep
     */
    Potential<GUM_SCALAR>
    margSumOut( const Set<const DiscreteVariable*>& del_vars ) const;
    Potential<GUM_SCALAR>
    margSumIn( const Set<const DiscreteVariable*>& kept_vars ) const;

    /**
     * Projection using multiplication as operation (and
     * implementation-optimized operations)
     * @param del_vars is the set of vars to eliminate
     * @param kept_vars is the set of vars to keep
     */
    Potential<GUM_SCALAR>
    margProdOut( const Set<const DiscreteVariable*>& del_vars ) const;
    Potential<GUM_SCALAR>
    margProdIn( const Set<const DiscreteVariable*>& kept_vars ) const;

    /**
     * Projection using min as operation (and implementation-optimized
     * operations)
     * @param del_vars is the set of vars to eliminate
     * @param kept_vars is the set of vars to keep
     */
    Potential<GUM_SCALAR>
    margMinOut( const Set<const DiscreteVariable*>& del_vars ) const;
    Potential<GUM_SCALAR>
    margMinIn( const Set<const DiscreteVariable*>& kept_vars ) const;

    /**
     * Projection using max as operation (and implementation-optimized
     * operations)
     * @param del_vars is the set of vars to eliminate
     * @param kept_vars is the set of vars to keep
     */
    Potential<GUM_SCALAR>
    margMaxOut( const Set<const DiscreteVariable*>& del_vars ) const;
    Potential<GUM_SCALAR>
    margMaxIn( const Set<const DiscreteVariable*>& kept_vars ) const;


    /// sum of all elements in the Potential
    GUM_SCALAR sum() const;
    /// product of all elements in the Potential
    GUM_SCALAR product() const;
    /// max of all elements in the Potential
    GUM_SCALAR max() const;
    /// min of all elements in the Potential
    GUM_SCALAR min() const;
    /// entropy of the Potential
    GUM_SCALAR entropy() const;

    /**
     * @brief Automatically fills the potential with the values in
     * v.
     *
     * @param v Vector of values.
     * @throw SizeError Raised if v size's does not matches this
     * MultiDimContainer domain size.
     */
    const Potential<GUM_SCALAR>&
    fillWith( const std::vector<GUM_SCALAR>& v ) const;

    /**
     * @brief Automatically fills this MultiDimContainer with the values in
     * l.
     *
     * @param l contains the data.
     * @throw SizeError Raised if l size's does not matches this
     * MultiDimContainer domain size.
     */
    const Potential<GUM_SCALAR>&
    fillWith( std::initializer_list<GUM_SCALAR> list ) const;
    /**
     * @brief Apply abs on every element of the container
     */
    const Potential<GUM_SCALAR>& abs() const;

    /**
     * @brief normalisation of this do nothing if sum is 0
     */
    const Potential<GUM_SCALAR>& normalize() const;

    /**
     * @brief apply $x^2$ on every element of the container
     */
    const Potential<GUM_SCALAR>& sq() const;

    ///@}

    // ========================================================================
    /// @name Potential algebra operators
    // ========================================================================
    ///@{


    /// the function to be used to add two Potentials
    Potential<GUM_SCALAR> operator+( const Potential<GUM_SCALAR>& p2 ) const {
      return Potential<GUM_SCALAR>( *this->content() + *p2.content() );
    }

    /// the function to be used to subtract two Potentials
    Potential<GUM_SCALAR> operator-( const Potential<GUM_SCALAR>& p2 ) const {
      return Potential<GUM_SCALAR>( *this->content() - *p2.content() );
    }

    /// the function to be used to multiply two Potentials
    Potential<GUM_SCALAR> operator*( const Potential<GUM_SCALAR>& p2 ) const {
      return Potential<GUM_SCALAR>( *this->content() * *p2.content() );
    }

    /// the function to be used to divide two Potentials
    Potential<GUM_SCALAR> operator/( const Potential<GUM_SCALAR>& p2 ) const {
      return Potential<GUM_SCALAR>( *this->content() / *p2.content() );
    }

    Potential<GUM_SCALAR>& operator+=( const Potential<GUM_SCALAR>& r ) {
      *this = *this + r;
      return *this;
    }

    Potential<GUM_SCALAR>& operator*=( const Potential<GUM_SCALAR>& r ) {
      *this = *this* r;
      return *this;
    }

    Potential<GUM_SCALAR>& operator-=( const Potential<GUM_SCALAR>& r ) {
      *this = *this - r;
      return *this;
    }

    Potential<GUM_SCALAR>& operator/=( const Potential<GUM_SCALAR>& r ) {
      *this = *this / r;
      return *this;
    }
    ///@}

    protected:
    virtual void _swap( const DiscreteVariable* x, const DiscreteVariable* y );

    Set<const DiscreteVariable*>
    _complementVars( const Set<const DiscreteVariable*>& del_vars ) const;
  };


  extern template class Potential<float>;
  extern template class Potential<double>;


} /* namespace gum */

#include <agrum/multidim/potential.tcc>
#endif /* GUM_POTENTIAL_H */
