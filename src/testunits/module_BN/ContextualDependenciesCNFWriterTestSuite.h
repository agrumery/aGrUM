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

#include <agrum/base/core/approximations/exactPolicy.h>
#include <agrum/base/core/approximations/linearApproximationPolicy.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/cnf/ContextualDependenciesCNFWriter.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class [[maybe_unused]] ContextualDependenciesCNFWriterTestSuite: public CxxTest::TestSuite {
    public:
    gum::BayesNet< double >* bn;
    gum::NodeId              i1, i2, i3, i4, i5;

    void setUp() {
      bn = new gum::BayesNet< double >();

      gum::LabelizedVariable n1("n1", "", 2), n2("n2", "", 2), n3("n3", "", 2);
      gum::LabelizedVariable n4("n4", "", 2), n5("n5", "", 2);

      i1 = bn->add(n1);
      i2 = bn->add(n2);
      i3 = bn->add(n3);
      i4 = bn->add(n4);
      i5 = bn->add(n5);

      bn->addArc(i1, i3);
      bn->addArc(i1, i4);
      bn->addArc(i3, i5);
      bn->addArc(i4, i5);
      bn->addArc(i2, i4);
      bn->addArc(i2, i5);

      fill(*bn);
    }

    void tearDown() { delete bn; }

    GUM_ACTIVE_TEST(Constuctor) {
      gum::ContextualDependenciesCNFWriter< double >* writer = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(writer = new gum::ContextualDependenciesCNFWriter< double >())
      delete writer;
    }

    GUM_ACTIVE_TEST(Constuctor_With_Aproximation) {
      using typCNF   = gum::ContextualDependenciesCNFWriter< double, gum::ExactPolicy >;
      typCNF* writer = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(writer = new typCNF())
      //   writer->setEpsilon( 0.2 );
      //         writer->setLowLimit ( 0 );
      //         writer->setHighLimit ( 0.5 );

      delete writer;
    }

    GUM_ACTIVE_TEST(Writer_ostream) {
      gum::ContextualDependenciesCNFWriter< double > writer;
      std::stringstream                              sstream;
      TS_GUM_ASSERT_THROWS_NOTHING(writer.write(sstream, *bn))
    }

    GUM_ACTIVE_TEST(Writer_ostream_With_Approximation) {
      gum::ContextualDependenciesCNFWriter< double, gum::LinearApproximationPolicy > writer;
      writer.setEpsilon(0.2);
      writer.setLowLimit(0);
      writer.setHighLimit(1);

      // Uncomment this to check the ouput
      // TS_GUM_ASSERT_THROWS_NOTHING(writer.write(std::cerr, *bn))
    }

    GUM_ACTIVE_TEST(Writer_string) {
      gum::ContextualDependenciesCNFWriter< double > writer;
      std::string                                    file
          = GET_RESSOURCES_PATH("outputs/ContextualDependenciesCNFWriter_TestFile.cnf");
      TS_GUM_ASSERT_THROWS_NOTHING(writer.write(file, *bn))
    }

    GUM_ACTIVE_TEST(Writer_string_With_Approximation) {
      gum::ContextualDependenciesCNFWriter< double, gum::LinearApproximationPolicy > writer;

      writer.setEpsilon(0.2);
      writer.setLowLimit(0);
      writer.setHighLimit(1);
      std::string file = GET_RESSOURCES_PATH(
          "outputs/ContextualDependenciesCNFWriter_TestFile_Approximation.cnf");

      TS_GUM_ASSERT_THROWS_NOTHING(writer.write(file, *bn))

      file = GET_RESSOURCES_PATH(
          "outputs/ContextualDependenciesCNFWriter_RO_TestFile_Approximation.cnf");

      try {
        writer.write(file, *bn);
        // TS_ASSERT(false)
      } catch (gum::IOError&) { TS_ASSERT(true); }
    }

    private:
    // Builds a BN to test the inference
    void fill(gum::BayesNet< double >& bn) {
      bn.cpt(i1).fillWith({0.5, 0.5});
      bn.cpt(i2).fillWith({0.3, 0.7});
      bn.cpt(i3).fillWith({0.1, 0.9, 0.9, 0.1});
      bn.cpt(i4).fillWith(   // clang-format off
              {0.4, 0.6,
               0.5, 0.5,
               0.5, 0.5,
               1.0, 0.0} );                                   // clang-format on
      bn.cpt(i5).fillWith(   // clang-format off
              {1.0,0.0,
               0.0,1.0,
               0.0,1.0,
               0.0,1.0,
               0.0,1.0,
               0.0,1.0,
               0.0,1.0,
               0.0,1.0} );   // clang-format on
    }
  };
}   // namespace gum_tests
