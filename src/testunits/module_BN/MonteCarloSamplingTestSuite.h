#include <iostream>
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/MonteCarloSampling.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/variables/labelizedVariable.h>

#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/core/approximations/approximationSchemeListener.h>

#include <cxxtest/AgrumApproximationUtils.h>  // must be last include

#define EPSILON_FOR_MONTECARLO_SIMPLE_TEST 9e-2
#define EPSILON_FOR_MONTECARLO 9e-2

namespace gum_tests {

  class aSimpleMCListener : public gum::ApproximationSchemeListener {

    private:
    int         __nbr;
    std::string __mess;

    public:
    aSimpleMCListener(gum::ApproximationScheme& sch)
        : gum::ApproximationSchemeListener(sch)
        , __nbr(0)
        , __mess(""){};
    void whenProgress(const void*     buffer,
                      const gum::Size a,
                      const double    b,
                      const double    c) {
      __nbr++;
    }
    void whenStop(const void* buffer, const std::string s) {
      __nbr++;
      __mess = s;
    }

    int         getNbr() { return __nbr; }
    std::string getMess() { return __mess; }
  };

  class MonteCarloSamplingTestSuite : public CxxTest::TestSuite {

    public:
    void testMCbasic() {
      auto bn = gum::BayesNet< float >::fastPrototype("a->h->c");
      bn.cpt("a").fillWith({0.2f, 0.8f});
      bn.cpt("h").fillWith({0.4f, 0.6f, 0.7f, 0.3f});
      bn.cpt("c").fillWith({0.2f, 0.8f, 0.9f, 0.1f});

      gum::LazyPropagation< float > lazy(&bn);
      lazy.makeInference();

      try {
        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< float > inf(&bn);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }

    void testMCBinaryTreeWithoutEvidence() {
      auto bn = gum::BayesNet< float >::fastPrototype(
        "a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      unsharpen(bn);

      gum::LazyPropagation< float > lazy(&bn);
      lazy.makeInference();

      try {
        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< float > inf(&bn);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }

    void testMCBinaryTreeWithEvidenceOnRoot() {

      auto bn = gum::BayesNet< float >::fastPrototype(
        "a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      unsharpen(bn);

      std::string ev = "b";

      try {

        gum::LazyPropagation< float > lazy(&bn);
        lazy.addEvidence(bn.idFromName(ev), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName(ev), 0);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)
      } catch (gum::Exception& e) {

        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }

    void testMCBinaryTreeWithEvidenceOnLeaf() {
      auto bn = gum::BayesNet< float >::fastPrototype(
        "a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      unsharpen(bn);
      std::string ev = "h";

      try {
        gum::LazyPropagation< float > lazy(&bn);
        lazy.addEvidence(bn.idFromName(ev), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName(ev), 0);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.setVerbosity(false);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }

    void testMCBinaryTreeWithEvidenceOnMid() {
      auto bn = gum::BayesNet< float >::fastPrototype(
        "a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      unsharpen(bn);
      std::string ev = "e";

      try {

        gum::LazyPropagation< float > lazy(&bn);
        lazy.addEvidence(bn.idFromName(ev), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName(ev), 0);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.setVerbosity(false);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }

    void testMCBinaryTreeWithMultipleEvidence() {
      auto bn = gum::BayesNet< float >::fastPrototype(
        "a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      unsharpen(bn);

      try {

        gum::LazyPropagation< float > lazy(&bn);
        lazy.addEvidence(bn.idFromName("e"), 0);
        lazy.addEvidence(bn.idFromName("b"), 1);
        lazy.addEvidence(bn.idFromName("h"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName("e"), 0);
        inf.addEvidence(bn.idFromName("b"), 1);
        inf.addEvidence(bn.idFromName("h"), 0);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.setVerbosity(false);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)

      } catch (gum::Exception& e) {

        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }

    void testMCNaryTreeWithMultipleEvidence() {
      auto bn = gum::BayesNet< float >::fastPrototype(
        "a[4]->d[8]->f[3];b->d->g[5];b->e[4]->h;c->e;i[10]->j[3]->h");
      unsharpen(bn);

      try {

        gum::LazyPropagation< float > lazy(&bn);
        lazy.addEvidence(bn.idFromName("e"), 0);
        lazy.addEvidence(bn.idFromName("b"), 1);
        lazy.addEvidence(bn.idFromName("h"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName("e"), 0);
        inf.addEvidence(bn.idFromName("b"), 1);
        inf.addEvidence(bn.idFromName("h"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)

      } catch (gum::Exception& e) {

        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }


    void testMCSimpleBN() {
      auto bn = gum::BayesNet< float >::fastPrototype("a->b->c;a->d->c", 3);
      unsharpen(bn);

      try {

        gum::LazyPropagation< float > lazy(&bn);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< float > inf(&bn);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)

      } catch (gum::Exception& e) {

        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }

      try {

        gum::LazyPropagation< float > lazy(&bn);
        lazy.addEvidence(bn.idFromName("a"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName("a"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)

      } catch (gum::Exception& e) {

        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }

      try {

        gum::LazyPropagation< float > lazy(&bn);
        lazy.addEvidence(bn.idFromName("d"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName("d"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)

      } catch (gum::Exception& e) {

        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }


    void testMCCplxBN() {
      auto bn = gum::BayesNet< float >::fastPrototype(
        "a->d->f;b->d->g;b->e->h;c->e->g;i->j->h;c->j;x->c;x->j;", 3);

      try {

        gum::LazyPropagation< float > lazy(&bn);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< float > inf(&bn);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)

      } catch (gum::Exception& e) {

        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }

      try {

        gum::LazyPropagation< float > lazy(&bn);
        lazy.addEvidence(bn.idFromName("a"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName("a"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)

      } catch (gum::Exception& e) {

        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }

      try {

        gum::LazyPropagation< float > lazy(&bn);
        lazy.addEvidence(bn.idFromName("d"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName("d"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }

    void testMCAsia() {
      gum::BayesNet< float >  bn;
      gum::BIFReader< float > reader(&bn, GET_RESSOURCES_PATH("asia.bif"));
      int                     nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == 0);

      try {

        gum::LazyPropagation< float > lazy(&bn);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< float > inf(&bn);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)

      } catch (gum::Exception& e) {

        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }


    void testMCAlarm() {
      gum::BayesNet< float >  bn;
      gum::BIFReader< float > reader(&bn, GET_RESSOURCES_PATH("alarm.bif"));
      int                     nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == 0);

      try {

        gum::LazyPropagation< float > lazy(&bn);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< float > inf(&bn);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)

      } catch (gum::Exception& e) {

        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }


    void testMCInfListener() {
      gum::BayesNet< float >  bn;
      gum::BIFReader< float > reader(&bn, GET_RESSOURCES_PATH("alarm.bif"));
      int                     nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == 0);

      gum::MonteCarloSampling< float > inf(&bn);
      aSimpleMCListener                agsl(inf);
      inf.setVerbosity(true);

      try {
        // Testing the inference
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.makeInference();

      } catch (gum::Exception e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }

      TS_ASSERT_EQUALS(agsl.getNbr() * inf.periodSize(), inf.nbrIterations());
      TS_ASSERT_DIFFERS(agsl.getMess(), std::string(""));
    }

    void testConstructor() {
      gum::BayesNet< float >  bn;
      gum::BIFReader< float > reader(&bn, GET_RESSOURCES_PATH("alarm.bif"));
      int                     nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == 0);
      try {
        gum::MonteCarloSampling< float > inf(&bn);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
      } catch (gum::Exception e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }


    void testEvidenceAsTargetOnCplxBN() {
      auto bn = gum::BayesNet< float >::fastPrototype(
        "a->d->f;b->d->g;b->e->h;c->e->g;i->j->h;c->j;x->c;x->j;", 3);
      unsharpen(bn);

      try {
        gum::MonteCarloSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName("d"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.makeInference();
        TS_GUM_ASSERT_THROWS_NOTHING(inf.posterior("d"));
        TS_GUM_ASSERT_THROWS_NOTHING(inf.posterior(bn.idFromName("d")));

      } catch (gum::Exception& e) {

        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }
  };
}
