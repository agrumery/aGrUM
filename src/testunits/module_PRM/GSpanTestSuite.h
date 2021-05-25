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


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/PRM/inference/gspan.h>

#include <agrum/PRM/o3prm/O3prmReader.h>

namespace gum_tests {

  class GSpanTestSuite: public CxxTest::TestSuite {
    private:
    gum::prm::o3prm::O3prmReader< double >*     _driver_;
    std::string                                 _dot_dir_;
    gum::prm::gspan::InterfaceGraph< double >*  _ig_;

    void  _local_setUp() {
       _driver_ = new gum::prm::o3prm::O3prmReader< double >();
       _driver_->readFile(GET_RESSOURCES_PATH("o3prm/specialprinters.o3prm"));

      if ( _driver_->errors() == 0) {
         _ig_ = new gum::prm::gspan::InterfaceGraph< double >(
            _driver_->prm()->getSystem("m"));
      } else {
         _driver_->showElegantErrorsAndWarnings();
        TS_ASSERT(false);
      }
    }

    void  _local_tearDown() {
      delete  _ig_;

      if ( _driver_->prm() != nullptr) { delete  _driver_->prm(); }

      delete  _driver_;
    }

    public:
    void testInit() {
      gum::prm::o3prm::O3prmReader< double > driver;
      driver.readFile(GET_RESSOURCES_PATH("o3prm/specialprinters.o3prm"));
      TS_ASSERT(driver.prm() != nullptr);

      if (driver.prm() != nullptr) delete driver.prm();
    }

    void testInterfaceGraph() {
      try {
         _local_setUp();
        TS_ASSERT_EQUALS( _ig_->graph().size(),
                         (gum::Size)1 + 5 * 2 + 4 * 3 + 4 * 3 + 5 + 3 + 4);
        TS_ASSERT_EQUALS( _ig_->graph().sizeEdges(),
                         (gum::Size)(5 * 2 + 3 * 4 + 4 * 3) + 5 + 3 * 3 + 4 * 2);
         _local_tearDown();
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }

    void testTree() {
       _local_setUp();
      gum::prm::GSpan< double >* gspan = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(
         gspan = new gum::prm::GSpan< double >(*( _driver_->prm()),
                                                _driver_->prm()->getSystem("m")));
      TS_ASSERT(gspan != nullptr);

      if (gspan != nullptr) {
        TS_GUM_ASSERT_THROWS_NOTHING(gspan->discoverPatterns());
        TS_GUM_ASSERT_THROWS_NOTHING(delete gspan);
      }

       _local_tearDown();
    }
  };

}   // namespace gum_tests
