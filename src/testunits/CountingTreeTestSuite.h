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
#include <agrum/learning/countingTree.h>

#include "testsuite_utils.h"


namespace gum_tests {

  class CountingTreeTestSuite: public CxxTest::TestSuite {
  public:

    void test_unconditional () {
      // read the database and create an empty tree
      gum::Database database =
        gum::Database::createFromCSV ( GET_PATH_STR( asia.csv ), ',' );
      gum::learning::CountingTree tree ( database );

      std::vector<unsigned int> db_conditioning_ids;
      std::vector<unsigned int> db_single_target_ids;
      std::vector< std::pair<unsigned int,unsigned int> > db_pair_target_ids;

      // parse single nodes in the database
      db_single_target_ids.push_back ( 3 );
      db_single_target_ids.push_back ( 1 );
      
      tree.setNodes ( db_conditioning_ids,
                      db_single_target_ids, db_pair_target_ids );

      // check whether we can retrieve the correct nodes
      TS_ASSERT ( tree.targetIndex ( 3 ) == 0 );
      TS_ASSERT ( tree.targetIndex ( 1 ) == 1 );

      // check the records
      const gum::ListBase<gum::learning::CountingTreeTargetSetBox*>&
        rec1 = tree.nbRecords ();
      TS_ASSERT ( rec1.size () == 1 );
      gum::learning::CountingTreeTargetSetBox* boxset1 = rec1.front ();
      TS_ASSERT ( boxset1->nbParentRecords () == 10000 );
      
      gum::learning::CountingTreeTargetBox* box3 = boxset1->child ( 0 );
      gum::learning::CountingTreeTargetBox* box1 = boxset1->child ( 1 );
      TS_ASSERT ( box3 != nullptr );
      TS_ASSERT ( box1 != nullptr );

      const std::vector<unsigned int>& rec31 = box3->nbRecords ();
      TS_ASSERT ( rec31.size () == 2 );
      TS_ASSERT ( rec31[0] == 9872 );
      TS_ASSERT ( rec31[1] == 128 );

      const std::vector<unsigned int>& rec11 = box1->nbRecords ();
      TS_ASSERT ( rec11.size () == 2 );
      TS_ASSERT ( rec11[0] == 538 );
      TS_ASSERT ( rec11[1] == 9462 );

      // check other nodes
      db_single_target_ids[0] = 0;
      db_single_target_ids[1] = 2;
      tree.setNodes ( db_conditioning_ids,
                      db_single_target_ids, db_pair_target_ids );

      // check whether we can retrieve the correct nodes
      TS_ASSERT ( tree.targetIndex ( 0 ) == 0 );
      TS_ASSERT ( tree.targetIndex ( 2 ) == 1 );

      // check the records
      const gum::ListBase<gum::learning::CountingTreeTargetSetBox*>&
        rec2 = tree.nbRecords ();
      TS_ASSERT ( rec2.size () == 1 );
      gum::learning::CountingTreeTargetSetBox* boxset2 = rec2.front ();
      TS_ASSERT ( boxset2->nbParentRecords () == 10000 );
      
      gum::learning::CountingTreeTargetBox* box0 = boxset2->child ( 0 );
      gum::learning::CountingTreeTargetBox* box2 = boxset2->child ( 1 );
      TS_ASSERT ( box0 != nullptr );
      TS_ASSERT ( box2 != nullptr );

      const std::vector<unsigned int>& rec02 = box0->nbRecords ();
      TS_ASSERT ( rec02.size () == 2 );
      TS_ASSERT ( rec02[0] == 5028 );
      TS_ASSERT ( rec02[1] == 4972 );

      const std::vector<unsigned int>& rec22 = box2->nbRecords ();
      TS_ASSERT ( rec22.size () == 2 );
      TS_ASSERT ( rec22[0] == 4506 );
      TS_ASSERT ( rec22[1] == 5494 );

      // add pairs
      std::pair<unsigned int,unsigned int> p23 (2,3);
      std::pair<unsigned int,unsigned int> p32 (3,2);
      std::pair<unsigned int,unsigned int> p01 (0,1);
      db_pair_target_ids.push_back ( p23 );
      db_pair_target_ids.push_back ( p32 );
      db_pair_target_ids.push_back ( p01 );
      tree.setNodes ( db_conditioning_ids,
                      db_single_target_ids, db_pair_target_ids );

     // check whether we can retrieve the correct nodes
      TS_ASSERT ( tree.targetIndex ( p23 ) == 0 );
      TS_ASSERT ( tree.targetIndex ( p32 ) == 1 );
      TS_ASSERT ( tree.targetIndex ( p01 ) == 2 );
      TS_ASSERT ( tree.targetIndex ( 0 ) == 3 );
      TS_ASSERT ( tree.targetIndex ( 2 ) == 4 );

      // check the records
      const gum::ListBase<gum::learning::CountingTreeTargetSetBox*>&
        rec3 = tree.nbRecords ();
      TS_ASSERT ( rec3.size () == 1 );
      gum::learning::CountingTreeTargetSetBox* boxset3 = rec3.front ();
      TS_ASSERT ( boxset3->nbParentRecords () == 10000 );
      
      gum::learning::CountingTreeTargetBox* box23 = boxset3->child ( 0 );
      gum::learning::CountingTreeTargetBox* box32 = boxset3->child ( 1 );
      gum::learning::CountingTreeTargetBox* box01 = boxset3->child ( 2 );
      gum::learning::CountingTreeTargetBox* box_0 = boxset3->child ( 3 );
      gum::learning::CountingTreeTargetBox* box_2 = boxset3->child ( 4 );
      TS_ASSERT ( box23 != nullptr );
      TS_ASSERT ( box32 != nullptr );
      TS_ASSERT ( box01 != nullptr );
      TS_ASSERT ( box_0 != nullptr );
      TS_ASSERT ( box_2 != nullptr );

      const std::vector<unsigned int>& rec_0 = box_0->nbRecords ();
      TS_ASSERT ( rec_0.size () == 2 );
      TS_ASSERT ( rec_0[0] == 5028 );
      TS_ASSERT ( rec_0[1] == 4972 );

      const std::vector<unsigned int>& rec_2 = box_2->nbRecords ();
      TS_ASSERT ( rec_2.size () == 2 );
      TS_ASSERT ( rec_2[0] == 4506 );
      TS_ASSERT ( rec_2[1] == 5494 );

      const std::vector<unsigned int>& rec_23 = box23->nbRecords ();
      TS_ASSERT ( rec_23.size () == 4 );
      TS_ASSERT ( rec_23[0] == 4445 );
      TS_ASSERT ( rec_23[1] == 5427 );
      TS_ASSERT ( rec_23[2] == 61 );
      TS_ASSERT ( rec_23[3] == 67 );

      const std::vector<unsigned int>& rec_32 = box32->nbRecords ();
      TS_ASSERT ( rec_32.size () == 4 );
      TS_ASSERT ( rec_32[0] == 4445 );
      TS_ASSERT ( rec_32[1] == 61 );
      TS_ASSERT ( rec_32[2] == 5427 );
      TS_ASSERT ( rec_32[3] == 67 );

      const std::vector<unsigned int>& rec_01 = box01->nbRecords ();
      TS_ASSERT ( rec_01.size () == 4 );
      TS_ASSERT ( rec_01[0] == 498 );
      TS_ASSERT ( rec_01[1] == 40 );
      TS_ASSERT ( rec_01[2] == 4530 );
      TS_ASSERT ( rec_01[3] == 4932 );

    }

    void tearDown() {
      gum::learning::CountingTree::clearPools ();
    }
    
  };
  
} /* namespace gum_tests */

