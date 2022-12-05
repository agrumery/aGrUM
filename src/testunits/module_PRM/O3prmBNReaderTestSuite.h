/**
 *
 *   Copyright (c) 2005-2022 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/PRM/o3prm/O3prmBNReader.h>

namespace gum_tests {

  class [[maybe_unused]] O3prmBNReaderTestSuite: public CxxTest::TestSuite {
    public:
    GUM_TEST(ClassWithoutSystem) {
      gum::BayesNet< double >      bn;
      gum::O3prmBNReader< double > reader(&bn, GET_RESSOURCES_PATH("o3prm/Asia.o3prm"));
      gum::Size                    res = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed())
      TS_ASSERT_EQUALS(bn.property("name"), "Asia")
      TS_ASSERT_EQUALS(res, (gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)1);   // no system
      TS_ASSERT_EQUALS(bn.size(), (gum::Size)8)
    }

    GUM_TEST(ClassWithoutSystemWithOtherClassName) {
      try {
        gum::BayesNet< double >      bn;
        gum::O3prmBNReader< double > reader(&bn,
                                            GET_RESSOURCES_PATH("o3prm/AsiaOtherClassName.o3prm"));
        gum::Size                    res = 0;
        TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed())
        TS_ASSERT_EQUALS(bn.property("name"), "Asia")
        TS_ASSERT_EQUALS(res, (gum::Size)0)
        TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)1);   // no system
        TS_ASSERT_EQUALS(bn.size(), (gum::Size)8)
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }

    GUM_TEST(DoublingClassWithoutSystem) {
      gum::BayesNet< double >      bn;
      gum::O3prmBNReader< double > reader(&bn, GET_RESSOURCES_PATH("o3prm/Asia.o3prm"));
      gum::Size                    res = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed())
      TS_ASSERT_EQUALS(res, (gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)1);   // no system
      TS_ASSERT_EQUALS(bn.size(), (gum::Size)8)


      gum::BayesNet< double >      bn2;
      gum::O3prmBNReader< double > reader2(&bn2, GET_RESSOURCES_PATH("o3prm/Asia.o3prm"));
      gum::Size                    res2 = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(res2 = reader2.proceed())
      TS_ASSERT_EQUALS(res2, (gum::Size)0)
      TS_ASSERT_EQUALS(reader2.warnings(), (gum::Size)1);   // no system
      TS_ASSERT_EQUALS(bn2.size(), (gum::Size)8)
    }

    GUM_TEST(ClassWithoutSystemAfterDeletingReader) {
      gum::BayesNet< double > bn;
      {
        gum::O3prmBNReader< double > reader(&bn, GET_RESSOURCES_PATH("o3prm/Asia.o3prm"));
        gum::Size                    res = 0;
        TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed())
        TS_ASSERT_EQUALS(res, (gum::Size)0)
        TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)1);   // no system
        TS_ASSERT_EQUALS(bn.size(), (gum::Size)8)
      }
    }

    GUM_TEST(ClassesWithSystem) {
      gum::BayesNet< double >      bn;
      gum::O3prmBNReader< double > reader(
         &bn,
         GET_RESSOURCES_PATH("o3prm/AsiaClassAndSystemWithTwoClasses.o3prm"),
         "Asia");
      gum::Size res = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed())
      TS_ASSERT_EQUALS(bn.property("name"), "Asia")
      TS_ASSERT_EQUALS(res, (gum::Size)0)
      TS_ASSERT_EQUALS(bn.size(), (gum::Size)8)
    }

    GUM_TEST(WithError) {
      gum::BayesNet< double >      bn;
      gum::O3prmBNReader< double > reader(&bn,
                                          GET_RESSOURCES_PATH("o3prm/DoesNotExists.o3prm"),
                                          "Asia");
      gum::Size                    res = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed())
      TS_ASSERT_EQUALS(res, (gum::Size)1);               // file not found
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)1);   // file not found
      TS_ASSERT_EQUALS(bn.size(), (gum::Size)0)

      gum::O3prmBNReader< double > reader2(&bn,
                                           GET_RESSOURCES_PATH("o3prm/AsiaWithError.o3prm"),
                                           "Asia");
      res = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(res = reader2.proceed())   // class plop not existing
      TS_ASSERT_EQUALS(reader2.errors(),
                       (gum::Size)1);   // class plop not existing
      TS_ASSERT_EQUALS(bn.size(), (gum::Size)0)
    }

    GUM_TEST(WithCplxFile) {
      gum::BayesNet< double >      bn;
      gum::O3prmBNReader< double > reader(&bn,
                                          GET_RESSOURCES_PATH("o3prm/inference.o3prm"),
                                          "aSys");
      gum::Size                    res = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed())
      TS_ASSERT_EQUALS(res, (gum::Size)0)
      TS_ASSERT_EQUALS(bn.size(), (gum::Size)72)
    }


    GUM_TEST(WithClassPathAndSystem) {
      gum::BayesNet< double >      bn;
      gum::O3prmBNReader< double > reader(
         &bn,
         GET_RESSOURCES_PATH("/o3prmr/ComplexPrinters/fr/lip6/printers/system.o3prm"),
         "Work",
         GET_RESSOURCES_PATH("o3prmr/ComplexPrinters"));
      gum::Size res = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed())
      TS_ASSERT_EQUALS(res, (gum::Size)0)
      TS_ASSERT_EQUALS(bn.size(), (gum::Size)144)
      reader.showElegantErrorsAndWarnings();
    }

    GUM_TEST(NameWithOrWithoutSystem) {
      // in a file with only one class and no system, there should not be any "."
      // in the names
      {
        gum::BayesNet< double >      bn;
        gum::O3prmBNReader< double > reader(&bn, GET_RESSOURCES_PATH("o3prm/Asia.o3prm"));
        gum::Size                    res = 0;
        TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed())
        TS_ASSERT_EQUALS(res, (gum::Size)0)
        TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)1);   // no system
        for (auto n: bn.nodes()) {
          TS_ASSERT_EQUALS(bn.variable(n).name().find("."), std::string::npos)
        }
      }
      {
        gum::BayesNet< double >      bn;
        gum::O3prmBNReader< double > reader(
           &bn,
           GET_RESSOURCES_PATH("o3prm/AsiaClassAndSystemWithTwoClasses.o3prm"),
           "Asia");
        gum::Size res = 0;
        TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed())
        TS_ASSERT_EQUALS(res, (gum::Size)0)
        TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
        for (auto n: bn.nodes()) {
          TS_ASSERT_DIFFERS(bn.variable(n).name().find("."), std::string::npos)
        }
      }
    }

    GUM_TEST(ReadAndWriteAndRead) {
      gum::BayesNet< double >      bn;
      gum::O3prmBNReader< double > reader(&bn, GET_RESSOURCES_PATH("o3prm/Asia.o3prm"));
      gum::Size                    res = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed())
      TS_ASSERT_EQUALS(res, (gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)1);   // no system
      TS_ASSERT_EQUALS(bn.size(), (gum::Size)8)
    }
  };
}   // namespace gum_tests
