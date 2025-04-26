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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#include <gumtest/AgrumTestSuite.h>
#include <agrum/base/core/timer.h>
#include <gumtest/utils.h>

#include <agrum/config.h>

#include <agrum/base/core/timer.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/generator/MCBayesNetGenerator.h>

#include <agrum/base/core/utils_random.h>

namespace gum_tests {
  // a test to see if GUM_RANDOMSEED is working
  class [[maybe_unused]] RandomProfilerTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(RandomSeed) {
      TS_ASSERT((GUM_RANDOMSEED == 0) || (GUM_RANDOMSEED == 10))

      TS_ASSERT_EQUALS(GUM_RANDOMSEED, 0);

      gum::initRandom(20);
      const auto x1 = gum::randomProba();
      const auto x2 = gum::randomProba();
      const auto x3 = gum::randomProba();

      gum::initRandom(0);
      // can falsely fail in rare occasion
      if (fabs((gum::randomProba()) - (x1)) < TS_GUM_SMALL_ERROR)
        if (fabs((gum::randomProba()) - (x2)) < TS_GUM_SMALL_ERROR)
          if (fabs((gum::randomProba()) - (x3)) < TS_GUM_SMALL_ERROR) TS_ASSERT(false);
      gum::initRandom(20);
      TS_GUM_ASSERT_ALMOST_EQUALS(gum::randomProba(), x1)
      TS_GUM_ASSERT_ALMOST_EQUALS(gum::randomProba(), x2)
      TS_GUM_ASSERT_ALMOST_EQUALS(gum::randomProba(), x3)

      gum::initRandom(GUM_RANDOMSEED);
    }

    GUM_ACTIVE_TEST(RandomSeeForStructure) {
      try {
        const auto n_nodes  = 100;
        const auto n_arcs   = 150;
        const auto n_modmax = 3;

        gum::MCBayesNetGenerator< double > gen(n_nodes, n_arcs, n_modmax);
        gum::BayesNet< double >            bn;

        gum::initRandom(20);
        gen.generateBN(bn);
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    GUM_ACTIVE_TEST(BugSeed) {
      gum::initRandom(0);
      const auto x0 = gum::randomProba();
      gum::initRandom(10);
      const auto x10 = gum::randomProba();
      gum::initRandom(42);
      const auto x42 = gum::randomProba();

      gum::initRandom(0);
      TS_ASSERT_DIFFERS(x0, gum::randomProba()) // may fail but highly improbable
      gum::initRandom(10);
      TS_ASSERT_EQUALS(x10, gum::randomProba())
      gum::initRandom(42);
      TS_ASSERT_EQUALS(x42, gum::randomProba())
      gum::initRandom(GUM_RANDOMSEED);
    }

    GUM_ACTIVE_TEST(BugSeed2) {
      gum::initRandom(0);
      const auto bn0 = gum::BayesNet< double >::fastPrototype("A->B<-C");
      const auto q0  = bn0.cpt("B") + bn0.cpt("C") + bn0.cpt("A");
      gum::initRandom(10);
      const auto bn10 = gum::BayesNet< double >::fastPrototype("A->B<-C");
      const auto q10  = bn10.cpt("B") + bn10.cpt("C") + bn10.cpt("A");
      gum::initRandom(42);
      const auto bn42 = gum::BayesNet< double >::fastPrototype("A->B<-C");
      const auto q42  = bn42.cpt("B") + bn42.cpt("C") + bn42.cpt("A");

      gum::Timer t;
      t.reset();
      double second = 0.5;
      while (t.step() <= second);

      gum::initRandom(0);
      const auto bn0b = gum::BayesNet< double >::fastPrototype("A->B<-C");
      const auto q0b  = bn0b.cpt("B") + bn0b.cpt("C") + bn0b.cpt("A");
      TS_ASSERT_DIFFERS(q0.sum(), q0b.sum()) // may fail but highly improbable
      gum::initRandom(10);
      const auto bn10b = gum::BayesNet< double >::fastPrototype("A->B<-C");
      const auto q10b  = bn10b.cpt("B") + bn10b.cpt("C") + bn10b.cpt("A");
      TS_ASSERT_EQUALS(q10.sum(), q10b.sum())
      gum::initRandom(42);
      const auto bn42b = gum::BayesNet< double >::fastPrototype("A->B<-C");
      const auto q42b  = bn42b.cpt("B") + bn42b.cpt("C") + bn42b.cpt("A");
      TS_ASSERT_EQUALS(q42.sum(), q42b.sum())
      gum::initRandom(GUM_RANDOMSEED);
    }
  };
} // namespace gum_tests
