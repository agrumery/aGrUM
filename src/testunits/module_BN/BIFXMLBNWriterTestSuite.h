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
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIFXML/BIFXMLBNWriter.h>

#include <sys/stat.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  BIFXMLBNWriter
#define GUM_CURRENT_MODULE BN

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {
  struct BIFXMLBNWriterTestSuite {
    public:
    static void testConstuctor() {
      gum::BIFXMLBNWriter< double >* writer = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(writer = new gum::BIFXMLBNWriter< double >());
      delete writer;
    }   // namespace gum_tests

    static void testWriter_ostream() {
      gum::BIFXMLBNWriter< double > writer;
      const auto                    bn = gum::BayesNet< double >::fastPrototype(
          "A[1,5]->B{a|b|c}->C{1|2|30|400}->D{1.|2|2.5|3|13.5}->E[1:5:10]");
      // Uncomment this to check the ouput
      // GUM_CHECK_ASSERT_THROWS_NOTHING(writer.write(std::cerr,bn));
    }

    static void testWriter_file() {
      const auto bn = gum::BayesNet< double >::fastPrototype(
          "A[1,5]->B{a|b|c}->C{1|2|30|400}->D{1.|2|2.5|3|13.5}->E[1:5:10]");
      const std::string file = GET_RESSOURCES_PATH("bifxml/BNBIFXMLWriter_file1.bifxml");
      gum::BIFXMLBNWriter< double > writer;
      GUM_CHECK_ASSERT_THROWS_NOTHING(writer.write(file, bn));


      gum::BayesNet< double >       reload;
      gum::BIFXMLBNReader< double > reader(&reload, file);
      GUM_CHECK_ASSERT_THROWS_NOTHING(reader.proceed());

      for (auto v: reload.nodes()) {
        CHECK((reload.variable(v).toFast()) == (bn.variable(reload.variable(v).name()).toFast()));
        GUM_CHECK_TENSOR_ALMOST_EQUALS(reload.cpt(v), bn.cpt(reload.variable(v).name()));
      }
    }
  };

  GUM_TEST_ACTIF(Constuctor)
  GUM_TEST_ACTIF(Writer_ostream)
  GUM_TEST_ACTIF(Writer_file)
}   // namespace gum_tests
