/**
 *
 *   Copyright (c) 2005-2024 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
#include <gumtest/utils.h>

#include <agrum/tools/variables/labelizedVariable.h>

#include <agrum/MRF/io/UAI/UAIMRFReader.h>
#include <agrum/MRF/io/UAI/UAIMRFWriter.h>
#include <agrum/MRF/MarkovRandomField.h>

#include <sys/stat.h>

// The graph used for the tests:
//          0   1_
//         / \ / /
//        2   3 /
//         \ / /
//          4_/
//

namespace gum_tests {

  class [[maybe_unused]] UAIMRFWriterTestSuite: public CxxTest::TestSuite {
    public:
    gum::MarkovRandomField< double >* mn;
    gum::NodeId                       i1, i2, i3, i4, i5;

    void setUp() {
      mn = new gum::MarkovRandomField< double >();

      gum::LabelizedVariable n1("0", "", 2), n2("1", "", 2), n3("2", "", 2);
      gum::LabelizedVariable n4("3", "", 2), n5("4", "", 3);

      i1 = mn->add(n1);
      i2 = mn->add(n2);
      i3 = mn->add(n3);
      i4 = mn->add(n4);
      i5 = mn->add(n5);

      mn->addFactor({i1, i3});
      mn->addFactor({i1, i4});
      mn->addFactor({i3, i5});
      mn->addFactor({i2, i4, i5});

      __fill(*mn);
    }

    void tearDown() { delete mn; }

    GUM_ACTIVE_TEST(Constructor) {
      gum::UAIMRFWriter< double >* writer = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(writer = new gum::UAIMRFWriter< double >())
      delete writer;
    }

    GUM_ACTIVE_TEST(Writer_ostream) {
      gum::UAIMRFWriter< double > writer;
      std::string                 file = GET_RESSOURCES_PATH("outputs/uaimn_generated.uai");
      writer.write(file, *mn);

      gum::MarkovRandomField< double > net;
      gum::UAIMRFReader< double >      reader(&net, file);

      gum::Size nbErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbErr = reader.proceed())
      TS_ASSERT_EQUALS(nbErr, (gum::Size)0)
      TS_ASSERT_EQUALS(*mn, net)
    }

    private:
    // Builds a MRF to test the inference
    void __fill(gum::MarkovRandomField< double >& mn) {
      try {
        mn.factor({"0", "2"}).fillWith({1, 2, 3, 4});
        mn.factor({"0", "3"}).fillWith({11, 12, 13, 14});
        mn.factor({"2", "4"}).fillWith({21, 22, 23, 24, 25, 26});
        mn.factor({"4", "1", "3"}).fillWith({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12});
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }
  };
}   // namespace gum_tests
