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
#include <agrum/BN/inference/GibbsInference.h>

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
      gum::GibbsInference<double> gibbs( bn );
      auto var_id = bn.idFromName( "bronchitis?" );
      gum::Potential<double> p_ve, p_ss, p_vebb, p_lazy, p_gibbs;
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( p_ve = ve.posterior( var_id ) );
      TS_GUM_ASSERT_THROWS_NOTHING( p_ss = ss.posterior( var_id ) );
      TS_GUM_ASSERT_THROWS_NOTHING( p_vebb = vebb.posterior( var_id ) );
      TS_GUM_ASSERT_THROWS_NOTHING( p_lazy = lazy.posterior( var_id ) );
      TS_GUM_ASSERT_THROWS_NOTHING( p_gibbs = gibbs.posterior( var_id ) );
      // Assert
      gum::Instantiation i_ve( p_ve ), i_ss( p_ss ), i_vebb( p_vebb ),
          i_lazy( p_lazy ), i_gibbs( p_gibbs );
      i_ve.setFirst();
      i_ss.setFirst();
      i_vebb.setFirst();
      i_lazy.setFirst();
      i_gibbs.setFirst();
      TS_ASSERT_EQUALS( i_ve.domainSize(), i_ss.domainSize() );
      TS_ASSERT_EQUALS( i_ve.domainSize(), i_vebb.domainSize() );
      TS_ASSERT_EQUALS( i_ve.domainSize(), i_lazy.domainSize() );
      TS_ASSERT_EQUALS( i_ve.domainSize(), i_gibbs.domainSize() );
      for ( ; not i_ve.end(); i_ve.inc(),
                              i_ss.inc(),
                              i_vebb.inc(),
                              i_lazy.inc(),
                              i_gibbs.inc() ) {
        TS_ASSERT_DELTA( p_ve[i_ve], p_ss[i_ss], 1e-6 );
        TS_ASSERT_DELTA( p_ve[i_ve], p_vebb[i_ss], 1e-6 );
        TS_ASSERT_DELTA( p_ve[i_ve], p_lazy[i_ss], 1e-6 );
        TS_ASSERT_DELTA( p_ve[i_ve], p_gibbs[i_ss], 0.1 );
      }
    }
  };

}  // tests
