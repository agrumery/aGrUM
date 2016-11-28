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

#include <agrum/core/exceptions.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/GibbsInference.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/multidim/ICIModels/multiDimLogit.h>
#include <agrum/multidim/potential.h>
#include <agrum/variables/labelizedVariable.h>
#include <agrum/variables/rangeVariable.h>

namespace gum_tests {

  class MultiDimLogitTestSuite : public CxxTest::TestSuite {
    public:
    void testCreationLogit() {
      gum::LabelizedVariable a( "a", "", 2 ), b( "b", "", 2 ), c( "c", "", 2 ),
          d( "d", "", 2 );
      gum::MultiDimLogit<float> p( 0.2f );

      // trying to change weight for a non cause
      TS_ASSERT_THROWS( p.causalWeight( b, 0.4f ), gum::InvalidArgument );
      TS_ASSERT_THROWS( p.causalWeight( d, 0.0f ), gum::InvalidArgument );

      // adding causes
      TS_GUM_ASSERT_THROWS_NOTHING( p << a << b << c << d );

      // doing the right stuff :)
      TS_ASSERT_EQUALS( p.toString(), "a<0,1>=logit(0.2)" );
      TS_GUM_ASSERT_THROWS_NOTHING( p.causalWeight( b, 0.4f ) );
      TS_ASSERT_EQUALS( p.toString(), "a<0,1>=logit(0.2 +0.4*b<0,1>)" );
      TS_GUM_ASSERT_THROWS_NOTHING( p.causalWeight( d, 0.7f ) );
      TS_ASSERT_EQUALS( p.toString(),
                        "a<0,1>=logit(0.2 +0.4*b<0,1> +0.7*d<0,1>)" );

      TS_ASSERT_EQUALS( p.nbrDim(), (gum::Size)4 );
      TS_ASSERT_EQUALS( p.realSize(), (gum::Size)4 );

      gum::MultiDimLogit<float> q( p );
      TS_ASSERT_EQUALS( q.toString(),
                        "a<0,1>=logit(0.2 +0.4*b<0,1> +0.7*d<0,1>)" );
      TS_ASSERT_EQUALS( p.realSize(), (gum::Size)4 );

      TS_GUM_ASSERT_THROWS_NOTHING( q.causalWeight( c, -1.3f ) );
      TS_ASSERT_EQUALS( q.toString(),
                        "a<0,1>=logit(0.2 +0.4*b<0,1> -1.3*c<0,1> +0.7*d<0,1>)" );
    }

    void testComputationInLogit() {
      // from "Pratique de la Régression Logique" / Ricco Rakotomalala / p33
      gum::RangeVariable     age( "age", "", 35, 67 );
      gum::RangeVariable     taux( "taux", "", 115, 171 );
      gum::LabelizedVariable angine( "angine", "", 2 );
      gum::LabelizedVariable coeur( "coeur", "", 0 );
      coeur.addLabel( "NON" ).addLabel( "OUI" );

      gum::MultiDimLogit<float> p( 14.4937f );
      p << coeur << age << taux
        << angine;  // taux, angine and coeur are causes of coeur
      p.causalWeight( age, -0.1256f );
      p.causalWeight( taux, -0.0636f );
      p.causalWeight( angine, 1.779f );

      gum::Instantiation i( p );

      /*for( i.setFirst(); ! i.end(); ++i ) {
        GUM_TRACE( i << " : " << p[i] );
      }*/

      std::string witness_age[] = {
          "50", "49", "46", "49", "62", "35", "67", "65", "47"};
      std::string witness_taux[] = {
          "126", "126", "144", "139", "154", "156", "160", "140", "143"};
      std::string witness_angine[] = {"1", "0", "0", "0", "1", "1", "0", "0", "0"};
      std::string witness_coeur[] = {
          "OUI", "OUI", "OUI", "OUI", "OUI", "OUI", "NON", "NON", "NON"};
      float witness_proba[] = {0.8786f,
                               0.5807f,
                               0.3912f,
                               0.3773f,
                               0.2127f,
                               0.8760f,
                               1 - 0.0163f,
                               1 - 0.0710f,
                               1 - 0.3765f};
      int nbr = 9;

      for ( int l = 0; l < nbr; l++ ) {
        try {
          i.chgVal( age, age[witness_age[l]] );
          i.chgVal( taux, taux[witness_taux[l]] );
          i.chgVal( angine, angine[witness_angine[l]] );
          i.chgVal( coeur, coeur[witness_coeur[l]] );
          TS_ASSERT_DELTA( witness_proba[l], p[i], 15e-3 );
        } catch ( gum::Exception& e ) {
          GUM_SHOWERROR( e );
        }
      }

      /*
              float witness[] =
         {0.2315f,0.7685f,0.1978f,0.8022f,0.4256f,0.5744f,0.3775f,0.6225f,0.0148f,0.9852f,0.0121f,0.9879f,0.0356f,0.9644f,0.0293f,0.9707f};

              int j = 0;
              for ( i.setFirst(); ! i.end(); ++i, j++ ) {
                TS_ASSERT_DELTA ( p[i], witness[j], 1e-3 );
              }

              gum::MultiDimLogit<float> q ( p );

              j = 0;

              for ( i.setFirst(); ! i.end(); ++i, j++ ) {
                TS_ASSERT_DELTA ( q[i], witness[j], 1e-6 );
              }*/
    }

    void /*test*/ ComputationInLogit2() {
      gum::LabelizedVariable lazy( "lazy", "", 2 );
      gum::LabelizedVariable degree( "degree", "", 2 );
      gum::LabelizedVariable motivation( "motivation", "", 2 );
      gum::LabelizedVariable requirement( "requirement", "", 2 );
      gum::LabelizedVariable competition( "competition", "", 2 );
      gum::LabelizedVariable unemployment( "unemployment", "", 2 );

      gum::MultiDimLogit<float> p( 1 - 0.0001f );
      p << unemployment << competition << requirement << motivation << degree
        << lazy;
      p.causalWeight( lazy, 0.8f );
      p.causalWeight( degree, 0.7f );
      p.causalWeight( motivation, 0.9f );
      p.causalWeight( requirement, 0.8f );
      p.causalWeight( competition, 0.9f );

      gum::Instantiation i( p );
      float              witness[] = {
          0.999880012f, 0.000119988f, 0.998920108f, 0.001079892f, 0.999520048f,
          0.000479952f, 0.995680432f, 0.004319568f, 0.998920108f, 0.001079892f,
          0.990280972f, 0.009719028f, 0.995680432f, 0.004319568f, 0.961123888f,
          0.038876112f, 0.999720028f, 0.000279972f, 0.997480252f, 0.002519748f,
          0.998880112f, 0.001119888f, 0.989921008f, 0.010078992f, 0.997480252f,
          0.002519748f, 0.977322268f, 0.022677732f, 0.989921008f, 0.010078992f,
          0.909289072f, 0.090710928f, 0.999520048f, 0.000479952f, 0.995680432f,
          0.004319568f, 0.998080192f, 0.001919808f, 0.982721728f, 0.017278272f,
          0.995680432f, 0.004319568f, 0.961123888f, 0.038876112f, 0.982721728f,
          0.017278272f, 0.844495552f, 0.155504448f, 0.998880112f, 0.001119888f,
          0.989921008f, 0.010078992f, 0.995520448f, 0.004479552f, 0.959684032f,
          0.040315968f, 0.989921008f, 0.010078992f, 0.909289072f, 0.090710928f,
          0.959684032f, 0.040315968f, 0.637156288f, 0.362843712f};

      int j = 0;

      for ( i.setFirst(); !i.end(); ++i, j++ ) {
        TS_ASSERT_DELTA( p[i], witness[j], 1e-6 );
      }

      gum::MultiDimLogit<float> q( p );

      j = 0;

      for ( i.setFirst(); !i.end(); ++i, j++ ) {
        TS_ASSERT_DELTA( q[i], witness[j], 1e-6 );
      }
    }

    void /*test*/ LogitInBN() {
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
      TS_GUM_ASSERT_THROWS_NOTHING( idFever = bn.addLogit( fever, 0.3f ) );
      gum::NodeId idOneMore = bn.add( oneMore );
      gum::NodeId idOneMoreParent1 = bn.add( oneMoreParent1 );
      gum::NodeId idOneMoreParent2 = bn.add( oneMoreParent2 );

      bn.addWeightedArc( idMalaria, idFever, 0.9f );
      bn.addWeightedArc( idFlu, idFever, 0.8f );
      bn.addWeightedArc( idCold, idFever, 0.4f );

      TS_ASSERT_THROWS( bn.addWeightedArc( idMalaria, idCold, 0.8f ),
                        gum::InvalidArc );

      const gum::Potential<float>& pOneMoreParent1 = bn.cpt( idOneMoreParent1 );
      // FILLING PARAMS
      pOneMoreParent1.fillWith( {0.2f, 0.8f} );

      const gum::Potential<float>& pOneMoreParent2 = bn.cpt( idOneMoreParent2 );
      // FILLING PARAMS
      pOneMoreParent2.fillWith( {0.3f, 0.7f} );

      bn.addArc( idOneMoreParent1, idOneMore );
      bn.addArc( idFever, idOneMore );
      bn.addArc( idOneMoreParent2, idOneMore );
      const gum::Potential<float>& pOneMore = bn.cpt( idOneMore );
      // FILLING PARAMS
      pOneMore.fillWith(  // clang-format off
                        {0.1f,0.9f,
                         0.8f,0.2f,
                         0.1f,0.9f,
                         0.8f,0.2f,
                         0.1f,0.9f,
                         0.8f,0.2f,
                         0.1f,0.9f,
                         0.8f,0.2f}); //clang-format on

      const gum::Potential<float>& p = bn.cpt( idFever );

      gum::Instantiation i( p );
      float witness[] = //clang-format off
                        {0.988012f,0.011988f,
                         0.892108f,0.107892f,
                         0.952048f,0.047952f,
                         0.568432f,0.431568f,
                         0.992008f,0.007992f,
                         0.928072f,0.071928f,
                         0.968032f,0.031968f,
                         0.712288f,0.287712f}; // clang-format off

      int j = 0;

      for ( i.setFirst(); !i.end(); ++i, j++ ) {
        TS_ASSERT_DELTA( p[i], witness[j], 1e-6 );
      }

      gum::LazyPropagation<float> inf_LazyProp( &bn );

      inf_LazyProp.makeInference();
    }
  };
}
