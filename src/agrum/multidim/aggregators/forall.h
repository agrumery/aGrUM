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
#ifndef GUM_FORALL_AGGREGATOR_H
#define GUM_FORALL_AGGREGATOR_H

#include <agrum/multidim/aggregators/multiDimAggregator.h>

namespace gum {

  namespace aggregator {
    /* ============================================================================ */
    /* ============================================================================ */
    /* ===                     GUM_MAX_AGGREGATOR                     === */
    /* ============================================================================ */
    /* ============================================================================ */
    /** @class Forall
    * @brief forall aggregator
    * @ingroup multidim_group
    *
    * @see MultiDimAggregator for more details of implementations
    *
    * Note that a <tt>Forall(i)</tt> aggregator should have a binary aggregator variable since only 0 and 1 indexes are adressed...
    */
    /* ============================================================================ */

    template<typename T_DATA> class Forall : public MultiDimAggregator<T_DATA> {
      public:
        Forall( Idx value );
        Forall( const Forall<T_DATA>& from );
        virtual ~Forall();

        virtual std::string aggregatorName( void ) const;
      protected:
        virtual Idx _neutralElt( void ) const ;
        virtual Idx _folder( const DiscreteVariable& v,Idx i1,Idx i2,bool& stop_iteration ) const;
      private:
        Idx __value;
    };
  } // aggregator
} //gum

#include <agrum/multidim/aggregators/forall.tcc>

#endif // GUM_FORALL_AGGREGATOR_H
