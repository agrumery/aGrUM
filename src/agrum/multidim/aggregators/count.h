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
 * @brief count aggregator
 *
* @author Pierre-Henri WUILLEMIN et Christophe GONZALES <{prenom.nom}_at_lip6.fr>
 */
#ifndef GUM_COUNT_AGGREGATOR_H
#define GUM_COUNT_AGGREGATOR_H

#include <agrum/multidim/aggregators/multiDimAggregator.h>

namespace gum {

  namespace aggregator {

    /* ============================================================================
     */
    /* ============================================================================
     */
    /* ===                     GUM_MAX_AGGREGATOR                     === */
    /* ============================================================================
     */
    /* ============================================================================
     */
    /** @class Count
    * @brief count aggregator
    * @ingroup multidim_group
    *
    * @see MultiDimAggregator for more details of implementations
    *
    * Note that a <tt>Count(i)</tt> aggregator with a binary aggregator variable is
    *the \f$ \exists i \f$ aggregator.
    * If the aggregator variable is n-ary variable, <tt>Count(i)</tt> can serve as
    *the \f$ \exists \f$ at least \f$ n\ times\ i \f$ aggregator.
    */
    /* ============================================================================
     */

    template <typename GUM_SCALAR>
    class Count : public MultiDimAggregator<GUM_SCALAR> {
      public:
      Count(Idx value);
      Count(const Count<GUM_SCALAR> &from);
      virtual ~Count();

      virtual std::string aggregatorName(void) const;

      /**
       * This method creates a clone of this object, withouth its content
       * (including variable), you must use this method if you want to ensure
       * that the generated object has the same type than the object containing
       * the called newFactory()
       * For example :
       *   MultiDimArray<double> y;
       *   MultiDimContainer<double>* x = y.newFactory();
       * Then x is a MultiDimArray<double>*
       *
       * @warning you must desallocate by yourself the memory
       * @return an empty clone of this object with the same type
       */
      virtual MultiDimContainer<GUM_SCALAR> *newFactory() const;

      protected:
      virtual Idx _neutralElt(void) const;
      virtual Idx _folder(const DiscreteVariable &v, Idx i1, Idx i2,
                          bool &stop_iteration) const;

      private:
      Idx __value;
    };

    extern template class Count<float>;
    extern template class Count<double>;
  } // aggregator
} // gum

#include <agrum/multidim/aggregators/count.tcc>

#endif // GUM_COUNT_AGGREGATOR_H
