/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
 * @brief Headers of the States Counter class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

// =========================================================================
#ifndef GUM_STATES_COUNTER_H
#define GUM_STATES_COUNTER_H
// =========================================================================
// =========================================================================
#include <agrum/core/sequence.h>
#include <agrum/multidim/multiDimFunctionGraph.h>
// =========================================================================

namespace gum {


  /**
   * @class StatesCounter statesCounter.h <agrum/FMDP/simulation/statesCounter.h>
   * @brief Provides the necessary to check whether or not states have been already visited.
   * @ingroup fmdp_group
   */
  class StatesCounter {

    public:

      // ==========================================================================
      /// @name Constructor & destructor.
      // ==========================================================================
      /// @{

       /**
        * Default constructor
        */
        StatesCounter (  );

       /**
        * Default destructor
        */
        ~StatesCounter ();

      /// @}

      // ==========================================================================
      /// @name Miscelleanous methods
      // ==========================================================================
      /// @{

        void incState( const Instantiation& );

        void reset( const Instantiation& );

        const MultiDimFunctionGraph<Idx>* counter();

      /// @}

  private:

      void __incState(const Instantiation&, NodeId, Idx, Idx );

      MultiDimFunctionGraph<Idx>* __counter;

      Set<Instantiation*> __visitedStates;
  };
} /* namespace gum */

#endif // GUM_STATES_COUNTER_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;

