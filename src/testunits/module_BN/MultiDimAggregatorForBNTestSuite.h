/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}@lip6.fr   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <sstream>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/GibbsInference.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/core/exceptions.h>
#include <agrum/multidim/ICIModels/multiDimNoisyAND.h>
#include <agrum/multidim/ICIModels/multiDimNoisyORCompound.h>
#include <agrum/multidim/ICIModels/multiDimNoisyORNet.h>
#include <agrum/multidim/aggregators/amplitude.h>
#include <agrum/multidim/aggregators/and.h>
#include <agrum/multidim/aggregators/count.h>
#include <agrum/multidim/aggregators/exists.h>
#include <agrum/multidim/aggregators/forall.h>
#include <agrum/multidim/aggregators/max.h>
#include <agrum/multidim/aggregators/median.h>
#include <agrum/multidim/aggregators/min.h>
#include <agrum/multidim/aggregators/or.h>
#include <agrum/multidim/potential.h>
#include <agrum/variables/labelizedVariable.h>
#include <agrum/variables/rangeVariable.h>

namespace gum_tests {

  class MultiDimAggregratorsForBNTestSuite : public CxxTest::TestSuite {

    public:
    void testBNwithMin() {
      gum::List<gum::NodeId> idList;
      gum::BayesNet<double> bn;

      int nbr = 6;  // nbr=nbr of parents

      std::string str;
      const double t[4] = {0.1, 0.1, 0.1, 0.7};
      int n = 4;
      const std::vector<double> v( t, t + n );

      for ( int i = 0; i <= nbr; i++ ) {
        str = "";
        std::stringstream ss;
        ss << "var" << ( i + 1 );
        ss >> str;

        gum::LabelizedVariable var( str, str, 4 );

        if ( i == 0 ) {
          idList.insert( bn.add( var, new gum::aggregator::Min<double>() ) );
        } else {
          idList.insert( bn.add( var ) );
          bn.addArc( idList[i], idList[0] );
          bn.cpt( idList[i] ).fillWith( v );

          gum::Instantiation i( bn.cpt( idList[0] ) );

          gum::Idx res;

          for ( i.setFirst(); !i.end(); ++i ) {
            res = (gum::Idx)10000;  // clearly arbitraty choosen

            for ( gum::Idx j = 1; j < i.nbrDim(); j++ ) {
              if ( res > i.val( j ) ) res = i.val( j );
            }

            TS_ASSERT_EQUALS( bn.cpt( idList[0] )[i],
                              ( res == i.val( (gum::Idx)0 ) ) ? (float)1
                                                              : (float)0 );
          }
        }
      }

      {
        gum::ShaferShenoyInference<double> inf( bn );

        try {
          // Testing the inference
          inf.makeInference();
        } catch ( gum::Exception e ) {
          TS_ASSERT( false );
        }

        try {
          TS_ASSERT_EQUALS(
              inf.posterior( idList[0] ).toString(),
              "<var1:0> :: 0.468559 /<var1:1> :: 0.269297 /<var1:2> :: "
              "0.144495 /<var1:3> :: 0.117649" );
          TS_ASSERT_EQUALS( inf.posterior( idList[1] ).toString(),
                            "<var2:0> :: 0.1 /<var2:1> :: 0.1 /<var2:2> :: 0.1 "
                            "/<var2:3> :: 0.7" );
        } catch ( gum::Exception e ) {
          TS_ASSERT( false );
        }
      }

      {
        gum::LazyPropagation<double> inf( &bn );

        try {
          // Testing the inference
          inf.makeInference();
        } catch ( gum::Exception e ) {
          TS_ASSERT( false );
        }

        try {
          TS_ASSERT_EQUALS(
              inf.posterior( idList[0] ).toString(),
              "<var1:0> :: 0.468559 /<var1:1> :: 0.269297 /<var1:2> :: "
              "0.144495 /<var1:3> :: 0.117649" );
          TS_ASSERT_EQUALS( inf.posterior( idList[1] ).toString(),
                            "<var2:0> :: 0.1 /<var2:1> :: 0.1 /<var2:2> :: 0.1 "
                            "/<var2:3> :: 0.7" );
        } catch ( gum::Exception e ) {
          TS_ASSERT( false );
        }
      }
    }

    void testNoisyORNetInBN() {
      gum::BayesNet<float> bn;

      gum::LabelizedVariable cold( "Cold", "", 2 );
      gum::LabelizedVariable flu( "Flu", "", 2 );
      gum::LabelizedVariable malaria( "Malaria", "", 2 );
      gum::LabelizedVariable fever( "Fever", "", 2 );
      gum::LabelizedVariable oneMore( "OneMore", "", 2 );
      gum::LabelizedVariable oneMoreParent1( "OneMoreParent1", "", 2 );
      gum::LabelizedVariable oneMoreParent2( "OneMoreParent2", "", 2 );

      gum::NodeId idCold = bn.add( cold );
      gum::NodeId idFlu = bn.add( flu );
      gum::NodeId idMalaria = bn.add( malaria );
      gum::NodeId idFever = bn.addNoisyORNet( fever, 0.0 );
      gum::NodeId idOneMore = bn.add( oneMore );
      gum::NodeId idOneMoreParent1 = bn.add( oneMoreParent1 );
      gum::NodeId idOneMoreParent2 = bn.add( oneMoreParent2 );

      bn.addWeightedArc( idMalaria, idFever, 0.9 );
      bn.addWeightedArc( idFlu, idFever, 0.8 );
      bn.addWeightedArc( idCold, idFever, 0.4 );

      TS_ASSERT_THROWS( bn.addWeightedArc( idMalaria, idCold, 0.8 ),
                        gum::InvalidArc );

      const gum::Potential<float>& pOneMoreParent1 = bn.cpt( idOneMoreParent1 );
      pOneMoreParent1.fillWith( std::vector<float>{0.2, 0.8} );

      const gum::Potential<float>& pOneMoreParent2 = bn.cpt( idOneMoreParent2 );
      pOneMoreParent2.fillWith( std::vector<float>{0.3, 0.7} );

      bn.addArc( idOneMoreParent1, idOneMore );
      bn.addArc( idFever, idOneMore );
      bn.addArc( idOneMoreParent2, idOneMore );
      const gum::Potential<float>& pOneMore = bn.cpt( idOneMore );
      pOneMore.fillWith( std::vector<float>{0.1,
                                            0.9,
                                            0.8,
                                            0.2,
                                            0.1,
                                            0.9,
                                            0.8,
                                            0.2,
                                            0.1,
                                            0.9,
                                            0.8,
                                            0.2,
                                            0.1,
                                            0.9,
                                            0.8,
                                            0.2} );

      const gum::Potential<float>& p = bn.cpt( idFever );

      gum::Instantiation i( p );
      float witness[] = {1.0,
                         0.0,
                         0.1,
                         0.9,
                         0.2,
                         0.8,
                         0.02,
                         0.98,
                         0.6,
                         0.4,
                         0.06,
                         0.94,
                         0.12,
                         0.88,
                         0.012,
                         0.988};

      int j = 0;

      for ( i.setFirst(); !i.end(); ++i, j++ ) {
        TS_ASSERT_DELTA( p[i], witness[j], 1e-6 );
      }

      gum::LazyPropagation<float> inf_LazyProp( &bn );

      inf_LazyProp.makeInference();
    }

    void testNoisyORCompoundInBN() {
      gum::BayesNet<float> bn;

      gum::LabelizedVariable cold( "Cold", "", 2 );
      gum::LabelizedVariable flu( "Flu", "", 2 );
      gum::LabelizedVariable malaria( "Malaria", "", 2 );
      gum::LabelizedVariable fever( "Fever", "", 2 );
      gum::LabelizedVariable oneMore( "OneMore", "", 2 );
      gum::LabelizedVariable oneMoreParent1( "OneMoreParent1", "", 2 );
      gum::LabelizedVariable oneMoreParent2( "OneMoreParent2", "", 2 );

      gum::NodeId idCold = bn.add( cold );
      gum::NodeId idFlu = bn.add( flu );
      gum::NodeId idMalaria = bn.add( malaria );
      gum::NodeId idFever = bn.addNoisyOR( fever, 0.0 );
      gum::NodeId idOneMore = bn.add( oneMore );
      gum::NodeId idOneMoreParent1 = bn.add( oneMoreParent1 );
      gum::NodeId idOneMoreParent2 = bn.add( oneMoreParent2 );

      bn.addWeightedArc( idMalaria, idFever, 0.9 );
      bn.addWeightedArc( idFlu, idFever, 0.8 );
      bn.addWeightedArc( idCold, idFever, 0.4 );

      TS_ASSERT_THROWS( bn.addWeightedArc( idMalaria, idCold, 0.8 ),
                        gum::InvalidArc );

      const gum::Potential<float>& pOneMoreParent1 = bn.cpt( idOneMoreParent1 );
      // FILLING PARAMS
      pOneMoreParent1.fillWith( std::vector<float>{0.2, 0.8} );

      const gum::Potential<float>& pOneMoreParent2 = bn.cpt( idOneMoreParent2 );
      // FILLING PARAMS
      pOneMoreParent2.fillWith( std::vector<float>{0.3, 0.7} );

      bn.addArc( idOneMoreParent1, idOneMore );
      bn.addArc( idFever, idOneMore );
      bn.addArc( idOneMoreParent2, idOneMore );
      const gum::Potential<float>& pOneMore = bn.cpt( idOneMore );
      // FILLING PARAMS
      pOneMore.fillWith( std::vector<float>{0.1,
                                            0.9,
                                            0.8,
                                            0.2,
                                            0.1,
                                            0.9,
                                            0.8,
                                            0.2,
                                            0.1,
                                            0.9,
                                            0.8,
                                            0.2,
                                            0.1,
                                            0.9,
                                            0.8,
                                            0.2} );

      const gum::Potential<float>& p = bn.cpt( idFever );

      gum::Instantiation i( p );
      float witness[] = {1.0,
                         0.0,
                         0.1,
                         0.9,
                         0.2,
                         0.8,
                         0.02,
                         0.98,
                         0.6,
                         0.4,
                         0.06,
                         0.94,
                         0.12,
                         0.88,
                         0.012,
                         0.988};

      int j = 0;

      for ( i.setFirst(); !i.end(); ++i, j++ ) {
        TS_ASSERT_DELTA( p[i], witness[j], 1e-6 );
      }

      gum::LazyPropagation<float> inf_LazyProp( &bn );

      inf_LazyProp.makeInference();
    }

    void testNoisyANDInBN() {
      gum::BayesNet<float> bn;

      gum::LabelizedVariable cold( "Cold", "", 2 );
      gum::LabelizedVariable flu( "Flu", "", 2 );
      gum::LabelizedVariable malaria( "Malaria", "", 2 );
      gum::LabelizedVariable fever( "Fever", "", 2 );
      gum::LabelizedVariable oneMore( "OneMore", "", 2 );
      gum::LabelizedVariable oneMoreParent1( "OneMoreParent1", "", 2 );
      gum::LabelizedVariable oneMoreParent2( "OneMoreParent2", "", 2 );

      gum::NodeId idCold = bn.add( cold );
      gum::NodeId idFlu = bn.add( flu );
      gum::NodeId idMalaria = bn.add( malaria );
      gum::NodeId idFever = 0;
      TS_ASSERT_THROWS( idFever = bn.addNoisyAND( fever, 0.0 ),
                        gum::InvalidArgument );
      TS_GUM_ASSERT_THROWS_NOTHING( idFever = bn.addNoisyAND( fever, 0.999 ) );
      gum::NodeId idOneMore = bn.add( oneMore );
      gum::NodeId idOneMoreParent1 = bn.add( oneMoreParent1 );
      gum::NodeId idOneMoreParent2 = bn.add( oneMoreParent2 );

      bn.addWeightedArc( idMalaria, idFever, 0.9 );
      bn.addWeightedArc( idFlu, idFever, 0.8 );
      bn.addWeightedArc( idCold, idFever, 0.4 );

      TS_ASSERT_THROWS( bn.addWeightedArc( idMalaria, idCold, 0.8 ),
                        gum::InvalidArc );

      const gum::Potential<float>& pOneMoreParent1 = bn.cpt( idOneMoreParent1 );
      // FILLING PARAMS
      pOneMoreParent1.fillWith( std::vector<float>{0.2, 0.8} );

      const gum::Potential<float>& pOneMoreParent2 = bn.cpt( idOneMoreParent2 );
      // FILLING PARAMS
      pOneMoreParent2.fillWith( std::vector<float>{0.3, 0.7} );

      bn.addArc( idOneMoreParent1, idOneMore );
      bn.addArc( idFever, idOneMore );
      bn.addArc( idOneMoreParent2, idOneMore );
      const gum::Potential<float>& pOneMore = bn.cpt( idOneMore );
      // FILLING PARAMS
      pOneMore.fillWith( std::vector<float>{0.1,
                                            0.9,
                                            0.8,
                                            0.2,
                                            0.1,
                                            0.9,
                                            0.8,
                                            0.2,
                                            0.1,
                                            0.9,
                                            0.8,
                                            0.2,
                                            0.1,
                                            0.9,
                                            0.8,
                                            0.2} );

      const gum::Potential<float>& p = bn.cpt( idFever );

      gum::Instantiation i( p );
      float witness[] = {0.988012,
                         0.011988,
                         0.892108,
                         0.107892,
                         0.952048,
                         0.047952,
                         0.568432,
                         0.431568,
                         0.992008,
                         0.007992,
                         0.928072,
                         0.071928,
                         0.968032,
                         0.031968,
                         0.712288,
                         0.287712};

      int j = 0;

      for ( i.setFirst(); !i.end(); ++i, j++ ) {
        TS_ASSERT_DELTA( p[i], witness[j], 1e-6 );
      }

      gum::LazyPropagation<float> inf_LazyProp( &bn );

      inf_LazyProp.makeInference();
    }
  };
}
