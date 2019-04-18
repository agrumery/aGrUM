
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Template implementations for the ContingencyTable class.
 *
 * @author Jean-Christophe MAGNAN
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
  template < typename GUM_SCALAR_A, typename GUM_SCALAR_B >
  ContingencyTable< GUM_SCALAR_A, GUM_SCALAR_B >::ContingencyTable() {
    GUM_CONSTRUCTOR(ContingencyTable);
  }


  // ==========================================================================
  // Default destructor
  // ==========================================================================
  template < typename GUM_SCALAR_A, typename GUM_SCALAR_B >
  ContingencyTable< GUM_SCALAR_A, GUM_SCALAR_B >::~ContingencyTable() {
    GUM_DESTRUCTOR(ContingencyTable);
  }


  // ##########################################################################
  //
  // ##########################################################################

  // ==========================================================================
  //
  // ==========================================================================
  template < typename GUM_SCALAR_A, typename GUM_SCALAR_B >
  void ContingencyTable< GUM_SCALAR_A, GUM_SCALAR_B >::add(GUM_SCALAR_A valueA,
                                                           GUM_SCALAR_B valueB) {
    // Updating
    if (__attrAMarginalTable.exists(valueA))
      __attrAMarginalTable[valueA]++;
    else
      __attrAMarginalTable.insert(valueA, 1);

    if (__attrBMarginalTable.exists(valueB))
      __attrBMarginalTable[valueB]++;
    else
      __attrBMarginalTable.insert(valueB, 1);

    std::pair< GUM_SCALAR_A, GUM_SCALAR_B > cell(valueA, valueB);
    if (__jointTable.exists(cell))
      __jointTable[cell]++;
    else
      __jointTable.insert(cell, 1);
  }

  template < typename GUM_SCALAR_A, typename GUM_SCALAR_B >
  ContingencyTable< GUM_SCALAR_A, GUM_SCALAR_B >&
        ContingencyTable< GUM_SCALAR_A, GUM_SCALAR_B >::
        operator+=(const ContingencyTable< GUM_SCALAR_A, GUM_SCALAR_B >& src) {
    // Ajout dans marginal A et table joint des valeurs pour src
    for (auto aTer = src.attrABeginSafe(); aTer != src.attrAEndSafe(); ++aTer) {
      if (__attrAMarginalTable.exists(aTer.key()))
        __attrAMarginalTable[aTer.key()] += aTer.val();
      else
        __attrAMarginalTable.insert(aTer.key(), aTer.val());

      for (auto bTer = src.attrBBeginSafe(); bTer != src.attrBEndSafe(); ++bTer) {
        std::pair< GUM_SCALAR_A, GUM_SCALAR_B > cell(aTer.key(), bTer.key());
        if (__jointTable.exists(cell))
          __jointTable[cell] += src.joint(aTer.key(), bTer.key());
        else
          __jointTable.insert(cell, src.joint(aTer.key(), bTer.key()));
      }
    }

    // Ajout dans marginal B des valeurs de src
    for (auto bTer = src.attrBBeginSafe(); bTer != src.attrBEndSafe(); ++bTer) {
      if (__attrBMarginalTable.exists(bTer.key()))
        __attrBMarginalTable[bTer.key()] += bTer.val();
      else
        __attrBMarginalTable.insert(bTer.key(), bTer.val());
    }
    return *this;
  }

}   // End of namespace gum
