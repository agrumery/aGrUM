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

#include <agrum/multidim/FunctionGraphUtilities/operators/o4DGContext.h>

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
    __DG1ExploredNode = exploredNode;
  }


  // Set DG2 diagram current explored Node

  INLINE void O4DGContext::setDG2Node(const NodeId &exploredNode ) {
    __DG2ExploredNode = exploredNode;
  }


  /* ********************************************************************************************* */
  /*                                                                                               */
  /*                         Variables modalities handlers                                         */
  /*                                                                                               */
  /* ********************************************************************************************* */


  // Changes given variable modality
  INLINE void O4DGContext::chgVarModality(  Idx varIndex, Idx newModality ) {
      __varInstantiation[ varIndex ] = newModality;
  }

  // Changes given variable modality
  INLINE Idx O4DGContext::varModality( Idx varIndex ){
      return __varInstantiation[ varIndex ];
  }


  /* ********************************************************************************************* */
  /*                                                                                               */
  /*                         O4DG Handling methods                                                 */
  /*                                                                                               */
  /* ********************************************************************************************* */

  // Returns o4DGContext key
  INLINE const double& O4DGContext::key( short int* instNeeded ){
      __key = __DG1ExploredNode * __logPrime[ __offset1 ] + __DG2ExploredNode * __logPrime[ __offset2 ];

      for( Idx varPos = 0, offset = __offsetv; varPos < __nbVar; varPos++, offset-- )
          if( instNeeded[varPos] )
            __key += __varInstantiation[varPos] * __logPrime[ offset ];

      return __key;
  }

}/* end of namespace gum */

#endif /* O4DGCONTEXT_INL */
