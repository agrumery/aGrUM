/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
#include <iostream>
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/hybridApproxInference.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/multidim/multiDimArray.h>
#include <agrum/variables/labelizedVariable.h>

#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/core/approximations/approximationSchemeListener.h>

#define EPSILON_FOR_HYBRID_SIMPLE_TEST 5e-2
#define EPSILON_FOR_HYBRID 5e-2


namespace gum_tests {

  class aSimpleHybridListener : public gum::ApproximationSchemeListener {
    private:
    int         __nbr;
    std::string __mess;

    public:
    aSimpleHybridListener( gum::ApproximationScheme& sch )
        : gum::ApproximationSchemeListener( sch )
        , __nbr( 0 )
        , __mess( "" ){};

    void whenProgress( const void*     buffer,
                       const gum::Size a,
                       const double    b,
                       const double    c ) {
      __nbr++;
    }
    void whenStop( const void* buffer, const std::string s ) { __mess = s; }

    int         getNbr() { return __nbr; }
    std::string getMess() { return __mess; }
  };

  class HybridApproxTestSuite : public CxxTest::TestSuite {
    public:
    void testHybridBinaryTreeWithoutEvidence() {
      auto bn = gum::BayesNet<float>::fastPrototype(
          "a->d->f;b->d->g;b->e->h;c->e;i->j->h" );

      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::HybridApproxInference<float, gum::WeightedSampling> inf( &bn );
        inf.setEpsilon( EPSILON_FOR_HYBRID );
        inf.setVerbosity( false );
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION( EPSILON_FOR_HYBRID_SIMPLE_TEST )
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }


    void testHybridBinaryTreeWithEvidenceOnRoot() {

      auto bn = gum::BayesNet<float>::fastPrototype(
          "a->d->f;b->d->g;b->e->h;c->e;i->j->h" );
      std::string ev = "b";

      try {

        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( ev ), 0 );
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::HybridApproxInference<float, gum::ImportanceSampling> inf(
            &bn );
        inf.addEvidence( bn.idFromName( ev ), 0 );
        inf.setEpsilon( EPSILON_FOR_HYBRID );
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION( EPSILON_FOR_HYBRID_SIMPLE_TEST )
      } catch ( gum::Exception& e ) {

        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }

    void testHybridBinaryTreeWithEvidenceOnLeaf() {
      auto bn = gum::BayesNet<float>::fastPrototype(
          "a->d->f;b->d->g;b->e->h;c->e;i->j->h" );
      std::string ev = "h";

      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( ev ), 0 );
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::HybridApproxInference<float, gum::MonteCarloSampling> inf(
            &bn );
        inf.addEvidence( bn.idFromName( ev ), 0 );
        inf.setEpsilon( EPSILON_FOR_HYBRID );
        inf.setVerbosity( false );
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION( EPSILON_FOR_HYBRID_SIMPLE_TEST )

      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }

    void testHybridBinaryTreeWithEvidenceOnMid() {
      auto bn = gum::BayesNet<float>::fastPrototype(
          "a->d->f;b->d->g;b->e->h;c->e;i->j->h" );
      std::string ev = "e";

      try {

        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( ev ), 0 );
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::HybridApproxInference<float, gum::WeightedSampling> inf( &bn );
        inf.addEvidence( bn.idFromName( ev ), 0 );
        inf.setEpsilon( EPSILON_FOR_HYBRID );
        inf.setVerbosity( false );
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION( EPSILON_FOR_HYBRID_SIMPLE_TEST )

      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }

    void testHybridBinaryTreeWithMultipleEvidence() {
      auto bn = gum::BayesNet<float>::fastPrototype(
          "a->d->f;b->d->g;b->e->h;c->e;i->j->h" );

      try {

        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( "e" ), 0 );
        lazy.addEvidence( bn.idFromName( "b" ), 1 );
        lazy.addEvidence( bn.idFromName( "h" ), 0 );
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::HybridApproxInference<float, gum::GibbsSampling> inf( &bn );
        inf.addEvidence( bn.idFromName( "e" ), 0 );
        inf.addEvidence( bn.idFromName( "b" ), 1 );
        inf.addEvidence( bn.idFromName( "h" ), 0 );
        inf.setEpsilon( EPSILON_FOR_HYBRID );
        inf.setVerbosity( false );
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION( EPSILON_FOR_HYBRID_SIMPLE_TEST );

      } catch ( gum::Exception& e ) {

        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }


    void testHybridNaryTreeWithMultipleEvidence() {
      auto bn = gum::BayesNet<float>::fastPrototype(
          "a[4]->d[8]->f[3];b->d->g[5];b->e[4]->h;c->e;i[10]->j[3]->h" );

      try {

        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( "e" ), 0 );
        lazy.addEvidence( bn.idFromName( "b" ), 1 );
        lazy.addEvidence( bn.idFromName( "h" ), 0 );
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::HybridApproxInference<float, gum::ImportanceSampling> inf(
            &bn );
        inf.addEvidence( bn.idFromName( "e" ), 0 );
        inf.addEvidence( bn.idFromName( "b" ), 1 );
        inf.addEvidence( bn.idFromName( "h" ), 0 );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_HYBRID );
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION( EPSILON_FOR_HYBRID_SIMPLE_TEST );

      } catch ( gum::Exception& e ) {

        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }


    void testHybridSimpleBN() {
      auto bn = gum::BayesNet<float>::fastPrototype( "a->b->c;a->d->c", 3 );

      try {

        gum::LazyPropagation<float> lazy( &bn );
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION;
        gum::HybridApproxInference<float, gum::GibbsSampling> inf( &bn );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_HYBRID );
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION( EPSILON_FOR_HYBRID_SIMPLE_TEST );

      } catch ( gum::Exception& e ) {

        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }

      try {

        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( "a" ), 0 );
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION;
        gum::HybridApproxInference<float, gum::GibbsSampling> inf( &bn );
        inf.addEvidence( bn.idFromName( "a" ), 0 );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_HYBRID );
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION( EPSILON_FOR_HYBRID_SIMPLE_TEST );

      } catch ( gum::Exception& e ) {

        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }

      try {

        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( "d" ), 0 );
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION;
        gum::HybridApproxInference<float, gum::GibbsSampling> inf( &bn );
        inf.addEvidence( bn.idFromName( "d" ), 0 );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_HYBRID );
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION( EPSILON_FOR_HYBRID_SIMPLE_TEST );

      } catch ( gum::Exception& e ) {

        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }


    void testHybridCplxBN() {
      auto bn = gum::BayesNet<float>::fastPrototype(
          "a->d->f;b->d->g;b->e->h;c->e->g;i->j->h;c->j;x->c;x->j;", 3 );

      try {

        gum::LazyPropagation<float> lazy( &bn );
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION;
        gum::HybridApproxInference<float, gum::WeightedSampling> inf( &bn );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_HYBRID );
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION( EPSILON_FOR_HYBRID_SIMPLE_TEST );

      } catch ( gum::Exception& e ) {

        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }

      try {

        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( "a" ), 0 );
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION;
        gum::HybridApproxInference<float, gum::ImportanceSampling> inf(
            &bn );
        inf.addEvidence( bn.idFromName( "a" ), 0 );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_HYBRID );
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION( EPSILON_FOR_HYBRID_SIMPLE_TEST );

      } catch ( gum::Exception& e ) {

        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }

      try {

        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( "d" ), 0 );
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION;
        gum::HybridApproxInference<float, gum::GibbsSampling> inf( &bn );
        inf.addEvidence( bn.idFromName( "d" ), 0 );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_HYBRID );
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION( EPSILON_FOR_HYBRID_SIMPLE_TEST );

      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }


    void testHybridAsia() {

      gum::BayesNet<float>  bn;
      gum::BIFReader<float> reader( &bn, GET_RESSOURCES_PATH( "asia.bif" ) );
      int                   nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING( nbrErr = reader.proceed() );
      TS_ASSERT( nbrErr == 0 );

      try {

        gum::LazyPropagation<float> lazy( &bn );
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION;
        gum::HybridApproxInference<float, gum::GibbsSampling> inf( &bn );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_HYBRID );
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION( EPSILON_FOR_HYBRID_SIMPLE_TEST );

      } catch ( gum::Exception& e ) {

        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }


    void testHybridAlarm() {

      gum::BayesNet<float>  bn;
      gum::BIFReader<float> reader( &bn, GET_RESSOURCES_PATH( "alarm.bif" ) );
      int                   nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING( nbrErr = reader.proceed() );
      TS_ASSERT( nbrErr == 0 );

      try {

        gum::LazyPropagation<float> lazy( &bn );
        lazy.makeInference();


        APPROXINFERENCE_TEST_BEGIN_ITERATION;
        gum::HybridApproxInference<float, gum::WeightedSampling> inf( &bn );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_HYBRID );
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION( EPSILON_FOR_HYBRID_SIMPLE_TEST );

      } catch ( gum::Exception& e ) {

        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }


    private:
    template <typename GUM_SCALAR, template <typename> class APPROX>
    bool __compareInference( const gum::BayesNet<GUM_SCALAR>&  bn,
                             gum::LazyPropagation<GUM_SCALAR>& lazy,
                             gum::HybridApproxInference<GUM_SCALAR, APPROX>& inf,
                             double errmax = 5e-2 ) {

      gum::Potential<float> softness, pl, pi;
      softness.fill( 1e-1 );
      GUM_SCALAR  err = static_cast<GUM_SCALAR>( 0 );
      std::string argstr = "";
      GUM_SCALAR  e;

      for ( const auto& node : bn.nodes() ) {

        if ( !inf.BN().dag().exists( node ) ) continue;

        try {

          //	e = lazy.posterior( node ).KL( inf.posterior( node ) );
          pl = ( lazy.posterior( node ) + softness ).normalize();
          pi = ( inf.posterior( node ) + softness ).normalize();
          e = pl.KL( pi );


        } catch ( gum::FatalError ) {
          // 0 in a proba
          e = std::numeric_limits<GUM_SCALAR>::infinity();

        } catch ( gum::NotFound e ) {
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

}  /// gum_tests
