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
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/graphs/algorithms/binaryJoinTreeConverterDefault.h>

namespace gum_tests {

  class [[maybe_unused]] BinaryJoinTreeConverterTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(1) {
      gum::CliqueGraph            graph;
      std::vector< gum::NodeSet > cliques(11);

      cliques[0] << 1 << 2;
      cliques[1] << 3 << 4 << 5;
      cliques[2] << 6 << 7 << 8;
      cliques[3] << 9 << 10;
      cliques[4] << 8 << 11 << 12 << 3;
      cliques[5] << 2 << 3 << 7 << 8 << 10 << 11 << 12;
      cliques[6] << 10 << 15;
      cliques[7] << 13 << 17;
      cliques[8] << 2 << 7 << 10 << 11 << 13;
      cliques[9] << 7 << 14;
      cliques[10] << 14 << 16;

      for (unsigned int i = 0; i <= 10; ++i) {
        graph.addNodeWithId(i, cliques[i]);
      }

      graph.addEdge(0, 5);
      graph.addEdge(1, 5);
      graph.addEdge(2, 5);
      graph.addEdge(3, 5);
      graph.addEdge(4, 5);
      graph.addEdge(5, 8);
      graph.addEdge(6, 8);
      graph.addEdge(7, 8);
      graph.addEdge(8, 9);
      graph.addEdge(9, 10);

      gum::NodeProperty< gum::Size > domain_sizes;

      for (gum::Idx i = 1; i <= 17; ++i) {
        domain_sizes.insert(i, (gum::Size)3);
      }

      gum::BinaryJoinTreeConverterDefault converter;
      gum::NodeSet                        roots;
      roots << 9;

      try {
        gum::CliqueGraph binTree = converter.convert(graph, domain_sizes, roots);
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }
  };

} /* namespace gum_tests */
