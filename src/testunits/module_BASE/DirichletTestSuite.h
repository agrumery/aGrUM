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


#include <iostream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/math/Dirichlet.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  Dirichlet
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct DirichletTestSuite {
    public:
    static void testXX() {
      std::vector< float > param{1.0f, 1.0f};
      gum::Dirichlet       dir(param);

      std::vector< float > res(2, 0);
      float                sum = 0;
      for (gum::Idx i = 0; i < 10000; ++i) {
        std::vector< float > sample = dir();
        for (gum::Idx j = 0; j < sample.size(); ++j) {
          res[j] += sample[j];
          sum += sample[j];
        }
      }
      for (gum::Idx j = 0; j < res.size(); ++j) {
        res[j] /= sum;
      }

      CHECK(fabs(res[0] - res[1]) < 0.03);


      param[0] = 10000;
      param[1] = 1;
      sum      = 0;
      res[0] = res[1] = 0;
      for (gum::Idx i = 0; i < 10000; ++i) {
        std::vector< float > sample = dir(param);
        for (gum::Idx j = 0; j < sample.size(); ++j) {
          res[j] += sample[j];
          sum += sample[j];
        }
      }
      for (gum::Idx j = 0; j < res.size(); ++j) {
        res[j] /= sum;
      }
      CHECK(1 - res[0] < 0.01);

      sum    = 0;
      res[0] = res[1] = 0;
      for (gum::Idx i = 0; i < 10000; ++i) {
        std::vector< float > sample = dir(param);
        for (gum::Idx j = 0; j < sample.size(); ++j) {
          res[j] += sample[j];
          sum += sample[j];
        }
      }
      for (gum::Idx j = 0; j < res.size(); ++j) {
        res[j] /= sum;
      }
      CHECK(1 - res[0] < 0.01);

      sum    = 0;
      res[0] = res[1] = 0;
      for (gum::Idx i = 0; i < 10000; ++i) {
        std::vector< float > sample = dir(dir.param());
        for (gum::Idx j = 0; j < sample.size(); ++j) {
          res[j] += sample[j];
          sum += sample[j];
        }
      }
      for (gum::Idx j = 0; j < res.size(); ++j) {
        res[j] /= sum;
      }
      CHECK(fabs(res[0] - res[1]) < 0.03);
    }   // namespace gum_tests
  };

  GUM_TEST_ACTIF(XX)

} /* namespace gum_tests */
