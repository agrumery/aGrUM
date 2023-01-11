/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief forall aggregator
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#ifndef GUM_FORALL_AGGREGATOR_H
#define GUM_FORALL_AGGREGATOR_H

#include <agrum/tools/multidim/aggregators/multiDimAggregator.h>

namespace gum {

  namespace aggregator {
    // =========================================================================
    // ===                     GUM_FORALL_AGGREGATOR                         ===
    // =========================================================================

    /**
     * @class Forall
     * @headerfile forall.h <agrum/tools/multidim/aggregators/forall.h>
     * @ingroup multidim_agg_group
     *
     * @brief forall aggregator
     *
     * @see MultiDimAggregator for more details of implementations
     *
     * Note that a <tt>Forall(i)</tt> aggregator should have a binary aggregator
     * variable since only 0 and 1 indexes are adressed...
     */
    template < typename GUM_SCALAR >
    class Forall: public MultiDimAggregator< GUM_SCALAR > {
      public:
      Forall(Idx value);
      Forall(const Forall< GUM_SCALAR >& from);
      virtual ~Forall();

      /**
       * This method creates a clone of this object, withouth its content
       * (including variable), you must use this method if you want to ensure
       * that the generated object has the same type than the object containing
       * the called newFactory()
       *
       * For example :
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
      virtual MultiDimContainer< GUM_SCALAR >* newFactory() const;

      virtual std::string aggregatorName() const;

      protected:
      virtual Idx neutralElt_() const;
      virtual Idx fold_(const DiscreteVariable& v, Idx i1, Idx i2, bool& stop_iteration) const;

      private:
      Idx _value_;
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class Forall< double >;
#endif


  }   // namespace aggregator
}   // namespace gum

#include <agrum/tools/multidim/aggregators/forall_tpl.h>

#endif   // GUM_FORALL_AGGREGATOR_H
