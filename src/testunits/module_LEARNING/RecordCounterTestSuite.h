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

#include <agrum/learning/databaseFromCSV.h>
#include <agrum/learning/recordCounter.h>


namespace gum_tests {

  class RecordCounterTestSuite: public CxxTest::TestSuite {
  public:
    
    class CellTranslator : public gum::learning::DBCellTranslator<1,1> {
    public:
      void translate () { out (0) = in (0).getFloat (); }
    };

    class SimpleGenerator : public gum::learning::FilteredRowGenerator {
    public:
      gum::learning::FilteredRow& generate () {
        decreaseRemainingRows ();
        return *_input_row;
      }
      unsigned int _computeRows () { return 1; }
    };

    
    void test1 () {
      gum::learning::DatabaseFromCSV database ( GET_PATH_STR( "asia.csv" ) );
      auto handler = database.handler ();

      auto translators = gum::learning::make_translators
        ( gum::learning::Create<CellTranslator, gum::learning::Col<0>, 8 > () );

      auto generators =  gum::learning::make_generators ( SimpleGenerator () );
      
      auto filter = gum::learning::make_row_filter ( handler, translators,
                                                     generators );

      std::vector<unsigned int> modalities ( 8, 2);

      gum::learning::RecordCounter<decltype ( filter ) >
        counter ( filter, modalities );

      std::vector<unsigned int> set1 { 0 };
      std::vector<unsigned int> set2 { 1 };
      std::vector<unsigned int> set3 { 1, 0 };
      unsigned int id1 = counter.addNodeSet ( set1 );
      unsigned int id2 = counter.addNodeSet ( set2 );
      unsigned int id3 = counter.addNodeSet ( set3 );

      counter.count ();

      const std::vector<float>& vect1 = counter.getCounts ( id1 );
      const std::vector<float>& vect2 = counter.getCounts ( id2 );
      const std::vector<float>& vect3 = counter.getCounts ( id3 );

      TS_ASSERT ( vect1[0] == 5028 );
      TS_ASSERT ( vect1[1] == 4972);
      TS_ASSERT ( vect2[0] == 538);
      TS_ASSERT ( vect2[1] == 9462);

      TS_ASSERT ( vect3[0] == 498 );
      TS_ASSERT ( vect3[1] == 4530 );
      TS_ASSERT ( vect3[2] == 40 );
      TS_ASSERT ( vect3[3] == 4932 );

    }
    
  };


} /* namespace gum_tests */
