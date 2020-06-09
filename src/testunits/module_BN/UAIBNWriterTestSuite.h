
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 * (@AMU) info_at_agrum_dot_org
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


#include <sys/stat.h>

#include <iostream>
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/UAI/UAIBNWriter.h>
#include <agrum/BN/io/UAI/UAIBNReader.h>
#include <agrum/tools/variables/labelizedVariable.h>

// The graph used for the tests:
//          0   1_          0 -> 2
//         / \ / /          0 -> 3
//        2   3 /           2 -> 4
//         \ / /            3 -> 4
//          4_/             1 -> 3
//                          1 -> 4

namespace gum_tests {

  class UAIBNWriterTestSuite: public CxxTest::TestSuite {
    public:
    gum::BayesNet< double >* bn;
    gum::NodeId              i1, i2, i3, i4, i5;

    void setUp() {
      bn = new gum::BayesNet< double >();

      gum::LabelizedVariable n1("0", "", 2), n2("1", "", 2), n3("2", "", 2);
      gum::LabelizedVariable n4("3", "", 2), n5("4", "", 3);

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

    void testConstructor() {
      gum::UAIBNWriter< double >* writer = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(writer = new gum::UAIBNWriter< double >());
      delete writer;
    }

    void testWriter_ostream() {
      gum::UAIBNWriter< double > writer;
      std::string file = GET_RESSOURCES_PATH("outputs/uaibn_generated.uai");
      // Uncomment this to check the ouput
      writer.write(file, *bn);

      gum::BayesNet< double >    net;
      gum::UAIBNReader< double > reader(&net, file);

      gum::Size nbErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbErr = reader.proceed());
      TS_ASSERT_EQUALS(nbErr, gum::Size(0));

      TS_ASSERT(*bn == net);
    }

    private:
    // Builds a BN to test the inference
    void fill(gum::BayesNet< double >& bn) {
      bn.cpt(i1).fillWith({0.2, 0.8});
      bn.cpt(i2).fillWith({0.3, 0.7});
      bn.cpt(i3).fillWith({0.1, 0.9, 0.9, 0.1});
      bn.cpt(i4).fillWith({0.4, 0.6, 0.5, 0.5, 0.5, 0.5, 1.0, 0.0});
      bn.cpt(i5).fillWith({0.3, 0.6, 0.1, 0.5, 0.5, 0.0, 0.5, 0.5,
                           0.0, 1.0, 0.0, 0.0, 0.4, 0.6, 0.0, 0.5,
                           0.5, 0.0, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0});
    }
  };
}   // namespace gum_tests
