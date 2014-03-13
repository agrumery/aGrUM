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

#include <agrum/learning/indepTestChi2.h>
#include <agrum/learning/indepTestG2.h>
#include <agrum/learning/scoreBIC.h>

#define ROWS 500000
#define COLS 30

namespace gum_tests {

  class NewArchTestSuite: public CxxTest::TestSuite {
  public:

    class CellTranslator1 : public gum::learning::DBCellTranslator<1,1> {
    public:
      void translate () {
        out (0) = in (0).getFloat ();
      }
    };

    class CellTranslator2 : public gum::learning::DBCellTranslator<2,1> {
    public:
      void translate () {
        outputRow() [outputCols() [0]] =
          inputRow() [inputCols() [0]].getFloat () +
          inputRow() [inputCols() [1]].getFloat ();
      }
    };

    class SimpleGenerator : public gum::learning::FilteredRowGenerator {
    public:
      /// returns the next FilteredRow
      gum::learning::FilteredRow& generate () {
        decreaseRemainingRows ();
        return *_input_row;
      }
      
      /// the method that computes the set of FilteredRows
      unsigned int _computeRows () {
        return 1;
      }
    };

    struct Database {
      std::vector<gum::learning::DBRow> data;

      struct EndRow {};
      
      struct  Handler {
        Handler ( const Database& db ) :
        __row ( &( db.data ) ),
        __end_index ( __row->size () ) {}
        
        Handler ( const Handler& h ) :
        __row ( h.__row ),
        __index ( h.__index ),
        __begin_index ( h.__begin_index ),
        __end_index ( h.__end_index ) {}

        unsigned long size () const noexcept {
          return __end_index - __begin_index;
        }

        unsigned long DBSize () const noexcept {
          return __row->size ();
        }
        
        const gum::learning::DBRow& row () {
          if ( __index >= __end_index ) {
            throw EndRow ();
          }
          return __row->operator[] ( __index );
        }

        void nextRow () {
          ++__index;
        }

        bool hasRows () const noexcept {
          return ( __index < __end_index );
        }

        void reset () {
          __index = __begin_index;
        }

        void setRange ( unsigned long begin, unsigned long end ) {
          __begin_index = begin;
          __end_index = end;
        }
 
        const std::vector<gum::learning::DBRow>* __row;
        unsigned long __index { 0 };
        unsigned long __begin_index { 0 };
        unsigned long __end_index { 0 };
        char __align[CACHE_SIZE];
        
      } ;

      Handler handler () const { return Handler ( *this ); }
  
    };


    
    void test_archi () {
      Database database;
      database.data.resize ( ROWS );
      for ( unsigned int i = 0; i < ROWS; ++i ) {
        gum::learning::DBRow& row = database.data[i];
        row.resize ( COLS );
        for ( unsigned int j = 0; j < COLS; ++j ) {
          row[j].setFloat ( j % 3 ); // 2 * i + j;
        }
      }

      auto translators = gum::learning::make_translators
        ( gum::learning::Create<CellTranslator1, gum::learning::Col<0> > (),
          gum::learning::Create<CellTranslator1, gum::learning::Col<1>, 4 > (),
          gum::learning::Create<CellTranslator1, gum::learning::Col<6> > (),
          gum::learning::Create<CellTranslator1, gum::learning::Col<10>, 4 > (),
          gum::learning::Create<CellTranslator2, gum::learning::Col<0,5>, 4 > (),
          gum::learning::Create<CellTranslator2, gum::learning::Col<10,11>, 4,
                                gum::learning::Incr<1,2> > ()
        );

      Database::Handler handler ( database );
      
      auto generators =  gum::learning::make_generators ( SimpleGenerator () );
      
      auto filter = gum::learning::make_row_filter ( handler, translators,
                                                     generators );

      std::vector<unsigned int> modalities;
      for ( unsigned int i = 0 ; i < COLS; ++i ) {
        modalities.push_back ( 3 );
      }

      /*
      gum::learning::RecordCounter<decltype ( filter ) >
        counter ( filter, modalities );
        
      std::vector<unsigned int> ids1 { 3, 1, 2 };
      std::vector<unsigned int> ids2 { 1 };
      std::vector<unsigned int> ids3 { 2 };
  
      unsigned int id1, id2, id3;

      for (unsigned int i = 0; i < 1000; ++i ) {
        counter.clearNodeSets ();
    
        id1 = counter.addNodeSet ( ids1 );
        id2 = counter.addNodeSet ( ids2 );
        id3 = counter.addNodeSet ( ids3 );

        counter.count ();
      }
      */
      gum::learning::IndepTestG2<decltype ( filter ) > score ( filter, modalities );
      //gum::learning::ScoreBIC<decltype ( filter ) > score ( filter, modalities );
      unsigned int id1, id2, id3, id4;
      id1 = score.addNodeSet ( 2, 3, std::vector<unsigned int> { 4 } );
      id2 = score.addNodeSet ( 1, 2 );
      id3 = score.addNodeSet ( 3, 2 );
      id4 = score.addNodeSet ( 3, 2 );

      //score.count ();

      std::cout << score.score ( id1 ) << std::endl;
      std::cout << score.score ( id2 ) << std::endl;
      std::cout << score.score ( id3 ) << std::endl;
      std::cout << score.score ( id4 ) << std::endl;
      
      
      /*
      
      std::cout << "======================================" << std::endl;
      const std::vector<float>& count1 = score.getTargetCounts ( id1 );
      for ( unsigned int i = 0; i < count1.size (); ++i ) {
        std::cout << count1[i] << " ";
      }
      std::cout << std::endl;

      std::cout << "======================================" << std::endl;
      const std::vector<float>& count2 = score.getTargetCounts ( id2 );
      for ( unsigned int i = 0; i < count2.size (); ++i ) {
        std::cout << count2[i] << " ";
      }
      std::cout << std::endl;
      
      std::cout << "======================================" << std::endl;
      const std::vector<float>& count3 = score.getTargetCounts ( id3 );
      for ( unsigned int i = 0; i < count3.size (); ++i ) {
        std::cout << count3[i] << " ";
      }
      std::cout << std::endl;

      */
    }
    
  };


} /* namespace gum_tests */
