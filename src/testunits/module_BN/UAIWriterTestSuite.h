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
#include <sys/stat.h>

#include <iostream>
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/UAI/UAIWriter.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class UAIWriterTestSuite : public CxxTest::TestSuite {
  public:
    gum::BayesNet<double>* bn;
    gum::Id i1, i2, i3, i4, i5;

    void setUp() {
      bn = new gum::BayesNet<double>();

      gum::LabelizedVariable n1( "1", "", 2 ), n2( "2", "", 2 ),
              n3( "3", "", 2 );
      gum::LabelizedVariable n4( "4", "", 2 ), n5( "5", "", 3 );

      i1 = bn->add( n1 );
      i2 = bn->add( n2 );
      i3 = bn->add( n3 );
      i4 = bn->add( n4 );
      i5 = bn->add( n5 );

      bn->addArc( i1, i3 );
      bn->addArc( i1, i4 );
      bn->addArc( i3, i5 );
      bn->addArc( i4, i5 );
      bn->addArc( i2, i4 );
      bn->addArc( i2, i5 );

      fill( *bn );
    }

    void tearDown() { delete bn; }

    void testConstuctor() {
      gum::UAIWriter<double>* writer = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING( writer =
                                            new gum::UAIWriter<double>() );
      delete writer;
    }

    void testWriter_ostream() {
      gum::UAIWriter<double> writer;
      // Uncomment this to check the ouput
      //TS_GUM_ASSERT_THROWS_NOTHING(writer.write(std::cerr, *bn));
    }

    void testWriter_string() {
      gum::UAIWriter<double> writer;
      std::string file = GET_RESSOURCES_PATH( "uai/BNUAIIO_file.uai" );
      TS_GUM_ASSERT_THROWS_NOTHING( writer.write( file, *bn ) );

      file = GET_RESSOURCES_PATH( "uai/BNUAIIO_RO_file.uai" );

      TS_ASSERT_EQUALS( chmod( file.c_str(), 0444 ), 0 );

      try {
        writer.write( file, *bn );
        TS_ASSERT( false );
      } catch ( gum::IOError& e ) {
        TS_ASSERT( true );
      }
    }

  private:
    // Builds a BN to test the inference
    void fill( gum::BayesNet<double>& bn ) {
      const gum::Potential<double>& p1 = bn.cpt( i1 );
      {
        // FILLING PARAMS
        const double t[2] = {0.2, 0.8};
        int n = 2;
        const std::vector<double> v( t, t + n );
        p1.fillWith( v );
      }

      const gum::Potential<double>& p2 = bn.cpt( i2 );
      {
        // FILLING PARAMS
        const double t[2] = {0.3, 0.7};
        int n = 2;
        const std::vector<double> v( t, t + n );
        p2.fillWith( v );
      }

      const gum::Potential<double>& p3 = bn.cpt( i3 );
      {
        // FILLING PARAMS
        const double t[4] = {0.1, 0.9, 0.9, 0.1};
        int n = 4;
        const std::vector<double> v( t, t + n );
        p3.fillWith( v );
      }

      const gum::Potential<double>& p4 = bn.cpt( i4 );
      {
        // FILLING PARAMS
        const double t[8] = {0.4, 0.6, 0.5, 0.5, 0.5, 0.5, 1.0, 0.0};
        int n = 8;
        const std::vector<double> v( t, t + n );
        p4.fillWith( v );
      }

      const gum::Potential<double>& p5 = bn.cpt( i5 );
      {
        // FILLING PARAMS
        const double t[24] = {0.3, 0.6, 0.1, 0.5, 0.5, 0.0, 0.5, 0.5,
                              0.0, 1.0, 0.0, 0.0, 0.4, 0.6, 0.0, 0.5,
                              0.5, 0.0, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0};
        int n = 24;
        const std::vector<double> v( t, t + n );
        p5.fillWith( v );
      }
    }
  };
}