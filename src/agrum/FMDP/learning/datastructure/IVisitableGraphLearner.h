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
 * @brief Headers of the Learning Strategy interface.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */


// =========================================================================
#ifndef GUM_INTERFACE_VISITABLE_GRAPH_LEARNER_H
#define GUM_INTERFACE_VISITABLE_GRAPH_LEARNER_H
// =========================================================================
#include <agrum/core/sequence.h>
// =========================================================================
#include <agrum/graphs/nodeGraphPart.h>
// =========================================================================
#include <agrum/multidim/multiDimFunctionGraph.h>
// =========================================================================
#include <agrum/variables/discreteVariable.h>
// =========================================================================

namespace gum {

  /**
   * @class IVisitableGraphLearner IVisitableGraphLearner.h
   * <agrum/FMDP/SDyna/IVisitableGraphLearner.h>
   * @brief Interface for manipulating FMDP learner
   * @ingroup fmdp_group
   *
   */
  class IVisitableGraphLearner {

    // ###################################################################
    /// @name Constructor & destructor.
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /// Destructor (virtual and empty since it's an interface)
    // ==========================================================================
    virtual ~IVisitableGraphLearner() {}

    /// @}


    // ###################################################################
    /// @name Visit Methods
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    ///
    // ==========================================================================
    virtual NodeId root() const = 0;

    // ==========================================================================
    ///
    // ==========================================================================
    virtual bool isTerminal( NodeId ni ) const = 0;

    // ==========================================================================
    ///
    // ==========================================================================
    virtual const DiscreteVariable* nodeVar( NodeId ni ) const = 0;

    // ==========================================================================
    ///
    // ==========================================================================
    virtual NodeId nodeSon( NodeId ni, Idx modality ) const = 0;

    // ==========================================================================
    ///
    // ==========================================================================
    virtual Idx nodeNbObservation( NodeId ni ) const = 0;

    virtual void insertSetOfVars( MultiDimFunctionGraph<double>* ) const = 0;


    /// @}
  };
}
#endif  // GUM_INTERFACE_VISITABLE_GRAPH_LEARNER_H
