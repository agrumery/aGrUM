/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/
#pragma once


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/PRM/inference/gspan.h>
#include <agrum/PRM/o3prm/O3prmReader.h>

namespace gum_tests {

  class GUM_TEST_SUITE(GSpan) {
    gum::prm::o3prm::O3prmReader< double >*    _driver_;
    std::string                                _dot_dir_;
    gum::prm::gspan::InterfaceGraph< double >* _ig_;

    void _local_setUp() {
      _driver_ = new gum::prm::o3prm::O3prmReader< double >();
      _driver_->readFile(GET_RESSOURCES_PATH("o3prm/specialprinters.o3prm"));

      if (_driver_->errors() == 0) {
        _ig_ = new gum::prm::gspan::InterfaceGraph< double >(_driver_->prm()->getSystem("m"));
      } else {
        _driver_->showElegantErrorsAndWarnings();
        TS_ASSERT(false)
      }
    }

    void _local_tearDown() {
      delete _ig_;

      if (_driver_->prm() != nullptr) { delete _driver_->prm(); }

      delete _driver_;
    }

    public:
    GUM_ACTIVE_TEST(Init) {
      gum::prm::o3prm::O3prmReader< double > driver;
      driver.readFile(GET_RESSOURCES_PATH("o3prm/specialprinters.o3prm"));
      TS_ASSERT_DIFFERS(driver.prm(), nullptr)

      if (driver.prm() != nullptr) delete driver.prm();
    }

    GUM_ACTIVE_TEST(InterfaceGraph) {
      try {
        _local_setUp();
        TS_ASSERT_EQUALS(_ig_->graph().size(),
                         static_cast< gum::Size >(1) + 5 * 2 + 4 * 3 + 4 * 3 + 5 + 3 + 4)
        TS_ASSERT_EQUALS(_ig_->graph().sizeEdges(),
                         (gum::Size)(5 * 2 + 3 * 4 + 4 * 3) + 5 + 3 * 3 + 4 * 2);
        _local_tearDown();
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }

    GUM_ACTIVE_TEST(Tree) {
      _local_setUp();
      gum::prm::GSpan< double >* gspan = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(
          gspan
          = new gum::prm::GSpan< double >(*(_driver_->prm()), _driver_->prm()->getSystem("m")));
      TS_ASSERT_DIFFERS(gspan, nullptr)

      if (gspan != nullptr) {
        TS_GUM_ASSERT_THROWS_NOTHING(gspan->discoverPatterns())
        TS_GUM_ASSERT_THROWS_NOTHING(delete gspan)
      }

      _local_tearDown();
    }
  };

}   // namespace gum_tests
