/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                                  *
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

/** @file
 * @brief Inlined implementation of the base class of every kind of node
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <iostream>
#include <string>
#include <agrum/gumDebug.h>
#include <agrum/gumId.h>
#include <agrum/gumNode.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS


// ================================================================================
/// basic constructor
// ================================================================================
INLINE gumNode::gumNode(const gumId& id, const std::string& aLabel) :
  __ID(id), __label(aLabel) {
  // for debugging purposes
  GUM_CONSTRUCTOR(gumNode);
}


// ================================================================================
/// copy constructor
// ================================================================================
INLINE gumNode::gumNode(const gumNode& from) : __ID(from.__ID), __label(from.__label) {
  // for debugging purposes
  GUM_CONS_CPY(gumNode);
}


// ==============================================================================
/// Destructor
// ==============================================================================
INLINE gumNode::~gumNode() {
  // for debugging purposes
  GUM_DESTRUCTOR(gumNode);
}


// ================================================================================
/// copy operator
// ================================================================================
INLINE gumNode& gumNode::operator= (const gumNode& from) {
  // for debugging purposes
  GUM_OP_CPY(gumNode);

  __label = from.__label;
  __ID = from.__ID;

  return *this;
}


// ================================================================================
/// method for changing the __label of a node
// ================================================================================
INLINE void gumNode::setLabel(const std::string& theValue) {
  __label = theValue;
}


// ================================================================================
/// method for changing the __label of a node
// ================================================================================
INLINE void gumNode::setID(const gumId& new_id)  {
  __ID = new_id;
}


// ================================================================================
/// method for retrieving the __label of a node
// ================================================================================
INLINE const std::string& gumNode::label() const  {
  return __label;
}


// ================================================================================
/// method for retrieving the id of the node
// ================================================================================
INLINE const gumId& gumNode::getID() const  {
  return __ID;
}


// ================================================================================
/// check if two nodes are equal, i.e., if they have the same __ID
// ================================================================================
INLINE const bool gumNode::operator== (const gumNode& from) const  {
  return (__ID == from.__ID);
}


// ================================================================================
/// check if two nodes are different, i.e., if they have different __ID
// ================================================================================
INLINE const bool gumNode::operator!= (const gumNode& from) const  {
  return (__ID != from.__ID);
}



#endif /* DOXYGEN_SHOULD_SKIP_THIS */
