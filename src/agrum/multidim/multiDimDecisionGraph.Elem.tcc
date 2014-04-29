
/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                              //
 *                                                                        //
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or    //
 *   (at your option) any later version.                                  //
 *                                                                        //
 *   This program is distributed in the hope that it will be useful,      //
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of       //
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
 *   GNU General Public License for more details.                         //
 *                                                                        //
 *   You should have received a copy of the GNU General Public License    //
 *   along with this program; if not, write to the                        //
 *   Free Software Foundation, Inc.,                                      //
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.            //
 ***************************************************************************/
/**
 * @file
 * @brief Template methods of MultiDimDecisionGraph.
 *
 * @author Jean-Christophe Magnan
 *
 */
#include <agrum/multidim/multiDimDecisionGraph.h>

namespace gum {

  template<typename GUM_SCALAR>
  Idx MultiDimDecisionGraph<GUM_SCALAR>::aNICLE(0);
  template<typename GUM_SCALAR>
  Idx MultiDimDecisionGraph<GUM_SCALAR>::dNICLE(0);
  template<typename GUM_SCALAR>
  Idx MultiDimDecisionGraph<GUM_SCALAR>::aPICLE(0);
  template<typename GUM_SCALAR>
  Idx MultiDimDecisionGraph<GUM_SCALAR>::dPICLE(0);

// ############################################################################
// Node Chained List methods
// ############################################################################
  // ============================================================================
  // Since elem chain are handled with soa, here is the operation of adding a node
  // to the list.
  // ============================================================================
  template<typename GUM_SCALAR>
  INLINE
  void MultiDimDecisionGraph<GUM_SCALAR>::_addElemToNICL( NICLElem **nodeChain, const NodeId& elemId){

    NICLElem* newElem = static_cast< NICLElem* >(soa.allocate(sizeof( NICLElem )));
    newElem->elemId = elemId;
    newElem->nextElem = *nodeChain;
    *nodeChain = newElem;
    aNICLE++;
  }

  // ============================================================================
  // And here the one to remove the elem
  // ============================================================================
  template<typename GUM_SCALAR>
  INLINE
  void MultiDimDecisionGraph<GUM_SCALAR>::_removeElemFromNICL( NICLElem **nodeChain, const NodeId& elemId){

    NICLElem* current = *nodeChain;
    NICLElem** prev = nodeChain;
    while( current != nullptr ){
      if( current->elemId == elemId ){
        *prev = current->nextElem;
        soa.deallocate(current, sizeof( NICLElem ));
        break;
      }
      prev = &(current->nextElem);
      current = current->nextElem;
    }
    dNICLE++;
  }

  // ============================================================================
  //  Delete completely the chain
  // ============================================================================
  template<typename GUM_SCALAR>
  INLINE
  void MultiDimDecisionGraph<GUM_SCALAR>::_deleteNICL( NICLElem* nodeChain ){

    // Déallocation des chainons parents dans la liste des parents
    NICLElem* currentNode = nodeChain;
    NICLElem* nextNode = nullptr;
    while( currentNode ){
      nextNode = currentNode->nextElem;
      soa.deallocate(currentNode, sizeof( NICLElem ));
      currentNode = nextNode;
      dNICLE++;
    }
    nodeChain = nullptr;
  }



// ############################################################################
// Parent Chained List methods
// ############################################################################
  // ============================================================================
  // Since elem chain are handled with soa, here is the operation of adding a node
  // to the list.
  // ============================================================================
  template<typename GUM_SCALAR>
  INLINE
  void MultiDimDecisionGraph<GUM_SCALAR>::_addElemToPICL( PICLElem **nodeChain, const NodeId& parent, const Idx& mod){

    PICLElem* newElem = static_cast< PICLElem* >(soa.allocate(sizeof( PICLElem )));
    newElem->parentId = parent;
    newElem->modality = mod;
    newElem->nextElem = *nodeChain;
    *nodeChain = newElem;
    aPICLE++;
  }

  // ============================================================================
  // And here the one to remove the elem
  // ============================================================================
  template<typename GUM_SCALAR>
  INLINE
  void MultiDimDecisionGraph<GUM_SCALAR>::_removeElemFromPICL( PICLElem **nodeChain, const NodeId& parent, const Idx& mod ){

    PICLElem* current = *nodeChain;
    PICLElem** prev = nodeChain;
    while( current != nullptr ){
      if( current->parentId == parent && current->modality == mod ){
        *prev = current->nextElem;
        soa.deallocate(current, sizeof( PICLElem ));
        dPICLE++;
        break;
      }
      prev = &(current->nextElem);
      current = current->nextElem;
    }
  }

  // ============================================================================
  //  Delete completely the chain
  // ============================================================================
  template<typename GUM_SCALAR>
  INLINE
  void MultiDimDecisionGraph<GUM_SCALAR>::_deletePICL( PICLElem* nodeChain ){

    // Déallocation des chainons parents dans la liste des parents
    PICLElem* currentNode = nodeChain;
    PICLElem* nextNode = nullptr;
    while( currentNode != nullptr ){
      nextNode = currentNode->nextElem;
      soa.deallocate(currentNode, sizeof( PICLElem ));
      currentNode = nextNode;
      dPICLE++;
    }
    nodeChain = nullptr;
  }
} // namespace gum

