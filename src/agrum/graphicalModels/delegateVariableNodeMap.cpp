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
 * @brief Outlined implementation DelegateVariableNodeMap
 *
 * @author Lionel Torti & Pierre-Henri Wuillemin
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <iostream>
#include <sstream>
#include <agrum/graphicalModels/delegateVariableNodeMap.h>

#ifdef GUM_NO_INLINE
#include <agrum/graphicalModels/delegateVariableNodeMap.inl>
#endif /* GUM_NO_INLINE */


namespace gum {


  // Default constructor.
  DelegateVariableNodeMap::DelegateVariableNodeMap() {
    GUM_CONSTRUCTOR( DelegateVariableNodeMap );
  }


  // Copy constructor.
  DelegateVariableNodeMap::DelegateVariableNodeMap( const DelegateVariableNodeMap& source ) {
    GUM_CONS_CPY( DelegateVariableNodeMap );

    for ( Bijection<NodeId, const DiscreteVariable*>::iterator iter =
            source.__nodes2vars.begin();
          iter != source.__nodes2vars.end(); ++iter ) {
      __nodes2vars.insert( iter.first(), iter.second()->copyFactory() );
      // copy factory is used inside insert
    }

    __names2nodes = source.__names2nodes;
  }


  // Destructor
  DelegateVariableNodeMap::~DelegateVariableNodeMap() {
    GUM_DESTRUCTOR( DelegateVariableNodeMap );

    for ( Bijection<NodeId, const DiscreteVariable*>::iterator iter =
            __nodes2vars.begin(); iter != __nodes2vars.end(); ++iter ) {
      delete iter.second();
    }

    __nodes2vars.clear();

    __names2nodes.clear();
  }


  // Copy operator.
  DelegateVariableNodeMap&
  DelegateVariableNodeMap::operator=( const DelegateVariableNodeMap& source ) {
    for ( Bijection<NodeId, const DiscreteVariable*>::iterator iter =
            __nodes2vars.begin(); iter != __nodes2vars.end(); ++iter ) {
      delete iter.second();
    }

    __nodes2vars.clear();

    for ( Bijection<NodeId, const DiscreteVariable*>::iterator iter =
            source.__nodes2vars.begin();
          iter != source.__nodes2vars.end(); ++iter ) {
      __nodes2vars.insert( iter.first(), iter.second()->copyFactory() );
    }

    __names2nodes = source.__names2nodes;

    return *this;
  }


  /// friendly displays the content of the DelegateVariableNodeMap
  const std::string DelegateVariableNodeMap::toString() const {
    std::stringstream stream;

    stream << "list of associations:" << std::endl;
    stream << __nodes2vars << std::endl << std::endl;
    stream << "list of variable names:" << std::endl;
    stream <<  __names2nodes << std::endl;

    return stream.str();
  }



  // ============================================================================
  /// for friendly displaying the content of clique graphs
  // ============================================================================
  std::ostream& operator<< ( std::ostream& stream, const DelegateVariableNodeMap& v ) {
    stream << v.toString();
    return stream;
  }


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */


