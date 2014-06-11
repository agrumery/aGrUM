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
#include <agrum/FMDP/learning/varInfo.h>
// =========================================================================

namespace gum {

    // ##########################################################################
    // Constructor & destructor.
    // ##########################################################################

        // ==========================================================================
        // Default constructor
        // ==========================================================================
        template <typename GUM_SCALAR>
        VarInfo::VarInfo(const DiscreteVariable* attribute, const DiscreteVariable* value) : __attr(attribute),
        __value(value){

            GUM_CONSTRUCTOR(VarInfo);

            for(Idx modality = 0; modality < __attr->domainSize(); modality++){
                __modality2Observations.insert(modality, new Set<const Observation<GUM_SCALAR>*>)
            }

        }


        // ==========================================================================
        // Default destructor
        // ==========================================================================
        template <typename GUM_SCALAR>
        VarInfo::~VarInfo(){

            for(Idx modality = 0; modality < __attr->domainSize(); modality++){
                delete __modality2Observations[modality];
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
        void VarInfo::addObservation( const Observation<GUM_SCALAR>* newObs){

            __modality2Observations[newObs->modality(__attr)]

        }
} // End of namespace gum
