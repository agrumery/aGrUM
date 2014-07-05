/***************************************************************************
 *   Copyright (C) 2007 by Lionel Torti                                    *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <iostream>
#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/learning/BNLearner.h>

#include <agrum/core/algorithms/approximationScheme/approximationSchemeListener.h>


namespace gum_tests {

  class aSimpleBNLeanerListener : public gum::ApproximationSchemeListener {
  private:
    int __nbr;
    std::string __mess;
  public:
    aSimpleBNLeanerListener ( gum::IApproximationSchemeConfiguration& sch ) : gum::ApproximationSchemeListener ( sch ), __nbr ( 0 ), __mess ( "" ) {};
    void whenProgress ( const void* buffer, gum::Size a, double b, double c ) {
      __nbr++;
    }
    void whenStop ( const void* buffer, std::string s ) {
      __mess = s;
    }

    int getNbr() {
      return __nbr;
    }
    std::string getMess() {
      return __mess;
    }
  };


  class BNLearnerTestSuite: public CxxTest::TestSuite {
  public:

    void test_asia () {
      gum::learning::BNLearner learner;

      learner.useLocalSearchWithTabuList ( 100, 1 );
      learner.setMaxIndegree ( 10 );
      learner.useScoreK2 ();
      learner.useK2 ( std::vector<gum::NodeId> { 1, 5, 2, 6, 0, 3, 4, 7 } );
      // learner.addForbiddenArc ( gum::Arc (4,3) );
      // learner.addForbiddenArc ( gum::Arc (5,1) );
      // learner.addForbiddenArc ( gum::Arc (5,7) );

      learner.addMandatoryArc ( gum::Arc (2, 1) );

      learner.useAprioriSmoothing ();
      learner.setAprioriWeight ( 10 );


      gum::NodeProperty<unsigned int> partial_order {
        std::make_pair( gum::NodeId ( 0 ), 1 ),
        std::make_pair( gum::NodeId ( 3 ), 0 ),
        std::make_pair( gum::NodeId ( 1 ), 0 ) };
      //learner.setSliceOrder ( partial_order );

      try {
        gum::Timer timer;
        gum::BayesNet<float> bn = learner.learnBN ( GET_PATH_STR( "asia.csv" ) );
        std::cout << timer.step () << " : " << std::endl;
        std::cout << bn << "  " << bn.dag () << std::endl;
      }
      catch ( gum::Exception& e ) {
        GUM_SHOWERROR ( e );
      }
    }

    void test_listener() {
      gum::learning::BNLearner learner;
      aSimpleBNLeanerListener listen(learner);

      learner.setVerbosity(true);
      learner.setMaxIndegree ( 10 );
      learner.useScoreK2 ();
      learner.useK2 ( std::vector<gum::NodeId> { 1, 5, 2, 6, 0, 3, 4, 7 } );

      gum::BayesNet<double> bn = learner.learnBN<double> ( GET_PATH_STR( "asia.csv" ) );

      GUM_TRACE_VAR(listen.getNbr());
      GUM_TRACE_VAR(listen.getMess());
      GUM_TRACE_VAR(learner.messageApproximationScheme());
    }

  };


} /* namespace gum_tests */

