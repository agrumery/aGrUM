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
 * @brief Implementation of the BayesNetFactory class.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <agrum/BN/BayesNetFactory.h>

#define VERBOSITY(x)                                                                \
  {                                                                                 \
    if (isVerbose())                                                                \
      std::cerr << "[BN factory] " << x << std::endl;                               \
  }

namespace gum {

  // Default constructor.
  // @param bn A pointer over the BayesNet filled by this factory.
  // @throw DuplicateElement Raised if two variables in bn share the same
  //                         name.
  template <typename GUM_SCALAR>
  INLINE BayesNetFactory<GUM_SCALAR>::BayesNetFactory(BayesNet<GUM_SCALAR> *bn)
      : __parents(0), __impl(0), __bn(bn) {
    GUM_CONSTRUCTOR(BayesNetFactory);
    __states.push_back(factory_state::NONE);

    for (auto node : bn->nodes()) {
      if (__varNameMap.exists(bn->variable(node).name()))
        GUM_ERROR(DuplicateElement, bn->variable(node).name());

      __varNameMap.insert(bn->variable(node).name(), node);
    }

    resetVerbose();
  }

  // Copy constructor.
  // The copy will have an exact copy of the constructed BayesNet in source.
  template <typename GUM_SCALAR>
  INLINE BayesNetFactory<GUM_SCALAR>::BayesNetFactory(
      const BayesNetFactory<GUM_SCALAR> &source)
      : __parents(0), __impl(0), __bn(0) {
    GUM_CONS_CPY(BayesNetFactory);

    if (source.state() != factory_state::NONE) {
      GUM_ERROR(OperationNotAllowed, "Illegal state to proceed make a copy.");
    } else {
      __states = source.__states;
      __bn = new BayesNet<GUM_SCALAR>(*(source.__bn));
    }
  }

  // Destructor
  template <typename GUM_SCALAR>
  INLINE BayesNetFactory<GUM_SCALAR>::~BayesNetFactory() {
    GUM_DESTRUCTOR(BayesNetFactory);

    if (__parents != 0)
      delete __parents;

    if (__impl != 0) {
      GUM_ERROR(FatalError, "Implementation defined for a variable but not used. "
                            "You should call endVariableDeclaration() before "
                            "deleting me.");
    }
  }

  // Returns the BayesNet created by this factory.
  template <typename GUM_SCALAR>
  INLINE BayesNet<GUM_SCALAR> *BayesNetFactory<GUM_SCALAR>::bayesNet() {

    return __bn;
  }

  template <typename GUM_SCALAR>
  INLINE const DiscreteVariable &BayesNetFactory<GUM_SCALAR>::varInBN(NodeId id) {
    return __bn->variable(id);
  }

  // Returns the current state of the factory.
  template <typename GUM_SCALAR>
  INLINE IBayesNetFactory::factory_state BayesNetFactory<GUM_SCALAR>::state() const {
    // This is ok because there is alway at least the state NONE in the stack.
    return __states.back();
  }

  // Returns the NodeId of a variable given it's name.
  // @throw NotFound Raised if no variable matches the name.
  template <typename GUM_SCALAR>
  INLINE NodeId
      BayesNetFactory<GUM_SCALAR>::variableId(const std::string &name) const {
    try {
      return __varNameMap[name];
    } catch (NotFound &e) {
      GUM_ERROR(NotFound, name);
    }
  }

  // Returns a constant reference on a variable given it's name.
  // @throw NotFound Raised if no variable matches the name.
  template <typename GUM_SCALAR>
  INLINE const DiscreteVariable &
  BayesNetFactory<GUM_SCALAR>::variable(const std::string &name) const {
    try {
      return __bn->variable(variableId(name));
    } catch (NotFound &) {
      GUM_ERROR(NotFound, name);
    }
  }

  // Returns the domainSize of the cpt for the node n.
  // @throw NotFound raised if no such NodeId exists.
  // @throw OperationNotAllowed if there is no bayesian networks.
  template <typename GUM_SCALAR>
  INLINE Size BayesNetFactory<GUM_SCALAR>::cptDomainSize(const NodeId n) const {
    // (from PH) nowhere in the code, I see a check type if (__bn) __bn->. I assume
    // __bn is forced not to be nullptr ...
    return __bn->cpt(n).domainSize();
  }

  // Tells the factory that we're in a network declaration.
  template <typename GUM_SCALAR>
  INLINE void BayesNetFactory<GUM_SCALAR>::startNetworkDeclaration() {
    if (state() != factory_state::NONE) {
      __illegalStateError("startNetworkDeclaration");
    } else {
      __states.push_back(factory_state::NETWORK);
    }

    VERBOSITY("starting network");
  }

  // Tells the factory to add a property to the current network.
  template <typename GUM_SCALAR>
  INLINE void
  BayesNetFactory<GUM_SCALAR>::addNetworkProperty(const std::string &propName,
                                                  const std::string &propValue) {
    __bn->setProperty(propName, propValue);
  }

  // Tells the factory that we're out of a network declaration.
  template <typename GUM_SCALAR>
  INLINE void BayesNetFactory<GUM_SCALAR>::endNetworkDeclaration() {
    if (state() != factory_state::NETWORK) {
      __illegalStateError("endNetworkDeclaration");
    } else {
      __states.pop_back();
    }

    VERBOSITY("network OK");
  }

  // Tells the factory that we're in a variable declaration.
  template <typename GUM_SCALAR>
  INLINE void BayesNetFactory<GUM_SCALAR>::startVariableDeclaration() {
    if (state() != factory_state::NONE) {
      __illegalStateError("startVariableDeclaration");
    } else {
      __states.push_back(factory_state::VARIABLE);
      __stringBag.push_back("name");
      __stringBag.push_back("desc");
    }

    VERBOSITY("  starting variable");
  }

  // Tells the factory the current variable's name.
  template <typename GUM_SCALAR>
  INLINE void BayesNetFactory<GUM_SCALAR>::variableName(const std::string &name) {
    if (state() != factory_state::VARIABLE) {
      __illegalStateError("variableName");
    } else {
      if (__varNameMap.exists(name)) {
        std::string str = "Name already used: ";
        GUM_ERROR(DuplicateElement, str + name);
      }

      __foo_flag = true;

      __stringBag[0] = name;
      VERBOSITY("  -- variable " << name);
    }
  }

  // Tells the factory the current variable's description.
  template <typename GUM_SCALAR>
  INLINE void
  BayesNetFactory<GUM_SCALAR>::variableDescription(const std::string &desc) {
    if (state() != factory_state::VARIABLE) {
      __illegalStateError("variableDescription");
    } else {
      __bar_flag = true;
      __stringBag[1] = desc;
    }
  }

  // Adds a modality to the current variable.
  // @throw DuplicateElement If the current variable already has a modality
  //                         with the same name.
  template <typename GUM_SCALAR>
  INLINE void BayesNetFactory<GUM_SCALAR>::addModality(const std::string &name) {
    if (state() != factory_state::VARIABLE) {
      __illegalStateError("addModality");
    } else {
      __checkModalityInBag(name);
      __stringBag.push_back(name);
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
  template <typename GUM_SCALAR>
  INLINE void BayesNetFactory<GUM_SCALAR>::setVariableCPTImplementation(
      MultiDimAdressable *adressable) {
    MultiDimImplementation<GUM_SCALAR> *impl =
        dynamic_cast<MultiDimImplementation<GUM_SCALAR> *>(adressable);

    if (state() != factory_state::VARIABLE) {
      __illegalStateError("setVariableCPTImplementation");
    } else {
      if (impl == 0) {
        GUM_ERROR(OperationNotAllowed,
                  "An implementation for this variable is already "
                  "defined.");
      } else if (impl->nbrDim() > 0) {
        GUM_ERROR(OperationNotAllowed, "This implementation is not empty.");
      }

      __impl = impl;
    }
  }

  // Tells the factory that we're out of a variable declaration.
  template <typename GUM_SCALAR>
  INLINE NodeId BayesNetFactory<GUM_SCALAR>::endVariableDeclaration() {
    if (state() != factory_state::VARIABLE) {
      __illegalStateError("endVariableDeclaration");
    } else if (__foo_flag and (__stringBag.size() > 3)) {
      LabelizedVariable *var = new LabelizedVariable(
          __stringBag[0], (__bar_flag) ? __stringBag[1] : "", 0);

      for (size_t i = 2; i < __stringBag.size(); ++i) {
        var->addLabel(__stringBag[i]);
      }

      if (__impl != 0) {
        __varNameMap.insert(var->name(), __bn->add(*var, __impl));
        __impl = 0;
      } else {
        __varNameMap.insert(var->name(), __bn->add(*var));
      }

      NodeId retVal = __varNameMap[var->name()];

      delete var;

      __resetParts();
      __states.pop_back();

      VERBOSITY("  variable " << var->name() << " OK");
      return retVal;
    } else {
      std::stringstream msg;
      msg << "Not enough modalities (";

      if (__stringBag.size() > 2) {
        msg << __stringBag.size() - 2;
      } else {
        msg << 0;
      }

      msg << ") declared for variable ";

      if (__foo_flag) {
        msg << __stringBag[0];
      } else {
        msg << "unknown";
      }

      __resetParts();

      __states.pop_back();
      GUM_ERROR(OperationNotAllowed, msg.str());
    }

    // For noisy compilers
    return 0;
  }

  // Tells the factory that we're declaring parents for some variable.
  // @var The concerned variable's name.
  template <typename GUM_SCALAR>
  INLINE void
  BayesNetFactory<GUM_SCALAR>::startParentsDeclaration(const std::string &var) {
    if (state() != factory_state::NONE) {
      __illegalStateError("startParentsDeclaration");
    } else {
      __checkVariableName(var);
      std::vector<std::string>::iterator iter = __stringBag.begin();
      __stringBag.insert(iter, var);
      __states.push_back(factory_state::PARENTS);
    }

    VERBOSITY("starting parents for " << var);
  }

  // Tells the factory for which variable we're declaring parents.
  // @var The parent's name.
  // @throw NotFound Raised if var does not exists.
  template <typename GUM_SCALAR>
  INLINE void BayesNetFactory<GUM_SCALAR>::addParent(const std::string &var) {
    if (state() != factory_state::PARENTS) {
      __illegalStateError("addParent");
    } else {
      __checkVariableName(var);
      __stringBag.push_back(var);
    }
  }

  // Tells the factory that we've finished declaring parents for some
  // variable. When parents exist, endParentsDeclaration creates some arcs. These
  // arcs
  // are created in the inverse order of the order of the parent specification.
  // @throw UndefinedElement Raised if the variable isn't defined (or not
  //                         enough defined).
  template <typename GUM_SCALAR>
  INLINE void BayesNetFactory<GUM_SCALAR>::endParentsDeclaration() {
    if (state() != factory_state::PARENTS) {
      __illegalStateError("endParentsDeclaration");
    } else {
      NodeId id = __varNameMap[__stringBag[0]];

      // PLEASE NOTE THAT THE ORDER IS INVERSE

      for (size_t i = __stringBag.size() - 1; i > 0; --i) {
        __bn->addArc(__varNameMap[__stringBag[i]], id);
        VERBOSITY("  adding parent " << __stringBag[i] << " for " << __stringBag[0]);
      }

      __resetParts();

      __states.pop_back();
    }

    VERBOSITY("end of parents for " << __stringBag[0]);
  }

  // Tells the factory that we're declaring a conditional probability table
  // for some variable.
  // @param var The concerned variable's name.
  template <typename GUM_SCALAR>
  INLINE void BayesNetFactory<GUM_SCALAR>::startRawProbabilityDeclaration(
      const std::string &var) {
    if (state() != factory_state::NONE) {
      __illegalStateError("startRawProbabilityDeclaration");
    } else {
      __checkVariableName(var);
      __stringBag.push_back(var);
      __states.push_back(factory_state::RAW_CPT);
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
  template <typename GUM_SCALAR>
  INLINE void BayesNetFactory<GUM_SCALAR>::rawConditionalTable(
      const std::vector<std::string> &variables,
      const std::vector<float> &rawTable) {
    if (state() != factory_state::RAW_CPT) {
      __illegalStateError("rawConditionalTable");
    } else {
      __fillProbaWithValuesTable(variables, rawTable);
    }
  }

  template <typename GUM_SCALAR>
  INLINE void BayesNetFactory<GUM_SCALAR>::__fillProbaWithValuesTable(
      const std::vector<std::string> &variables,
      const std::vector<float> &rawTable) {
    const Potential<GUM_SCALAR> &table = __bn->cpt(__varNameMap[__stringBag[0]]);
    Instantiation cptInst(table);

    List<const DiscreteVariable *> varList;

    for (size_t i = 0; i < variables.size(); ++i) {
      varList.pushBack(&(__bn->variable(__varNameMap[variables[i]])));
    }

    // varList.pushFront(&(__bn->variable(__varNameMap[__stringBag[0]])));

    Idx nbrVar = varList.size();

    std::vector<Idx> modCounter;

    // initializing the array
    for (Idx i = 0; i < nbrVar; i++) {
      modCounter.push_back(0);
    }

    Idx j = 0;

    do {
      for (Idx i = 0; i < nbrVar; i++) {
        cptInst.chgVal(*(varList[i]), modCounter[i]);
      }

      if (j < rawTable.size()) {
        table.set(cptInst, (GUM_SCALAR)rawTable[j]);
      } else {
        table.set(cptInst, (GUM_SCALAR)0);
      }

      j++;
    } while (__increment(modCounter, varList));
  }

  template <typename GUM_SCALAR>
  INLINE void BayesNetFactory<GUM_SCALAR>::rawConditionalTable(
      const std::vector<float> &rawTable) {
    if (state() != factory_state::RAW_CPT) {
      __illegalStateError("rawConditionalTable");
    } else {
      __fillProbaWithValuesTable(rawTable);
    }
  }

  template <typename GUM_SCALAR>
  INLINE void BayesNetFactory<GUM_SCALAR>::__fillProbaWithValuesTable(
      const std::vector<float> &rawTable) {
    const Potential<GUM_SCALAR> &table = __bn->cpt(__varNameMap[__stringBag[0]]);

    Instantiation cptInst(table);

    // the main loop is on the first variables. The others are in the right order.
    const DiscreteVariable &first = table.variable(0);
    Idx j = 0;

    for (cptInst.setFirstVar(first); !cptInst.end(); cptInst.incVar(first)) {
      for (cptInst.setFirstNotVar(first); !cptInst.end(); cptInst.incNotVar(first))
        table.set(cptInst,
                  (j < rawTable.size()) ? (GUM_SCALAR)rawTable[j++] : (GUM_SCALAR)0);

      cptInst.unsetEnd();
    }
  }

  template <typename GUM_SCALAR>
  INLINE bool
  BayesNetFactory<GUM_SCALAR>::__increment(std::vector<gum::Idx> &modCounter,
                                           List<const DiscreteVariable *> &varList) {
    bool last = true;

    for (unsigned int j = 0; j < modCounter.size(); j++) {
      last = (modCounter[j] == (varList[j]->domainSize() - 1)) && last;

      if (!last)
        break;
    }

    if (last) {
      return false;
    }

    bool add = false;

    unsigned int i = varList.size() - 1;

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
  template <typename GUM_SCALAR>
  INLINE void BayesNetFactory<GUM_SCALAR>::endRawProbabilityDeclaration() {
    if (state() != factory_state::RAW_CPT) {
      __illegalStateError("endRawProbabilityDeclaration");
    } else {
      __resetParts();
      __states.pop_back();
    }

    VERBOSITY("  cpt ending for " << __stringBag[0]);
  }

  // Tells the factory that we're starting a factorized declaration.
  template <typename GUM_SCALAR>
  INLINE void BayesNetFactory<GUM_SCALAR>::startFactorizedProbabilityDeclaration(
      const std::string &var) {
    if (state() != factory_state::NONE) {
      __illegalStateError("startFactorizedProbabilityDeclaration");
    } else {
      __checkVariableName(var);
      std::vector<std::string>::iterator iter = __stringBag.begin();
      __stringBag.insert(iter, var);
      __states.push_back(factory_state::FACT_CPT);
    }
  }

  // Tells the factory that we start an entry of a factorized conditional
  // probability table.
  template <typename GUM_SCALAR>
  INLINE void BayesNetFactory<GUM_SCALAR>::startFactorizedEntry() {
    if (state() != factory_state::FACT_CPT) {
      __illegalStateError("startFactorizedEntry");
    } else {
      __parents = new Instantiation();
      __states.push_back(factory_state::FACT_ENTRY);
    }
  }

  // Tells the factory that we finished declaring a conditional probability
  // table.
  template <typename GUM_SCALAR>
  INLINE void BayesNetFactory<GUM_SCALAR>::endFactorizedEntry() {
    if (state() != factory_state::FACT_ENTRY) {
      __illegalStateError("endFactorizedEntry");
    } else {
      delete __parents;
      __parents = 0;
      __states.pop_back();
    }
  }

  // Tells the factory on which modality we want to instantiate one of
  // variable's parent.
  template <typename GUM_SCALAR>
  INLINE void
  BayesNetFactory<GUM_SCALAR>::setParentModality(const std::string &parent,
                                                 const std::string &modality) {
    if (state() != factory_state::FACT_ENTRY) {
      __illegalStateError("string");
    } else {
      __checkVariableName(parent);
      Idx id = __checkVariableModality(parent, modality);
      (*__parents) << __bn->variable(__varNameMap[parent]);
      __parents->chgVal(__bn->variable(__varNameMap[parent]), id);
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
  // as for rawProba, if value's size is different than the number of modalities of
  // the current variable,
  // we don't use the supplementary values and we fill by 0 the missign values.
  template <typename GUM_SCALAR>
  INLINE void BayesNetFactory<GUM_SCALAR>::setVariableValuesUnchecked(
      const std::vector<float> &values) {
    if (state() != factory_state::FACT_ENTRY) {
      __illegalStateError("setVariableValues");
    } else {
      const DiscreteVariable &var = __bn->variable(__varNameMap[__stringBag[0]]);
      Idx varId = __varNameMap[__stringBag[0]];

      if (__parents->domainSize() > 0) {
        Instantiation inst(__bn->cpt(__varNameMap[var.name()]));
        inst.setVals(*__parents);
        // Creating an instantiation containing all the variables not ins __parents.
        Instantiation inst_default;
        inst_default << var;

        for (auto node : __bn->dag().parents(varId)) {
          if (!__parents->contains(__bn->variable(node))) {
            inst_default << __bn->variable(node);
          }
        }

        // Filling the variable's table.
        for (inst.setFirstIn(inst_default); !inst.end(); inst.incIn(inst_default)) {
          (__bn->cpt(varId)).set(inst, inst.val(var) < values.size()
                                           ? (GUM_SCALAR)values[inst.val(var)]
                                           : (GUM_SCALAR)0);
        }
      } else {
        Instantiation inst(__bn->cpt(__varNameMap[var.name()]));
        Instantiation var_inst;
        var_inst << var;

        for (var_inst.setFirst(); !var_inst.end(); ++var_inst) {
          inst.setVals(var_inst);

          for (inst.setFirstOut(var_inst); !inst.end(); inst.incOut(var_inst)) {
            (__bn->cpt(varId)).set(inst, inst.val(var) < values.size()
                                             ? (GUM_SCALAR)values[inst.val(var)]
                                             : (GUM_SCALAR)0);
          }
        }
      }
    }
  }

  template <typename GUM_SCALAR>
  INLINE void
  BayesNetFactory<GUM_SCALAR>::setVariableValues(const std::vector<float> &values) {
    if (state() != factory_state::FACT_ENTRY) {
      __illegalStateError("setVariableValues");
    } else {
      const DiscreteVariable &var = __bn->variable(__varNameMap[__stringBag[0]]);
      //     Checking consistency between values and var.

      if (values.size() != var.domainSize()) {
        GUM_ERROR(OperationNotAllowed,
                  var.name() << " : invalid number of modalities: found "
                             << values.size() << " while needed "
                             << var.domainSize());
      }

      setVariableValuesUnchecked(values);
    }
  }

  // Tells the factory that we finished declaring a conditional probability
  // table.
  template <typename GUM_SCALAR>
  INLINE void BayesNetFactory<GUM_SCALAR>::endFactorizedProbabilityDeclaration() {
    if (state() != factory_state::FACT_CPT) {
      __illegalStateError("endFactorizedProbabilityDeclaration");
    } else {
      __resetParts();
      __states.pop_back();
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
  template <typename GUM_SCALAR>
  INLINE void BayesNetFactory<GUM_SCALAR>::setVariable(const DiscreteVariable &var) {
    if ((state() != factory_state::NONE)) {
      __illegalStateError("setVariable");
    } else {
      try {
        __checkVariableName(var.name());
        GUM_ERROR(DuplicateElement, var.name());
      } catch (NotFound &) {
        // The var name is unused
        __varNameMap.insert(var.name(), __bn->add(var));
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
  template <typename GUM_SCALAR>
  INLINE void BayesNetFactory<GUM_SCALAR>::setVariableCPT(const std::string &varName,
                                                          MultiDimAdressable *adr,
                                                          bool redefineParents) {
    Potential<GUM_SCALAR> *table = dynamic_cast<Potential<GUM_SCALAR> *>(adr);

    if (state() != factory_state::NONE) {
      __illegalStateError("setVariableCPT");
    } else {
      __checkVariableName(varName);
      const DiscreteVariable &var = __bn->variable(__varNameMap[varName]);
      NodeId varId = __varNameMap[varName];
      // If we have to change the structure of the BayesNet, then we call a sub
      // method.

      if (redefineParents) {
        __setCPTAndParents(var, table);
      } else if (table->contains(var)) {
        for (auto node : __bn->dag().parents(varId)) {
          if (!table->contains(__bn->variable(node))) {
            GUM_ERROR(OperationNotAllowed,
                      "The CPT is not valid in the current BayesNet.");
          }
        }

        // CPT are created when a variable is added.
        __bn->_unsafeChangePotential(varId, table);
      }
    }
  }

  // Copy operator is illegal, use only copy constructor.
  template <typename GUM_SCALAR>
  INLINE BayesNetFactory<GUM_SCALAR> &BayesNetFactory<GUM_SCALAR>::
  operator=(const BayesNetFactory<GUM_SCALAR> &source) {
    GUM_ERROR(OperationNotAllowed, "Illegal!");
    // For noisy compilers
    return *this;
  }

  // Raise an OperationNotAllowed with the message "Illegal state."
  template <typename GUM_SCALAR>
  INLINE void
  BayesNetFactory<GUM_SCALAR>::__illegalStateError(const std::string &s) {
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

      default: { msg += "Unknown state"; }
    }

    GUM_ERROR(OperationNotAllowed, msg);
  }

  // Check if a variable with the given name exists, if not raise an NotFound
  // exception.
  template <typename GUM_SCALAR>
  INLINE void
  BayesNetFactory<GUM_SCALAR>::__checkVariableName(const std::string &name) {
    if (!__varNameMap.exists(name)) {
      GUM_ERROR(NotFound, name);
    }
  }

  // Check if var exists and if mod is one of it's modality, if not raise an
  // NotFound exception.
  template <typename GUM_SCALAR>
  INLINE Idx
      BayesNetFactory<GUM_SCALAR>::__checkVariableModality(const std::string &name,
                                                           const std::string &mod) {
    __checkVariableName(name);
    const DiscreteVariable &var = __bn->variable(__varNameMap[name]);

    for (Idx i = 0; i < var.domainSize(); ++i) {
      if (mod == var.label(i)) {
        return i;
      }
    }

    GUM_ERROR(NotFound, mod);
  }

  // Check if in __stringBag there is no other modality with the same name.
  template <typename GUM_SCALAR>
  INLINE void
  BayesNetFactory<GUM_SCALAR>::__checkModalityInBag(const std::string &mod) {
    for (size_t i = 2; i < __stringBag.size(); ++i) {
      if (mod == __stringBag[i]) {
        GUM_ERROR(DuplicateElement, mod);
      }
    }
  }

  // Sub method of setVariableCPT() which redefine the BayesNet's DAG with
  // respect to table.
  template <typename GUM_SCALAR>
  INLINE void
  BayesNetFactory<GUM_SCALAR>::__setCPTAndParents(const DiscreteVariable &var,
                                                  Potential<GUM_SCALAR> *table) {
    NodeId varId = __varNameMap[var.name()];
    __bn->_dag.eraseParents(varId);

    for (auto v : table->variablesSequence()) {
      if (v != (&var)) {
        __checkVariableName(v->name());
        __bn->_dag.addArc(__varNameMap[v->name()], varId);
      }
    }

    // CPT are created when a variable is added.
    __bn->_unsafeChangePotential(varId, table);
  }

  // Reset the different parts used to constructed the BayesNet.
  template <typename GUM_SCALAR>
  INLINE void BayesNetFactory<GUM_SCALAR>::__resetParts() {
    __foo_flag = false;
    __bar_flag = false;
    __stringBag.clear();
  }
} /* namespace gum */

// kate: indent-mode cstyle; indent-width 2; replace-tabs on;
