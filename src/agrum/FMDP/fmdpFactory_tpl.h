/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


/**
 * @file
 * @brief Template Implementation of the FMDPFactory class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */

#include <agrum/base/multidim/implementations/multiDimImplementation.h>
#include <agrum/base/multidim/tensor.h>
#include <agrum/FMDP/fmdpFactory.h>

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

  template < typename GUM_ELEMENT >
  INLINE FMDPFactory< GUM_ELEMENT >::FMDPFactory(FMDP< GUM_ELEMENT >* fmdp) : _fmdp_(fmdp) {
    GUM_CONSTRUCTOR(FMDPFactory);

    _states_.push_back(FMDPfactory_state::NONE);
    resetVerbose();
    _actionIdcpt_ = 1;
  }

  // Destructor

  template < typename GUM_ELEMENT >
  INLINE FMDPFactory< GUM_ELEMENT >::~FMDPFactory() {
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

  template < typename GUM_ELEMENT >
  INLINE FMDP< GUM_ELEMENT >* FMDPFactory< GUM_ELEMENT >::fmdp() const {
    if (state() != FMDPfactory_state::NONE)
      GUM_ERROR(OperationNotAllowed,
                "Illegal state to return the factored "
                "markov decision process: it is not yet "
                "finished.");

    return _fmdp_;
  }

  // Returns the current state of the factory.

  template < typename GUM_ELEMENT >
  INLINE FMDPfactory_state FMDPFactory< GUM_ELEMENT >::state() const {
    // This is ok because there is alway at least the state
    // FMDPfactory_state::NONE in the stack.
    return _states_.back();
  }

  // Returns a constant reference on a variable given it's name.
  // @throw NotFound Raised if no variable matches the name.

  template < typename GUM_ELEMENT >
  INLINE const DiscreteVariable* FMDPFactory< GUM_ELEMENT >::variable(std::string_view name) const {
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

  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::startVariableDeclaration() {
    if (state() != FMDPfactory_state::NONE) _illegalStateError_("startVariableDeclaration");
    else {
      _states_.push_back(FMDPfactory_state::VARIABLE);
      _stringBag_.push_back("name");
      _stringBag_.push_back("desc");
    }

    //       VERBOSITY ( "  starting variable" );
  }

  // Tells the factory the current variable's name.

  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::variableName(std::string_view name) {
    if (state() != FMDPfactory_state::VARIABLE) _illegalStateError_("variableName");
    else {
      if (_varNameMap_.exists(name)) GUM_ERROR(DuplicateElement, "Name already used: " << name)

      _foo_flag_     = true;
      _stringBag_[0] = name;
      //         VERBOSITY ( "  -- variable " << name );
    }
  }

  // Tells the factory the current variable's description.

  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::variableDescription(std::string_view desc) {
    if (state() != FMDPfactory_state::VARIABLE) _illegalStateError_("variableDescription");
    else {
      _bar_flag_     = true;
      _stringBag_[1] = desc;
    }
  }

  // Adds a modality to the current variable.
  // @throw DuplicateElement If the current variable already has a modality
  //                         with the same name.

  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::addModality(std::string_view name) {
    if (state() != FMDPfactory_state::VARIABLE) _illegalStateError_("addModality");
    else {
      _checkModalityInBag_(name);
      _stringBag_.emplace_back(name);
    }
  }

  // Check if in  _stringBag_ there is no other modality with the same name.

  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::_checkModalityInBag_(std::string_view mod) {
    for (size_t i = 2; i < _stringBag_.size(); ++i)
      if (mod == _stringBag_[i])
        GUM_ERROR(DuplicateElement, "Modality" << mod << " already exists.")
  }

  // Tells the factory that we're out of a variable declaration.

  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::endVariableDeclaration() {
    if (state() != FMDPfactory_state::VARIABLE) _illegalStateError_("endVariableDeclaration");
    else if (_foo_flag_ && (_stringBag_.size() > 3)) {
      LabelizedVariable* var
          = new LabelizedVariable(_stringBag_[0], (_bar_flag_) ? _stringBag_[1] : "", 0);

      for (size_t i = 2; i < _stringBag_.size(); ++i) {
        var->addLabel(_stringBag_[i]);
      }

      _fmdp_->addVariable(var);
      _fmdp_->setDeleteVariablesOnDestruction(true);
      _varNameMap_.insert(var->name(), var);
      _varNameMap_.insert(_fmdp_->main2prime(var)->name(), _fmdp_->main2prime(var));

      _resetParts_();
      _states_.pop_back();

      //         VERBOSITY ( "  variable " << var->name() << " OK" );

    } else {
      const size_t      nModa   = _stringBag_.size() > 2 ? _stringBag_.size() - 2 : size_t{0};
      const std::string varName = _foo_flag_ ? _stringBag_[0] : "unknown";

      _resetParts_();
      _states_.pop_back();

      GUM_ERROR(OperationNotAllowed,
                std::format("Not enough modalities ({}) declared for variable {}", nModa, varName))
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

  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::startActionDeclaration() {
    if (state() != FMDPfactory_state::NONE) _illegalStateError_("startActionDeclaration");
    else {
      _foo_flag_ = true;
      _states_.push_back(FMDPfactory_state::ACTION);
    }

    //       VERBOSITY ( "starting action declaration" );
  }

  // Tells the factory to add an action

  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::addAction(std::string_view action) {
    if (state() != FMDPfactory_state::ACTION) _illegalStateError_("addAction");
    else {
      _stringBag_.emplace_back(action);
      _fmdp_->addAction(_actionIdcpt_++, std::string(action));
    }
  }

  // Tells the factory that we're out of an action declaration.

  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::endActionDeclaration() {
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

  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::startTransitionDeclaration() {
    if (state() != FMDPfactory_state::NONE && state() != FMDPfactory_state::ACTION)
      _illegalStateError_("startTransitionDeclaration");
    else _states_.push_back(FMDPfactory_state::TRANSITION);

    //       VERBOSITY ( "starting transition declaration" );
    this->_initializeFunctionGraph_();
  }

  // Tells the factory to add an action

  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::addTransition(std::string_view          var,
                                                        const MultiDimAdressable* transition) {
    const MultiDimImplementation< GUM_ELEMENT >* t
        = static_cast< const MultiDimImplementation< GUM_ELEMENT >* >(transition);

    if (state() != FMDPfactory_state::TRANSITION) _illegalStateError_("addTransition");
    else if (_foo_flag_)
      _fmdp_->addTransitionForAction(_fmdp_->actionId(_stringBag_[0]), variable(var), t);
    else _fmdp_->addTransition(variable(var), t);
  }

  // Tells the factory to add a transition table to the current fmdp.
  // This transition table will be extracted from incorporated
  // multiDimFunctionGraph.

  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::addTransition(std::string_view var) {
    if (state() != FMDPfactory_state::TRANSITION) _illegalStateError_("addTransition");
    else {
      this->_finalizeFunctionGraph_();

      if (_foo_flag_) {
        this->_FunctionGraph_->setTableName("ACTION :" + _stringBag_[0]
                                            + " - VARIABLE : " + std::string(var));
        _fmdp_->addTransitionForAction(_fmdp_->actionId(_stringBag_[0]),
                                       variable(var),
                                       this->_FunctionGraph_);
      } else {
        _fmdp_->addTransition(variable(var), this->_FunctionGraph_);
      }
    }
  }

  // Tells the factory that we're out of a transition declaration.

  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::endTransitionDeclaration() {
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

  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::startCostDeclaration() {
    if (state() != FMDPfactory_state::NONE && state() != FMDPfactory_state::ACTION)
      _illegalStateError_("startTransitionDeclaration");
    else _states_.push_back(FMDPfactory_state::COST);

    //       VERBOSITY ( "starting Cost declaration" );
    this->_initializeFunctionGraph_();
  }

  // Tells the factory to add a cost

  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::addCost(const MultiDimAdressable* cost) {
    const MultiDimImplementation< GUM_ELEMENT >* c
        = static_cast< const MultiDimImplementation< GUM_ELEMENT >* >(cost);

    if (state() != FMDPfactory_state::COST) _illegalStateError_("addCost");
    else if (_foo_flag_) _fmdp_->addCostForAction(_fmdp_->actionId(_stringBag_[0]), c);
    else _fmdp_->addCost(c);
  }

  // Tells the factory to add a cost
  // This cost table will be extracted from incorporated multiDimFunctionGraph.

  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::addCost() {
    if (state() != FMDPfactory_state::COST) _illegalStateError_("addCost");
    else {
      this->_finalizeFunctionGraph_();

      if (_foo_flag_)
        _fmdp_->addCostForAction(_fmdp_->actionId(_stringBag_[0]), this->_FunctionGraph_);
      else _fmdp_->addCost(this->_FunctionGraph_);
    }
  }

  // Tells the factory that we're out of a cost declaration.

  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::endCostDeclaration() {
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

  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::startRewardDeclaration() {
    if (state() != FMDPfactory_state::NONE && state() != FMDPfactory_state::ACTION)
      _illegalStateError_("startRewardDeclaration");
    else _states_.push_back(FMDPfactory_state::REWARD);

    //       VERBOSITY ( "starting reward declaration" );
    this->_initializeFunctionGraph_();
  }

  // Tells the factory that we're in a reward declaration mode where the global
  // reward diagram is an operation between simplier dd

  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::setOperationModeOn(std::string_view operationType) {
    _foo_flag_ = true;
    _stringBag_.push_back(std::string{operationType});
  }

  // Tells the factory to add a reward

  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::addReward(const MultiDimAdressable* reward) {
    const MultiDimImplementation< GUM_ELEMENT >* r
        = static_cast< const MultiDimImplementation< GUM_ELEMENT >* >(reward);

    if (state() != FMDPfactory_state::REWARD) _illegalStateError_("addReward");
    else _fmdp_->addReward(r);
  }

  // Tells the factory to add a reward
  // This reward table will be extracted from incorporated
  // multiDimFunctionGraph.

  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::addReward() {
    if (state() != FMDPfactory_state::REWARD) _illegalStateError_("addReward");
    else {
      this->_finalizeFunctionGraph_();
      _FunctionGraph_->setTableName("Reward");

      if (_foo_flag_) _ddBag_.push_back(this->_FunctionGraph_);
      else _fmdp_->addReward(this->_FunctionGraph_);
    }
  }

  // Tells the factory that we're out of a reward declaration.

  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::endRewardDeclaration() {
    if (state() != FMDPfactory_state::REWARD) _illegalStateError_("endRewardDeclaration");
    else {
      if (_foo_flag_) {
        MultiDimImplementation< GUM_ELEMENT >* res = nullptr;

        for (const auto elt: _ddBag_) {
          if (res == nullptr) {
            res = const_cast< MultiDimImplementation< GUM_ELEMENT >* >(elt);
            continue;
          }
          MultiDimImplementation< GUM_ELEMENT >* temp = res;

          switch (_stringBag_[0][0]) {
            case '+' : res = add2MultiDimFunctionGraphs(res, elt); break;

            case '-' : res = subtract2MultiDimFunctionGraphs(res, elt); break;

            case '*' : res = multiply2MultiDimFunctionGraphs(res, elt); break;

            case '/' : res = divide2MultiDimFunctionGraphs(res, elt); break;

            default : break;
          }

          delete elt;
          delete temp;
        }

        if (res != nullptr) {
          static_cast< MultiDimFunctionGraph< GUM_ELEMENT >* >(res)->setTableName("Reward");
          _fmdp_->addReward(res);
        }
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
  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::startDiscountDeclaration() {
    if (state() != FMDPfactory_state::NONE) _illegalStateError_("startDiscountDeclaration");
    else _states_.push_back(FMDPfactory_state::DISCOUNT);

    //       VERBOSITY ( "starting discount declaration" );
  }

  // Tells the factory to add a discount

  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::addDiscount(float discount) {
    if (state() != FMDPfactory_state::DISCOUNT) _illegalStateError_("addDiscount");
    else _fmdp_->setDiscount((GUM_ELEMENT)discount);
  }

  // Tells the factory that we're out of a discount declaration.

  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::endDiscountDeclaration() {
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

  template < typename GUM_ELEMENT >
  INLINE NodeId FMDPFactory< GUM_ELEMENT >::addInternalNode(std::string_view name_of_var) {
    return _FunctionGraph_->manager()->addInternalNode(variable(name_of_var));
  }

  // Insert in diagram a terminal node

  template < typename GUM_ELEMENT >
  INLINE NodeId FMDPFactory< GUM_ELEMENT >::addTerminalNode(float value) {
    return _FunctionGraph_->manager()->addTerminalNode((GUM_ELEMENT)value);
  }

  // Insert an arc in diagram
  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::addArc(NodeId from, NodeId to, Idx modality) {
    _FunctionGraph_->manager()->setSon(from, modality, to);
  }

  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::setRoot(NodeId rootId) {
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

  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::_illegalStateError_(std::string_view s) {
    std::string msg = "Illegal state call (";
    msg += s;
    msg += ") in state ";

    switch (state()) {
      case FMDPfactory_state::NONE : msg += "FMDPfactory_state::NONE"; break;

      case FMDPfactory_state::VARIABLE : msg += "FMDPfactory_state::VARIABLE"; break;

      case FMDPfactory_state::ACTION : msg += "FMDPfactory_state::ACTION"; break;

      case FMDPfactory_state::TRANSITION : msg += "FMDPfactory_state::TRANSITION"; break;

      case FMDPfactory_state::COST : msg += "FMDPfactory_state::COST"; break;

      case FMDPfactory_state::REWARD : msg += "FMDPfactory_state::REWARD"; break;

      case FMDPfactory_state::DISCOUNT : msg += "FMDPfactory_state::DISCOUNT"; break;

      default : msg += "Unknown state";
    }

    GUM_ERROR(OperationNotAllowed, msg)
  }

  // Reset the different parts used to constructed the IBayesNet.

  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::_resetParts_() {
    _foo_flag_ = false;
    _bar_flag_ = false;
    _stringBag_.clear();
    _ddBag_.clear();
  }

  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::_initializeFunctionGraph_() {
    this->_FunctionGraph_ = MultiDimFunctionGraph< GUM_ELEMENT >::getReducedAndOrderedInstance();
    // Recopie des variables principales dans le graphe de décision
    for (auto varIter = _fmdp_->beginVariables(); varIter != _fmdp_->endVariables(); ++varIter) {
      _FunctionGraph_->add(**varIter);
    }

    // Recopie des version primes des variables dans le graphe de décision
    for (auto varIter = _fmdp_->beginVariables(); varIter != _fmdp_->endVariables(); ++varIter) {
      _FunctionGraph_->add(*(_fmdp_->main2prime(*varIter)));
    }
  }

  template < typename GUM_ELEMENT >
  INLINE void FMDPFactory< GUM_ELEMENT >::_finalizeFunctionGraph_() {
    this->_FunctionGraph_->manager()->reduce();
    this->_FunctionGraph_->manager()->clean();
  }

  //~ ==============
  //~ // Check if a variable with the given name exists, if not raise an
  // NotFound
  //~ // exception.
  //~ ==============
  //~ template<typename GUM_ELEMENT> INLINE
  //~ void
  //~ FMDPFactory<GUM_ELEMENT>:: _checkVariableName_ ( const std::string& name ) {
  //~ if ( ! _varNameMap_.exists ( name ) )
  //~ GUM_ERROR ( NotFound, name )
  //~ }


  // Copy operator is illegal, use only copy constructor.

  template < typename GUM_ELEMENT >
  INLINE FMDPFactory< GUM_ELEMENT >&
         FMDPFactory< GUM_ELEMENT >::operator=(const FMDPFactory< GUM_ELEMENT >& source) {
    GUM_ERROR(FatalError, "Illegal!")
    // For noisy compilers
    return *this;
  }
} /* namespace gum */
