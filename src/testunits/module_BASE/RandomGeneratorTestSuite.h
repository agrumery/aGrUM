/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
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

#include <agrum/config.h>

#include <agrum/base/core/timer.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/generator/MCBayesNetGenerator.h>

#include <agrum/base/core/utils_random.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  RandomGenerator
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {
  // a test to see if GUM_RANDOMSEED is working
  struct RandomGeneratorTestSuite {
    public:
    static void testRandomSeed() {
      CHECK(((GUM_RANDOMSEED == 0) || (GUM_RANDOMSEED == 10)));

      CHECK((GUM_RANDOMSEED) == (0));

      gum::initRandom(20);
      const auto x1 = gum::randomProba();
      const auto x2 = gum::randomProba();
      const auto x3 = gum::randomProba();

      gum::initRandom(0);
      // can falsely fail in rare occasion
      if (fabs((gum::randomProba()) - (x1)) < GUM_SMALL_ERROR)
        if (fabs((gum::randomProba()) - (x2)) < GUM_SMALL_ERROR)
          if (fabs((gum::randomProba()) - (x3)) < GUM_SMALL_ERROR) CHECK(false);
      gum::initRandom(20);
      CHECK(fabs((gum::randomProba()) - (x1)) < 1e-5);
      CHECK(fabs((gum::randomProba()) - (x2)) < 1e-5);
      CHECK(fabs((gum::randomProba()) - (x3)) < 1e-5);

      gum::initRandom(GUM_RANDOMSEED);
    }   // namespace gum_tests

    static void testRandomSeeForStructure() {
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

    static void testBugSeed() {
      gum::initRandom(0);
      const auto x0 = gum::randomProba();
      gum::initRandom(10);
      const auto x10 = gum::randomProba();
      gum::initRandom(42);
      const auto x42 = gum::randomProba();

      gum::initRandom(0);
      CHECK((x0) != (gum::randomProba()));   // may fail but highly improbable
      gum::initRandom(10);
      CHECK((x10) == (gum::randomProba()));
      gum::initRandom(42);
      CHECK((x42) == (gum::randomProba()));
      gum::initRandom(GUM_RANDOMSEED);
    }

    static void testBugSeed2() {
      gum::initRandom(0);
      const auto bn0 = gum::BayesNet< double >::fastPrototype("A->B<-C");
      gum::initRandom(10);
      const auto bn10 = gum::BayesNet< double >::fastPrototype("A->B<-C");
      gum::initRandom(42);
      const auto bn42 = gum::BayesNet< double >::fastPrototype("A->B<-C");

      gum::Timer t;
      t.reset();
      double second = 0.5;
      while (t.step() <= second)
        ;

      gum::initRandom(0);
      const auto bn0b = gum::BayesNet< double >::fastPrototype("A->B<-C");
      gum::initRandom(10);
      const auto bn10b = gum::BayesNet< double >::fastPrototype("A->B<-C");
      gum::initRandom(42);
      const auto bn42b = gum::BayesNet< double >::fastPrototype("A->B<-C");
      const auto q42b  = bn42b.cpt("B") + bn42b.cpt("C") + bn42b.cpt("A");

      for (const auto i: bn0.nodes())
        // may fail but highly improbable
        GUM_CHECK_TENSOR_DIFFERS(bn0.cpt(i), bn0b.cpt(bn0.variable(i).name()));

      for (const auto i: bn10.nodes())
        GUM_CHECK_TENSOR_ALMOST_EQUALS(bn10.cpt(i), bn10b.cpt(bn10.variable(i).name()));

      for (const auto i: bn42.nodes())
        GUM_CHECK_TENSOR_ALMOST_EQUALS(bn42.cpt(i), bn42b.cpt(bn42.variable(i).name()));

      gum::initRandom(GUM_RANDOMSEED);
    }
  };

  GUM_TEST_ACTIF(RandomSeed)
  GUM_TEST_ACTIF(RandomSeeForStructure)
  GUM_TEST_ACTIF(BugSeed)
  GUM_TEST_ACTIF(BugSeed2)
}   // namespace gum_tests
