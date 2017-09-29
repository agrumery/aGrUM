
#include <iostream>
#include <string>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/weightedSampling.h>
#include <agrum/variables/labelizedVariable.h>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/core/approximations/approximationSchemeListener.h>

#include <cxxtest/AgrumApproximationUtils.h>  // must be last include


#define EPSILON_FOR_WEIGHTED_SIMPLE_TEST 5e-2
#define EPSILON_FOR_WEIGHTED 5e-2


namespace gum_tests {

  class aSimpleWeightedListener : public gum::ApproximationSchemeListener {

    private:
    int         __nbr;
    std::string __mess;

    public:
    aSimpleWeightedListener(gum::ApproximationScheme& sch)
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

  class WeightedSamplingTestSuite : public CxxTest::TestSuite {

    public:
    void testWeightedBinaryTreeWithoutEvidence() {
      auto bn = gum::BayesNet< float >::fastPrototype(
        "a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      unsharpen(bn);

      gum::LazyPropagation< float > lazy(&bn);
      lazy.makeInference();


      try {
        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::WeightedSampling< float > inf(&bn);
        inf.setEpsilon(EPSILON_FOR_WEIGHTED);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_WEIGHTED_SIMPLE_TEST);
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }

    void testWeightedBinaryTreeWithEvidenceOnRoot() {

      auto bn = gum::BayesNet< float >::fastPrototype(
        "a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      unsharpen(bn);
      std::string ev = "b";

      try {  // ok //

        gum::LazyPropagation< float > lazy(&bn);
        lazy.addEvidence(bn.idFromName(ev), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::WeightedSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName(ev), 0);
        inf.setEpsilon(EPSILON_FOR_WEIGHTED);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_WEIGHTED_SIMPLE_TEST);
      } catch (gum::Exception& e) {

        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }

    void testWeightedBinaryTreeWithEvidenceOnLeaf() {
      auto bn = gum::BayesNet< float >::fastPrototype(
        "a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      unsharpen(bn);
      std::string ev = "h";

      try {
        gum::LazyPropagation< float > lazy(&bn);
        lazy.addEvidence(bn.idFromName(ev), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::WeightedSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName(ev), 0);
        inf.setEpsilon(EPSILON_FOR_WEIGHTED);
        inf.setVerbosity(false);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_WEIGHTED_SIMPLE_TEST);

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }

    void testWeightedBinaryTreeWithEvidenceOnMid() {
      auto bn = gum::BayesNet< float >::fastPrototype(
        "a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      unsharpen(bn);
      std::string ev = "e";

      try {

        gum::LazyPropagation< float > lazy(&bn);
        lazy.addEvidence(bn.idFromName(ev), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::WeightedSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName(ev), 0);
        inf.setEpsilon(EPSILON_FOR_WEIGHTED);
        inf.setVerbosity(false);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_WEIGHTED_SIMPLE_TEST);

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }

    void testWeightedBinaryTreeWithMultipleEvidence() {
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
        gum::WeightedSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName("e"), 0);
        inf.addEvidence(bn.idFromName("b"), 1);
        inf.addEvidence(bn.idFromName("h"), 0);
        inf.setEpsilon(EPSILON_FOR_WEIGHTED);
        inf.setVerbosity(false);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_WEIGHTED_SIMPLE_TEST);
      } catch (gum::Exception& e) {

        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }

    void testWeightedNaryTreeWithMultipleEvidence() {
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
        gum::WeightedSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName("e"), 0);
        inf.addEvidence(bn.idFromName("b"), 1);
        inf.addEvidence(bn.idFromName("h"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_WEIGHTED);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_WEIGHTED_SIMPLE_TEST);

      } catch (gum::Exception& e) {

        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }


    void testWeightedSimpleBN() {
      auto bn = gum::BayesNet< float >::fastPrototype("a->b->c;a->d->c", 3);
      unsharpen(bn);

      try {

        gum::LazyPropagation< float > lazy(&bn);
        lazy.makeInference();
        ;
        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::WeightedSampling< float > inf(&bn);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_WEIGHTED);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_WEIGHTED_SIMPLE_TEST);

      } catch (gum::Exception& e) {

        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }

      try {

        gum::LazyPropagation< float > lazy(&bn);
        lazy.addEvidence(bn.idFromName("a"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::WeightedSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName("a"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_WEIGHTED);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_WEIGHTED_SIMPLE_TEST);


      } catch (gum::Exception& e) {

        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }

      try {

        gum::LazyPropagation< float > lazy(&bn);
        lazy.addEvidence(bn.idFromName("d"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::WeightedSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName("d"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_WEIGHTED);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_WEIGHTED_SIMPLE_TEST);

      } catch (gum::Exception& e) {

        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }


    void testWeightedCplxBN() {
      auto bn = gum::BayesNet< float >::fastPrototype(
        "a->d->f;b->d->g;b->e->h;c->e->g;i->j->h;c->j;x->c;x->j;", 3);
      unsharpen(bn);

      try {

        gum::LazyPropagation< float > lazy(&bn);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::WeightedSampling< float > inf(&bn);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_WEIGHTED);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_WEIGHTED_SIMPLE_TEST);

      } catch (gum::Exception& e) {

        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }

      try {

        gum::LazyPropagation< float > lazy(&bn);
        lazy.addEvidence(bn.idFromName("a"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::WeightedSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName("a"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_WEIGHTED);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_WEIGHTED_SIMPLE_TEST);

      } catch (gum::Exception& e) {

        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }

      try {

        gum::LazyPropagation< float > lazy(&bn);
        lazy.addEvidence(bn.idFromName("d"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::WeightedSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName("d"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_WEIGHTED);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_WEIGHTED_SIMPLE_TEST);

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }

    void testWeightedAsia() {

      gum::BayesNet< float >  bn;
      gum::BIFReader< float > reader(&bn, GET_RESSOURCES_PATH("asia.bif"));
      int                     nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == 0);

      try {

        gum::LazyPropagation< float > lazy(&bn);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::WeightedSampling< float > inf(&bn);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_WEIGHTED);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_WEIGHTED_SIMPLE_TEST);

      } catch (gum::Exception& e) {

        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }


    void testWeightedAlarm() {

      gum::BayesNet< float >  bn;
      gum::BIFReader< float > reader(&bn, GET_RESSOURCES_PATH("alarm.bif"));
      int                     nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == 0);

      try {
        gum::LazyPropagation< float > lazy(&bn);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::WeightedSampling< float > inf(&bn);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_WEIGHTED);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_WEIGHTED_SIMPLE_TEST);

      } catch (gum::Exception& e) {

        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }


    void testWeightedInfListener() {

      gum::BayesNet< float >  bn;
      gum::BIFReader< float > reader(&bn, GET_RESSOURCES_PATH("alarm.bif"));
      int                     nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == 0);

      gum::WeightedSampling< float > inf(&bn);
      aSimpleWeightedListener        agsl(inf);
      inf.setVerbosity(true);

      try {
        // Testing the inference
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_WEIGHTED);
        inf.makeInference();

      } catch (gum::Exception e) TS_ASSERT(false);

      TS_ASSERT_EQUALS(agsl.getNbr() * inf.periodSize(), inf.nbrIterations());
      TS_ASSERT_DIFFERS(agsl.getMess(), std::string(""));
    }


    void testEvidenceAsTargetOnCplxBN() {
      auto bn = gum::BayesNet< float >::fastPrototype(
        "a->d->f;b->d->g;b->e->h;c->e->g;i->j->h;c->j;x->c;x->j;", 3);
      unsharpen(bn);

      try {
        gum::WeightedSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName("d"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_WEIGHTED);
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
