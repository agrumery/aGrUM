/*********************************************************************************
 *    Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES  	*
 *   {prenom.nom}_at_lip6.fr                                                                           	*
 *                                                                                                                     	*
 *   This program is free software; you can redistribute it and/or modify         	*
 *   it under the terms of the GNU General Public License as published by      	*
 *   the Free Software Foundation; either version 2 of the License, or            	*
 *   (at your option) any later version.                                                             	*
 *                                                                                                                     	*
 *   This program is distributed in the hope that it will be useful,                    	*
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          	*
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         	*
 *   GNU General Public License for more details.                                            	*
 *                                                                                                                     	*
 *   You should have received a copy of the GNU General Public License         	*
 *   along with this program; if not, write to the                                               	*
 *   Free Software Foundation, Inc.,                                                                	*
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                     	*
 ********************************************************************************/
// ============================================================================
#include <cmath>
// ============================================================================
#include <agrum/multidim/patterns/o4DDContext.h>
// ============================================================================
#ifndef O4DDCONTEXT_INL
#define O4DDCONTEXT_INL

namespace gum{


/* **********************************************************************************************/
/*                                                                                                                                            */
/*                          Diagrams current nodes setters & getters.                                                */
/*                                                                                                                                            */
/* **********************************************************************************************/

    // =============================================================================
    // Set leader diagram current explored Node
    // =============================================================================
    INLINE 
    void
    O4DDContext::setLeaderNode (gum::NodeId exploredNode ){ 
      __leaderExploredNode = exploredNode;
    }

    // =============================================================================
    // Set follower diagram current explored Node
    // =============================================================================
    INLINE 
    void
    O4DDContext::setFollowerNode (gum::NodeId exploredNode ){
      __followerExploredNode = exploredNode;
    }


/* **********************************************************************************************/
/*                                                                                                                                            */
/*                         Variables modalities handlers                                                                          */
/*                                                                                                                                            */
/* **********************************************************************************************/

    // =============================================================================
    // Inserts a new retrograde variable
    // =============================================================================
    INLINE
    void
    O4DDContext::addRetrogradeVar( const gum::DiscreteVariable*  var)  {
      if( __varSeq.size() == __nbLogPrime )
	  GUM_ERROR( gum::OperationNotAllowed, "Not enough prime log2 in database. Please add more in o4DDContext.cpp files." );
      __varSeq.insert( var );
      __retrogradeVarInstantiation.push_back( 0 );
      __var2PrimeLog.push_back( __logPrime[ __nbLogPrime - __varSeq.pos( var ) - 3 ] );
    }

    // =============================================================================
    // Changes given variable modality
    // =============================================================================
    INLINE
    void
    O4DDContext::chgVarModality( const gum::DiscreteVariable* var, gum::Idx newModality ){
      __retrogradeVarInstantiation[ __varSeq.pos( var ) ] = newModality;
    }

    // =============================================================================
    // Updates o4DDContextKey
    // =============================================================================
    INLINE
    const double
    O4DDContext::contextKey( ) const{
      double o4DDContextKey = __leaderExploredNode * __leaderPrimeLog + __followerExploredNode * __followerPrimeLog;
      for( Idx i = 0; i < __retrogradeVarInstantiation.size(); i++ )
	o4DDContextKey += __retrogradeVarInstantiation[i] * __var2PrimeLog[i];
      return o4DDContextKey;
    }
    
}/* end of namespace gum */

#endif /* O4DDCONTEXT_INL */
