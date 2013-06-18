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
#include <agrum/learning/database.h>
#include <agrum/learning/scoreBIC.h>

#include "testsuite_utils.h"

namespace gum_tests {

  class ScoreTestSuite: public CxxTest::TestSuite {
  public:
    void test_unlimited () {
      gum::Database database =
        gum::Database::createFromCSV ( GET_PATH_STR( asia.csv ), ',' );
      gum::learning::ScoreBIC score ( database );

      std::vector<unsigned int> db_conditioning_ids;
      std::vector<unsigned int> db_single_target_ids;
      std::vector< std::pair<unsigned int,unsigned int> > db_pair_target_ids;

      db_single_target_ids.push_back ( 3 );
      db_single_target_ids.push_back ( 1 );
      score.computeScores ( db_single_target_ids );

      TS_ASSERT ( fabs ( score.score ( 3 ) + 689.649 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( 1 ) + 2100.161 ) <= 0.01 );


      db_single_target_ids[0] = 0;
      db_single_target_ids[1] = 2;
      score.computeScores ( db_single_target_ids );
      TS_ASSERT ( fabs ( score.score ( 0 ) + 6935.920 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( 2 ) + 6887.190 ) <= 0.01 );

      // add pairs
      std::pair<unsigned int,unsigned int> p23 (2,3);
      std::pair<unsigned int,unsigned int> p32 (3,2);
      std::pair<unsigned int,unsigned int> p01 (0,1);
      db_pair_target_ids.clear ();
      db_pair_target_ids.push_back ( p23 );
      db_pair_target_ids.push_back ( p32 );
      db_pair_target_ids.push_back ( p01 );
      score.computeScores ( db_pair_target_ids );      

      TS_ASSERT ( fabs ( score.score ( p23 ) + 694.075  ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( p32 ) + 6891.619 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( p01 ) + 1865.902 ) <= 0.01 );

      
    }


    void tearDown() {
      gum::learning::CountingTree::clearPools ();
    }
    
  };


} /* namespace gum_tests */
