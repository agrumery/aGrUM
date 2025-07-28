/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Inlined implementation of VariableNodeMap
 *
 * @author Lionel TORTI  and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease parsers in IDE
#  include <agrum/base/graphicalModels/variableNodeMap.h>

namespace gum {

  // Returns a discrete variable given it's node id.
  // @throws NotFound Raised if no nodes matches id.
  INLINE
  const DiscreteVariable& VariableNodeMap::get(NodeId id) const {
    return *(_nodes2vars_.second(id));
  }

  // Returns a node id given it's variable.
  // @throws NotFound Raised if no nodes matches var.
  INLINE
  NodeId VariableNodeMap::get(const DiscreteVariable& var) const {
    return _nodes2vars_.first(&var);
  }

  // Return true if id matches a node
  INLINE
  bool VariableNodeMap::exists(NodeId id) const { return _nodes2vars_.existsFirst(id); }

  // Return true if var matches a node
  INLINE
  bool VariableNodeMap::exists(const DiscreteVariable& var) const {
    return _nodes2vars_.existsSecond(&var);
  }

  // Return the size of the map
  INLINE
  gum::Size VariableNodeMap::size() const { return _nodes2vars_.size(); }

  // Returns a node id given it's variable.
  // @throws NotFound Raised if no nodes matches var.
  INLINE
  const DiscreteVariable& VariableNodeMap::operator[](NodeId varId) const { return get(varId); }

  // Returns a node id given it's variable.
  // @throws NotFound Raised if no nodes matches var.
  INLINE
  NodeId VariableNodeMap::operator[](const DiscreteVariable& var) const { return get(var); }

  // Maps id with var. Var is added by copy.
  // @throw DuplicateLabel if the name already exists in the mapping
  // @throw DuplicateElement if the id already exists in the mapping
  INLINE
  NodeId VariableNodeMap::insert(NodeId id, const DiscreteVariable& var) {
    if (_names2nodes_.existsFirst(var.name())) {
      GUM_ERROR(DuplicateLabel, "Unable to insert var with the name '" << var.name() << "'.")
    }

    if (exists(id)) {
      GUM_ERROR(DuplicateElement, "Unable to insert a new variable with id " << id << ".")
    }

    _nodes2vars_.insert(id, var.clone());
    _names2nodes_.insert(var.name(), id);

    return id;
  }

  // Removes a var and it's id of this mapping. The pointer is deleted.
  INLINE
  void VariableNodeMap::erase(NodeId id) {
    const DiscreteVariable* var = _nodes2vars_.second(id);
    _names2nodes_.eraseFirst(var->name());
    delete var;
    _nodes2vars_.eraseFirst(id);
  }

  // Removes a var and it's id of this mapping. The pointer is deleted.
  INLINE
  void VariableNodeMap::erase(const DiscreteVariable& var) {
    NodeId id = _nodes2vars_.first(&var);
    erase(id);
  }

  INLINE
  NodeId VariableNodeMap::idFromName(const std::string& name) const {
    return _names2nodes_.second(name);
  }

  INLINE
  const DiscreteVariable& VariableNodeMap::variableFromName(const std::string& name) const {
    return *_nodes2vars_.second(idFromName(name));
  }

  // we allow the user to change the name of a variable
  // @throws DuplicateLabel if this name already exists
  // @throws NotFound Raised if no nodes matches id.
  INLINE
  void VariableNodeMap::changeName(NodeId id, const std::string& new_name) {
    if (_names2nodes_.existsFirst(new_name)) {
      GUM_ERROR(DuplicateLabel, "Unable to insert var with the name '" << new_name << "'.")
    }

    auto var = const_cast< DiscreteVariable* >(_nodes2vars_.second(id));

    _names2nodes_.eraseFirst(var->name());
    var->setName(new_name);
    _names2nodes_.insert(new_name, id);
  }

  INLINE const std::string& VariableNodeMap::name(NodeId id) const {
    return _names2nodes_.first(id);
  }

  INLINE const std::string& VariableNodeMap::name(const DiscreteVariable& var) const {
    return _names2nodes_.first(_nodes2vars_.first(&var));
  }

} /* namespace gum */

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/
