/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief A class for sampling w.r.t. Dirichlet distributions.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_DIRICHLET_H
#define GUM_LEARNING_DIRICHLET_H

#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/core/utils_random.h>

namespace gum {

  // =========================================================================
  // ===                          DIRICHLET CLASS                          ===
  // =========================================================================

  /**
   * @class Dirichlet
   * @headerfile Dirichlet.h <agrum/base/core/math/Dirichlet.h>
   * @brief A class for sampling w.r.t. Dirichlet distributions.
   * @ingroup math_group
   */
  class Dirichlet {
    public:
    /// The parameter type.
    using param_type = std::vector< float >;

    /// The type for the samples generated.
    using result_type = std::vector< float >;

    // ==========================================================================
    /// @name Constructors / Destructors
    // ==========================================================================
    /// @{

    /**
     * @brief Default constructor.
     * @param params The distribution parameters.
     * @param seed The distribution seed.
     */
    Dirichlet(const param_type& params);

    /**
     * @brief Copy constructor.
     * @param from The distribution to copy.
     */
    Dirichlet(const Dirichlet& from);

    /**
     * @brief Move constructor.
     * @param from The distribution to move.
     */
    Dirichlet(Dirichlet&& from);

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
    Dirichlet& operator=(const Dirichlet& from);

    /**
     * @brief Move operator.
     * @param from The distribution to move.
     * @return Returns this gum::Dirichlet distribution.
     */
    Dirichlet& operator=(Dirichlet&& from);

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
    result_type operator()(const param_type& p);

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
    void param(const param_type& p);

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
    /// The gamma distribution used to compute the Dirichlet unnormalized
    /// samples.
    std::gamma_distribution< float > _gamma_;

    /// The parameters of the distribution.
    param_type _params_;
  };

} /* namespace gum */

// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/base/core/math/Dirichlet_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_DIRICHLET_H */
