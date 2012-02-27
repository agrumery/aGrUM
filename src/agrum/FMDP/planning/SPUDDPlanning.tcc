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
    SPUDDPlanning<T_DATA>::SPUDDPlanning ( FactoredMarkovDecisionProcess<T_DATA>* fmdp, T_DATA epsilon ){
    
	GUM_CONSTRUCTOR( SPUDDPlanning );
	__epsilon = epsilon;
	__fmdp = fmdp;
 
    }

    // ===========================================================================
    // Default destructor
    // ===========================================================================
    template<typename T_DATA> INLINE
    SPUDDPlanning<T_DATA>::~SPUDDPlanning(){

      GUM_DESTRUCTOR( SPUDDPlanning );
      
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
	    T_DATA threshold = this->__epsilon * ( 1 - __fmdp->discount() ) / ( 2 * __fmdp->discount() );

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

	    // *****************************************************************************************
	    // Loop reset
	    __fmdp->resetActionsIterator();
	    Set< MultiDimDecisionDiagramBase< T_DATA >* > VactionCollector;

	    Vnew = reinterpret_cast<MultiDimDecisionDiagramBase<T_DATA>*>( Vold->newFactory() );
	    Vnew->copyAndReassign( Vold, __fmdp->main2prime() );

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

	    for( SetIterator< MultiDimDecisionDiagramBase< T_DATA >* > VActionsIter = VactionCollector.begin(); VActionsIter != VactionCollector.end(); ++VActionsIter ){

		Vtemp = Vnew;
		Vnew = maximize2MultiDimDecisionDiagrams( Vnew, *VActionsIter );		

		delete Vtemp;
		delete *VActionsIter;
	    }


	    // *******************************************************************************************
	    // Next, we eval the new function value
	    Vtemp = Vnew;
	    Vnew = __addReward( Vtemp );
	    delete Vtemp;


	    // *****************************************************************************************
	    // Then we compare new value function and the old one

	    MultiDimDecisionDiagramBase< T_DATA >* deltaV = subtract2MultiDimDecisionDiagrams( Vnew, Vold );
	    gap = 0;
	    for( BijectionIterator< NodeId, T_DATA > valIter = deltaV->valuesMap().begin(); valIter != deltaV->valuesMap().end(); ++valIter )
		if( gap < fabs( valIter.second() ) )
		    gap = fabs( valIter.second() );
	    delete deltaV;

	    // std::cout << " ------------------- Fin itération n° " << nbIte << std::endl << " Gap : " << gap << std::endl;
	    // *****************************************************************************************
	    // And eventually we update pointers for next loop
	    delete Vold;
	    Vold = Vnew;
	}
	delete Vold;
    }



    // ===========================================================================
    // Evals the policy corresponding to the given value function
    // ===========================================================================
    template<typename T_DATA>
    std::pair< MultiDimDecisionDiagramBase< T_DATA >*, HashTable< NodeId, List<Idx>* >* >
    SPUDDPlanning<T_DATA>::__evalPolicy( const MultiDimDecisionDiagramBase< T_DATA >* V ){


	// *****************************************************************************************
	// We have to do one last step to get best policy
	// *****************************************************************************************

	    // *****************************************************************************************
	    // Loop reset
	    __fmdp->resetActionsIterator();
	    Bijection< Idx, MultiDimDecisionDiagramBase< T_DATA >* > VactionCollector;
	    MultiDimDecisionDiagramBase< T_DATA >* Vnew = __fmdp->primeCopy( V );
	    MultiDimDecisionDiagramBase< T_DATA >* Vtemp;

	    // *****************************************************************************************
	    // For each action
	    while( __fmdp->hasAction() ){

		MultiDimDecisionDiagramBase< T_DATA >* Vaction = __evalActionValue( Vnew );

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

	    delete Vnew;
	    Vnew = NULL;
	    for( BijectionIterator< Idx, MultiDimDecisionDiagramBase< T_DATA >* > VActionsIter = VactionCollector.begin(); VActionsIter != VactionCollector.end(); ++VActionsIter ){

		//// // std::cout << std::endl << " Action : " << __fmdp->actionName( VActionsIter.key() ) << std::endl << (*VActionsIter)->toDot() << std::endl;
		Vtemp = Vnew;
		Vnew = maximize2MultiDimDecisionDiagrams( Vnew, VActionsIter.second() );
		delete Vtemp;
	    }

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
		for( BijectionIterator< Idx, MultiDimDecisionDiagramBase< T_DATA >* > VActionsIter = VactionCollector.begin(); VActionsIter != VactionCollector.end(); ++VActionsIter ){
		    if( (VActionsIter.second())->isInDiagramVariable( *varIter ) ){
			inst.add( **varIter );
			break;
		    }
		}
	    }

	    for( inst.setFirst(); !inst.end(); inst.inc() ) {

		NodeId n = Vnew->getNode( inst );

		if( !__bestPolicy.exists( n ) )
		    __bestPolicy.insert( n, new List<Idx> );

		for( BijectionIterator< Idx, MultiDimDecisionDiagramBase< T_DATA >* > VActionsIter = VactionCollector.begin(); VActionsIter != VactionCollector.end(); ++VActionsIter )
		    if( Vnew->get( inst ) == (VActionsIter.second())->get(inst) )
			if( !__bestPolicy[n]->exists( VActionsIter.first() ) )
			    __bestPolicy[n]->insert( VActionsIter.first() );

	    }

	    for( BijectionIterator< Idx, MultiDimDecisionDiagramBase< T_DATA >* > VActionsIter = VactionCollector.begin(); VActionsIter != VactionCollector.end(); ++VActionsIter )
		delete VActionsIter.second();

	// *****************************************************************************************
	// And eventually we display the result
	// *****************************************************************************************


	std::stringstream output;
	std::stringstream terminalStream;
	std::stringstream nonTerminalStream;
	std::stringstream arcstream;
	std::stringstream defaultarcstream;
	output << "digraph \"no_name\" {" << std::endl;

	terminalStream << "node [shape = box];" << std::endl;
	nonTerminalStream << "node [shape = ellipse];" << std::endl;
	std::string tab = "  ";

	for ( NodeGraphPart::NodeIterator nodeIter = Vnew->nodesMap().beginNodes(); nodeIter != Vnew->nodesMap().endNodes(); ++nodeIter ) {
	    if( *nodeIter != 0 ){

		if ( Vnew->isTerminalNode( *nodeIter ) ){

		    terminalStream << tab << *nodeIter << ";" << tab << *nodeIter  << " [label=\"" << Vnew->nodeValue( *nodeIter );

		    if( __bestPolicy.exists(*nodeIter) )
			for( ListIterator<Idx> actionIter = __bestPolicy[*nodeIter]->begin(); actionIter != __bestPolicy[*nodeIter]->end(); ++actionIter )
			    terminalStream << " - " << __fmdp->actionName( *actionIter );

		    terminalStream << "\"]"<< ";" << std::endl;

		} 
		else 
		{
		    nonTerminalStream << tab << *nodeIter << ";" << tab << *nodeIter  << " [label=\"" << *nodeIter << "-" << Vnew->nodeVariable( *nodeIter )->name() << "\"]"<< ";" << std::endl;

		    if (  Vnew->nodeSons( *nodeIter ) != NULL ){
			for ( std::vector<NodeId>::const_iterator sonIter =  Vnew->nodeSons( *nodeIter )->begin(); sonIter != Vnew->nodeSons( *nodeIter )->end(); ++sonIter)
			    if( *sonIter != 0 )
				arcstream << tab <<  *nodeIter << " -> " << *sonIter << " [label=\"" << Vnew->nodeVariable( *nodeIter )->label( std::distance( Vnew->nodeSons( *nodeIter )->begin(), sonIter ) ) << "\",color=\"#0000ff\"]"<< ";" << std::endl;
		    }

		    if ( Vnew->hasNodeDefaultSon( *nodeIter ) )
			defaultarcstream << tab <<  *nodeIter << " -> " << Vnew->hasNodeDefaultSon(*nodeIter ) << " [color=\"#ff0000\"]"<< ";" << std::endl;

		}
	    }
	}

	output << terminalStream.str() << std::endl << nonTerminalStream.str() << std::endl <<  arcstream.str() << std::endl << defaultarcstream.str() << std::endl << std::endl << "}" << std::endl;

	// std::cout << std::endl << output.str();

	// *****************************************************************************************
	// Cleaning
	// *****************************************************************************************
	for( HashTableConstIterator< NodeId, List<Idx>* > iter = __bestPolicy.begin(); iter != __bestPolicy.end(); ++iter )
	    delete *iter;

	// delete Vnew;

	std::pair< MultiDimDecisionDiagramBase< T_DATA >*, HashTable< NodeId, List<Idx>* > > res;
	res.first() = Vnew;
	res.second() = new HashTable< NodeId, List<Idx>* >( __bestPolicy );
	return res;
    }



    // ===========================================================================
    // Evals the value function for current fmdp action
    // ===========================================================================
    template<typename T_DATA>
    MultiDimDecisionDiagramBase<T_DATA>*
    SPUDDPlanning<T_DATA>::__evalActionValue( const MultiDimDecisionDiagramBase< T_DATA >* Vold ){

	// *****************************************************************************************
	// Initialisation
	__fmdp->resetVariablesIterator();

	MultiDimDecisionDiagramBase< T_DATA >* Vaction = reinterpret_cast<MultiDimDecisionDiagramBase<T_DATA>*>( Vold->newFactory() );
	Vaction->copy( *Vold );
	MultiDimDecisionDiagramBase< T_DATA >* Vtemp = NULL;

	// *****************************************************************************************
	// To evaluate action value function, we multiply old main value function by transition table
	// of each variable
	while( __fmdp->hasVariable() ){

	    // ***************************************************************************************
	    // Multiplication of Vaction by current variable's CPT
	    Vtemp = Vaction;
	    Vaction = multiply2MultiDimDecisionDiagrams(  Vaction, reinterpret_cast<const MultiDimDecisionDiagramBase<T_DATA>*>( __fmdp->transition() ) );
	    delete Vtemp;

	    // ***************************************************************************************
	    // Projection of Vaction on current var by summing on each of its modalities
	    Vtemp = Vaction;
	    Set< const DiscreteVariable* > varSet;
	    varSet << __fmdp->variable();
	    Vaction = projectSumMultiDimDecisionDiagram( Vaction, varSet );
	    delete Vtemp;

	    __fmdp->nextVariable();
	}

	return Vaction;

    }
    
    

    // ===========================================================================
    // Updates the value function by multiplying by discount and adding reward
    // ===========================================================================
    template<typename T_DATA>
    MultiDimDecisionDiagramBase<T_DATA>*
    SPUDDPlanning<T_DATA>::__addReward( const MultiDimDecisionDiagramBase< T_DATA >* Vold ){

	// *****************************************************************************************
	// ... we multiply the result by the discount factor, ...
	MultiDimDecisionDiagramBase< T_DATA >* Vnew = reinterpret_cast<MultiDimDecisionDiagramBase<T_DATA>*>( Vold->newFactory() );
	Vnew->multiplyByScalar( Vold, __fmdp->discount() );

	// *****************************************************************************************
	// ... and finally add reward
	MultiDimDecisionDiagramBase< T_DATA >* Vtemp = Vnew;
	Vnew = add2MultiDimDecisionDiagrams( reinterpret_cast<const MultiDimDecisionDiagramBase<T_DATA>*>( __fmdp->reward() ), Vtemp );

	delete Vtemp;

	return Vnew;

    }


/* ************************************************************************************************** **/
/* **                                                                                                                                                 **/
/* **                                                        Analyse Methods                                                              **/
/* **                                                                                                                                                 **/
/* ************************************************************************************************** **/  

    // ===========================================================================
    // Called for evaluation on algorithm
    // ===========================================================================
    template<typename T_DATA>
   MultiDimDecisionDiagramBase< T_DATA >*
    SPUDDPlanning<T_DATA>::makePlanningAlgoEvaluation ( const std::string saveFilesName, Idx mode ){

	// *****************************************************************************************
	// Initialisation
	// *****************************************************************************************

	    // *****************************************************************************************
	    // Threshold stopping criterion evaluation
	    T_DATA threshold =  0.1;//this->__epsilon * ( 1 - __fmdp->discount() ) / ( 2 * __fmdp->discount() );

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

	    // *****************************************************************************************
	    // Loop reset
	    __fmdp->resetActionsIterator();
	    Set< MultiDimDecisionDiagramBase< T_DATA >* > VactionCollector;

	    Vnew = reinterpret_cast<MultiDimDecisionDiagramBase<T_DATA>*>( Vold->newFactory() );
	    Vnew->copyAndReassign( Vold, __fmdp->main2prime() );

	    // *****************************************************************************************
	    // For each action
	    while( __fmdp->hasAction() ){

// std::cout << " ------------ Evaluation Nouvelle Action " << std::endl;
		VactionCollector.insert( __evalActionValueAlgoEvaluation( Vnew, saveFilesName, mode ) );
// std::cout << " Fin Evaluation" << std::endl;

		__fmdp->nextAction();
	    }

	    // *****************************************************************************************
	    // Next to evaluate main value function, we take maximise over all action value, ...
// std::cout << " ------------- Maximisation " << std::endl;

	    delete Vnew;
	    Vnew = NULL;
// 	    Idx nbNodeT1;
// 	    Idx nbNodeT2;
	    Idx nbRetroVarDirect;
	    Idx tailleEspaceRetrogradeDirect;
	    Idx nbRetroVarIndirect;
	    Idx tailleEspaceRetrogradeIndirect;

// 	    Idx nbFinalNodeDirect;
// 	    double iterationTimeDirect;
// 	    Idx nbFinalNodeIndirect;
// 	    double iterationTimeIndirect;

// 	    double iterationTime;
// 	    double totalTime = 0;

// 	    std::stringstream traceFileName;
// 	    traceFileName << "./Trace/Maximisation/" << saveFilesName << "." << mode << ".log";
// 	    __traceAlgoSaveFile.open(  traceFileName.str().c_str(), ios::out | ios::app ); 
// 	    if(!__traceAlgoSaveFile){
// 		cerr << "Erreur à l'ouverture !" << endl;
// 		return;
// 	    }

	    for( SetIterator< MultiDimDecisionDiagramBase< T_DATA >* > VActionsIter = VactionCollector.begin(); VActionsIter != VactionCollector.end(); ++VActionsIter ){

// 		if( Vnew != NULL )
// 		    nbNodeT1 = Vnew->nodesMap().size();
// 		else
// 		    nbNodeT1= 0;
// 		    nbNodeT2 = (*VActionsIter)->nodesMap().size();

		std::pair<Idx, Idx> resEvalRetro = __evalNbRetrograde( Vnew, *VActionsIter );
		nbRetroVarDirect = resEvalRetro.first;
		tailleEspaceRetrogradeDirect = resEvalRetro.second;

		resEvalRetro = __evalNbRetrograde( *VActionsIter, Vnew );
		nbRetroVarIndirect = resEvalRetro.first;
		tailleEspaceRetrogradeIndirect = resEvalRetro.second;

		bool sensDirect = true;
		switch( mode ){
		    case 2 : sensDirect = false; break;
		    case 3 : if( nbRetroVarDirect > nbRetroVarIndirect )
				      sensDirect = false;
				  break;
		    case 4 : if( tailleEspaceRetrogradeDirect > tailleEspaceRetrogradeIndirect )
				      sensDirect = false;
				  break;
		    default : break;
		}

		Vtemp = Vnew;
// 		Timer time;

		if( sensDirect ){
// 		    time.reset();
// 		    MultiDimDecisionDiagramBase< T_DATA >* Vind = maximize2MultiDimDecisionDiagrams( *VActionsIter, Vnew );
// 		    nbFinalNodeIndirect = Vind->nodesMap().size();
// 		    iterationTimeIndirect = time.step();
// 		    delete Vind;

// 		    time.reset();
		    Vnew = maximize2MultiDimDecisionDiagrams( Vnew, *VActionsIter );
// 		    nbFinalNodeDirect = Vnew->nodesMap().size();
// 		    iterationTimeDirect = time.step();
// 		    iterationTime = iterationTimeDirect;
		} else {
// 		    time.reset();
// 		    MultiDimDecisionDiagramBase< T_DATA >* Vind = maximize2MultiDimDecisionDiagrams( Vnew, *VActionsIter );
// 		    nbFinalNodeDirect = Vind->nodesMap().size();
// 		    iterationTimeDirect = time.step();
// 		    delete Vind;

// 		    time.reset();
		    Vnew = maximize2MultiDimDecisionDiagrams( *VActionsIter, Vnew );
// 		    nbFinalNodeIndirect = Vnew->nodesMap().size();
// 		    iterationTimeIndirect = time.step();
// 		    iterationTime = iterationTimeIndirect;
		}

// 		totalTime += time.step();
		delete Vtemp;
		delete *VActionsIter;

// 		__traceAlgoSaveFile << nbNodeT1 << "\t" << nbNodeT2 << "\t" << nbRetroVarDirect << "\t" << tailleEspaceRetrogradeDirect << "\t" << nbFinalNodeDirect << "\t" << 
// 			    iterationTimeDirect << "\t" << nbRetroVarIndirect << "\t" << tailleEspaceRetrogradeIndirect << "\t" << nbFinalNodeIndirect << "\t" << iterationTimeIndirect  
// 				      << "\t" << sensDirect << "\t" << iterationTime << "\t" << totalTime << std::endl;
	    }

// 	    __traceAlgoSaveFile.close();
// std::cout << " Fin Maximisation " << std::endl;

	    // *******************************************************************************************
	    // Next, we eval the new function value
	    Vtemp = Vnew;
// std::cout << " ---------- Ajout Récompense " << std::endl;
	    Vnew = __addRewardAlgoEvaluation( Vtemp, saveFilesName, mode );
// std::cout << " Fin Ajout Récompense " << std::endl;
	    delete Vtemp;


	    // *****************************************************************************************
	    // Then we compare new value function and the old one

	    MultiDimDecisionDiagramBase< T_DATA >* deltaV = subtract2MultiDimDecisionDiagrams( Vnew, Vold );
	    gap = 0;
	    for( BijectionIterator< NodeId, T_DATA > valIter = deltaV->valuesMap().begin(); valIter != deltaV->valuesMap().end(); ++valIter )
		if( gap < fabs( valIter.second() ) )
		    gap = fabs( valIter.second() );
	    delete deltaV;

// 	    std::cout << "*************************************************" << std::endl << " Essai : " << saveFilesName << std::endl << " ------------------- Fin itération n° " << nbIte << std::endl << " Gap : " << gap << std::endl;
	    // ****************************************************************************************
	    // And eventually we update pointers for next loop
	    delete Vold;
	    Vold = Vnew;
	}

// 	delete Vold;
	return Vold;
    
    }



    // ===========================================================================
    // Evals the value function for current fmdp action
    // ===========================================================================
    template<typename T_DATA>
    MultiDimDecisionDiagramBase<T_DATA>*
    SPUDDPlanning<T_DATA>::__evalActionValueAlgoEvaluation( const MultiDimDecisionDiagramBase< T_DATA >* Vold, const std::string saveFilesName, Idx mode ){
      
	__fmdp->resetVariablesIterator();

	MultiDimDecisionDiagramBase< T_DATA >* Vaction = reinterpret_cast<MultiDimDecisionDiagramBase<T_DATA>*>( Vold->newFactory() );
	Vaction->copy( *Vold );
	MultiDimDecisionDiagramBase< T_DATA >* Vtemp = NULL;

// 	Idx nbNodeT1;
// 	Idx nbNodeT2;
	Idx nbRetroVarDirect;
	Idx tailleEspaceRetrogradeDirect;
	Idx nbRetroVarIndirect;
	Idx tailleEspaceRetrogradeIndirect;

// 	Idx nbFinalNodeDirect;
// 	double iterationTimeDirect;
// 	Idx nbFinalNodeIndirect;
// 	double iterationTimeIndirect;

// 	double iterationTime;
// 	double totalTime = 0;

// 	std::string actionName = __fmdp->actionName( __fmdp->actionIterId() );
// 	std::stringstream traceFileName;
// 	traceFileName << "./Trace/" << actionName << "/" << saveFilesName << "." << mode << ".log";
// 	__traceAlgoSaveFile.open(  traceFileName.str().c_str(), ios::out | ios::app ); 
// 	if(!__traceAlgoSaveFile){
// 	    cerr << "Erreur à l'ouverture ! Fichier en cause : " << traceFileName.str() << endl;
// 	    return NULL;
// 	}

	// *****************************************************************************************
	// To evaluate action value function, we multiply old main value function by transition table
	// of each variable
	while( __fmdp->hasVariable() ){

	    // =======================================================================================
	    // Multiplication of Vaction by current variable's CPT
	    const MultiDimDecisionDiagramBase<T_DATA>* cpt = reinterpret_cast<const MultiDimDecisionDiagramBase<T_DATA>*>( __fmdp->transition() );

// 	    nbNodeT1 =  Vaction->nodesMap().size();
// 	    nbNodeT2 = cpt->nodesMap().size();

	    std::pair<Idx, Idx> resEvalRetro = __evalNbRetrograde( Vaction, cpt );
	    nbRetroVarDirect = resEvalRetro.first;
	    tailleEspaceRetrogradeDirect = resEvalRetro.second;
	    
	    resEvalRetro = __evalNbRetrograde( cpt, Vaction );
	    nbRetroVarIndirect = resEvalRetro.first;
	    tailleEspaceRetrogradeIndirect = resEvalRetro.second;
	    
	    bool sensDirect = true;
	    switch( mode ){
		case 2 : sensDirect = false; break;
		case 3 : 
		    if( nbRetroVarDirect > nbRetroVarIndirect )
		      sensDirect = false;
		    break;
		case 4 : 
		    if( tailleEspaceRetrogradeDirect > tailleEspaceRetrogradeIndirect )
		      sensDirect = false;
		    break;
		default : break;
	    }

// 	    Timer time;
	    Vtemp = Vaction;

	    if( sensDirect ){
// 		time.reset();
// 		MultiDimDecisionDiagramBase< T_DATA >* Vind = multiply2MultiDimDecisionDiagrams( cpt, Vaction );
// 		nbFinalNodeIndirect = Vind->nodesMap().size();
// 		iterationTimeIndirect = time.step();
// 		delete Vind;

// 		time.reset();
		Vaction = multiply2MultiDimDecisionDiagrams( Vaction, cpt );
// 		nbFinalNodeDirect = Vaction->nodesMap().size();
// 		iterationTimeDirect = time.step();
// 		iterationTime = iterationTimeDirect;
	    } else {
// 		time.reset();
// 		MultiDimDecisionDiagramBase< T_DATA >* Vind = multiply2MultiDimDecisionDiagrams( Vaction, cpt );
// 		nbFinalNodeDirect = Vind->nodesMap().size();
// 		iterationTimeDirect = time.step();
// 		delete Vind;

// 		time.reset();
		Vaction = multiply2MultiDimDecisionDiagrams( cpt, Vaction );
// 		nbFinalNodeIndirect = Vaction->nodesMap().size();
// 		iterationTimeIndirect = time.step();
// 		iterationTime = iterationTimeIndirect;
	    }

	    delete Vtemp;

	    Vtemp = Vaction;
	    Set< const DiscreteVariable* > varSet;
	    varSet << __fmdp->variable();
	    Vaction = projectSumMultiDimDecisionDiagram( Vaction, varSet );
	    delete Vtemp;
// 	    totalTime += time.step();

// 	   __traceAlgoSaveFile << nbNodeT1 << "\t" << nbNodeT2 << "\t" << nbRetroVarDirect << "\t" << tailleEspaceRetrogradeDirect << "\t" << nbFinalNodeDirect << "\t" << 
// 			iterationTimeDirect << "\t" << nbRetroVarIndirect << "\t" << tailleEspaceRetrogradeIndirect << "\t" << nbFinalNodeIndirect << "\t" << iterationTimeIndirect  
// 					<< "\t" << sensDirect << "\t" << iterationTime << "\t" << totalTime << std::endl;
	    
	    __fmdp->nextVariable();

	}

// 	__traceAlgoSaveFile.close();

// std::cout << "DONE!" << std::endl;

	return Vaction;

    }



    // ===========================================================================
    // Updates the value function by multiplying by discount and adding reward
    // ===========================================================================
    template<typename T_DATA>
    MultiDimDecisionDiagramBase<T_DATA>*
    SPUDDPlanning<T_DATA>::__addRewardAlgoEvaluation( const MultiDimDecisionDiagramBase< T_DATA >* Vold, const std::string saveFilesName, Idx mode ){

	// *****************************************************************************************
	// ... we multiply the result by the discount factor, ...
	MultiDimDecisionDiagramBase< T_DATA >* Vnew = reinterpret_cast<MultiDimDecisionDiagramBase<T_DATA>*>( Vold->newFactory() );
	Vnew->multiplyByScalar( Vold, __fmdp->discount() );

	// *****************************************************************************************
	// ... and finally add reward
	MultiDimDecisionDiagramBase< T_DATA >* Vtemp = Vnew;

// 	Idx nbNodeT1;
// 	Idx nbNodeT2;
	Idx nbRetroVarDirect;
	Idx tailleEspaceRetrogradeDirect;
	Idx nbRetroVarIndirect;
	Idx tailleEspaceRetrogradeIndirect;

// 	Idx nbFinalNodeDirect;
// 	double iterationTimeDirect;
// 	Idx nbFinalNodeIndirect;
// 	double iterationTimeIndirect;

// 	double totalTime = 0;

// 	std::stringstream traceFileName;
// 	traceFileName << "./Trace/AddReward/" << saveFilesName << "." << mode << ".log";
// 	__traceAlgoSaveFile.open(  traceFileName.str().c_str(), ios::out | ios::app ); 
// 	if(!__traceAlgoSaveFile){
// 	    cerr << "Erreur à l'ouverture !" << endl;
// 	    return NULL;
// 	}
	
	const MultiDimDecisionDiagramBase<T_DATA>* reward = reinterpret_cast<const MultiDimDecisionDiagramBase<T_DATA>*>( __fmdp->reward() );
	
// 	if( Vnew != NULL )
// 	    nbNodeT1 = Vnew->nodesMap().size();
// 	else
// 	    nbNodeT1= 0;
// 	nbNodeT2 = reward->nodesMap().size();

	std::pair<Idx, Idx> resEvalRetro = __evalNbRetrograde( Vnew, reward );
	nbRetroVarDirect = resEvalRetro.first;
	tailleEspaceRetrogradeDirect = resEvalRetro.second;
	
	resEvalRetro = __evalNbRetrograde( reward, Vnew );
	nbRetroVarIndirect = resEvalRetro.first;
	tailleEspaceRetrogradeIndirect = resEvalRetro.second;
	
	bool sensDirect = true;
	switch( mode ){
	    case 2 : sensDirect = false; break;
	    case 3 : 
		if( nbRetroVarDirect > nbRetroVarIndirect )
		  sensDirect = false;
		break;
	    case 4 : 
		if( tailleEspaceRetrogradeDirect > tailleEspaceRetrogradeIndirect )
		  sensDirect = false;
		break;
	    default : break;
	}
	
	
	Timer time;

	if( sensDirect ){
// 	    time.reset();
// 	    MultiDimDecisionDiagramBase< T_DATA >* Vind =add2MultiDimDecisionDiagrams( reward, Vnew );
// 	    nbFinalNodeIndirect = Vind->nodesMap().size();
// 	    iterationTimeIndirect = time.step();
// 	    delete Vind;

// 	    time.reset();
	    Vnew = add2MultiDimDecisionDiagrams( Vnew, reward );
// 	    nbFinalNodeDirect = Vnew->nodesMap().size();
// 	    iterationTimeDirect = time.step();
	} else {
// 	    time.reset();
// 	    MultiDimDecisionDiagramBase< T_DATA >* Vind = add2MultiDimDecisionDiagrams( Vnew, reward );
// 	    nbFinalNodeDirect = Vind->nodesMap().size();
// 	    iterationTimeDirect = time.step();
// 	    delete Vind;

// 	    time.reset();
	    Vnew = add2MultiDimDecisionDiagrams( reward, Vnew );
// 	    nbFinalNodeIndirect = Vnew->nodesMap().size();
// 	    iterationTimeIndirect = time.step();
	}

// 	totalTime += time.step();

// 	__traceAlgoSaveFile << nbNodeT1 << "\t" << nbNodeT2 << "\t" << nbRetroVarDirect << "\t" << tailleEspaceRetrogradeDirect << "\t" << nbFinalNodeDirect << "\t" << 
// 		    iterationTimeDirect << "\t" << nbRetroVarIndirect << "\t" << tailleEspaceRetrogradeIndirect << "\t" << nbFinalNodeIndirect << "\t" << iterationTimeIndirect  << "\t" << totalTime << std::endl;

// 	__traceAlgoSaveFile.close();
	delete Vtemp;

	return Vnew;

    }



    // ===========================================================================
    // Evals how many retrograde variable there will be if we do the operation in that order
    // ===========================================================================
    template<typename T_DATA>
    std::pair<Idx,Idx>
    SPUDDPlanning<T_DATA>::__evalNbRetrograde( const MultiDimDecisionDiagramBase<T_DATA>* t1, const MultiDimDecisionDiagramBase<T_DATA>* t2 ){
      

	std::pair<Idx,Idx> res;
      
	if( t1 == NULL || t2 == NULL ){
	      res.first = 0;
	      res.second = 0;
	      return res;
	}

	// *****************************************************************************
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

	// ******************************************************************************
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

	Idx sizeRetro = 0;
	if( watchedRetroVar.size() != 0 ){
	    sizeRetro = 1;
	    for( SetIterator< const DiscreteVariable* > siter = watchedRetroVar.begin(); siter != watchedRetroVar.end(); ++siter )
		sizeRetro *= (*siter)->domainSize();
	}
	res.first = watchedRetroVar.size();
	res.second = sizeRetro;
	
	return res;

    }

}
