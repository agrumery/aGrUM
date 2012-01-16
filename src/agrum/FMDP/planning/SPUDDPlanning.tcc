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
* @brief Template implementation of FMDP/planning/SPUDDPlanning.h classes.
*
* @author Jean-Christophe Magnan
*/
// #define TRACE_ALGO
// ============================================================================
#include <math.h>
// ============================================================================
#include <agrum/multidim/potential.h>
#include <agrum/multidim/instantiation.h>
// ============================================================================
#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
// ============================================================================
#ifdef TRACE_ALGO
#include <fstream>
#include <agrum/core/timer.h>
#endif
// ============================================================================

namespace gum {


  /* ************************************************************************************************** **/
  /* **                                                                                                                                                 **/
  /* **                                                Constructors / Destructors                                                       **/
  /* **                                                                                                                                                 **/
  /* ************************************************************************************************** **/

    // ===========================================================================
    // Default constructor
    // ===========================================================================
    template<typename T_DATA> INLINE
    SPUDDPlanning<T_DATA>::SPUDDPlanning ( FactoredMarkovDecisionProcess<T_DATA>* fmdp, const std::string saveFilesName, T_DATA epsilon ){
    
	GUM_CONSTRUCTOR( SPUDDPlanning );
	__epsilon = epsilon;
	__fmdp = fmdp;
      
#ifdef TRACE_ALGO
	std::string traceFileName = saveFilesName +"trace.log";
	__traceAlgoSaveFile.open(   traceFileName.c_str(), ios::out | ios::trunc); 
	if(!__traceAlgoSaveFile){
	    cerr << "Erreur à l'ouverture !" << endl;
	    return;
	}
#endif
    }

    // ===========================================================================
    // Default destructor
    // ===========================================================================
    template<typename T_DATA> INLINE
    SPUDDPlanning<T_DATA>::~SPUDDPlanning(){

      GUM_DESTRUCTOR( SPUDDPlanning );
      
#ifdef TRACE_ALGO
      __traceAlgoSaveFile.close();
#endif
    }


  /* ************************************************************************************************** **/
  /* **                                                                                                                                                 **/
  /* **                                                        General Methods                                                              **/
  /* **                                                                                                                                                 **/
  /* ************************************************************************************************** **/

    // ===========================================================================
    // Makes a spudd planning on FMDP
    // ===========================================================================
    template<typename T_DATA>
    void
    SPUDDPlanning<T_DATA>::makePlanning (  ){

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

	Idx nbIte = 0;
	// *****************************************************************************************
	// Main loop
	// *****************************************************************************************
	while( gap >  threshold ){
	    ++nbIte;
// 	    __traceAlgoSaveFile << ";" << nbIte << ";";
	    // *****************************************************************************************
	    // Loop reset
	    __fmdp->resetActionsIterator();
	    Set< MultiDimDecisionDiagramBase< T_DATA >* > VactionCollector;
	    Vnew = __fmdp->primeCopy( Vold );

	    // *****************************************************************************************
	    // For each action
	    while( __fmdp->hasAction() ){

		VactionCollector.insert( __evalActionValue( Vnew ) );

		__fmdp->nextAction();
	    }

	    // *****************************************************************************************
	    // Next to evaluate main value function, we take maximise over all action value, ...
	    delete Vnew;
	    Vnew = NULL;


#ifdef TRACE_ALGO
		List<Idx> nbNodeT1;
		List<Idx> nbNodeT2;
		List<Idx> nbVarRetroDirect;
		List<Idx> nbVarRetroIndirect;
		List<Idx> nbNodeFD;
		List<Idx> nbNodeFI;
		List<double> timeIndirect;
		List<double> timeDirect;
#endif

	    for( SetIterator< MultiDimDecisionDiagramBase< T_DATA >* > VActionsIter = VactionCollector.begin(); VActionsIter != VactionCollector.end(); ++VActionsIter ){
	      
		Vtemp = Vnew;
#ifdef TRACE_ALGO
		if( Vnew != NULL )
		    nbNodeT1.insert( Vnew->nodesMap().size() );
		else
		    nbNodeT1.insert( 0 );
		nbNodeT2.insert( (*VActionsIter)->nodesMap().size() );
		nbVarRetroDirect.insert( __evalNbRetrograde( Vnew, *VActionsIter ) );
		nbVarRetroIndirect.insert(  __evalNbRetrograde( *VActionsIter, Vnew ) );
		Timer time;
		time.reset();
		
		MultiDimDecisionDiagramBase<T_DATA>* Vtempo = maximize2MultiDimDecisionDiagrams(*VActionsIter, Vnew );
		
		timeIndirect.insert( time.step() );
		nbNodeFI.insert( Vtempo->nodesMap().size() );
		time.reset();
		
		delete Vtempo;
		
		time.reset();
		
		Vnew = maximize2MultiDimDecisionDiagrams( Vnew, *VActionsIter );
		
		timeDirect.insert( time.step() );
		nbNodeFD.insert( Vnew->nodesMap().size() );
#else
		Vnew = maximize2MultiDimDecisionDiagrams( Vnew, *VActionsIter );
#endif
		
		
		delete Vtemp;
		delete *VActionsIter;
	    }

	    
#ifdef TRACE_ALGO
		double moyenneNodeT1 = 0;
		double varNodeT1 = 0;
		for(ListIterator<Idx> iter = nbNodeT1.begin(); iter != nbNodeT1.end(); ++iter )
		    moyenneNodeT1 += *iter;
		moyenneNodeT1 /= nbNodeT1.size();
		for(ListIterator<Idx> iter = nbNodeT1.begin(); iter != nbNodeT1.end(); ++iter )
		    varNodeT1 += ( (double) (*iter - moyenneNodeT1) )*( (double) (*iter - moyenneNodeT1) );
		varNodeT1 /= nbNodeT1.size();
		
		double moyenneNodeT2 = 0;
		double varNodeT2 = 0;
		for(ListIterator<Idx> iter = nbNodeT2.begin(); iter != nbNodeT2.end(); ++iter )
		    moyenneNodeT2 += *iter;
		moyenneNodeT2 /= nbNodeT2.size();
		for(ListIterator<Idx> iter = nbNodeT2.begin(); iter != nbNodeT2.end(); ++iter )
		    varNodeT2 += ( *iter - moyenneNodeT2 )*( *iter - moyenneNodeT2 );
		varNodeT2 /= nbNodeT2.size();
		
		double moyenneVarRetroDirect = 0;
		double varVarRetroDirect = 0;
		for(ListIterator<Idx> iter = nbVarRetroDirect.begin(); iter != nbVarRetroDirect.end(); ++iter )
		    moyenneVarRetroDirect += *iter;
		moyenneVarRetroDirect /= nbVarRetroDirect.size();
		for(ListIterator<Idx> iter = nbVarRetroDirect.begin(); iter != nbVarRetroDirect.end(); ++iter )
		    varVarRetroDirect += ( *iter - moyenneVarRetroDirect )*( *iter - moyenneVarRetroDirect );
		varVarRetroDirect /= nbVarRetroDirect.size();
		
		double moyenneVarRetroIndirect = 0;
		double varVarRetroIndirect = 0;
		for(ListIterator<Idx> iter = nbVarRetroIndirect.begin(); iter != nbVarRetroIndirect.end(); ++iter )
		    moyenneVarRetroIndirect += *iter;
		moyenneVarRetroIndirect /= nbVarRetroIndirect.size();
		for(ListIterator<Idx> iter = nbVarRetroIndirect.begin(); iter != nbVarRetroIndirect.end(); ++iter )
		    varVarRetroIndirect += ( *iter - moyenneVarRetroIndirect )*( *iter - moyenneVarRetroIndirect );
		varVarRetroIndirect /= nbVarRetroIndirect.size();
		
		double moyenneNodeFD = 0;
		double varNodeFD = 0;
		for(ListIterator<Idx> iter = nbNodeFD.begin(); iter != nbNodeFD.end(); ++iter )
		    moyenneNodeFD += *iter;
		moyenneNodeFD /= nbNodeFD.size();
		for(ListIterator<Idx> iter = nbNodeFD.begin(); iter != nbNodeFD.end(); ++iter )
		    varNodeFD += ( (double) (*iter - moyenneNodeFD) )*( (double) (*iter - moyenneNodeFD) );
		varNodeFD /= nbNodeFD.size();
		
		double moyenneNodeFI = 0;
		double varNodeFI = 0;
		for(ListIterator<Idx> iter = nbNodeFI.begin(); iter != nbNodeFI.end(); ++iter )
		    moyenneNodeFI += *iter;
		moyenneNodeFI /= nbNodeFI.size();
		for(ListIterator<Idx> iter = nbNodeFI.begin(); iter != nbNodeFI.end(); ++iter )
		    varNodeFI += ( *iter - moyenneNodeFI )*( *iter - moyenneNodeFI );
		varNodeFI /= nbNodeFI.size();
		
		double moyennetimeIndirect = 0;
		double vartimeIndirect = 0;
		for(ListIterator<double> iter = timeIndirect.begin(); iter != timeIndirect.end(); ++iter )
		    moyennetimeIndirect += *iter;
		moyennetimeIndirect /= timeIndirect.size();
		for(ListIterator<double> iter = timeIndirect.begin(); iter != timeIndirect.end(); ++iter )
		    vartimeIndirect += ( *iter - moyennetimeIndirect )*( *iter - moyennetimeIndirect );
		vartimeIndirect /= timeIndirect.size();
		
		double moyennetimeDirect = 0;
		double vartimeDirect = 0;
		for(ListIterator<double> iter = timeDirect.begin(); iter != timeDirect.end(); ++iter )
		    moyennetimeDirect += *iter;
		moyennetimeDirect /= timeDirect.size();
		for(ListIterator<double> iter = timeDirect.begin(); iter != timeDirect.end(); ++iter )
		    vartimeDirect += ( *iter - moyennetimeDirect )*( *iter - moyennetimeDirect );
		vartimeDirect /= timeDirect.size();
		
		__traceAlgoSaveFile << ";" << moyenneNodeT1 << ";" << varNodeT1 << ";" << moyenneNodeT2 << ";" << varNodeT2 << ";" << moyenneVarRetroDirect << ";" << varVarRetroDirect << ";" 
			    << moyenneVarRetroIndirect << ";" << varVarRetroIndirect << ";" << moyenneNodeFD << ";" << varNodeFD << ";" << moyenneNodeFI << ";" << varNodeFI << ";" 
					<< moyennetimeDirect << ";" << vartimeDirect << ";" << moyennetimeIndirect << ";" << vartimeIndirect << ";";
#endif
			    
			    
	    // *******************************************************************************************
	    // Next, we eval the new function value
	    Vtemp = Vnew;
	    Vnew = __evalValueFunction( Vtemp );
	    delete Vtemp;


	    // *****************************************************************************************
	    // Then we compare new value function and the old one

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
		
// 	    std::cout << " ------------------- Fin itération n° " << nbIte << std::endl << " Gap : " << gap << std::endl;
	    // *****************************************************************************************
	    // And eventually we update pointers for next loop
	    delete Vold;
	    Vold = Vnew;
#ifdef TRACE_ALGO
	    __traceAlgoSaveFile << std::endl;
#endif
	}


	// *****************************************************************************************
	// We have to do one last step to get best policy
	// *****************************************************************************************

// 	// *****************************************************************************************
// 	// Loop reset
// 	__fmdp->resetActionsIterator();
// 	Set< MultiDimDecisionDiagramBase< T_DATA >* > VactionCollector;
// 	Vnew = __fmdp->primeCopy( Vold );
// 
// 	// *****************************************************************************************
// 	// For each action
// 	while( __fmdp->hasAction() ){
// 
// 	    MultiDimDecisionDiagramBase< T_DATA >* Vaction = __evalActionValue( Vnew );
// 	    
// 	    Vtemp = reinterpret_cast<MultiDimDecisionDiagramBase<T_DATA>*>( Vaction->newFactory() );
// 	    Vtemp->multiplyByScalar( Vaction, __fmdp->discount() );
// 	    delete Vaction;
// 	    Vaction = Vtemp;
// 	    
// 	    Vtemp = Vaction;
// 	    Vaction = add2MultiDimDecisionDiagrams( reinterpret_cast<const MultiDimDecisionDiagramBase<T_DATA>*>( __fmdp->reward() ), Vaction  );
// 	    delete Vtemp;
// 	    
// 	    VactionCollector.insert( __fmdp->actionIterId(), Vaction );
// 	    __fmdp->nextAction();
// 	}
// 	
// 	Vnew = NULL;
// 	for( HashTableConstIterator< Idx, MultiDimDecisionDiagramBase< T_DATA >* > VActionsIter = VactionCollector.begin(); VActionsIter != VactionCollector.end(); ++VActionsIter ){
// 	
// 	    //std::cout << std::endl << " Action : " << __fmdp->actionName( VActionsIter.key() ) << std::endl << (*VActionsIter)->toDot() << std::endl;
// 	    Vtemp = Vnew;
// 	    Vnew = maximize2MultiDimDecisionDiagrams( Vnew, *VActionsIter );
// 	    delete Vtemp;
// 	}
// 	
// 	// *****************************************************************************************
// 	// And eventually we update pointers for next loop
// 	delete Vold;
// 	Vold = Vnew;

    // *****************************************************************************************
    // Then we look for best policy
    // *****************************************************************************************

    // *********************************************************
    // First of all we have to find all needed to be instantiate variables
    // in order to make good instantiation for next step.
    // Sequence< const DiscreteVariable* > globalVarsSeq = Vnew->variablesSequence();
    // Instantiation inst;
    // for( SequenceIterator< const DiscreteVariable* > varIter = globalVarsSeq.begin(); varIter != globalVarsSeq.end(); ++varIter ){
    // 
    // if( Vnew->isInDiagramVariable( *varIter ) ){
    // inst.add( **varIter );
    // continue;
    // }
    // for( HashTableConstIterator< Idx, MultiDimDecisionDiagramBase< T_DATA >* > VActionsIter = VactionCollector.begin(); VActionsIter != VactionCollector.end(); ++VActionsIter ){
    // if( (*VActionsIter)->isInDiagramVariable( *varIter ) ){
    // inst.add( **varIter );
    // break;
    // }
    // 
    // }
    // 
    // }
    // 
    // for( inst.setFirst(); !inst.end(); inst.inc() ) {
    // 
    // NodeId n = Vnew->getNode( inst );
    // 
    // if( !__bestPolicy.exists( n ) )
    // __bestPolicy.insert( n, new List<Idx> );
    // 
    // for( HashTableConstIterator< Idx, MultiDimDecisionDiagramBase< T_DATA >* > VActionsIter = VactionCollector.begin(); VActionsIter != VactionCollector.end(); ++VActionsIter )
    // if( Vnew->get( inst ) == (*VActionsIter)->get(inst) )
    // if( !__bestPolicy[n]->exists( VActionsIter.key() ) )
    // __bestPolicy[n]->insert( VActionsIter.key() );
    // 
    // }
    // 
    // for( HashTableConstIterator< Idx, MultiDimDecisionDiagramBase< T_DATA >* > VActionsIter = VactionCollector.begin(); VActionsIter != VactionCollector.end(); ++VActionsIter )
    // delete *VActionsIter;

    // *****************************************************************************************
    // And eventually we display the result
    // *****************************************************************************************


    // std::stringstream output;
    // std::stringstream terminalStream;
    // std::stringstream nonTerminalStream;
    // std::stringstream arcstream;
    // std::stringstream defaultarcstream;
    // output << "digraph \"no_name\" {" << std::endl;
    // 
    // terminalStream << "node [shape = box];" << std::endl;
    // nonTerminalStream << "node [shape = ellipse];" << std::endl;
    // std::string tab = "  ";
    // 
    // for ( NodeGraphPart::NodeIterator nodeIter = Vold->nodesMap().beginNodes(); nodeIter != Vold->nodesMap().endNodes(); ++nodeIter ) {
    // 
    // if ( Vold->isTerminalNode( *nodeIter ) ){
    // 
    // terminalStream << tab << *nodeIter << ";" << tab << *nodeIter  << " [label=\"" << Vold->nodeValue( *nodeIter );
    // 
    // if( __bestPolicy.exists(*nodeIter) )
    // for( ListIterator<Idx> actionIter = __bestPolicy[*nodeIter]->begin(); actionIter != __bestPolicy[*nodeIter]->end(); ++actionIter )
    // terminalStream << " - " << __fmdp->actionName( *actionIter );
    // 
    // terminalStream << "\"]"<< ";" << std::endl;
    // 
    // } 
    // else 
    // {
    // 
    // nonTerminalStream << tab << *nodeIter << ";" << tab << *nodeIter  << " [label=\"" << Vold->nodeVariable( *nodeIter )->name() << "\"]"<< ";" << std::endl;
    // 
    // for ( HashTableConstIterator<Idx,NodeId> arcIter = Vold->nodeSons(*nodeIter)->begin(); arcIter != Vold->nodeSons(*nodeIter)->end(); ++arcIter )
    // arcstream << tab <<  *nodeIter << " -> " << *arcIter << " [label=\"" << Vold->nodeVariable( *nodeIter )->label( arcIter.key() ) << "\",color=\"#0000ff\"]"<< ";" << std::endl;
    // 
    // if (Vold->hasNodeDefaultSon( *nodeIter ) )
    // defaultarcstream << tab <<  *nodeIter << " -> " << Vold->nodeDefaultSon(*nodeIter) << " [color=\"#ff0000\"]"<< ";" << std::endl;
    // 
    // }
    // }
    // 
    // output << terminalStream.str() << std::endl << nonTerminalStream.str() << std::endl <<  arcstream.str() << std::endl << defaultarcstream.str() << std::endl << std::endl << "}" << std::endl;
    // 
    // // // std::cout << std::endl << output.str();

    // *****************************************************************************************
    // Cleaning
    // *****************************************************************************************
    // for( HashTableConstIterator< NodeId, List<Idx>* > iter = __bestPolicy.begin(); iter != __bestPolicy.end(); ++iter )
    // delete *iter;

    delete Vold;

    }

    template<typename T_DATA>
    MultiDimDecisionDiagramBase<T_DATA>*
    SPUDDPlanning<T_DATA>::__evalActionValue( const MultiDimDecisionDiagramBase< T_DATA >* Vold ){

	__fmdp->resetVariablesIterator();

	MultiDimDecisionDiagramBase< T_DATA >* Vaction = reinterpret_cast<MultiDimDecisionDiagramBase<T_DATA>*>( Vold->newFactory() );
	Vaction->copy( *Vold );
	MultiDimDecisionDiagramBase< T_DATA >* Vtemp = NULL;


#ifdef TRACE_ALGO
		List<Idx> nbNodeT1;
		List<Idx> nbNodeT2;
		List<Idx> nbVarRetroDirect;
		List<Idx> nbVarRetroIndirect;
		List<Idx> nbNodeFD;
		List<Idx> nbNodeFI;
		List<double> timeIndirect;
		List<double> timeDirect;
#endif
		
	// *****************************************************************************************
	// To evaluate action value function, we multiply old main value function by transition table
	// of each variable
	while( __fmdp->hasVariable() ){

	    // =======================================================================================
	    // Multiplication of Vaction by current variable's CPT
	    
#ifdef TRACE_ALGO
	    MultiDimDecisionDiagramBase< T_DATA >* Vtemp2 = NULL;
	
	    nbNodeT1.insert( Vaction->nodesMap().size() );
	    nbNodeT2.insert( (reinterpret_cast<const MultiDimDecisionDiagramBase<T_DATA>*>( __fmdp->transition() ))->nodesMap().size() );
	    nbVarRetroDirect.insert( __evalNbRetrograde( Vaction, reinterpret_cast<const MultiDimDecisionDiagramBase<T_DATA>*>( __fmdp->transition() ) ) );
	    nbVarRetroIndirect.insert(  __evalNbRetrograde( reinterpret_cast<const MultiDimDecisionDiagramBase<T_DATA>*>( __fmdp->transition() ), Vaction ) );
	    Timer time;
	    time.reset();
	    
	    Vtemp2 = multiply2MultiDimDecisionDiagrams( reinterpret_cast<const MultiDimDecisionDiagramBase<T_DATA>*>( __fmdp->transition() ), Vaction );
	    Set< const DiscreteVariable* > varSet;
	    varSet << __fmdp->variable();
	    Vtemp = projectSumMultiDimDecisionDiagram( Vtemp2, varSet );
	    delete Vtemp2;
	    
	    timeIndirect.insert( time.step() );
	    nbNodeFI.insert( Vtemp->nodesMap().size() );
	    time.reset();
	    
	    delete Vtemp;
	    
	    time.reset();
	    
	    Vtemp = Vaction;
	    Vaction = multiply2MultiDimDecisionDiagrams( Vaction, reinterpret_cast<const MultiDimDecisionDiagramBase<T_DATA>*>( __fmdp->transition() )  );
	    delete Vtemp;
	    Vtemp = Vaction;
	    varSet.clear();
	    varSet << __fmdp->variable();
	    Vaction = projectSumMultiDimDecisionDiagram( Vaction, varSet );
	    delete Vtemp;
	    
	    timeDirect.insert( time.step() );
	    nbNodeFD.insert( Vtemp->nodesMap().size() );
#else
		
	    Vtemp = Vaction;
	    Vaction = multiply2MultiDimDecisionDiagrams(  Vaction, reinterpret_cast<const MultiDimDecisionDiagramBase<T_DATA>*>( __fmdp->transition() ) );
	    delete Vtemp;
	    // =======================================================================================


	    // =======================================================================================
	    // Projection of Vaction on current var by summing on each of its modalities
	    Vtemp = Vaction;
	    Set< const DiscreteVariable* > varSet;
	    varSet << __fmdp->variable();
	    Vaction = projectSumMultiDimDecisionDiagram( Vaction, varSet );
	    delete Vtemp;
	    // =======================================================================================
#endif
	    __fmdp->nextVariable();
	}

#ifdef TRACE_ALGO
		double moyenneNodeT1 = 0;
		double varNodeT1 = 0;
		for(ListIterator<Idx> iter = nbNodeT1.begin(); iter != nbNodeT1.end(); ++iter )
		    moyenneNodeT1 += *iter;
		moyenneNodeT1 /= nbNodeT1.size();
		for(ListIterator<Idx> iter = nbNodeT1.begin(); iter != nbNodeT1.end(); ++iter )
		    varNodeT1 += ( (double) (*iter - moyenneNodeT1) )*( (double) (*iter - moyenneNodeT1) );
		varNodeT1 /= nbNodeT1.size();
		
		double moyenneNodeT2 = 0;
		double varNodeT2 = 0;
		for(ListIterator<Idx> iter = nbNodeT2.begin(); iter != nbNodeT2.end(); ++iter )
		    moyenneNodeT2 += *iter;
		moyenneNodeT2 /= nbNodeT2.size();
		for(ListIterator<Idx> iter = nbNodeT2.begin(); iter != nbNodeT2.end(); ++iter )
		    varNodeT2 += ( *iter - moyenneNodeT2 )*( *iter - moyenneNodeT2 );
		varNodeT2 /= nbNodeT2.size();
		
		double moyenneVarRetroDirect = 0;
		double varVarRetroDirect = 0;
		for(ListIterator<Idx> iter = nbVarRetroDirect.begin(); iter != nbVarRetroDirect.end(); ++iter )
		    moyenneVarRetroDirect += *iter;
		moyenneVarRetroDirect /= nbVarRetroDirect.size();
		for(ListIterator<Idx> iter = nbVarRetroDirect.begin(); iter != nbVarRetroDirect.end(); ++iter )
		    varVarRetroDirect += ( *iter - moyenneVarRetroDirect )*( *iter - moyenneVarRetroDirect );
		varVarRetroDirect /= nbVarRetroDirect.size();
		
		double moyenneVarRetroIndirect = 0;
		double varVarRetroIndirect = 0;
		for(ListIterator<Idx> iter = nbVarRetroIndirect.begin(); iter != nbVarRetroIndirect.end(); ++iter )
		    moyenneVarRetroIndirect += *iter;
		moyenneVarRetroIndirect /= nbVarRetroIndirect.size();
		for(ListIterator<Idx> iter = nbVarRetroIndirect.begin(); iter != nbVarRetroIndirect.end(); ++iter )
		    varVarRetroIndirect += ( *iter - moyenneVarRetroIndirect )*( *iter - moyenneVarRetroIndirect );
		varVarRetroIndirect /= nbVarRetroIndirect.size();
		
		double moyenneNodeFD = 0;
		double varNodeFD = 0;
		for(ListIterator<Idx> iter = nbNodeFD.begin(); iter != nbNodeFD.end(); ++iter )
		    moyenneNodeFD += *iter;
		moyenneNodeFD /= nbNodeFD.size();
		for(ListIterator<Idx> iter = nbNodeFD.begin(); iter != nbNodeFD.end(); ++iter )
		    varNodeFD += ( (double) (*iter - moyenneNodeFD) )*( (double) (*iter - moyenneNodeFD) );
		varNodeFD /= nbNodeFD.size();
		
		double moyenneNodeFI = 0;
		double varNodeFI = 0;
		for(ListIterator<Idx> iter = nbNodeFI.begin(); iter != nbNodeFI.end(); ++iter )
		    moyenneNodeFI += *iter;
		moyenneNodeFI /= nbNodeFI.size();
		for(ListIterator<Idx> iter = nbNodeFI.begin(); iter != nbNodeFI.end(); ++iter )
		    varNodeFI += ( *iter - moyenneNodeFI )*( *iter - moyenneNodeFI );
		varNodeFI /= nbNodeFI.size();
		
		double moyennetimeIndirect = 0;
		double vartimeIndirect = 0;
		for(ListIterator<double> iter = timeIndirect.begin(); iter != timeIndirect.end(); ++iter )
		    moyennetimeIndirect += *iter;
		moyennetimeIndirect /= timeIndirect.size();
		for(ListIterator<double> iter = timeIndirect.begin(); iter != timeIndirect.end(); ++iter )
		    vartimeIndirect += ( *iter - moyennetimeIndirect )*( *iter - moyennetimeIndirect );
		vartimeIndirect /= timeIndirect.size();
		
		double moyennetimeDirect = 0;
		double vartimeDirect = 0;
		for(ListIterator<double> iter = timeDirect.begin(); iter != timeDirect.end(); ++iter )
		    moyennetimeDirect += *iter;
		moyennetimeDirect /= timeDirect.size();
		for(ListIterator<double> iter = timeDirect.begin(); iter != timeDirect.end(); ++iter )
		    vartimeDirect += ( *iter - moyennetimeDirect )*( *iter - moyennetimeDirect );
		vartimeDirect /= timeDirect.size();
		
		__traceAlgoSaveFile << ";" << moyenneNodeT1 << ";" << varNodeT1 << ";" << moyenneNodeT2 << ";" << varNodeT2 << ";" << moyenneVarRetroDirect << ";" << varVarRetroDirect << ";" 
			    << moyenneVarRetroIndirect << ";" << varVarRetroIndirect << ";" << moyenneNodeFD << ";" << varNodeFD << ";" << moyenneNodeFI << ";" << varNodeFI << ";" 
					<< moyennetimeDirect << ";" << vartimeDirect << ";" << moyennetimeIndirect << ";" << vartimeIndirect << ";";
#endif
	return Vaction;
    }

    template<typename T_DATA>
    MultiDimDecisionDiagramBase<T_DATA>*
    SPUDDPlanning<T_DATA>::__evalValueFunction( const MultiDimDecisionDiagramBase< T_DATA >* Vold ){

	// *****************************************************************************************
	// ... we multiply the result by the discount factor, ...
	MultiDimDecisionDiagramBase< T_DATA >* Vnew = reinterpret_cast<MultiDimDecisionDiagramBase<T_DATA>*>( Vold->newFactory() );
	Vnew->multiplyByScalar( Vold, __fmdp->discount() );

	// *****************************************************************************************
	// ... and finally add reward
	
	MultiDimDecisionDiagramBase< T_DATA >* Vtemp = Vnew;

#ifdef TRACE_ALGO
		Idx nbNodeT1 = Vnew->nodesMap().size();
		Idx nbNodeT2 = (reinterpret_cast<const MultiDimDecisionDiagramBase<T_DATA>*>( __fmdp->reward() ))->nodesMap().size();
		Idx nbVarRetroDirect = __evalNbRetrograde( Vnew, reinterpret_cast<const MultiDimDecisionDiagramBase<T_DATA>*>( __fmdp->reward() ) );
		Idx nbVarRetroIndirect = __evalNbRetrograde( reinterpret_cast<const MultiDimDecisionDiagramBase<T_DATA>*>( __fmdp->reward() ), Vnew );
		Timer time;
		time.reset();
		MultiDimDecisionDiagramBase< T_DATA >* Vtempo = add2MultiDimDecisionDiagrams( reinterpret_cast<const MultiDimDecisionDiagramBase<T_DATA>*>( __fmdp->reward() ), Vtemp );
		double timeIndirect = time.step();
		Idx nbNodeFI = Vtempo->nodesMap().size();
		time.reset();
		delete Vtempo;
		time.reset();
		Vnew = add2MultiDimDecisionDiagrams(  Vtemp, reinterpret_cast<const MultiDimDecisionDiagramBase<T_DATA>*>( __fmdp->reward() ) );
		double timeDirect = time.step();
		Idx nbNodeFD = Vnew->nodesMap().size();
		__traceAlgoSaveFile << ";" << nbNodeT1 << ";" << nbNodeT2 << ";" << nbVarRetroDirect << ";" << nbVarRetroIndirect << ";" << nbNodeFI << ";" << nbNodeFD << ";"  << timeDirect << ";" << timeIndirect << ";";
#else
		Vnew = add2MultiDimDecisionDiagrams( reinterpret_cast<const MultiDimDecisionDiagramBase<T_DATA>*>( __fmdp->reward() ), Vtemp );
#endif
	
	delete Vtemp;
	
	return Vnew;
    }

// #define TRACE_ALGO
#ifdef TRACE_ALGO
    // ===========================================================================
    // Evals how many retrograde variable there will be if we do the operation in that order
    // ===========================================================================
    template<typename T_DATA>
    Idx
    SPUDDPlanning<T_DATA>::__evalNbRetrograde( const MultiDimDecisionDiagramBase<T_DATA>* t1, const MultiDimDecisionDiagramBase<T_DATA>* t2 ){
      
	if( t1 == NULL || t2 == NULL )
	return 0;

	// =========================================================================================================
	// First we determine the new var sequence

	Sequence< const DiscreteVariable* > a1VarSeq = t1->variablesSequence();
	Sequence< const DiscreteVariable* > a2VarSeq = t2->variablesSequence();
	Sequence< const DiscreteVariable* > fusVarSeq;  

	SequenceIterator< const DiscreteVariable* > iterS1 = a1VarSeq.begin(); 
	SequenceIterator< const DiscreteVariable* > iterS2 = a2VarSeq.begin();

	while( iterS1 != a1VarSeq.end() || iterS2 != a2VarSeq.end() ){
	    if( iterS1 == a1VarSeq.end() ){
		for( ; iterS2 != a2VarSeq.end(); ++iterS2 )
		    if( !fusVarSeq.exists(*iterS2) )
			fusVarSeq.insert( *iterS2 );
	    }
	    else if( iterS2 == a2VarSeq.end() ){
		for( ; iterS1 != a1VarSeq.end(); ++iterS1 )
		    if( !fusVarSeq.exists(*iterS1) )
			fusVarSeq.insert( *iterS1 );
	    } else {
		if( *iterS1 == *iterS2 ){
		    if( !fusVarSeq.exists(*iterS1) )
			fusVarSeq.insert( *iterS1 );
		    ++iterS1;
		    ++iterS2;
		    continue;
		}
		if( a1VarSeq.pos(*iterS1) <= a2VarSeq.pos(*iterS2) || a1VarSeq.exists( *iterS2 ) ){
		    if( !fusVarSeq.exists(*iterS1) )
			fusVarSeq.insert( *iterS1 );
		    ++iterS1;
		    continue;
		} else {
		    if( !fusVarSeq.exists(*iterS2) )
			fusVarSeq.insert( *iterS2 );
		    ++iterS2;
		    continue;
		}
	    }
	}

	// =========================================================================================================
	// Then we search in second diagram for possible preneeded variable
	HashTable< NodeId, Set< const DiscreteVariable* >* > retrogradeVarTable;
	t2->findRetrogradeVariables( &fusVarSeq, &retrogradeVarTable );
	Set< const DiscreteVariable* > watchedRetroVar;

	for( HashTableIterator< NodeId, Set< const DiscreteVariable* >* > retroVarIter = retrogradeVarTable.begin(); retroVarIter != retrogradeVarTable.end(); ++retroVarIter )
	    if( *retroVarIter != NULL && !(*retroVarIter)->empty())
		for( SetIterator< const DiscreteVariable* > iter = (*retroVarIter)->begin(); iter != (*retroVarIter)->end(); ++iter )
		    if( ! watchedRetroVar.exists( *iter ) )
			watchedRetroVar.insert( *iter );

	for( HashTableIterator< NodeId, Set< const DiscreteVariable* >* > iterH = retrogradeVarTable.begin(); iterH != retrogradeVarTable.end(); ++iterH )
	    delete *iterH;
	
	return watchedRetroVar.size();
    }
#endif
}
