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
 * @brief Classes used to practice approximation on value
 *
 * @author Pierre-Henri WUILLEMIN & Jean-Christophe Magnan
 *
 */
#ifndef GUM_APPROXIMATION_POLICY_H
#define GUM_APPROXIMATION_POLICY_H
#include <agrum/config.h>

namespace gum {

  /**
   * @class ApproximationPolicy
   * @brief Mother class for all approximation policy classes.
   * @ingroup approximationpolicy_group
   *
   * @warning Doxygen does not like spanning command on multiple line, so we
   * could not configure it with the correct include directive. Use the
   * following code snippet to include this file.
   * @code
   * #include <agrum/core/approximations/approximationPolicy.h>
   * @endcode
   */
  template <typename GUM_SCALAR>
  class ApproximationPolicy {

    public:
    // ===========================================================================
    /// @name Constructors / Destructors
    // ===========================================================================
    /// @{

    /**
     * @brief Default constructor.
     */
    ApproximationPolicy(){};

    /**
     *  @brief Copy constructor.
     */
    ApproximationPolicy( const ApproximationPolicy<GUM_SCALAR>* md ){};

    /// @}
    // ===========================================================================
    /// @name Accessors / Modifiers
    // ===========================================================================
    /// @{

    /**
     * Convert value to his approximation. This method, at least in release
     * mode, should not verify the limits
     * @param value The converted value.
     * @return The value approximation representation.
     */
    virtual GUM_SCALAR fromExact( const GUM_SCALAR& value ) const = 0;

    /**
     * @brief Combine using addition with the given gum::ApproximationPolicy.
     * @param ap The policy to combine with.
     */
    virtual void combineAdd( const ApproximationPolicy<GUM_SCALAR>* ap ) = 0;

    /**
     * @brief Combine using substraction with the given
     * gum::ApproximationPolicy.
     * @param ap The policy to combine with.
     */
    virtual void combineSub( const ApproximationPolicy<GUM_SCALAR>* ap ) = 0;

    /**
     * @brief Combine using multiplication with the given
     * gum::ApproximationPolicy.
     * @param ap The policy to combine with.
     */
    virtual void combineMult( const ApproximationPolicy<GUM_SCALAR>* ap ) = 0;

    /**
     * @brief Combine using division with the given gum::ApproximationPolicy.
     * @param ap The policy to combine with.
     */
    virtual void combineDiv( const ApproximationPolicy<GUM_SCALAR>* ap ) = 0;

    /**
     * @brief Combine using max with the given gum::ApproximationPolicy.
     * @param ap The policy to combine with.
     */
    virtual void combineMax( const ApproximationPolicy<GUM_SCALAR>* ap ) = 0;

    /**
     * @brief Combine using min with the given gum::ApproximationPolicy.
     * @param ap The policy to combine with.
     */
    virtual void combineMin( const ApproximationPolicy<GUM_SCALAR>* ap ) = 0;

    /// @}
  };
}

#endif /* GUM_APPROXIMATION_POLICY_H */
