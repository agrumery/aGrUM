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

#include <agrum/config.h>

namespace gum_tests {

  // a test to see if GUM_RANDOMSEED is working
  class RandomProfilerTestSuite : public CxxTest::TestSuite {
    public:
    void testRandomSeed() {

      TS_ASSERT((GUM_RANDOMSEED == 0) || (GUM_RANDOMSEED == 10));

      if (GUM_RANDOMSEED == 0) {
        TS_ASSERT(!matchedRandomValues());
      } else { // GUM_RANDOMSEED==10
        TS_ASSERT(matchedRandomValues());
      }
    }

    private:
#define DELTA_DIFFERS(a, b) ((a) < (b)) ? ((b) - (a) > 1e-6) : ((a) - (b) > 1e-6)
    bool matchedRandomValues(void) {
      gum::initRandom(GUM_RANDOMSEED);

      std::vector<double> v1 = gum::randomDistribution<double>(10);
      std::vector<double> ref1{0.134374,  0.145089, 0.120114, 0.0426642, 0.193954,
                               0.0435726, 0.138849, 0.100258, 0.0060166, 0.0751083};

      for (int i = 0; i < 10; i++) {
        if (DELTA_DIFFERS(v1[i], ref1[i]))
          return false;
      }

      // just to be  sure
      std::vector<double> v2 = gum::randomDistribution<double>(30);
      std::vector<double> ref2{
          0.00436645, 0.00596141, 0.0696046, 0.0696949, 0.062272,    0.00378217,
          0.0191677,  0.00657988, 0.0627761, 0.0400881, 0.047281,    0.069936,
          0.0685442,  0.00993678, 0.0159221, 0.015279,  0.000854096, 0.0618802,
          0.0236426,  0.0382025,  0.0396622, 0.0639614, 0.010478,    0.00444403,
          0.00550434, 0.0686738,  0.0175179, 0.0471658, 0.0274975,   0.0193232,
          0.00436645, 0.00596141, 0.0696046, 0.0696949, 0.062272,    0.00378217,
          0.0191677,  0.00657988, 0.0627761, 0.0400881, 0.047281,    0.069936,
          0.0685442,  0.00993678, 0.0159221, 0.015279,  0.000854096, 0.0618802,
          0.0236426,  0.0382025,  0.0396622, 0.0639614, 0.010478,    0.00444403,
          0.00550434, 0.0686738,  0.0175179, 0.0471658, 0.0274975,   0.0193232};

      for (int i = 0; i < 30; i++) {
        if (DELTA_DIFFERS(v2[i], ref2[i]))
          return false;
      }

      return true;
    }
  };
}
