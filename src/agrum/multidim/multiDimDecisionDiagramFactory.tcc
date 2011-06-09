/***************************************************************************
*   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
/**
* @file
* @brief Template implementation of MultiDimDecisionDiagramFactory.h class.
*
* @author Jean-Christophe Magnan
*/
// ============================================================================
#include <agrum/multidim/multiDimDecisionDiagramFactory.h>
// ============================================================================
using namespace std;
using namespace gum;

/* **********************************************************************************************/
/*																								*/
/*                         Constructors, Destructors											*/
/*																								*/
/* **********************************************************************************************/

    // =============================================================================
    // Default constructor.
    // =============================================================================
    template<typename T_DATA, template <class> class IApproximationPolicy>
    MultiDimDecisionDiagramFactory< T_DATA, IApproximationPolicy >::MultiDimDecisionDiagramFactory(){
		GUM_CONSTRUCTOR( MultiDimDecisionDiagramFactory ) ;
	}

    // =============================================================================
    // Destructor.
    // @warnings : this will not destroy properties on node. They ahve to be removed on multidim destruction
    // =============================================================================
    template<typename T_DATA, template <class> class IApproximationPolicy>
    MultiDimDecisionDiagramFactory< T_DATA, IApproximationPolicy >::~MultiDimDecisionDiagramFactory(){
		GUM_DESTRUCTOR( MultiDimDecisionDiagramFactory );
	}

/* **********************************************************************************************/
/*																								*/
/*                        Graph Manipulation methods											*/
/*																								*/
/* **********************************************************************************************/
	
	// =============================================================================
    // Returns the multidimDecisionDiagram made
    // =============================================================================
    template<typename T_DATA, template <class> class IApproximationPolicy>
	MultiDimDecisionDiagramBase<T_DATA>*
	MultiDimDecisionDiagramFactory< T_DATA, IApproximationPolicy >::getMultiDimDecisionDiagram(bool fillWithDefaultArc, T_DATA defaultValue ) {
		
		if( fillWithDefaultArc ){
			if( defaultValue < this->getLowerLimit() )
				defaultValue = ( this->getLowerLimit() + this->getHigherLimit() )/2;
			NodeId zeroId = this->addTerminalNode( defaultValue );
			for ( DAG::NodeIterator iter = this->_model.beginNodes(); iter != this->_model.endNodes(); ++iter ) {
				if ( !this->_valueMap.existsFirst(*iter) && !this->_defaultArcMap.exists(*iter) && this->_arcMap[*iter]->size() < this->_varMap[*iter]->domainSize() ){
					this->_defaultArcMap.insert( *iter, zeroId );
				}
			}
		}
		
		MultiDimDecisionDiagram< T_DATA, IApproximationPolicy >* ret =  new MultiDimDecisionDiagram< T_DATA, IApproximationPolicy >( );
		
		ret->beginMultipleChanges();
		
		ret->setEpsilon( this->getEpsilon() );
		ret->setLowerLimit( this->getLowerLimit() );
		ret->setHigherLimit( this->getHigherLimit() );
		
		ret->setVariableSequence( this->_varsSeq );
		
		ret->setDiagramNodes( this->_model );
		
		ret->setVariableMap( this->_varMap );
		
		ret->setValueMap( this->_valueMap );
		ret->setDiagramArcs( this->_arcMap, this->_defaultArcMap );
		
		NodeId root = 0;
		for ( DAG::NodeIterator iter = this->_model.beginNodes(); iter != this->_model.endNodes(); ++iter ) {
			if ( !this->_valueMap.existsFirst(*iter) && !this->_model.children(*iter).empty() && this->_model.parents(*iter).empty() ){
					root = *iter;
					break;
				}
		}
		ret->setRoot( root );
		
		ret->endMultipleChanges();
		
		return ret;
	}
