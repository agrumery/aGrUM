
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
 * @brief Inlined implementation of VariableNodeMap
 *
 * @author Lionel TORTI  and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease parsers in IDE
#  include <agrum/graphicalModels/variableNodeMap.h>

namespace gum {

  // Returns a discrete variable given it's node id.
  // @throws NotFound Raised if no nodes matches id.
  INLINE
  const DiscreteVariable& VariableNodeMap::get(NodeId id) const {
    return *(__nodes2vars.second(id));
  }

  // Returns a node id given it's variable.
  // @throws NotFound Raised if no nodes matches var.
  INLINE
  NodeId VariableNodeMap::get(const DiscreteVariable& var) const {
    return __nodes2vars.first(&var);
  }

  // Return true if id matches a node
  INLINE
  bool VariableNodeMap::exists(NodeId id) const {
    return __nodes2vars.existsFirst(id);
  }

  // Return true if var matches a node
  INLINE
  bool VariableNodeMap::exists(const DiscreteVariable& var) const {
    return __nodes2vars.existsSecond(&var);
  }

  // Returns a node id given it's variable.
  // @throws NotFound Raised if no nodes matches var.
  INLINE
  const DiscreteVariable& VariableNodeMap::operator[](NodeId varId) const {
    return get(varId);
  }

  // Returns a node id given it's variable.
  // @throws NotFound Raised if no nodes matches var.
  INLINE
  NodeId VariableNodeMap::operator[](const DiscreteVariable& var) const {
    return get(var);
  }

  // Maps id with var. Var is added by copy.
  // @throw DuplicateLabel if the name already exists in the mapping
  // @throw DuplicateElement if the id already exists in the mapping
  INLINE
  NodeId VariableNodeMap::insert(NodeId id, const DiscreteVariable& var) {
    if (__names2nodes.existsFirst(var.name())) {
      GUM_ERROR(DuplicateLabel,
                "Unable to insert var with the name '" << var.name() << "'.");
    }

    if (exists(id)) {
      GUM_ERROR(DuplicateElement,
                "Unable to insert a new variable with id " << id << ".");
    }

    __nodes2vars.insert(id, var.clone());
    __names2nodes.insert(var.name(), id);

    return id;
  }

  // Removes a var and it's id of this mapping. The pointer is deleted.
  INLINE
  void VariableNodeMap::erase(NodeId id) {
    const DiscreteVariable* var = __nodes2vars.second(id);
    __names2nodes.eraseFirst(var->name());
    delete (var);
    __nodes2vars.eraseFirst(id);
  }

  // Removes a var and it's id of this mapping. The pointer is deleted.
  INLINE
  void VariableNodeMap::erase(const DiscreteVariable& var) {
    NodeId id = __nodes2vars.first(&var);
    erase(id);
  }

  INLINE
  NodeId VariableNodeMap::idFromName(const std::string& name) const {
    return __names2nodes.second(name);
  }

  INLINE
  const DiscreteVariable&
     VariableNodeMap::variableFromName(const std::string& name) const {
    return *__nodes2vars.second(idFromName(name));
  }

  // we allow the user to change the name of a variable
  // @throws DuplicateLabel if this name already exists
  // @throws NotFound Raised if no nodes matches id.
  INLINE
  void VariableNodeMap::changeName(NodeId id, const std::string& new_name) {
    if (__names2nodes.existsFirst(new_name)) {
      GUM_ERROR(DuplicateLabel,
                "Unable to insert var with the name '" << new_name << "'.");
    }

    DiscreteVariable* var =
       const_cast< DiscreteVariable* >(__nodes2vars.second(id));

    __names2nodes.eraseFirst(var->name());
    var->setName(new_name);
    __names2nodes.insert(new_name, id);
  }

  INLINE const std::string& VariableNodeMap::name(NodeId id) const {
    return __names2nodes.first(id);
  }

  INLINE const std::string&
               VariableNodeMap::name(const DiscreteVariable& var) const {
    return __names2nodes.first(__nodes2vars.first(&var));
  }

} /* namespace gum */

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/
