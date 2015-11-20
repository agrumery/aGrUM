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
 * @brief Inlined implementation of gum::ExactPolicy.
 *
 * @author Pierre-Henri WUILLEMIN & Jean-Christophe Magnan
 *
 */

// To help IDE Parsers
#include <agrum/core/approximations/exactPolicy.h>

namespace gum {

  // Default constructor.
  template <typename GUM_SCALAR>
  ExactPolicy<GUM_SCALAR>::ExactPolicy() {}

  // @brief Convert value to approximation representation.
  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR
  ExactPolicy<GUM_SCALAR>::fromExact( const GUM_SCALAR& value ) const {
    return value;
  }

  // @brief Combine using addition with the given gum::ApproximationPolicy.
  template <typename GUM_SCALAR>
  void ExactPolicy<GUM_SCALAR>::combineAdd(
      const ApproximationPolicy<GUM_SCALAR>* ap ) {}

  // @brief Combine using substraction with the given
  // gum::ApproximationPolicy.
  template <typename GUM_SCALAR>
  void ExactPolicy<GUM_SCALAR>::combineSub(
      const ApproximationPolicy<GUM_SCALAR>* ap ) {}

  // @brief Combine using multiplication with the given
  // gum::ApproximationPolicy.
  template <typename GUM_SCALAR>
  void ExactPolicy<GUM_SCALAR>::combineMult(
      const ApproximationPolicy<GUM_SCALAR>* ap ) {}

  // @brief Combine using division with the given gum::ApproximationPolicy.
  template <typename GUM_SCALAR>
  void ExactPolicy<GUM_SCALAR>::combineDiv(
      const ApproximationPolicy<GUM_SCALAR>* ap ) {}

  // @brief Combine using max with the given gum::ApproximationPolicy.
  template <typename GUM_SCALAR>
  void ExactPolicy<GUM_SCALAR>::combineMax(
      const ApproximationPolicy<GUM_SCALAR>* ap ) {}

  // @brief Combine using min with the given gum::ApproximationPolicy.
  template <typename GUM_SCALAR>
  void ExactPolicy<GUM_SCALAR>::combineMin(
      const ApproximationPolicy<GUM_SCALAR>* ap ) {}

}  // namespace gum
