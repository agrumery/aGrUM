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
 * @brief Headers of the IFMDPFactory interface-like class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_ABSTRACT_FMDP_FACTORY_H
#define GUM_ABSTRACT_FMDP_FACTORY_H

#include <string>
#include <vector>

#include <agrum/graphs/graphElements.h>
#include <agrum/variables/labelizedVariable.h>

#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>

namespace gum {

  /**
   * @brief The enumeration of states in which the factory can be in.
   *
   * Every documentation section's name indicates from which state you can
   * call it's methods, and in which state it places the factory.
   *
   * There is an exception for the delegated CPT definition methods which do
   * not changes the state of the factory.
   */
  enum class FMDPfactory_state : char {
    NONE,
    PROPERTY,
    VARIABLE,
    ACTION,
    TRANSITION,
    COST,
    REWARD,
    DISCOUNT
  };

  /**
   * @class FMDPFactory FMDPFactory.h <agrum/FMDP/FMDPFactory.h>
   * @brief A factory class to ease Factored Markov Decision Process construction.
   * @ingroup fmdp_group
   * A FMDPFactory will never create a Factored Markov Decision Process and works on
   *only one
   * Factored Markov Decision Process.
   *
   * The only exception of this behaviour is when you create a copy of the factory,
   * it will create a copy of its FMDP. This is useful when you want to create
   * two FMDP sharing a common base.
   * However be very careful because the copy will not delete its FMDP.
   *
   * Each method will raise an OperationNotAllowed if you call it when the factory
   * is not in a valid state for that call. The error message is "Illegal state.".
   *
   */

  class AbstractFMDPFactory {

    public:
    // ==========================================================================
    /// @name Constructor & destructor.
    // ==========================================================================
    /// @{

    //~ /**
    //~ * @brief Copy constructor.
    //~ * The copy will have an exact copy of the constructed Factored markov
    // Decision Process in source.
    //~ * @warning You can only copy a factory if its current state is NONE or
    //~ *          NETWORK.
    //~ * @throw OperationNotAllowed Raised if the state of source is not NONE or
    //~ *                            NETWORK.
    //~ */
    //~ FMDPFactory ( const FMDPFactory<GUM_SCALAR>& source );

    /**
     * @brief Destructor.
     *
     * To prevent strange behaviour you should always destroy a FMDPFactory
     * when it's state equals NONE.
     *
     * @throw FatalError Raised if the state of the factory prevents it to die
     *                   peacefully.
     */
    virtual ~AbstractFMDPFactory(){};

    /// @}
    // ==========================================================================
    /// @name Getter and setters.
    // ==========================================================================
    /// @{

    /// Returns the current state of the factory.
    virtual FMDPfactory_state state() const = 0;

    /// Returns a constant reference on a variable given it's name.
    /// @throw NotFound Raised if no variable matches the name.
    virtual const DiscreteVariable *variable(const std::string &name) const = 0;

    /// @}
    // ==========================================================================
    /// @name Network declaration methods (NONE <-> PROPERTY)
    // ==========================================================================
    /// @{

    /// Tells the factory that we're in a property declaration.
    virtual void startPropertyDeclaration() = 0;

    /// Tells the factory to add a property to the current fmdp.
    virtual void addProperty(const std::string &propName,
                             const std::string &propValue) = 0;

    /// Tells the factory that we're out of a property declaration.
    virtual void endPropertyDeclaration() = 0;

    /// @}
    // ==========================================================================
    /// @name Variable declaration methods (NONE -> VARIABLE)
    // ==========================================================================
    /// @{

    /// Tells the factory that we're in a variable declaration.
    virtual void startVariableDeclaration() = 0;

    /// Tells the factory the current variable's name.
    /// @throw DuplicateElement Raised if a variable with the same name already
    ///                         exist.
    virtual void variableName(const std::string &name) = 0;

    /// Tells the factory the current variable's description.
    virtual void variableDescription(const std::string &desc) = 0;

    /// Adds a modality to the current variable.
    virtual void addModality(const std::string &name) = 0;

    /// Tells the factory that we're out of a variable declaration.
    /// @throw UndefinedElement Raised if the variable isn't defined (or not
    ///                         enough defined).
    virtual void endVariableDeclaration() = 0;

    /// @}
    // ==========================================================================
    /// @name Action declaration methods (NONE -> ACTION)
    // ==========================================================================
    /// @{

    /// Tells the factory that we're in an action declaration.
    virtual void startActionDeclaration() = 0;

    /// Tells the factory to add an action to the current fmdp.
    virtual void addAction(const std::string &action) = 0;

    /// Tells the factory that we're out of an action declaration.
    virtual void endActionDeclaration() = 0;

    /// @}
    // ==========================================================================
    /// @name Transition declaration methods (NONE -> TRANSITION <- ACTION)
    // ==========================================================================
    /// @{

    /// Tells the factory that we're in a transition declaration.
    virtual void startTransitionDeclaration() = 0;

    /// Tells the factory to add a transition table to the current fmdp.
    virtual void addTransition(const std::string &var,
                               const MultiDimAdressable *transition) = 0;

    /// Tells the factory to add a transition table to the current fmdp.
    /// This transition table will be extracted from incorporated
    /// multiDimDecisionDiagram.
    virtual void addTransition(const std::string &var) = 0;

    /// Tells the factory that we're out of a transition declaration.
    virtual void endTransitionDeclaration() = 0;

    /// @}
    // ==========================================================================
    /// @name Cost declaration methods (NONE -> COST <- ACTION)
    // ==========================================================================
    /// @{

    /// Tells the factory that we're in a cost declaration.
    virtual void startCostDeclaration() = 0;

    /// Tells the factory to add a cost table to the current fmdp.
    virtual void addCost(const MultiDimAdressable *cost) = 0;

    /// Tells the factory to add current decision diagram it has as a cost table.
    virtual void addCost() = 0;

    /// Tells the factory that we're out of a cost declaration.
    virtual void endCostDeclaration() = 0;

    /// @}
    // ==========================================================================
    /// @name Reward declaration methods (NONE -> REWARD <- ACTION)
    // ==========================================================================
    /// @{

    /// Tells the factory that we're in a cost declaration.
    virtual void startRewardDeclaration() = 0;

    /// Tells the factory that we're in a reward declaration mode where the global
    /// reward diagram is an operation between
    /// simplier decision diagram..
    virtual void setOperationModeOn(std::string operationType) = 0;

    /// Tells the factory to add a reward table to the current fmdp.
    virtual void addReward(const MultiDimAdressable *reward) = 0;

    /// Tells the factory to add current decision diagram it has as a reward table.
    virtual void addReward() = 0;

    /// Tells the factory that we're out of a cost declaration.
    virtual void endRewardDeclaration() = 0;

    /// @}
    // ==========================================================================
    /// @name Discount declaration methods (NONE -> DISCOUNT)
    // ==========================================================================
    /// @{

    /// Tells the factory that we're in a cost declaration.
    virtual void startDiscountDeclaration() = 0;

    /// Tells the factory to add a cost table to the current fmdp.
    virtual void addDiscount(float discount) = 0;

    /// Tells the factory that we're out of a cost declaration.
    virtual void endDiscountDeclaration() = 0;

    /// @}
    // ==========================================================================
    /// @name DecisionDiagram Creation specific methods
    /// @brief methods used by internal decisiondiagramfactory to build up
    /// a multidimdecisiondiagram
    // ==========================================================================
    /// @{

    /// Insert in diagram a non terminal node
    virtual NodeId addNonTerminalNode(std::string name_of_var) = 0;

    /// Insert in diagram a terminal node
    virtual NodeId addTerminalNode(float value) = 0;

    /// Insert in diagram an arc
    virtual void insertArc(NodeId from, NodeId to, Idx modality) = 0;

    /// @}

    /**
     * @name verbosity control
     * @{
     */
    void setVerbose() { __verbose = true; };

    void resetVerbose() { __verbose = false; };

    bool isVerbose() { return __verbose; };
    /// @}

    private:
    bool __verbose;
  };
} /* namespace gum */

#endif // GUM_ABSTRACT_FMDP_FACTORY_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
