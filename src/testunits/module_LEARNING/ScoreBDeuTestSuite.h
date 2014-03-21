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
#include <agrum/learning/scoreBDeu.h>

namespace gum_tests {

  class ScoreK2TestSuite: public CxxTest::TestSuite {
  public:

    class CellTranslator : public gum::learning::DBCellTranslator<1,1> {
    public:
      void translate () { out (0) = in (0).getFloat (); }
      void initialize () {
        unsigned int nb = in(0).getFloat ();
        if ( ! __values.exists ( nb ) ) __values.insert ( nb );
      }
      void modalities ( std::vector<unsigned int>& modal ) const noexcept {
        modal.push_back ( __values.size () );
      }
      bool requiresInitialization () const noexcept { return true; }
      std::string translateBack ( unsigned int col,
                                  unsigned int translated_val ) {
        std::stringstream str;
        str << translated_val;
        return  str.str ();
      }

    private:
      gum::Set<unsigned int> __values;
      
    };

    class SimpleGenerator : public gum::learning::FilteredRowGenerator {
    public:
      gum::learning::FilteredRow& generate () {
        decreaseRemainingRows ();
        return *_input_row;
      }
      unsigned int _computeRows () { return 1; }
    };


    void test_BDeu () {
      gum::learning::DatabaseFromCSV database ( GET_PATH_STR( "asia.csv" ) );
      
      auto handler = database.handler ();
      
      auto translators = gum::learning::make_translators
        ( gum::learning::Create<CellTranslator, gum::learning::Col<0>, 8 > () );

      auto generators =  gum::learning::make_generators ( SimpleGenerator () );
      
      auto filter = gum::learning::make_row_filter ( handler, translators,
                                                     generators );
      
      std::vector<unsigned int> modalities = filter.modalities ();

      gum::learning::ScoreBDeu<decltype ( filter ) >
        score ( filter, modalities );

      // to test, we exploit the fact that if the effective sample size is
      // equal to ri * qi, then score BDeu = score K2

      unsigned int id1 = score.addNodeSet ( 3, 2 );
      unsigned int id2 = score.addNodeSet ( 1, 2 );
      TS_ASSERT ( fabs ( score.score ( id1 ) + 996.781  ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( id2 ) + 3030.73  ) <= 0.01 );

      score.clear ();
      id1 = score.addNodeSet ( 0, 2 );
      id2 = score.addNodeSet ( 2, 2 );
      TS_ASSERT ( fabs ( score.score ( id1 ) + 10006.1 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( id2 ) + 9935.8  ) <= 0.01 );

      score.clear ();
      id1 = score.addNodeSet ( 3, std::vector<unsigned int> { 4 }, 4 );
      id2 = score.addNodeSet ( 1, std::vector<unsigned int> { 4 }, 4 );
      TS_ASSERT ( fabs ( score.score ( id1 ) + 991.062 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( id2 ) + 3030.55 ) <= 0.01 );

      score.clear ();
      id1 = score.addNodeSet ( 3, std::vector<unsigned int> { 1, 2 }, 8 );
      TS_ASSERT ( fabs ( score.score ( id1 ) + 1014.4 ) <= 0.01 );

      unsigned int id3, id4, id5, id6, id7;
      score.clear ();
      id1 = score.addNodeSet ( 3, 2 );
      id2 = score.addNodeSet ( 1, 2 );
      id3 = score.addNodeSet ( 3, std::vector<unsigned int> { 1, 2 }, 8 );
      id4 = score.addNodeSet ( 2, 2 );
      id5 = score.addNodeSet ( 3, std::vector<unsigned int> { 4 }, 4 );
      id6 = score.addNodeSet ( 2, 2 );
      id7 = score.addNodeSet ( 3, std::vector<unsigned int> { 4 }, 4 );
      TS_ASSERT ( fabs ( score.score ( id1 ) + 996.781 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( id2 ) + 3030.73 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( id3 ) + 1014.4  ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( id4 ) + 9935.8  ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( id5 ) + 991.062 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( id6 ) + 9935.8  ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( id7 ) + 991.062 ) <= 0.01 );
    }

  };


} /* namespace gum_tests */
