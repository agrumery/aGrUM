/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#include <iostream>
#include <sstream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/external/json/json.hpp>
using json = nlohmann::json;

namespace gum_tests {
  class [[maybe_unused]] GumJsonTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(FirstTest) {
      auto j = json::parse(R"({
  "nodes": [
    "A{yes|maybe|no}",
    "B[3,7]",
    "C[1.3,1.6,1.9,2.2,2.5]"
  ],
  "parents": {
    "A": [],
    "B": [
      "A"
    ],
    "C": []
  },
  "cpt": {
    "A": [
      0.417380844629963,
      0.21085478447297942,
      0.37176437089705744
    ],
    "B": [
      0.009610030586263115,
      0.3721091654607065,
      0.34718174263420487,
      0.03826527618922873,
      0.23283378512959688,
      0.434471695470014,
      0.3375636061969701,
      0.03905128623714965,
      0.07184988937761709,
      0.1170635227182491,
      0.2503618850578262,
      0.07082022137858084,
      0.26516105371233256,
      0.1936664148631508,
      0.2199904249881097
    ],
    "C": [
      0.26461164685135036,
      0.35007036472067804,
      0.22752123368575725,
      0.15779675474221427
    ]
  },
  "properties": {
    "creation": "2025-07-25 17:06:06",
    "lastModification": "2025-07-25 17:06:06",
    "version": "pyAgrum 2.2.0"
  }
})");

      std::cout << j << std::endl;
    }
  };
}
