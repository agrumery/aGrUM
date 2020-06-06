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

#ifndef GUM_SUM_AGGREGATOR_H
#define GUM_SUM_AGGREGATOR_H

#include <agrum/tools/multidim/aggregators/multiDimAggregator.h>

namespace gum {

    namespace aggregator {
        // =========================================================================
        // ===                     GUM_SUM_AGGREGATOR                            ===
        // =========================================================================

        /**
         * @class Sum
         * @headerfile sum.h <agrum/multidim/aggregators/sum.h>
         * @ingroup multidim_agg_group
         *
         * @brief Sum aggregator
         *
         * @see MultiDimAggregator for more details of implementations
         */
        template < typename GUM_SCALAR >
        class Sum : public MultiDimAggregator< GUM_SCALAR > {
        public:
            Sum();
            Sum(const Sum< GUM_SCALAR >& from);
            virtual ~Sum();

            /**
             * This method creates a clone of this object, without its content
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
        extern template class Sum< double >;
#endif


    }   // namespace aggregator
}   // namespace gum

#include <agrum/tools/multidim/aggregators/sum_tpl.h>

#endif   // GUM_SUM_AGGREGATOR_H