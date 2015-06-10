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
 * @brief a class for sampling w.r.t. Dirichlet distributions
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_DIRICHLET_H
#define GUM_LEARNING_DIRICHLET_H

#include <random>
#include <vector>

#include <agrum/config.h>

namespace gum {

  /* ========================================================================= */
  /* ===                          DIRICHLET CLASS                          === */
  /* ========================================================================= */
  /** @class Dirichlet
   * @brief a class for sampling w.r.t. Dirichlet distributions
   * @ingroup math_group
   */
  class Dirichlet {
    public:
    /// the parameter type
    using param_type = std::vector<float>;

    /// the type for the samples generated
    using result_type = std::vector<float>;

    // ##########################################################################
    /// @name Constructors / Destructors
    // ##########################################################################
    /// @{

    /// default constructor
    Dirichlet(const param_type &params, unsigned int seed = 0);

    /// copy constructor
    Dirichlet(const Dirichlet &from);

    /// move constructor
    Dirichlet(Dirichlet &&from);

    /// destructor
    ~Dirichlet();

    /// @}

    // ##########################################################################
    /// @name Operators
    // ##########################################################################
    /// @{

    /// copy operator
    Dirichlet &operator=(const Dirichlet &from);

    /// move operator
    Dirichlet &operator=(Dirichlet &&from);

    /// returns a sample from the Dirichlet distribution
    result_type operator()();

    /// returns a sample from the Dirichlet distribution
    /** @param parm An object representing the distribution's parameters,
     * obtained by a call to member function param. */
    result_type operator()(const param_type &parm);

    /// returns a sample from the Dirichlet distribution
    /** @param generator A uniform random number generator object, used as the
     * source of randomness. URNG shall be a uniform random number generator
     * type, such as one of the standard generator classes.
     * @param parm An object representing the distribution's parameters,
     * obtained by a call to member function param. */
    template <class URNG>
    result_type operator()(URNG &generator, const param_type &parm);

    /// @}

    // ##########################################################################
    /// @name Accessors / Modifiers
    // ##########################################################################
    /// @{

    /// returns the parameters of the distribution
    const param_type &param() const noexcept;

    /// sets the parameters of the distribution
    /** @param parm An object representing the distribution's parameters,
     * obtained by a call to member function param. */
    void param(const param_type &parm);

    /** @brief Returns the greatest lower bound of the range of values returned
     * by operator() */
    float min() const noexcept;

    /** @brief Returns the lowest higher bound of the range of values returned
     * by operator() */
    float max() const noexcept;

    /// @}

    private:
    /// the random engine used by the unform random distribution
    std::default_random_engine __generator;

    /// the gamma distribution used to compute the Dirichlet unnormalized samples
    std::gamma_distribution<float> __gamma;

    /// the parameters of the distribution
    param_type __params;
  };

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/core/math/Dirichlet.inl>
#endif /* GUM_NO_INLINE */

/// always include templates
#include <agrum/core/math/Dirichlet.tcc>

#endif /* GUM_LEARNING_DIRICHLET_H */
