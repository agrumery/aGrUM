
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
 * @brief Headers of the Learning Strategy interface.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */


// =========================================================================
#ifndef GUM_SDYNA_LEARNING_STRATEGY_H
#define GUM_SDYNA_LEARNING_STRATEGY_H
// =========================================================================
#include <string>
// =========================================================================
#include <agrum/core/types.h>
// =========================================================================
#include <agrum/FMDP/fmdp.h>
#include <agrum/FMDP/learning/datastructure/IVisitableGraphLearner.h>
#include <agrum/FMDP/learning/observation.h>
// =========================================================================
#include <agrum/variables/discreteVariable.h>
// =========================================================================

namespace gum {

  /**
   * @class ILearningStrategy ILearningStrategy.h
   * <agrum/FMDP/SDyna/ILearningStrategy.h>
   * @brief Interface for manipulating FMDP learner
   * @ingroup fmdp_group
   *
   */
  class ILearningStrategy {
    // ###################################################################
    /// @name Constructor & destructor.
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /// Destructor (virtual and empty since it's an interface)
    // ==========================================================================
    virtual ~ILearningStrategy() {}

    /// @}

    // ###################################################################
    /// @name Initialization
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /// Initializes the learner
    // ==========================================================================
    virtual void initialize(FMDP< double >* fmdp) = 0;

    /// @}


    // ###################################################################
    /// @name Incremental methods
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /**
     * Gives to the learner a new transition
     * @param actionId : the action on which the transition was made
     * @param obs : the observed transition
     * @return true if learning this transition implies structural changes
     * (can trigger a new planning)
     */
    // ==========================================================================
    virtual bool addObservation(Idx actionId, const Observation* obs) = 0;


    // ==========================================================================
    /**
     * Starts an update of datastructure in the associated FMDP
     */
    // ==========================================================================
    virtual void updateFMDP() = 0;

    /// @}


    // ###################################################################
    /// @name Miscelleanous methods
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /**
     * @brief learnerSize
     * @return
     */
    // ==========================================================================
    virtual Size size() = 0;

    // ==========================================================================
    /**
     * @brief Required for RMax
     * @return
     */
    // ==========================================================================
    virtual const IVisitableGraphLearner*
       varLearner(Idx actionId, const DiscreteVariable* var) const = 0;

    virtual double rMax() const = 0;
    virtual double modaMax() const = 0;

    /// @}
  };
}   // namespace gum
#endif   // GUM_SDYNA_LEARNING_STRATEGY_H
