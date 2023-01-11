/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
    if (_attrAMarginalTable_.exists(valueA)) _attrAMarginalTable_[valueA]++;
    else _attrAMarginalTable_.insert(valueA, 1);

    if (_attrBMarginalTable_.exists(valueB)) _attrBMarginalTable_[valueB]++;
    else _attrBMarginalTable_.insert(valueB, 1);

    std::pair< GUM_SCALAR_A, GUM_SCALAR_B > cell(valueA, valueB);
    if (_jointTable_.exists(cell)) _jointTable_[cell]++;
    else _jointTable_.insert(cell, 1);
  }

  template < typename GUM_SCALAR_A, typename GUM_SCALAR_B >
  ContingencyTable< GUM_SCALAR_A, GUM_SCALAR_B >&
     ContingencyTable< GUM_SCALAR_A, GUM_SCALAR_B >::operator+=(
        const ContingencyTable< GUM_SCALAR_A, GUM_SCALAR_B >& src) {
    // Ajout dans marginal A et table joint des valeurs pour src
    for (auto aTer = src.attrABeginSafe(); aTer != src.attrAEndSafe(); ++aTer) {
      if (_attrAMarginalTable_.exists(aTer.key())) _attrAMarginalTable_[aTer.key()] += aTer.val();
      else _attrAMarginalTable_.insert(aTer.key(), aTer.val());

      for (auto bTer = src.attrBBeginSafe(); bTer != src.attrBEndSafe(); ++bTer) {
        std::pair< GUM_SCALAR_A, GUM_SCALAR_B > cell(aTer.key(), bTer.key());
        if (_jointTable_.exists(cell)) _jointTable_[cell] += src.joint(aTer.key(), bTer.key());
        else _jointTable_.insert(cell, src.joint(aTer.key(), bTer.key()));
      }
    }

    // Ajout dans marginal B des valeurs de src
    for (auto bTer = src.attrBBeginSafe(); bTer != src.attrBEndSafe(); ++bTer) {
      if (_attrBMarginalTable_.exists(bTer.key())) _attrBMarginalTable_[bTer.key()] += bTer.val();
      else _attrBMarginalTable_.insert(bTer.key(), bTer.val());
    }
    return *this;
  }

}   // End of namespace gum
