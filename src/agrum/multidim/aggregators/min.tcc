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
/** @file
 * @brief min aggregator
 *
* @author Pierre-Henri WUILLEMIN et Christophe GONZALES <{prenom.nom}_at_lip6.fr>
 */

// to ease parser in IDEs
#include <agrum/multidim/aggregators/multiDimAggregator.h>
#include <agrum/multidim/aggregators/min.h>

namespace gum {

  namespace aggregator {

    /// Default constructor

    template <typename GUM_SCALAR>
    INLINE Min<GUM_SCALAR>::Min()
        : MultiDimAggregator<GUM_SCALAR>() {
      GUM_CONSTRUCTOR(Min)
    }

    /// Default constructor

    template <typename GUM_SCALAR>
    INLINE Min<GUM_SCALAR>::Min(const Min<GUM_SCALAR> &from)
        : MultiDimAggregator<GUM_SCALAR>(from) {
      GUM_CONS_CPY(Min);
    }

    /// destructor

    template <typename GUM_SCALAR> INLINE Min<GUM_SCALAR>::~Min() {
      GUM_DESTRUCTOR(Min);
    }

    template <typename GUM_SCALAR> INLINE Idx Min<GUM_SCALAR>::_neutralElt() const {
      return (Idx)100000;
    } // clearly arbitrary choosen

    template <typename GUM_SCALAR>
    INLINE Idx Min<GUM_SCALAR>::_folder(const DiscreteVariable &v, Idx i1, Idx i2,
                                        bool &stop_iteration) const {
      return (i1 < i2) ? i1 : i2;
    }

    template <typename GUM_SCALAR>
    INLINE std::string Min<GUM_SCALAR>::aggregatorName(void) const {
      return "min";
    }

    template <typename GUM_SCALAR>
    INLINE MultiDimContainer<GUM_SCALAR> *Min<GUM_SCALAR>::newFactory() const {
      return new Min<GUM_SCALAR>;
    }

  } // aggregator
} // namespace gum
