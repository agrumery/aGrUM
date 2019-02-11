/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/PRM/o3prm/O3prmBNReader.h>

namespace gum_tests {

  class O3prmBNReaderTestSuite : public CxxTest::TestSuite {
    public:
    void testClassWithoutSystem() {
      gum::BayesNet< double >      bn;
      gum::O3prmBNReader< double > reader(&bn,
                                          GET_RESSOURCES_PATH("o3prm/Asia.o3prm"));
      gum::Size                    res = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed());
      TS_ASSERT_EQUALS(res, (gum::Size)0);
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)1);   // no system
      TS_ASSERT_EQUALS(bn.size(), (gum::Size)8);
    }

    void testDoublingClassWithoutSystem() {
      gum::BayesNet< double >      bn;
      gum::O3prmBNReader< double > reader(&bn,
                                          GET_RESSOURCES_PATH("o3prm/Asia.o3prm"));
      gum::Size                    res = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed());
      TS_ASSERT_EQUALS(res, (gum::Size)0);
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)1);   // no system
      TS_ASSERT_EQUALS(bn.size(), (gum::Size)8);


      gum::BayesNet< double >      bn2;
      gum::O3prmBNReader< double > reader2(
         &bn2, GET_RESSOURCES_PATH("o3prm/Asia.o3prm"));
      gum::Size res2 = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(res2 = reader2.proceed());
      TS_ASSERT_EQUALS(res2, (gum::Size)0);
      TS_ASSERT_EQUALS(reader2.warnings(), (gum::Size)1);   // no system
      TS_ASSERT_EQUALS(bn2.size(), (gum::Size)8);
    }

    void testClassWithoutSystemAfterDeletingReader() {
      gum::BayesNet< double > bn;
      {
        gum::O3prmBNReader< double > reader(
           &bn, GET_RESSOURCES_PATH("o3prm/Asia.o3prm"));
        gum::Size res = 0;
        TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed());
        TS_ASSERT_EQUALS(res, (gum::Size)0);
        TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)1);   // no system
        TS_ASSERT_EQUALS(bn.size(), (gum::Size)8);
      }
    }

    void testClassesWithSystem() {
      gum::BayesNet< double >      bn;
      gum::O3prmBNReader< double > reader(
         &bn,
         GET_RESSOURCES_PATH("o3prm/AsiaClassAndSystemWithTwoClasses.o3prm"),
         "Asia");
      gum::Size res = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed());
      TS_ASSERT_EQUALS(res, (gum::Size)0);
      TS_ASSERT_EQUALS(bn.size(), (gum::Size)8);
    }

    void testWithError() {
      gum::BayesNet< double >      bn;
      gum::O3prmBNReader< double > reader(
         &bn, GET_RESSOURCES_PATH("o3prm/DoesNotExists.o3prm"), "Asia");
      gum::Size res = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed());
      TS_ASSERT_EQUALS(res, (gum::Size)1);               // file not found
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)1);   // file not found
      TS_ASSERT_EQUALS(bn.size(), (gum::Size)0);

      gum::O3prmBNReader< double > reader2(
         &bn, GET_RESSOURCES_PATH("o3prm/AsiaWithError.o3prm"), "Asia");
      res = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(
         res = reader2.proceed())   // class plop not existing
      TS_ASSERT_EQUALS(reader2.errors(),
                       (gum::Size)1);   // class plop not existing
      TS_ASSERT_EQUALS(bn.size(), (gum::Size)0);
    }

    void testWithCplxFile() {
      gum::BayesNet< double >      bn;
      gum::O3prmBNReader< double > reader(
         &bn, GET_RESSOURCES_PATH("o3prm/inference.o3prm"), "aSys");
      gum::Size res = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed());
      TS_ASSERT_EQUALS(res, (gum::Size)0);
      TS_ASSERT_EQUALS(bn.size(), (gum::Size)72);
    }


    void testWithClassPathAndSystem() {
      gum::BayesNet< double >      bn;
      gum::O3prmBNReader< double > reader(
         &bn,
         GET_RESSOURCES_PATH(
            "/o3prmr/ComplexPrinters/fr/lip6/printers/system.o3prm"),
         "Work",
         GET_RESSOURCES_PATH("o3prmr/ComplexPrinters"));
      gum::Size res = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed());
      TS_ASSERT_EQUALS(res, (gum::Size)0);
      TS_ASSERT_EQUALS(bn.size(), (gum::Size)144);
      reader.showElegantErrorsAndWarnings();
    }

    void testNameWithOrWithoutSystem() {
      // in a file with only one class and no system, there should not be any "."
      // in the names
      {
        gum::BayesNet< double >      bn;
        gum::O3prmBNReader< double > reader(
           &bn, GET_RESSOURCES_PATH("o3prm/Asia.o3prm"));
        gum::Size res = 0;
        TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed());
        TS_ASSERT_EQUALS(res, (gum::Size)0);
        TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)1);   // no system
        for (auto n : bn.nodes()) {
          TS_ASSERT(bn.variable(n).name().find(".") == std::string::npos);
        }
      }
      {
        gum::BayesNet< double >      bn;
        gum::O3prmBNReader< double > reader(
           &bn,
           GET_RESSOURCES_PATH("o3prm/AsiaClassAndSystemWithTwoClasses.o3prm"),
           "Asia");
        gum::Size res = 0;
        TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed());
        TS_ASSERT_EQUALS(res, (gum::Size)0);
        TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
        for (auto n : bn.nodes()) {
          TS_ASSERT(bn.variable(n).name().find(".") != std::string::npos);
        }
      }
    }
  };
}   // namespace gum_tests
