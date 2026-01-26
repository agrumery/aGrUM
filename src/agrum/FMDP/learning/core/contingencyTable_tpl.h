/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


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
