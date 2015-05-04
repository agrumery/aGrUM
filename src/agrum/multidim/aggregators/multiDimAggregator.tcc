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
 * @brief Abstract base class for all multi dimensionnal aggregator
 *
* @author Pierre-Henri WUILLEMIN et Christophe GONZALES <{prenom.nom}_at_lip6.fr>
 */

// to ease parser in IDEs
#include <agrum/multidim/aggregators/multiDimAggregator.h>

namespace gum {
  namespace aggregator {
    /// Default constructor

    template <typename GUM_SCALAR>
    INLINE MultiDimAggregator<GUM_SCALAR>::MultiDimAggregator()
        : MultiDimReadOnly<GUM_SCALAR>() {
      GUM_CONSTRUCTOR(MultiDimAggregator);
    }

    /// Default constructor

    template <typename GUM_SCALAR>
    INLINE MultiDimAggregator<GUM_SCALAR>::MultiDimAggregator(
        const MultiDimAggregator<GUM_SCALAR> &from)
        : MultiDimReadOnly<GUM_SCALAR>(from) {
      GUM_CONS_CPY(MultiDimAggregator);
    }

    /// destructor

    template <typename GUM_SCALAR>
    INLINE MultiDimAggregator<GUM_SCALAR>::~MultiDimAggregator() {
      GUM_DESTRUCTOR(MultiDimAggregator);
    }

    template <typename GUM_SCALAR>
    GUM_SCALAR MultiDimAggregator<GUM_SCALAR>::get(const Instantiation &i) const {
      if (this->nbrDim() < 2) {
        GUM_ERROR(OperationNotAllowed, "Not enough variable for an aggregator");
      }

      const DiscreteVariable &agg = this->variable((Idx)0);

      // is i equal to f(f(f(f...(j_,,neutral_elt))))
      Idx current = _neutralElt();

      bool stop_iteration = false;

      for (Idx j = 1; j < this->nbrDim(); j++) {
        current = _folder(this->variable(j), i.val(this->variable(j)), current,
                          stop_iteration);

        if (stop_iteration)
          break;
      }

      // truncate to fit in aggreegator domain size
      if (current >= agg.domainSize())
        current = agg.domainSize() - 1;

      return (i.val(agg) == current) ? (GUM_SCALAR)1 : (GUM_SCALAR)0;
    }

    template <typename GUM_SCALAR>
    const std::string MultiDimAggregator<GUM_SCALAR>::toString() const {
      std::stringstream s;
      s << MultiDimImplementation<GUM_SCALAR>::variable(0) << "=" << aggregatorName()
        << "(";

      for (Idx i = 1; i < MultiDimImplementation<GUM_SCALAR>::nbrDim(); i++) {
        if (i > 1)
          s << ",";

        s << MultiDimImplementation<GUM_SCALAR>::variable(i);
      }

      s << ")";

      return s.str();
    }

    // returns the name of the implementation
    template <typename GUM_SCALAR>
    const std::string &MultiDimAggregator<GUM_SCALAR>::name() const {
      static const std::string str = "MultiDimAggregator";
      return str;
    }

    // For friendly displaying the content of the variable.

    template <typename GUM_SCALAR>
    INLINE std::ostream &operator<<(std::ostream &s,
                                    const MultiDimAggregator<GUM_SCALAR> &ag) {
      return s << ag.toString();
    }

    template <typename GUM_SCALAR>
    INLINE void MultiDimAggregator<GUM_SCALAR>::_swap(const DiscreteVariable *x,
                                                      const DiscreteVariable *y) {
      MultiDimImplementation<GUM_SCALAR>::_swap(x, y);
    }

    // ==================================================

  } /* namespace aggregator */
} /* namespace gum */
