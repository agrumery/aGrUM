/****************************************************************************
 *    Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                                *
 *                                                                          *
 *   This program is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program; if not, write to the                          *
 *   Free Software Foundation, Inc.,                                        *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.              *
 ****************************************************************************/

#include <cmath>

#include <agrum/multidim/patterns/DGUtility/o4DGContext.h>

#ifndef O4DGCONTEXT_INL
#define O4DGCONTEXT_INL

namespace gum {


  /* ********************************************************************************************* */
  /*                                                                                               */
  /*                          Diagrams current nodes setters & getters.                            */
  /*                                                                                               */
  /* ********************************************************************************************* */


  // Set DG1 diagram current explored Node
  INLINE void O4DGContext::setDG1Node(const NodeId &exploredNode ) {
      __key += ( exploredNode - __DG1ExploredNode )*__logPrime[ __nbLogPrime - 1 ];
    __DG1ExploredNode = exploredNode;
  }


  // Set DG2 diagram current explored Node

  INLINE void O4DGContext::setDG2Node(const NodeId &exploredNode ) {
      __key += ( exploredNode - __DG2ExploredNode )*__logPrime[ __nbLogPrime - 2 ];
    __DG2ExploredNode = exploredNode;
  }


  /* ********************************************************************************************* */
  /*                                                                                               */
  /*                         Variables modalities handlers                                         */
  /*                                                                                               */
  /* ********************************************************************************************* */


  // Changes given variable modality

  INLINE void O4DGContext::chgVarModality(  Idx varIndex, Idx newModality ) {
      __key += ( newModality - __varInstantiation[ varIndex ] )*__logPrime[ __nbLogPrime - 3 - varIndex ];
      __varInstantiation[ varIndex ] = newModality;
  }

  /// Changes given variable modality
  INLINE Idx O4DGContext::varModality( Idx varIndex ){
      return __varInstantiation[ varIndex ];
  }

  // ============================================================================
  /// Allocators and Deallocators redefinition
  // ============================================================================
  INLINE void* O4DGContext::operator new(size_t l){
      return MultiDimDecisionGraph::soa.allocate( l);
  }

  INLINE void O4DGContext::operator delete(void* p){
      MultiDimDecisionGraph::soa.deallocate( p, sizeof(O4DGContext) );
  }

}/* end of namespace gum */

#endif /* O4DGCONTEXT_INL */
