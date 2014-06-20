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
* @brief Template implementation of FMDP/planning/SPUMDDEval.h classes.
*
* @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
*/

#include <math.h>
#include <fstream>

#include <agrum/core/timer.h>
#include <agrum/core/exceptions.h>

#include <agrum/multidim/potential.h>
#include <agrum/multidim/instantiation.h>
#include <agrum/multidim/multiDimDecisionGraph.h>

#include <agrum/multidim/patterns/DDUtility/o4DDMiscellaneous.h>

#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
#include <agrum/FMDP/planning/spumddEval.h>

namespace gum {


  /* ************************************************************************************************** **/
  /* **                                                                                                 **/
  /* **                                Constructors / Destructors                                       **/
  /* **                                                                                                 **/
  /* ************************************************************************************************** **/

// ===========================================================================
// Default constructor
// ===========================================================================
  template<typename GUM_SCALAR> INLINE
  SPUMDDEval<GUM_SCALAR>::SPUMDDEval ( FactoredMarkovDecisionProcess<GUM_SCALAR>* fmdp, GUM_SCALAR epsilon ) {

    GUM_CONSTRUCTOR ( SPUMDDEval );
    __epsilon = epsilon;
    __fmdp = fmdp;

  }

// ===========================================================================
// Default destructor
// ===========================================================================
  template<typename GUM_SCALAR> INLINE
  SPUMDDEval<GUM_SCALAR>::~SPUMDDEval() {

    GUM_DESTRUCTOR ( SPUMDDEval );

  }


  /* ************************************************************************************************** **/
  /* **                                                                                                 **/
  /* **                                                        Analyse Methods                          **/
  /* **                                                                                                 **/
  /* ************************************************************************************************** **/

// ===========================================================================
// Called for evaluation on algorithm
// ===========================================================================
//  template<typename GUM_SCALAR>
//  MultiDimDecisionGraph< GUM_SCALAR >*
//  SPUMDDEval<GUM_SCALAR>::makePlanningAlgoEvaluation ( const std::string saveFilesName, Idx mode ) {

//    // *****************************************************************************************
//    // Initialisation
//    // *****************************************************************************************

//    // *****************************************************************************************
//    // Threshold stopping criterion evaluation
//    GUM_SCALAR threshold =  0.1;

//    GUM_SCALAR gap = threshold + 1;

//    // *****************************************************************************************
//    // Initialisation of Vold, Vnew and Vtemp
//    MultiDimDecisionGraph< GUM_SCALAR >* Vold = reinterpret_cast<MultiDimDecisionGraph<GUM_SCALAR>*> ( __fmdp->reward()->newFactory() );
//    Vold->copy ( *reinterpret_cast<const MultiDimDecisionGraph<GUM_SCALAR>*> ( __fmdp->reward() ) );

//    MultiDimDecisionGraph< GUM_SCALAR >* Vnew = nullptr;
//    MultiDimDecisionGraph< GUM_SCALAR >* Vtemp = nullptr;

//    Idx nbIte = 0;

//    // *****************************************************************************************
//    // Main loop
//    // *****************************************************************************************
//    while ( gap >  threshold ) {

//      ++nbIte;

//      // *****************************************************************************************
//      // Loop reset
//      __fmdp->resetActionsIterator();
//      Set< MultiDimDecisionGraph< GUM_SCALAR >* > VactionCollector;

//      Vnew = reinterpret_cast<MultiDimDecisionGraph<GUM_SCALAR>*> ( Vold->newFactory() );
//      Vnew->copyAndReassign ( Vold, __fmdp->main2prime() );

//      // *****************************************************************************************
//      // For each action
//      while ( __fmdp->hasAction() ) {

//// //std::cout << " ------------ Evaluation Nouvelle Action " << std::endl;
//        VactionCollector.insert ( __evalActionValueAlgoEvaluation ( Vnew, saveFilesName, mode ) );
//// //std::cout << " Fin Evaluation" << std::endl;

//        __fmdp->nextAction();
//      }

//      // *****************************************************************************************
//      // Next to evaluate main value function, we take maximise over all action value, ...
//// //std::cout << " ------------- Maximisation " << std::endl;

//      delete Vnew;
//      Vnew = nullptr;
//      Idx nbNodeT1;
//      Idx nbNodeT2;
//      Idx nbRetroVarDirect;
//      Idx tailleEspaceRetrogradeDirect;
//      Idx nbRetroVarIndirect;
//      Idx tailleEspaceRetrogradeIndirect;

//      Idx nbFinalNodeDirect;
//      double iterationTimeDirect;
//      Idx nbFinalNodeIndirect;
//      double iterationTimeIndirect;

//      double iterationTime;
//      double totalTime = 0;

//      std::stringstream traceFileName;
//      traceFileName << "./Trace/Maximisation/" << saveFilesName << "." << mode << ".log";
//      __traceAlgoSaveFile.open ( traceFileName.str().c_str(), std::ios::out | std::ios::app );

//      if ( !__traceAlgoSaveFile ) {
//        GUM_ERROR ( IOError, "log file" << traceFileName << " does not open correctly" );
//      }

//      for ( SetIteratorSafe< MultiDimDecisionGraph< GUM_SCALAR >* > VActionsIter = VactionCollector.beginSafe(); VActionsIter != VactionCollector.endSafe(); ++VActionsIter ) {

//        if ( Vnew != nullptr )
//          nbNodeT1 = Vnew->nodesMap().size();
//        else
//          nbNodeT1 = 0;

//        nbNodeT2 = ( *VActionsIter )->nodesMap().size();

//        std::pair<Idx, Idx> resEvalRetro = __evalNbRetrogradeEvaluation ( Vnew, *VActionsIter );
//        nbRetroVarDirect = resEvalRetro.first;
//        tailleEspaceRetrogradeDirect = resEvalRetro.second;

//        resEvalRetro = __evalNbRetrogradeEvaluation ( *VActionsIter, Vnew );
//        nbRetroVarIndirect = resEvalRetro.first;
//        tailleEspaceRetrogradeIndirect = resEvalRetro.second;

//        bool sensDirect = true;

//        switch ( mode ) {
//          case 2 :
//            sensDirect = false;
//            break;

//          case 3 :
//            if ( nbRetroVarDirect > nbRetroVarIndirect )
//              sensDirect = false;

//            break;

//          case 4 :
//            if ( tailleEspaceRetrogradeDirect > tailleEspaceRetrogradeIndirect )
//              sensDirect = false;

//            break;

//          default :
//            break;
//        }

//        Vtemp = Vnew;
//        Timer time;

//        if ( sensDirect ) {
//          time.reset();
//          MultiDimDecisionGraph< GUM_SCALAR >* Vind = maximize2MultiDimDecisionGraphs ( *VActionsIter, Vnew );
//          nbFinalNodeIndirect = Vind->nodesMap().size();
//          iterationTimeIndirect = time.step();
//          delete Vind;

//          time.reset();
//          Vnew = maximize2MultiDimDecisionGraphs ( Vnew, *VActionsIter );
//          nbFinalNodeDirect = Vnew->nodesMap().size();
//          iterationTimeDirect = time.step();
//          iterationTime = iterationTimeDirect;
//        } else {
//          time.reset();
//          MultiDimDecisionGraph< GUM_SCALAR >* Vind = maximize2MultiDimDecisionGraphs ( Vnew, *VActionsIter );
//          nbFinalNodeDirect = Vind->nodesMap().size();
//          iterationTimeDirect = time.step();
//          delete Vind;

//          time.reset();
//          Vnew = maximize2MultiDimDecisionGraphs ( *VActionsIter, Vnew );
//          nbFinalNodeIndirect = Vnew->nodesMap().size();
//          iterationTimeIndirect = time.step();
//          iterationTime = iterationTimeIndirect;
//        }

//        totalTime += time.step();
//        delete Vtemp;
//        delete *VActionsIter;

//        __traceAlgoSaveFile << nbNodeT1 << "\t" << nbNodeT2 << "\t" << nbRetroVarDirect << "\t" << tailleEspaceRetrogradeDirect << "\t" << nbFinalNodeDirect << "\t" <<
//                            iterationTimeDirect << "\t" << nbRetroVarIndirect << "\t" << tailleEspaceRetrogradeIndirect << "\t" << nbFinalNodeIndirect << "\t" << iterationTimeIndirect
//                            << "\t" << sensDirect << "\t" << iterationTime << "\t" << totalTime << std::endl;
//      }

//      __traceAlgoSaveFile.close();
//// //std::cout << " Fin Maximisation " << std::endl;

//      // *******************************************************************************************
//      // Next, we eval the new function value
//      Vtemp = Vnew;
//// //std::cout << " ---------- Ajout Récompense " << std::endl;
//      Vnew = __addRewardAlgoEvaluation ( Vtemp, saveFilesName, mode );
//// //std::cout << " Fin Ajout Récompense " << std::endl;
//      delete Vtemp;


//      // *****************************************************************************************
//      // Then we compare new value function and the old one

//      MultiDimDecisionGraph< GUM_SCALAR >* deltaV = subtract2MultiDimDecisionGraphs ( Vnew, Vold );
//      gap = 0;

//      for ( BijectionIteratorSafe< NodeId, GUM_SCALAR > valIter = deltaV->valuesMap().beginSafe(); valIter != deltaV->valuesMap().endSafe(); ++valIter )
//        if ( gap < fabs ( valIter.second() ) )
//          gap = fabs ( valIter.second() );

//      delete deltaV;

//      // //std::cout << "*************************************************" << std::endl << " Essai : " << saveFilesName << std::endl << " ------------------- Fin itération n° " << nbIte << std::endl << " Gap : " << gap << std::endl;
//      // ****************************************************************************************
//      // And eventually we update pointers for next loop
//      delete Vold;
//      Vold = Vnew;
//    }

//    delete Vold;
//    return Vold;

//      return nullptr;

//  }



// ===========================================================================
// Evals the value function for current fmdp action
// ===========================================================================
//  template<typename GUM_SCALAR>
//  MultiDimDecisionGraph<GUM_SCALAR>*
//  SPUMDDEval<GUM_SCALAR>::__evalActionValueAlgoEvaluation ( const MultiDimDecisionGraph< GUM_SCALAR >* Vold, const std::string saveFilesName, Idx mode ) {

//    __fmdp->resetVariablesIterator();

//    MultiDimDecisionGraph< GUM_SCALAR >* Vaction = reinterpret_cast<MultiDimDecisionGraph<GUM_SCALAR>*> ( Vold->newFactory() );
//    Vaction->copy ( *Vold );
//    MultiDimDecisionGraph< GUM_SCALAR >* Vtemp = nullptr;

//    Idx nbNodeT1;
//    Idx nbNodeT2;
//    Idx nbRetroVarDirect;
//    Idx tailleEspaceRetrogradeDirect;
//    Idx nbRetroVarIndirect;
//    Idx tailleEspaceRetrogradeIndirect;

//    Idx nbFinalNodeDirect;
//    double iterationTimeDirect;
//    Idx nbFinalNodeIndirect;
//    double iterationTimeIndirect;

//    double iterationTime;
//    double totalTime = 0;

//    std::string actionName = __fmdp->actionName ( __fmdp->actionIterId() );
//    std::stringstream traceFileName;
//    traceFileName << "./Trace/" << actionName << "/" << saveFilesName << "." << mode << ".log";
//    __traceAlgoSaveFile.open ( traceFileName.str().c_str(), std::ios::out | std::ios::app );

//    if ( !__traceAlgoSaveFile ) {
//      std::cerr << "Erreur à l'ouverture ! Fichier en cause : " << traceFileName.str() << std::endl;
//      return nullptr;
//    }

//    // *****************************************************************************************
//    // To evaluate action value function, we multiply old main value function by transition table
//    // of each variable
//    while ( __fmdp->hasVariable() ) {


//      // Multiplication of Vaction by current variable's CPT
//      const MultiDimDecisionGraph<GUM_SCALAR>* cpt = reinterpret_cast<const MultiDimDecisionGraph<GUM_SCALAR>*> ( __fmdp->transition() );

//      nbNodeT1 =  Vaction->nodesMap().size();
//      nbNodeT2 = cpt->nodesMap().size();

//      std::pair<Idx, Idx> resEvalRetro = __evalNbRetrogradeEvaluation ( Vaction, cpt );
//      nbRetroVarDirect = resEvalRetro.first;
//      tailleEspaceRetrogradeDirect = resEvalRetro.second;

//      resEvalRetro = __evalNbRetrogradeEvaluation ( cpt, Vaction );
//      nbRetroVarIndirect = resEvalRetro.first;
//      tailleEspaceRetrogradeIndirect = resEvalRetro.second;

//      bool sensDirect = true;

//      switch ( mode ) {
//        case 2 :
//          sensDirect = false;
//          break;

//        case 3 :
//          if ( nbRetroVarDirect > nbRetroVarIndirect )
//            sensDirect = false;

//          break;

//        case 4 :
//          if ( tailleEspaceRetrogradeDirect > tailleEspaceRetrogradeIndirect )
//            sensDirect = false;

//          break;

//        default :
//          break;
//      }

//      Timer time;
//      Vtemp = Vaction;

//      if ( sensDirect ) {
//        time.reset();
//        MultiDimDecisionGraph< GUM_SCALAR >* Vind = multiply2MultiDimDecisionGraphs ( cpt, Vaction );
//        nbFinalNodeIndirect = Vind->nodesMap().size();
//        iterationTimeIndirect = time.step();
//        delete Vind;

//        time.reset();
//        Vaction = multiply2MultiDimDecisionGraphs ( Vaction, cpt );
//        nbFinalNodeDirect = Vaction->nodesMap().size();
//        iterationTimeDirect = time.step();
//        iterationTime = iterationTimeDirect;
//      } else {
//        time.reset();
//        MultiDimDecisionGraph< GUM_SCALAR >* Vind = multiply2MultiDimDecisionGraphs ( Vaction, cpt );
//        nbFinalNodeDirect = Vind->nodesMap().size();
//        iterationTimeDirect = time.step();
//        delete Vind;

//        time.reset();
//        Vaction = multiply2MultiDimDecisionGraphs ( cpt, Vaction );
//        nbFinalNodeIndirect = Vaction->nodesMap().size();
//        iterationTimeIndirect = time.step();
//        iterationTime = iterationTimeIndirect;
//      }

//      delete Vtemp;

//      Vtemp = Vaction;
//      Set< const DiscreteVariable* > varSet;
//      varSet << __fmdp->variable();
//      Vaction = projectSumMultiDimDecisionGraph ( Vaction, varSet );
//      delete Vtemp;
//      totalTime += time.step();

//      __traceAlgoSaveFile << nbNodeT1 << "\t" << nbNodeT2 << "\t" << nbRetroVarDirect << "\t" << tailleEspaceRetrogradeDirect << "\t" << nbFinalNodeDirect << "\t" <<
//                          iterationTimeDirect << "\t" << nbRetroVarIndirect << "\t" << tailleEspaceRetrogradeIndirect << "\t" << nbFinalNodeIndirect << "\t" << iterationTimeIndirect
//                          << "\t" << sensDirect << "\t" << iterationTime << "\t" << totalTime << std::endl;

//      __fmdp->nextVariable();

//    }

//    __traceAlgoSaveFile.close();

//// //std::cout << "DONE!" << std::endl;

//    return Vaction;
//      return nullptr;

//  }



// ===========================================================================
// Updates the value function by multiplying by discount and adding reward
// ===========================================================================
//  template<typename GUM_SCALAR>
//  MultiDimDecisionGraph<GUM_SCALAR>*
//  SPUMDDEval<GUM_SCALAR>::__addRewardAlgoEvaluation ( const MultiDimDecisionGraph< GUM_SCALAR >* Vold, const std::string saveFilesName, Idx mode ) {

//    // *****************************************************************************************
//    // ... we multiply the result by the discount factor, ...
//    MultiDimDecisionGraph< GUM_SCALAR >* Vnew = reinterpret_cast<MultiDimDecisionGraph<GUM_SCALAR>*> ( Vold->newFactory() );
//    Vnew->multiplyByScalar ( Vold, __fmdp->discount() );

//    // *****************************************************************************************
//    // ... and finally add reward
//    MultiDimDecisionGraph< GUM_SCALAR >* Vtemp = Vnew;

//    Idx nbNodeT1;
//    Idx nbNodeT2;
//    Idx nbRetroVarDirect;
//    Idx tailleEspaceRetrogradeDirect;
//    Idx nbRetroVarIndirect;
//    Idx tailleEspaceRetrogradeIndirect;

//    Idx nbFinalNodeDirect;
//    double iterationTimeDirect;
//    Idx nbFinalNodeIndirect;
//    double iterationTimeIndirect;

//    double totalTime = 0;

//    std::stringstream traceFileName;
//    traceFileName << "./Trace/AddReward/" << saveFilesName << "." << mode << ".log";
//    __traceAlgoSaveFile.open ( traceFileName.str().c_str(), std::ios::out | std::ios::app );

//    if ( !__traceAlgoSaveFile ) {
//      std::cerr << "Erreur à l'ouverture !" << std::endl;
//      return nullptr;
//    }

//    const MultiDimDecisionGraph<GUM_SCALAR>* reward = reinterpret_cast<const MultiDimDecisionGraph<GUM_SCALAR>*> ( __fmdp->reward() );

//    if ( Vnew != nullptr )
//      nbNodeT1 = Vnew->nodesMap().size();
//    else
//      nbNodeT1 = 0;

//    nbNodeT2 = reward->nodesMap().size();

//    std::pair<Idx, Idx> resEvalRetro = __evalNbRetrogradeEvaluation ( Vnew, reward );
//    nbRetroVarDirect = resEvalRetro.first;
//    tailleEspaceRetrogradeDirect = resEvalRetro.second;

//    resEvalRetro = __evalNbRetrogradeEvaluation ( reward, Vnew );
//    nbRetroVarIndirect = resEvalRetro.first;
//    tailleEspaceRetrogradeIndirect = resEvalRetro.second;

//    bool sensDirect = true;

//    switch ( mode ) {
//      case 2 :
//        sensDirect = false;
//        break;

//      case 3 :
//        if ( nbRetroVarDirect > nbRetroVarIndirect )
//          sensDirect = false;

//        break;

//      case 4 :
//        if ( tailleEspaceRetrogradeDirect > tailleEspaceRetrogradeIndirect )
//          sensDirect = false;

//        break;

//      default :
//        break;
//    }


//    Timer time;

//    if ( sensDirect ) {
//      time.reset();
//      MultiDimDecisionGraph< GUM_SCALAR >* Vind = add2MultiDimDecisionGraphs ( reward, Vnew );
//      nbFinalNodeIndirect = Vind->nodesMap().size();
//      iterationTimeIndirect = time.step();
//      delete Vind;

//      time.reset();
//      Vnew = add2MultiDimDecisionGraphs ( Vnew, reward );
//      nbFinalNodeDirect = Vnew->nodesMap().size();
//      iterationTimeDirect = time.step();
//    } else {
//      time.reset();
//      MultiDimDecisionGraph< GUM_SCALAR >* Vind = add2MultiDimDecisionGraphs ( Vnew, reward );
//      nbFinalNodeDirect = Vind->nodesMap().size();
//      iterationTimeDirect = time.step();
//      delete Vind;

//      time.reset();
//      Vnew = add2MultiDimDecisionGraphs ( reward, Vnew );
//      nbFinalNodeIndirect = Vnew->nodesMap().size();
//      iterationTimeIndirect = time.step();
//    }

//    totalTime += time.step();

//    __traceAlgoSaveFile << nbNodeT1 << "\t" << nbNodeT2 << "\t" << nbRetroVarDirect << "\t" << tailleEspaceRetrogradeDirect << "\t" << nbFinalNodeDirect << "\t" <<
//                        iterationTimeDirect << "\t" << nbRetroVarIndirect << "\t" << tailleEspaceRetrogradeIndirect << "\t" << nbFinalNodeIndirect << "\t" << iterationTimeIndirect  << "\t" << totalTime << std::endl;

//    __traceAlgoSaveFile.close();
//    delete Vtemp;

//    return Vnew;
//      return nullptr;

//  }



// ===========================================================================
// Evals how many retrograde variable there will be if we do the operation in
// that order
// ===========================================================================
//  template<typename GUM_SCALAR>
//  std::pair<Idx, Idx>
//  SPUMDDEval<GUM_SCALAR>::__evalNbRetrogradeEvaluation ( const MultiDimDecisionGraph<GUM_SCALAR>* dD1, const MultiDimDecisionGraph<GUM_SCALAR>* dD2 ) {


//    std::pair<Idx, Idx> res;

//    if ( dD1 == nullptr || dD2 == nullptr ) {
//      res.first = 0;
//      res.second = 0;
//      return res;
//    }

//    // *****************************************************************************
//    // First we determine the new var sequence

//    Sequence< const DiscreteVariable* > dD1VarSeq = dD1->variablesSequence();
//    Sequence< const DiscreteVariable* > dD2VarSeq = dD2->variablesSequence();
//    Sequence< const DiscreteVariable* > fusVarSeq;

//    SequenceIteratorSafe< const DiscreteVariable* > iterS1 = dD1VarSeq.beginSafe();
//    SequenceIteratorSafe< const DiscreteVariable* > iterS2 = dD2VarSeq.beginSafe();

//    while ( iterS1 != dD1VarSeq.endSafe() || iterS2 != dD2VarSeq.endSafe() ) {
//      if ( iterS1 == dD1VarSeq.endSafe() ) {
//        for ( ; iterS2 != dD2VarSeq.endSafe(); ++iterS2 )
//          if ( !fusVarSeq.exists ( *iterS2 ) )
//            fusVarSeq.insert ( *iterS2 );
//      } else if ( iterS2 == dD2VarSeq.endSafe() ) {
//        for ( ; iterS1 != dD1VarSeq.endSafe(); ++iterS1 )
//          if ( !fusVarSeq.exists ( *iterS1 ) )
//            fusVarSeq.insert ( *iterS1 );
//      } else {
//        if ( *iterS1 == *iterS2 ) {
//          if ( !fusVarSeq.exists ( *iterS1 ) )
//            fusVarSeq.insert ( *iterS1 );

//          ++iterS1;
//          ++iterS2;
//          continue;
//        }

//        if ( dD1VarSeq.pos ( *iterS1 ) <= dD2VarSeq.pos ( *iterS2 ) || dD1VarSeq.exists ( *iterS2 ) ) {
//          if ( !fusVarSeq.exists ( *iterS1 ) )
//            fusVarSeq.insert ( *iterS1 );

//          ++iterS1;
//          continue;
//        } else {
//          if ( !fusVarSeq.exists ( *iterS2 ) )
//            fusVarSeq.insert ( *iterS2 );

//          ++iterS2;
//          continue;
//        }
//      }
//    }

//    // ******************************************************************************
//    // Then we search in second Graph for possible preneeded variable
//    Idx sizeRetro = 1;
//    Idx nbRetroVar = 0;

//    for ( iterS2 = dD2VarSeq.beginSafe(); iterS2 != dD2VarSeq.endSafe(); ++iterS2 )
//      for ( iterS1 = iterS2; iterS1 != dD2VarSeq.rendSafe(); --iterS1 )
//        if ( fusVarSeq.pos ( *iterS1 ) > fusVarSeq.pos ( *iterS2 ) ) {
//          nbRetroVar++;
//          sizeRetro *= ( *iterS2 )->domainSize();
//        }

//    res.first = nbRetroVar;
//    res.second = sizeRetro;

//    return res;
//      return nullptr;

//  }


}



// ===========================================================================
// Computed arg max of two vactions given in parameter
// ===========================================================================

//#define GUM_MULTI_DIM_OPERATOR(x,y) x.first>y.first?x:y
//#define GUM_MULTI_DIM_DECISION_Graph_RECUR_FUNCTION DecisionGraphRecur4argMax

//#define GUM_DECISION_Graph_OPERATOR_FUNCTION_DEFINITION
//#ifdef O4DDWITHORDER
//#define GUM_MULTI_DIM_DECISION_Graph_GO_DOWN_FUNCTION DecisionGraphGoDown4argMax
//#ifdef O4DDDEBUG
//#include <agrum/multidim/patterns/DDUtility/o4DDWithOrderRecursionFunctionsDebug.h>
//#else
//#include <agrum/multidim/patterns/DDUtility/o4DDWithOrderRecursionFunctions.h>
//#endif
//#undef GUM_MULTI_DIM_DECISION_Graph_GO_DOWN_FUNCTION
//#else
//#define GUM_MULTI_DIM_DECISION_Graph_GO_DOWN_ON_LEADER_FUNCTION DecisionGraphGoDownOnLeader4argMax
//#define GUM_MULTI_DIM_DECISION_Graph_GO_DOWN_ON_FOLLOWER_FUNCTION DecisionGraphGoDownOnFollower4argMax
//#ifdef O4DDDEBUG
//#include <agrum/multidim/patterns/DDUtility/o4DDWithoutOrderRecursionFunctionsDebug.h>
//#else
//#include <agrum/multidim/patterns/DDUtility/o4DDWithoutOrderRecursionFunctions.h>
//#endif
//#undef GUM_MULTI_DIM_DECISION_Graph_GO_DOWN_ON_LEADER_FUNCTION
//#undef GUM_MULTI_DIM_DECISION_Graph_GO_DOWN_ON_FOLLOWER_FUNCTION
//#endif
//#undef GUM_DECISION_Graph_OPERATOR_FUNCTION_DEFINITION

//template <typename GUM_SCALAR>
//gum::MultiDimDecisionGraph< std::pair< double, long > >*
//gum::SPUMDDEval<GUM_SCALAR>::__argMaxOn2MultiDimDecisionGraphs ( const gum::MultiDimDecisionGraph< std::pair< double, long > >* Vaction1,
//    const gum::MultiDimDecisionGraph< std::pair< double, long > >* Vaction2 ) {

//  gum::MultiDimDecisionGraph< std::pair< double, long > >* ret = nullptr;

//  if ( ( Vaction1 == nullptr || Vaction1->empty() ) && ( Vaction2 == nullptr || Vaction2->empty() ) )
//    return ret;

//  if ( Vaction1 == nullptr || Vaction1->empty() ) {
//    ret = reinterpret_cast< gum::MultiDimDecisionGraph< std::pair< double, long > >* > ( Vaction2->newFactory() );
//    ret->copy ( *Vaction2 );
//    return ret;
//  }

//  if ( Vaction2 == nullptr || Vaction2->empty() ) {
//    ret = reinterpret_cast< gum::MultiDimDecisionGraph< std::pair< double, long > >* > ( Vaction1->newFactory() );
//    ret->copy ( *Vaction1 );
//    return ret;
//  }

//  gum::NonOrderedOperatorData< std::pair< double, long > > opData;
//  opData.initialize ( Vaction1, Vaction2 );

//#ifdef O4DDWITHORDER
//  gum::GUM_MULTI_DIM_DECISION_Graph_RECUR_FUNCTION ( opData, "" );
//#else
//  gum::GUM_MULTI_DIM_DECISION_Graph_RECUR_FUNCTION ( opData, nullptr, "" );
//#endif
//  std::pair< double, long > defaultPair ( 0, 0 );
//  ret = opData.factory->getMultiDimDecisionGraph ( false, defaultPair );

//  return ret;
//}

//#undef GUM_MULTI_DIM_OPERATOR
//#undef GUM_MULTI_DIM_DECISION_Graph_RECUR_FUNCTION

//// ===========================================================================
//// Computed the difference of two policy to find out if it's the same or not.
//// ===========================================================================

//#define GUM_MULTI_DIM_OPERATOR(x,y) T( x.first - y.first, x.second - y.second)
//#define GUM_MULTI_DIM_DECISION_Graph_RECUR_FUNCTION DecisionGraphRecur4difPol

//#define GUM_DECISION_Graph_OPERATOR_FUNCTION_DEFINITION
//#ifdef O4DDWITHORDER
//#define GUM_MULTI_DIM_DECISION_Graph_GO_DOWN_FUNCTION DecisionGraphGoDown4difPol
//#ifdef O4DDDEBUG
//#include <agrum/multidim/patterns/DDUtility/o4DDWithOrderRecursionFunctionsDebug.h>
//#else
//#include <agrum/multidim/patterns/DDUtility/o4DDWithOrderRecursionFunctions.h>
//#endif
//#undef GUM_MULTI_DIM_DECISION_Graph_GO_DOWN_FUNCTION
//#else
//#define GUM_MULTI_DIM_DECISION_Graph_GO_DOWN_ON_LEADER_FUNCTION DecisionGraphGoDownOnLeader4difPol
//#define GUM_MULTI_DIM_DECISION_Graph_GO_DOWN_ON_FOLLOWER_FUNCTION DecisionGraphGoDownOnFollower4difPol
//#ifdef O4DDDEBUG
//#include <agrum/multidim/patterns/DDUtility/o4DDWithoutOrderRecursionFunctionsDebug.h>
//#else
//#include <agrum/multidim/patterns/DDUtility/o4DDWithoutOrderRecursionFunctions.h>
//#endif
//#undef GUM_MULTI_DIM_DECISION_Graph_GO_DOWN_ON_LEADER_FUNCTION
//#undef GUM_MULTI_DIM_DECISION_Graph_GO_DOWN_ON_FOLLOWER_FUNCTION
//#endif
//#undef GUM_DECISION_Graph_OPERATOR_FUNCTION_DEFINITION

//template <typename GUM_SCALAR>
//gum::MultiDimDecisionGraph< std::pair< double, long > >*
//gum::SPUMDDEval<GUM_SCALAR>::__differenceOnPolicy ( const gum::MultiDimDecisionGraph< std::pair< double, long > >* Vaction1,
//    const gum::MultiDimDecisionGraph< std::pair< double, long > >* Vaction2 ) {

//  gum::MultiDimDecisionGraph< std::pair< double, long > >* ret = nullptr;

//  if ( ( Vaction1 == nullptr || Vaction1->empty() ) && ( Vaction2 == nullptr || Vaction2->empty() ) )
//    return ret;

//  if ( Vaction1 == nullptr || Vaction1->empty() ) {
//    ret = reinterpret_cast< gum::MultiDimDecisionGraph< std::pair< double, long > >* > ( Vaction2->newFactory() );
//    ret->copy ( *Vaction2 );
//    return ret;
//  }

//  if ( Vaction2 == nullptr || Vaction2->empty() ) {
//    ret = reinterpret_cast< gum::MultiDimDecisionGraph< std::pair< double, long > >* > ( Vaction1->newFactory() );
//    ret->copy ( *Vaction1 );
//    return ret;
//  }

//  gum::NonOrderedOperatorData< std::pair< double, long > > opData;
//  opData.initialize ( Vaction1, Vaction2 );

//#ifdef O4DDWITHORDER
//  gum::GUM_MULTI_DIM_DECISION_Graph_RECUR_FUNCTION ( opData, "" );
//#else
//  gum::GUM_MULTI_DIM_DECISION_Graph_RECUR_FUNCTION ( opData, nullptr, "" );
//#endif
//  //     //std::cout << "Fin opération" << std::endl;
//  std::pair< double, long > defaultPair ( 0, 0 );
//  ret = opData.factory->getMultiDimDecisionGraph ( false, defaultPair );

//  return ret;
//}

//#undef GUM_MULTI_DIM_OPERATOR
//#undef GUM_MULTI_DIM_DECISION_Graph_RECUR_FUNCTION
