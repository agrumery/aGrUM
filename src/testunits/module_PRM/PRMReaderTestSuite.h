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
#include <testsuite_utils.h>

#include <agrum/PRM/o3prm/O3prmReader.h>

namespace gum_tests {

  class PRMReaderTestSuite : public CxxTest::TestSuite {
    public:
    void testBasicRead() {
      gum::prm::o3prm::O3prmReader<double> reader;
      int res;
      TS_GUM_ASSERT_THROWS_NOTHING(
          res = reader.readFile(GET_PATH_STR("o3prm/inference.o3prm")));

      if (res != 0)
        reader.showElegantErrors();

      TS_ASSERT_EQUALS(res, 0);

      gum::prm::PRM<double> *prm = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(prm = reader.prm());
      TS_ASSERT(prm != nullptr);

      delete (prm);
    }

    void /*test*/ Regression1() { // error when attribute defined with interface as
                                  // type
      gum::prm::o3prm::O3prmReader<double> reader;
      int res;
      TS_GUM_ASSERT_THROWS_NOTHING(
          res = reader.readFile(GET_PATH_STR("o3prm/withinterfaces.o3prm")));

      if (res != 0)
        reader.showElegantErrors();

      TS_ASSERT_EQUALS(res, 0);

      gum::prm::PRM<double> *prm = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(prm = reader.prm());
      TS_ASSERT(prm != nullptr);
      delete (prm);
    }

    void testRegression2() {
      gum::prm::o3prm::O3prmReader<double> reader;
      int res;
      TS_GUM_ASSERT_THROWS_NOTHING(
          res = reader.readFile(GET_PATH_STR("o3prm/foo2.o3prm")));

      // if ( res!=0 ) reader.showElegantErrors();

      TS_ASSERT_EQUALS(res, 0);

      gum::prm::PRM<double> *prm = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(prm = reader.prm());
      TS_ASSERT(prm != nullptr);

      delete (prm);
    }
  };

} // namespace gum_tests
