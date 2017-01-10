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
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/loopyBeliefPropagation.h>
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

namespace gum_tests {

  class aSimpleLBPListener : public gum::ApproximationSchemeListener {
    private:
    int         __nbr;
    std::string __mess;

    public:
    aSimpleLBPListener( gum::ApproximationScheme& sch )
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

  class LoopyBeliefPropagationTestSuite : public CxxTest::TestSuite {
    public:
    void testLBPBinaryTreeWithoutEvidence() {
      auto bn = gum::BayesNet<float>::fastPrototype(
          "a->d->f;b->d->g;b->e->h;c->e;i->j->h" );

      gum::LazyPropagation<float> lazy( &bn );
      lazy.makeInference();

      gum::LoopyBeliefPropagation<float> inf( &bn );
      try {
        inf.setVerbosity( false );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }

    void testLBPBinaryTreeWithEvidenceOnRoot() {
      auto bn = gum::BayesNet<float>::fastPrototype(
          "a->d->f;b->d->g;b->e->h;c->e;i->j->h" );
      std::string ev = "b";

      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( ev ), 0 );
        lazy.makeInference();

        gum::LoopyBeliefPropagation<float> inf( &bn );
        inf.addEvidence( bn.idFromName( ev ), 0 );
        inf.setVerbosity( false );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        TS_ASSERT( false );
      }
      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( ev ), std::vector<float>{0.2, 0.8} );
        lazy.makeInference();

        gum::LoopyBeliefPropagation<float> inf( &bn );
        inf.addEvidence( bn.idFromName( ev ), std::vector<float>{0.2, 0.8} );
        inf.setVerbosity( false );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        TS_ASSERT( false );
      }
    }

    void testLBPBinaryTreeWithEvidenceOnLeaf() {
      auto bn = gum::BayesNet<float>::fastPrototype(
          "a->d->f;b->d->g;b->e->h;c->e;i->j->h" );
      std::string ev = "h";

      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( ev ), 0 );
        lazy.makeInference();

        gum::LoopyBeliefPropagation<float> inf( &bn );
        inf.addEvidence( bn.idFromName( ev ), 0 );
        inf.setVerbosity( false );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        TS_ASSERT( false );
      }
      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( ev ), std::vector<float>{0.2, 0.8} );
        lazy.makeInference();

        gum::LoopyBeliefPropagation<float> inf( &bn );
        inf.addEvidence( bn.idFromName( ev ), std::vector<float>{0.2, 0.8} );
        inf.setVerbosity( false );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        TS_ASSERT( false );
      }
    }

    void testLBPBinaryTreeWithEvidenceOnMid() {
      auto bn = gum::BayesNet<float>::fastPrototype(
          "a->d->f;b->d->g;b->e->h;c->e;i->j->h" );
      std::string ev = "e";

      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( ev ), 0 );
        lazy.makeInference();

        gum::LoopyBeliefPropagation<float> inf( &bn );
        inf.addEvidence( bn.idFromName( ev ), 0 );
        inf.setVerbosity( false );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        TS_ASSERT( false );
      }
      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( ev ), std::vector<float>{0.2, 0.8} );
        lazy.makeInference();

        gum::LoopyBeliefPropagation<float> inf( &bn );
        inf.addEvidence( bn.idFromName( ev ), std::vector<float>{0.2, 0.8} );
        inf.setVerbosity( false );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        TS_ASSERT( false );
      }
    }

    void testLBPBinaryTreeWithMultipleEvidence() {
      auto bn = gum::BayesNet<float>::fastPrototype(
          "a->d->f;b->d->g;b->e->h;c->e;i->j->h" );

      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( "e" ), 0 );
        lazy.addEvidence( bn.idFromName( "b" ), 1 );
        lazy.addEvidence( bn.idFromName( "h" ), 0 );
        lazy.makeInference();

        gum::LoopyBeliefPropagation<float> inf( &bn );
        inf.addEvidence( bn.idFromName( "e" ), 0 );
        inf.addEvidence( bn.idFromName( "b" ), 1 );
        inf.addEvidence( bn.idFromName( "h" ), 0 );
        inf.setVerbosity( false );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( "e" ), std::vector<float>{0.2, 0.8} );
        lazy.addEvidence( bn.idFromName( "b" ), 0 );
        lazy.addEvidence( bn.idFromName( "h" ), std::vector<float>{0.7, 0.3} );
        lazy.makeInference();

        gum::LoopyBeliefPropagation<float> inf( &bn );
        inf.addEvidence( bn.idFromName( "e" ), std::vector<float>{0.2, 0.8} );
        inf.addEvidence( bn.idFromName( "b" ), 0 );
        inf.addEvidence( bn.idFromName( "h" ), std::vector<float>{0.7, 0.3} );
        inf.setVerbosity( false );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        TS_ASSERT( false );
      }
    }
    void testLBPNaryTreeWithMultipleEvidence() {
      auto bn = gum::BayesNet<float>::fastPrototype(
          "a[4]->d[8]->f[3];b->d->g[5];b->e[4]->h;c->e;i[10]->j[3]->h" );

      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( "e" ), 0 );
        lazy.addEvidence( bn.idFromName( "b" ), 1 );
        lazy.addEvidence( bn.idFromName( "h" ), 0 );
        lazy.makeInference();

        gum::LoopyBeliefPropagation<float> inf( &bn );
        inf.addEvidence( bn.idFromName( "e" ), 0 );
        inf.addEvidence( bn.idFromName( "b" ), 1 );
        inf.addEvidence( bn.idFromName( "h" ), 0 );
        inf.setVerbosity( false );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
      try {
        gum::LazyPropagation<float> lazy( &bn );
        lazy.addEvidence( bn.idFromName( "e" ),
                          std::vector<float>{0.1, 0.3, 0.4, 0.7} );
        lazy.addEvidence( bn.idFromName( "b" ), 0 );
        lazy.addEvidence( bn.idFromName( "h" ), std::vector<float>{0.7, 0.3} );
        lazy.makeInference();

        gum::LoopyBeliefPropagation<float> inf( &bn );
        inf.addEvidence( bn.idFromName( "e" ),
                         std::vector<float>{0.1, 0.3, 0.4, 0.7} );
        inf.addEvidence( bn.idFromName( "b" ), 0 );
        inf.addEvidence( bn.idFromName( "h" ), std::vector<float>{0.7, 0.3} );
        inf.setVerbosity( false );
        inf.makeInference();
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        TS_ASSERT( false );
      }
    }

    private:
    template <typename GUM_SCALAR>
    void __compareInference( const gum::BayesNet<GUM_SCALAR>&         bn,
                             gum::LazyPropagation<GUM_SCALAR>&        lazy,
                             gum::LoopyBeliefPropagation<GUM_SCALAR>& inf ) {
      GUM_SCALAR err = static_cast<GUM_SCALAR>( 0 );

      for ( const auto& node : bn.nodes() ) {
        GUM_SCALAR e = lazy.posterior( node ).KL( inf.posterior( node ) );
        if ( e > err ) {
          err = e;
          GUM_TRACE( bn.variable( node ).name() );
          GUM_TRACE( lazy.posterior( node ) );
          GUM_TRACE( inf.posterior( node ) );
          GUM_TRACE_VAR( "==>" << err )
        }
      }
      TS_ASSERT_LESS_THAN( err, 1e-3 );
    }
  };
}
