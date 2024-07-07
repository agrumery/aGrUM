/**
 *
 *   Copyright (c) 2005-2024 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


#include <iostream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/approximations/approximationSchemeListener.h>
#include <agrum/base/multidim/potential.h>
#include <agrum/base/variables/labelizedVariable.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/importanceSampling.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/io/BIF/BIFReader.h>

#include <gumtest/AgrumApproximationUtils.h>   // must be last include

#define EPSILON_FOR_IMPORTANCE_SIMPLE_TEST 15e-2
#define EPSILON_FOR_IMPORTANCE             7e-2

namespace gum_tests {

  class aSimpleImportanceListener: public gum::ApproximationSchemeListener {
    private:
    int         __nbr;
    std::string __mess;

    public:
    aSimpleImportanceListener(gum::ApproximationScheme& sch) :
        gum::ApproximationSchemeListener(sch), __nbr(0), __mess("") {};

    void whenProgress(const void* buffer, const gum::Size a, const double b, const double c) {
      __nbr++;
    }

    void whenStop(const void* buffer, const std::string& s) {
      __nbr++;
      __mess = s;
    }

    int getNbr() { return __nbr; }

    std::string getMess() { return __mess; }
  };

  class [[maybe_unused]] ImportanceSamplingTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(ImportanceBinaryTreeWithoutEvidence) {
      auto bn = gum::BayesNet< double >::fastPrototype("a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      unsharpen(bn);

      gum::LazyPropagation< double > lazy(&bn);
      lazy.makeInference();


      try {
        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::ImportanceSampling< double > inf(&bn);

          inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_IMPORTANCE_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false)
      }
    }

    GUM_ACTIVE_TEST(ImportanceBinaryTreeWithEvidenceOnRoot) {
      auto bn = gum::BayesNet< double >::fastPrototype("a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      unsharpen(bn);
      std::string ev = "b";

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName(ev), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::ImportanceSampling< double > inf(&bn);
          inf.addEvidence(bn.idFromName(ev), 0);
          inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_IMPORTANCE_SIMPLE_TEST)
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false)
      }
    }

    GUM_ACTIVE_TEST(ImportanceBinaryTreeWithEvidenceOnLeaf) {
      auto bn = gum::BayesNet< double >::fastPrototype("a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      unsharpen(bn);
      std::string ev = "h";

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName(ev), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::ImportanceSampling< double > inf(&bn);
          inf.addEvidence(bn.idFromName(ev), 0);
          inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
          inf.setVerbosity(false);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_IMPORTANCE_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false)
      }
    }

    GUM_ACTIVE_TEST(ImportanceBinaryTreeWithEvidenceOnMid) {
      auto bn = gum::BayesNet< double >::fastPrototype("a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      unsharpen(bn);
      std::string ev = "e";

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName(ev), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::ImportanceSampling< double > inf(&bn);
          inf.addEvidence(bn.idFromName(ev), 0);
          inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
          inf.setVerbosity(false);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_IMPORTANCE_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false)
      }
    }

    GUM_ACTIVE_TEST(ImportanceBinaryTreeWithMultipleEvidence) {
      auto bn = gum::BayesNet< double >::fastPrototype("a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      unsharpen(bn);

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName("e"), 0);
        lazy.addEvidence(bn.idFromName("b"), 1);
        lazy.addEvidence(bn.idFromName("h"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::ImportanceSampling< double > inf(&bn);
          inf.addEvidence(bn.idFromName("e"), 0);
          inf.addEvidence(bn.idFromName("b"), 1);
          inf.addEvidence(bn.idFromName("h"), 0);
          inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
          inf.setVerbosity(false);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_IMPORTANCE_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false)
      }
    }

    GUM_ACTIVE_TEST(ImportanceNaryTreeWithMultipleEvidence) {
      auto bn = gum::BayesNet< double >::fastPrototype(
          "a[4]->d[8]->f[3];b->d->g[5];b->e[4]->h;c->e;i[10]->j[3]->h");
      unsharpen(bn);

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName("e"), 0);
        lazy.addEvidence(bn.idFromName("b"), 1);
        lazy.addEvidence(bn.idFromName("h"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::ImportanceSampling< double > inf(&bn);
          inf.addEvidence(bn.idFromName("e"), 0);
          inf.addEvidence(bn.idFromName("b"), 1);
          inf.addEvidence(bn.idFromName("h"), 0);
          inf.setVerbosity(false);
          inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_IMPORTANCE_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false)
      }
    }

    GUM_ACTIVE_TEST(ImportanceSimpleBN) {
      auto bn = gum::BayesNet< double >::fastPrototype("a->b->c;a->d->c", 3);
      unsharpen(bn);

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::ImportanceSampling< double > inf(&bn);
          inf.setVerbosity(false);
          inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_IMPORTANCE_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false)
      }

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName("a"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::ImportanceSampling< double > inf(&bn);
          inf.addEvidence(bn.idFromName("a"), 0);
          inf.setVerbosity(false);
          inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_IMPORTANCE_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false)
      }

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName("d"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::ImportanceSampling< double > inf(&bn);
          inf.addEvidence(bn.idFromName("d"), 0);
          inf.setVerbosity(false);
          inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_IMPORTANCE_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false)
      }
    }

    GUM_ACTIVE_TEST(ImportanceCplxBN) {
      auto bn = gum::BayesNet< double >::fastPrototype(
          "a->d->f;b->d->g;b->e->h;c->e->g;i->j->h;c->j;x->c;x->j;",
          3);
      unsharpen(bn);

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::ImportanceSampling< double > inf(&bn);
          inf.setVerbosity(false);
          inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_IMPORTANCE_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false)
      }

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName("a"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::ImportanceSampling< double > inf(&bn);
          inf.addEvidence(bn.idFromName("a"), 0);
          inf.setVerbosity(false);
          inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_IMPORTANCE_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false)
      }


      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName("d"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::ImportanceSampling< double > inf(&bn);
          inf.addEvidence(bn.idFromName("d"), 0);
          inf.setVerbosity(false);
          inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_IMPORTANCE_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false)
      }
    }

    GUM_ACTIVE_TEST(ImportanceEvidenceAsTargetOnCplxBN) {
      auto bn = gum::BayesNet< double >::fastPrototype(
          "a->d->f;b->d->g;b->e->h;c->e->g;i->j->h;c->j;x->c;x->j;",
          3);
      unsharpen(bn);

      try {
        gum::ImportanceSampling< double > inf(&bn);
        inf.addEvidence(bn.idFromName("d"), 0);
        TS_ASSERT_THROWS(inf.addEvidence("i", std::vector< double >{1, 0, 1}),
                         const gum::FatalError&)

        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
        inf.makeInference();
        TS_GUM_ASSERT_THROWS_NOTHING(inf.posterior("d"))
        TS_GUM_ASSERT_THROWS_NOTHING(inf.posterior(bn.idFromName("d")))

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false)
      }
    }

    GUM_ACTIVE_TEST(ImportanceAsia) {
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("bif/asia.bif"));
      gum::Size                nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)0)

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::ImportanceSampling< double > inf(&bn);
          inf.setVerbosity(false);
          inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_IMPORTANCE_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false)
      }
    }

    GUM_ACTIVE_TEST(ImportanceAlarm) {
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("bif/alarm.bif"));
      gum::Size                nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)0)

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::ImportanceSampling< double > inf(&bn);
          inf.setVerbosity(false);
          inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_IMPORTANCE_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false)
      }
    }

    GUM_ACTIVE_TEST(ImportanceDiabetes) {
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("bif/Diabetes.bif"));
      gum::Size                nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)0)

      try {
        gum::ImportanceSampling< double > inf(&bn);
        inf.setVerbosity(false);
        inf.setMaxTime(5);
        inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
        inf.makeInference();

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false)
      }
      TS_ASSERT(true)
    }

    GUM_ACTIVE_TEST(ImportanceInfListener) {
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("bif/alarm.bif"));
      gum::Size                nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)0)

      gum::ImportanceSampling< double > inf(&bn);
      aSimpleImportanceListener         agsl(inf);
      inf.setVerbosity(false);

      inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
      TS_GUM_ASSERT_THROWS_NOTHING(inf.makeInference())

      TS_ASSERT_EQUALS(agsl.getNbr() * inf.periodSize(), inf.nbrIterations())
      TS_ASSERT_DIFFERS(agsl.getMess(), std::string(""))
    }
  };
}   // namespace gum_tests
