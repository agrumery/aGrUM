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
 * @author Jean-Christophe MAGNAN
 */
// =========================================================================
#include <cmath>
// =========================================================================
#include <agrum/FMDP/learning/datastructure/varInfo.h>
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
    template < typename GUM_SCALAR, template <class> class TestPolicy >
    VarInfo< GUM_SCALAR, TestPolicy>::VarInfo(const DiscreteVariable* attribute) : TestPolicy<GUM_SCALAR>( ), __attr(attribute) {

      GUM_CONSTRUCTOR(VarInfo);

      // Initialisation
      for(Idx modality = 0; modality < __attr->domainSize(); ++modality ){
        __modality2Observations.insert(modality, new Set<const Observation*>);
      }
    }


    // ==========================================================================
    // Constructor with a list of Observation to iniialize
    // ==========================================================================
    template < typename GUM_SCALAR, template <class> class TestPolicy >
    VarInfo< GUM_SCALAR, TestPolicy >::VarInfo( const DiscreteVariable* attribute,
                                   const Set<const Observation*>* obsSet) : TestPolicy<GUM_SCALAR>( ), __attr(attribute) {

      GUM_CONSTRUCTOR(VarInfo);

      // Initialisation nécessaire avant la prise en compte des observations
      for(Idx modality = 0; modality < __attr->domainSize(); modality++){
        __modality2Observations.insert(modality, new Set<const Observation*>);
      }


      // Prise en compte des observations
      for(SetIteratorSafe<const Observation*> obsIter = obsSet->cbeginSafe(); obsIter != obsSet->cendSafe(); ++obsIter){
        this->addObservation( *obsIter );
      }
    }


    // ==========================================================================
    // Default destructor
    // ==========================================================================
    template < typename GUM_SCALAR, template <class> class TestPolicy >
    VarInfo< GUM_SCALAR, TestPolicy >::~VarInfo(){

      for(Idx modality = 0; modality < __attr->domainSize(); modality++){
        delete __modality2Observations[modality];
      }
      GUM_DESTRUCTOR(VarInfo);
    }



  // ##########################################################################
  //
  // ##########################################################################

    // ==========================================================================
    //
    // ==========================================================================
    template < typename GUM_SCALAR, template <class> class TestPolicy >
    void VarInfo< GUM_SCALAR, TestPolicy >::addObservation( const Observation* newObs, Idx moda, GUM_SCALAR value ){

      if( !__modality2Observations.exists(moda) )
        __modality2Observations.insert(moda, new Set<const Observation*>());
      __modality2Observations[moda]->insert( newObs );

      this->addObservation( moda, value );
    }

} // End of namespace gum
