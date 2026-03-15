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
 *   useful, but WITHOUT ANY WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,      *
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


#include <iostream>
#include <string>
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/graphicalModels/algorithms/informationTheory.h>
#include <agrum/MRF/inference/ShaferShenoyMRFInference.h>
#include <agrum/MRF/MarkovRandomField.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  MRFInformationTheory
#define GUM_CURRENT_MODULE MRF

namespace gum_tests {
  struct MRFInformationTheoryTestSuite {
    public:
    static void testMRFCheckConsistency() {
      const auto mrf = gum::MarkovRandomField< double >::fastPrototype("A--B--C;A--B");

      gum::ShaferShenoyMRFInference ie(&mrf);

      auto it = gum::InformationTheory(ie, {"A", "C"}, {"B"});
      check_this_information_theoryXY(it);
    }

    static void testMRFCheckConsistency3points() {
      const auto mrf = gum::MarkovRandomField< double >::fastPrototype("A--B--C;A--D--E--B");

      gum::ShaferShenoyMRFInference ie(&mrf);

      auto it = gum::InformationTheory(ie, {"A", "E"}, {"B"}, {"C", "D"});
      check_this_information_theoryXY(it);
      check_this_information_theoryXYZ(it);
    }

    private:
    template < class IT >
    static void check_this_information_theoryXY(IT& it) {
      // H(X|Y)=H(X,Y)-H(Y)
      CHECK_LT(fabs((it.entropyXgivenY()) - (it.entropyXY() - it.entropyY())),
               GUM_VERY_SMALL_ERROR);

      // H(Y|X)=H(X|Y)-H(X)+H(Y)
      CHECK_LT(fabs((it.entropyYgivenX()) - (it.entropyXgivenY() - it.entropyX() + it.entropyY())),
               GUM_VERY_SMALL_ERROR);

      // I(X,Y)=H(X)-H(X|Y)=H(Y)-H(Y|X)
      CHECK_LT(fabs((it.mutualInformationXY()) - (it.entropyX() - it.entropyXgivenY())),
               GUM_VERY_SMALL_ERROR);
      CHECK_LT(fabs((it.mutualInformationXY()) - (it.entropyY() - it.entropyYgivenX())),
               GUM_VERY_SMALL_ERROR);

      // I(X,Y)=H(X)+H(Y)-H(X,Y)
      CHECK_LT(fabs((it.mutualInformationXY()) - (it.entropyX() + it.entropyY() - it.entropyXY())),
               GUM_VERY_SMALL_ERROR);

      // I(X,Y)=H(X,Y)-H(X|Y)-H(Y|X)
      CHECK_LT(fabs((it.mutualInformationXY())
                    - (it.entropyXY() - it.entropyXgivenY() - it.entropyYgivenX())),
               GUM_VERY_SMALL_ERROR);

      // V(X,Y)=2H(X,Y)-H(X)-H(Y)
      CHECK_LT(fabs((it.variationOfInformationXY())
                    - (2 * it.entropyXY() - it.entropyXgivenY() - it.entropyYgivenX())),
               GUM_VERY_SMALL_ERROR);
    }

    template < class IT >
    static void check_this_information_theoryXYZ(IT& it) {
      // H(X|Y);Z=H(X,Y|Z)-H(Y|Z)
      CHECK_LT(fabs((it.entropyXgivenYZ()) - (it.entropyXYgivenZ() - it.entropyYgivenZ())),
               GUM_VERY_SMALL_ERROR);

      // I(X,Y|Z)=H(X|Z)+H(Y|Z)-H(X,Y|Z)
      CHECK_LT(fabs((it.mutualInformationXYgivenZ())
                    - (it.entropyXgivenZ() + it.entropyYgivenZ() - it.entropyXYgivenZ())),
               GUM_VERY_SMALL_ERROR);
    }
  };

  GUM_TEST_ACTIF(MRFCheckConsistency)
  GUM_TEST_ACTIF(MRFCheckConsistency3points)
}   // namespace gum_tests
