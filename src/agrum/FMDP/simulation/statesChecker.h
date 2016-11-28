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
 * @brief Headers of the States Checker class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

// =========================================================================
#ifndef GUM_STATES_CHECKER_H
#define GUM_STATES_CHECKER_H
// =========================================================================
// =========================================================================
#include <agrum/core/sequence.h>
#include <agrum/multidim/multiDimFunctionGraph.h>
// =========================================================================

namespace gum {


  /**
   * @class StatesChecker statesChecker.h
   * <agrum/FMDP/simulation/statesChecker.h>
   * @brief Provides the necessary to check whether or not states have been
   * already visited.
   * @ingroup fmdp_group
   */
  class StatesChecker {

    public:
    // ==========================================================================
    /// @name Constructor & destructor.
    // ==========================================================================
    /// @{

    /**
     * Default constructor
     */
    StatesChecker();

    /**
     * Default destructor
     */
    ~StatesChecker();

    /// @}

    // ==========================================================================
    /// @name Miscelleanous methods
    // ==========================================================================
    /// @{

    bool checkState( const Instantiation& state ) {
      return __checker->get( state );
    }

    void addState( const Instantiation& );

    Idx nbVisitedStates() { return __nbVisitedStates; }

    void reset( const Instantiation& );

    /// @}

    private:
    void __insertState( const Instantiation&, NodeId, Idx );

    Idx __nbVisitedStates;

    MultiDimFunctionGraph<bool>* __checker;
    NodeId                       __checkerTrueId, __checkerFalseId;

    Set<Instantiation*> __visitedStates;
  };
} /* namespace gum */

#endif  // GUM_STATES_CHECKER_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
