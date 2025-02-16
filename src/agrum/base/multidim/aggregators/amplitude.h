/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief amplitude aggregator
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#ifndef GUM_AMPLITUDE_AGGREGATOR_H
#define GUM_AMPLITUDE_AGGREGATOR_H

#include <agrum/base/multidim/aggregators/multiDimAggregator.h>

namespace gum {

  namespace aggregator {
    // =========================================================================
    // ===                     GUM_AMPLITUDE_AGGREGATOR                      ===
    // =========================================================================

    /**
     * @class Amplitude
     * @headerfile amplitude.h <agrum/base/multidim/aggregators/amplitude.h>
     * @ingroup multidim_agg_group
     *
     * @brief amplitude aggregator
     *
     * @see MultiDimAggregator for more details of implementations
     *
     * Amplitude may be truncated since the amplitude has not the same type as
     * its parents.
     */
    template < typename GUM_SCALAR >
    class Amplitude: public MultiDimAggregator< GUM_SCALAR > {
      public:
      Amplitude();

      Amplitude(const Amplitude< GUM_SCALAR >& from);

      virtual ~Amplitude();

      virtual std::string aggregatorName() const;

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
      virtual MultiDimContainer< GUM_SCALAR >* newFactory() const;

      protected:
      virtual Idx buildValue_(const gum::Instantiation& i) const;

      // fold scheme is not used, these methods are neutralized
      virtual Idx neutralElt_() const { return 0; };

      virtual Idx fold_(const DiscreteVariable& v, Idx i1, Idx i2, bool& stop_iteration) const;

      private:
      Idx _value_;
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class Amplitude< double >;
#endif

  }   // namespace aggregator
}   // namespace gum

#include <agrum/base/multidim/aggregators/amplitude_tpl.h>

#endif   // GUM_AMPLITUDE_AGGREGATOR_H
