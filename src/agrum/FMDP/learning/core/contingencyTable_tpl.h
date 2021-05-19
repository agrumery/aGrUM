/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
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
    if (attrAMarginalTable__.exists(valueA))
      attrAMarginalTable__[valueA]++;
    else
      attrAMarginalTable__.insert(valueA, 1);

    if (attrBMarginalTable__.exists(valueB))
      attrBMarginalTable__[valueB]++;
    else
      attrBMarginalTable__.insert(valueB, 1);

    std::pair< GUM_SCALAR_A, GUM_SCALAR_B > cell(valueA, valueB);
    if (jointTable__.exists(cell))
      jointTable__[cell]++;
    else
      jointTable__.insert(cell, 1);
  }

  template < typename GUM_SCALAR_A, typename GUM_SCALAR_B >
  ContingencyTable< GUM_SCALAR_A, GUM_SCALAR_B >&
     ContingencyTable< GUM_SCALAR_A, GUM_SCALAR_B >::operator+=(
        const ContingencyTable< GUM_SCALAR_A, GUM_SCALAR_B >& src) {
    // Ajout dans marginal A et table joint des valeurs pour src
    for (auto aTer = src.attrABeginSafe(); aTer != src.attrAEndSafe(); ++aTer) {
      if (attrAMarginalTable__.exists(aTer.key()))
        attrAMarginalTable__[aTer.key()] += aTer.val();
      else
        attrAMarginalTable__.insert(aTer.key(), aTer.val());

      for (auto bTer = src.attrBBeginSafe(); bTer != src.attrBEndSafe(); ++bTer) {
        std::pair< GUM_SCALAR_A, GUM_SCALAR_B > cell(aTer.key(), bTer.key());
        if (jointTable__.exists(cell))
          jointTable__[cell] += src.joint(aTer.key(), bTer.key());
        else
          jointTable__.insert(cell, src.joint(aTer.key(), bTer.key()));
      }
    }

    // Ajout dans marginal B des valeurs de src
    for (auto bTer = src.attrBBeginSafe(); bTer != src.attrBEndSafe(); ++bTer) {
      if (attrBMarginalTable__.exists(bTer.key()))
        attrBMarginalTable__[bTer.key()] += bTer.val();
      else
        attrBMarginalTable__.insert(bTer.key(), bTer.val());
    }
    return *this;
  }

}   // End of namespace gum
