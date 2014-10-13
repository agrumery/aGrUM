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
#include <agrum/FMDP/learning/core/contingencyTable.h>
// =========================================================================



namespace gum {

  // ##########################################################################
  // Constructor & destructor.
  // ##########################################################################

    // ==========================================================================
    // Default constructor
    // ==========================================================================
    template< typename GUM_SCALAR_A, typename GUM_SCALAR_B >
    ContingencyTable<GUM_SCALAR_A, GUM_SCALAR_B>::ContingencyTable() {
      GUM_CONSTRUCTOR(ContingencyTable);
    }


    // ==========================================================================
    // Default destructor
    // ==========================================================================
    template< typename GUM_SCALAR_A, typename GUM_SCALAR_B >
    ContingencyTable<GUM_SCALAR_A, GUM_SCALAR_B>::~ContingencyTable(){
      GUM_DESTRUCTOR(ContingencyTable);
    }



  // ##########################################################################
  //
  // ##########################################################################

    // ==========================================================================
    //
    // ==========================================================================
    template< typename GUM_SCALAR_A, typename GUM_SCALAR_B >
    void ContingencyTable<GUM_SCALAR_A, GUM_SCALAR_B>::add( GUM_SCALAR_A attr, GUM_SCALAR_B value){

      // Updating
      if(__attrMarginalTable.exists(attr))
        __attrMarginalTable[attr]++;
      else
        __attrMarginalTable.insert(attr,1);

      if(__valueMarginalTable.exists(value))
        __valueMarginalTable[value]++;
      else
        __valueMarginalTable.insert(value,1);

      std::pair<GUM_SCALAR_A, GUM_SCALAR_B> cell(attr, value);
      if(__contingencyTable.exists(cell))
        __contingencyTable[cell]++;
      else
        __contingencyTable.insert(cell, 1);
    }

} // End of namespace gum
