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
 * @brief Template Implementation of the FMDPFactory class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
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
  INLINE FMDPFactory< GUM_SCALAR >::FMDPFactory(FMDP< GUM_SCALAR >* fmdp) : _fmdp_(fmdp) {
    GUM_CONSTRUCTOR(FMDPFactory);

    _states_.push_back(FMDPfactory_state::NONE);
    resetVerbose();
    _actionIdcpt_ = 1;
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

    return _fmdp_;
  }


  // Returns the current state of the factory.

  template < typename GUM_SCALAR >
  INLINE FMDPfactory_state FMDPFactory< GUM_SCALAR >::state() const {
    // This is ok because there is alway at least the state
    // FMDPfactory_state::NONE in the stack.
    return _states_.back();
  }


  // Returns a constant reference on a variable given it's name.
  // @throw NotFound Raised if no variable matches the name.

  template < typename GUM_SCALAR >
  INLINE const DiscreteVariable*
     FMDPFactory< GUM_SCALAR >::variable(const std::string& name) const {
    for (const auto& elt: _varNameMap_)
      if (elt.first.compare(name) == 0) return elt.second;

    GUM_ERROR(NotFound, name)

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
    if (state() != FMDPfactory_state::NONE) _illegalStateError_("startVariableDeclaration");
    else {
      _states_.push_back(FMDPfactory_state::VARIABLE);
      _stringBag_.push_back("name");
      _stringBag_.push_back("desc");
    }

    //       VERBOSITY ( "  starting variable" );
  }


  // Tells the factory the current variable's name.

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::variableName(const std::string& name) {
    if (state() != FMDPfactory_state::VARIABLE) _illegalStateError_("variableName");
    else {
      if (_varNameMap_.exists(name)) GUM_ERROR(DuplicateElement, "Name already used: " + name)

      _foo_flag_     = true;
      _stringBag_[0] = name;
      //         VERBOSITY ( "  -- variable " << name );
    }
  }


  // Tells the factory the current variable's description.

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::variableDescription(const std::string& desc) {
    if (state() != FMDPfactory_state::VARIABLE) _illegalStateError_("variableDescription");
    else {
      _bar_flag_     = true;
      _stringBag_[1] = desc;
    }
  }


  // Adds a modality to the current variable.
  // @throw DuplicateElement If the current variable already has a modality
  //                         with the same name.

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::addModality(const std::string& name) {
    if (state() != FMDPfactory_state::VARIABLE) _illegalStateError_("addModality");
    else {
      _checkModalityInBag_(name);
      _stringBag_.push_back(name);
    }
  }


  // Check if in  _stringBag_ there is no other modality with the same name.

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::_checkModalityInBag_(const std::string& mod) {
    for (size_t i = 2; i < _stringBag_.size(); ++i)
      if (mod == _stringBag_[i])
        GUM_ERROR(DuplicateElement, "Modality" << mod << " already exists.")
  }


  // Tells the factory that we're out of a variable declaration.

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::endVariableDeclaration() {
    if (state() != FMDPfactory_state::VARIABLE) _illegalStateError_("endVariableDeclaration");
    else if (_foo_flag_ && (_stringBag_.size() > 3)) {
      LabelizedVariable* var
         = new LabelizedVariable(_stringBag_[0], (_bar_flag_) ? _stringBag_[1] : "", 0);

      for (size_t i = 2; i < _stringBag_.size(); ++i) {
        var->addLabel(_stringBag_[i]);
      }

      _fmdp_->addVariable(var);
      _varNameMap_.insert(var->name(), var);
      _varNameMap_.insert(_fmdp_->main2prime(var)->name(), _fmdp_->main2prime(var));

      _resetParts_();
      _states_.pop_back();

      //         VERBOSITY ( "  variable " << var->name() << " OK" );

    } else {
      std::stringstream msg;
      msg << "Not enough modalities (";

      if (_stringBag_.size() > 2) msg << _stringBag_.size() - 2;
      else msg << 0;

      msg << ") declared for variable ";

      if (_foo_flag_) msg << _stringBag_[0];
      else msg << "unknown";

      _resetParts_();
      _states_.pop_back();

      GUM_ERROR(OperationNotAllowed, msg.str())
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
    if (state() != FMDPfactory_state::NONE) _illegalStateError_("startActionDeclaration");
    else {
      _foo_flag_ = true;
      _states_.push_back(FMDPfactory_state::ACTION);
    }

    //       VERBOSITY ( "starting action declaration" );
  }


  // Tells the factory to add an action

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::addAction(const std::string& action) {
    if (state() != FMDPfactory_state::ACTION) _illegalStateError_("addAction");
    else {
      _stringBag_.push_back(action);
      _fmdp_->addAction(_actionIdcpt_++, action);
    }
  }


  // Tells the factory that we're out of an action declaration.

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::endActionDeclaration() {
    if (state() != FMDPfactory_state::ACTION) _illegalStateError_("endActionDeclaration");
    else {
      _states_.pop_back();
      _resetParts_();
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
      _illegalStateError_("startTransitionDeclaration");
    else _states_.push_back(FMDPfactory_state::TRANSITION);

    //       VERBOSITY ( "starting transition declaration" );
    this->_initializeFunctionGraph_();
  }


  // Tells the factory to add an action

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::addTransition(const std::string&        var,
                                                       const MultiDimAdressable* transition) {
    const MultiDimImplementation< GUM_SCALAR >* t
       = reinterpret_cast< const MultiDimImplementation< GUM_SCALAR >* >(transition);

    if (state() != FMDPfactory_state::TRANSITION) _illegalStateError_("addTransition");
    else if (_foo_flag_)
      _fmdp_->addTransitionForAction(_fmdp_->actionId(_stringBag_[0]), _varNameMap_[var], t);
    else _fmdp_->addTransition(_varNameMap_[var], t);
  }


  // Tells the factory to add a transition table to the current fmdp.
  // This transition table will be extracted from incorporated
  // multiDimFunctionGraph.

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::addTransition(const std::string& var) {
    if (state() != FMDPfactory_state::TRANSITION) _illegalStateError_("addTransition");
    else {
      this->_finalizeFunctionGraph_();

      if (_foo_flag_) {
        this->_FunctionGraph_->setTableName("ACTION :" + _stringBag_[0] + " - VARIABLE : " + var);
        _fmdp_->addTransitionForAction(_fmdp_->actionId(_stringBag_[0]),
                                       _varNameMap_[var],
                                       this->_FunctionGraph_);
      } else {
        _fmdp_->addTransition(_varNameMap_[var], this->_FunctionGraph_);
      }
    }
  }


  // Tells the factory that we're out of a transition declaration.

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::endTransitionDeclaration() {
    if (state() != FMDPfactory_state::TRANSITION) _illegalStateError_("endTransitionDeclaration");
    else _states_.pop_back();

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
      _illegalStateError_("startTransitionDeclaration");
    else _states_.push_back(FMDPfactory_state::COST);

    //       VERBOSITY ( "starting Cost declaration" );
    this->_initializeFunctionGraph_();
  }


  // Tells the factory to add a cost

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::addCost(const MultiDimAdressable* cost) {
    const MultiDimImplementation< GUM_SCALAR >* c
       = reinterpret_cast< const MultiDimImplementation< GUM_SCALAR >* >(cost);

    if (state() != FMDPfactory_state::COST) _illegalStateError_("addCost");
    else if (_foo_flag_) _fmdp_->addCostForAction(_fmdp_->actionId(_stringBag_[0]), c);
    else _fmdp_->addCost(c);
  }


  // Tells the factory to add a cost
  // This cost table will be extracted from incorporated multiDimFunctionGraph.

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::addCost() {
    if (state() != FMDPfactory_state::COST) _illegalStateError_("addCost");
    else {
      this->_finalizeFunctionGraph_();

      if (_foo_flag_)
        _fmdp_->addCostForAction(_fmdp_->actionId(_stringBag_[0]), this->_FunctionGraph_);
      else _fmdp_->addCost(this->_FunctionGraph_);
    }
  }


  // Tells the factory that we're out of a cost declaration.

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::endCostDeclaration() {
    if (state() != FMDPfactory_state::COST) _illegalStateError_("endCostDeclaration");
    else _states_.pop_back();

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
      _illegalStateError_("startRewardDeclaration");
    else _states_.push_back(FMDPfactory_state::REWARD);

    //       VERBOSITY ( "starting reward declaration" );
    this->_initializeFunctionGraph_();
  }


  // Tells the factory that we're in a reward declaration mode where the global
  // reward diagram is an operation between simplier dd

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::setOperationModeOn(std::string operationType) {
    _foo_flag_ = true;
    std::string ot(operationType);
    _stringBag_.push_back(ot);
  }


  // Tells the factory to add a reward

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::addReward(const MultiDimAdressable* reward) {
    const MultiDimImplementation< GUM_SCALAR >* r
       = reinterpret_cast< const MultiDimImplementation< GUM_SCALAR >* >(reward);

    if (state() != FMDPfactory_state::REWARD) _illegalStateError_("addReward");
    else _fmdp_->addReward(r);
  }


  // Tells the factory to add a reward
  // This reward table will be extracted from incorporated
  // multiDimFunctionGraph.

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::addReward() {
    if (state() != FMDPfactory_state::REWARD) _illegalStateError_("addReward");
    else {
      this->_finalizeFunctionGraph_();
      _FunctionGraph_->setTableName("Reward");

      if (_foo_flag_) _ddBag_.push_back(this->_FunctionGraph_);
      else _fmdp_->addReward(this->_FunctionGraph_);
    }
  }


  // Tells the factory that we're out of a reward declaration.

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::endRewardDeclaration() {
    if (state() != FMDPfactory_state::REWARD) _illegalStateError_("endRewardDeclaration");
    else {
      if (_foo_flag_) {
        MultiDimImplementation< GUM_SCALAR >* res = nullptr;
        MultiDimImplementation< GUM_SCALAR >* temp;

        for (const auto elt: _ddBag_) {
          temp = res;

          switch (_stringBag_[0][0]) {
            case '+': res = add2MultiDimFunctionGraphs(res, elt); break;

            case '-': res = subtract2MultiDimFunctionGraphs(res, elt); break;

            case '*': res = multiply2MultiDimFunctionGraphs(res, elt); break;

            case '/': res = divide2MultiDimFunctionGraphs(res, elt); break;

            default: break;
          }

          delete elt;

          if (temp != nullptr) delete temp;
        }
        reinterpret_cast< MultiDimFunctionGraph< GUM_SCALAR >* >(res)->setTableName("Reward");
        _fmdp_->addReward(res);
      }

      _resetParts_();
      _states_.pop_back();
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
    if (state() != FMDPfactory_state::NONE) _illegalStateError_("startDiscountDeclaration");
    else _states_.push_back(FMDPfactory_state::DISCOUNT);

    //       VERBOSITY ( "starting discount declaration" );
  }


  // Tells the factory to add a discount

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::addDiscount(float discount) {
    if (state() != FMDPfactory_state::DISCOUNT) _illegalStateError_("addDiscount");
    //    else
    //       _fmdp_->setDiscount ( ( GUM_SCALAR ) discount );
  }


  // Tells the factory that we're out of a discount declaration.

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::endDiscountDeclaration() {
    if (state() != FMDPfactory_state::DISCOUNT) _illegalStateError_("endDiscountDeclaration");
    else _states_.pop_back();

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
  INLINE NodeId FMDPFactory< GUM_SCALAR >::addInternalNode(std::string name_of_var) {
    return _FunctionGraph_->manager()->addInternalNode(variable(name_of_var));
  }


  // Insert in diagram a terminal node

  template < typename GUM_SCALAR >
  INLINE NodeId FMDPFactory< GUM_SCALAR >::addTerminalNode(float value) {
    return _FunctionGraph_->manager()->addTerminalNode((GUM_SCALAR)value);
  }


  // Insert an arc in diagram
  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::addArc(NodeId from, NodeId to, Idx modality) {
    _FunctionGraph_->manager()->setSon(from, modality, to);
  }


  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::setRoot(NodeId rootId) {
    _FunctionGraph_->manager()->setRootNode(rootId);
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
  INLINE void FMDPFactory< GUM_SCALAR >::_illegalStateError_(const std::string& s) {
    std::string msg = "Illegal state call (";
    msg += s;
    msg += ") in state ";

    switch (state()) {
      case FMDPfactory_state::NONE: msg += "FMDPfactory_state::NONE"; break;

      case FMDPfactory_state::VARIABLE: msg += "FMDPfactory_state::VARIABLE"; break;

      case FMDPfactory_state::ACTION: msg += "FMDPfactory_state::ACTION"; break;

      case FMDPfactory_state::TRANSITION: msg += "FMDPfactory_state::TRANSITION"; break;

      case FMDPfactory_state::COST: msg += "FMDPfactory_state::COST"; break;

      case FMDPfactory_state::REWARD: msg += "FMDPfactory_state::REWARD"; break;

      case FMDPfactory_state::DISCOUNT: msg += "FMDPfactory_state::DISCOUNT"; break;

      default: msg += "Unknown state";
    }

    GUM_ERROR(OperationNotAllowed, msg)
  }


  // Reset the different parts used to constructed the IBayesNet.

  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::_resetParts_() {
    _foo_flag_ = false;
    _bar_flag_ = false;
    _stringBag_.clear();
    _ddBag_.clear();
  }


  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::_initializeFunctionGraph_() {
    this->_FunctionGraph_ = MultiDimFunctionGraph< GUM_SCALAR >::getReducedAndOrderedInstance();
    // Recopie des variables principales dans le graphe de décision
    for (auto varIter = _fmdp_->beginVariables(); varIter != _fmdp_->endVariables(); ++varIter) {
      _FunctionGraph_->add(**varIter);
    }

    // Recopie des version primes des variables dans le graphe de décision
    for (auto varIter = _fmdp_->beginVariables(); varIter != _fmdp_->endVariables(); ++varIter) {
      _FunctionGraph_->add(*(_fmdp_->main2prime(*varIter)));
    }
  }


  template < typename GUM_SCALAR >
  INLINE void FMDPFactory< GUM_SCALAR >::_finalizeFunctionGraph_() {
    this->_FunctionGraph_->manager()->reduce();
    this->_FunctionGraph_->manager()->clean();
  }

  //~ ==============
  //~ // Check if a variable with the given name exists, if not raise an
  // NotFound
  //~ // exception.
  //~ ==============
  //~ template<typename GUM_SCALAR> INLINE
  //~ void
  //~ FMDPFactory<GUM_SCALAR>:: _checkVariableName_ ( const std::string& name ) {
  //~ if ( ! _varNameMap_.exists ( name ) )
  //~ GUM_ERROR ( NotFound, name )
  //~ }


  // Copy operator is illegal, use only copy constructor.

  template < typename GUM_SCALAR >
  INLINE FMDPFactory< GUM_SCALAR >&
     FMDPFactory< GUM_SCALAR >::operator=(const FMDPFactory< GUM_SCALAR >& source) {
    GUM_ERROR(FatalError, "Illegal!")
    // For noisy compilers
    return *this;
  }
} /* namespace gum */
