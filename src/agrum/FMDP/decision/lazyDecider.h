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
 * @brief Headers of the lazy decision maker class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */


// =========================================================================
#ifndef GUM_LAZY_DECIDER_H
#define GUM_LAZY_DECIDER_H
// =========================================================================
#include <agrum/FMDP/SDyna/Strategies/IDecisionStrategy.h>
// =========================================================================

namespace gum {

  /**
   * @class LazyDecider lazyDecider.h <agrum/FMDP/SDyna/lazyDecider.h>
   * @brief Class to make decision randomly
   * @ingroup fmdp_group
   *
   * Does nothing more than the interface for DecisionStrategy does
   *
   */
  class LazyDecider : public IDecisionStrategy {

    // ###################################################################
    /// @name Constructor & destructor.
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /// Constructor
    // ==========================================================================
    LazyDecider() {}

    // ==========================================================================
    /// Destructor
    // ==========================================================================
    ~LazyDecider() {}

    /// @}


    // ###################################################################
    /// @name Incremental methods
    // ###################################################################
    /// @{
    public:
    void checkState( const Instantiation& newState, Idx actionId ) {}
  };
}
#endif  // GUM_LAZY_DECIDER_H
