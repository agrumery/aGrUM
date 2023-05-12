/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIF/BIFWriter.h>
#include <agrum/tools/variables/labelizedVariable.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class [[maybe_unused]] BIFWriterTestSuite: public CxxTest::TestSuite {
    private:
    // Builds a BN to test the inference
    void fill(gum::BayesNet< double >& bn) {
      bn.cpt(i1).fillWith({0.2, 0.8});
      bn.cpt(i2).fillWith({0.3, 0.7});
      bn.cpt(i3).fillWith({0.1, 0.9, 0.9, 0.1});
      bn.cpt(i4).fillWith(   // clang-format off
                             {0.4, 0.6,
                              0.5, 0.5,
                              0.5, 0.5,
                              1.0, 0.0} );   // clang-format on
      bn.cpt(i5).fillWith(                             // clang-format off
                             {0.3, 0.6, 0.1,
                              0.5, 0.5, 0.0,
                              0.5, 0.5, 0.0,
                              1.0, 0.0, 0.0,
                              0.4, 0.6, 0.0,
                              0.5, 0.5, 0.0,
                              0.5, 0.5, 0.0,
                              0.0, 0.0, 1.0} );
      // clang-format on
    }
    static bool _compareFile_(const std::string& f1, const std::string& f2) {
      std::ifstream file1;
      std::ifstream file2;
      file1.open(f1, std::ios::binary);
      file2.open(f2, std::ios::binary);

      //---------- compare number of lines in both files ------------------//
      int c1 = 0;
      int c2 = 0;

      std::string str;
      while (!file1.eof()) {
        getline(file1, str);
        c1++;
      }
      while (!file2.eof()) {
        getline(file2, str);
        c2++;
      }

      if (c1 != c2) {
        GUM_TRACE("Not the same number of lines : " << c1 << " != " << c2)
        return false;
      }

      //---------- compare two files line by line ------------------//
      file1.clear();   //  set new value for error control state  //
      file1.seekg(0, std::ios::beg);
      file2.clear();
      file2.seekg(0, std::ios::beg);

      char string1[256];
      char string2[256];
      while (!file1.eof()) {
        file1.getline(string1, 256);
        file2.getline(string2, 256);

        // get rid of linux or windows eol
        if (strlen(string1) > 0) {
          for (char* p = string1 + strlen(string1) - 1; p >= string1; p--) {
            if (*p == '\n') *p = '\0';
            else if (*p == '\r') *p = '\0';
            else break;
          }
        }
        if (strlen(string2) > 0) {
          for (char* p = string2 + strlen(string2) - 1; p >= string2; p--) {
            if (*p == '\n') *p = '\0';
            else if (*p == '\r') *p = '\0';
            else break;
          }
        }

        // we do not compare the comments (can contain version number of agrum
        if ((strncmp(string1, "//", 2) == 0) && (strncmp(string2, "//", 2) == 0)) continue;
        if (strcmp(string1, string2) != 0) {
          GUM_TRACE("The two files differ : ")
          GUM_TRACE_VAR(string1)
          GUM_TRACE_NEWLINE
          GUM_TRACE_VAR(string2)
          return false;
        }
      }

      return true;
    }

    public:
    gum::BayesNet< double >* bn;
    gum::NodeId              i1;
    gum::NodeId              i2;
    gum::NodeId              i3;
    gum::NodeId              i4;
    gum::NodeId              i5;

    void setUp() final {
      bn = new gum::BayesNet< double >();

      gum::LabelizedVariable n1("1", "", 2);
      gum::LabelizedVariable n2("2", "", 2);
      gum::LabelizedVariable n3("3", "", 2);
      gum::LabelizedVariable n4("4", "", 2);
      gum::LabelizedVariable n5("5", "", 3);

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

    void tearDown() final { delete bn; }


    GUM_ACTIVE_TEST(Constuctor) {
      gum::BIFWriter< double >* writer = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(writer = new gum::BIFWriter< double >())
      delete writer;
    }

    GUM_ACTIVE_TEST(Writer_ostream) {
      gum::BIFWriter< double > writer;
      // Uncomment this to check the ouput
      // TS_GUM_ASSERT_THROWS_NOTHING(writer.write(std::cerr, *bn))
    }

    GUM_ACTIVE_TEST(Writer_string) {
      gum::BIFWriter< double > writer;
      std::string              file = GET_RESSOURCES_PATH("outputs/BIFWriter_TestFile.txt");
      TS_GUM_ASSERT_THROWS_NOTHING(writer.write(file, *bn))
      TS_ASSERT(_compareFile_(file, GET_RESSOURCES_PATH("txt/BIFWriter_Model.txt")))
    }

    GUM_ACTIVE_TEST(SyntaxicError) {
      gum::BIFWriter< double > writer;
      {
        TS_ASSERT(!writer.isModificationAllowed())
        std::string file = GET_RESSOURCES_PATH("outputs/shouldNotBeWrittenBIF.txt");
        {
          auto bn = gum::BayesNet< double >::fastPrototype("A->Hello World !->c");
          TS_ASSERT_THROWS(writer.write(file, bn), gum::FatalError&)
        }
        {
          auto bn = gum::BayesNet< double >::fastPrototype("A->Hello World->c");
          TS_ASSERT_THROWS(writer.write(file, bn), gum::FatalError&)
        }
        {
          auto bn = gum::BayesNet< double >::fastPrototype("A->HelloWorld!->c");
          TS_ASSERT_THROWS(writer.write(file, bn), gum::FatalError&)
        }
      }
      {
        writer.setAllowModification(true);
        TS_ASSERT(writer.isModificationAllowed())
        std::string file = GET_RESSOURCES_PATH("outputs/shouldBeWrittenBIF.txt");
        {
          auto bn = gum::BayesNet< double >::fastPrototype("A->Hello World !->c");
          TS_GUM_ASSERT_THROWS_NOTHING(writer.write(file, bn))
        }
        {
          auto bn = gum::BayesNet< double >::fastPrototype("A->Hello World->c");
          TS_GUM_ASSERT_THROWS_NOTHING(writer.write(file, bn))
        }
        {
          auto bn = gum::BayesNet< double >::fastPrototype("A->HelloWorld!->c");
          TS_GUM_ASSERT_THROWS_NOTHING(writer.write(file, bn))
        }
      }
    }
  };
}   // namespace gum_tests
