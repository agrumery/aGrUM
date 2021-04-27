/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Template Implementation of the FMDPFactory class.
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(@AMU)
 */

#include <agrum/FMDP/fmdpFactory.h>

#include <agrum/tools/multidim/implementations/multiDimImplementation.h>
#include <agrum/tools/multidim/potential.h>


// #define FMDP_VERBOSITY(x) {  if (isVerbose()) std::cerr << "[FMDP factory]
// "<< x << std::endl; }


namespace gum {

  /* ****************************************************************************************************
   * **/
  /* ** **/
  /* **                                                   Constructor &
   * Destructor                        **/
  /* ** **/
  /* ****************************************************************************************************
   * **/


  // Default constructor.
  // @param fmdp A pointer over the Factored Markov Decision Process filled by
  // this factory.

  template < typename GUM_SCALAR >
  INLINE FMDPFactory< GUM_SCALAR >::FMDPFactory(FMDP< GUM_SCALAR >* fmdp) :
      fmdp__(fmdp) {
    GUM_CONSTRUCTOR(FMDPFactory);

    states__.push_back(FMDPfactory_state::NONE);
    resetVerbose();
    actionIdcpt__ = 1;
  }


  // Destructor

  template < typename GUM_SCALAR >
  INLINE FMDPFactory< GUM_SCALAR >::~FMDPFactory() {
    GUM_DESTRUCTOR(FMDPFactory);
  }

  /* ****************************************************************************************************
   * **/
  /* ** **/
  /* **                                                        Getter and
   * setters                         **/
  /* ** **/
  /* ****************************************************************************************************
   * **/


  // Returns the IBayesNet created by this factory.

  template < typename GUM_SCALAR >
  INLINE FMDP< GUM_SCALAR >* FMDPFactory< GUM_SCALAR >::fmdp() const {
    if (state() != FMDPfactory_state::NONE)
      GUM_ERROR(OperationNotAllowed,
                "Illegal state to return the factored "
                "markov decision process: it is not yet "
                "finished.");

    return fmdp__;
  }


  // Returns the current state of the factory.

  template < typename GUM_SCALAR >
  INLINE FMDPfactory_state FMDPFactory< GUM_SCALAR >::state() const {
    // This is ok because there is alway at least the state
    // FMDPfactory_state::NONE in the stack.
    return states__.back();
  }


  // Returns a constant reference on a variable given it's name.
  // @throw NotFound Raised if no variable matches the name.

  template < typename GUM_SCALAR >
  INLINE const DiscreteVariable*
               FMDPFactory< GUM_SCALAR >::variable(const std::string& name) const {
    for (const auto& elt: varNameMap__)
      if (elt.first.compare(name) == 0) return elt.second;

    GUM_ERROR(NotFound, name);

    return nullptr;
  }


  /* ****************************************************************************************************
   * **/
  /* ** **/
  /* **   Variable declaration methods (FMDPfactory_state::NONE <->
   * FMDPfactory_state::VARIABLE)          **/
  /* ** **/
  /* ****************************************************************************************************
   * **/


  // Tells the factory that we're in a variable declaration.

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::startVariableDeclaration() {
    if (state() != FMDPfactory_state::NONE)
      illegalStateError__("startVariableDeclaration");
    else {
      states__.push_back(FMDPfactory_state::VARIABLE);
      stringBag__.push_back("name");
      stringBag__.push_back("desc");
    }

    //       VERBOSITY ( "  starting variable" );
  }


  // Tells the factory the current variable's name.

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::variableName(const std::string& name) {
    if (state() != FMDPfactory_state::VARIABLE)
      illegalStateError__("variableName");
    else {
      if (varNameMap__.exists(name))
        GUM_ERROR(DuplicateElement, "Name already used: " + name);

      foo_flag__     = true;
      stringBag__[0] = name;
      //         VERBOSITY ( "  -- variable " << name );
    }
  }


  // Tells the factory the current variable's description.

  template < typename GUM_SCALAR >
  INLINE void
     FMDPFactory< GUM_SCALAR >::variableDescription(const std::string& desc) {
    if (state() != FMDPfactory_state::VARIABLE)
      illegalStateError__("variableDescription");
    else {
      bar_flag__     = true;
      stringBag__[1] = desc;
    }
  }


  // Adds a modality to the current variable.
  // @throw DuplicateElement If the current variable already has a modality
  //                         with the same name.

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::addModality(const std::string& name) {
    if (state() != FMDPfactory_state::VARIABLE)
      illegalStateError__("addModality");
    else {
      checkModalityInBag__(name);
      stringBag__.push_back(name);
    }
  }


  // Check if in stringBag__ there is no other modality with the same name.

  template < typename GUM_SCALAR >
  INLINE void
     FMDPFactory< GUM_SCALAR >::checkModalityInBag__(const std::string& mod) {
    for (size_t i = 2; i < stringBag__.size(); ++i)
      if (mod == stringBag__[i])
        GUM_ERROR(DuplicateElement, "Modality" << mod << " already exists.");
  }


  // Tells the factory that we're out of a variable declaration.

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::endVariableDeclaration() {
    if (state() != FMDPfactory_state::VARIABLE)
      illegalStateError__("endVariableDeclaration");
    else if (foo_flag__ && (stringBag__.size() > 3)) {
      LabelizedVariable* var
         = new LabelizedVariable(stringBag__[0],
                                 (bar_flag__) ? stringBag__[1] : "",
                                 0);

      for (size_t i = 2; i < stringBag__.size(); ++i) {
        var->addLabel(stringBag__[i]);
      }

      fmdp__->addVariable(var);
      varNameMap__.insert(var->name(), var);
      varNameMap__.insert(fmdp__->main2prime(var)->name(),
                          fmdp__->main2prime(var));

      resetParts__();
      states__.pop_back();

      //         VERBOSITY ( "  variable " << var->name() << " OK" );

    } else {
      std::stringstream msg;
      msg << "Not enough modalities (";

      if (stringBag__.size() > 2)
        msg << stringBag__.size() - 2;
      else
        msg << 0;

      msg << ") declared for variable ";

      if (foo_flag__)
        msg << stringBag__[0];
      else
        msg << "unknown";

      resetParts__();
      states__.pop_back();

      GUM_ERROR(OperationNotAllowed, msg.str());
    }
  }


  /* ****************************************************************************************************
   * **/
  /* ** **/
  /* **    Action declaration methods (FMDPfactory_state::NONE <->
   * FMDPfactory_state::ACTION)             **/
  /* ** **/
  /* ****************************************************************************************************
   * **/


  // Tells the factory that we're declaring action

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::startActionDeclaration() {
    if (state() != FMDPfactory_state::NONE)
      illegalStateError__("startActionDeclaration");
    else {
      foo_flag__ = true;
      states__.push_back(FMDPfactory_state::ACTION);
    }

    //       VERBOSITY ( "starting action declaration" );
  }


  // Tells the factory to add an action

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::addAction(const std::string& action) {
    if (state() != FMDPfactory_state::ACTION)
      illegalStateError__("addAction");
    else {
      stringBag__.push_back(action);
      fmdp__->addAction(actionIdcpt__++, action);
    }
  }


  // Tells the factory that we're out of an action declaration.

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::endActionDeclaration() {
    if (state() != FMDPfactory_state::ACTION)
      illegalStateError__("endActionDeclaration");
    else {
      states__.pop_back();
      resetParts__();
    }

    //       VERBOSITY ( "action OK" );
  }


  /* ****************************************************************************************************
   * **/
  /* ** **/
  /* **                                 Transition declaration methods **/
  /* **      (FMDPfactory_state::NONE <-> FMDPfactory_state::TRANSITION <->
   * FMDPfactory_state::ACTION)    **/
  /* ** **/
  /* ****************************************************************************************************
   * **/


  // Tells the factory that we're declaring transition

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::startTransitionDeclaration() {
    if (state() != FMDPfactory_state::NONE && state() != FMDPfactory_state::ACTION)
      illegalStateError__("startTransitionDeclaration");
    else
      states__.push_back(FMDPfactory_state::TRANSITION);

    //       VERBOSITY ( "starting transition declaration" );
    this->initializeFunctionGraph__();
  }


  // Tells the factory to add an action

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::addTransition(
     const std::string&        var,
     const MultiDimAdressable* transition) {
    const MultiDimImplementation< GUM_SCALAR >* t
       = reinterpret_cast< const MultiDimImplementation< GUM_SCALAR >* >(
          transition);

    if (state() != FMDPfactory_state::TRANSITION)
      illegalStateError__("addTransition");
    else if (foo_flag__)
      fmdp__->addTransitionForAction(fmdp__->actionId(stringBag__[0]),
                                     varNameMap__[var],
                                     t);
    else
      fmdp__->addTransition(varNameMap__[var], t);
  }


  // Tells the factory to add a transition table to the current fmdp.
  // This transition table will be extracted from incorporated
  // multiDimFunctionGraph.

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::addTransition(const std::string& var) {
    if (state() != FMDPfactory_state::TRANSITION)
      illegalStateError__("addTransition");
    else {
      this->finalizeFunctionGraph__();

      if (foo_flag__) {
        this->FunctionGraph__->setTableName("ACTION :" + stringBag__[0]
                                            + " - VARIABLE : " + var);
        fmdp__->addTransitionForAction(fmdp__->actionId(stringBag__[0]),
                                       varNameMap__[var],
                                       this->FunctionGraph__);
      } else {
        fmdp__->addTransition(varNameMap__[var], this->FunctionGraph__);
      }
    }
  }


  // Tells the factory that we're out of a transition declaration.

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::endTransitionDeclaration() {
    if (state() != FMDPfactory_state::TRANSITION)
      illegalStateError__("endTransitionDeclaration");
    else
      states__.pop_back();

    //       VERBOSITY ( "transition OK" );
  }


  /* ****************************************************************************************************
   * **/
  /* ** **/
  /* **                                  Cost declaration methods **/
  /* **        (FMDPfactory_state::NONE <-> FMDPfactory_state::COST <->
   * FMDPfactory_state::ACTION)        **/
  /* ** **/
  /* ****************************************************************************************************
   * **/


  // Tells the factory that we're declaring cost

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::startCostDeclaration() {
    if (state() != FMDPfactory_state::NONE && state() != FMDPfactory_state::ACTION)
      illegalStateError__("startTransitionDeclaration");
    else
      states__.push_back(FMDPfactory_state::COST);

    //       VERBOSITY ( "starting Cost declaration" );
    this->initializeFunctionGraph__();
  }


  // Tells the factory to add a cost

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::addCost(const MultiDimAdressable* cost) {
    const MultiDimImplementation< GUM_SCALAR >* c
       = reinterpret_cast< const MultiDimImplementation< GUM_SCALAR >* >(cost);

    if (state() != FMDPfactory_state::COST)
      illegalStateError__("addCost");
    else if (foo_flag__)
      fmdp__->addCostForAction(fmdp__->actionId(stringBag__[0]), c);
    else
      fmdp__->addCost(c);
  }


  // Tells the factory to add a cost
  // This cost table will be extracted from incorporated multiDimFunctionGraph.

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::addCost() {
    if (state() != FMDPfactory_state::COST)
      illegalStateError__("addCost");
    else {
      this->finalizeFunctionGraph__();

      if (foo_flag__)
        fmdp__->addCostForAction(fmdp__->actionId(stringBag__[0]),
                                 this->FunctionGraph__);
      else
        fmdp__->addCost(this->FunctionGraph__);
    }
  }


  // Tells the factory that we're out of a cost declaration.

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::endCostDeclaration() {
    if (state() != FMDPfactory_state::COST)
      illegalStateError__("endCostDeclaration");
    else
      states__.pop_back();

    //       VERBOSITY ( "Cost OK" );
  }


  /* ****************************************************************************************************
   * **/
  /* ** **/
  /* **                                 Reward declaration methods **/
  /* **       (FMDPfactory_state::NONE <-> FMDPfactory_state::REWARD <->
   * FMDPfactory_state::ACTION)       **/
  /* ** **/
  /* ****************************************************************************************************
   * **/


  // Tells the factory that we're declaring reward

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::startRewardDeclaration() {
    if (state() != FMDPfactory_state::NONE && state() != FMDPfactory_state::ACTION)
      illegalStateError__("startRewardDeclaration");
    else
      states__.push_back(FMDPfactory_state::REWARD);

    //       VERBOSITY ( "starting reward declaration" );
    this->initializeFunctionGraph__();
  }


  // Tells the factory that we're in a reward declaration mode where the global
  // reward diagram is an operation between simplier dd

  template < typename GUM_SCALAR >
  INLINE void
     FMDPFactory< GUM_SCALAR >::setOperationModeOn(std::string operationType) {
    foo_flag__ = true;
    std::string ot(operationType);
    stringBag__.push_back(ot);
  }


  // Tells the factory to add a reward

  template < typename GUM_SCALAR >
  INLINE void
     FMDPFactory< GUM_SCALAR >::addReward(const MultiDimAdressable* reward) {
    const MultiDimImplementation< GUM_SCALAR >* r
       = reinterpret_cast< const MultiDimImplementation< GUM_SCALAR >* >(reward);

    if (state() != FMDPfactory_state::REWARD)
      illegalStateError__("addReward");
    else
      fmdp__->addReward(r);
  }


  // Tells the factory to add a reward
  // This reward table will be extracted from incorporated
  // multiDimFunctionGraph.

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::addReward() {
    if (state() != FMDPfactory_state::REWARD)
      illegalStateError__("addReward");
    else {
      this->finalizeFunctionGraph__();
      FunctionGraph__->setTableName("Reward");

      if (foo_flag__)
        ddBag__.push_back(this->FunctionGraph__);
      else
        fmdp__->addReward(this->FunctionGraph__);
    }
  }


  // Tells the factory that we're out of a reward declaration.

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::endRewardDeclaration() {
    if (state() != FMDPfactory_state::REWARD)
      illegalStateError__("endRewardDeclaration");
    else {
      if (foo_flag__) {
        MultiDimImplementation< GUM_SCALAR >* res = nullptr;
        MultiDimImplementation< GUM_SCALAR >* temp;

        for (const auto elt: ddBag__) {
          temp = res;

          switch (stringBag__[0][0]) {
            case '+':
              res = add2MultiDimFunctionGraphs(res, elt);
              break;

            case '-':
              res = subtract2MultiDimFunctionGraphs(res, elt);
              break;

            case '*':
              res = multiply2MultiDimFunctionGraphs(res, elt);
              break;

            case '/':
              res = divide2MultiDimFunctionGraphs(res, elt);
              break;

            default:
              break;
          }

          delete elt;

          if (temp != nullptr) delete temp;
        }
        reinterpret_cast< MultiDimFunctionGraph< GUM_SCALAR >* >(res)
           ->setTableName("Reward");
        fmdp__->addReward(res);
      }

      resetParts__();
      states__.pop_back();
    }
    //       VERBOSITY ( "reward OK" );
  }


  /* ****************************************************************************************************
   * **/
  /* ** **/
  /* **                                Discount declaration methods **/
  /* **    (FMDPfactory_state::NONE <-> FMDPfactory_state::DISCOUNT <->
   * FMDPfactory_state::ACTION)        **/
  /* ** **/
  /* ****************************************************************************************************
   * **/


  // Tells the factory that we're declaring discount
  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::startDiscountDeclaration() {
    if (state() != FMDPfactory_state::NONE)
      illegalStateError__("startDiscountDeclaration");
    else
      states__.push_back(FMDPfactory_state::DISCOUNT);

    //       VERBOSITY ( "starting discount declaration" );
  }


  // Tells the factory to add a discount

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::addDiscount(float discount) {
    if (state() != FMDPfactory_state::DISCOUNT) illegalStateError__("addDiscount");
    //    else
    //      fmdp__->setDiscount ( ( GUM_SCALAR ) discount );
  }


  // Tells the factory that we're out of a discount declaration.

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::endDiscountDeclaration() {
    if (state() != FMDPfactory_state::DISCOUNT)
      illegalStateError__("endDiscountDeclaration");
    else
      states__.pop_back();

    //       VERBOSITY ( "discount OK" );
  }


  /* ****************************************************************************************************
   * **/
  /* ** **/
  /* **                                 Discount declaration methods **/
  /* **       (FMDPfactory_state::NONE <-> FMDPfactory_state::DISCOUNT <->
   * FMDPfactory_state::ACTION)     **/
  /* ** **/
  /* ****************************************************************************************************
   * **/


  // Insert in diagram a non terminal node

  template < typename GUM_SCALAR >
  INLINE NodeId
     FMDPFactory< GUM_SCALAR >::addInternalNode(std::string name_of_var) {
    return FunctionGraph__->manager()->addInternalNode(variable(name_of_var));
  }


  // Insert in diagram a terminal node

  template < typename GUM_SCALAR >
  INLINE NodeId FMDPFactory< GUM_SCALAR >::addTerminalNode(float value) {
    return FunctionGraph__->manager()->addTerminalNode((GUM_SCALAR)value);
  }


  // Insert an arc in diagram
  template < typename GUM_SCALAR >
  INLINE void
     FMDPFactory< GUM_SCALAR >::addArc(NodeId from, NodeId to, Idx modality) {
    FunctionGraph__->manager()->setSon(from, modality, to);
  }


  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::setRoot(NodeId rootId) {
    FunctionGraph__->manager()->setRootNode(rootId);
  }

  /* ****************************************************************************************************
   * **/
  /* ** **/
  /* **                  Various Private Methods **/
  /* ** **/
  /* ****************************************************************************************************
   * **/


  // Raise an OperationNotAllowed with the message "Illegal state."

  template < typename GUM_SCALAR >
  INLINE void
     FMDPFactory< GUM_SCALAR >::illegalStateError__(const std::string& s) {
    std::string msg = "Illegal state call (";
    msg += s;
    msg += ") in state ";

    switch (state()) {
      case FMDPfactory_state::NONE:
        msg += "FMDPfactory_state::NONE";
        break;

      case FMDPfactory_state::VARIABLE:
        msg += "FMDPfactory_state::VARIABLE";
        break;

      case FMDPfactory_state::ACTION:
        msg += "FMDPfactory_state::ACTION";
        break;

      case FMDPfactory_state::TRANSITION:
        msg += "FMDPfactory_state::TRANSITION";
        break;

      case FMDPfactory_state::COST:
        msg += "FMDPfactory_state::COST";
        break;

      case FMDPfactory_state::REWARD:
        msg += "FMDPfactory_state::REWARD";
        break;

      case FMDPfactory_state::DISCOUNT:
        msg += "FMDPfactory_state::DISCOUNT";
        break;

      default:
        msg += "Unknown state";
    }

    GUM_ERROR(OperationNotAllowed, msg);
  }


  // Reset the different parts used to constructed the IBayesNet.

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::resetParts__() {
    foo_flag__ = false;
    bar_flag__ = false;
    stringBag__.clear();
    ddBag__.clear();
  }


  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::initializeFunctionGraph__() {
    this->FunctionGraph__
       = MultiDimFunctionGraph< GUM_SCALAR >::getReducedAndOrderedInstance();
    // Recopie des variables principales dans le graphe de décision
    for (auto varIter = fmdp__->beginVariables();
         varIter != fmdp__->endVariables();
         ++varIter) {
      FunctionGraph__->add(**varIter);
    }

    // Recopie des version primes des variables dans le graphe de décision
    for (auto varIter = fmdp__->beginVariables();
         varIter != fmdp__->endVariables();
         ++varIter) {
      FunctionGraph__->add(*(fmdp__->main2prime(*varIter)));
    }
  }


  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::finalizeFunctionGraph__() {
    this->FunctionGraph__->manager()->reduce();
    this->FunctionGraph__->manager()->clean();
  }

  //~ ==============
  //~ // Check if a variable with the given name exists, if not raise an
  // NotFound
  //~ // exception.
  //~ ==============
  //~ template<typename GUM_SCALAR> INLINE
  //~ void
  //~ FMDPFactory<GUM_SCALAR>::checkVariableName__ ( const std::string& name ) {
  //~ if ( !varNameMap__.exists ( name ) )
  //~ GUM_ERROR ( NotFound, name );
  //~ }


  // Copy operator is illegal, use only copy constructor.

  template < typename GUM_SCALAR >
  INLINE FMDPFactory< GUM_SCALAR >& FMDPFactory< GUM_SCALAR >::operator=(
     const FMDPFactory< GUM_SCALAR >& source) {
    GUM_ERROR(FatalError, "Illegal!");
    // For noisy compilers
    return *this;
  }
} /* namespace gum */
