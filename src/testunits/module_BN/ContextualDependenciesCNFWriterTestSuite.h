
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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


#include <iostream>
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/cnf/ContextualDependenciesCNFWriter.h>
#include <agrum/tools/core/approximations/exactPolicy.h>
#include <agrum/tools/core/approximations/linearApproximationPolicy.h>
#include <agrum/tools/variables/labelizedVariable.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class ContextualDependenciesCNFWriterTestSuite: public CxxTest::TestSuite {
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

    void testConstuctor() {
      gum::ContextualDependenciesCNFWriter< double >* writer = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(
         writer = new gum::ContextualDependenciesCNFWriter< double >());
      delete writer;
    }

    void testConstuctor_With_Aproximation() {
      typedef gum::ContextualDependenciesCNFWriter< double, gum::ExactPolicy >
              typCNF;
      typCNF* writer = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(writer = new typCNF());
      //   writer->setEpsilon( 0.2 );
      //         writer->setLowLimit ( 0 );
      //         writer->setHighLimit ( 0.5 );

      delete writer;
    }

    void testWriter_ostream() {
      gum::ContextualDependenciesCNFWriter< double > writer;
      std::stringstream                              sstream;
      TS_GUM_ASSERT_THROWS_NOTHING(writer.write(sstream, *bn));
    }

    void testWriter_ostream_With_Approximation() {
      gum::ContextualDependenciesCNFWriter< double,
                                            gum::LinearApproximationPolicy >
         writer;
      writer.setEpsilon(0.2);
      writer.setLowLimit(0);
      writer.setHighLimit(1);

      // Uncomment this to check the ouput
      // TS_GUM_ASSERT_THROWS_NOTHING(writer.write(std::cerr, *bn));
    }

    void testWriter_string() {
      gum::ContextualDependenciesCNFWriter< double > writer;
      std::string                                    file = GET_RESSOURCES_PATH(
         "outputs/ContextualDependenciesCNFWriter_TestFile.cnf");
      TS_GUM_ASSERT_THROWS_NOTHING(writer.write(file, *bn));
    }

    void testWriter_string_With_Approximation() {
      gum::ContextualDependenciesCNFWriter< double,
                                            gum::LinearApproximationPolicy >
         writer;

      writer.setEpsilon(0.2);
      writer.setLowLimit(0);
      writer.setHighLimit(1);
      std::string file = GET_RESSOURCES_PATH(
         "outputs/ContextualDependenciesCNFWriter_TestFile_Approximation.cnf");

      TS_GUM_ASSERT_THROWS_NOTHING(writer.write(file, *bn));

      file = GET_RESSOURCES_PATH(
         "outputs/ContextualDependenciesCNFWriter_RO_TestFile_Approximation.cnf");

      try {
        writer.write(file, *bn);
        // TS_ASSERT(false);
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
               1.0, 0.0} );   // clang-format on
      bn.cpt(i5).fillWith(   // clang-format off
              {1.0,0.0,
               0.0,1.0,
               0.0,1.0,
               0.0,1.0,
               0.0,1.0,
               0.0,1.0,
               0.0,1.0,
               0.0,1.0} );                                      // clang-format on
    }
  };
}   // namespace gum_tests
