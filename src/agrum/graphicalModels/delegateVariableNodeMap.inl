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
 * @brief Inlined implementation of DelegateVariableNodeMap
 *
 * @author Lionel Torti & Pierre-Henri Wuillemin
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS


// to ease parsers in IDE
#include <agrum/graphicalModels/delegateVariableNodeMap.h>


namespace gum {


  // Returns a discrete variable given it's node id.
  // @throws NotFound Raised if no nodes matches id.
  INLINE
  const DiscreteVariable& DelegateVariableNodeMap::get( NodeId id ) const {
    return *( __nodes2vars.second( id ) );
  }


  // Returns a node id given it's variable.
  // @throws NotFound Raised if no nodes matches var.
  INLINE
  NodeId DelegateVariableNodeMap::get( const DiscreteVariable& var ) const {
    return __nodes2vars.first( &var );
  }


  // Return true if id matches a node
  INLINE
  bool DelegateVariableNodeMap::exists( NodeId id ) const {
    return __nodes2vars.existsFirst( id );
  }


  // Return true if var matches a node
  INLINE
  bool DelegateVariableNodeMap::exists( const DiscreteVariable& var ) const {
    return __nodes2vars.existsSecond( &var );
  }


  // Returns a node id given it's variable.
  // @throws NotFound Raised if no nodes matches var.
  INLINE
  const DiscreteVariable& DelegateVariableNodeMap::operator[]( NodeId varId ) const {
    return get( varId );
  }


  // Returns a node id given it's variable.
  // @throws NotFound Raised if no nodes matches var.
  INLINE
  NodeId DelegateVariableNodeMap::operator[]( const DiscreteVariable& var ) const {
    return get( var );
  }


  // Maps id with var. Var is added by copy.
  // @warning If the map already exist it will delete the precedent var!
  INLINE
  NodeId DelegateVariableNodeMap::insert( NodeId id, const DiscreteVariable& var ) {
    if ( __names2nodes.existsFirst( var.name() ) ) {
      GUM_ERROR( DuplicateLabel, "Unable to insert var with this name." );
    }

    if ( exists( id ) ) {
      erase( id );
    }

    __nodes2vars.insert( id, var.copyFactory() );

    __names2nodes.insert( var.name(), id );

    return id;
  }


  // Removes a var and it's id of this mapping. The pointer is deleted.
  INLINE
  void DelegateVariableNodeMap::erase( NodeId id ) {
    const DiscreteVariable* var = __nodes2vars.second( id );
    __names2nodes.eraseFirst( var->name() );
    delete( var );
    __nodes2vars.eraseFirst( id );
  }

  // Removes a var and it's id of this mapping. The pointer is deleted.
  INLINE
  void DelegateVariableNodeMap::erase( const DiscreteVariable& var ) {
    NodeId id = __nodes2vars.first( &var );
    erase( id );
  }


  INLINE
  NodeId DelegateVariableNodeMap::idFromName( const std::string& name ) const {
    return __names2nodes.second( name );
  }


  INLINE
  const DiscreteVariable&
  DelegateVariableNodeMap::variableFromName( const std::string& name ) const {
    return *__nodes2vars.second( idFromName( name ) );
  }


  // we allow the user to change the name of a variable
  // @throws DuplicateLabel if this name already exists
  // @throws NotFound Raised if no nodes matches id.
  INLINE
  void DelegateVariableNodeMap::changeName( NodeId id, const std::string& new_name ) {
    if ( __names2nodes.existsFirst( new_name ) ) {
      GUM_ERROR( DuplicateLabel, "Unable to insert var with this name." );
    }

    DiscreteVariable* var =

      const_cast<DiscreteVariable*>( __nodes2vars.second( id ) );

    __names2nodes.eraseFirst( var->name() );
    var->setName( new_name );
    __names2nodes.insert( new_name, id );
  }


  /// removes all the associations
  INLINE void DelegateVariableNodeMap::clear() {
    __nodes2vars.clear();
    __names2nodes.clear();
  }


  INLINE const std::string& DelegateVariableNodeMap::name( NodeId id ) const {
    return __names2nodes.first( id );
  }

  INLINE const std::string& DelegateVariableNodeMap::name( const DiscreteVariable& var ) const {
    return __names2nodes.first( __nodes2vars.first( &var ) );
  }



} /* namespace gum */


#endif /*DOXYGEN_SHOULD_SKIP_THIS*/


