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
/** @file
 * @brief A class to account for changes in a graph
 *
 * This class shall be used by learning algorithms to notify scores, structural
 * constraints, etc, that the learnt graph has been modified.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {
  
  
  namespace learning {


    /// default constructor
    INLINE GraphChange::GraphChange ( GraphChangeType type,
                                      NodeId node1,
                                      NodeId node2 ) noexcept :
      __type  { type },
      __node1 { node1 },
      __node2 { node2 } {
        GUM_CONSTRUCTOR ( learning::GraphChange );
    }
  

    /// copy constructor
    INLINE GraphChange::GraphChange ( const GraphChange& from ) noexcept :
      __type  { from.__type },
      __node1 { from.__node1 },
      __node2 { from.__node2 } {
        GUM_CONS_CPY ( learning::GraphChange );
    }


    /// move constructor
    INLINE GraphChange::GraphChange ( GraphChange&& from ) noexcept :
      __type  { from.__type },
      __node1 { from.__node1 },
      __node2 { from.__node2 } {
        GUM_CONS_MOV ( learning::GraphChange );
    }


    /// destructor
    INLINE GraphChange::~GraphChange () noexcept {
      GUM_DESTRUCTOR ( learning::GraphChange );
    }


    /// copy constructor
    INLINE GraphChange&
    GraphChange::operator= ( const GraphChange& from ) noexcept {
      __type  = from.__type;
      __node1 = from.__node1;
      __node2 = from.__node2;
      return *this;
    }


    /// move operator
    INLINE GraphChange&
    GraphChange::operator= ( GraphChange&& from ) noexcept {
      __type  = from.__type;
      __node1 = from.__node1;
      __node2 = from.__node2;
      return *this;
    }


    /// returns the type of the operation
    INLINE GraphChangeType GraphChange::type () const noexcept {
      return __type;
    }


    /// returns the first node involved in the modification
    INLINE NodeId GraphChange::node1 () const noexcept {
      return __node1;
    }


    /// returns the second node involved in the modification
    INLINE NodeId GraphChange::node2 () const noexcept {
      return __node2;
    }
    

  } /* namespace learning */
  
  
} /* namespace gum */



#endif /* DOXYGEN_SHOULD_SKIP_THIS */
