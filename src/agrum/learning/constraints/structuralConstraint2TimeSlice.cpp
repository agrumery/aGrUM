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
 * @brief the class for structural constraints over nodes that belong to 2 time
 * slices
 *
 * In DBNs, it is forbidden to add arcs from nodes at time slice t to nodes at
 * time slice s, where s < t. This class allows for taking this kind of constaint
 * into account
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/learning/constraints/structuralConstraint2TimeSlice.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#include <agrum/learning/constraints/structuralConstraint2TimeSlice.inl>
#endif /* GUM_NO_INLINE */


namespace gum {

  
  namespace learning {

    
    /// default constructor
    StructuralConstraint2TimeSlice::StructuralConstraint2TimeSlice () {
      GUM_CONSTRUCTOR ( StructuralConstraint2TimeSlice );
    }


    /// constructor starting with an empty graph with a given number of nodes
    StructuralConstraint2TimeSlice::StructuralConstraint2TimeSlice
    ( const NodeProperty<bool>& time_slice ) {
      for ( auto iter = time_slice.cbegin(); iter != time_slice.cend (); ++iter ) {
        _DiGraph__graph.insertNode ( iter.key () );
      }
      _2TimeSlice__time_slice = time_slice;

      GUM_CONSTRUCTOR ( StructuralConstraint2TimeSlice );
    }
    
    
    /// constructor starting with a given graph
    StructuralConstraint2TimeSlice::StructuralConstraint2TimeSlice
    ( const DiGraph& graph,
      const NodeProperty<bool>& time_slice ) :
    StructuralConstraintDiGraph ( graph ) {
      // check that each node has an appropriate time slice
      if ( time_slice.size () != graph.size () ) {
        GUM_ERROR ( SizeError,
                    "the graph and the property do not have the same size" );
      }
      for ( const auto id : graph ) {
        if ( ! time_slice.exists ( id ) ) {
          GUM_ERROR ( InvalidNode,
                      "there exists a node in the graph without time slice" );
        }
      }
      _2TimeSlice__time_slice = time_slice;

      GUM_CONSTRUCTOR ( StructuralConstraint2TimeSlice );
    }


    /// copy constructor
    StructuralConstraint2TimeSlice::StructuralConstraint2TimeSlice
    ( const StructuralConstraint2TimeSlice& from ) :
      StructuralConstraintDiGraph ( from ),
      _2TimeSlice__time_slice ( from._2TimeSlice__time_slice ),
      _2TimeSlice__default_slice ( from._2TimeSlice__default_slice ) {
      GUM_CONS_CPY ( StructuralConstraint2TimeSlice );
    }
    
      
    /// move constructor
    StructuralConstraint2TimeSlice::StructuralConstraint2TimeSlice
    ( StructuralConstraint2TimeSlice&& from ) :
    StructuralConstraintDiGraph ( std::move ( from ) ),
      _2TimeSlice__time_slice ( std::move ( from._2TimeSlice__time_slice ) ),
      _2TimeSlice__default_slice ( std::move ( from._2TimeSlice__default_slice ) )
    {
      GUM_CONS_MOV ( StructuralConstraint2TimeSlice );
    }

 
    /// destructor
    StructuralConstraint2TimeSlice::~StructuralConstraint2TimeSlice () {
      GUM_DESTRUCTOR ( StructuralConstraint2TimeSlice );
    }


    /// copy operator
    StructuralConstraint2TimeSlice&
    StructuralConstraint2TimeSlice::operator=
    ( const StructuralConstraint2TimeSlice& from ) {
      if ( this != &from ) {
        StructuralConstraintDiGraph::operator= ( from );
        _2TimeSlice__time_slice = from._2TimeSlice__time_slice;
        _2TimeSlice__default_slice = from._2TimeSlice__default_slice;
      }
      return *this;
    }
        

    /// move operator
    StructuralConstraint2TimeSlice&
    StructuralConstraint2TimeSlice::operator=
    ( StructuralConstraint2TimeSlice&& from ) {
      if ( this != &from ) {
        StructuralConstraintDiGraph::operator= ( std::move ( from ) );
        _2TimeSlice__time_slice = std::move ( from._2TimeSlice__time_slice );
        _2TimeSlice__default_slice = std::move ( from._2TimeSlice__default_slice );
      }
      return *this;
    }
    
 
  } /* namespace learning */

  
} /* namespace gum */

