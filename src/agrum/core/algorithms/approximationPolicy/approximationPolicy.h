
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
//**********************************************************************
#include <agrum/config.h>

//**********************************************************************

namespace gum {

  /**
   * @class ApproximationPolicy approximationPolicy.h
   *<agrum/multidim/approximationPolicy.h>
   * @brief Mother class for all approximation policy classes
   * @ingroup multidim_group
   *
   */
  template <typename GUM_SCALAR> class ApproximationPolicy {

    public:
    // ===========================================================================
    /// @name Constructors / Destructors
    // ===========================================================================
    /// @{

    /**
     * Default constructor.
     */
    ApproximationPolicy(){};

    /**
     * copy constructor.
     */
    ApproximationPolicy(const ApproximationPolicy<GUM_SCALAR> *md){};

    /// @}

    // ===========================================================================
    /// @name Accessors/Modifiers
    // ===========================================================================
    /// @{

    /// Convert value to his approximation. This method (at least in release mode,
    /// should not verify the limits
    virtual GUM_SCALAR fromExact(const GUM_SCALAR &value) const = 0;

    virtual void combineAdd(const ApproximationPolicy<GUM_SCALAR> *ap) = 0;

    virtual void combineSub(const ApproximationPolicy<GUM_SCALAR> *ap) = 0;

    virtual void combineMult(const ApproximationPolicy<GUM_SCALAR> *ap) = 0;

    virtual void combineDiv(const ApproximationPolicy<GUM_SCALAR> *ap) = 0;

    virtual void combineMax(const ApproximationPolicy<GUM_SCALAR> *ap) = 0;

    virtual void combineMin(const ApproximationPolicy<GUM_SCALAR> *ap) = 0;

    /// @}
  };
}

#endif /* GUM_APPROXIMATION_POLICY_H */
