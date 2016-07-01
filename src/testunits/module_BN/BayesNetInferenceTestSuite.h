/***************************************************************************
 *   (C) 2007-2013 by Christophe GONZALES and Pierre-Henri WUILLEMIN       *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   (gumSize) 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.   *
 ***************************************************************************/
#include <iostream>
#include <string>
#include <vector>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIF/BIFReader.h>

#include <agrum/BN/inference/variableElimination.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/VEWithBB.h>

namespace gum_tests {
  int intest = 0;
  class BayesNetInferenceTestSuite : public CxxTest::TestSuite {

    public:
    void testAsia() {
      // Arrange
      std::string file = GET_RESSOURCES_PATH( "asia.bif" );
      gum::BayesNet<double> bn;
      gum::BIFReader<double> reader( &bn, file );
      TS_GUM_ASSERT_THROWS_NOTHING( reader.proceed() );
      gum::VariableElimination<double> ve( bn );
      gum::ShaferShenoyInference<double> ss( bn );
      gum::VEWithBB<double> vebb( bn );
      gum::LazyPropagation<double> lazy( bn );
      gum::Potential<double> p_ve, p_ss, p_vebb, p_lazy;
      for ( auto var_id : bn.nodes() ) {
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING( p_ve = ve.posterior( var_id ) );
        TS_GUM_ASSERT_THROWS_NOTHING( p_ss = ss.posterior( var_id ) );
        TS_GUM_ASSERT_THROWS_NOTHING( p_vebb = vebb.posterior( var_id ) );
        TS_GUM_ASSERT_THROWS_NOTHING( p_lazy = lazy.posterior( var_id ) );
        // Assert
        TS_ASSERT_EQUALS( p_ve.domainSize(), p_ss.domainSize() );
        TS_ASSERT_EQUALS( p_ve.domainSize(), p_vebb.domainSize() );
        TS_ASSERT_EQUALS( p_ve.domainSize(), p_lazy.domainSize() );

        gum::Instantiation i_ve( p_ve );
        for ( gum::Instantiation i( p_ve ); not i.end(); i.inc() ) {
          TS_ASSERT_DELTA( p_ve[i], p_ss[i], 1e-6 );
          TS_ASSERT_DELTA( p_ve[i], p_vebb[i], 1e-6 );
          TS_ASSERT_DELTA( p_ve[i], p_lazy[i], 1e-6 );
        }
      }
    }

    void testAsiaWithHardEvidence() {
      // Arrange
      std::string file = GET_RESSOURCES_PATH( "asia.bif" );
      gum::BayesNet<double> bn;
      gum::BIFReader<double> reader( &bn, file );
      TS_GUM_ASSERT_THROWS_NOTHING( reader.proceed() );
      gum::VariableElimination<double> ve( bn );
      gum::ShaferShenoyInference<double> ss( bn );
      gum::VEWithBB<double> vebb( bn );
      gum::LazyPropagation<double> lazy( bn );
      gum::Potential<double> p_ve, p_ss, p_vebb, p_lazy;
      auto e_id = bn.idFromName( "bronchitis?" );
      auto inf_list =
          std::vector<gum::BayesNetInference<double>*>{&ve, &ss, &vebb, &lazy};
      for ( auto inf: inf_list ) {
        inf->addHardEvidence( e_id, 0 );
      }
      for ( auto var_id : bn.nodes() ) {
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING( p_ve = ve.posterior( var_id ) );
        TS_GUM_ASSERT_THROWS_NOTHING( p_ss = ss.posterior( var_id ) );
        TS_GUM_ASSERT_THROWS_NOTHING( p_vebb = vebb.posterior( var_id ) );
        TS_GUM_ASSERT_THROWS_NOTHING( p_lazy = lazy.posterior( var_id ) );
        // Assert
        TS_ASSERT_EQUALS( p_ve.domainSize(), p_ss.domainSize() );
        TS_ASSERT_EQUALS( p_ve.domainSize(), p_vebb.domainSize() );
        TS_ASSERT_EQUALS( p_ve.domainSize(), p_lazy.domainSize() );

        gum::Instantiation i_ve( p_ve );
        for ( gum::Instantiation i( p_ve ); not i.end(); i.inc() ) {
          TS_ASSERT_DELTA( p_ve[i], p_ss[i], 1e-6 );
          TS_ASSERT_DELTA( p_ve[i], p_vebb[i], 1e-6 );
          TS_ASSERT_DELTA( p_ve[i], p_lazy[i], 1e-6 );
        }
      }
    }

    void testAsiaWithSoftEvidence() {
      // Arrange
      std::string file = GET_RESSOURCES_PATH( "asia.bif" );
      gum::BayesNet<double> bn;
      gum::BIFReader<double> reader( &bn, file );
      TS_GUM_ASSERT_THROWS_NOTHING( reader.proceed() );
      gum::VariableElimination<double> ve( bn );
      gum::ShaferShenoyInference<double> ss( bn );
      gum::VEWithBB<double> vebb( bn );
      gum::LazyPropagation<double> lazy( bn );
      gum::Potential<double> p_ve, p_ss, p_vebb, p_lazy;
      auto e_id = bn.idFromName( "bronchitis?" );
      auto e_p = gum::Potential<double>();
      e_p.add( bn.variable( e_id ) );
      e_p.fillWith( std::vector<double>{0.1, 0.9} );
      gum::List<const gum::Potential<double>*> list;
      list.insert( &e_p );
      auto inf_list =
          std::vector<gum::BayesNetInference<double>*>{&ve, &ss, &vebb, &lazy};
      for ( auto inf: inf_list ) {
        inf->insertEvidence( list );
      }
      for ( auto var_id : bn.nodes() ) {
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING( p_ve = ve.posterior( var_id ) );
        TS_GUM_ASSERT_THROWS_NOTHING( p_ss = ss.posterior( var_id ) );
        TS_GUM_ASSERT_THROWS_NOTHING( p_vebb = vebb.posterior( var_id ) );
        TS_GUM_ASSERT_THROWS_NOTHING( p_lazy = lazy.posterior( var_id ) );
        // Assert
        TS_ASSERT_EQUALS( p_ve.domainSize(), p_ss.domainSize() );
        TS_ASSERT_EQUALS( p_ve.domainSize(), p_vebb.domainSize() );
        TS_ASSERT_EQUALS( p_ve.domainSize(), p_lazy.domainSize() );

        gum::Instantiation i_ve( p_ve );
        for ( gum::Instantiation i( p_ve ); not i.end(); i.inc() ) {
          TS_ASSERT_DELTA( p_ve[i], p_ss[i], 1e-6 );
          TS_ASSERT_DELTA( p_ve[i], p_vebb[i], 1e-6 );
          TS_ASSERT_DELTA( p_ve[i], p_lazy[i], 1e-6 );
        }
      }
    }

    void testAlarm() {
      // Arrange
      std::string file = GET_RESSOURCES_PATH( "alarm.bif" );
      gum::BayesNet<double> bn;
      gum::BIFReader<double> reader( &bn, file );
      TS_GUM_ASSERT_THROWS_NOTHING( reader.proceed() );
      gum::VariableElimination<double> ve( bn );
      gum::ShaferShenoyInference<double> ss( bn );
      gum::VEWithBB<double> vebb( bn );
      gum::LazyPropagation<double> lazy( bn );
      gum::Potential<double> p_ve, p_ss, p_vebb, p_lazy;
      for ( auto var_id : bn.nodes() ) {
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING( p_ve = ve.posterior( var_id ) );
        TS_GUM_ASSERT_THROWS_NOTHING( p_ss = ss.posterior( var_id ) );
        TS_GUM_ASSERT_THROWS_NOTHING( p_vebb = vebb.posterior( var_id ) );
        TS_GUM_ASSERT_THROWS_NOTHING( p_lazy = lazy.posterior( var_id ) );
        // Assert
        TS_ASSERT_EQUALS( p_ve.domainSize(), p_ss.domainSize() );
        TS_ASSERT_EQUALS( p_ve.domainSize(), p_vebb.domainSize() );
        TS_ASSERT_EQUALS( p_ve.domainSize(), p_lazy.domainSize() );
        for ( gum::Instantiation i( p_ve ); not i.end(); i.inc() ) {
          TS_ASSERT_DELTA( p_ve[i], p_ss[i], 1e-6 );
          TS_ASSERT_DELTA( p_ve[i], p_vebb[i], 1e-6 );
          TS_ASSERT_DELTA( p_ve[i], p_lazy[i], 1e-6 );
        }
      }
    }

    void testAlarmWithHardEvidence() {
      // Arrange
      std::string file = GET_RESSOURCES_PATH( "alarm.bif" );
      gum::BayesNet<double> bn;
      gum::BIFReader<double> reader( &bn, file );
      TS_GUM_ASSERT_THROWS_NOTHING( reader.proceed() );
      gum::VariableElimination<double> ve( bn );
      gum::ShaferShenoyInference<double> ss( bn );
      gum::VEWithBB<double> vebb( bn );
      gum::LazyPropagation<double> lazy( bn );
      gum::Potential<double> p_ve, p_ss, p_vebb, p_lazy;
      auto e_id = bn.idFromName( "CATECHOL" );
      auto inf_list =
          std::vector<gum::BayesNetInference<double>*>{&ve, &ss, &vebb, &lazy};
      for ( auto inf: inf_list ) {
        inf->addHardEvidence( e_id, 0 );
      }
      for ( auto var_id : bn.nodes() ) {
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING( p_ve = ve.posterior( var_id ) );
        TS_GUM_ASSERT_THROWS_NOTHING( p_ss = ss.posterior( var_id ) );
        TS_GUM_ASSERT_THROWS_NOTHING( p_vebb = vebb.posterior( var_id ) );
        TS_GUM_ASSERT_THROWS_NOTHING( p_lazy = lazy.posterior( var_id ) );
        // Assert
        TS_ASSERT_EQUALS( p_ve.domainSize(), p_ss.domainSize() );
        TS_ASSERT_EQUALS( p_ve.domainSize(), p_vebb.domainSize() );
        TS_ASSERT_EQUALS( p_ve.domainSize(), p_lazy.domainSize() );
        for ( gum::Instantiation i( p_ve ); not i.end(); i.inc() ) {
          TS_ASSERT_DELTA( p_ve[i], p_ss[i], 1e-6 );
          TS_ASSERT_DELTA( p_ve[i], p_vebb[i], 1e-6 );
          TS_ASSERT_DELTA( p_ve[i], p_lazy[i], 1e-6 );
        }
      }
    }

    void testAlarmWithSoftEvidence() {
      // Arrange
      std::string file = GET_RESSOURCES_PATH( "alarm.bif" );
      gum::BayesNet<double> bn;
      gum::BIFReader<double> reader( &bn, file );
      TS_GUM_ASSERT_THROWS_NOTHING( reader.proceed() );
      gum::VariableElimination<double> ve( bn );
      gum::ShaferShenoyInference<double> ss( bn );
      gum::VEWithBB<double> vebb( bn );
      gum::LazyPropagation<double> lazy( bn );
      gum::Potential<double> p_ve, p_ss, p_vebb, p_lazy;
      auto e_id = bn.idFromName( "CATECHOL" );
      auto e_p = gum::Potential<double>();
      e_p.add( bn.variable( e_id ) );
      e_p.fillWith( std::vector<double>{0.1, 0.9} );
      gum::List<const gum::Potential<double>*> list;
      list.insert( &e_p );
      auto inf_list =
          std::vector<gum::BayesNetInference<double>*>{&ve, &ss, &vebb, &lazy};
      for ( auto inf: inf_list ) {
        inf->insertEvidence( list );
      }
      for ( auto var_id : bn.nodes() ) {
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING( p_ve = ve.posterior( var_id ) );
        TS_GUM_ASSERT_THROWS_NOTHING( p_ss = ss.posterior( var_id ) );
        TS_GUM_ASSERT_THROWS_NOTHING( p_vebb = vebb.posterior( var_id ) );
        TS_GUM_ASSERT_THROWS_NOTHING( p_lazy = lazy.posterior( var_id ) );
        // Assert
        TS_ASSERT_EQUALS( p_ve.domainSize(), p_ss.domainSize() );
        TS_ASSERT_EQUALS( p_ve.domainSize(), p_vebb.domainSize() );
        TS_ASSERT_EQUALS( p_ve.domainSize(), p_lazy.domainSize() );

        gum::Instantiation i_ve( p_ve );
        for ( gum::Instantiation i( p_ve ); not i.end(); i.inc() ) {
          TS_ASSERT_DELTA( p_ve[i], p_ss[i], 1e-6 );
          TS_ASSERT_DELTA( p_ve[i], p_vebb[i], 1e-6 );
          TS_ASSERT_DELTA( p_ve[i], p_lazy[i], 1e-6 );
        }
      }
    }
  };

}  // tests
