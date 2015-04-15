/***************************************************************************
 *   (C) 2007-2013 by Christophe GONZALES and Pierre-Henri WUILLEMIN       *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
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

#include <agrum/PRM/inference/gspan.h>

#include <agrum/PRM/o3prm/O3prmReader.h>

namespace gum_tests {

  class GSpanTestSuite : public CxxTest::TestSuite {
    private:
    gum::prm::o3prm::O3prmReader<double> *__driver;
    std::string dot_dir;
    gum::prm::gspan::InterfaceGraph<double> *ig;

    void local__setUp() {
      __driver = new gum::prm::o3prm::O3prmReader<double>();
      __driver->readFile(GET_PATH_STR("o3prm/specialprinters.o3prm"));

      if (__driver->prm() != nullptr)
        ig = new gum::prm::gspan::InterfaceGraph<double>(
            __driver->prm()->system("m"));
      else {
        __driver->showErrorCounts();
        TS_ASSERT(false);
      }
    }

    void local__tearDown() {
      delete ig;

      if (__driver->prm() != nullptr)
        delete __driver->prm();

      delete __driver;
    }

    public:
    void testInit() {
      gum::prm::o3prm::O3prmReader<double> driver;
      driver.readFile(GET_PATH_STR("o3prm/specialprinters.o3prm"));
      TS_ASSERT(driver.prm() != nullptr);

      if (driver.prm() != nullptr)
        delete driver.prm();
    }

    void testInterfaceGraph() {
      local__setUp();
      TS_ASSERT_EQUALS(ig->graph().size(),
                       (gum::Size)1 + 5 * 2 + 4 * 3 + 4 * 3 + 5 + 3 + 4);
      TS_ASSERT_EQUALS(ig->graph().sizeEdges(),
                       (gum::Size)(5 * 2 + 3 * 4 + 4 * 3) + 5 + 3 * 3 + 4 * 2);
      local__tearDown();
    }

    void testTree() {
      local__setUp();
      gum::prm::GSpan<double> *gspan = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(
          gspan = new gum::prm::GSpan<double>(*(__driver->prm()),
                                              __driver->prm()->system("m")));
      TS_ASSERT(gspan != nullptr);

      if (gspan != nullptr) {
        TS_GUM_ASSERT_THROWS_NOTHING(gspan->discoverPatterns());
        TS_GUM_ASSERT_THROWS_NOTHING(delete gspan);
      }

      local__tearDown();
    }
  };

} // tests
