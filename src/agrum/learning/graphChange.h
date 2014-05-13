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
#ifndef GUM_LEARNING_GRAPH_CHANGE_H
#define GUM_LEARNING_GRAPH_CHANGE_H


#include <agrum/config.h>
#include <agrum/graphs/nodeGraphPart.h>


namespace gum {

  
  namespace learning {


    /// the type of modification that can be applied to the graph
    enum GraphChangeType {
      ARC_ADDITION,
      ARC_DELETION,
      ARC_REVERSAL,
      EDGE_ADDITION,
      EDGE_DELETION
    };

  
    /* ========================================================================= */
    /* ===                        GRAPH CHANGE CLASS                         === */
    /* ========================================================================= */
    /** @class GraphChange
     * @brief 
     * @ingroup learning_group
     */
    class GraphChange {
    public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      GraphChange ( GraphChangeType type, NodeId node1, NodeId node2 ) noexcept;

      /// copy constructor
      GraphChange ( const GraphChange& from ) noexcept;

      /// move constructor
      GraphChange ( GraphChange&& from ) noexcept;

      /// destructor
      ~GraphChange () noexcept;

      /// @}
      

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy constructor
      GraphChange& operator= ( const GraphChange& from ) noexcept;

      /// move operator
      GraphChange& operator= ( GraphChange&& from ) noexcept;

      /// @}

      
      // ##########################################################################
      /// @name Accessors/Modifiers
      // ##########################################################################
      /// @{
      
      /// returns the type of the operation
      GraphChangeType type () const noexcept;

      /// returns the first node involved in the modification
      NodeId node1 () const noexcept;

      /// returns the second node involved in the modification
      NodeId node2 () const noexcept;

      /// @}
      
      
    private:
      /// the type of modification
      GraphChangeType __type;

      /// the first node in the edge or arc to be modified
      NodeId __node1;

      /// the second node in the edge or arc to be modified
      NodeId __node2;

    };


  } /* namespace learning */
  
  
} /* namespace gum */


#ifndef GUM_NO_INLINE
#include <agrum/learning/graphChange.inl>
#endif //GUM_NOINLINE


#endif /* GUM_LEARNING_GRAPH_CHANGE_H */
