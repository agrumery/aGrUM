
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(@LIP6)

 */

#include <agrum/BN/BayesNetFactory.h>

#define VERBOSITY(x)                                                 \
  {                                                                  \
    if (isVerbose()) std::cerr << "[BN factory] " << x << std::endl; \
  }

namespace gum {

  // Default constructor.
  // @param bn A pointer over the BayesNet filled by this factory.
  // @throw DuplicateElement Raised if two variables in bn share the same
  //                         name.
  template < typename GUM_SCALAR >
  INLINE
     BayesNetFactory< GUM_SCALAR >::BayesNetFactory(BayesNet< GUM_SCALAR >* bn) :
      parents__(0),
      impl__(0), bn__(bn) {
    GUM_CONSTRUCTOR(BayesNetFactory);
    states__.push_back(factory_state::NONE);

    for (auto node: bn->nodes()) {
      if (varNameMap__.exists(bn->variable(node).name()))
        GUM_ERROR(DuplicateElement,
                  "Name already used: " << bn->variable(node).name());

      varNameMap__.insert(bn->variable(node).name(), node);
    }

    resetVerbose();
  }

  // Copy constructor.
  // The copy will have an exact copy of the constructed BayesNet in source.
  template < typename GUM_SCALAR >
  INLINE BayesNetFactory< GUM_SCALAR >::BayesNetFactory(
     const BayesNetFactory< GUM_SCALAR >& source) :
      parents__(0),
      impl__(0), bn__(0) {
    GUM_CONS_CPY(BayesNetFactory);

    if (source.state() != factory_state::NONE) {
      GUM_ERROR(OperationNotAllowed, "Illegal state to proceed make a copy.");
    } else {
      states__ = source.states__;
      bn__ = new BayesNet< GUM_SCALAR >(*(source.bn__));
    }
  }

  // Destructor
  template < typename GUM_SCALAR >
  INLINE BayesNetFactory< GUM_SCALAR >::~BayesNetFactory() {
    GUM_DESTRUCTOR(BayesNetFactory);

    if (parents__ != 0) delete parents__;

    if (impl__ != 0) {
      //@todo better than throwing an exception from inside a destructor but
      // still ...
      std::cerr
         << "[BN factory] Implementation defined for a variable but not used. "
            "You should call endVariableDeclaration() before "
            "deleting me."
         << std::endl;
      exit(1000);
    }
  }

  // Returns the BayesNet created by this factory.
  template < typename GUM_SCALAR >
  INLINE BayesNet< GUM_SCALAR >* BayesNetFactory< GUM_SCALAR >::bayesNet() {
    return bn__;
  }

  template < typename GUM_SCALAR >
  INLINE const DiscreteVariable&
               BayesNetFactory< GUM_SCALAR >::varInBN(NodeId id) {
    return bn__->variable(id);
  }

  // Returns the current state of the factory.
  template < typename GUM_SCALAR >
  INLINE IBayesNetFactory::factory_state
         BayesNetFactory< GUM_SCALAR >::state() const {
    // This is ok because there is alway at least the state NONE in the stack.
    return states__.back();
  }

  // Returns the NodeId of a variable given it's name.
  // @throw NotFound Raised if no variable matches the name.
  template < typename GUM_SCALAR >
  INLINE NodeId
     BayesNetFactory< GUM_SCALAR >::variableId(const std::string& name) const {
    try {
      return varNameMap__[name];
    } catch (NotFound&) { GUM_ERROR(NotFound, name); }
  }

  // Returns a constant reference on a variable given it's name.
  // @throw NotFound Raised if no variable matches the name.
  template < typename GUM_SCALAR >
  INLINE const DiscreteVariable&
               BayesNetFactory< GUM_SCALAR >::variable(const std::string& name) const {
    try {
      return bn__->variable(variableId(name));
    } catch (NotFound&) { GUM_ERROR(NotFound, name); }
  }

  // Returns the domainSize of the cpt for the node n.
  // @throw NotFound raised if no such NodeId exists.
  // @throw OperationNotAllowed if there is no bayesian networks.
  template < typename GUM_SCALAR >
  INLINE Size BayesNetFactory< GUM_SCALAR >::cptDomainSize(const NodeId n) const {
    return bn__->cpt(n).domainSize();
  }

  // Tells the factory that we're in a network declaration.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::startNetworkDeclaration() {
    if (state() != factory_state::NONE) {
      illegalStateError__("startNetworkDeclaration");
    } else {
      states__.push_back(factory_state::NETWORK);
    }

    VERBOSITY("starting network");
  }

  // Tells the factory to add a property to the current network.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::addNetworkProperty(
     const std::string& propName, const std::string& propValue) {
    bn__->setProperty(propName, propValue);
  }

  // Tells the factory that we're out of a network declaration.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::endNetworkDeclaration() {
    if (state() != factory_state::NETWORK) {
      illegalStateError__("endNetworkDeclaration");
    } else {
      states__.pop_back();
    }

    VERBOSITY("network OK");
  }

  // Tells the factory that we're in a variable declaration.
  // A variable is considered as a LabelizedVariable while its type is not defined.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::startVariableDeclaration() {
    if (state() != factory_state::NONE) {
      illegalStateError__("startVariableDeclaration");
    } else {
      states__.push_back(factory_state::VARIABLE);
      stringBag__.push_back("name");
      stringBag__.push_back("desc");
      stringBag__.push_back("L");
    }

    VERBOSITY("  starting variable");
  }

  // Tells the factory the current variable's name.
  template < typename GUM_SCALAR >
  INLINE void
     BayesNetFactory< GUM_SCALAR >::variableName(const std::string& name) {
    if (state() != factory_state::VARIABLE) {
      illegalStateError__("variableName");
    } else {
      if (varNameMap__.exists(name)) {
        GUM_ERROR(DuplicateElement, "Name already used: " << name);
      }

      foo_flag__ = true;
      stringBag__[0] = name;
      VERBOSITY("  -- variable " << name);
    }
  }

  // Tells the factory the current variable's description.
  template < typename GUM_SCALAR >
  INLINE void
     BayesNetFactory< GUM_SCALAR >::variableDescription(const std::string& desc) {
    if (state() != factory_state::VARIABLE) {
      illegalStateError__("variableDescription");
    } else {
      bar_flag__ = true;
      stringBag__[1] = desc;
    }
  }

  // Tells the factory the current variable's type.
  // L : Labelized
  // R : Range
  // C : Continuous
  // D : Discretized
  template < typename GUM_SCALAR >
  INLINE void
     BayesNetFactory< GUM_SCALAR >::variableType(const gum::VarType& type) {
    if (state() != factory_state::VARIABLE) {
      illegalStateError__("variableType");
    } else {
      switch (type) {
        case VarType::Discretized: stringBag__[2] = "D"; break;
        case VarType::Range: stringBag__[2] = "R"; break;
        case VarType::Continuous:
          GUM_ERROR(OperationNotAllowed,
                    "Continuous variable (" + stringBag__[0]
                       + ") are not supported in bayesian networks.");
        case VarType::Labelized: stringBag__[2] = "L"; break;
      }
    }
  }

  // Adds a modality to the current variable.
  // @throw DuplicateElement If the current variable already has a modality
  //                         with the same name.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::addModality(const std::string& name) {
    if (state() != factory_state::VARIABLE) {
      illegalStateError__("addModality");
    } else {
      checkModalityInBag__(name);
      stringBag__.push_back(name);
    }
  }

  // Adds a modality to the current variable.
  // @throw DuplicateElement If the current variable already has a modality
  //                         with the same name.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::addMin(const long& min) {
    if (state() != factory_state::VARIABLE) {
      illegalStateError__("addMin");
    } else {
      stringBag__.push_back(std::to_string(min));
    }
  }

  // Adds a modality to the current variable.
  // @throw DuplicateElement If the current variable already has a modality
  //                         with the same name.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::addMax(const long& max) {
    if (state() != factory_state::VARIABLE) {
      illegalStateError__("addMin");
    } else {
      stringBag__.push_back(std::to_string(max));
    }
  }

  // Adds a modality to the current variable.
  // @throw DuplicateElement If the current variable already has a modality
  //                         with the same name.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::addTick(const GUM_SCALAR& tick) {
    if (state() != factory_state::VARIABLE) {
      illegalStateError__("addTick");
    } else {
      stringBag__.push_back(std::to_string(tick));
    }
  }

  // @brief Defines the implementation to use for var's Potential.
  // @warning The implementation must be empty.
  // @warning The pointer is always delegated to var's Potential! No copy of it
  //          is made.
  // @todo When copy of a MultiDimImplementation is available use a copy
  //       behaviour for this method.
  // @throw NotFound Raised if no variable matches var.
  // @throw OperationNotAllowed Raised if impl is not empty.
  // @throw OperationNotAllowed If an implementation is already defined for the
  //                            current variable.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::setVariableCPTImplementation(
     MultiDimAdressable* adressable) {
    MultiDimImplementation< GUM_SCALAR >* impl =
       dynamic_cast< MultiDimImplementation< GUM_SCALAR >* >(adressable);

    if (state() != factory_state::VARIABLE) {
      illegalStateError__("setVariableCPTImplementation");
    } else {
      if (impl == 0) {
        GUM_ERROR(OperationNotAllowed,
                  "An implementation for this variable is already "
                  "defined.");
      } else if (impl->nbrDim() > 0) {
        GUM_ERROR(OperationNotAllowed, "This implementation is not empty.");
      }

      impl__ = impl;
    }
  }

  // Tells the factory that we're out of a variable declaration.
  template < typename GUM_SCALAR >
  INLINE NodeId BayesNetFactory< GUM_SCALAR >::endVariableDeclaration() {
    if (state() != factory_state::VARIABLE) {
      illegalStateError__("endVariableDeclaration");
    } else if (foo_flag__ && (stringBag__.size() > 4)) {
      DiscreteVariable* var = nullptr;

      // if the current variable is a LabelizedVariable
      if (stringBag__[2] == "L") {
        LabelizedVariable* l = new LabelizedVariable(
           stringBag__[0], (bar_flag__) ? stringBag__[1] : "", 0);

        for (size_t i = 3; i < stringBag__.size(); ++i) {
          l->addLabel(stringBag__[i]);
        }

        var = l;
        // if the current variable is a RangeVariable
      } else if (stringBag__[2] == "R") {
        RangeVariable* r = new RangeVariable(stringBag__[0],
                                             (bar_flag__) ? stringBag__[1] : "",
                                             std::stol(stringBag__[3]),
                                             std::stol(stringBag__[4]));

        var = r;
        // if the current variable is a DiscretizedVariable
      } else if (stringBag__[2] == "D") {
        DiscretizedVariable< GUM_SCALAR >* d =
           new DiscretizedVariable< GUM_SCALAR >(
              stringBag__[0], (bar_flag__) ? stringBag__[1] : "");

        for (size_t i = 3; i < stringBag__.size(); ++i) {
          d->addTick(std::stof(stringBag__[i]));
        }

        var = d;
      }

      if (impl__ != 0) {
        varNameMap__.insert(var->name(), bn__->add(*var, impl__));
        impl__ = 0;
      } else {
        varNameMap__.insert(var->name(), bn__->add(*var));
      }

      NodeId retVal = varNameMap__[var->name()];

      delete var;

      resetParts__();
      states__.pop_back();

      VERBOSITY("  variable " << var->name() << " OK");
      return retVal;
    } else {
      std::stringstream msg;
      msg << "Not enough modalities (";

      if (stringBag__.size() > 3) {
        msg << stringBag__.size() - 3;
      } else {
        msg << 0;
      }

      msg << ") declared for variable ";

      if (foo_flag__) {
        msg << stringBag__[0];
      } else {
        msg << "unknown";
      }

      resetParts__();

      states__.pop_back();
      GUM_ERROR(OperationNotAllowed, msg.str());
    }

    // For noisy compilers
    return 0;
  }

  // Tells the factory that we're declaring parents for some variable.
  // @var The concerned variable's name.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::startParentsDeclaration(
     const std::string& var) {
    if (state() != factory_state::NONE) {
      illegalStateError__("startParentsDeclaration");
    } else {
      checkVariableName__(var);
      std::vector< std::string >::iterator iter = stringBag__.begin();
      stringBag__.insert(iter, var);
      states__.push_back(factory_state::PARENTS);
    }

    VERBOSITY("starting parents for " << var);
  }

  // Tells the factory for which variable we're declaring parents.
  // @var The parent's name.
  // @throw NotFound Raised if var does not exists.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::addParent(const std::string& var) {
    if (state() != factory_state::PARENTS) {
      illegalStateError__("addParent");
    } else {
      checkVariableName__(var);
      stringBag__.push_back(var);
    }
  }

  // Tells the factory that we've finished declaring parents for some
  // variable. When parents exist, endParentsDeclaration creates some arcs.
  // These arcs are created in the inverse order of the order of the parent
  // specification.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::endParentsDeclaration() {
    if (state() != factory_state::PARENTS) {
      illegalStateError__("endParentsDeclaration");
    } else {
      NodeId id = varNameMap__[stringBag__[0]];

      // PLEASE NOTE THAT THE ORDER IS INVERSE

      for (size_t i = stringBag__.size() - 1; i > 0; --i) {
        bn__->addArc(varNameMap__[stringBag__[i]], id);
        VERBOSITY("  adding parent " << stringBag__[i] << " for "
                                     << stringBag__[0]);
      }

      resetParts__();

      states__.pop_back();
    }

    VERBOSITY("end of parents for " << stringBag__[0]);
  }

  // Tells the factory that we're declaring a conditional probability table
  // for some variable.
  // @param var The concerned variable's name.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::startRawProbabilityDeclaration(
     const std::string& var) {
    if (state() != factory_state::NONE) {
      illegalStateError__("startRawProbabilityDeclaration");
    } else {
      checkVariableName__(var);
      stringBag__.push_back(var);
      states__.push_back(factory_state::RAW_CPT);
    }

    VERBOSITY("  cpt starting for " << var);
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
  INLINE void BayesNetFactory< GUM_SCALAR >::rawConditionalTable(
     const std::vector< std::string >& variables,
     const std::vector< float >&       rawTable) {
    if (state() != factory_state::RAW_CPT) {
      illegalStateError__("rawConditionalTable");
    } else {
      fillProbaWithValuesTable__(variables, rawTable);
    }
  }

  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::fillProbaWithValuesTable__(
     const std::vector< std::string >& variables,
     const std::vector< float >&       rawTable) {
    const Potential< GUM_SCALAR >& table = bn__->cpt(varNameMap__[stringBag__[0]]);
    Instantiation                  cptInst(table);

    List< const DiscreteVariable* > varList;

    for (size_t i = 0; i < variables.size(); ++i) {
      varList.pushBack(&(bn__->variable(varNameMap__[variables[i]])));
    }

    // varList.pushFront(&(bn__->variable(varNameMap__[stringBag__[0]])));

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
    } while (increment__(modCounter, varList));
  }

  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::rawConditionalTable(
     const std::vector< float >& rawTable) {
    if (state() != factory_state::RAW_CPT) {
      illegalStateError__("rawConditionalTable");
    } else {
      fillProbaWithValuesTable__(rawTable);
    }
  }

  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::fillProbaWithValuesTable__(
     const std::vector< float >& rawTable) {
    const Potential< GUM_SCALAR >& table = bn__->cpt(varNameMap__[stringBag__[0]]);

    Instantiation cptInst(table);

    // the main loop is on the first variables. The others are in the right
    // order.
    const DiscreteVariable& first = table.variable(0);
    Idx                     j = 0;

    for (cptInst.setFirstVar(first); !cptInst.end(); cptInst.incVar(first)) {
      for (cptInst.setFirstNotVar(first); !cptInst.end(); cptInst.incNotVar(first))
        table.set(cptInst,
                  (j < rawTable.size()) ? (GUM_SCALAR)rawTable[j++]
                                        : (GUM_SCALAR)0);

      cptInst.unsetEnd();
    }
  }

  template < typename GUM_SCALAR >
  INLINE bool BayesNetFactory< GUM_SCALAR >::increment__(
     std::vector< gum::Idx >&         modCounter,
     List< const DiscreteVariable* >& varList) {
    bool last = true;

    for (NodeId j = 0; j < modCounter.size(); j++) {
      last = (modCounter[j] == (varList[j]->domainSize() - 1)) && last;

      if (!last) break;
    }

    if (last) { return false; }

    bool add = false;

    NodeId i = NodeId(varList.size() - 1);

    do {
      if (modCounter[i] == (varList[i]->domainSize() - 1)) {
        modCounter[i] = 0;
        add = true;
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
      illegalStateError__("endRawProbabilityDeclaration");
    } else {
      resetParts__();
      states__.pop_back();
    }

    VERBOSITY("  cpt ending for " << stringBag__[0]);
  }

  // Tells the factory that we're starting a factorized declaration.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::startFactorizedProbabilityDeclaration(
     const std::string& var) {
    if (state() != factory_state::NONE) {
      illegalStateError__("startFactorizedProbabilityDeclaration");
    } else {
      checkVariableName__(var);
      std::vector< std::string >::iterator iter = stringBag__.begin();
      stringBag__.insert(iter, var);
      states__.push_back(factory_state::FACT_CPT);
    }
  }

  // Tells the factory that we start an entry of a factorized conditional
  // probability table.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::startFactorizedEntry() {
    if (state() != factory_state::FACT_CPT) {
      illegalStateError__("startFactorizedEntry");
    } else {
      parents__ = new Instantiation();
      states__.push_back(factory_state::FACT_ENTRY);
    }
  }

  // Tells the factory that we finished declaring a conditional probability
  // table.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::endFactorizedEntry() {
    if (state() != factory_state::FACT_ENTRY) {
      illegalStateError__("endFactorizedEntry");
    } else {
      delete parents__;
      parents__ = 0;
      states__.pop_back();
    }
  }

  // Tells the factory on which modality we want to instantiate one of
  // variable's parent.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::setParentModality(
     const std::string& parent, const std::string& modality) {
    if (state() != factory_state::FACT_ENTRY) {
      illegalStateError__("string");
    } else {
      checkVariableName__(parent);
      Idx id = checkVariableModality__(parent, modality);
      (*parents__) << bn__->variable(varNameMap__[parent]);
      parents__->chgVal(bn__->variable(varNameMap__[parent]), id);
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
  // as for rawProba, if value's size is different than the number of modalities
  // of
  // the current variable,
  // we don't use the supplementary values and we fill by 0 the missign values.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::setVariableValuesUnchecked(
     const std::vector< float >& values) {
    if (state() != factory_state::FACT_ENTRY) {
      illegalStateError__("setVariableValues");
    } else {
      const DiscreteVariable& var = bn__->variable(varNameMap__[stringBag__[0]]);
      NodeId                  varId = varNameMap__[stringBag__[0]];

      if (parents__->domainSize() > 0) {
        Instantiation inst(bn__->cpt(varNameMap__[var.name()]));
        inst.setVals(*parents__);
        // Creating an instantiation containing all the variables not ins
        // parents__.
        Instantiation inst_default;
        inst_default << var;

        for (auto node: bn__->parents(varId)) {
          if (!parents__->contains(bn__->variable(node))) {
            inst_default << bn__->variable(node);
          }
        }

        // Filling the variable's table.
        for (inst.setFirstIn(inst_default); !inst.end();
             inst.incIn(inst_default)) {
          (bn__->cpt(varId))
             .set(inst,
                  inst.val(var) < values.size() ? (GUM_SCALAR)values[inst.val(var)]
                                                : (GUM_SCALAR)0);
        }
      } else {
        Instantiation inst(bn__->cpt(varNameMap__[var.name()]));
        Instantiation var_inst;
        var_inst << var;

        for (var_inst.setFirst(); !var_inst.end(); ++var_inst) {
          inst.setVals(var_inst);

          for (inst.setFirstOut(var_inst); !inst.end(); inst.incOut(var_inst)) {
            (bn__->cpt(varId))
               .set(inst,
                    inst.val(var) < values.size()
                       ? (GUM_SCALAR)values[inst.val(var)]
                       : (GUM_SCALAR)0);
          }
        }
      }
    }
  }

  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::setVariableValues(
     const std::vector< float >& values) {
    if (state() != factory_state::FACT_ENTRY) {
      illegalStateError__("setVariableValues");
    } else {
      const DiscreteVariable& var = bn__->variable(varNameMap__[stringBag__[0]]);
      //     Checking consistency between values and var.

      if (values.size() != var.domainSize()) {
        GUM_ERROR(OperationNotAllowed,
                  var.name()
                     << " : invalid number of modalities: found " << values.size()
                     << " while needed " << var.domainSize());
      }

      setVariableValuesUnchecked(values);
    }
  }

  // Tells the factory that we finished declaring a conditional probability
  // table.
  template < typename GUM_SCALAR >
  INLINE void
     BayesNetFactory< GUM_SCALAR >::endFactorizedProbabilityDeclaration() {
    if (state() != factory_state::FACT_CPT) {
      illegalStateError__("endFactorizedProbabilityDeclaration");
    } else {
      resetParts__();
      states__.pop_back();
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
  INLINE void
     BayesNetFactory< GUM_SCALAR >::setVariable(const DiscreteVariable& var) {
    if ((state() != factory_state::NONE)) {
      illegalStateError__("setVariable");
    } else {
      try {
        checkVariableName__(var.name());
        GUM_ERROR(DuplicateElement, "Name already used: " << var.name());
      } catch (NotFound&) {
        // The var name is unused
        varNameMap__.insert(var.name(), bn__->add(var));
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
  // @param redefineParents If true redefine var's parents to match table's
  //                        variables set.
  //
  // @throw NotFound Raised if no variable matches var.
  // @throw OperationNotAllowed Raised if redefineParents == false and if table
  //                            is not a valid CPT for var in the current state
  //                            of the BayesNet.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::setVariableCPT(
     const std::string& varName, MultiDimAdressable* table, bool redefineParents) {
    auto pot = dynamic_cast< Potential< GUM_SCALAR >* >(table);

    if (state() != factory_state::NONE) {
      illegalStateError__("setVariableCPT");
    } else {
      checkVariableName__(varName);
      const DiscreteVariable& var = bn__->variable(varNameMap__[varName]);
      NodeId                  varId = varNameMap__[varName];
      // If we have to change the structure of the BayesNet, then we call a sub
      // method.

      if (redefineParents) {
        setCPTAndParents__(var, pot);
      } else if (pot->contains(var)) {
        for (auto node: bn__->parents(varId)) {
          if (!pot->contains(bn__->variable(node))) {
            GUM_ERROR(OperationNotAllowed,
                      "The CPT is not valid in the current BayesNet.");
          }
        }

        // CPT are created when a variable is added.
        bn__->unsafeChangePotential_(varId, pot);
      }
    }
  }

  // Copy operator is illegal, use only copy constructor.
  template < typename GUM_SCALAR >
  INLINE BayesNetFactory< GUM_SCALAR >& BayesNetFactory< GUM_SCALAR >::operator=(
     const BayesNetFactory< GUM_SCALAR >& source) {
    GUM_ERROR(OperationNotAllowed, "Illegal!");
    // For noisy compilers
    return *this;
  }

  // Raise an OperationNotAllowed with the message "Illegal state."
  template < typename GUM_SCALAR >
  INLINE void
     BayesNetFactory< GUM_SCALAR >::illegalStateError__(const std::string& s) {
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

    GUM_ERROR(OperationNotAllowed, msg);
  }

  // Check if a variable with the given name exists, if not raise an NotFound
  // exception.
  template < typename GUM_SCALAR >
  INLINE void
     BayesNetFactory< GUM_SCALAR >::checkVariableName__(const std::string& name) {
    if (!varNameMap__.exists(name)) { GUM_ERROR(NotFound, name); }
  }

  // Check if var exists and if mod is one of it's modality, if not raise an
  // NotFound exception.
  template < typename GUM_SCALAR >
  INLINE Idx BayesNetFactory< GUM_SCALAR >::checkVariableModality__(
     const std::string& name, const std::string& mod) {
    checkVariableName__(name);
    const DiscreteVariable& var = bn__->variable(varNameMap__[name]);

    for (Idx i = 0; i < var.domainSize(); ++i) {
      if (mod == var.label(i)) { return i; }
    }

    GUM_ERROR(NotFound, mod);
  }

  // Check if in stringBag__ there is no other modality with the same name.
  template < typename GUM_SCALAR >
  INLINE void
     BayesNetFactory< GUM_SCALAR >::checkModalityInBag__(const std::string& mod) {
    for (size_t i = 3; i < stringBag__.size(); ++i) {
      if (mod == stringBag__[i]) {
        GUM_ERROR(DuplicateElement, "Label already used: " << mod);
      }
    }
  }

  // Sub method of setVariableCPT() which redefine the BayesNet's DAG with
  // respect to table.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::setCPTAndParents__(
     const DiscreteVariable& var, Potential< GUM_SCALAR >* table) {
    NodeId varId = varNameMap__[var.name()];
    bn__->dag_.eraseParents(varId);

    for (auto v: table->variablesSequence()) {
      if (v != (&var)) {
        checkVariableName__(v->name());
        bn__->dag_.addArc(varNameMap__[v->name()], varId);
      }
    }

    // CPT are created when a variable is added.
    bn__->unsafeChangePotential_(varId, table);
  }

  // Reset the different parts used to constructed the BayesNet.
  template < typename GUM_SCALAR >
  INLINE void BayesNetFactory< GUM_SCALAR >::resetParts__() {
    foo_flag__ = false;
    bar_flag__ = false;
    stringBag__.clear();
  }
} /* namespace gum */