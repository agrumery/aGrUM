/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES et Pierre-Henri WUILLEMIN   *
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

namespace gum {

  
  /// default constructor
  INLINE BarrenNodesFinder::BarrenNodesFinder ( const DAG& dag ) :
    __dag ( &dag ) {
    // for debugging purposes
    GUM_CONSTRUCTOR ( BarrenNodesFinder );
  }

  
  /// copy constructor
  INLINE BarrenNodesFinder::BarrenNodesFinder ( const BarrenNodesFinder& from ) :
    __dag ( from.__dag ),
    __observed_nodes ( from.__observed_nodes ) {
    // for debugging purposes
    GUM_CONS_CPY ( BarrenNodesFinder );
  }


  /// move constructor
  INLINE BarrenNodesFinder::BarrenNodesFinder ( BarrenNodesFinder&& from ) :
    __dag ( from.__dag ),
    __observed_nodes ( std::move ( from.__observed_nodes ) ) {
    // for debugging purposes
    GUM_CONS_MOV ( BarrenNodesFinder );
  }


  /// destructor
  INLINE BarrenNodesFinder::~BarrenNodesFinder () {
    // for debugging purposes
    GUM_DESTRUCTOR ( BarrenNodesFinder );
  }


  /// copy operator
  INLINE BarrenNodesFinder&
  BarrenNodesFinder::operator= ( const BarrenNodesFinder& from ) {
    if ( this != &from ) {
      __dag = from.__dag;
      __observed_nodes = from.__observed_nodes;
    }
    return *this;
  }

  
  /// move operator
  INLINE BarrenNodesFinder&
  BarrenNodesFinder::operator= ( BarrenNodesFinder&& from ) {
    if ( this != &from ) {
      __dag = from.__dag;
      __observed_nodes = std::move ( from.__observed_nodes );
    }
    return *this;
  }

 
  /// sets a new DAG
  INLINE void
  BarrenNodesFinder::setDAG ( const DAG& new_dag ) {
    __dag = &new_dag;
  }


  /// sets the observed nodes in the DAG
  INLINE void
  BarrenNodesFinder::setEvidence ( const NodeSet& observed_nodes ) {
    __observed_nodes = observed_nodes;
  }

  
} /* namespace gum */

