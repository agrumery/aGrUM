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
 * @brief Template implementations for the ContingencyTable class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */
// =========================================================================
#include <cmath>
// =========================================================================
#include <agrum/FMDP/learning/core/contingencyTable.h>
// =========================================================================


// constants used by Gary Perlman for his code for computing chi2 critical values

namespace gum {

  // ##########################################################################
  // Constructor & destructor.
  // ##########################################################################

    // ==========================================================================
    // Default constructor
    // ==========================================================================
    ContingencyTable::ContingencyTable(const DiscreteVariable* attribute,
                     const DiscreteVariable* value) : __attrDomainSize(attribute->domainSize()),
                                                      __valueDomainSize(value->domainSize()){

      GUM_CONSTRUCTOR(ContingencyTable);

      // Initialisation
      for(Idx attrMod = 0; attrMod < __attrDomainSize; ++attrMod ){

        __contingencyTable.insert( attrMod, new HashTable<Idx, Idx>());
        for( Idx valMod = 0; valMod < __valueDomainSize; ++valMod )
          __contingencyTable[attrMod]->insert(valMod, 0);

        __attrMarginalTable.insert( attrMod, 0);
      }

      for( Idx valMod = 0; valMod < __valueDomainSize; ++valMod )
        __valueMarginalTable.insert(valMod, 0);

    }


    // ==========================================================================
    // Default destructor
    // ==========================================================================
    ContingencyTable::~ContingencyTable(){
      GUM_DESTRUCTOR(ContingencyTable);

      for(Idx modality = 0; modality < __attrDomainSize; modality++){
        delete __contingencyTable[modality];
      }
    }



  // ##########################################################################
  //
  // ##########################################################################

    // ==========================================================================
    //
    // ==========================================================================
    void ContingencyTable::add( Idx iattr, Idx ivalue){

      // Updating
      __attrMarginalTable[iattr]++;
      __valueMarginalTable[ivalue]++;
      (*__contingencyTable[iattr])[ivalue]++;
    }

} // End of namespace gum
