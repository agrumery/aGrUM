/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/net/netReader.h>
#include <agrum/BN/io/net/netWriter.h>
#include <agrum/tools/variables/labelizedVariable.h>

#include <gumtest/testsuite_utils.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class NetWriterTestSuite: public CxxTest::TestSuite {
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
      gum::NetWriter< double >* writer = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(writer = new gum::NetWriter< double >());
      delete writer;
    }

    void testWriter_ostream() {
      gum::NetWriter< double > writer;
      // Uncomment this to check the ouput
      // TS_GUM_ASSERT_THROWS_NOTHING(writer.write(std::cerr, *bn));
    }

    void testWriter_string() {
      gum::NetWriter< double > writer;
      std::string file = GET_RESSOURCES_PATH("outputs/NetWriter_TestFile.net");
      TS_GUM_ASSERT_THROWS_NOTHING(writer.write(file, *bn));
    }

    void test_isreadable() {
      std::string file = GET_RESSOURCES_PATH("net/NetWriter_RO_TestFile.net");
      gum::BayesNet< double >* net = new gum::BayesNet< double >();

      gum::NetReader< double > reader(net, file);
      TS_GUM_ASSERT_THROWS_NOTHING(reader.trace(false));

      gum::Size nbrErr = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      reader.showElegantErrors();

      TS_ASSERT(nbrErr == 0);
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      // 0 warnings : no properties
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)

      TS_ASSERT(net != nullptr);

      if (net != nullptr) {
        TS_ASSERT(!net->empty());

        delete net;
      }
    }

    private:
    // Builds a BN to test the inference
    void fill(gum::BayesNet< double >& bn) {
      const gum::Potential< double >& p1 = bn.cpt(i1);
      {
        // FILLING PARAMS
        const double                t[2] = {0.2, 0.8};
        int                         n    = 2;
        const std::vector< double > v(t, t + n);
        p1.fillWith(v);
      }

      const gum::Potential< double >& p2 = bn.cpt(i2);
      {
        // FILLING PARAMS
        const double                t[2] = {0.3, 0.7};
        int                         n    = 2;
        const std::vector< double > v(t, t + n);
        p2.fillWith(v);
      }

      const gum::Potential< double >& p3 = bn.cpt(i3);
      {
        // FILLING PARAMS
        const double                t[4] = {0.1, 0.9, 0.9, 0.1};
        int                         n    = 4;
        const std::vector< double > v(t, t + n);
        p3.fillWith(v);
      }

      const gum::Potential< double >& p4 = bn.cpt(i4);
      {
        // FILLING PARAMS
        const double t[8] = {0.4, 0.6, 0.5, 0.5, 0.5, 0.5, 1.0, 0.0};
        int          n    = 8;
        const std::vector< double > v(t, t + n);
        p4.fillWith(v);
      }

      const gum::Potential< double >& p5 = bn.cpt(i5);
      {
        // FILLING PARAMS
        const double t[16] = {1.0,
                              0.0,
                              0.0,
                              1.0,
                              0.0,
                              1.0,
                              0.0,
                              1.0,
                              0.0,
                              1.0,
                              0.0,
                              1.0,
                              0.0,
                              1.0,
                              0.0,
                              1.0};

        int                         n = 16;
        const std::vector< double > v(t, t + n);
        p5.fillWith(v);
      }
    }
  };
}   // namespace gum_tests
