
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES
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


#include <iostream>
#include <string>
#include <vector>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/MN/inference/ShaferShenoyMNInference.h>
#include <agrum/MN/MarkovNet.h>
#include <agrum/tools/multidim/potential.h>
#include <agrum/tools/variables/labelizedVariable.h>
#include <agrum/tools/va®riables/rangeVariable.h>
#include <agrum/tools/variables/discretizedVariable.h>

namespace gum_tests {
  class MarkovNetTestSuite: public CxxTest::TestSuite {

    void testConstructor() {
      auto mn=gum::MarkovNet<double>::fastPrototype("A-B-C;C-D;C-E-F");
      gum::ShaferShenoyMNInference<double> ie(mn);
    }
  };
  } // gum_tests