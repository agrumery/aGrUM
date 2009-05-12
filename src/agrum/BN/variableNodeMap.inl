/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}@lip6.fr                                                  *
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
 * @brief Inlined implementation of VariableNodeMap
 *
 * @author Lionel Torti
 */
// ============================================================================
#ifndef DOXYGEN_SHOULD_SKIP_THIS
// ============================================================================
namespace gum {

// Returns a discrete variable given it's node id.
// @throws NotFound Raised if no nodes matches id.
INLINE
const DiscreteVariable&
VariableNodeMap::get(NodeId id) const
{
  return *( __nodes2vars[id] );
}

// Returns a node id given it's variable.
// @throws NotFound Raised if no nodes matches var.
INLINE
NodeId
VariableNodeMap::get(const DiscreteVariable& var) const
{
  return __vars2nodes[&(const_cast<DiscreteVariable&>(var))];
}

// Return true if id matches a node
INLINE
bool
VariableNodeMap::exists( NodeId id ) const {
  return __nodes2vars.exists(id);
}

// Return true if var matches a node
INLINE
bool
VariableNodeMap::exists(const DiscreteVariable& var) const {
  return __vars2nodes.exists(const_cast<DiscreteVariable*>(&var));
}

// Returns a node id given it's variable.
// @throws NotFound Raised if no nodes matches var.
INLINE
const DiscreteVariable&
VariableNodeMap::operator[](NodeId varId) const {
  return get(varId);
}

// Returns a node id given it's variable.
// @throws NotFound Raised if no nodes matches var.
INLINE
NodeId
VariableNodeMap::operator[](const DiscreteVariable& var) const {
  return get(var);
}

// Maps id with var. Var is added by copy.
// @warning If the map already exist it will delete the precedent var!
INLINE
NodeId
VariableNodeMap::_set(NodeId id, const DiscreteVariable& var)
{
  if (__nodes2vars.exists(id)) {
    __vars2nodes.erase(__nodes2vars[id]);
    delete __nodes2vars[id];
    __nodes2vars[id] = var.copyFactory();
    __vars2nodes.insert(__nodes2vars[id], id);
  } else {
    __nodes2vars.insert(id, var.copyFactory());
    __vars2nodes.insert(__nodes2vars[id], id);
  }
  return id;
}

// Removes a var and it's id of this mapping. The pointer is deleted.
INLINE
void
VariableNodeMap::_erase(NodeId id)
{
  __vars2nodes.erase(__nodes2vars[id]);
  delete __nodes2vars[id];
  __nodes2vars.erase(id);
}

// Removes a var and it's id of this mapping. The pointer is deleted.
INLINE
void
VariableNodeMap::_erase(const DiscreteVariable& var)
{
  NodeId id = __vars2nodes[const_cast<DiscreteVariable*>(&var)];
  __vars2nodes.erase(__nodes2vars[id]);
  delete __nodes2vars[id];
  __nodes2vars.erase(id);
}

} /* namespace gum */
// ============================================================================
#endif /*DOXYGEN_SHOULD_SKIP_THIS*/
// ============================================================================
