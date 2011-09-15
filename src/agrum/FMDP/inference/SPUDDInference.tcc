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
* @brief Template implementation of FMDP/FactoredMarkovDecisionProcess.h classes.
*
* @author Jean-Christophe Magnan
*/
// ============================================================================
#include <cstdio>
#include <iostream>
#include <sstream>
#include <math.h>
// ============================================================================
#include <agrum/multidim/potential.h>
#include <agrum/multidim/instantiation.h>
// ============================================================================
#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
// ============================================================================

namespace gum {
		

	/* **************************************************************************************************** **/
	/* **																									**/
	/* **									Constructors / Destructors										**/
	/* **																									**/
	/* **************************************************************************************************** **/

		// ===========================================================================
		// Default constructor
		// ===========================================================================
		template<typename T_DATA> INLINE
		SPUDDInference<T_DATA>::SPUDDInference ( FactoredMarkovDecisionProcess<T_DATA>* fmdp, T_DATA epsilon ){
		
			GUM_CONSTRUCTOR( SPUDDInference );
			__epsilon = epsilon;
			__fmdp = fmdp;
		}

		// ===========================================================================
		// Default destructor
		// ===========================================================================
		template<typename T_DATA> INLINE
		SPUDDInference<T_DATA>::~SPUDDInference(){
			
			GUM_DESTRUCTOR( SPUDDInference );
		}
		

	/* **************************************************************************************************** **/
	/* **																									**/
	/* **										General Methods												**/
	/* **																									**/
	/* **************************************************************************************************** **/

		// ===========================================================================
		// Makes a spudd inference on FMDP
		// ===========================================================================
		template<typename T_DATA>
		void
		SPUDDInference<T_DATA>::makeInference (  ){
		
			// *****************************************************************************************
			// Initialisation
			// *****************************************************************************************
			
				// *****************************************************************************************
				// Threshold stopping criterion evaluation
				T_DATA threshold =  this->__epsilon * ( 1 - __fmdp->discount() ) / ( 2 * __fmdp->discount() );
				threshold *= threshold;
				
				T_DATA gap = threshold + 1;
				
				
				// *****************************************************************************************
				// Initialisation of Vold, Vnew and Vtemp
				MultiDimDecisionDiagramBase< T_DATA >* Vold = reinterpret_cast<MultiDimDecisionDiagramBase<T_DATA>*>( __fmdp->reward()->newFactory() );
				Vold->copy( *reinterpret_cast<const MultiDimDecisionDiagramBase<T_DATA>*>( __fmdp->reward() ) );
				
				MultiDimDecisionDiagramBase< T_DATA >* Vnew = NULL;
				
				MultiDimDecisionDiagramBase< T_DATA >* Vtemp = NULL;
			
			
			// *****************************************************************************************
			// Main loop
			// *****************************************************************************************
			while( gap >  threshold ){
			
				// *****************************************************************************************
				// Loop reset
				__fmdp->resetActionsIterator();
				Set< MultiDimDecisionDiagramBase< T_DATA >* > VactionCollector;
				
				// *****************************************************************************************
				// For each action
				while( __fmdp->hasAction() ){
				
					__fmdp->resetVariablesIterator();
					
				//~ std::cout << std::endl << "---- Primérisation des diagrammes ";
					MultiDimDecisionDiagramBase< T_DATA >* Vaction = __fmdp->primeCopy( Vold );
				//~ std::cout << std::endl << "---- DONE ";
					
					
					// *****************************************************************************************
					// To evaluate action value function, we multiply old main value function by transition table
					// of each variable
				//~ std::cout << std::endl << "---- Multiplication des diagrammes ";
					while( __fmdp->hasVariable() ){
						
						Vtemp = Vaction;
						Vaction = multiply2MultiDimDecisionDiagrams( reinterpret_cast<const MultiDimDecisionDiagramBase<T_DATA>*>( __fmdp->transition() ), Vaction );
						delete Vtemp;
							
						__fmdp->nextVariable();
					}
				//~ std::cout << std::endl << "---- DONE ";
										
					// *****************************************************************************************
					// the we project on each of those variables (by doing sums )
					Vtemp = Vaction;
				//~ std::cout << std::endl << "---- Projection des diagrammes ";
					Vaction = projectSumMultiDimDecisionDiagram( Vaction, __fmdp->primedVariables() );
					delete Vtemp;
					
					VactionCollector.insert( Vaction );
					__fmdp->nextAction();
				//~ std::cout << std::endl << "---- DONE ";
				}
				
				// *****************************************************************************************
				// Next to evaluate main value function, we take maximise over all action value, ...
				Vnew = NULL;
				//~ std::cout << std::endl << "---- Maximisation des diagrammes ";
				for( SetIterator< MultiDimDecisionDiagramBase< T_DATA >* > VActionsIter = VactionCollector.begin(); VActionsIter != VactionCollector.end(); ++VActionsIter ){
					Vtemp = Vnew;
					Vnew = maximize2MultiDimDecisionDiagrams( Vnew, *VActionsIter );
					delete Vtemp;
					delete *VActionsIter;
				}
				//~ std::cout << std::endl << "---- DONE ";
					
				
				// *****************************************************************************************
				// ... we multiply the result by the discount factor, ...
				Vtemp = reinterpret_cast<MultiDimDecisionDiagramBase<T_DATA>*>( Vnew->newFactory() );
				Vtemp->multiplyByScalar( Vnew, __fmdp->discount() );
				delete Vnew;
				Vnew = Vtemp;
				
				// *****************************************************************************************
				// ... and finally add reward
				Vtemp = Vnew;
				Vnew = add2MultiDimDecisionDiagrams( reinterpret_cast<const MultiDimDecisionDiagramBase<T_DATA>*>( __fmdp->reward() ), Vnew );
				delete Vtemp;
				
				// *****************************************************************************************
				// Then we compare new value function and the old one
					//~ std::cout << std::endl << "---- Gap des diagrammes "<< std::endl;
				const MultiDimDecisionDiagramBase< T_DATA >* Vnewcarre = multiply2MultiDimDecisionDiagrams( Vnew, Vnew );
				const MultiDimDecisionDiagramBase< T_DATA >* Voldcarre = multiply2MultiDimDecisionDiagrams( Vold, Vold );
				MultiDimDecisionDiagramBase< T_DATA >* deltaV = subtract2MultiDimDecisionDiagrams( Vnewcarre, Voldcarre );
				delete Voldcarre;
				delete Vnewcarre;
				gap = 0;
				for( BijectionIterator< NodeId, T_DATA > valIter = deltaV->valuesMap().begin(); valIter != deltaV->valuesMap().end(); ++valIter )
					if( gap < fabs( valIter.second() ) )
						gap = fabs( valIter.second() );
				delete deltaV;
				
				//~ std::cout << std::endl << "Gap max : " << gap << std::endl;
				
				// *****************************************************************************************
				// And eventually we update pointers for next loop
				delete Vold;
				Vold = Vnew;
			}
			//~ GUM_TRACE( "DONE!" );
			
			// *****************************************************************************************
			// We have to do one last step to get best policy
			// *****************************************************************************************
					__fmdp->resetActionsIterator();
					HashTable< Idx, MultiDimDecisionDiagramBase< T_DATA >* > VactionCollector;
					
					while( __fmdp->hasAction() ){
					
						__fmdp->resetVariablesIterator();
						
						MultiDimDecisionDiagramBase< T_DATA >* Vaction = __fmdp->primeCopy( Vold );
						
						while( __fmdp->hasVariable() ){
							
							Vtemp = Vaction;
							Vaction = multiply2MultiDimDecisionDiagrams( reinterpret_cast<const MultiDimDecisionDiagramBase<T_DATA>*>( __fmdp->transition() ), Vaction  );
							delete Vtemp;
								
							__fmdp->nextVariable();
						}
						
						Vtemp = Vaction;
						Vaction = projectSumMultiDimDecisionDiagram( Vaction, __fmdp->primedVariables() );
						delete Vtemp;
						
						Vtemp = reinterpret_cast<MultiDimDecisionDiagramBase<T_DATA>*>( Vaction->newFactory() );
						Vtemp->multiplyByScalar( Vaction, __fmdp->discount() );
						delete Vaction;
						Vaction = Vtemp;
						
						Vtemp = Vaction;
						Vaction = add2MultiDimDecisionDiagrams( reinterpret_cast<const MultiDimDecisionDiagramBase<T_DATA>*>( __fmdp->reward() ), Vaction  );
						delete Vtemp;
						
						VactionCollector.insert( __fmdp->actionIterId(), Vaction );
						__fmdp->nextAction();
					}
					
					Vnew = NULL;
					for( HashTableConstIterator< Idx, MultiDimDecisionDiagramBase< T_DATA >* > VActionsIter = VactionCollector.begin(); VActionsIter != VactionCollector.end(); ++VActionsIter ){
						
						//~ std::cout << std::endl << " Action : " << __fmdp->actionName( VActionsIter.key() ) << std::endl << (*VActionsIter)->toDot() << std::endl;
						Vtemp = Vnew;
						Vnew = maximize2MultiDimDecisionDiagrams( Vnew, *VActionsIter );
						delete Vtemp;
					}
					
					// *****************************************************************************************
					// And eventually we update pointers for next loop
					delete Vold;
					Vold = Vnew;
			
			// *****************************************************************************************
			// Then we look for best policy
			// *****************************************************************************************
			
				// *********************************************************
				// First of all we have to find all needed to be instantiate variables
				// in order to make good instantiation for next step.
				Sequence< const DiscreteVariable* > globalVarsSeq = Vnew->variablesSequence();
				Instantiation inst;
				for( SequenceIterator< const DiscreteVariable* > varIter = globalVarsSeq.begin(); varIter != globalVarsSeq.end(); ++varIter ){
					
					if( Vnew->isInDiagramVariable( *varIter ) ){
						inst.add( **varIter );
						continue;
					}
					for( HashTableConstIterator< Idx, MultiDimDecisionDiagramBase< T_DATA >* > VActionsIter = VactionCollector.begin(); VActionsIter != VactionCollector.end(); ++VActionsIter ){
						if( (*VActionsIter)->isInDiagramVariable( *varIter ) ){
							inst.add( **varIter );
							break;
						}
						
					}
					
				}
				
				for( inst.setFirst(); !inst.end(); inst.inc() ) {
					
					NodeId n = Vnew->getNode( inst );
					
					if( !__bestPolicy.exists( n ) )
						__bestPolicy.insert( n, new List<Idx> );
						
					for( HashTableConstIterator< Idx, MultiDimDecisionDiagramBase< T_DATA >* > VActionsIter = VactionCollector.begin(); VActionsIter != VactionCollector.end(); ++VActionsIter )
						if( Vnew->get( inst ) == (*VActionsIter)->get(inst) )
							if( !__bestPolicy[n]->exists( VActionsIter.key() ) )
								__bestPolicy[n]->insert( VActionsIter.key() );
								
				}
				
				for( HashTableConstIterator< Idx, MultiDimDecisionDiagramBase< T_DATA >* > VActionsIter = VactionCollector.begin(); VActionsIter != VactionCollector.end(); ++VActionsIter )
					delete *VActionsIter;
			
			// *****************************************************************************************
			// And eventually we display the result
			// *****************************************************************************************
			
				
				//~ std::stringstream output;
				//~ std::stringstream terminalStream;
				//~ std::stringstream nonTerminalStream;
				//~ std::stringstream arcstream;
				//~ std::stringstream defaultarcstream;
				//~ output << "digraph \"no_name\" {" << std::endl;
//~ 
				//~ terminalStream << "node [shape = box];" << std::endl;
				//~ nonTerminalStream << "node [shape = ellipse];" << std::endl;
				//~ std::string tab = "  ";
//~ 
				//~ for ( NodeGraphPart::NodeIterator nodeIter = Vold->nodesMap().beginNodes(); nodeIter != Vold->nodesMap().endNodes(); ++nodeIter ) {
					//~ 
					//~ if ( Vold->isTerminalNode( *nodeIter ) ){
						//~ 
						//~ terminalStream << tab << *nodeIter << ";" << tab << *nodeIter  << " [label=\"" << Vold->nodeValue( *nodeIter );
						//~ 
						//~ if( __bestPolicy.exists(*nodeIter) )
							//~ for( ListIterator<Idx> actionIter = __bestPolicy[*nodeIter]->begin(); actionIter != __bestPolicy[*nodeIter]->end(); ++actionIter )
								//~ terminalStream << " - " << __fmdp->actionName( *actionIter );
								//~ 
						//~ terminalStream << "\"]"<< ";" << std::endl;
						//~ 
					//~ } 
					//~ else 
					//~ {
						//~ 
						//~ nonTerminalStream << tab << *nodeIter << ";" << tab << *nodeIter  << " [label=\"" << Vold->nodeVariable( *nodeIter )->name() << "\"]"<< ";" << std::endl;
//~ 
						//~ for ( HashTableConstIterator<Idx,NodeId> arcIter = Vold->nodeSons(*nodeIter)->begin(); arcIter != Vold->nodeSons(*nodeIter)->end(); ++arcIter )
								//~ arcstream << tab <<  *nodeIter << " -> " << *arcIter << " [label=\"" << Vold->nodeVariable( *nodeIter )->label( arcIter.key() ) << "\",color=\"#0000ff\"]"<< ";" << std::endl;
//~ 
						//~ if (Vold->hasNodeDefaultSon( *nodeIter ) )
							//~ defaultarcstream << tab <<  *nodeIter << " -> " << Vold->nodeDefaultSon(*nodeIter) << " [color=\"#ff0000\"]"<< ";" << std::endl;
							//~ 
					//~ }
				//~ }
//~ 
				//~ output << terminalStream.str() << std::endl << nonTerminalStream.str() << std::endl <<  arcstream.str() << std::endl << defaultarcstream.str() << std::endl << std::endl << "}" << std::endl;
//~ 
				//~ std::cout << std::endl << output.str();
			
			// *****************************************************************************************
			// Cleaning
			// *****************************************************************************************
			for( HashTableConstIterator< NodeId, List<Idx>* > iter = __bestPolicy.begin(); iter != __bestPolicy.end(); ++iter )
				delete *iter;
				
			delete Vold;
			
		}
}
