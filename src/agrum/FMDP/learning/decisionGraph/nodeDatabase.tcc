/***************************************************************************
 *  Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN *
 *  {prenom.nom}_at_lip6.fr                        *
 *                                     *
 *  This program is free software; you can redistribute it and/or modify *
 *  it under the terms of the GNU General Public License as published by *
 *  the Free Software Foundation; either version 2 of the License, or   *
 *  (at your option) any later version.                  *
 *                                     *
 *  This program is distributed in the hope that it will be useful,    *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of    *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the     *
 *  GNU General Public License for more details.             *
 *                                     *
 *  You should have received a copy of the GNU General Public License   *
 *  along with this program; if not, write to the             *
 *  Free Software Foundation, Inc.,                    *
 *  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.       *
 ***************************************************************************/
/**
 * @file
 * @brief Headers of the NodeDatabase class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */


// =========================================================================
#include <agrum/FMDP/learning/decisionGraph/nodeDatabase.h>
// =========================================================================
#include <agrum/multidim/multiDimDecisionGraph.h>
// =========================================================================

namespace gum {

  // ==========================================================================
  // Constructor & destructor.
  // ==========================================================================

    // ###################################################################
    // Default constructor
    // ###################################################################
    template <typename GUM_SCALAR>
    NodeDatabase<GUM_SCALAR>::NodeDatabase(const Set<const DiscreteVariable*>* attrSet,
                                           const DiscreteVariable* value ) : __attrSet(attrSet),
                                                                             __value(value){

      GUM_CONSTRUCTOR(NodeDatabase);

      for(SetIteratorSafe<const DiscreteVariable*> varIter = __attrSet->cbeginSafe(); varIter != __attrSet->cendSafe(); ++varIter)
        __attrTable.insert( *varIter, new VarInfo(*varIter, value) );

      __nbObservation = 0;

      for(Idx modality = 0; modality < __value->domainSize(); ++modality)
        __valueCount.insert(modality, 0);
    }


    // ###################################################################
    // Default constructor
    // ###################################################################
    template <typename GUM_SCALAR>
    NodeDatabase<GUM_SCALAR>::NodeDatabase(const Set<const DiscreteVariable*>* attrSet,
                                           const DiscreteVariable* value,
                                           const Set< const Observation*>* obsList) : __attrSet(attrSet),
                                                                                      __value(value){

      GUM_CONSTRUCTOR(NodeDatabase);

      for(SetIteratorSafe<const DiscreteVariable*> varIter = __attrSet->cbeginSafe(); varIter != __attrSet->cendSafe(); ++varIter)
        __attrTable.insert( *varIter, new VarInfo(*varIter, value, obsList) );

      __nbObservation = obsList->size();

      for(Idx modality = 0; modality < __value->domainSize(); ++modality)
        __valueCount.insert(modality, 0);
      for( auto obsIter = obsList->cbeginSafe(); obsIter != obsList->cendSafe(); ++obsIter )
        __valueCount[(*obsIter)->modality(__value)]++;
    }


    // ###################################################################
    // Default desstructor
    // ###################################################################
    template <typename GUM_SCALAR>
    NodeDatabase<GUM_SCALAR>::~NodeDatabase(){

      for(auto varIter = __attrTable.beginSafe(); varIter != __attrTable.endSafe(); ++varIter)
        delete varIter.val();

      GUM_DESTRUCTOR(NodeDatabase);
    }



  // ==========================================================================
  //
  // ==========================================================================

    // ###################################################################
    //
    // ###################################################################
    template <typename GUM_SCALAR>
    void NodeDatabase<GUM_SCALAR>::addObservation( const Observation* newObs){

      for(auto varIter = __attrTable.cbeginSafe(); varIter != __attrTable.cendSafe(); ++varIter)
        varIter.val()->addObservation( newObs);
      __nbObservation++;

      __valueCount[newObs->modality(__value)]++;
    }


    // ###################################################################
    //
    // ###################################################################
    template <typename GUM_SCALAR>
    Sequence<NodeDatabase<GUM_SCALAR>*> NodeDatabase<GUM_SCALAR>::splitOnVar(const DiscreteVariable* var){

      Sequence<NodeDatabase<GUM_SCALAR>*> reseq;

      for(Idx modality = 0; modality < var->domainSize(); ++modality)
        reseq.insert( new NodeDatabase<GUM_SCALAR>( __attrSet, __value, __attrTable[var]->observationSet(modality)) );

      return reseq;
    }

    // ###################################################################
    ///
    // ###################################################################
    template <typename GUM_SCALAR>
    GUM_SCALAR* NodeDatabase<GUM_SCALAR>::probDist(){
//        std::cout << "NodeDataBase 134" << std::endl;
      GUM_SCALAR* ret = static_cast<GUM_SCALAR*>(MultiDimDecisionGraph<GUM_SCALAR>::soa.allocate(sizeof(GUM_SCALAR)*__value->domainSize()));
      for(Idx modality = 0; modality < __value->domainSize(); ++modality)
        ret[modality] = (GUM_SCALAR)__valueCount[modality] / (GUM_SCALAR)__nbObservation;
      return ret;
    }

    // ###################################################################
    ///
    // ###################################################################
    template <typename GUM_SCALAR>
    GUM_SCALAR NodeDatabase<GUM_SCALAR>::rewardValue(){
      GUM_SCALAR ret = (GUM_SCALAR)0;
      for(Idx modality = 0; modality < __value->domainSize(); ++modality)
        ret += (GUM_SCALAR)__valueCount[modality] / (GUM_SCALAR)__nbObservation * std::stod(__value->label(modality));
      return ret;
    }

    // ###################################################################
    ///
    // ###################################################################
    template <typename GUM_SCALAR>
    GUM_SCALAR* NodeDatabase<GUM_SCALAR>::effectif(){
        GUM_SCALAR* ret = static_cast<GUM_SCALAR*>(MultiDimDecisionGraph<GUM_SCALAR>::soa.allocate(sizeof(GUM_SCALAR)*__value->domainSize()));
        for(Idx modality = 0; modality < __value->domainSize(); ++modality)
          ret[modality] = (GUM_SCALAR)__valueCount[modality];
        return ret;
    }
} // End of namespace gum
