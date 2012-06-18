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
// ============================================================================
#include <math.h>
// ============================================================================
#include <agrum/multidim/potential.h>
#include <agrum/multidim/instantiation.h>
#include <agrum/multidim/multiDimDecisionDiagram.h>
// ============================================================================
#include <agrum/multidim/patterns/DDUtility/o4DDMiscellaneous.h>
// ============================================================================
#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
#include <agrum/FMDP/planning/SPUDDPlanning.h>
// ============================================================================
#include <fstream>
#include <agrum/core/timer.h>

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
SPUDDPlanning<T_DATA>::SPUDDPlanning ( FactoredMarkovDecisionProcess<T_DATA>* fmdp, T_DATA epsilon ) {

    GUM_CONSTRUCTOR( SPUDDPlanning );
    __epsilon = epsilon;
    __fmdp = fmdp;

}

// ===========================================================================
// Default destructor
// ===========================================================================
template<typename T_DATA> INLINE
SPUDDPlanning<T_DATA>::~SPUDDPlanning() {

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
MultiDimDecisionDiagramBase< T_DATA >* 
SPUDDPlanning<T_DATA>::makePlanning (  ) {

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
    while ( gap >  threshold ){

        ++nbIte;

        // *****************************************************************************************
        // Loop reset
        __fmdp->resetActionsIterator();
//         Set< MultiDimDecisionDiagramBase< T_DATA >* > VactionCollector;
	HashTable< Idx, MultiDimDecisionDiagramBase< T_DATA >* > VactionCollector;

        Vnew = reinterpret_cast<MultiDimDecisionDiagramBase<T_DATA>*>( Vold->newFactory() );
        Vnew->copyAndReassign( Vold, __fmdp->main2prime() );

#ifdef O4DDWITHORDER
	Sequence< const DiscreteVariable* > elVarSeq;
#else
	Sequence< const DiscreteVariable* > elVarSeq = Vnew->variablesSequence();
#endif
	__fmdp->resetVariablesIterator();
	while( __fmdp->hasVariable() ){
	  const DiscreteVariable* var = __fmdp->variable();
	  if( ! elVarSeq.exists( var ) ){
	    elVarSeq << var;
	  }
	  __fmdp->nextVariable();
	}

        // *****************************************************************************************
        // For each action
        while ( __fmdp->hasAction() ) {

            VactionCollector.insert( __fmdp->actionIterId(), __evalActionValue( Vnew, elVarSeq ) );

            __fmdp->nextAction();
        }

        // *****************************************************************************************
        // Next to evaluate main value function, we take maximise over all action value, ...
        delete Vnew;
        Vnew = NULL;

//         for ( SetIterator< MultiDimDecisionDiagramBase< T_DATA >* > VActionsIter = VactionCollector.begin(); VActionsIter != VactionCollector.end(); ++VActionsIter ) {
	for( Idx acta = 1; acta < VactionCollector.size() + 1; acta++ ){
            Vtemp = Vnew;
//             Vnew = maximize2MultiDimDecisionDiagrams( Vnew, *VActionsIter );
            Vnew = maximize2MultiDimDecisionDiagrams( Vnew, VactionCollector[ acta ] );

            delete Vtemp;
            delete VactionCollector[ acta ];
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
        for ( BijectionIterator< NodeId, T_DATA > valIter = deltaV->valuesMap().begin(); valIter != deltaV->valuesMap().end(); ++valIter )
            if ( gap < fabs( valIter.second() ) )
                gap = fabs( valIter.second() );
        delete deltaV;

//         std::cout << " ------------------- Fin itération n° " << nbIte << std::endl << " Gap : " << gap << " - ValueMapSize : " << Vnew->valuesMap().size() <<  std::endl;
        // *****************************************************************************************
        // And eventually we update pointers for next loop
        delete Vold;
        Vold = Vnew;

    }
    
    __evalPolicy( Vold );
    return Vold;
    
//     delete Vold;
}



// ===========================================================================
// Evals the policy corresponding to the given value function
// ===========================================================================
template<typename T_DATA>
void
SPUDDPlanning<T_DATA>::__evalPolicy( const MultiDimDecisionDiagramBase< T_DATA >* V ) {

    // *****************************************************************************************
    // We have to do one last step to get best policy
    // *****************************************************************************************
    MultiDimDecisionDiagramBase< std::pair< double, long > >* Vamnew = __argMaxValueFunction( V );
    
    MultiDimDecisionDiagramFactory< Idx >* factory = new MultiDimDecisionDiagramFactory<Idx>();
    factory->setVariablesSequence( Vamnew->variablesSequence() );
    HashTable< NodeId, NodeId > explorationTable;
    __makeOptimalPolicyDecisionDiagram( Vamnew, Vamnew->root(), factory, explorationTable );
    delete Vamnew;

    MultiDimDecisionDiagramBase< Idx >* optimalPolicy = factory->getMultiDimDecisionDiagram(false, 0, true);

    __displayOptimalPolicy( optimalPolicy );

    delete optimalPolicy;
    delete factory;
    
}



// ===========================================================================
// Evals the value function for current fmdp action
// ===========================================================================
template<typename T_DATA>
MultiDimDecisionDiagramBase<T_DATA>*
SPUDDPlanning<T_DATA>::__evalActionValue( const MultiDimDecisionDiagramBase< T_DATA >* Vold, Sequence< const DiscreteVariable*>& elVarSeq ) {

    // *****************************************************************************************
    // Initialisation
//     __fmdp->resetVariablesIterator();

    MultiDimDecisionDiagramBase< T_DATA >* Vaction = reinterpret_cast<MultiDimDecisionDiagramBase<T_DATA>*>( Vold->newFactory() );
    Vaction->copy( *Vold );
    MultiDimDecisionDiagramBase< T_DATA >* Vtemp = NULL;
    
    // *****************************************************************************************
    // To evaluate action value function, we multiply old main value function by transition table
    // of each variable
//     while ( __fmdp->hasVariable() ) {
   for( SequenceIterator<const DiscreteVariable*> varIter = elVarSeq.rbegin(); varIter != elVarSeq.rend(); --varIter ){
        // ***************************************************************************************
        // Multiplication of Vaction by current variable's CPT
        Vtemp = Vaction;
//         Vaction = multiply2MultiDimDecisionDiagrams(  Vaction, reinterpret_cast<const MultiDimDecisionDiagramBase<T_DATA>*>( __fmdp->transition() ) );
        Vaction = multiply2MultiDimDecisionDiagrams(  Vaction, reinterpret_cast<const MultiDimDecisionDiagramBase<T_DATA>*>( __fmdp->transition( __fmdp->main2prime().first( *varIter ) ) ) );
        delete Vtemp;

        // ***************************************************************************************
        // Projection of Vaction on current var by summing on each of its modalities
        Vtemp = Vaction;
        Set< const DiscreteVariable* > varSet;
//         varSet << __fmdp->variable();
        varSet << *varIter;
        Vaction = projectSumMultiDimDecisionDiagram( Vaction, varSet );
        delete Vtemp;

//         __fmdp->nextVariable();
    }

    return Vaction;

}



// ===========================================================================
// Updates the value function by multiplying by discount and adding reward
// ===========================================================================
template<typename T_DATA>
MultiDimDecisionDiagramBase<T_DATA>*
SPUDDPlanning<T_DATA>::__addReward( const MultiDimDecisionDiagramBase< T_DATA >* Vold ) {

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
SPUDDPlanning<T_DATA>::makePlanningAlgoEvaluation ( const std::string saveFilesName, Idx mode ) {

    // *****************************************************************************************
    // Initialisation
    // *****************************************************************************************

    // *****************************************************************************************
    // Threshold stopping criterion evaluation
    T_DATA threshold =  0.1;

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
    while ( gap >  threshold ) {

        ++nbIte;

        // *****************************************************************************************
        // Loop reset
        __fmdp->resetActionsIterator();
        Set< MultiDimDecisionDiagramBase< T_DATA >* > VactionCollector;

        Vnew = reinterpret_cast<MultiDimDecisionDiagramBase<T_DATA>*>( Vold->newFactory() );
        Vnew->copyAndReassign( Vold, __fmdp->main2prime() );

        // *****************************************************************************************
        // For each action
        while ( __fmdp->hasAction() ) {

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
        Idx nbNodeT1;
        Idx nbNodeT2;
        Idx nbRetroVarDirect;
        Idx tailleEspaceRetrogradeDirect;
        Idx nbRetroVarIndirect;
        Idx tailleEspaceRetrogradeIndirect;

        Idx nbFinalNodeDirect;
        double iterationTimeDirect;
        Idx nbFinalNodeIndirect;
        double iterationTimeIndirect;

        double iterationTime;
        double totalTime = 0;

        std::stringstream traceFileName;
        traceFileName << "./Trace/Maximisation/" << saveFilesName << "." << mode << ".log";
        __traceAlgoSaveFile.open(  traceFileName.str().c_str(), std::ios::out | std::ios::app );
        if (!__traceAlgoSaveFile) {
            std::cerr << "Erreur à l'ouverture !" << std::endl;
            return;
        }

        for ( SetIterator< MultiDimDecisionDiagramBase< T_DATA >* > VActionsIter = VactionCollector.begin(); VActionsIter != VactionCollector.end(); ++VActionsIter ) {

            if ( Vnew != NULL )
                nbNodeT1 = Vnew->nodesMap().size();
            else
                nbNodeT1= 0;
            nbNodeT2 = (*VActionsIter)->nodesMap().size();

            std::pair<Idx, Idx> resEvalRetro = __evalNbRetrograde( Vnew, *VActionsIter );
            nbRetroVarDirect = resEvalRetro.first;
            tailleEspaceRetrogradeDirect = resEvalRetro.second;

            resEvalRetro = __evalNbRetrograde( *VActionsIter, Vnew );
            nbRetroVarIndirect = resEvalRetro.first;
            tailleEspaceRetrogradeIndirect = resEvalRetro.second;

            bool sensDirect = true;
            switch ( mode ) {
            case 2 :
                sensDirect = false;
                break;
            case 3 :
                if ( nbRetroVarDirect > nbRetroVarIndirect )
                    sensDirect = false;
                break;
            case 4 :
                if ( tailleEspaceRetrogradeDirect > tailleEspaceRetrogradeIndirect )
                    sensDirect = false;
                break;
            default :
                break;
            }

            Vtemp = Vnew;
            Timer time;

            if ( sensDirect ) {
                time.reset();
                MultiDimDecisionDiagramBase< T_DATA >* Vind = maximize2MultiDimDecisionDiagrams( *VActionsIter, Vnew );
                nbFinalNodeIndirect = Vind->nodesMap().size();
                iterationTimeIndirect = time.step();
                delete Vind;

                time.reset();
                Vnew = maximize2MultiDimDecisionDiagrams( Vnew, *VActionsIter );
                nbFinalNodeDirect = Vnew->nodesMap().size();
                iterationTimeDirect = time.step();
                iterationTime = iterationTimeDirect;
            } else {
                time.reset();
                MultiDimDecisionDiagramBase< T_DATA >* Vind = maximize2MultiDimDecisionDiagrams( Vnew, *VActionsIter );
                nbFinalNodeDirect = Vind->nodesMap().size();
                iterationTimeDirect = time.step();
                delete Vind;

                time.reset();
                Vnew = maximize2MultiDimDecisionDiagrams( *VActionsIter, Vnew );
                nbFinalNodeIndirect = Vnew->nodesMap().size();
                iterationTimeIndirect = time.step();
                iterationTime = iterationTimeIndirect;
            }

            totalTime += time.step();
            delete Vtemp;
            delete *VActionsIter;

            __traceAlgoSaveFile << nbNodeT1 << "\t" << nbNodeT2 << "\t" << nbRetroVarDirect << "\t" << tailleEspaceRetrogradeDirect << "\t" << nbFinalNodeDirect << "\t" <<
            iterationTimeDirect << "\t" << nbRetroVarIndirect << "\t" << tailleEspaceRetrogradeIndirect << "\t" << nbFinalNodeIndirect << "\t" << iterationTimeIndirect
            << "\t" << sensDirect << "\t" << iterationTime << "\t" << totalTime << std::endl;
        }

        __traceAlgoSaveFile.close();
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
        for ( BijectionIterator< NodeId, T_DATA > valIter = deltaV->valuesMap().begin(); valIter != deltaV->valuesMap().end(); ++valIter )
            if ( gap < fabs( valIter.second() ) )
                gap = fabs( valIter.second() );
        delete deltaV;

        // std::cout << "*************************************************" << std::endl << " Essai : " << saveFilesName << std::endl << " ------------------- Fin itération n° " << nbIte << std::endl << " Gap : " << gap << std::endl;
        // ****************************************************************************************
        // And eventually we update pointers for next loop
        delete Vold;
        Vold = Vnew;
    }

    delete Vold;
    return Vold;

}



// ===========================================================================
// Evals the value function for current fmdp action
// ===========================================================================
template<typename T_DATA>
MultiDimDecisionDiagramBase<T_DATA>*
SPUDDPlanning<T_DATA>::__evalActionValueAlgoEvaluation( const MultiDimDecisionDiagramBase< T_DATA >* Vold, const std::string saveFilesName, Idx mode ) {

    __fmdp->resetVariablesIterator();

    MultiDimDecisionDiagramBase< T_DATA >* Vaction = reinterpret_cast<MultiDimDecisionDiagramBase<T_DATA>*>( Vold->newFactory() );
    Vaction->copy( *Vold );
    MultiDimDecisionDiagramBase< T_DATA >* Vtemp = NULL;

    Idx nbNodeT1;
    Idx nbNodeT2;
    Idx nbRetroVarDirect;
    Idx tailleEspaceRetrogradeDirect;
    Idx nbRetroVarIndirect;
    Idx tailleEspaceRetrogradeIndirect;

    Idx nbFinalNodeDirect;
    double iterationTimeDirect;
    Idx nbFinalNodeIndirect;
    double iterationTimeIndirect;

    double iterationTime;
    double totalTime = 0;

    std::string actionName = __fmdp->actionName( __fmdp->actionIterId() );
    std::stringstream traceFileName;
    traceFileName << "./Trace/" << actionName << "/" << saveFilesName << "." << mode << ".log";
    __traceAlgoSaveFile.open(  traceFileName.str().c_str(), std::ios::out | std::ios::app );
    if (!__traceAlgoSaveFile) {
        std::cerr << "Erreur à l'ouverture ! Fichier en cause : " << traceFileName.str() << std::endl;
        return NULL;
    }

    // *****************************************************************************************
    // To evaluate action value function, we multiply old main value function by transition table
    // of each variable
    while ( __fmdp->hasVariable() ) {

        // =======================================================================================
        // Multiplication of Vaction by current variable's CPT
        const MultiDimDecisionDiagramBase<T_DATA>* cpt = reinterpret_cast<const MultiDimDecisionDiagramBase<T_DATA>*>( __fmdp->transition() );

        nbNodeT1 =  Vaction->nodesMap().size();
        nbNodeT2 = cpt->nodesMap().size();

        std::pair<Idx, Idx> resEvalRetro = __evalNbRetrograde( Vaction, cpt );
        nbRetroVarDirect = resEvalRetro.first;
        tailleEspaceRetrogradeDirect = resEvalRetro.second;

        resEvalRetro = __evalNbRetrograde( cpt, Vaction );
        nbRetroVarIndirect = resEvalRetro.first;
        tailleEspaceRetrogradeIndirect = resEvalRetro.second;

        bool sensDirect = true;
        switch ( mode ) {
        case 2 :
            sensDirect = false;
            break;
        case 3 :
            if ( nbRetroVarDirect > nbRetroVarIndirect )
                sensDirect = false;
            break;
        case 4 :
            if ( tailleEspaceRetrogradeDirect > tailleEspaceRetrogradeIndirect )
                sensDirect = false;
            break;
        default :
            break;
        }

        Timer time;
        Vtemp = Vaction;

        if ( sensDirect ) {
            time.reset();
            MultiDimDecisionDiagramBase< T_DATA >* Vind = multiply2MultiDimDecisionDiagrams( cpt, Vaction );
            nbFinalNodeIndirect = Vind->nodesMap().size();
            iterationTimeIndirect = time.step();
            delete Vind;

            time.reset();
            Vaction = multiply2MultiDimDecisionDiagrams( Vaction, cpt );
            nbFinalNodeDirect = Vaction->nodesMap().size();
            iterationTimeDirect = time.step();
            iterationTime = iterationTimeDirect;
        } else {
            time.reset();
            MultiDimDecisionDiagramBase< T_DATA >* Vind = multiply2MultiDimDecisionDiagrams( Vaction, cpt );
            nbFinalNodeDirect = Vind->nodesMap().size();
            iterationTimeDirect = time.step();
            delete Vind;

            time.reset();
            Vaction = multiply2MultiDimDecisionDiagrams( cpt, Vaction );
            nbFinalNodeIndirect = Vaction->nodesMap().size();
            iterationTimeIndirect = time.step();
            iterationTime = iterationTimeIndirect;
        }

        delete Vtemp;

        Vtemp = Vaction;
        Set< const DiscreteVariable* > varSet;
        varSet << __fmdp->variable();
        Vaction = projectSumMultiDimDecisionDiagram( Vaction, varSet );
        delete Vtemp;
        totalTime += time.step();

        __traceAlgoSaveFile << nbNodeT1 << "\t" << nbNodeT2 << "\t" << nbRetroVarDirect << "\t" << tailleEspaceRetrogradeDirect << "\t" << nbFinalNodeDirect << "\t" <<
        iterationTimeDirect << "\t" << nbRetroVarIndirect << "\t" << tailleEspaceRetrogradeIndirect << "\t" << nbFinalNodeIndirect << "\t" << iterationTimeIndirect
        << "\t" << sensDirect << "\t" << iterationTime << "\t" << totalTime << std::endl;

        __fmdp->nextVariable();

    }

    __traceAlgoSaveFile.close();

// std::cout << "DONE!" << std::endl;

    return Vaction;

}



// ===========================================================================
// Updates the value function by multiplying by discount and adding reward
// ===========================================================================
template<typename T_DATA>
MultiDimDecisionDiagramBase<T_DATA>*
SPUDDPlanning<T_DATA>::__addRewardAlgoEvaluation( const MultiDimDecisionDiagramBase< T_DATA >* Vold, const std::string saveFilesName, Idx mode ) {

    // *****************************************************************************************
    // ... we multiply the result by the discount factor, ...
    MultiDimDecisionDiagramBase< T_DATA >* Vnew = reinterpret_cast<MultiDimDecisionDiagramBase<T_DATA>*>( Vold->newFactory() );
    Vnew->multiplyByScalar( Vold, __fmdp->discount() );

    // *****************************************************************************************
    // ... and finally add reward
    MultiDimDecisionDiagramBase< T_DATA >* Vtemp = Vnew;

    Idx nbNodeT1;
    Idx nbNodeT2;
    Idx nbRetroVarDirect;
    Idx tailleEspaceRetrogradeDirect;
    Idx nbRetroVarIndirect;
    Idx tailleEspaceRetrogradeIndirect;

    Idx nbFinalNodeDirect;
    double iterationTimeDirect;
    Idx nbFinalNodeIndirect;
    double iterationTimeIndirect;

    double totalTime = 0;

    std::stringstream traceFileName;
    traceFileName << "./Trace/AddReward/" << saveFilesName << "." << mode << ".log";
    __traceAlgoSaveFile.open(  traceFileName.str().c_str(), std::ios::out | std::ios::app );
    if (!__traceAlgoSaveFile) {
        std::cerr << "Erreur à l'ouverture !" << std::endl;
        return NULL;
    }

    const MultiDimDecisionDiagramBase<T_DATA>* reward = reinterpret_cast<const MultiDimDecisionDiagramBase<T_DATA>*>( __fmdp->reward() );

    if ( Vnew != NULL )
        nbNodeT1 = Vnew->nodesMap().size();
    else
        nbNodeT1= 0;
    nbNodeT2 = reward->nodesMap().size();

    std::pair<Idx, Idx> resEvalRetro = __evalNbRetrograde( Vnew, reward );
    nbRetroVarDirect = resEvalRetro.first;
    tailleEspaceRetrogradeDirect = resEvalRetro.second;

    resEvalRetro = __evalNbRetrograde( reward, Vnew );
    nbRetroVarIndirect = resEvalRetro.first;
    tailleEspaceRetrogradeIndirect = resEvalRetro.second;

    bool sensDirect = true;
    switch ( mode ) {
    case 2 :
        sensDirect = false;
        break;
    case 3 :
        if ( nbRetroVarDirect > nbRetroVarIndirect )
            sensDirect = false;
        break;
    case 4 :
        if ( tailleEspaceRetrogradeDirect > tailleEspaceRetrogradeIndirect )
            sensDirect = false;
        break;
    default :
        break;
    }


    Timer time;

    if ( sensDirect ) {
        time.reset();
        MultiDimDecisionDiagramBase< T_DATA >* Vind =add2MultiDimDecisionDiagrams( reward, Vnew );
        nbFinalNodeIndirect = Vind->nodesMap().size();
        iterationTimeIndirect = time.step();
        delete Vind;

        time.reset();
        Vnew = add2MultiDimDecisionDiagrams( Vnew, reward );
        nbFinalNodeDirect = Vnew->nodesMap().size();
        iterationTimeDirect = time.step();
    } else {
        time.reset();
        MultiDimDecisionDiagramBase< T_DATA >* Vind = add2MultiDimDecisionDiagrams( Vnew, reward );
        nbFinalNodeDirect = Vind->nodesMap().size();
        iterationTimeDirect = time.step();
        delete Vind;

        time.reset();
        Vnew = add2MultiDimDecisionDiagrams( reward, Vnew );
        nbFinalNodeIndirect = Vnew->nodesMap().size();
        iterationTimeIndirect = time.step();
    }

    totalTime += time.step();

    __traceAlgoSaveFile << nbNodeT1 << "\t" << nbNodeT2 << "\t" << nbRetroVarDirect << "\t" << tailleEspaceRetrogradeDirect << "\t" << nbFinalNodeDirect << "\t" <<
    iterationTimeDirect << "\t" << nbRetroVarIndirect << "\t" << tailleEspaceRetrogradeIndirect << "\t" << nbFinalNodeIndirect << "\t" << iterationTimeIndirect  << "\t" << totalTime << std::endl;

    __traceAlgoSaveFile.close();
    delete Vtemp;

    return Vnew;

}



// ===========================================================================
// Evals how many retrograde variable there will be if we do the operation in that order
// ===========================================================================
template<typename T_DATA>
std::pair<Idx,Idx>
SPUDDPlanning<T_DATA>::__evalNbRetrogradeEvaluation( const MultiDimDecisionDiagramBase<T_DATA>* dD1, const MultiDimDecisionDiagramBase<T_DATA>* dD2 ) {


    std::pair<Idx,Idx> res;

    if ( dD1 == NULL || dD2 == NULL ) {
        res.first = 0;
        res.second = 0;
        return res;
    }

    // *****************************************************************************
    // First we determine the new var sequence

    Sequence< const DiscreteVariable* > dD1VarSeq = dD1->variablesSequence();
    Sequence< const DiscreteVariable* > dD2VarSeq = dD2->variablesSequence();
    Sequence< const DiscreteVariable* > fusVarSeq;

    SequenceIterator< const DiscreteVariable* > iterS1 = dD1VarSeq.begin();
    SequenceIterator< const DiscreteVariable* > iterS2 = dD2VarSeq.begin();

    while ( iterS1 != dD1VarSeq.end() || iterS2 != dD2VarSeq.end() ) {
        if ( iterS1 == dD1VarSeq.end() ) {
            for ( ; iterS2 != dD2VarSeq.end(); ++iterS2 )
                if ( !fusVarSeq.exists(*iterS2) )
                    fusVarSeq.insert( *iterS2 );
        }
        else if ( iterS2 ==dD2VarSeq.end() ) {
            for ( ; iterS1 != dD1VarSeq.end(); ++iterS1 )
                if ( !fusVarSeq.exists(*iterS1) )
                    fusVarSeq.insert( *iterS1 );
        } else {
            if ( *iterS1 == *iterS2 ) {
                if ( !fusVarSeq.exists(*iterS1) )
                    fusVarSeq.insert( *iterS1 );
                ++iterS1;
                ++iterS2;
                continue;
            }
            if ( dD1VarSeq.pos(*iterS1) <= dD2VarSeq.pos(*iterS2) || dD1VarSeq.exists( *iterS2 ) ) {
                if ( !fusVarSeq.exists(*iterS1) )
                    fusVarSeq.insert( *iterS1 );
                ++iterS1;
                continue;
            } else {
                if ( !fusVarSeq.exists(*iterS2) )
                    fusVarSeq.insert( *iterS2 );
                ++iterS2;
                continue;
            }
        }
    }

    // ******************************************************************************
    // Then we search in second diagram for possible preneeded variable
    Idx sizeRetro = 1;
    Idx nbRetroVar = 0;
    for ( iterS2 = dD2VarSeq.begin(); iterS2 != dD2VarSeq.end(); ++iterS2 )
        for ( iterS1 = iterS2; iterS1 != dD2VarSeq.rend(); --iterS1 )
            if ( fusVarSeq.pos( *iterS1 ) > fusVarSeq.pos( *iterS2 ) ) {
                nbRetroVar++;
                sizeRetro *= (*iterS2)->domainSize();
            }

    res.first = nbRetroVar;
    res.second = sizeRetro;

    return res;

}




/* ************************************************************************************************** **/
/* **                                                                                                                                                 **/
/* **                                   Optimal Policy Evaluation Methods                                                         **/
/* **                                                                                                                                                 **/
/* ************************************************************************************************** **/

// ===========================================================================
// Performs one last step of the algorithm to obtain the arg max equivalent of the so far computed value function
// ===========================================================================
template<typename T_DATA>
MultiDimDecisionDiagramBase< std::pair< double, long > >*
SPUDDPlanning< T_DATA >::__argMaxValueFunction( const MultiDimDecisionDiagramBase< T_DATA >* V ){

    // *****************************************************************************************
    // Loop reset
    __fmdp->resetActionsIterator();
    Bijection< Idx, MultiDimDecisionDiagramBase< T_DATA >* > VactionCollector;
    MultiDimDecisionDiagramBase< T_DATA >* Vnew = reinterpret_cast<MultiDimDecisionDiagramBase<T_DATA>*>( V->newFactory() );
    Vnew->copyAndReassign( V, __fmdp->main2prime() );
    MultiDimDecisionDiagramBase< T_DATA >* Vtemp;

#ifdef O4DDWITHORDER
	Sequence< const DiscreteVariable* > elVarSeq;
#else
	Sequence< const DiscreteVariable* > elVarSeq = Vnew->variablesSequence();
#endif
	__fmdp->resetVariablesIterator();
	while( __fmdp->hasVariable() ){
	  const DiscreteVariable* var = __fmdp->variable();
	  if( ! elVarSeq.exists( var ) ){
	    elVarSeq << var;
	  }
	  __fmdp->nextVariable();
	}

    // *****************************************************************************************
    // For each action
    while ( __fmdp->hasAction() ) {

        MultiDimDecisionDiagramBase< T_DATA >* Vaction = __evalActionValue( Vnew, elVarSeq );

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
    MultiDimDecisionDiagramBase< std::pair< double, long > >* Vamnew = NULL;
//     for ( BijectionIterator< Idx, MultiDimDecisionDiagramBase< T_DATA >* > VActionsIter = VactionCollector.begin(); VActionsIter != VactionCollector.end(); ++VActionsIter ) {
  for( Idx acta = 1; acta < VactionCollector.size() + 1; acta++ ){
//         MultiDimDecisionDiagramBase< std::pair< double, long > >* Vamaction = __createArgMaxCopy( VActionsIter.second(), VActionsIter.first() );
        MultiDimDecisionDiagramBase< std::pair< double, long > >* Vamaction = __createArgMaxCopy( VactionCollector.second( acta ), acta );
	delete VactionCollector.second( acta );
        MultiDimDecisionDiagramBase< std::pair< double, long > >* Vamtemp = Vamnew;
        Vamnew = __argMaxOn2MultiDimDecisionDiagrams( Vamnew, Vamaction );
        delete Vamtemp;
        delete Vamaction;
    }
    return Vamnew;
    
}

// ===========================================================================
// Creates a copy of given in parameter decision diagram and replaces leaves of that diagram by a pair containing value of the leaf and
// action to which is bind this diagram (given in parameter).
// ===========================================================================
template<typename T_DATA>
MultiDimDecisionDiagramBase< std::pair< double, long > >*
SPUDDPlanning<T_DATA>::__createArgMaxCopy( const MultiDimDecisionDiagramBase<T_DATA>* Vaction, Idx actionId ) {

    MultiDimDecisionDiagramBase< std::pair< double, long > >* amcpy = new MultiDimDecisionDiagram< std::pair< double, long > >();

    amcpy->beginInstantiation();

    amcpy->setVariableSequence( Vaction->variablesSequence() );

    amcpy->setDiagramNodes( Vaction->nodesMap() );
    Bijection< NodeId, std::pair< double, long > > amvm( Vaction->valuesMap().size() );
    for ( BijectionIterator< NodeId, T_DATA > valueIter = Vaction->valuesMap().begin(); valueIter != Vaction->valuesMap().end(); ++valueIter ) {
        std::pair< double, long >  amv( (double) valueIter.second(), (long ) actionId );
        amvm.insert( valueIter.first(), amv );
    }
    
    amcpy->setValueMap ( amvm );

    amcpy->setVariableMap ( Vaction->variableMap() );
    amcpy->setVar2NodeMap ( Vaction-> var2NodeIdMap() );
    amcpy->setVarUsedModalitiesMap( Vaction->varUsedModalitiesMap() );
    amcpy->setDiagramArcs ( Vaction->arcMap(), Vaction->defaultArcMap() );

    amcpy->setRoot ( Vaction->root() );
    amcpy->endInstantiation();

    return amcpy;
}



// ===========================================================================
// Once final V is computed upon arg max on last Vactions, this function creates a diagram where all leaves tied to the same action are merged together.
// Since this function is a recursive one to ease the merge of all identic nodes to converge toward a cannonical policy, a factory and the current node are needed to build
// resulting diagram. Also we need an exploration table to avoid exploration of already visited sub-graph part.
// ===========================================================================
template<typename T_DATA>
NodeId
SPUDDPlanning<T_DATA>::__makeOptimalPolicyDecisionDiagram( const MultiDimDecisionDiagramBase< std::pair< double, long > >* V,
        const NodeId& currentNode,
        MultiDimDecisionDiagramFactoryBase<Idx>* factory,
        HashTable< NodeId, NodeId >& explorationTable ) {  

    std::vector<NodeId> sonsMap( V->unsafeNodeSons( currentNode )->size(), 0 );
    for ( std::vector<NodeId>::const_iterator sonIter = V->unsafeNodeSons( currentNode )->begin(); sonIter != V->unsafeNodeSons( currentNode )->end(); ++ sonIter ) {
        NodeId sonId = 0;
        if ( *sonIter != 0 ) {
            if ( explorationTable.exists( *sonIter ) )
                sonId = explorationTable[*sonIter];
            else {
                if ( V->isTerminalNode( *sonIter ) )
                    sonId = factory->addTerminalNode( V->unsafeNodeValue( *sonIter ).second );
                else
                    sonId = __makeOptimalPolicyDecisionDiagram( V, *sonIter, factory, explorationTable );
                explorationTable.insert( *sonIter, sonId );
            }
        }
        sonsMap[ std::distance( V->unsafeNodeSons( currentNode )->begin(), sonIter ) ] = sonId;
    }

    NodeId defaultSonId = 0;
    if ( V->unsafeHasNodeDefaultSon( currentNode ) ) {
        NodeId defaultSon = V->unsafeNodeDefaultSon( currentNode );
        if ( explorationTable.exists( defaultSon ) )
            defaultSonId = explorationTable[ defaultSon ];
        else {
            if ( V->isTerminalNode( defaultSon ) )
                defaultSonId = factory->addTerminalNode( V->unsafeNodeValue( defaultSon ).second );
            else
                defaultSonId = __makeOptimalPolicyDecisionDiagram( V, defaultSon, factory, explorationTable );
            explorationTable.insert( defaultSon, defaultSonId );
        }
    }



    if ( defaultSonId != 0 ) {
        Idx nbDefault = 0;
        for ( std::vector<NodeId >::iterator iterArcMap = sonsMap.begin(); iterArcMap != sonsMap.end(); ++iterArcMap ) {
            if ( *iterArcMap == 0 )
                ++nbDefault;
            if ( *iterArcMap == defaultSonId ) {
                ++nbDefault;
                sonsMap[ std::distance( sonsMap.begin(), iterArcMap ) ] = 0;
            }
        }
        if ( nbDefault == 1 )
            for ( std::vector<NodeId >::iterator iterArcMap = sonsMap.begin(); iterArcMap != sonsMap.end(); ++iterArcMap )
                if ( *iterArcMap == 0 ) {
                    sonsMap[ std::distance( sonsMap.begin(), iterArcMap ) ] = defaultSonId;
                    defaultSonId = 0;
                    break;
                }
    }

    return factory->unsafeAddNonTerminalNodeWithArcs( V->unsafeNodeVariable( currentNode ), sonsMap, defaultSonId );

}



// ===========================================================================
// Displays the optimal computed policy diagram
// ===========================================================================
template<typename T_DATA>
void
SPUDDPlanning<T_DATA>::__displayOptimalPolicy( MultiDimDecisionDiagramBase< Idx >* op ) {

    // *****************************************************************************************
    // And eventually we display the result
    // *****************************************************************************************
    std::stringstream output;
    std::stringstream terminalStream;
    std::stringstream nonTerminalStream;
    std::stringstream arcstream;
    std::stringstream defaultarcstream;
    output << "digraph \"Politique Optimale\" {" << std::endl;

    terminalStream << "node [shape = box];" << std::endl;
    nonTerminalStream << "node [shape = ellipse];" << std::endl;
    std::string tab = "  ";

    for ( NodeGraphPart::NodeIterator nodeIter = op->nodesMap().beginNodes(); nodeIter != op->nodesMap().endNodes(); ++nodeIter ) {
        if ( *nodeIter != 0 ) {

            if ( op->isTerminalNode( *nodeIter ) ) {

                terminalStream << tab << *nodeIter << ";" << tab << *nodeIter  << " [label=\"" << __fmdp->actionName( op->nodeValue( *nodeIter ) ) << "\"]"<< ";" << std::endl;

            }
            else
            {
                nonTerminalStream << tab << *nodeIter << ";" << tab << *nodeIter  << " [label=\"" << op->nodeVariable( *nodeIter )->name() << "\"]"<< ";" << std::endl;

                if (  op->nodeSons( *nodeIter ) != NULL ) {
                    for ( std::vector<NodeId>::const_iterator sonIter =  op->nodeSons( *nodeIter )->begin(); sonIter != op->nodeSons( *nodeIter )->end(); ++sonIter)
                        if ( *sonIter != 0 )
                            arcstream << tab <<  *nodeIter << " -> " << *sonIter << " [label=\"" << op->nodeVariable( *nodeIter )->label( std::distance( op->nodeSons( *nodeIter )->begin(), sonIter ) ) << "\",color=\"#0000ff\"]"<< ";" << std::endl;
                }

                if ( op->hasNodeDefaultSon( *nodeIter ) )
                    defaultarcstream << tab <<  *nodeIter << " -> " << op->nodeDefaultSon(*nodeIter ) << " [color=\"#ff0000\"]"<< ";" << std::endl;

            }
        }
    }

    output << terminalStream.str() << std::endl << nonTerminalStream.str() << std::endl <<  arcstream.str() << std::endl << defaultarcstream.str() << std::endl << std::endl << "}" << std::endl;

//     std::cout << std::endl << output.str();
}

}



// ===========================================================================
// Computed arg max of two vactions given in parameter
// ===========================================================================

#define GUM_MULTI_DIM_OPERATOR(x,y) x.first>y.first?x:y
#define GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION DecisionDiagramRecur4argMax

#define GUM_DECISION_DIAGRAM_OPERATOR_FUNCTION_DEFINITION
#ifdef O4DDWITHORDER
#define GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_FUNCTION DecisionDiagramGoDown4argMax
#ifdef O4DDDEBUG
#include <agrum/multidim/patterns/DDUtility/o4DDWithOrderRecursionFunctionsDebug.h>
#else
#include <agrum/multidim/patterns/DDUtility/o4DDWithOrderRecursionFunctions.h>
#endif
#undef GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_FUNCTION
#else
#define GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION DecisionDiagramGoDownOnLeader4argMax
#define GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION DecisionDiagramGoDownOnFollower4argMax
#ifdef O4DDDEBUG
#include <agrum/multidim/patterns/DDUtility/o4DDWithoutOrderRecursionFunctionsDebug.h>
#else
#include <agrum/multidim/patterns/DDUtility/o4DDWithoutOrderRecursionFunctions.h>
#endif
#undef GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION
#undef GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION
#endif
#undef GUM_DECISION_DIAGRAM_OPERATOR_FUNCTION_DEFINITION

template <typename T_DATA>
gum::MultiDimDecisionDiagramBase< std::pair< double, long > >*
gum::SPUDDPlanning<T_DATA>::__argMaxOn2MultiDimDecisionDiagrams( const gum::MultiDimDecisionDiagramBase< std::pair< double, long > >* Vaction1,
        const gum::MultiDimDecisionDiagramBase< std::pair< double, long > >* Vaction2 ) {

    gum::MultiDimDecisionDiagramBase< std::pair< double, long > >* ret = NULL;

    if ( ( Vaction1 == NULL || Vaction1->empty() ) && ( Vaction2 == NULL || Vaction2->empty() ) )
        return ret;

    if ( Vaction1 == NULL || Vaction1->empty() ) {
        ret = reinterpret_cast< gum::MultiDimDecisionDiagramBase< std::pair< double, long > >* >( Vaction2->newFactory() );
        ret->copy( *Vaction2 );
        return ret;
    }

    if ( Vaction2 == NULL || Vaction2->empty() ) {
        ret = reinterpret_cast< gum::MultiDimDecisionDiagramBase< std::pair< double, long > >* >( Vaction1->newFactory() );
        ret->copy( *Vaction1 );
        return ret;
    }

    gum::NonOrderedOperatorData< std::pair< double, long > > opData;
    opData.initialize( Vaction1, Vaction2 );

#ifdef O4DDWITHORDER
    gum::GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( opData, "" );
#else
    gum::GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( opData, NULL, "" );
#endif
    std::pair< double, long > defaultPair( 0,0 );
    ret = opData.factory->getMultiDimDecisionDiagram( false, defaultPair );

    return ret;
}

#undef GUM_MULTI_DIM_OPERATOR
#undef GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION

// ===========================================================================
// Computed the difference of two policy to find out if it's the same or not.
// ===========================================================================

#define GUM_MULTI_DIM_OPERATOR(x,y) T( x.first - y.first, x.second - y.second)
#define GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION DecisionDiagramRecur4difPol

#define GUM_DECISION_DIAGRAM_OPERATOR_FUNCTION_DEFINITION
#ifdef O4DDWITHORDER
#define GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_FUNCTION DecisionDiagramGoDown4difPol
#ifdef O4DDDEBUG
#include <agrum/multidim/patterns/DDUtility/o4DDWithOrderRecursionFunctionsDebug.h>
#else
#include <agrum/multidim/patterns/DDUtility/o4DDWithOrderRecursionFunctions.h>
#endif
#undef GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_FUNCTION
#else
#define GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION DecisionDiagramGoDownOnLeader4difPol
#define GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION DecisionDiagramGoDownOnFollower4difPol
#ifdef O4DDDEBUG
#include <agrum/multidim/patterns/DDUtility/o4DDWithoutOrderRecursionFunctionsDebug.h>
#else
#include <agrum/multidim/patterns/DDUtility/o4DDWithoutOrderRecursionFunctions.h>
#endif
#undef GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION
#undef GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION
#endif
#undef GUM_DECISION_DIAGRAM_OPERATOR_FUNCTION_DEFINITION

template <typename T_DATA>
gum::MultiDimDecisionDiagramBase< std::pair< double, long > >*
gum::SPUDDPlanning<T_DATA>::__differenceOnPolicy( const gum::MultiDimDecisionDiagramBase< std::pair< double, long > >* Vaction1,
        const gum::MultiDimDecisionDiagramBase< std::pair< double, long > >* Vaction2 ) {

    gum::MultiDimDecisionDiagramBase< std::pair< double, long > >* ret = NULL;

    if ( ( Vaction1 == NULL || Vaction1->empty() ) && ( Vaction2 == NULL || Vaction2->empty() ) )
        return ret;

    if ( Vaction1 == NULL || Vaction1->empty() ) {
        ret = reinterpret_cast< gum::MultiDimDecisionDiagramBase< std::pair< double, long > >* >( Vaction2->newFactory() );
        ret->copy( *Vaction2 );
        return ret;
    }

    if ( Vaction2 == NULL || Vaction2->empty() ) {
        ret = reinterpret_cast< gum::MultiDimDecisionDiagramBase< std::pair< double, long > >* >( Vaction1->newFactory() );
        ret->copy( *Vaction1 );
        return ret;
    }

    gum::NonOrderedOperatorData< std::pair< double, long > > opData;
    opData.initialize( Vaction1, Vaction2);

#ifdef O4DDWITHORDER
    gum::GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( opData, "" );
#else
    gum::GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( opData, NULL, "" );
#endif
    //     std::cout << "Fin opération" << std::endl;
    std::pair< double, long > defaultPair( 0,0 );
    ret = opData.factory->getMultiDimDecisionDiagram( false, defaultPair );

    return ret;
}

#undef GUM_MULTI_DIM_OPERATOR
#undef GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION