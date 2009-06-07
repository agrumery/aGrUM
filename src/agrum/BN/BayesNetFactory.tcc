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
 * @author Lionel Torti
 */
// ============================================================================

namespace gum {

// Default constructor.
template<typename T_DATA> INLINE
BayesNetFactory<T_DATA>::BayesNetFactory():
  __parents(0), __impl(0), __bn(new BayesNet<T_DATA>())
{
  GUM_CONSTRUCTOR( BayesNetFactory );
  __states.push_back(BayesNetFactory<T_DATA>::NONE);
}

// Default constructor.
// @param bn A pointer over the BayesNet filled by this factory.
// @throw DuplicateElement Raised if two variables in bn share the same
//                         name.
template<typename T_DATA> INLINE
BayesNetFactory<T_DATA>::BayesNetFactory(BayesNet<T_DATA>* bn):
  __parents(0), __impl(0), __bn(bn)
{
  GUM_CONSTRUCTOR( BayesNetFactory );
  __states.push_back(BayesNetFactory<T_DATA>::NONE);
  for (NodeSetIterator iter = bn->beginNodes(); iter != bn->endNodes(); ++iter) {
    if (__varNameMap.exists(bn->variable(*iter).name())) {
      GUM_ERROR(DuplicateElement, bn->variable(*iter).name());
    }
    __varNameMap.insert(bn->variable(*iter).name(), *iter);
  }
}

// Copy constructor.
// The copy will have an exact copy of the constructed BayesNet in source.
template<typename T_DATA> INLINE
BayesNetFactory<T_DATA>::BayesNetFactory(const BayesNetFactory<T_DATA>& source):
  __parents(0), __impl(0), __bn(0)
{
  GUM_CONS_CPY( BayesNetFactory );
  if (source.state() != BayesNetFactory<T_DATA>::NONE) {
    GUM_ERROR(OperationNotAllowed, "Illegal state to proceed make a copy.");
  } else {
    __states = source.__states;
    __bn = new BayesNet<T_DATA>(*(source.__bn));
  }
}

// Destructor
template<typename T_DATA> INLINE
BayesNetFactory<T_DATA>::~BayesNetFactory()
{
  GUM_DESTRUCTOR( BayesNetFactory );
  if (__parents != 0) delete __parents;
  if (__impl != 0) {
    GUM_ERROR(FatalError, "Implementation defined for a variable but not used. "
                          "You should call endVariableDeclaration() before "
                          "deleting me.");
  }
}

// Returns the BayesNet created by this factory.
template<typename T_DATA> INLINE
BayesNet<T_DATA>*
BayesNetFactory<T_DATA>::bayesNet()
{
  //if (state() != BayesNetFactory<T_DATA>::NONE) {
  //  GUM_ERROR(OperationNotAllowed, "Illegal state to return the BayesNet: it is not yet finished.");
  //}
  return __bn;
}

// Returns the current state of the factory.
template<typename T_DATA> INLINE
AbstractBayesNetFactory::factory_state
BayesNetFactory<T_DATA>::state() const
{
  // This is ok because there is alway at least the state NONE in the stack.
  return __states.back();
}

// Returns the NodeId of a variable given it's name.
// @throw NotFound Raised if no variable matches the name.
template<typename T_DATA> INLINE
NodeId
BayesNetFactory<T_DATA>::variableId(const std::string& name) const
{
  try {
    return __varNameMap[name];
  } catch (NotFound&) {
    GUM_ERROR(NotFound, name);
  }
}

// Tells the factory that we're in a network declaration.
template<typename T_DATA> INLINE
void
BayesNetFactory<T_DATA>::startNetworkDeclaration()
{
  if (state() != NONE) {
    __illegalStateError();
  } else {
    __states.push_back(BayesNetFactory<T_DATA>::NETWORK);
  }
}

// Tells the factory to add a property to the current network.
template<typename T_DATA> INLINE
void
BayesNetFactory<T_DATA>::addNetworkProperty(const std::string& propName,
                                            const std::string& propValue)
{
  __bn->setProperty(propName, propValue);
}

// Tells the factory that we're out of a network declaration.
template<typename T_DATA> INLINE
void
BayesNetFactory<T_DATA>::endNetworkDeclaration()
{
  if (state() != NETWORK) {
    __illegalStateError();
  } else {
    __states.pop_back();
  }
}

// Tells the factory that we're in a variable declaration.
template<typename T_DATA> INLINE
void
BayesNetFactory<T_DATA>::startVariableDeclaration()
{
  if (state() != NETWORK) {
    __illegalStateError();
  } else {
    __states.push_back(VARIABLE);
    __stringBag.push_back("name");
    __stringBag.push_back("desc");
  }
}

// Tells the factory the current variable's name.
template<typename T_DATA> INLINE
void
BayesNetFactory<T_DATA>::variableName(const std::string& name)
{
  if (state() != VARIABLE) {
    __illegalStateError();
  } else {
    if (__varNameMap.exists(name)) {
      std::string str = "Name already used: ";
      GUM_ERROR(DuplicateElement, str + name);
    }
    __foo_flag = true;
    __stringBag[0] = name;
  }
}

// Tells the factory the current variable's description.
template<typename T_DATA> INLINE
void
BayesNetFactory<T_DATA>::variableDescription(const std::string& desc)
{
  if (state() != VARIABLE) {
    __illegalStateError();
  } else {
    __bar_flag = true;
    __stringBag[1] = desc;
  }
}

// Adds a modality to the current variable.
// @throw DuplicateElement If the current variable already has a modality
//                         with the same name.
template<typename T_DATA> INLINE
void
BayesNetFactory<T_DATA>::addModality(const std::string& name)
{
  if (state() != VARIABLE) {
    __illegalStateError();
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
template<typename T_DATA> INLINE
void
BayesNetFactory<T_DATA>::setVariableCPTImplementation(MultiDimImplementation<T_DATA>* impl)
{
  if (state() != VARIABLE) {
    __illegalStateError();
  } else {
    if (__impl != 0) {
    GUM_ERROR(OperationNotAllowed, "An implementation for this variable is already "
                                   "defined.");
    } else if (__impl->nbrDim() > 0) {
      GUM_ERROR(OperationNotAllowed, "This implementation is not empty.");
    }
    __impl = impl;
  }
}

// Tells the factory that we're out of a variable declaration.
template<typename T_DATA> INLINE
void
BayesNetFactory<T_DATA>::endVariableDeclaration()
{
  if (state() != VARIABLE) {
    __illegalStateError();
  } else if ( (__foo_flag && (! __bar_flag) && __stringBag.size() > 2) ||
              (__foo_flag && __bar_flag && __stringBag.size() > 3) )
  {
    LabelizedVariable* var = 0;
    if (__bar_flag) {
      var = new LabelizedVariable(__stringBag[0], __stringBag[1], 0);
    } else {
      var = new LabelizedVariable(__stringBag[0], "", 0);
    }
    size_t i = (__foo_flag and __bar_flag)?2:1;
    while (i < __stringBag.size()) {
      var->addLabel(__stringBag[i++]);
    }
    if (__impl != 0) {
      __varNameMap.insert(var->name(), __bn->add(*var, __impl));
      __impl = 0;
    } else {
      __varNameMap.insert(var->name(), __bn->add(*var));
    }
    delete var;
    __resetParts();
    __states.pop_back();
  } else {
    GUM_ERROR(OperationNotAllowed, "Not enough elements defined to end variable "
                                   "declaration.");
  }
}

// Tells the factory that we're declaring parents for some variable.
// @var The concerned variable's name.
template<typename T_DATA> INLINE
void
BayesNetFactory<T_DATA>::startParentsDeclaration(const std::string& var)
{
  if (state() != NETWORK) {
    __illegalStateError();
  } else {
    __checkVariableName(var);
    std::vector<std::string>::iterator iter = __stringBag.begin();
    __stringBag.insert(iter, var);
    __states.push_back(PARENTS);
  }
}

// Tells the factory for which variable we're declaring parents.
// @var The parent's name.
// @throw NotFound Raised if var does not exists.
template<typename T_DATA> INLINE
void
BayesNetFactory<T_DATA>::addParent(const std::string& var)
{
  if (state() != PARENTS) {
    __illegalStateError();
  } else {
    __checkVariableName(var);
    __stringBag.push_back(var);
  }
}

// Tells the factory that we've finished declaring parents for some
// variable.
// @throw UndefinedElement Raised if the variable isn't defined (or not
//                         enough defined).
template<typename T_DATA> INLINE
void
BayesNetFactory<T_DATA>::endParentsDelclaration()
{
  if (state() != PARENTS) {
    __illegalStateError();
  } else {
    if (__foo_flag) {
      NodeId id = __varNameMap[__stringBag[0]];
      for (size_t i = 1; i < __stringBag.size(); ++i) {
        __bn->insertArc(__varNameMap[__stringBag[i]], id);
      }
    }
    __resetParts();
    __states.pop_back();
  }
}

// Tells the factory that we're declaring a conditional probability table
// for some variable.
// @param var The concerned variable's name.
template<typename T_DATA> INLINE
void
BayesNetFactory<T_DATA>::startRawProbabilityDeclaration(const std::string& var)
{
  if (state() != NETWORK) {
    __illegalStateError();
  } else {
    __checkVariableName(var);
    __stringBag.push_back(var);
    __states.push_back(BayesNetFactory<T_DATA>::RAW_CPT);
  }
}

// @brief Fills the variable's table with the values in rawTable.
// Parse the parents in the same order in which they were added to the
// variable.
// Given a sequence [var, p_1, p_2, ...,p_n-1, p_n] of parents, modalities are parsed
// in the given order (if all p_i are binary):
// [0, 0, ..., 0, 0], [0, 0, ..., 0, 1],
// [0, 0, ..., 1, 0], [0, 0, ..., 1, 1],
// ...,
// [1, 1, ..., 1, 0], [1, 1, ..., 1, 1].
// @param rawTable The raw table.
template<typename T_DATA> INLINE
void
BayesNetFactory<T_DATA>::rawConditionalTable(const std::vector<T_DATA>& rawTable)
{
  if (state() != RAW_CPT) {
    __illegalStateError();
  } else {
    const Potential<T_DATA>& table = __bn->cpt(__varNameMap[__stringBag[0]]);
    Instantiation inst(table);
    size_t i = 0;
    for (inst.setFirst(); (! inst.end()) && (i < rawTable.size()); ++i, ++inst) {
      table.set(inst, rawTable[i]);
    }
    while (! inst.end()) {
      table.set(inst, (T_DATA) 0);
    }
  }
}

// Tells the factory that we finished declaring a conditional probability
// table.
template<typename T_DATA> INLINE
void
BayesNetFactory<T_DATA>::endRawProbabilityDeclaration()
{
  if (state() != RAW_CPT) {
    __illegalStateError();
  } else {
    __resetParts();
    __states.pop_back();
  }
}

// Tells the factory that we're starting a factorized declaration.
template<typename T_DATA> INLINE
void
BayesNetFactory<T_DATA>::startFactorizedProbabilityDeclaration(const std::string& var)
{
  if (state() != NETWORK) {
    __illegalStateError();
  } else {
    __checkVariableName(var);
    std::vector<std::string>::iterator iter = __stringBag.begin();
    __stringBag.insert(iter, var);
    __states.push_back(FACT_CPT);
  }
}

// Tells the factory that we start an entry of a factorized conditional
// probability table.
template<typename T_DATA> INLINE
void
BayesNetFactory<T_DATA>::startFactorizedEntry()
{
  if (state() != FACT_CPT) {
    __illegalStateError();
  } else {
    __parents = new Instantiation();
    __states.push_back(FACT_ENTRY);
  }
}

// Tells the factory that we finished declaring a conditional probability
// table.
template<typename T_DATA> INLINE
void
BayesNetFactory<T_DATA>::endFactorizedEntry()
{
  if (state() != FACT_ENTRY) {
    __illegalStateError();
  } else {
    delete __parents;
    __parents = 0;
    __states.pop_back();
  }
}

// Tells the factory on which modality we want to instantiate one of
// variable's parent.
template<typename T_DATA> INLINE
void
BayesNetFactory<T_DATA>::setParentModality(const std::string& parent,
                                           const std::string& modality)
{
  if (state() != FACT_ENTRY) {
    __illegalStateError();
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
// factory.endParentsDelclaration();
// factory.startFactorizedProbabilityDeclaration("foo");
// std::vector<double> seq;
// seq.insert(0.4); // if foo true
// seq.insert(O.6); // if foo false
// factory.setVariableValues(seq); // fills the table with a default value
// // finish your stuff
// factory.endFactorizedProbabilityDeclaration();
// @code
// @throw OperationNotAllowed Raised if value's size is different than the number
//                            of modalities of the current variable.
template<typename T_DATA> INLINE
void
BayesNetFactory<T_DATA>::setVariableValues(const std::vector<T_DATA>& values)
{
  if (state() != FACT_ENTRY) {
    __illegalStateError();
  } else {
    const DiscreteVariable& var = __bn->variable(__varNameMap[__stringBag[0]]);
    Idx varId = __varNameMap[__stringBag[0]];
    // Checking consistency between values and var.
    if (values.size() != var.domainSize()) {
      GUM_ERROR(OperationNotAllowed, "Invalid number of modalities.");
    }
    Instantiation inst(__bn->cpt(__varNameMap[var.name()]));
    inst.chgValIn(*__parents);
    // Creating an instantiation containing all the variables not ins __parents.
    Instantiation inst_default;
    inst_default << var;
    for (ArcSet::iterator iter = __bn->dag().parents(varId).begin(); iter != __bn->dag().parents(varId).end(); ++iter) {
      if (! __parents->contains(__bn->variable(iter->tail()))) {
        inst_default << __bn->variable(iter->tail());
      }
    }
    // Filling the variable's table.
    for (inst.setFirstIn(inst_default); ! inst.end(); inst.incIn(inst_default)) {
      (__bn->cpt(varId)).set(inst, values[inst.val(var)]);
    }
  }
}

// Tells the factory that we finished declaring a conditional probability
// table.
template<typename T_DATA> INLINE
void
BayesNetFactory<T_DATA>::endFactorizedProbabilityDeclaration()
{
  if (state() != FACT_CPT) {
    __illegalStateError();
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
//            varialbe in the built BayesNet.
// @throw DuplicateElement Raised if a variable with the same name already
//                         exists.
// @throw OperationNotAllowed Raised if redefineParents == false and if table
//                            is not a valid CPT for var in the current state
//                            of the BayesNet.
template<typename T_DATA> INLINE
void
BayesNetFactory<T_DATA>::setVariable(const DiscreteVariable& var)
{
  if ( (state() != BayesNetFactory<T_DATA>::NONE) and
       (state() != BayesNetFactory<T_DATA>::NETWORK) ) {
    __illegalStateError();
  } else {
    __checkVariableName(var.name());
    __varNameMap.insert(var.name(), __bn->addVariable(var));
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
template<typename T_DATA> INLINE
void
BayesNetFactory<T_DATA>::setVariableCPT(const std::string& varName,
                                        Potential<T_DATA>* table,
                                        bool redefineParents)
{
  if ( (state() != BayesNetFactory<T_DATA>::NONE) and
       (state() != BayesNetFactory<T_DATA>::NETWORK) ) {
    __illegalStateError();
  } else {
    __checkVariableName(varName);
    const DiscreteVariable& var = __bn->variable(__varNameMap[varName]);
    NodeId varId = __varNameMap[varName];
    // If we have to change the structure of the BayesNet, then we call a sub method.
    if (redefineParents) {
      __setCPTAndParents(var, table);
    } else if (table->contains(var)) {
      for (ArcSet::iterator iter = __bn->dag().parents(varId).begin(); iter != __bn->dag().parents(varId).end(); ++iter) {
        if (! table->contains(__bn->variable(iter->tail())))
          GUM_ERROR(OperationNotAllowed, "The CPT is not valid in the current BayesNet.");
      }
      // CPT are created when a variable is added.
      delete __bn->__probaMap[varId];
      __bn->__probaMap[varId] = table;
    }
  }
}

// Copy operator is illegal, use only copy constructor.
template<typename T_DATA> INLINE
BayesNetFactory<T_DATA>&
BayesNetFactory<T_DATA>::operator=(const BayesNetFactory<T_DATA>& source)
{
  GUM_ERROR(FatalError, "Illegal!");
  // For noisy compilers
  return *this;
}


// Raise an OperationNotAllowed with the message "Illegal state."
template<typename T_DATA> INLINE
void
BayesNetFactory<T_DATA>::__illegalStateError()
{
  GUM_ERROR(OperationNotAllowed, "Illegal state.");
}

// Check if a variable with the given name exists, if not raise an NotFound
// exception.
template<typename T_DATA> INLINE
void
BayesNetFactory<T_DATA>::__checkVariableName(const std::string& name)
{
  if (!__varNameMap.exists(name)) {
    GUM_ERROR(NotFound, name);
  }
}

// Check if var exists and if mod is one of it's modality, if not raise an
// NotFound exception.
template<typename T_DATA> INLINE
Idx
BayesNetFactory<T_DATA>::__checkVariableModality(const std::string& name,
                                                 const std::string& mod)
{
  __checkVariableName(name);
  const DiscreteVariable& var = __bn->variable(__varNameMap[name]);
  for (Idx i = 0; i < var.domainSize(); ++i) {
    if (mod == var.label(i)) {
      return i;
    }
  }
  GUM_ERROR(NotFound, mod);
}

// Check if in __stringBag there is no other modality with the same name.
template<typename T_DATA> INLINE
void
BayesNetFactory<T_DATA>::__checkModalityInBag(const std::string& mod)
{
  for (size_t i = 2; i < __stringBag.size(); ++i) {
    if (mod == __stringBag[i]) {
      GUM_ERROR(DuplicateElement, mod);
    }
  }
}

// Sub method of setVariableCPT() which redefine the BayesNet's DAG with
// respect to table.
template<typename T_DATA> INLINE
void
BayesNetFactory<T_DATA>::__setCPTAndParents(const DiscreteVariable& var,
                                            Potential<T_DATA>* table)
{
  __bn->__dag.eraseParents(__varNameMap[var.name()]);
  for (Sequence<const DiscreteVariable*>::iterator iter = table->variablesSequence().begin(); iter != table->variablesSequence().end(); ++iter) {
    if ( (*iter) != (&var) ) {
      __checkVariableName((*iter)->name());
      __bn->__dag.insertArc(__varNameMap[(*iter)->name()], __varNameMap[var.name()]);
    }
  }
  // CPT are created when a variable is added.
  delete __bn->__probaMap[__varNameMap[var.name()]];
  __bn->__probaMap[__varNameMap[var.name()]] = table;
}

// Reset the different parts used to constructed the BayesNet.
template<typename T_DATA> INLINE
void
BayesNetFactory<T_DATA>::__resetParts()
{
  __foo_flag = false;
  __bar_flag = false;
  __stringBag.clear();
}
} /* namespace gum */

