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
 * @brief Implementation of the BayesNetFactory class.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)

 */

#include <agrum/BN/BayesNetFactory.h>

namespace gum {

  // Default constructor.
  // @param bn A pointer over the BayesNet filled by this factory.
  // @throw DuplicateElement Raised if two variables in bn share the same
  //                         name.
  template < typename GUM_SCALAR >
  INLINE BayesNetFactory< GUM_SCALAR >::BayesNetFactory(BayesNet< GUM_SCALAR >* bn) :
      _parents_(nullptr), _impl_(0), _bn_(bn) {
    GUM_CONSTRUCTOR(BayesNetFactory);
    _states_.push_back(factory_state::NONE);

    for (auto node: bn->nodes()) {
      if (_varNameMap_.exists(bn->variable(node).name()))
        GUM_ERROR(DuplicateElement, "Name already used: " << bn->variable(node).name())

      _varNameMap_.insert(bn->variable(node).name(), node);
    }

    resetVerbose();
  }

  // Copy constructor.
  // The copy will have an exact copy of the constructed BayesNet in source.
  template < typename GUM_SCALAR >
  INLINE
     BayesNetFactory< GUM_SCALAR >::BayesNetFactory(const BayesNetFactory< GUM_SCALAR >& source) :
      _parents_(nullptr),
      _impl_(nullptr), _bn_(nullptr) {
    GUM_CONS_CPY(BayesNetFactory);

    if (source.state() != factory_state::NONE) {
      GUM_ERROR(OperationNotAllowed, "Illegal state to proceed make a copy.")
    } else {
      _states_ = source._states_;
      _bn_     = new BayesNet< GUM_SCALAR >(*(source._bn_));
    }
  }

  // Destructor
  template < typename GUM_SCALAR >
  INLINE BayesNetFactory< GUM_SCALAR >::~BayesNetFactory() {
    GUM_DESTRUCTOR(BayesNetFactory);

    if (_parents_ != nullptr) delete _parents_;

    if (_impl_ != nullptr) {
      //@todo better than throwing an exception from inside a destructor but
      // still ...
      std::cerr << "[BN factory] Implementation defined for a variable but not used. "
                   "You should call endVariableDeclaration() before "
                   "deleting me."
                << std::endl;
      exit(1);
    }
  }

  // Returns the BayesNet created by this factory.
  template < typename GUM_SCALAR >
  INLINE BayesNet< GUM_SCALAR >* BayesNetFactory< GUM_SCALAR >::bayesNet() {
    return _bn_;
  }

  template < typename GUM_SCALAR >
  INLINE const DiscreteVariable& BayesNetFactory< GUM_SCALAR >::varInBN(NodeId id) {
    return _bn_->variable(id);
  }

  // Returns the current state of the factory.
  template < typename GUM_SCALAR >
  INLINE IBayesNetFactory::factory_state BayesNetFactory< GUM_SCALAR >::state() const {
    // This is ok because there is always at least the state NONE in the stack.
    return _states_.back();
  }

  // Returns the NodeId of a variable given it's name.
  // @throw NotFound Raised if no variable matches the name.
  template < typename GUM_SCALAR >
  INLINE NodeId BayesNetFactory< GUM_SCALAR >::variableId(const std::string& name) const {
    try {
      return _varNameMap_[name];
    } catch (NotFound const&) { GUM_ERROR(NotFound, name) }
  }

  // Returns a constant reference on a variable given it's name.
  // @throw NotFound Raised if no variable matches the name.
  template < typename GUM_SCALAR >
  INLINE const DiscreteVariable&
     BayesNetFactory< GUM_SCALAR >::variable(const std::string& name) const {
    try {
      return _bn_->variable(variableId(name));
    } catch (NotFound const&) { GUM_ERROR(NotFound, name) }
  }

  // Returns the domainSize of the cpt for the node n.
  // @throw NotFound raised if no such NodeId exists.
  // @throw OperationNotAllowed if there is no Bayesian networks.
  template < typename GUM_SCALAR >
  INLINE Size BayesNetFactory< GUM_SCALAR >::cptDomainSize(const NodeId n) const {
    return _bn_->cpt(n).domainSize();
  }

  // Tells the factory that we're in a network declaration.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::startNetworkDeclaration() {
    if (state() != factory_state::NONE) {
      _illegalStateError_("startNetworkDeclaration");
    } else {
      _states_.push_back(factory_state::NETWORK);
    }
  }

  // Tells the factory to add a property to the current network.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::addNetworkProperty(const std::string& propName,
                                                                const std::string& propValue) {
    _bn_->setProperty(propName, propValue);
  }

  // Tells the factory that we're out of a network declaration.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::endNetworkDeclaration() {
    if (state() != factory_state::NETWORK) {
      _illegalStateError_("endNetworkDeclaration");
    } else {
      _states_.pop_back();
    }
  }

  // Tells the factory that we're in a variable declaration.
  // A variable is considered as a LabelizedVariable while its type is not defined.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::startVariableDeclaration() {
    if (state() != factory_state::NONE) {
      _illegalStateError_("startVariableDeclaration");
    } else {
      _states_.push_back(factory_state::VARIABLE);
      _stringBag_.emplace_back("name");
      _stringBag_.emplace_back("desc");
      _stringBag_.emplace_back("L");
    }
  }

  // Tells the factory the current variable's name.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::variableName(const std::string& name) {
    if (state() != factory_state::VARIABLE) {
      _illegalStateError_("variableName");
    } else {
      if (_varNameMap_.exists(name)) { GUM_ERROR(DuplicateElement, "Name already used: " << name) }

      _foo_flag_     = true;
      _stringBag_[0] = name;
    }
  }

  // Tells the factory the current variable's description.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::variableDescription(const std::string& desc) {
    if (state() != factory_state::VARIABLE) {
      _illegalStateError_("variableDescription");
    } else {
      _bar_flag_     = true;
      _stringBag_[1] = desc;
    }
  }

  // Tells the factory the current variable's type.
  // L : Labelized
  // R : Range
  // C : Continuous
  // D : Discretized
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::variableType(const gum::VarType& type) {
    if (state() != factory_state::VARIABLE) {
      _illegalStateError_("variableType");
    } else {
      switch (type) {
        case VarType::Discretized: _stringBag_[2] = "D"; break;
        case VarType::Range: _stringBag_[2] = "R"; break;
        case VarType::Integer: _stringBag_[2] = "I"; break;
        case VarType::Labelized: _stringBag_[2] = "L"; break;
        case VarType::Continuous:
          GUM_ERROR(OperationNotAllowed,
                    "Continuous variable (" + _stringBag_[0]
                       + ") are not supported in Bayesian networks.")
        default: GUM_ERROR(OperationNotAllowed, "Unknown type for (" + _stringBag_[0] + ")")
      }
    }
  }

  // Adds a modality to the current variable.
  // @throw DuplicateElement If the current variable already has a modality
  //                         with the same name.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::addModality(const std::string& name) {
    if (state() != factory_state::VARIABLE) {
      _illegalStateError_("addModality");
    } else {
      _checkModalityInBag_(name);
      _stringBag_.push_back(name);
    }
  }

  // Adds a modality to the current variable.
  // @throw DuplicateElement If the current variable already has a modality
  //                         with the same name.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::addMin(const long& min) {
    if (state() != factory_state::VARIABLE) {
      _illegalStateError_("addMin");
    } else {
      _stringBag_.push_back(std::to_string(min));
    }
  }

  // Adds a modality to the current variable.
  // @throw DuplicateElement If the current variable already has a modality
  //                         with the same name.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::addMax(const long& max) {
    if (state() != factory_state::VARIABLE) {
      _illegalStateError_("addMin");
    } else {
      _stringBag_.push_back(std::to_string(max));
    }
  }

  // Adds a modality to the current variable.
  // @throw DuplicateElement If the current variable already has a modality
  //                         with the same name.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::addTick(const GUM_SCALAR& tick) {
    if (state() != factory_state::VARIABLE) {
      _illegalStateError_("addTick");
    } else {
      _stringBag_.push_back(std::to_string(tick));
    }
  }

  // @brief Defines the implementation to use for Potential.
  // @warning The implementation must be empty.
  // @warning The pointer is always delegated to Potential! No copy of it
  //          is made.
  // @todo When copy of a MultiDimImplementation is available use a copy
  //       behaviour for this method.
  // @throw NotFound Raised if no variable matches var.
  // @throw OperationNotAllowed Raised if impl is not empty.
  // @throw OperationNotAllowed If an implementation is already defined for the
  //                            current variable.
  template < typename GUM_SCALAR >
  INLINE void
     BayesNetFactory< GUM_SCALAR >::setVariableCPTImplementation(MultiDimAdressable* adressable) {
    auto impl = dynamic_cast< MultiDimImplementation< GUM_SCALAR >* >(adressable);

    if (state() != factory_state::VARIABLE) {
      _illegalStateError_("setVariableCPTImplementation");
    } else {
      if (impl == nullptr) {
        GUM_ERROR(OperationNotAllowed,
                  "An implementation for this variable is already "
                  "defined.")
      } else if (impl->nbrDim() > 0) {
        GUM_ERROR(OperationNotAllowed, "This implementation is not empty.")
      }

      _impl_ = impl;
    }
  }

  // Tells the factory that we're out of a variable declaration.
  template < typename GUM_SCALAR >
  INLINE NodeId BayesNetFactory< GUM_SCALAR >::endVariableDeclaration() {
    if (state() != factory_state::VARIABLE) {
      _illegalStateError_("endVariableDeclaration");
    } else if (_foo_flag_ && (_stringBag_.size() > 4)) {
      DiscreteVariable* var = nullptr;

      // if the current variable is a LabelizedVariable
      if (_stringBag_[2] == "L") {
        auto l = new LabelizedVariable(_stringBag_[0], (_bar_flag_) ? _stringBag_[1] : "", 0);

        for (size_t i = 3; i < _stringBag_.size(); ++i) {
          l->addLabel(_stringBag_[i]);
        }

        var = l;
        // if the current variable is a RangeVariable
      } else if (_stringBag_[2] == "I") {
        // try to create the domain of the variable
        std::vector< int > domain;
        for (size_t i = 3; i < _stringBag_.size(); ++i) {
          domain.push_back(std::stoi(_stringBag_[i]));
        }

        IntegerVariable* v
           = new IntegerVariable(_stringBag_[0], _bar_flag_ ? _stringBag_[1] : "", domain);
        var = v;
      } else if (_stringBag_[2] == "R") {
        RangeVariable* r = new RangeVariable(_stringBag_[0],
                                             _bar_flag_ ? _stringBag_[1] : "",
                                             std::stol(_stringBag_[3]),
                                             std::stol(_stringBag_[4]));

        var = r;
        // if the current variable is a DiscretizedVariable
      } else if (_stringBag_[2] == "D") {
        auto d = new DiscretizedVariable< GUM_SCALAR >(_stringBag_[0],
                                                       _bar_flag_ ? _stringBag_[1] : "");

        for (size_t i = 3; i < _stringBag_.size(); ++i) {
          d->addTick(std::stof(_stringBag_[i]));
        }

        var = d;
      }

      if (_impl_ != 0) {
        _varNameMap_.insert(var->name(), _bn_->add(*var, _impl_));
        _impl_ = 0;
      } else {
        _varNameMap_.insert(var->name(), _bn_->add(*var));
      }

      NodeId retVal = _varNameMap_[var->name()];

      delete var;

      _resetParts_();
      _states_.pop_back();

      return retVal;
    } else {
      std::stringstream msg;
      msg << "Not enough modalities (";

      if (_stringBag_.size() > 3) {
        msg << _stringBag_.size() - 3;
      } else {
        msg << 0;
      }

      msg << ") declared for variable ";

      if (_foo_flag_) {
        msg << _stringBag_[0];
      } else {
        msg << "unknown";
      }

      _resetParts_();

      _states_.pop_back();
      GUM_ERROR(OperationNotAllowed, msg.str())
    }

    // For noisy compilers
    return 0;
  }

  // Tells the factory that we're declaring parents for some variable.
  // @var The concerned variable's name.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::startParentsDeclaration(const std::string& var) {
    if (state() != factory_state::NONE) {
      _illegalStateError_("startParentsDeclaration");
    } else {
      _checkVariableName_(var);
      _stringBag_.insert(_stringBag_.begin(), var);
      _states_.push_back(factory_state::PARENTS);
    }
  }

  // Tells the factory for which variable we're declaring parents.
  // @var The parent's name.
  // @throw NotFound Raised if var does not exists.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::addParent(const std::string& var) {
    if (state() != factory_state::PARENTS) {
      _illegalStateError_("addParent");
    } else {
      _checkVariableName_(var);
      _stringBag_.push_back(var);
    }
  }

  // Tells the factory that we've finished declaring parents for some
  // variable. When parents exist, endParentsDeclaration creates some arcs.
  // These arcs are created in the inverse order of the order of the parent
  // specification.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::endParentsDeclaration() {
    if (state() != factory_state::PARENTS) {
      _illegalStateError_("endParentsDeclaration");
    } else {
      NodeId id = _varNameMap_[_stringBag_[0]];

      // PLEASE NOTE THAT THE ORDER IS INVERSE

      for (size_t i = _stringBag_.size() - 1; i > 0; --i) {
        _bn_->addArc(_varNameMap_[_stringBag_[i]], id);
      }

      _resetParts_();

      _states_.pop_back();
    }
  }

  // Tells the factory that we're declaring a conditional probability table
  // for some variable.
  // @param var The concerned variable's name.
  template < typename GUM_SCALAR >
  INLINE void
     BayesNetFactory< GUM_SCALAR >::startRawProbabilityDeclaration(const std::string& var) {
    if (state() != factory_state::NONE) {
      _illegalStateError_("startRawProbabilityDeclaration");
    } else {
      _checkVariableName_(var);
      _stringBag_.push_back(var);
      _states_.push_back(factory_state::RAW_CPT);
    }
  }

  // @brief Fills the variable's table with the values in rawTable.
  // Parse the parents in the same order in which they were added to the
  // variable.
  // Given a sequence [var, p_1, p_2, ...,p_n-1, p_n] of parents, modalities are
  // parsed
  // in the given order (if all p_i are binary):
  // [0, 0, ..., 0, 0], [0, 0, ..., 0, 1],
  // [0, 0, ..., 1, 0], [0, 0, ..., 1, 1],
  // ...,
  // [1, 1, ..., 1, 0], [1, 1, ..., 1, 1].
  // @param rawTable The raw table.
  template < typename GUM_SCALAR >
  INLINE void
     BayesNetFactory< GUM_SCALAR >::rawConditionalTable(const std::vector< std::string >& variables,
                                                        const std::vector< float >& rawTable) {
    if (state() != factory_state::RAW_CPT) {
      _illegalStateError_("rawConditionalTable");
    } else {
      _fillProbaWithValuesTable_(variables, rawTable);
    }
  }

  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::_fillProbaWithValuesTable_(
     const std::vector< std::string >& variables,
     const std::vector< float >&       rawTable) {
    const Potential< GUM_SCALAR >& table = _bn_->cpt(_varNameMap_[_stringBag_[0]]);
    Instantiation                  cptInst(table);

    List< const DiscreteVariable* > varList;

    for (size_t i = 0; i < variables.size(); ++i) {
      varList.pushBack(&(_bn_->variable(_varNameMap_[variables[i]])));
    }

    Idx nbrVar = varList.size();

    std::vector< Idx > modCounter;

    // initializing the array
    for (NodeId i = 0; i < nbrVar; i++) {
      modCounter.push_back(Idx(0));
    }

    Idx j = 0;

    do {
      for (NodeId i = 0; i < nbrVar; i++) {
        cptInst.chgVal(*(varList[i]), modCounter[i]);
      }

      if (j < rawTable.size()) {
        table.set(cptInst, (GUM_SCALAR)rawTable[j]);
      } else {
        table.set(cptInst, (GUM_SCALAR)0);
      }

      j++;
    } while (_increment_(modCounter, varList));
  }

  template < typename GUM_SCALAR >
  INLINE void
     BayesNetFactory< GUM_SCALAR >::rawConditionalTable(const std::vector< float >& rawTable) {
    if (state() != factory_state::RAW_CPT) {
      _illegalStateError_("rawConditionalTable");
    } else {
      _fillProbaWithValuesTable_(rawTable);
    }
  }

  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::_fillProbaWithValuesTable_(
     const std::vector< float >& rawTable) {
    const Potential< GUM_SCALAR >& table = _bn_->cpt(_varNameMap_[_stringBag_[0]]);

    Instantiation cptInst(table);

    // the main loop is on the first variables. The others are in the right
    // order.
    const DiscreteVariable& first = table.variable(0);
    Idx                     j     = 0;

    for (cptInst.setFirstVar(first); !cptInst.end(); cptInst.incVar(first)) {
      for (cptInst.setFirstNotVar(first); !cptInst.end(); cptInst.incNotVar(first))
        table.set(cptInst, (j < rawTable.size()) ? (GUM_SCALAR)rawTable[j++] : (GUM_SCALAR)0);

      cptInst.unsetEnd();
    }
  }

  template < typename GUM_SCALAR >
  INLINE bool
     BayesNetFactory< GUM_SCALAR >::_increment_(std::vector< gum::Idx >&         modCounter,
                                                List< const DiscreteVariable* >& varList) const {
    bool last = true;

    for (NodeId j = 0; j < modCounter.size(); j++) {
      last = (modCounter[j] == (varList[j]->domainSize() - 1)) && last;

      if (!last) break;
    }

    if (last) { return false; }

    bool add = false;

    auto i = NodeId(varList.size() - 1);

    do {
      if (modCounter[i] == (varList[i]->domainSize() - 1)) {
        modCounter[i] = 0;
        add           = true;
      } else {
        modCounter[i] += 1;
        add = false;
      }

      i--;
    } while (add);

    return true;
  }

  // Tells the factory that we finished declaring a conditional probability
  // table.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::endRawProbabilityDeclaration() {
    if (state() != factory_state::RAW_CPT) {
      _illegalStateError_("endRawProbabilityDeclaration");
    } else {
      _resetParts_();
      _states_.pop_back();
    }
  }

  // Tells the factory that we're starting a factorized declaration.
  template < typename GUM_SCALAR >
  INLINE void
     BayesNetFactory< GUM_SCALAR >::startFactorizedProbabilityDeclaration(const std::string& var) {
    if (state() != factory_state::NONE) {
      _illegalStateError_("startFactorizedProbabilityDeclaration");
    } else {
      _checkVariableName_(var);
      _stringBag_.insert(_stringBag_.begin(), var);
      _states_.push_back(factory_state::FACT_CPT);
    }
  }

  // Tells the factory that we start an entry of a factorized conditional
  // probability table.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::startFactorizedEntry() {
    if (state() != factory_state::FACT_CPT) {
      _illegalStateError_("startFactorizedEntry");
    } else {
      _parents_ = new Instantiation();
      _states_.push_back(factory_state::FACT_ENTRY);
    }
  }

  // Tells the factory that we finished declaring a conditional probability
  // table.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::endFactorizedEntry() {
    if (state() != factory_state::FACT_ENTRY) {
      _illegalStateError_("endFactorizedEntry");
    } else {
      delete _parents_;
      _parents_ = nullptr;
      _states_.pop_back();
    }
  }

  // Tells the factory on which modality we want to instantiate one of
  // variable's parent.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::setParentModality(const std::string& parent,
                                                               const std::string& modality) {
    if (state() != factory_state::FACT_ENTRY) {
      _illegalStateError_("string");
    } else {
      _checkVariableName_(parent);
      Idx id = _checkVariableModality_(parent, modality);
      (*_parents_) << _bn_->variable(_varNameMap_[parent]);
      _parents_->chgVal(_bn_->variable(_varNameMap_[parent]), id);
    }
  }

  // @brief Gives the values of the variable with respect to precedent
  //        parents modality.
  // If some parents have no modality set, then we apply values for all
  // instantiations of that parent.
  //
  // This means you can declare a default value for the table by doing
  // @code
  // BayesNetFactory factory;
  // // Do stuff
  // factory.startVariableDeclaration();
  // factory.variableName("foo");
  // factory.endVariableDeclaration();
  // factory.startParentsDeclaration("foo");
  // // add parents
  // factory.endParentsDeclaration();
  // factory.startFactorizedProbabilityDeclaration("foo");
  // std::vector<float> seq;
  // seq.insert(0.4); // if foo true
  // seq.insert(O.6); // if foo false
  // factory.setVariableValues(seq); // fills the table with a default value
  // // finish your stuff
  // factory.endFactorizedProbabilityDeclaration();
  // @code
  // as for raw Probability, if value's size is different than the number of
  // modalities of the current variable, we don't use the supplementary values and
  // we fill by 0 the missing values.
  template < typename GUM_SCALAR >
  INLINE void
     BayesNetFactory< GUM_SCALAR >::setVariableValuesUnchecked(const std::vector< float >& values) {
    if (state() != factory_state::FACT_ENTRY) {
      _illegalStateError_("setVariableValues");
    } else {
      const DiscreteVariable& var   = _bn_->variable(_varNameMap_[_stringBag_[0]]);
      NodeId                  varId = _varNameMap_[_stringBag_[0]];

      if (_parents_->domainSize() > 0) {
        Instantiation inst(_bn_->cpt(_varNameMap_[var.name()]));
        inst.setVals(*_parents_);
        // Creating an instantiation containing all the variables not ins
        //  _parents_.
        Instantiation inst_default;
        inst_default << var;

        for (auto node: _bn_->parents(varId)) {
          if (!_parents_->contains(_bn_->variable(node))) { inst_default << _bn_->variable(node); }
        }

        // Filling the variable's table.
        for (inst.setFirstIn(inst_default); !inst.end(); inst.incIn(inst_default)) {
          (_bn_->cpt(varId))
             .set(inst,
                  inst.val(var) < values.size() ? (GUM_SCALAR)values[inst.val(var)]
                                                : (GUM_SCALAR)0);
        }
      } else {
        Instantiation inst(_bn_->cpt(_varNameMap_[var.name()]));
        Instantiation var_inst;
        var_inst << var;

        for (var_inst.setFirst(); !var_inst.end(); ++var_inst) {
          inst.setVals(var_inst);

          for (inst.setFirstOut(var_inst); !inst.end(); inst.incOut(var_inst)) {
            (_bn_->cpt(varId))
               .set(inst,
                    inst.val(var) < values.size() ? (GUM_SCALAR)values[inst.val(var)]
                                                  : (GUM_SCALAR)0);
          }
        }
      }
    }
  }

  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::setVariableValues(const std::vector< float >& values) {
    if (state() != factory_state::FACT_ENTRY) {
      _illegalStateError_("setVariableValues");
    } else {
      //     Checking consistency between values and var.
      if (const DiscreteVariable& var = _bn_->variable(_varNameMap_[_stringBag_[0]]);
          values.size() != var.domainSize()) {
        GUM_ERROR(OperationNotAllowed,
                  var.name() << " : invalid number of modalities: found " << values.size()
                             << " while needed " << var.domainSize())
      }

      setVariableValuesUnchecked(values);
    }
  }

  // Tells the factory that we finished declaring a conditional probability
  // table.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::endFactorizedProbabilityDeclaration() {
    if (state() != factory_state::FACT_CPT) {
      _illegalStateError_("endFactorizedProbabilityDeclaration");
    } else {
      _resetParts_();
      _states_.pop_back();
    }
  }

  // @brief Define a variable.
  // You can only call this method is the factory is in the NONE or NETWORK
  // state.
  // The variable is added by copy.
  // @param var The pointer over a DiscreteVariable used to define a new
  //            variable in the built BayesNet.
  // @throw DuplicateElement Raised if a variable with the same name already
  //                         exists.
  // @throw OperationNotAllowed Raised if redefineParents == false and if table
  //                            is not a valid CPT for var in the current state
  //                            of the BayesNet.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::setVariable(const DiscreteVariable& var) {
    if (state() != factory_state::NONE) {
      _illegalStateError_("setVariable");
    } else {
      try {
        _checkVariableName_(var.name());
        GUM_ERROR(DuplicateElement, "Name already used: " << var.name())
      } catch (NotFound const&) {
        // The var name is unused
        _varNameMap_.insert(var.name(), _bn_->add(var));
      }
    }
  }

  // @brief Define a variable's CPT.
  // You can only call this method if the factory is in the NONE or NETWORK
  // state.
  // Be careful that table is given to the built BayesNet, so it will be
  // deleted with it, and you should not directly access it after you call
  // this method.
  // When the redefineParents flag is set to true the constructed BayesNet's
  // DAG is changed to fit with table's definition.
  // @param var The name of the concerned variable.
  // @param table A pointer over the CPT used for var.
  // @param redefineParents If true redefine parents of the variable to match
  // table's
  //                        variables set.
  //
  // @throw NotFound Raised if no variable matches var.
  // @throw OperationNotAllowed Raised if redefineParents == false and if table
  //                            is not a valid CPT for var in the current state
  //                            of the BayesNet.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::setVariableCPT(const std::string&  varName,
                                                            MultiDimAdressable* table,
                                                            bool                redefineParents) {
    auto pot = dynamic_cast< Potential< GUM_SCALAR >* >(table);

    if (state() != factory_state::NONE) {
      _illegalStateError_("setVariableCPT");
    } else {
      _checkVariableName_(varName);
      const DiscreteVariable& var   = _bn_->variable(_varNameMap_[varName]);
      NodeId                  varId = _varNameMap_[varName];
      // If we have to change the structure of the BayesNet, then we call a sub
      // method.

      if (redefineParents) {
        _setCPTAndParents_(var, pot);
      } else if (pot->contains(var)) {
        for (auto node: _bn_->parents(varId)) {
          if (!pot->contains(_bn_->variable(node))) {
            GUM_ERROR(OperationNotAllowed, "The CPT is not valid in the current BayesNet.")
          }
        }

        // CPT are created when a variable is added.
        _bn_->_unsafeChangePotential_(varId, pot);
      }
    }
  }

  // Raise an OperationNotAllowed with the message "Illegal state."
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::_illegalStateError_(const std::string& s) {
    std::string msg = "Illegal state call (";
    msg += s;
    msg += ") in state ";

    switch (state()) {
      case factory_state::NONE: {
        msg += "NONE";
        break;
      }

      case factory_state::NETWORK: {
        msg += "NETWORK";
        break;
      }

      case factory_state::VARIABLE: {
        msg += "VARIABLE";
        break;
      }

      case factory_state::PARENTS: {
        msg += "PARENTS";
        break;
      }

      case factory_state::RAW_CPT: {
        msg += "RAW_CPT";
        break;
      }

      case factory_state::FACT_CPT: {
        msg += "FACT_CPT";
        break;
      }

      case factory_state::FACT_ENTRY: {
        msg += "FACT_ENTRY";
        break;
      }

      default: {
        msg += "Unknown state";
      }
    }

    GUM_ERROR(OperationNotAllowed, msg)
  }

  // Check if a variable with the given name exists, if not raise an NotFound
  // exception.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::_checkVariableName_(const std::string& name) const {
    if (!_varNameMap_.exists(name)) { GUM_ERROR(NotFound, name) }
  }

  // Check if var exists and if mod is one of it's modality, if not raise an
  // NotFound exception.
  template < typename GUM_SCALAR >
  INLINE Idx BayesNetFactory< GUM_SCALAR >::_checkVariableModality_(const std::string& name,
                                                                    const std::string& mod) {
    _checkVariableName_(name);
    const DiscreteVariable& var = _bn_->variable(_varNameMap_[name]);

    for (Idx i = 0; i < var.domainSize(); ++i) {
      if (mod == var.label(i)) { return i; }
    }

    GUM_ERROR(NotFound, mod)
  }

  // Check if in  _stringBag_ there is no other modality with the same name.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::_checkModalityInBag_(const std::string& mod) {
    for (size_t i = 3; i < _stringBag_.size(); ++i) {
      if (mod == _stringBag_[i]) { GUM_ERROR(DuplicateElement, "Label already used: " << mod) }
    }
  }

  // Sub method of setVariableCPT() which redefine the BayesNet's DAG with
  // respect to table.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::_setCPTAndParents_(const DiscreteVariable&  var,
                                                                Potential< GUM_SCALAR >* table) {
    NodeId varId = _varNameMap_[var.name()];
    _bn_->dag_.eraseParents(varId);

    for (auto v: table->variablesSequence()) {
      if (v != (&var)) {
        _checkVariableName_(v->name());
        _bn_->dag_.addArc(_varNameMap_[v->name()], varId);
      }
    }

    // CPT are created when a variable is added.
    _bn_->_unsafeChangePotential_(varId, table);
  }

  // Reset the different parts used to constructed the BayesNet.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::_resetParts_() {
    _foo_flag_ = false;
    _bar_flag_ = false;
    _stringBag_.clear();
  }
} /* namespace gum */