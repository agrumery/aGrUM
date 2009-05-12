/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}@lip6.fr                                                  *
 *   test $Id: $                                                           *
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
 * @brief Outlined implementation VariableNodeMap
 *
 * @author Lionel Torti
 */
// ============================================================================
#include <agrum/BN/variableNodeMap.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/BN/variableNodeMap.inl>
#endif /* GUM_NO_INLINE */
// ============================================================================
namespace gum {

// Default constructor.
VariableNodeMap::VariableNodeMap() {
  GUM_CONSTRUCTOR( VariableNodeMap );
}

// Copy constructor.
VariableNodeMap::VariableNodeMap(const VariableNodeMap& source) {
  GUM_CONS_CPY( VariableNodeMap );
  for (HashTable<NodeId, DiscreteVariable*>::iterator iter = source.__nodes2vars.begin();
       iter != source.__nodes2vars.end();
       ++iter)
  {
    __nodes2vars.insert(iter.key(), (*iter)->copyFactory());
    __vars2nodes.insert(__nodes2vars[iter.key()], iter.key());
  }
}

// Destructor
VariableNodeMap::~VariableNodeMap() {
  GUM_DESTRUCTOR( VariableNodeMap );
  for (HashTable<NodeId, DiscreteVariable*>::iterator iter = __nodes2vars.begin();
       iter != __nodes2vars.end();
       ++iter)
  {
    delete *iter;
  }
  __nodes2vars.clear();
  __vars2nodes.clear();
}

// Copy operator.
INLINE
VariableNodeMap&
VariableNodeMap::operator=(const VariableNodeMap& source) {
  for (HashTable<NodeId, DiscreteVariable*>::iterator iter = __nodes2vars.begin();
       iter != __nodes2vars.end();
       ++iter)
  {
    delete *iter;
  }
  __nodes2vars.clear();
  __vars2nodes.clear();

  for (HashTable<NodeId, DiscreteVariable*>::iterator iter = source.__nodes2vars.begin();
       iter != source.__nodes2vars.end();
       ++iter)
  {
    __nodes2vars.insert(iter.key(), (*iter)->copyFactory());
    __vars2nodes.insert(__nodes2vars[iter.key()], iter.key());
  }
  return *this;
}


} /* namespace gum */
// ============================================================================
