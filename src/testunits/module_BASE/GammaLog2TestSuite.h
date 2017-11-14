
/***************************************************************************
*   Copyright (C) 2017 by Pierre-Henri WUILLEMIN and Christophe GONZALES   *
*   {prenom.nom}_at_lip6.fr                                               *
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


#include <cmath>
#include <iostream>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>
#include <ressources/myalloc.h>

#include <agrum/core/math/gammaLog2.h>

namespace gum_tests {

  class GammaLog2TestSuite : public CxxTest::TestSuite {
    public:
    void test1() {
      gum::GammaLog2 f;
      gum::GammaLog2 g(true);

      for (auto i = 0; i < 60; ++i) {
        for (auto j = 0; j < 100; ++j) {
          if (i + j > 0) {
            double index = i + j / 100.0;
            TS_ASSERT_DELTA(f(index), g(index), 0.1);

            double findex = i + j / 100.0;
            TS_ASSERT_DELTA(f(findex), g(findex), 0.1);
          }
        }
      }
    }
  };

}  // namespace gum_tests
