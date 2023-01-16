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
 * @brief Headers of the Learning Strategy interface.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */


// =========================================================================
#ifndef GUM_INTERFACE_VISITABLE_GRAPH_LEARNER_H
#define GUM_INTERFACE_VISITABLE_GRAPH_LEARNER_H
// =========================================================================
// =========================================================================
// =========================================================================
#include <agrum/tools/multidim/implementations/multiDimFunctionGraph.h>
// =========================================================================
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
    virtual bool isTerminal(NodeId ni) const = 0;

    // ==========================================================================
    ///
    // ==========================================================================
    virtual const DiscreteVariable* nodeVar(NodeId ni) const = 0;

    // ==========================================================================
    ///
    // ==========================================================================
    virtual NodeId nodeSon(NodeId ni, Idx modality) const = 0;

    // ==========================================================================
    ///
    // ==========================================================================
    virtual Idx nodeNbObservation(NodeId ni) const = 0;

    virtual void insertSetOfVars(MultiDimFunctionGraph< double >*) const = 0;


    /// @}
  };
}   // namespace gum
#endif   // GUM_INTERFACE_VISITABLE_GRAPH_LEARNER_H
