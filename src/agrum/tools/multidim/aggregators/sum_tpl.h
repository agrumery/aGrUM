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
 * @brief sum aggregator
 *
 * @author Gaspard DUCAMP
 */

#include <agrum/tools/multidim/aggregators/sum.h>

namespace gum {

    namespace aggregator {
        template < typename GUM_SCALAR >
        INLINE Sum< GUM_SCALAR >::Sum() : MultiDimAggregator< GUM_SCALAR >() {
            this->_decomposable=true;
            GUM_CONSTRUCTOR(Sum);
        }

        template < typename GUM_SCALAR >
        INLINE Sum< GUM_SCALAR >::Sum(const Sum< GUM_SCALAR >& from) :
                MultiDimAggregator< GUM_SCALAR >(from) {
            GUM_CONS_CPY(Sum);
        }

        template < typename GUM_SCALAR >
        INLINE Sum< GUM_SCALAR >::~Sum() {
            GUM_DESTRUCTOR(Sum);
        }

        template < typename GUM_SCALAR >
        INLINE Idx Sum< GUM_SCALAR >::_neutralElt() const {
            return (Idx)0;
        }

        template < typename GUM_SCALAR >
        INLINE Idx Sum< GUM_SCALAR >::_fold(const DiscreteVariable& v,
                                            Idx                     i1,
                                            Idx                     i2,
                                            bool& stop_iteration) const {
          GUM_SCALAR max_val = this->variable((Idx)0).numerical(this->variable((Idx)0).domainSize()-1);
          if((i1 + i2) > max_val){
              stop_iteration = true;
              return max_val;
          }
          return i1 + i2;
        }

        template < typename GUM_SCALAR >
        INLINE std::string Sum< GUM_SCALAR >::aggregatorName() const {
            return "sum";
        }

        template < typename GUM_SCALAR >
        INLINE MultiDimContainer< GUM_SCALAR >* Sum< GUM_SCALAR >::newFactory() const {
            return new Sum< GUM_SCALAR >;
        }

    }   // namespace aggregator
}   // namespace gum