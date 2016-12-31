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
 * @brief A class for sampling w.r.t. Dirichlet distributions.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_DIRICHLET_H
#define GUM_LEARNING_DIRICHLET_H

#include <random>
#include <vector>

#include <agrum/config.h>
#include <agrum/core/utils_random.h>

namespace gum {

  // =========================================================================
  // ===                          DIRICHLET CLASS                          ===
  // =========================================================================

  /**
   * @class Dirichlet Dirichlet.h <agrum/core/math/Dirichlet.h>
   * @brief A class for sampling w.r.t. Dirichlet distributions.
   * @ingroup math_group
   */
  class Dirichlet {
    public:
    /// The parameter type.
    using param_type = std::vector<float>;

    /// The type for the samples generated.
    using result_type = std::vector<float>;

    // ==========================================================================
    /// @name Constructors / Destructors
    // ==========================================================================
    /// @{

    /**
     * @brief Default constructor.
     * @param params The distribution parameters.
     * @param seed The distribution seed.
     */
    Dirichlet( const param_type& params, unsigned int seed = GUM_RANDOMSEED );

    /**
     * @brief Copy constructor.
     * @param from The distribution to copy.
     */
    Dirichlet( const Dirichlet& from );

    /**
     * @brief Move constructor.
     * @param from The distribution to move.
     */
    Dirichlet( Dirichlet&& from );

    /**
     * @brief Class destructor.
     */
    ~Dirichlet();

    /// @}
    // ==========================================================================
    /// @name Operators
    // ==========================================================================
    /// @{

    /**
     * @brief Copy operator.
     * @param from The distribution to copy.
     * @return Returns this gum::Dirichlet distribution.
     */
    Dirichlet& operator=( const Dirichlet& from );

    /**
     * @brief Move operator.
     * @param from The distribution to move.
     * @return Returns this gum::Dirichlet distribution.
     */
    Dirichlet& operator=( Dirichlet&& from );

    /**
     * @brief Returns a sample from the Dirichlet distribution.
     * @return Returns a sample from the Dirichlet distribution.
     */
    result_type operator()();

    /**
     * @brief Returns a sample from the Dirichlet distribution.
     * @param p An object representing the distribution's parameters,
     * obtained by a call to gum::Dirichlet::param(const param_type&).
     */
    result_type operator()( const param_type& p );

    /**
     * @brief Returns a sample from the Dirichlet distribution.
     *
     * @param generator A uniform random number generator object, used as the
     * source of randomness. URNG shall be a uniform random number generator
     * type, such as one of the standard generator classes.
     * @param p An object representing the distribution's parameters,
     * obtained by a call to gum::Dirichlet::param(const param_type&).
     */
    template <class URNG>
    result_type operator()( URNG& generator, const param_type& p );

    /// @}
    // ==========================================================================
    /// @name Accessors / Modifiers
    // ==========================================================================
    /// @{

    /**
     * @brief Returns the parameters of the distribution.
     * @return Returns the parameters of the distribution.
     */
    const param_type& param() const noexcept;

    /**
     * @brief Sets the parameters of the distribution.
     * @param p An object representing the distribution's parameters, obtained
     * by a call to member function param.
     */
    void param( const param_type& p );

    /**
     * @brief Returns the greatest lower bound of the range of values returned
     * by gum::Dirichlet::operator()().
     * @return Returns the greatest lower bound of the range of values returned
     * by gum::Dirichlet::operator()().
     */
    float min() const noexcept;

    /**
     * @brief Returns the lowest higher bound of the range of values returned
     * by gum::Dirichlet::operator()().
     * @return Returns the lowest higher bound of the range of values returned
     * by gum::Dirichlet::operator()().
     */
    float max() const noexcept;

    /// @}

    private:
    /// The random engine used by the unform random distribution.
    std::default_random_engine __generator;

    /// The gamma distribution used to compute the Dirichlet unnormalized
    /// samples.
    std::gamma_distribution<float> __gamma;

    /// The parameters of the distribution.
    param_type __params;
  };

} /* namespace gum */

// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/core/math/Dirichlet_inl.h>
#endif /* GUM_NO_INLINE */

// always include templates
#include <agrum/core/math/Dirichlet_tpl.h>

#endif /* GUM_LEARNING_DIRICHLET_H */
