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
#include <agrum/FMDP/learning/decision graph/varInfo.h>
// =========================================================================

namespace gum {

    // ##########################################################################
    // Constructor & destructor.
    // ##########################################################################

        // ==========================================================================
        // Default constructor
        // ==========================================================================
        template <typename GUM_SCALAR>
        VarInfo<GUM_SCALAR>::VarInfo(const DiscreteVariable* attribute, const DiscreteVariable* value) : __attr(attribute),
        __value(value){

            GUM_CONSTRUCTOR(VarInfo);

            for(Idx modality = 0; modality < __attr->domainSize(); modality++){
                __modality2Observations.insert(modality, new Set<const Observation<GUM_SCALAR>*>);
                __contingencyTable.insert( modality, new HashTable<Idx, Idx>());
            }

        }


        template <typename GUM_SCALAR>
        VarInfo<GUM_SCALAR>::VarInfo(const DiscreteVariable* attribute, const DiscreteVariable* value, const Set<const Observation<GUM_SCALAR>*>* obsSet) : __attr(attribute),
        __value(value){

            GUM_CONSTRUCTOR(VarInfo);

            for(Idx modality = 0; modality < __attr->domainSize(); modality++){
                __modality2Observations.insert(modality, new Set<const Observation<GUM_SCALAR>*>);
                __contingencyTable.insert( modality, new HashTable<Idx, Idx>());
            }

            for(SetIteratorSafe<const Observation<GUM_SCALAR>*> obsIter = obsSet->cbeginSafe(); obsIter != obsSet->cendSafe(); ++obsIter){

                __modality2Observations[(*obsIter)->modality(__attr)]->insert( (*obsIter) );

                __nbObservation++;
                __attrMarginalTable[(*obsIter)->modality(__attr)]++;
                __valueMarginalTable[(*obsIter)->modality(__attr)]++;
                (*__contingencyTable[(*obsIter)->modality(__attr)])[(*obsIter)->modality(__value)]++;

            }

            for( Idx attrModality = 0; attrModality < __attr->domainSize(); ++attrModality )
                for( Idx valModality = 0; valModality < __value->domainSize(); ++valModality )
                    __GStat += __GVal( attrModality, valModality );

        }


        // ==========================================================================
        // Default destructor
        // ==========================================================================
        template <typename GUM_SCALAR>
        VarInfo<GUM_SCALAR>::~VarInfo(){

            for(Idx modality = 0; modality < __attr->domainSize(); modality++){
                delete __modality2Observations[modality];
                delete __contingencyTable[modality];
            }
            GUM_DESTRUCTOR(VarInfo);
        }



    // ##########################################################################
    // Constructor & destructor.
    // ##########################################################################

        // ==========================================================================
        // Default constructor
        // ==========================================================================

        template <typename GUM_SCALAR>
        void VarInfo<GUM_SCALAR>::addObservation( const Observation<GUM_SCALAR>* newObs){

            __modality2Observations[newObs->modality(__attr)]->insert( newObs );

            __GStat -= __GVal(newObs->modality(__attr),  newObs->modality(__value));

            __nbObservation++;
            __attrMarginalTable[newObs->modality(__attr)]++;
            __valueMarginalTable[newObs->modality(__attr)]++;
            (*__contingencyTable[newObs->modality(__attr)])[newObs->modality(__value)]++;

            __GStat += __GVal(newObs->modality(__attr),  newObs->modality(__value));

        }


        template <typename GUM_SCALAR>
        double VarInfo<GUM_SCALAR>::__GVal( Idx attrModa, Idx valueModa ){
            if( (*__contingencyTable[attrModa])[valueModa] < 5 || __nbObservation < 25 )
                return 0;

            double ceil = (double)(*__contingencyTable[attrModa])[valueModa];
            double expected = (double)__attrMarginalTable[attrModa]*(double)__valueMarginalTable[valueModa]/(double)__nbObservation;

            return 2*ceil*log(ceil/expected);
        }
} // End of namespace gum
