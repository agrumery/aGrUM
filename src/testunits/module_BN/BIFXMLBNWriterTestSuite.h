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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#include <iostream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIFXML/BIFXMLBNWriter.h>

#include <sys/stat.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {
  class [[maybe_unused]] BIFXMLBNWriterTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(Constuctor) {
      gum::BIFXMLBNWriter< double >* writer = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(writer = new gum::BIFXMLBNWriter< double >())
      delete writer;
    }

    GUM_ACTIVE_TEST(Writer_ostream) {
      gum::BIFXMLBNWriter< double > writer;
      const auto                    bn = gum::BayesNet< double >::fastPrototype(
          "A[1,5]->B{a|b|c}->C{1|2|30|400}->D{1.|2|2.5|3|13.5}->E[1:5:10]");
      // Uncomment this to check the ouput
      // TS_GUM_ASSERT_THROWS_NOTHING(writer.write(std::cerr,bn))
    }

    GUM_ACTIVE_TEST(Writer_file) {
      const auto bn = gum::BayesNet< double >::fastPrototype(
          "A[1,5]->B{a|b|c}->C{1|2|30|400}->D{1.|2|2.5|3|13.5}->E[1:5:10]");
      const std::string file = GET_RESSOURCES_PATH("bifxml/BNBIFXMLWriter_file1.bifxml");
      gum::BIFXMLBNWriter< double > writer;
      TS_GUM_ASSERT_THROWS_NOTHING(writer.write(file, bn));


      gum::BayesNet< double >       reload;
      gum::BIFXMLBNReader< double > reader(&reload, file);
      TS_GUM_ASSERT_THROWS_NOTHING(reader.proceed())

      for (auto v: reload.nodes()) {
        TS_ASSERT_EQUALS(reload.variable(v).toFast(),
                         bn.variable(reload.variable(v).name()).toFast())
        TS_GUM_TENSOR_ALMOST_EQUALS(reload.cpt(v), bn.cpt(reload.variable(v).name()))
      }
    }
  };
}   // namespace gum_tests
