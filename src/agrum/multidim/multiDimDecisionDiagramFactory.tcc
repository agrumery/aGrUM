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
/*                                                                                                                                            */
/*                              Constructors, Destructors                                                                     */
/*                                                                                                                                            */
/* **********************************************************************************************/

    // =============================================================================
    // Default constructor.
    // =============================================================================
    template<typename T_DATA, template <class> class IApproximationPolicy>
    MultiDimDecisionDiagramFactory< T_DATA, IApproximationPolicy >::MultiDimDecisionDiagramFactory() {
	GUM_CONSTRUCTOR( MultiDimDecisionDiagramFactory ) ;
    }

    // =============================================================================
    // clone constructor.
    // =============================================================================
    template<typename T_DATA, template <class> class IApproximationPolicy>
    MultiDimDecisionDiagramFactory< T_DATA, IApproximationPolicy >::MultiDimDecisionDiagramFactory( const IApproximationPolicy<T_DATA>& md ) :
    MultiDimDecisionDiagramFactoryBase<T_DATA>(),
    IApproximationPolicy<T_DATA>( md ) {
	GUM_CONSTRUCTOR( MultiDimDecisionDiagramFactory ) ;
    }
    // =============================================================================
    // Destructor.
    // @warnings : this will not destroy properties on node. They ahve to be removed on multidim destruction
    // =============================================================================
    template<typename T_DATA, template <class> class IApproximationPolicy>
    MultiDimDecisionDiagramFactory< T_DATA, IApproximationPolicy >::~MultiDimDecisionDiagramFactory() {
	GUM_DESTRUCTOR( MultiDimDecisionDiagramFactory );
    }

/* **********************************************************************************************/
/*                                                                                                                                            */
/*                              Graph Manipulation methods                                                                 */
/*                                                                                                                                            */
/* **********************************************************************************************/

    // =============================================================================
    // Returns the multidimDecisionDiagram made
    // =============================================================================
    template<typename T_DATA, template <class> class IApproximationPolicy>
    MultiDimDecisionDiagramBase<T_DATA>*
    MultiDimDecisionDiagramFactory< T_DATA, IApproximationPolicy >::getMultiDimDecisionDiagram( bool fillWithDefaultArc, T_DATA defaultValue ) {
      
	Bijection<NodeId, T_DATA> newValueMap;
	if( this->_model.size() == 2 && !this->_valueMap.empty() )
	    newValueMap = this->_valueMap;
	else
	    for( BijectionIterator<NodeId,T_DATA> valueIter = this->_valueMap.begin(); valueIter != this->_valueMap.end(); ++valueIter )
		if( this->_model.parents( valueIter.first() ).empty() && valueIter.first() != this->_rootId )
		    this->_model.eraseNode( valueIter.first() );
		else
		    newValueMap.insert( valueIter.first(), valueIter.second() );

	if ( fillWithDefaultArc ) {

	    if ( defaultValue < this->lowLimit() )
		defaultValue = ( this->lowLimit() + this->highLimit() )/2;
	    NodeId zeroId = 0;
	    bool zeroNotCreated = true;

	    for ( DiGraph::NodeIterator iter = this->_model.beginNodes(); iter != this->_model.endNodes(); ++iter ) {
		if( *iter != 0 && !newValueMap.existsFirst( *iter ) ){

		    bool needDefault = false;
		    for( std::vector<NodeId>::iterator sonIter = this->_arcMap[*iter]->begin(); sonIter != this->_arcMap[*iter]->end(); ++sonIter )
			if( *sonIter == 0 ){
			    needDefault = true;
			    break;
			}

		    if ( needDefault && !this->_defaultArcMap.exists( *iter ) ) {
			if( zeroNotCreated ){
			    zeroId = this->addTerminalNode( defaultValue );
			    if( !newValueMap.existsSecond( defaultValue ) )
				newValueMap.insert( zeroId, defaultValue );
			    zeroNotCreated = false;
			}
			this->_defaultArcMap.insert( *iter, zeroId );
		    }

		}
	    }
	}

	MultiDimDecisionDiagram< T_DATA, IApproximationPolicy >* ret =  new MultiDimDecisionDiagram< T_DATA, IApproximationPolicy >( *this );

	ret->beginInstantiation();

	//   ret->setEpsilon( this->epsilon() );
	ret->setLimits(this->lowLimit(),this->highLimit());

	if( this->_noVariableCheckMode )
	    this->_varsSeq = this->_findVariableOrder();
	ret->setVariableSequence( this->_varsSeq );

	ret->setDiagramNodes( this->_model );

	ret->setVariableMap( this->_varMap );
	ret->setVar2NodeMap( this->_var2NodeIdMap );

	ret->setValueMap( newValueMap );
	ret->setDiagramArcs( this->_arcMap, this->_defaultArcMap );

	if( this->_rootId == 0 )
	    for ( DiGraph::NodeIterator iter = this->_model.beginNodes(); iter != this->_model.endNodes(); ++iter ) {
		if( newValueMap.existsFirst( *iter ) && this->_model.size() == 2 ){
		    this->_rootId = *iter;
		    break;
		}
		if ( *iter != 0 && !newValueMap.existsFirst( *iter ) && !this->_model.children( *iter ).empty() && this->_model.parents( *iter ).empty() ) {
		    this->_rootId = *iter;
		    break;
		}
	    }
	ret->setRoot( this->_rootId );

	ret->endInstantiation();

	return ret;
    }
