#ifndef TESTSUITE_UTILS_H
#  define TESTSUITE_UTILS_H

#  include <fstream>
#  include <iostream>

#  define xstrfy(s) strfy(s)
#  define strfy(x)  #x

#  define GET_RESSOURCES_PATH(relpath) xstrfy(GUM_SRC_PATH) "/testunits/ressources/" relpath
#  define XGET_RESSOURCES_PATH(x)      xstrfy(GUM_SRC_PATH) "/testunits/ressources/" xstrfy(x)

#  define FASTVARDBL gum::fastVariable< double >

namespace gum_tests {
  const std::string waiter[]{"[*  ]", "[ * ]", "[  *]", "[ * ]"};
  const std::string backst = "\b\b\b\b\b";

  void begin_test_waiting() {
    std::cout << waiter[0];
    std::flush(std::cout);
  }

  void test_waiting(int s = -1) {
    static int ss = 0;

    if (s == -1) s = ss++;
    std::cout << backst << waiter[s % 4];
    std::flush(std::cout);
  }

  void end_test_waiting() {
    std::cout << backst;
    std::flush(std::cout);
  }

  void str2file(const std::string& filename, const std::string& message) {
    std::ofstream outFile;
    outFile.open(filename);
    outFile << message << std::endl;
    outFile.close();
  }
}   // namespace gum_tests
#endif   // TESTSUITE_UTILS_H
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
