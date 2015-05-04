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

#include <iostream>
#include <cmath>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>
#include <ressources/myalloc.h>

#include <agrum/core/math/gammaLog2.h>

namespace gum_tests {

  class GammaLog2TestSuite : public CxxTest::TestSuite {
    public:
    void test1() {
      gum::GammaLog2 f;
      gum::GammaLog2 g(true);

      for (unsigned int i = 0; i < 60; ++i) {
        for (unsigned int j = 0; j < 100; ++j) {
          if (i + j > 0) {
            float index = i + j / 100.0f;
            TS_ASSERT(fabs(f(index) - g(index)) < 0.01);
          }
        }
      }
    }
  };

} // namespace gum_tests
