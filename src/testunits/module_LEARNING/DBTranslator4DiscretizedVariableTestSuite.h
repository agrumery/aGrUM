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

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>
#include <ressources/myalloc.h>
#include <ressources/learningAlloc.h>
#include <iostream>

#include <agrum/core/thread.h>
#include <agrum/learning/database/DBTranslator4DiscretizedVariable.h>

namespace gum_tests {

  class DBTranslator4DiscretizedVariableTestSuite : public CxxTest::TestSuite {
    public:
    void test_trans1() {
      gum::DiscretizedVariable<int> var ( "X1", "" );
      var.addTick ( 1 );
      var.addTick ( 3 );
      var.addTick ( 10 );
      
      gum::learning::DBTranslator4DiscretizedVariable<MyAlloc>
        translator ( var );
      TS_GUM_ASSERT_THROWS_NOTHING( translator.translate ( "1.3" ) );
      TS_ASSERT( translator.translate ( "1.2" ).discr_val == 0 );
      TS_ASSERT( translator.translate ( "1" ).discr_val == 0 );
      TS_ASSERT( translator.translate ( "2.4" ).discr_val == 0 );
      TS_ASSERT_THROWS( translator.translate ( "0" ), gum::UnknownLabelInDatabase );
      TS_ASSERT_THROWS( translator.translate ( "11" ), gum::UnknownLabelInDatabase );
      TS_ASSERT_THROWS( translator.translate ( "aaa" ), gum::TypeError );
                        
      TS_GUM_ASSERT_THROWS_NOTHING( translator.translate ( "7" ) );
      TS_ASSERT( translator.translate ( "10" ).discr_val == 1 );
      TS_ASSERT( translator.translate ( "9" ).discr_val == 1 );
      TS_ASSERT( translator.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{0}}) == "[1;3[" );
      TS_ASSERT( translator.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{1}}) == "[3;10]" );
 
      std::vector<std::string> missing { "?", "N/A", "???" };
      gum::learning::DBTranslator4DiscretizedVariable<>
        translator2 ( var, missing );
      TS_GUM_ASSERT_THROWS_NOTHING( translator2.translate ( "1.3" ) );
      TS_GUM_ASSERT_THROWS_NOTHING( translator2.translate ( "3" ) );
      TS_ASSERT( translator2.translate ( "1" ).discr_val == 0 );
      TS_ASSERT( translator2.translate ( "3" ).discr_val == 1 );
      TS_ASSERT( translator2.translate ( "N/A" ).discr_val ==
                 std::numeric_limits<std::size_t>::max () );
      TS_ASSERT( translator2.translate ( "?" ).discr_val ==
                 std::numeric_limits<std::size_t>::max () );
      TS_ASSERT( translator2.translate ( "???" ).discr_val ==
                 std::numeric_limits<std::size_t>::max () );
      TS_ASSERT_THROWS( translator2.translate ( "??" ), gum::TypeError );
      TS_ASSERT( translator.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{0}}) == "[1;3[" );
      TS_ASSERT( translator.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{1}}) == "[3;10]" );

      gum::learning::DBTranslator4DiscretizedVariable<MyAlloc>
        translator3 ( var, missing, 3 );
      TS_GUM_ASSERT_THROWS_NOTHING( translator3.translate ( "1" ) );
      TS_GUM_ASSERT_THROWS_NOTHING( translator3.translate ( "10" ) );
      TS_ASSERT( translator3.translate ( "1" ).discr_val == 0 );
      TS_ASSERT( translator3.translate ( "9.9" ).discr_val == 1 );
      TS_ASSERT( translator3.translate ( "N/A" ).discr_val ==
                 std::numeric_limits<std::size_t>::max () );
      TS_ASSERT( translator3.translate ( "?" ).discr_val ==
                 std::numeric_limits<std::size_t>::max () );
      TS_ASSERT( translator3.translate ( "???" ).discr_val ==
                 std::numeric_limits<std::size_t>::max () );
      TS_ASSERT_THROWS( translator3.translate ( "??" ), gum::TypeError );
      TS_ASSERT_THROWS( translator3.translate ( "a" ), gum::TypeError );

      TS_ASSERT( translator3.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{0}}) == "[1;3[" );
      TS_ASSERT( translator3.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{1}}) == "[3;10]" );
      TS_ASSERT_THROWS( translator3.translateBack
                        (gum::learning::DBTranslatedValue{std::size_t{2}}), gum::UnknownLabelInDatabase );
      TS_ASSERT( translator3.translateBack
                 ( gum::learning::DBTranslatedValue{std::numeric_limits<std::size_t>::max ()}) == "?" );

      TS_ASSERT( translator3.domainSize () == 2 );

      TS_ASSERT_THROWS( gum::learning::DBTranslator4DiscretizedVariable<>
                        translator4 ( var, missing, 1 ), gum::SizeError );
    }


    void test_trans2() {
      {
      gum::DiscretizedVariable<float> var ( "X1", "" );
      var.addTick ( 1 );
      var.addTick ( 4 );
      var.addTick ( 2 );
      var.addTick ( 3 );
      
      gum::learning::DBTranslator4DiscretizedVariable<> translator ( var );
      TS_ASSERT( translator.translate ( "1" ).discr_val == 0 );
      TS_ASSERT( translator.translate ( "4" ).discr_val == 2 );
      TS_ASSERT( translator.translate ( "2" ).discr_val == 1 );
      TS_ASSERT( translator.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{0}})
                 == "[1;2[" );
      TS_ASSERT( translator.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{1}})
                 == "[2;3[" );
      TS_ASSERT( translator.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{2}})
                 == "[3;4]" );
      TS_ASSERT( translator.variable ()->toString() == "X1<[1;2[,[2;3[,[3;4]>" );

      TS_ASSERT( translator.translate ( translator.translateBack ( translator.translate ( "1" ) ) ).discr_val == 0 );
      
      gum::DiscretizedVariable<float> var2 ( "X2", "" );
      var2.addTick ( 1 );
      var2.addTick ( 2 );
      var2.addTick ( 3 );
      var2.addTick ( 4 );

      gum::learning::DBTranslator4DiscretizedVariable<MyAlloc>
        translator2 ( var2 );
      TS_ASSERT( translator2.translate ( "1" ).discr_val == 0 );
      TS_ASSERT( translator2.translate ( "2" ).discr_val == 1 );
      TS_ASSERT( translator2.translate ( "4" ).discr_val == 2 );
      TS_ASSERT( translator2.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{0}})
                 == "[1;2[" );
      TS_ASSERT( translator2.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{1}})
                 == "[2;3[" );
      TS_ASSERT( translator2.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{2}})
                 == "[3;4]" );
      TS_ASSERT( translator2.variable ()->toString() == "X2<[1;2[,[2;3[,[3;4]>" );
      
      gum::learning::DBTranslator4DiscretizedVariable<>
        translator3 ( translator );
      TS_ASSERT( translator3.translate ( "1" ).discr_val == 0 );
      TS_ASSERT( translator3.translate ( "4" ).discr_val == 2 );
      TS_ASSERT( translator3.translate ( "2" ).discr_val == 1 );
      TS_ASSERT( translator3.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{0}})
                 == "[1;2[" );
      TS_ASSERT( translator3.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{1}})
                 == "[2;3[" );
      TS_ASSERT( translator3.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{2}})
                 == "[3;4]" );
      TS_ASSERT( translator3.variable ()->toString() == "X1<[1;2[,[2;3[,[3;4]>" );

      gum::learning::DBTranslator4DiscretizedVariable<MyAlloc>
        translator4( translator2 );
      TS_ASSERT( translator4.translate ( "1" ).discr_val == 0 );
      TS_ASSERT( translator4.translate ( "2" ).discr_val == 1 );
      TS_ASSERT( translator4.translate ( "4" ).discr_val == 2 );
      TS_ASSERT( translator4.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{0}})
                 == "[1;2[" );
      TS_ASSERT( translator4.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{1}})
                 == "[2;3[" );
      TS_ASSERT( translator4.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{2}})
                 == "[3;4]" );
      TS_ASSERT( translator4.variable ()->toString() == "X2<[1;2[,[2;3[,[3;4]>" );

      gum::learning::DBTranslator4DiscretizedVariable<>
        translator5 ( std::move ( translator3 ) );
      TS_ASSERT( translator5.translate ( "1" ).discr_val == 0 );
      TS_ASSERT( translator5.translate ( "4" ).discr_val == 2 );
      TS_ASSERT( translator5.translate ( "2" ).discr_val == 1 );
      TS_ASSERT( translator5.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{0}})
                 == "[1;2[" );
      TS_ASSERT( translator5.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{1}})
                 == "[2;3[" );
      TS_ASSERT( translator5.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{2}})
                 == "[3;4]" );
      TS_ASSERT( translator5.variable ()->toString() == "X1<[1;2[,[2;3[,[3;4]>" );
 
      gum::learning::DBTranslator4DiscretizedVariable<MyAlloc>
        translator6 ( std::move ( translator4 ) );
      TS_ASSERT( translator6.translate ( "1" ).discr_val == 0 );
      TS_ASSERT( translator6.translate ( "2" ).discr_val == 1 );
      TS_ASSERT( translator6.translate ( "4" ).discr_val == 2 );
      TS_ASSERT( translator6.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{0}})
                 == "[1;2[" );
      TS_ASSERT( translator6.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{1}})
                 == "[2;3[" );
      TS_ASSERT( translator6.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{2}})
                 == "[3;4]" );
      TS_ASSERT( translator6.variable ()->toString() == "X2<[1;2[,[2;3[,[3;4]>" );

      gum::learning::DBTranslator4DiscretizedVariable<MyAlloc>*
        translator7 = translator6.clone ();
      TS_ASSERT( translator7->translate ( "1" ).discr_val == 0 );
      TS_ASSERT( translator7->translate ( "2" ).discr_val == 1 );
      TS_ASSERT( translator7->translate ( "4" ).discr_val == 2 );
      TS_ASSERT( translator7->translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{0}})
                 == "[1;2[" );
      TS_ASSERT( translator7->translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{1}})
                 == "[2;3[" );
      TS_ASSERT( translator7->translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{2}})
                 == "[3;4]" );
      TS_ASSERT( translator7->variable ()->toString() == "X2<[1;2[,[2;3[,[3;4]>" );

      MyAlloc<gum::learning::DBTranslator4DiscretizedVariable<MyAlloc>>
        allocator ( translator7->getAllocator () );
      allocator.destroy ( translator7 );
      allocator.deallocate ( translator7, 1 );


      translator4 = translator6;
      TS_ASSERT( translator4.translate ( "1" ).discr_val == 0 );
      TS_ASSERT( translator4.translate ( "2" ).discr_val == 1 );
      TS_ASSERT( translator4.translate ( "4" ).discr_val == 2 );
      TS_ASSERT( translator4.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{0}})
                 == "[1;2[" );
      TS_ASSERT( translator4.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{1}})
                 == "[2;3[" );
      TS_ASSERT( translator4.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{2}})
                 == "[3;4]" );
      TS_ASSERT( translator4.variable ()->toString() == "X2<[1;2[,[2;3[,[3;4]>" );

      translator3 = translator5;
      TS_ASSERT( translator3.translate ( "1" ).discr_val == 0 );
      TS_ASSERT( translator3.translate ( "2" ).discr_val == 1 );
      TS_ASSERT( translator3.translate ( "4" ).discr_val == 2 );
      TS_ASSERT( translator3.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{0}})
                 == "[1;2[" );
      TS_ASSERT( translator3.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{1}})
                 == "[2;3[" );
      TS_ASSERT( translator3.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{2}})
                 == "[3;4]" );
      TS_ASSERT( translator3.variable ()->toString() == "X1<[1;2[,[2;3[,[3;4]>" );
      
      translator5 = std::move ( translator3 );
      TS_ASSERT( translator5.translate ( "1" ).discr_val == 0 );
      TS_ASSERT( translator5.translate ( "2" ).discr_val == 1 );
      TS_ASSERT( translator5.translate ( "4" ).discr_val == 2 );
      TS_ASSERT( translator5.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{0}})
                 == "[1;2[" );
      TS_ASSERT( translator5.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{1}})
                 == "[2;3[" );
      TS_ASSERT( translator5.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{2}})
                 == "[3;4]" );
      TS_ASSERT( translator5.variable ()->toString() == "X1<[1;2[,[2;3[,[3;4]>" );
      
      }

      TS_ASSERT(MyAllocCount::hasMeroryLeak() == false);
    }

  
    void xtest_trans5() {
      {
      gum::DiscretizedVariable<float> var ( "X1", "" );
      var.addTick ( 1 );
      var.addTick ( 2 );
      var.addTick ( 3 );
      var.addTick ( 4 );

      LearningAlloc<float> alloc ( 10000 );

      gum::learning::DBTranslator4DiscretizedVariable<LearningAlloc>
        translator ( var, 1000, alloc );

      std::cout << alloc.toString () << std::endl;
      std::cout << translator.getAllocator ().toString () << std::endl;
      
      TS_ASSERT( translator.translate ( "1" ).discr_val == 0 );
      TS_ASSERT( translator.translate ( "2" ).discr_val == 1 );
      TS_ASSERT( translator.translate ( "4" ).discr_val == 2 );
      TS_ASSERT( translator.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{0}})
                 == "[1,2[" );
      TS_ASSERT( translator.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{1}})
                 == "[2,3[" );
      TS_ASSERT( translator.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{2}})
                 == "[3,4]" );
      TS_ASSERT( translator.variable ()->toString() == "X1<[1,2[,[2,3[,[3,4]>" );

      const auto& allocator =  translator.getAllocator ();
      std::cout << (allocator == alloc ) << std::endl;
      std::cout << alloc.toString () << std::endl;
      
      gum::DiscretizedVariable<float> var2 ( "X2", "" );
      var2.addTick ( 1 );
      var2.addTick ( 2 );
      var2.addTick ( 3 );
      var2.addTick ( 4 );

      gum::learning::DBTranslator4DiscretizedVariable<LearningAlloc>
        translator2 ( var2 );
      TS_ASSERT( translator2.translate ( "1" ).discr_val == 0 );
      TS_ASSERT( translator2.translate ( "2" ).discr_val == 1 );
      TS_ASSERT( translator2.translate ( "4" ).discr_val == 2 );
      TS_ASSERT( translator2.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{0}})
                 == "[1;2[" );
      TS_ASSERT( translator2.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{1}})
                 == "[2;3[" );
      TS_ASSERT( translator2.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{2}})
                 == "[3;4]" );
      TS_ASSERT( translator2.variable ()->toString() == "X2<[1;2[,[2;3[,[3;4]>" );
      
      gum::learning::DBTranslator4DiscretizedVariable<LearningAlloc>
        translator3 ( translator );
      TS_ASSERT( translator3.translate ( "1" ).discr_val == 0 );
      TS_ASSERT( translator3.translate ( "4" ).discr_val == 2 );
      TS_ASSERT( translator3.translate ( "2" ).discr_val == 1 );
      TS_ASSERT( translator3.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{0}})
                 == "[1;2[" );
      TS_ASSERT( translator3.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{1}})
                 == "[2;3[" );
      TS_ASSERT( translator3.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{2}})
                 == "[3;4]" );
      TS_ASSERT( translator3.variable ()->toString() == "X1<[1;2[,[2;3[,[3;4]>" );

      gum::learning::DBTranslator4DiscretizedVariable<LearningAlloc>
        translator4 ( translator2 );
      TS_ASSERT( translator4.translate ( "1" ).discr_val == 0 );
      TS_ASSERT( translator4.translate ( "2" ).discr_val == 1 );
      TS_ASSERT( translator4.translate ( "4" ).discr_val == 2 );
      TS_ASSERT( translator4.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{0}})
                 == "[1;2[" );
      TS_ASSERT( translator4.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{1}})
                 == "[2;3[" );
      TS_ASSERT( translator4.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{2}})
                 == "[3;4]" );
      TS_ASSERT( translator4.variable ()->toString() == "X2<[1;2[,[2;3[,[3;4]>" );
      
      gum::learning::DBTranslator4DiscretizedVariable<LearningAlloc>
        translator5 ( std::move ( translator3 ) );
      TS_ASSERT( translator5.translate ( "1" ).discr_val == 0 );
      TS_ASSERT( translator5.translate ( "4" ).discr_val == 2 );
      TS_ASSERT( translator5.translate ( "2" ).discr_val == 1 );
      TS_ASSERT( translator5.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{0}})
                 == "[1;2[" );
      TS_ASSERT( translator5.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{1}})
                 == "[2;3[" );
      TS_ASSERT( translator5.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{2}})
                 == "[3;4]" );
      TS_ASSERT( translator5.variable ()->toString() == "X1<[1;2[,[2;3[,[3;4]>" );

      gum::learning::DBTranslator4DiscretizedVariable<LearningAlloc>
        translator6 ( std::move ( translator4 ) );
      TS_ASSERT( translator6.translate ( "1" ).discr_val == 0 );
      TS_ASSERT( translator6.translate ( "2" ).discr_val == 1 );
      TS_ASSERT( translator6.translate ( "4" ).discr_val == 2 );
      TS_ASSERT( translator6.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{0}})
                 == "[1;2[" );
      TS_ASSERT( translator6.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{1}})
                 == "[2;3[" );
      TS_ASSERT( translator6.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{2}})
                 == "[3;4]" );
      TS_ASSERT( translator6.variable ()->toString() == "X2<[1;2[,[2;3[,[3;4]>" );

      gum::learning::DBTranslator4DiscretizedVariable<LearningAlloc>*
        translator7 = translator6.clone ();
      TS_ASSERT( translator7->translate ( "1" ).discr_val == 0 );
      TS_ASSERT( translator7->translate ( "2" ).discr_val == 1 );
      TS_ASSERT( translator7->translate ( "4" ).discr_val == 2 );
      TS_ASSERT( translator7->translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{0}})
                 == "[1;2[" );
      TS_ASSERT( translator7->translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{1}})
                 == "[2;3[" );
      TS_ASSERT( translator7->translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{2}})
                 == "[3;4]" );
      TS_ASSERT( translator7->variable ()->toString() == "X2<[1;2[,[2;3[,[3;4]>" );

      LearningAlloc<gum::learning::DBTranslator4DiscretizedVariable<LearningAlloc>>
        allocator2 ( translator7->getAllocator () );
      allocator2.destroy ( translator7 );
      allocator2.deallocate ( translator7, 1 );

      translator5 = translator6;
      TS_ASSERT( translator5.translate ( "1" ).discr_val == 0 );
      TS_ASSERT( translator5.translate ( "2" ).discr_val == 1 );
      TS_ASSERT( translator5.translate ( "4" ).discr_val == 2 );
      TS_ASSERT( translator5.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{0}})
                 == "[1;2[" );
      TS_ASSERT( translator5.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{1}})
                 == "[2;3[" );
      TS_ASSERT( translator5.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{2}})
                 == "[3;4]" );
      TS_ASSERT( translator5.variable ()->toString() == "X2<[1;2[,[2;3[,[3;4]>" );

      translator3 = translator5;
      TS_ASSERT( translator3.translate ( "1" ).discr_val == 0 );
      TS_ASSERT( translator3.translate ( "2" ).discr_val == 1 );
      TS_ASSERT( translator3.translate ( "4" ).discr_val == 2 );
      TS_ASSERT( translator3.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{0}})
                 == "[1;2[" );
      TS_ASSERT( translator3.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{1}})
                 == "[2;3[" );
      TS_ASSERT( translator3.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{2}})
                 == "[3;4]" );
      TS_ASSERT( translator3.variable ()->toString() == "X2<[1;2[,[2;3[,[3;4]>" );

      translator5 = std::move ( translator3 );
      TS_ASSERT( translator5.translate ( "1" ).discr_val == 0 );
      TS_ASSERT( translator5.translate ( "2" ).discr_val == 1 );
      TS_ASSERT( translator5.translate ( "4" ).discr_val == 2 );
      TS_ASSERT( translator5.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{0}})
                 == "[1;2[" );
      TS_ASSERT( translator5.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{1}})
                 == "[2;3[" );
      TS_ASSERT( translator5.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{2}})
                 == "[3;4]" );
      TS_ASSERT( translator5.variable ()->toString() == "X2<[1;2[,[2;3[,[3;4]>" );
      }

      TS_ASSERT(MyAllocCount::hasMeroryLeak() == false);

    }

   
    void test_trans6() {
      gum::DiscretizedVariable<float> var ( "X1", "" );
      var.addTick ( 1 );
      var.addTick ( 3 );
      var.addTick ( 7 );
      var.addTick ( 9 );

      std::vector<std::string>
        missing { "[1;3[", "NA", "[7;9]", "1", "4.3", "10", "?" };
      
      gum::learning::DBTranslator4DiscretizedVariable<>
        translator ( var, missing );

      TS_ASSERT( translator.translate ( "1" ).discr_val == 0 );
      TS_ASSERT( translator.translate ( "3.2" ).discr_val == 1 );
      TS_ASSERT( translator.translate ( "NA" ).discr_val ==
                 std::numeric_limits<std::size_t>::max () );
      TS_ASSERT( translator.translate ( "?" ).discr_val ==
                 std::numeric_limits<std::size_t>::max () );
      TS_ASSERT( translator.translate ( "7" ).discr_val == 2 );

      TS_ASSERT( translator.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{0}})
                 == "[1;3[" );
      TS_ASSERT( translator.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{1}})
                 == "[3;7[" );
      TS_ASSERT( translator.translateBack
                 (gum::learning::DBTranslatedValue{std::size_t{2}})
                 == "[7;9]" );

      std::string back = translator.translateBack
        (gum::learning::DBTranslatedValue{std::numeric_limits<std::size_t>::max ()} );
      TS_ASSERT( back == *( translator.missingSymbols ().begin() ) );
      gum::Set<std::string> missing_kept { "NA", "10", "?" };
      TS_ASSERT ( translator.missingSymbols () == missing_kept );
      TS_ASSERT ( translator.needsReordering () == false);

      auto new_order = translator.reorder ();
      TS_ASSERT ( new_order.size () == 0 );
      
    }

    
  };

} /* namespace gum_tests */
