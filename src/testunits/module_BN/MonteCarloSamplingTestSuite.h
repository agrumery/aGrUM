#include <iostream>
#include <string>

#include <agrum/BN/BayesNet.h>
#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/MonteCarloSampling.h>
#include <agrum/variables/labelizedVariable.h>

#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/core/approximations/approximationSchemeListener.h>

#define EPSILON_FOR_MONTECARLO_SIMPLE_TEST 9e-2
#define EPSILON_FOR_MONTECARLO 9e-2

namespace gum_tests {

  class aSimpleMCListener : public gum::ApproximationSchemeListener {

    private:
    int         __nbr;
    std::string __mess;

    public:
    aSimpleMCListener( gum::ApproximationScheme& sch )
        : gum::ApproximationSchemeListener( sch )
        , __nbr( 0 )
        , __mess( "" ){};
    void whenProgress( const void*     buffer,
                       const gum::Size a,
                       const double    b,
                       const double    c ) {
      __nbr++;
    }
    void whenStop( const void* buffer, const std::string s ) {
      __nbr++;
      __mess = s;
    }

    int         getNbr() { return __nbr; }
    std::string getMess() { return __mess; }
  };

  class MonteCarloSamplingTestSuite : public CxxTest::TestSuite {

    public:
    void testMCBinaryTreeWithoutEvidence() {
      auto bn = gum::BayesNet<float>::fastPrototype(
          "a->d->f;b->d->g;b->e->h;c->e;i->j->h" );

      gum::LazyPropagation<float> lazy( &bn );
      lazy.makeInference();

      try {
        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::MonteCarloApproxInference<float> inf( &bn );
        inf.setEpsilon( EPSILON_FOR_MONTECARLO );
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION( EPSILON_FOR_MONTECARLO_SIMPLE_TEST )

      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }

    void testMCBinaryTreeWithEvidenceOnRoot() {

      auto bn = gum::BayesNet<float>::fastPrototype(
          "a->d->f;b->d->g;b->e->h;c->e;i->j->h" );
      std::string ev = "b";

      try {

        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( ev ), 0 );
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::MonteCarloApproxInference<float> inf( &bn );
        inf.addEvidence( bn.idFromName( ev ), 0 );
        inf.setEpsilon( EPSILON_FOR_MONTECARLO );
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION( EPSILON_FOR_MONTECARLO_SIMPLE_TEST )
      } catch ( gum::Exception& e ) {

        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }

    void testMCBinaryTreeWithEvidenceOnLeaf() {
      auto bn = gum::BayesNet<float>::fastPrototype(
          "a->d->f;b->d->g;b->e->h;c->e;i->j->h" );
      std::string ev = "h";

      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( ev ), 0 );
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::MonteCarloApproxInference<float> inf( &bn );
        inf.addEvidence( bn.idFromName( ev ), 0 );
        inf.setEpsilon( EPSILON_FOR_MONTECARLO );
        inf.setVerbosity( false );
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION( EPSILON_FOR_MONTECARLO_SIMPLE_TEST )

      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }

    void testMCBinaryTreeWithEvidenceOnMid() {
      auto bn = gum::BayesNet<float>::fastPrototype(
          "a->d->f;b->d->g;b->e->h;c->e;i->j->h" );
      std::string ev = "e";

      try {

        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( ev ), 0 );
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::MonteCarloApproxInference<float> inf( &bn );
        inf.addEvidence( bn.idFromName( ev ), 0 );
        inf.setEpsilon( EPSILON_FOR_MONTECARLO );
        inf.setVerbosity( false );
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION( EPSILON_FOR_MONTECARLO_SIMPLE_TEST )

      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }

    void testMCBinaryTreeWithMultipleEvidence() {
      auto bn = gum::BayesNet<float>::fastPrototype(
          "a->d->f;b->d->g;b->e->h;c->e;i->j->h" );

      try {

        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( "e" ), 0 );
        lazy.addEvidence( bn.idFromName( "b" ), 1 );
        lazy.addEvidence( bn.idFromName( "h" ), 0 );
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::MonteCarloApproxInference<float> inf( &bn );
        inf.addEvidence( bn.idFromName( "e" ), 0 );
        inf.addEvidence( bn.idFromName( "b" ), 1 );
        inf.addEvidence( bn.idFromName( "h" ), 0 );
        inf.setEpsilon( EPSILON_FOR_MONTECARLO );
        inf.setVerbosity( false );
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION( EPSILON_FOR_MONTECARLO_SIMPLE_TEST )

      } catch ( gum::Exception& e ) {

        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }

    void testMCNaryTreeWithMultipleEvidence() {
      auto bn = gum::BayesNet<float>::fastPrototype(
          "a[4]->d[8]->f[3];b->d->g[5];b->e[4]->h;c->e;i[10]->j[3]->h" );

      try {

        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( "e" ), 0 );
        lazy.addEvidence( bn.idFromName( "b" ), 1 );
        lazy.addEvidence( bn.idFromName( "h" ), 0 );
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::MonteCarloApproxInference<float> inf( &bn );
        inf.addEvidence( bn.idFromName( "e" ), 0 );
        inf.addEvidence( bn.idFromName( "b" ), 1 );
        inf.addEvidence( bn.idFromName( "h" ), 0 );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_MONTECARLO );
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION( EPSILON_FOR_MONTECARLO_SIMPLE_TEST )

      } catch ( gum::Exception& e ) {

        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }


    void testMCSimpleBN() {
      auto bn = gum::BayesNet<float>::fastPrototype( "a->b->c;a->d->c", 3 );

      try {

        gum::LazyPropagation<float> lazy( &bn );
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::MonteCarloApproxInference<float> inf( &bn );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_MONTECARLO );
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION( EPSILON_FOR_MONTECARLO_SIMPLE_TEST )

      } catch ( gum::Exception& e ) {

        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }

      try {

        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( "a" ), 0 );
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::MonteCarloApproxInference<float> inf( &bn );
        inf.addEvidence( bn.idFromName( "a" ), 0 );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_MONTECARLO );
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION( EPSILON_FOR_MONTECARLO_SIMPLE_TEST )

      } catch ( gum::Exception& e ) {

        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }

      try {

        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( "d" ), 0 );
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::MonteCarloApproxInference<float> inf( &bn );
        inf.addEvidence( bn.idFromName( "d" ), 0 );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_MONTECARLO );
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION( EPSILON_FOR_MONTECARLO_SIMPLE_TEST )

      } catch ( gum::Exception& e ) {

        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }


    void testMCCplxBN() {
      auto bn = gum::BayesNet<float>::fastPrototype(
          "a->d->f;b->d->g;b->e->h;c->e->g;i->j->h;c->j;x->c;x->j;", 3 );

      try {

        gum::LazyPropagation<float> lazy( &bn );
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::MonteCarloApproxInference<float> inf( &bn );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_MONTECARLO );
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION( EPSILON_FOR_MONTECARLO_SIMPLE_TEST )

      } catch ( gum::Exception& e ) {

        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }

      try {

        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( "a" ), 0 );
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::MonteCarloApproxInference<float> inf( &bn );
        inf.addEvidence( bn.idFromName( "a" ), 0 );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_MONTECARLO );
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION( EPSILON_FOR_MONTECARLO_SIMPLE_TEST )

      } catch ( gum::Exception& e ) {

        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }

      try {

        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( "d" ), 0 );
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::MonteCarloApproxInference<float> inf( &bn );
        inf.addEvidence( bn.idFromName( "d" ), 0 );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_MONTECARLO );
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION( EPSILON_FOR_MONTECARLO_SIMPLE_TEST )

      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }

    void testMCAsia() {
      gum::BayesNet<float>  bn;
      gum::BIFReader<float> reader( &bn, GET_RESSOURCES_PATH( "asia.bif" ) );
      int                   nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING( nbrErr = reader.proceed() );
      TS_ASSERT( nbrErr == 0 );

      try {

        gum::LazyPropagation<float> lazy( &bn );
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::MonteCarloApproxInference<float> inf( &bn );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_MONTECARLO );
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION( EPSILON_FOR_MONTECARLO_SIMPLE_TEST )

      } catch ( gum::Exception& e ) {

        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }


    void testMCAlarm() {
      gum::BayesNet<float>  bn;
      gum::BIFReader<float> reader( &bn, GET_RESSOURCES_PATH( "alarm.bif" ) );
      int                   nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING( nbrErr = reader.proceed() );
      TS_ASSERT( nbrErr == 0 );

      try {

        gum::LazyPropagation<float> lazy( &bn );
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::MonteCarloApproxInference<float> inf( &bn );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_MONTECARLO );
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION( EPSILON_FOR_MONTECARLO_SIMPLE_TEST )

      } catch ( gum::Exception& e ) {

        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }


    void testMCInfListener() {
      gum::BayesNet<float>  bn;
      gum::BIFReader<float> reader( &bn, GET_RESSOURCES_PATH( "alarm.bif" ) );
      int                   nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING( nbrErr = reader.proceed() );
      TS_ASSERT( nbrErr == 0 );

      gum::MonteCarloApproxInference<float> inf( &bn );
      aSimpleMCListener                     agsl( inf );
      inf.setVerbosity( true );

      try {
        // Testing the inference
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_MONTECARLO );
        inf.makeInference();

      } catch ( gum::Exception e ) TS_ASSERT( false );

      TS_ASSERT_EQUALS( agsl.getNbr() * inf.periodSize(), inf.nbrIterations() );
      TS_ASSERT_DIFFERS( agsl.getMess(), std::string( "" ) );
    }


    private:
    template <typename GUM_SCALAR>
    bool __compareInference( const gum::BayesNet<GUM_SCALAR>&            bn,
                             gum::LazyPropagation<GUM_SCALAR>&           lazy,
                             gum::MonteCarloApproxInference<GUM_SCALAR>& inf,
                             double errmax = 5e-2 ) {

      GUM_SCALAR  err = static_cast<GUM_SCALAR>( 0 );
      std::string argstr = "";

      for ( const auto& node : bn.nodes() ) {

        if ( !inf.BN().dag().exists( node ) ) continue;

        GUM_SCALAR e;
        try {

          e = lazy.posterior( node ).KL( inf.posterior( node ) );

        } catch ( gum::FatalError ) {
          // 0 in a proba
          e = std::numeric_limits<GUM_SCALAR>::infinity();

        }

        catch ( gum::NotFound e ) {
          continue;
        }

        if ( e > err ) {
          err = e;
          argstr = bn.variable( node ).name() + " (err=" + std::to_string( err ) +
                   ") : \n";
          argstr += "  lazy : " + lazy.posterior( node ).toString() + "\n";
          argstr += "  inf : " + inf.posterior( node ).toString() + " \n";
        }
      }
      return err <= errmax;
    }
  };
}
