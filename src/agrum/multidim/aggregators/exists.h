
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 */

#ifndef GUM_EXISTS_AGGREGATOR_H
#define GUM_EXISTS_AGGREGATOR_H

#include <agrum/multidim/aggregators/multiDimAggregator.h>

namespace gum {

  namespace aggregator {
    // =========================================================================
    // ===                     GUM_MAX_AGGREGATOR                            ===
    // =========================================================================
    /**
     * @class Exists
     * @headerfile exists.h <agrum/multidim/aggregator/exists.h>
     * @ingroup multidim_agg_group
     *
     * @brief exists aggregator
     *
     * @see MultiDimAggregator for more details of implementations
     *
     * Note that a <tt>Exists(i)</tt> aggregator should have a binary
     * aggregator variable since only 0 and 1 indexes are adressed...
     */
    template < typename GUM_SCALAR >
    class Exists : public MultiDimAggregator< GUM_SCALAR > {
      public:
      Exists(Idx value);
      Exists(const Exists< GUM_SCALAR >& from);
      virtual ~Exists();

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
      virtual Idx _neutralElt() const;
      virtual Idx _fold(const DiscreteVariable& v,
                        Idx                     i1,
                        Idx                     i2,
                        bool&                   stop_iteration) const;

      private:
      Idx __value;
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class Exists< double >;
#endif


  }   // namespace aggregator
}   // namespace gum

#include <agrum/multidim/aggregators/exists_tpl.h>

#endif   // GUM_EXISTS_AGGREGATOR_H
