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
 * @brief the class imposing a N-sized tabu list as a structural constraints for
 * learning algorithms
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/learning/structuralConstraintTabuList.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#include <agrum/learning/structuralConstraintTabuList.inl>
#endif /* GUM_NO_INLINE */


namespace gum {

  
  namespace learning {

    
    /// default constructor
    StructuralConstraintTabuList::StructuralConstraintTabuList () :
      _TabuList__changes ( GUM_STRUCTURAL_CONSTRAINT_TABU_LIST_DEFAULT_SIZE ) {
      // put dummy elements into the list (this avoids having tests to do
      // afterwards)
      for ( unsigned int i = 0;
            i < GUM_STRUCTURAL_CONSTRAINT_TABU_LIST_DEFAULT_SIZE; ++i ) {
        _TabuList__changes.insert
          (  ArcAddition ( std::numeric_limits<NodeId>::max () -i,
                           std::numeric_limits<NodeId>::max () ), i );
      }
      
      GUM_CONSTRUCTOR ( StructuralConstraintTabuList );
    }

    
    /// constructor starting with a given graph
    StructuralConstraintTabuList::StructuralConstraintTabuList
    ( const DiGraph& graph ) :
      _TabuList__changes ( GUM_STRUCTURAL_CONSTRAINT_TABU_LIST_DEFAULT_SIZE ) {
      // put dummy elements into the list (this avoids having tests to do
      // afterwards)
      for ( unsigned int i = 0;
            i < GUM_STRUCTURAL_CONSTRAINT_TABU_LIST_DEFAULT_SIZE; ++i ) {
        _TabuList__changes.insert
          (  ArcAddition ( std::numeric_limits<NodeId>::max () -i,
                           std::numeric_limits<NodeId>::max () ), i );
      }

      GUM_CONSTRUCTOR ( StructuralConstraintTabuList );
    }


    /// destructor
    StructuralConstraintTabuList::~StructuralConstraintTabuList () {
      GUM_DESTRUCTOR ( StructuralConstraintTabuList );
    }
    

  } /* namespace learning */

  
} /* namespace gum */

