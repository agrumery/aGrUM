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
 * @brief median aggregator
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 */

#ifndef GUM_AMPLITUDE_AGGREGATOR_H
#define GUM_AMPLITUDE_AGGREGATOR_H

#include <agrum/multidim/aggregators/multiDimAggregator.h>

namespace gum {

  namespace aggregator {
    // =========================================================================
    // ===                     GUM_AMPLITUDE_AGGREGATOR                      ===
    // =========================================================================

    /**
     * @class Amplitude
     * @headerfile amplitude.h <agrum/multidim/aggregators/amplitude.h>
     * @ingroup multidim_agg_group
     *
     * @brief amplitude aggregator
     *
     * @see MultiDimAggregator for more details of implementations
     *
     * Amplitude may be truncated since the amplitude has not the same type as
     * its parents.
     */
    template <typename GUM_SCALAR>
    class Amplitude : public MultiDimAggregator<GUM_SCALAR> {
      public:
      Amplitude();

      Amplitude( const Amplitude<GUM_SCALAR>& from );

      virtual ~Amplitude();

      virtual std::string aggregatorName( void ) const;

      /**
       * This method creates a clone of this object, withouth its content
       * (including variable), you must use this method if you want to ensure
       * that the generated object has the same type than the object containing
       * the called newFactory()
       * For example :
       *
       * @code
       *   MultiDimArray<double> y;
       *   MultiDimContainer<double>* x = y.newFactory();
       * @endcode
       *
       * Then x is a MultiDimArray<double>*
       *
       * @warning you must desallocate by yourself the memory
       * @return an empty clone of this object with the same type
       */
      virtual MultiDimContainer<GUM_SCALAR>* newFactory() const;

      protected:
      virtual Idx _buildValue( const gum::Instantiation& i ) const;

      // fold scheme is not used, these methods are neutralized
      virtual Idx _neutralElt( void ) const { return 0; };
      virtual Idx _fold( const DiscreteVariable& v,
                         Idx                     i1,
                         Idx                     i2,
                         bool&                   stop_iteration ) const;

      private:
      Idx __value;
    };

    extern template class Amplitude<float>;
    extern template class Amplitude<double>;

  }  // aggregator
}  // gum

#include <agrum/multidim/aggregators/amplitude_tpl.h>

#endif  // GUM_AMPLITUDE_AGGREGATOR_H
