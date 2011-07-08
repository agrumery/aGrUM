
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
#ifndef GUM_EXACT_POLICY_H
#define GUM_EXACT_POLICY_H
//**********************************************************************
#include <agrum/core/inline.h>
#include <agrum/core/types.h>
#include <agrum/core/exceptions.h>

#include <agrum/core/approximationPolicy/approximationPolicy.h>
//**********************************************************************

namespace gum {

  /**
   * @class ExactPolicy approximationPolicy.h <agrum/multidim/approximationPolicy.h>
   * @brief Class implementing exact approximation policy (meaning a value is approximate to itself)
   * @ingroup multidim_group
   */
  template<typename T_DATA>
  class ExactPolicy : public virtual ApproximationPolicy<T_DATA> {
    public:

      // ===========================================================================
      /// @name Constructors / Destructors
      // ===========================================================================
      /// @{

      /**
       * Default constructor.
       */
      ExactPolicy() {};

      /// @}

      // ===========================================================================
      /// @name Accessors/Modifiers
      // ===========================================================================
      /// @{

      /// Convert value to approximation representation
      INLINE T_DATA fromExact( const T_DATA& value ) const {
        return value;
      };

      /// @}
  };
}

#endif /* GUM_EXACT_POLICY_H */
