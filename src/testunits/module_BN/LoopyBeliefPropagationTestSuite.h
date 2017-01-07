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
    void testFill() {
      gum::BayesNet<float> bn;
      __fillBN( bn );

      TS_ASSERT( bn.cpt( "1" ).nbrDim() == 1 );

      TS_ASSERT( bn.cpt( "2" ).nbrDim() == 1 );

      TS_ASSERT( bn.cpt( "3" ).nbrDim() == 2 );

      // CHECKING IS FOR EACH INSTANCE OF PARENTS, WE HAVE A PROBA (SUM to 1)
      auto p3 = bn.cpt( "3" ).margSumOut( {&( bn.variable( "3" ) )} );
      for ( gum::Instantiation j( p3 ); !j.end(); ++j )
        TS_ASSERT_DELTA( p3.get( j ), 1.0f, 1e-5 );

      // CHECKING IS FOR EACH INSTANCE OF PARENTS, WE HAVE A PROBA (SUM to 1)
      auto p4 = bn.cpt( "4" ).margSumOut( {&( bn.variable( "4" ) )} );
      for ( gum::Instantiation j( p4 ); !j.end(); ++j )
        TS_ASSERT_DELTA( p4.get( j ), 1.0f, 1e-5 );

      // CHECKING IS FOR EACH INSTANCE OF PARENTS, WE HAVE A PROBA (SUM to 1)
      auto p5 = bn.cpt( "5" ).margSumOut( {&( bn.variable( "5" ) )} );
      for ( gum::Instantiation j( p5 ); !j.end(); ++j ) {
        TS_ASSERT_DELTA( p5.get( j ), 1.0f, 1e-5 );
      }
    }

    // Testing when there is no evidence
    void /*test*/ LBPInf_1() {
      gum::BayesNet<float> bn;
      __fillBN( bn );
      gum::LazyPropagation<float> lazy( &bn );
      lazy.makeInference();

      gum::LoopyBeliefPropagation<float> inf( &bn );
      try {
        inf.setVerbosity( false );
        inf.makeInference();
        GUM_TRACE_VAR( inf.messageApproximationScheme() );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }

      try {
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }

    // Testing when there is no evidence
    void testLBPInf_2() {
      gum::BayesNet<float> bn;
      __fillBN2( bn );
      gum::LazyPropagation<float> lazy( &bn );
      lazy.makeInference();

      gum::LoopyBeliefPropagation<float> inf( &bn );
      try {
        inf.setVerbosity( false );
        inf.makeInference();
        GUM_TRACE_VAR( inf.messageApproximationScheme() );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }

      try {
        __compareInference( bn, lazy, inf );
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }

    void testLBPInf_alarm() {
      try {
        std::string           file = GET_RESSOURCES_PATH( "alarm.bif" );
        gum::BayesNet<float>  alarm;
        gum::BIFReader<float> reader( &alarm, file );
        reader.proceed();

        gum::LazyPropagation<float> lazy( &alarm );
        lazy.makeInference();

        gum::LoopyBeliefPropagation<float> inf( &alarm );
        inf.setVerbosity( false );
        // Testing the inference

        inf.makeInference();
        GUM_TRACE_VAR( inf.messageApproximationScheme() );

        __compareInference( alarm, lazy, inf );

      } catch ( gum::Exception e ) {
        GUM_SHOWERROR( e );
        TS_ASSERT( false );
      }
    }

    void testLBPInf_3() {
      /*
      gum::List<const gum::Potential<float>*> e_list;
      e_list.insert( e_i1 );
      e_list.insert( e_i4 );

      gum::LoopyBeliefPropagation<float> inf( bn );
      inf.setVerbosity( false );

      try {
        for ( auto pot : e_list )
          inf.addEvidence( *pot );
      } catch ( gum::Exception e ) {
        std::cerr << std::endl << e.errorContent() << std::endl;
        TS_ASSERT( false );
      }

      try {
        // Testing the inference
        inf.makeInference();
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }

      try {
        const gum::Potential<float>& posterior = inf.posterior( i1 );
        printPotential( posterior );
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }

      try {
        const gum::Potential<float>& posterior = inf.posterior( i2 );
        printPotential( posterior );
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }

      try {
        const gum::Potential<float>& posterior = inf.posterior( i3 );
        printPotential( posterior );
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }

      try {
        const gum::Potential<float>& posterior = inf.posterior( i4 );
        printPotential( posterior );
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }

      try {
        const gum::Potential<float>& posterior = inf.posterior( i5 );
        printPotential( posterior );
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }*/
    }

    void /*test*/ LBPInfListener() {
      gum::BayesNet<float> bn;
      __fillBN( bn );

      gum::Potential<float> e_i1;
      e_i1 << bn.variable( "1" );
      e_i1.fillWith( {1.0f, 0.0f} );

      gum::Potential<float> e_i4;
      e_i4 << bn.variable( "4" );
      e_i1.fillWith( {0.0f, 1.0f} );

      gum::List<const gum::Potential<float>*> e_list;
      e_list.insert( &e_i1 );
      e_list.insert( &e_i4 );

      gum::LoopyBeliefPropagation<float> inf( &bn );

      aSimpleLBPListener agsl( inf );

      try {
        // Testing the inference
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
    void __compareInference( const gum::BayesNet<GUM_SCALAR>&        bn,
                             gum::LazyPropagation<GUM_SCALAR>&       lazy,
                             gum::LoopyBeliefPropagation<GUM_SCALAR> inf ) {
      GUM_SCALAR err = static_cast<GUM_SCALAR>( 0 );

      for ( const auto& node : bn.nodes() ) {
        GUM_SCALAR e = lazy.posterior( node ).KL( inf.posterior( node ) );
        if ( e > err ) {
          err = e;
          GUM_TRACE( bn.variable( node ).name() );
          GUM_TRACE( lazy.posterior( node ) );
          GUM_TRACE( inf.posterior( node ) );
        }
      }
      TS_ASSERT_LESS_THAN( err, 1e-3 );
    }

    void __fillBN( gum::BayesNet<float>& bn ) {
      if ( bn.size() == 0 ) {  // we fill this only once
        gum::LabelizedVariable n1( "1", "", 2 ), n2( "2", "", 2 ),
            n3( "3", "", 2 );
        gum::LabelizedVariable n4( "4", "", 2 ), n5( "5", "", 3 );

        auto i1 = bn.add( n1 );
        auto i2 = bn.add( n2 );
        auto i3 = bn.add( n3 );
        auto i4 = bn.add( n4 );
        auto i5 = bn.add( n5 );

        bn.addArc( i1, i3 );
        bn.addArc( i1, i4 );
        bn.addArc( i3, i5 );
        bn.addArc( i4, i5 );
        bn.addArc( i2, i4 );
        bn.addArc( i2, i5 );

        bn.cpt( i1 ).fillWith( {0.2f, 0.8f} );
        bn.cpt( i2 ).fillWith( {0.3f, 0.7f} );
        bn.cpt( i3 ).fillWith( {0.1f, 0.9f, 0.9f, 0.1f} );
        bn.cpt( i4 ).fillWith(  // clang-format off
                  {0.4f, 0.6f,
                   0.5f, 0.5f,
                   0.5f, 0.5f,
                   1.0f, 0.0f});  // clang-format
          // on
          bn.cpt(i5).fillWith(  // clang-format off
                  {0.3f, 0.6f, 0.1f,
                   0.5f, 0.5f, 0.0f,
                   0.5f, 0.5f, 0.0f,
                   1.0f, 0.0f, 0.0f,
                   0.4f, 0.6f, 0.0f,
                   0.5f, 0.5f, 0.0f,
                   0.5f, 0.5f, 0.0f,
                   0.0f, 0.0f, 1.0f});  // clang-format on
      }
    }
    void __fillBN2( gum::BayesNet<float>& bn ) {
      if ( bn.size() == 0 ) {  // we fill this only once
        gum::LabelizedVariable n1( "1", "", 2 ), n2( "2", "", 3 ),
            n3( "3", "", 4 );
        gum::LabelizedVariable n4( "4", "", 5 ), n5( "5", "", 6 );

        auto i1 = bn.add( n1 );
        auto i2 = bn.add( n2 );
        auto i3 = bn.add( n3 );
        auto i4 = bn.add( n4 );
        auto i5 = bn.add( n5 );

        bn.addArc( i1, i3 );
        bn.addArc( i1, i4 );
        bn.addArc( i3, i5 );
        bn.addArc( i4, i5 );
        bn.addArc( i2, i4 );
        bn.addArc( i2, i5 );

        bn.generateCPTs();
      }
    }
  };
}
