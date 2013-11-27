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

#include <agrum/learning/database.h>
#include <agrum/learning/scoreBIC.h>

namespace gum_tests {

  class ScoreTestSuite: public CxxTest::TestSuite {
  public:
    void test_unlimited () {
      gum::Database database =
        gum::Database::createFromCSV ( GET_PATH_STR( "asia.csv" ), ',' );
      gum::learning::ScoreBIC score ( database );

      std::vector<unsigned int> db_conditioning_ids;
      std::vector<unsigned int> db_single_target_ids;
      std::vector< std::pair<unsigned int,unsigned int> > db_pair_target_ids;

      db_single_target_ids.push_back ( 3 );
      db_single_target_ids.push_back ( 1 );
      score.computeScores ( db_single_target_ids );

      TS_ASSERT ( fabs ( score.score ( 3 ) + 994.953  ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( 1 ) + 3029.892 ) <= 0.01 );

      db_single_target_ids[0] = 0;
      db_single_target_ids[1] = 2;
      score.computeScores ( db_single_target_ids );
      TS_ASSERT ( fabs ( score.score ( 0 ) + 10006.417 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( 2 ) + 9936.115  ) <= 0.01 );

      // add pairs
      std::pair<unsigned int,unsigned int> p23 (2,3);
      std::pair<unsigned int,unsigned int> p32 (3,2);
      std::pair<unsigned int,unsigned int> p01 (0,1);
      db_pair_target_ids.clear ();
      db_pair_target_ids.push_back ( p23 );
      db_pair_target_ids.push_back ( p32 );
      db_pair_target_ids.push_back ( p01 );
      score.computeScores ( db_pair_target_ids );      

      TS_ASSERT ( fabs ( score.score ( p23 ) + 1001.338  ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( p32 ) + 9942.504 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( p01 ) + 2691.927 ) <= 0.01 );

      score.computeScores ( db_pair_target_ids );      

      TS_ASSERT ( fabs ( score.score ( p23 ) + 1001.338 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( p32 ) + 9942.504 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( p01 ) + 2691.927 ) <= 0.01 );

      
      db_conditioning_ids.clear ();
      db_conditioning_ids.push_back  ( 4 );
      db_single_target_ids.clear ();
      db_single_target_ids.push_back ( 3 );
      db_single_target_ids.push_back ( 1 );
      score.computeScores ( db_conditioning_ids, db_single_target_ids );

      TS_ASSERT ( fabs ( score.score ( 3 ) + 991.688  ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( 1 ) + 3030.851 ) <= 0.01 );      

      score.computeScores ( db_conditioning_ids, db_pair_target_ids );
      TS_ASSERT ( fabs ( score.score ( p23 ) + 1004.619 ) <= 0.01 );      
      TS_ASSERT ( fabs ( score.score ( p32 ) + 9952.828 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( p01 ) + 2700.123 ) <= 0.01 );


      db_conditioning_ids.clear ();
      db_conditioning_ids.push_back  ( 1 );
      db_conditioning_ids.push_back  ( 2 );
      db_pair_target_ids.clear ();
      std::pair<unsigned int,unsigned int> p12 (1,2);
      std::pair<unsigned int,unsigned int> p34 (3,4);
      db_pair_target_ids.push_back ( p12 );
      db_pair_target_ids.push_back ( p34 );
      score.computeScores ( db_conditioning_ids, db_pair_target_ids );
      TS_ASSERT ( fabs ( score.score ( p12 ) + 53.151 ) <= 0.01 );      
      TS_ASSERT ( fabs ( score.score ( p34 ) + 997.184 ) <= 0.01 );      


      
      score.setMaxSize ( 1 );

      
      score.computeScores ( db_conditioning_ids, db_pair_target_ids );
      TS_ASSERT ( fabs ( score.score ( p12 ) + 53.151 ) <= 0.01 );      
      TS_ASSERT ( fabs ( score.score ( p34 ) + 997.184 ) <= 0.01 );      

      db_conditioning_ids.clear ();
      db_single_target_ids.clear ();
      db_single_target_ids.push_back ( 3 );
      db_single_target_ids.push_back ( 1 );
      score.computeScores ( db_single_target_ids );
      TS_ASSERT ( fabs ( score.score ( 3 ) + 994.953  ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( 1 ) + 3029.892 ) <= 0.01 );

      db_single_target_ids[0] = 0;
      db_single_target_ids[1] = 2;
      score.computeScores ( db_single_target_ids );
      TS_ASSERT ( fabs ( score.score ( 0 ) + 10006.417 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( 2 ) + 9936.115  ) <= 0.01 );

      db_pair_target_ids.clear ();
      db_pair_target_ids.push_back ( p23 );
      db_pair_target_ids.push_back ( p32 );
      db_pair_target_ids.push_back ( p01 );
      score.computeScores ( db_pair_target_ids );      

      TS_ASSERT ( fabs ( score.score ( p23 ) + 1001.338  ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( p32 ) + 9942.504 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( p01 ) + 2691.927 ) <= 0.01 );

      score.computeScores ( db_pair_target_ids );      

      TS_ASSERT ( fabs ( score.score ( p23 ) + 1001.338 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( p32 ) + 9942.504 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( p01 ) + 2691.927 ) <= 0.01 );



      score.setMaxSize ( 100 );
       
      db_conditioning_ids.clear ();
      db_conditioning_ids.push_back  ( 1 );
      db_conditioning_ids.push_back  ( 2 );
      db_pair_target_ids.clear ();
      db_pair_target_ids.push_back ( p12 );
      db_pair_target_ids.push_back ( p34 );
      score.computeScores ( db_conditioning_ids, db_pair_target_ids );
     
      TS_ASSERT ( fabs ( score.score ( p12 ) + 53.151 ) <= 0.01 );      
      TS_ASSERT ( fabs ( score.score ( p34 ) + 997.184 ) <= 0.01 );      

      db_conditioning_ids.clear ();
      db_single_target_ids.clear ();
      db_single_target_ids.push_back ( 3 );
      db_single_target_ids.push_back ( 1 );
      score.computeScores ( db_single_target_ids );
      TS_ASSERT ( fabs ( score.score ( 3 ) + 994.953  ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( 1 ) + 3029.892 ) <= 0.01 );

      db_single_target_ids[0] = 0;
      db_single_target_ids[1] = 2;
      score.computeScores ( db_single_target_ids );
      TS_ASSERT ( fabs ( score.score ( 0 ) + 10006.417 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( 2 ) + 9936.115  ) <= 0.01 );

      db_pair_target_ids.clear ();
      db_pair_target_ids.push_back ( p23 );
      db_pair_target_ids.push_back ( p32 );
      db_pair_target_ids.push_back ( p01 );
     
      score.computeScores ( db_pair_target_ids );      

      TS_ASSERT ( fabs ( score.score ( p23 ) + 1001.338  ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( p32 ) + 9942.504 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( p01 ) + 2691.927 ) <= 0.01 );

      score.computeScores ( db_pair_target_ids );      

      TS_ASSERT ( fabs ( score.score ( p23 ) + 1001.338 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( p32 ) + 9942.504 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( p01 ) + 2691.927 ) <= 0.01 );



      score.setMaxSize ( 32 );
       
      db_conditioning_ids.clear ();
      db_conditioning_ids.push_back  ( 1 );
      db_conditioning_ids.push_back  ( 2 );
      db_pair_target_ids.clear ();
      db_pair_target_ids.push_back ( p12 );
      db_pair_target_ids.push_back ( p34 );
      score.computeScores ( db_conditioning_ids, db_pair_target_ids );
     
      TS_ASSERT ( fabs ( score.score ( p12 ) + 53.151 ) <= 0.01 );      
      TS_ASSERT ( fabs ( score.score ( p34 ) + 997.184 ) <= 0.01 );      

      db_conditioning_ids.clear ();
      db_single_target_ids.clear ();
      db_single_target_ids.push_back ( 3 );
      db_single_target_ids.push_back ( 1 );
      score.computeScores ( db_single_target_ids );
      TS_ASSERT ( fabs ( score.score ( 3 ) + 994.953  ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( 1 ) + 3029.892 ) <= 0.01 );

      db_single_target_ids[0] = 0;
      db_single_target_ids[1] = 2;
      score.computeScores ( db_single_target_ids );
      TS_ASSERT ( fabs ( score.score ( 0 ) + 10006.417 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( 2 ) + 9936.115  ) <= 0.01 );

      db_pair_target_ids.clear ();
      db_pair_target_ids.push_back ( p23 );
      db_pair_target_ids.push_back ( p32 );
      db_pair_target_ids.push_back ( p01 );
     
      score.computeScores ( db_pair_target_ids );      

      TS_ASSERT ( fabs ( score.score ( p23 ) + 1001.338  ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( p32 ) + 9942.504 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( p01 ) + 2691.927 ) <= 0.01 );

      score.computeScores ( db_pair_target_ids );      

      TS_ASSERT ( fabs ( score.score ( p23 ) + 1001.338 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( p32 ) + 9942.504 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( p01 ) + 2691.927 ) <= 0.01 );
    }


    void tearDown() {
      gum::learning::CountingTree::clearPools ();
    }
    
  };


} /* namespace gum_tests */
