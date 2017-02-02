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
#include <agrum/BN/inference/GibbsInference.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/multidim/multiDimArray.h>
#include <agrum/variables/labelizedVariable.h>

#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/core/approximations/approximationSchemeListener.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5
#define EPSILON_FOR_SIMPLE_TEST 1e-2
#define EPSILON_FOR_BN 1e-3
namespace gum_tests {

  class aSimpleGibbsListener : public gum::ApproximationSchemeListener {
    private:
    int         __nbr;
    std::string __mess;

    public:
    aSimpleGibbsListener( gum::ApproximationScheme& sch )
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
  class GibbsInferenceTestSuite : public CxxTest::TestSuite {
    public:
    void testGibbsBinaryTreeWithoutEvidence() {
      auto bn = gum::BayesNet<float>::fastPrototype(
          "a->d->f;b->d->g;b->e->h;c->e;i->j->h" );

      gum::LazyPropagation<float> lazy( &bn );
      lazy.makeInference();

      gum::GibbsInference<float> inf( &bn );
      try {
        inf.setEpsilon( EPSILON_FOR_SIMPLE_TEST );
        inf.setVerbosity( false );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }

    void testGibbsBinaryTreeWithEvidenceOnRoot() {
      auto bn = gum::BayesNet<float>::fastPrototype(
          "a->d->f;b->d->g;b->e->h;c->e;i->j->h" );
      std::string ev = "b";

      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( ev ), 0 );
        lazy.makeInference();

        gum::GibbsInference<float> inf( &bn );
        inf.addEvidence( bn.idFromName( ev ), 0 );
        inf.setEpsilon( EPSILON_FOR_SIMPLE_TEST );
        inf.setVerbosity( false );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( ev ), std::vector<float>{0.2f, 0.8f} );
        lazy.makeInference();

        gum::GibbsInference<float> inf( &bn );
        inf.addEvidence( bn.idFromName( ev ), std::vector<float>{0.2f, 0.8f} );
        inf.setEpsilon( EPSILON_FOR_SIMPLE_TEST );
        inf.setVerbosity( false );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }

    void testGibbsBinaryTreeWithEvidenceOnLeaf() {
      auto bn = gum::BayesNet<float>::fastPrototype(
          "a->d->f;b->d->g;b->e->h;c->e;i->j->h" );
      std::string ev = "h";

      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( ev ), 0 );
        lazy.makeInference();

        gum::GibbsInference<float> inf( &bn );
        inf.addEvidence( bn.idFromName( ev ), 0 );
        inf.setEpsilon( EPSILON_FOR_SIMPLE_TEST );
        inf.setVerbosity( false );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( ev ), std::vector<float>{0.2f, 0.8f} );
        lazy.makeInference();

        gum::GibbsInference<float> inf( &bn );
        inf.addEvidence( bn.idFromName( ev ), std::vector<float>{0.2f, 0.8f} );
        inf.setEpsilon( EPSILON_FOR_SIMPLE_TEST );
        inf.setVerbosity( false );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }

    void testGibbsBinaryTreeWithEvidenceOnMid() {
      auto bn = gum::BayesNet<float>::fastPrototype(
          "a->d->f;b->d->g;b->e->h;c->e;i->j->h" );
      std::string ev = "e";

      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( ev ), 0 );
        lazy.makeInference();

        gum::GibbsInference<float> inf( &bn );
        inf.addEvidence( bn.idFromName( ev ), 0 );
        inf.setEpsilon( EPSILON_FOR_SIMPLE_TEST );
        inf.setVerbosity( false );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( ev ), std::vector<float>{0.2f, 0.8f} );
        lazy.makeInference();

        gum::GibbsInference<float> inf( &bn );
        inf.addEvidence( bn.idFromName( ev ), std::vector<float>{0.2f, 0.8f} );
        inf.setEpsilon( EPSILON_FOR_SIMPLE_TEST );
        inf.setVerbosity( false );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }

    void testGibbsBinaryTreeWithMultipleEvidence() {
      auto bn = gum::BayesNet<float>::fastPrototype(
          "a->d->f;b->d->g;b->e->h;c->e;i->j->h" );

      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( "e" ), 0 );
        lazy.addEvidence( bn.idFromName( "b" ), 1 );
        lazy.addEvidence( bn.idFromName( "h" ), 0 );
        lazy.makeInference();

        gum::GibbsInference<float> inf( &bn );
        inf.addEvidence( bn.idFromName( "e" ), 0 );
        inf.addEvidence( bn.idFromName( "b" ), 1 );
        inf.addEvidence( bn.idFromName( "h" ), 0 );
        inf.setEpsilon( EPSILON_FOR_SIMPLE_TEST );
        inf.setVerbosity( false );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( "e" ), std::vector<float>{0.2f, 0.8f} );
        lazy.addEvidence( bn.idFromName( "b" ), 0 );
        lazy.addEvidence( bn.idFromName( "h" ), std::vector<float>{0.7f, 0.3f} );
        lazy.makeInference();

        gum::GibbsInference<float> inf( &bn );
        inf.addEvidence( bn.idFromName( "e" ), std::vector<float>{0.2f, 0.8f} );
        inf.addEvidence( bn.idFromName( "b" ), 0 );
        inf.addEvidence( bn.idFromName( "h" ), std::vector<float>{0.7f, 0.3f} );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_SIMPLE_TEST );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }
    void testGibbsNaryTreeWithMultipleEvidence() {
      auto bn = gum::BayesNet<float>::fastPrototype(
          "a[4]->d[8]->f[3];b->d->g[5];b->e[4]->h;c->e;i[10]->j[3]->h" );

      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( "e" ), 0 );
        lazy.addEvidence( bn.idFromName( "b" ), 1 );
        lazy.addEvidence( bn.idFromName( "h" ), 0 );
        lazy.makeInference();

        gum::GibbsInference<float> inf( &bn );
        inf.addEvidence( bn.idFromName( "e" ), 0 );
        inf.addEvidence( bn.idFromName( "b" ), 1 );
        inf.addEvidence( bn.idFromName( "h" ), 0 );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_SIMPLE_TEST );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( "e" ),
                          std::vector<float>{0.1f, 0.3f, 0.4f, 0.7f} );
        lazy.addEvidence( bn.idFromName( "b" ), 0 );
        lazy.addEvidence( bn.idFromName( "h" ), std::vector<float>{0.7f, 0.3f} );
        lazy.makeInference();

        gum::GibbsInference<float> inf( &bn );
        inf.addEvidence( bn.idFromName( "e" ),
                         std::vector<float>{0.1f, 0.3f, 0.4f, 0.7f} );
        inf.addEvidence( bn.idFromName( "b" ), 0 );
        inf.addEvidence( bn.idFromName( "h" ), std::vector<float>{0.7f, 0.3f} );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_SIMPLE_TEST );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }

    void testGibbsSimpleBN() {
      auto bn = gum::BayesNet<float>::fastPrototype( "a->b->c;a->d->c", 3 );

      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.makeInference();

        gum::GibbsInference<float> inf( &bn );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_SIMPLE_TEST );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }

      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( "a" ), 0 );
        lazy.makeInference();

        gum::GibbsInference<float> inf( &bn );
        inf.addEvidence( bn.idFromName( "a" ), 0 );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_SIMPLE_TEST );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }

      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( "d" ), 0 );
        lazy.makeInference();

        gum::GibbsInference<float> inf( &bn );
        inf.addEvidence( bn.idFromName( "d" ), 0 );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_SIMPLE_TEST );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }

      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( "a" ),
                          std::vector<float>{0.7f, 0.3f, 1.0f} );
        lazy.makeInference();

        gum::GibbsInference<float> inf( &bn );
        inf.addEvidence( bn.idFromName( "a" ),
                         std::vector<float>{0.7f, 0.3f, 1.0f} );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_SIMPLE_TEST );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }

      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( "d" ),
                          std::vector<float>{0.7f, 0.3f, 1.0f} );
        lazy.makeInference();

        gum::GibbsInference<float> inf( &bn );
        inf.addEvidence( bn.idFromName( "d" ),
                         std::vector<float>{0.7f, 0.3f, 1.0f} );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_SIMPLE_TEST );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }

    void testGibbsCplxBN() {
      auto bn = gum::BayesNet<float>::fastPrototype(
          "a->d->f;b->d->g;b->e->h;c->e->g;i->j->h;c->j;x->c;x->j;", 3 );

      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.makeInference();

        gum::GibbsInference<float> inf( &bn );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_SIMPLE_TEST );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }

      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( "a" ), 0 );
        lazy.makeInference();

        gum::GibbsInference<float> inf( &bn );
        inf.addEvidence( bn.idFromName( "a" ), 0 );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_SIMPLE_TEST );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }

      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( "d" ), 0 );
        lazy.makeInference();

        gum::GibbsInference<float> inf( &bn );
        inf.addEvidence( bn.idFromName( "d" ), 0 );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_SIMPLE_TEST );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }

      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( "a" ),
                          std::vector<float>{0.7f, 0.3f, 1.0f} );
        lazy.makeInference();

        gum::GibbsInference<float> inf( &bn );
        inf.addEvidence( bn.idFromName( "a" ),
                         std::vector<float>{0.7f, 0.3f, 1.0f} );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_SIMPLE_TEST );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }

      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( "d" ),
                          std::vector<float>{0.7f, 0.3f, 1.0f} );
        lazy.makeInference();

        gum::GibbsInference<float> inf( &bn );
        inf.addEvidence( bn.idFromName( "d" ),
                         std::vector<float>{0.7f, 0.3f, 1.0f} );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_SIMPLE_TEST );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }

    void testGibbsAsia() {
      gum::BayesNet<float>  bn;
      gum::BIFReader<float> reader( &bn, GET_RESSOURCES_PATH( "asia.bif" ) );
      int                   nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING( nbrErr = reader.proceed() );
      TS_ASSERT( nbrErr == 0 );

      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.makeInference();

        gum::GibbsInference<float> inf( &bn );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_BN );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }

    void testGibbsAlarm() {
      gum::BayesNet<float>  bn;
      gum::BIFReader<float> reader( &bn, GET_RESSOURCES_PATH( "alarm.bif" ) );
      int                   nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING( nbrErr = reader.proceed() );
      TS_ASSERT( nbrErr == 0 );

      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.makeInference();

        gum::GibbsInference<float> inf( &bn );
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_BN );
        inf.makeInference();

        // alarm is not good for Gibbs
        __compareInference( bn, lazy, inf, 2e-1 );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }

    void testGibbsInfListener() {
      gum::BayesNet<float>  bn;
      gum::BIFReader<float> reader( &bn, GET_RESSOURCES_PATH( "alarm.bif" ) );
      int                   nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING( nbrErr = reader.proceed() );
      TS_ASSERT( nbrErr == 0 );

      gum::GibbsInference<float> inf( &bn );
      aSimpleGibbsListener       agsl( inf );
      inf.setVerbosity( true );

      try {
        // Testing the inference
        inf.setVerbosity( false );
        inf.setEpsilon( EPSILON_FOR_SIMPLE_TEST );
        inf.makeInference();
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }
      TS_ASSERT_EQUALS( agsl.getNbr() * inf.periodSize() + inf.burnIn(),
                        inf.nbrIterations() );
      TS_ASSERT_DIFFERS( agsl.getMess(), std::string( "" ) );
    }


    private:
    template <typename GUM_SCALAR>
    void __compareInference( const gum::BayesNet<GUM_SCALAR>&  bn,
                             gum::LazyPropagation<GUM_SCALAR>& lazy,
                             gum::GibbsInference<GUM_SCALAR>&  inf,
                             double                            errmax = 5e-2 ) {
      gum::Potential<GUM_SCALAR> softness, pl, pi;
      softness.fill( 1e-1);

      GUM_SCALAR  err = static_cast<GUM_SCALAR>( 0 );
      std::string argstr = "";
      GUM_SCALAR  e;

      for ( const auto& node : bn.nodes() ) {
        try {
          pl = (lazy.posterior( node ) + softness ).normalize();
          pi = ( inf.posterior( node ) + softness ).normalize();
          e = pl.KL( pi );
        } catch ( gum::FatalError ) {
          // 0 in a proba
          e = std::numeric_limits<GUM_SCALAR>::infinity();
        }
        if ( e > err ) {
          err = e;
          argstr = bn.variable( node ).name() + " (err=" + std::to_string( err ) +
                   ") : \n";
          argstr += "  lazy : " + pl.toString() + "\n";
          argstr += "  inf : " + pi.toString() + " \n";
        }
      }
      if ( err > errmax ) {
        GUM_TRACE( argstr );
      }
      TS_ASSERT_LESS_THAN( err, errmax );
    }
  };
}
