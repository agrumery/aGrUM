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

      // add pairs
      tree.setTargetNodes ( db_single_target_ids, db_pair_target_ids );

      // check whether we can retrieve the correct nodes
      TS_ASSERT ( tree.targetIndex ( p23 ) == 0 );
      TS_ASSERT ( tree.targetIndex ( p32 ) == 1 );
      TS_ASSERT ( tree.targetIndex ( p01 ) == 2 );
      TS_ASSERT ( tree.targetIndex ( 0 ) == 3 );
      TS_ASSERT ( tree.targetIndex ( 2 ) == 4 );

      // check the records
      const gum::ListBase<gum::learning::CountingTreeTargetSetBox*>&
        recc3 = tree.nbRecords ();
      TS_ASSERT ( recc3.size () == 1 );
      gum::learning::CountingTreeTargetSetBox* boxsetc3 = recc3.front ();
      TS_ASSERT ( boxsetc3->nbParentRecords () == 10000 );
      
      box23 = boxsetc3->child ( 0 );
      box32 = boxsetc3->child ( 1 );
      box01 = boxsetc3->child ( 2 );
      box_0 = boxsetc3->child ( 3 );
      box_2 = boxsetc3->child ( 4 );
      TS_ASSERT ( box23 != nullptr );
      TS_ASSERT ( box32 != nullptr );
      TS_ASSERT ( box01 != nullptr );
      TS_ASSERT ( box_0 != nullptr );
      TS_ASSERT ( box_2 != nullptr );

      const std::vector<unsigned int>& recc_0 = box_0->nbRecords ();
      TS_ASSERT ( recc_0.size () == 2 );
      TS_ASSERT ( recc_0[0] == 5028 );
      TS_ASSERT ( recc_0[1] == 4972 );

      const std::vector<unsigned int>& recc_2 = box_2->nbRecords ();
      TS_ASSERT ( recc_2.size () == 2 );
      TS_ASSERT ( recc_2[0] == 4506 );
      TS_ASSERT ( recc_2[1] == 5494 );

      const std::vector<unsigned int>& recc_23 = box23->nbRecords ();
      TS_ASSERT ( recc_23.size () == 4 );
      TS_ASSERT ( recc_23[0] == 4445 );
      TS_ASSERT ( recc_23[1] == 5427 );
      TS_ASSERT ( recc_23[2] == 61 );
      TS_ASSERT ( recc_23[3] == 67 );

      const std::vector<unsigned int>& recc_32 = box32->nbRecords ();
      TS_ASSERT ( recc_32.size () == 4 );
      TS_ASSERT ( recc_32[0] == 4445 );
      TS_ASSERT ( recc_32[1] == 61 );
      TS_ASSERT ( recc_32[2] == 5427 );
      TS_ASSERT ( recc_32[3] == 67 );

      const std::vector<unsigned int>& recc_01 = box01->nbRecords ();
      TS_ASSERT ( recc_01.size () == 4 );
      TS_ASSERT ( recc_01[0] == 498 );
      TS_ASSERT ( recc_01[1] == 40 );
      TS_ASSERT ( recc_01[2] == 4530 );
      TS_ASSERT ( recc_01[3] == 4932 );

    }

    
   void test_conditional () {
      // read the database and create an empty tree
      gum::Database database =
        gum::Database::createFromCSV ( GET_PATH_STR( asia.csv ), ',' );
      gum::learning::CountingTree tree ( database );

      std::vector<unsigned int> db_conditioning_ids;
      std::vector<unsigned int> db_single_target_ids;
      std::vector< std::pair<unsigned int,unsigned int> > db_pair_target_ids;

      // parse single nodes in the database
      db_conditioning_ids.push_back  ( 4 );
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
      TS_ASSERT ( rec1.size () == 2 );
      gum::learning::CountingTreeTargetSetBox* boxset1 = rec1[0];
      gum::learning::CountingTreeTargetSetBox* boxset2 = rec1[1];
      
      TS_ASSERT ( boxset1->nbParentRecords () == 9876 );
      TS_ASSERT ( boxset2->nbParentRecords () == 124 );
      
      gum::learning::CountingTreeTargetBox* box31 = boxset1->child ( 0 );
      gum::learning::CountingTreeTargetBox* box32 = boxset2->child ( 0 );
      gum::learning::CountingTreeTargetBox* box11 = boxset1->child ( 1 );
      gum::learning::CountingTreeTargetBox* box12 = boxset2->child ( 1 );

      TS_ASSERT ( box31 != nullptr );
      TS_ASSERT ( box32 != nullptr );
      TS_ASSERT ( box11 != nullptr );
      TS_ASSERT ( box12 != nullptr );

      const std::vector<unsigned int>& rec31 = box31->nbRecords ();
      TS_ASSERT ( rec31.size () == 2 );
      TS_ASSERT ( rec31[0] == 9756 );
      TS_ASSERT ( rec31[1] == 120 );
 
      const std::vector<unsigned int>& rec32 = box32->nbRecords ();
      TS_ASSERT ( rec32.size () == 2 );
      TS_ASSERT ( rec32[0] == 116 );
      TS_ASSERT ( rec32[1] == 8 );

      const std::vector<unsigned int>& rec11 = box11->nbRecords ();
      TS_ASSERT ( rec11.size () == 2 );
      TS_ASSERT ( rec11[0] == 537 );
      TS_ASSERT ( rec11[1] == 9339 );

      const std::vector<unsigned int>& rec12 = box12->nbRecords ();
      TS_ASSERT ( rec12.size () == 2 );
      TS_ASSERT ( rec12[0] == 1 );
      TS_ASSERT ( rec12[1] == 123 );


      // check other nodes
      tree.setTargetNodes ( db_single_target_ids, db_pair_target_ids );

      // check whether we can retrieve the correct nodes
      TS_ASSERT ( tree.targetIndex ( 3 ) == 0 );
      TS_ASSERT ( tree.targetIndex ( 1 ) == 1 );

      // check the records
      const gum::ListBase<gum::learning::CountingTreeTargetSetBox*>&
        recq2 = tree.nbRecords ();
      TS_ASSERT ( recq2.size () == 2 );
      boxset1 = recq2[0];
      boxset2 = recq2[1];
      
      TS_ASSERT ( boxset1->nbParentRecords () == 9876 );
      TS_ASSERT ( boxset2->nbParentRecords () == 124 );
      
      box31 = boxset1->child ( 0 );
      box32 = boxset2->child ( 0 );
      box11 = boxset1->child ( 1 );
      box12 = boxset2->child ( 1 );

      TS_ASSERT ( box31 != nullptr );
      TS_ASSERT ( box32 != nullptr );
      TS_ASSERT ( box11 != nullptr );
      TS_ASSERT ( box12 != nullptr );

      const std::vector<unsigned int>& recq31 = box31->nbRecords ();
      TS_ASSERT ( recq31.size () == 2 );
      TS_ASSERT ( recq31[0] == 9756 );
      TS_ASSERT ( recq31[1] == 120 );
 
      const std::vector<unsigned int>& recq32 = box32->nbRecords ();
      TS_ASSERT ( recq32.size () == 2 );
      TS_ASSERT ( recq32[0] == 116 );
      TS_ASSERT ( recq32[1] == 8 );

      const std::vector<unsigned int>& recq11 = box11->nbRecords ();
      TS_ASSERT ( recq11.size () == 2 );
      TS_ASSERT ( recq11[0] == 537 );
      TS_ASSERT ( recq11[1] == 9339 );

      const std::vector<unsigned int>& recq12 = box12->nbRecords ();
      TS_ASSERT ( recq12.size () == 2 );
      TS_ASSERT ( recq12[0] == 1 );
      TS_ASSERT ( recq12[1] == 123 );

      // add pairs
      std::pair<unsigned int,unsigned int> p23 (2,3);
      std::pair<unsigned int,unsigned int> p32 (3,2);
      std::pair<unsigned int,unsigned int> p01 (0,1);
      db_pair_target_ids.push_back ( p23 );
      db_pair_target_ids.push_back ( p32 );
      db_pair_target_ids.push_back ( p01 );
      tree.setTargetNodes ( db_single_target_ids, db_pair_target_ids );

      // check whether we can retrieve the correct nodes
      TS_ASSERT ( tree.targetIndex ( p23 ) == 0 );
      TS_ASSERT ( tree.targetIndex ( p32 ) == 1 );
      TS_ASSERT ( tree.targetIndex ( p01 ) == 2 );
      TS_ASSERT ( tree.targetIndex ( 3 ) == 3 );
      TS_ASSERT ( tree.targetIndex ( 1 ) == 4 );

      // check the records
      const gum::ListBase<gum::learning::CountingTreeTargetSetBox*>&
        prec3 = tree.nbRecords ();
      TS_ASSERT ( prec3.size () == 2 );
      boxset1 = prec3[0];
      boxset2 = prec3[1];

      TS_ASSERT ( boxset1->nbParentRecords () == 9876 );
      TS_ASSERT ( boxset2->nbParentRecords () == 124 );

      gum::learning::CountingTreeTargetBox* box231 = boxset1->child ( 0 );
      gum::learning::CountingTreeTargetBox* box232 = boxset2->child ( 0 );
      gum::learning::CountingTreeTargetBox* box011 = boxset1->child ( 2 );
      gum::learning::CountingTreeTargetBox* box012 = boxset2->child ( 2 );

      TS_ASSERT ( box231 != nullptr );
      TS_ASSERT ( box232 != nullptr );
      TS_ASSERT ( box011 != nullptr );
      TS_ASSERT ( box012 != nullptr );

      const std::vector<unsigned int>& rec231 = box231->nbRecords ();
      TS_ASSERT ( rec231.size () == 4 );
      TS_ASSERT ( rec231[0] == 4403 );
      TS_ASSERT ( rec231[1] == 5353 );
      TS_ASSERT ( rec231[2] == 58 );
      TS_ASSERT ( rec231[3] == 62 );
     
      const std::vector<unsigned int>& rec232 = box232->nbRecords ();
      TS_ASSERT ( rec232.size () == 4 );
      TS_ASSERT ( rec232[0] == 42 );
      TS_ASSERT ( rec232[1] == 74 );
      TS_ASSERT ( rec232[2] == 3 );
      TS_ASSERT ( rec232[3] == 5 );

      const std::vector<unsigned int>& rec011 = box011->nbRecords ();
      TS_ASSERT ( rec011.size () == 4 );
      TS_ASSERT ( rec011[0] == 497 );
      TS_ASSERT ( rec011[1] == 40 );
      TS_ASSERT ( rec011[2] == 4476 );
      TS_ASSERT ( rec011[3] == 4863 );

      const std::vector<unsigned int>& rec012 = box012->nbRecords ();
      TS_ASSERT ( rec012.size () == 4 );
      TS_ASSERT ( rec012[0] == 1 );
      TS_ASSERT ( rec012[1] == 0 );
      TS_ASSERT ( rec012[2] == 54 );
      TS_ASSERT ( rec012[3] == 69 );


      // recompute from scratch
      tree.setNodes ( db_conditioning_ids,
                      db_single_target_ids, db_pair_target_ids );
      
      // check whether we can retrieve the correct nodes
      TS_ASSERT ( tree.targetIndex ( p23 ) == 0 );
      TS_ASSERT ( tree.targetIndex ( p32 ) == 1 );
      TS_ASSERT ( tree.targetIndex ( p01 ) == 2 );
      TS_ASSERT ( tree.targetIndex ( 3 ) == 3 );
      TS_ASSERT ( tree.targetIndex ( 1 ) == 4 );

      // check the records
      const gum::ListBase<gum::learning::CountingTreeTargetSetBox*>&
        pprec3 = tree.nbRecords ();
      TS_ASSERT ( pprec3.size () == 2 );
      boxset1 = pprec3[0];
      boxset2 = pprec3[1];

      TS_ASSERT ( boxset1->nbParentRecords () == 9876 );
      TS_ASSERT ( boxset2->nbParentRecords () == 124 );

      box231 = boxset1->child ( 0 );
      box232 = boxset2->child ( 0 );
      box011 = boxset1->child ( 2 );
      box012 = boxset2->child ( 2 );

      TS_ASSERT ( box231 != nullptr );
      TS_ASSERT ( box232 != nullptr );
      TS_ASSERT ( box011 != nullptr );
      TS_ASSERT ( box012 != nullptr );

      const std::vector<unsigned int>& prec231 = box231->nbRecords ();
      TS_ASSERT ( prec231.size () == 4 );
      TS_ASSERT ( prec231[0] == 4403 );
      TS_ASSERT ( prec231[1] == 5353 );
      TS_ASSERT ( prec231[2] == 58 );
      TS_ASSERT ( prec231[3] == 62 );
     
      const std::vector<unsigned int>& prec232 = box232->nbRecords ();
      TS_ASSERT ( prec232.size () == 4 );
      TS_ASSERT ( prec232[0] == 42 );
      TS_ASSERT ( prec232[1] == 74 );
      TS_ASSERT ( prec232[2] == 3 );
      TS_ASSERT ( prec232[3] == 5 );

      const std::vector<unsigned int>& prec011 = box011->nbRecords ();
      TS_ASSERT ( prec011.size () == 4 );
      TS_ASSERT ( prec011[0] == 497 );
      TS_ASSERT ( prec011[1] == 40 );
      TS_ASSERT ( prec011[2] == 4476 );
      TS_ASSERT ( prec011[3] == 4863 );

      const std::vector<unsigned int>& prec012 = box012->nbRecords ();
      TS_ASSERT ( prec012.size () == 4 );
      TS_ASSERT ( prec012[0] == 1 );
      TS_ASSERT ( prec012[1] == 0 );
      TS_ASSERT ( prec012[2] == 54 );
      TS_ASSERT ( prec012[3] == 69 );

      tree.clear ();

      // recompute from scratch
      tree.setNodes ( db_conditioning_ids,
                      db_single_target_ids, db_pair_target_ids );
      
      // check whether we can retrieve the correct nodes
      TS_ASSERT ( tree.targetIndex ( p23 ) == 0 );
      TS_ASSERT ( tree.targetIndex ( p32 ) == 1 );
      TS_ASSERT ( tree.targetIndex ( p01 ) == 2 );
      TS_ASSERT ( tree.targetIndex ( 3 ) == 3 );
      TS_ASSERT ( tree.targetIndex ( 1 ) == 4 );

      // check the records
      const gum::ListBase<gum::learning::CountingTreeTargetSetBox*>&
        ppprec3 = tree.nbRecords ();
      TS_ASSERT ( ppprec3.size () == 2 );
      boxset1 = ppprec3[0];
      boxset2 = ppprec3[1];

      TS_ASSERT ( boxset1->nbParentRecords () == 9876 );
      TS_ASSERT ( boxset2->nbParentRecords () == 124 );

      box231 = boxset1->child ( 0 );
      box232 = boxset2->child ( 0 );
      box011 = boxset1->child ( 2 );
      box012 = boxset2->child ( 2 );

      TS_ASSERT ( box231 != nullptr );
      TS_ASSERT ( box232 != nullptr );
      TS_ASSERT ( box011 != nullptr );
      TS_ASSERT ( box012 != nullptr );

      const std::vector<unsigned int>& pprec231 = box231->nbRecords ();
      TS_ASSERT ( pprec231.size () == 4 );
      TS_ASSERT ( pprec231[0] == 4403 );
      TS_ASSERT ( pprec231[1] == 5353 );
      TS_ASSERT ( pprec231[2] == 58 );
      TS_ASSERT ( pprec231[3] == 62 );
     
      const std::vector<unsigned int>& pprec232 = box232->nbRecords ();
      TS_ASSERT ( pprec232.size () == 4 );
      TS_ASSERT ( pprec232[0] == 42 );
      TS_ASSERT ( pprec232[1] == 74 );
      TS_ASSERT ( pprec232[2] == 3 );
      TS_ASSERT ( pprec232[3] == 5 );

      const std::vector<unsigned int>& pprec011 = box011->nbRecords ();
      TS_ASSERT ( pprec011.size () == 4 );
      TS_ASSERT ( pprec011[0] == 497 );
      TS_ASSERT ( pprec011[1] == 40 );
      TS_ASSERT ( pprec011[2] == 4476 );
      TS_ASSERT ( pprec011[3] == 4863 );

      const std::vector<unsigned int>& pprec012 = box012->nbRecords ();
      TS_ASSERT ( pprec012.size () == 4 );
      TS_ASSERT ( pprec012[0] == 1 );
      TS_ASSERT ( pprec012[1] == 0 );
      TS_ASSERT ( pprec012[2] == 54 );
      TS_ASSERT ( pprec012[3] == 69 );

    }
    

    void tearDown() {
      gum::learning::CountingTree::clearPools ();
    }
    
  };
  
} /* namespace gum_tests */

