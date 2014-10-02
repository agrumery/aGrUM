/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
 * @brief Template implementations for the VarInfo class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */
// =========================================================================
#include <cmath>
// =========================================================================
#include <agrum/FMDP/learning/decisionGraph/varInfo.h>
#include <agrum/FMDP/learning/core/chiSquare.h>
// =========================================================================


// constants used by Gary Perlman for his code for computing chi2 critical values

namespace gum {

  // ##########################################################################
  // Constructor & destructor.
  // ##########################################################################

    // ==========================================================================
    // Default constructor
    // ==========================================================================
    VarInfo::VarInfo(const DiscreteVariable* attribute,
                     const DiscreteVariable* value) : __attr(attribute),
                                                      __value(value),
                                                      __degreeOfFreedom((value->domainSize()-1)*(attribute->domainSize()-1)),
                                                      __isRelevant(false)/*,
                                                      __areRelevant(attribute->domainSize()*value->domainSize(),false)*/{

      GUM_CONSTRUCTOR(VarInfo);

      // Initialisation
      for(Idx modality = 0; modality < __attr->domainSize(); ++modality ){

        __modality2Observations.insert(modality, new Set<const Observation*>);

        __contingencyTable.insert( modality, new HashTable<Idx, Idx>());
        for( Idx valMod = 0; valMod < __value->domainSize(); ++valMod )
          __contingencyTable[modality]->insert(valMod, 0);

        __attrMarginalTable.insert( modality, 0);
      }

      for( Idx valMod = 0; valMod < __value->domainSize(); ++valMod )
        __valueMarginalTable.insert(valMod, 0);

      __nbObservation = 0;
      __GStat = 0;
      __pvalue = 1 - ChiSquare::probaChi2(__GStat, __degreeOfFreedom);
    }


    // ==========================================================================
    // Constructor with a list of Observation to iniialize
    // ==========================================================================
    VarInfo::VarInfo(const DiscreteVariable* attribute,
                     const DiscreteVariable* value,
                     const Set<const Observation*>* obsSet) : __attr(attribute),
                                                              __value(value),
                                                              __degreeOfFreedom( (value->domainSize()-1)*(attribute->domainSize()-1) ),
                                                              __isRelevant(false)/*,
                                                              __areRelevant(attribute->domainSize()*value->domainSize(),false)*/{

      GUM_CONSTRUCTOR(VarInfo);

      // Initialisation nécessaire avant la prise en compte des observations
      for(Idx modality = 0; modality < __attr->domainSize(); modality++){
        __modality2Observations.insert(modality, new Set<const Observation*>);

        __contingencyTable.insert( modality, new HashTable<Idx, Idx>());
        for( Idx valMod = 0; valMod < __value->domainSize(); ++valMod )
          __contingencyTable[modality]->insert(valMod, 0);

        __attrMarginalTable.insert( modality, 0);
      }

      for( Idx valMod = 0; valMod < __value->domainSize(); ++valMod )
        __valueMarginalTable.insert(valMod, 0);

      __nbObservation = 0;


      // Prise en compte des observations
      for(SetIteratorSafe<const Observation*> obsIter = obsSet->cbeginSafe(); obsIter != obsSet->cendSafe(); ++obsIter){

        __modality2Observations[(*obsIter)->modality(__attr)]->insert( (*obsIter) );

        __nbObservation++;
        __attrMarginalTable[(*obsIter)->modality(__attr)]++;
        __valueMarginalTable[(*obsIter)->modality(__value)]++;
        (*__contingencyTable[(*obsIter)->modality(__attr)])[(*obsIter)->modality(__value)]++;

      }


      // Calcul de la GStat
      __computeG();
      __pvalue = 1 - ChiSquare::probaChi2(__GStat, __degreeOfFreedom);

      // Check relevance of collected data
//      for( Idx attrModality = 0; attrModality < __attr->domainSize(); ++attrModality ){
//        Idx rank = attrModality*__value->domainSize();
//        for( Idx valModality = 0; valModality < __value->domainSize(); ++valModality ) {
//          if( (*__contingencyTable[attrModality])[valModality] >= 5 )
//              __areRelevant[rank+valModality] = true;
//        }
//      }
      __checkRelevance();
    }


    // ==========================================================================
    // Default destructor
    // ==========================================================================
    VarInfo::~VarInfo(){

      for(Idx modality = 0; modality < __attr->domainSize(); modality++){
        delete __modality2Observations[modality];
        delete __contingencyTable[modality];
      }
      GUM_DESTRUCTOR(VarInfo);
    }



  // ##########################################################################
  //
  // ##########################################################################

    // ==========================================================================
    //
    // ==========================================================================
    void VarInfo::addObservation( const Observation* newObs){

//      std::cout << "Hello There : " << __attr->name() << " - " << __value->name()  << std::endl;
      // Addition of the observation to its matching set for fast splitting
      __modality2Observations[newObs->modality(__attr)]->insert( newObs );

      // Updating
      __nbObservation++;
      __attrMarginalTable[newObs->modality(__attr)]++;
      __valueMarginalTable[newObs->modality(__value)]++;
      (*__contingencyTable[newObs->modality(__attr)])[newObs->modality(__value)]++;

//      if( !__isRelevant && (*__contingencyTable[newObs->modality(__attr)])[newObs->modality(__value)] >= 5 ){
//        __areRelevant[newObs->modality(__attr)*__value->domainSize() + newObs->modality(__value)] = true;
        __checkRelevance();
//      }

      __computeG();
      __pvalue = 1 - ChiSquare::probaChi2(__GStat, __degreeOfFreedom);

//      std::cout << "GStat final : " << __GStat  << " - " << __pvalue << " - " << __isRelevant << std::endl;
      if ( std::isnan(__GStat) )
          exit(-1);
    }


    // ==========================================================================
    //
    // ==========================================================================
    void VarInfo::__computeG(){
      __GStat = 0;
      for (Idx attrModa = 0; attrModa < __attr->domainSize(); ++attrModa ){
        double semiExpected = (double)__attrMarginalTable[attrModa]/(double)__nbObservation;
        for (Idx valModa = 0; valModa < __value->domainSize(); ++valModa) {
          double ceil = (double)(*__contingencyTable[attrModa])[valModa];
          if( ceil < 5 )
            continue;
          double expected = semiExpected*(double)__valueMarginalTable[valModa];

          __GStat += 2*ceil*log(ceil/expected);
//          std::cout << "Attr Moda : " << attrModa << " - Val Modal : " << valModa << " - Ceil : " << ceil << " - Expected : " << expected <<  " - " << __GStat <<std::endl;
        }
      }
    }


    // ==========================================================================
    //
    // ==========================================================================
    void VarInfo::__checkRelevance(){
        if( __isRelevant ) return;

        if( __nbObservation < 25 ) return;/* {
            for(Idx i = 0; i < __areRelevant.size(); ++i )
              if( !__areRelevant[i] ) return;
        }*/

        __isRelevant = true;
    }

} // End of namespace gum
