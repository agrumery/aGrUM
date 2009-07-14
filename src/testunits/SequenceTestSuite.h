/***************************************************************************
 *   Copyright (C) 2007 by Lionel Torti                                    *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,       *
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
#include <agrum/core/sequence.h>

class SequenceTestSuite: public CxxTest::TestSuite {
  private:
    void fill( gum::Sequence<int>& seq ) {
      seq<<1<<3<<5<<2<<4;
    }

  public:

    void testConstructor1() {
      gum::Sequence<int> *p_seq=NULL;
      TS_GUM_ASSERT_THROWS_NOTHING( p_seq=new gum::Sequence<int>() );

      TS_GUM_ASSERT_THROWS_NOTHING( if ( p_seq ) delete( p_seq ) );

      gum::Sequence<int> seq;

      TS_GUM_ASSERT_THROWS_NOTHING( seq<<1 );

      TS_GUM_ASSERT_THROWS_NOTHING( seq<<3 );

      TS_GUM_ASSERT_THROWS_NOTHING( seq<<5 );

      TS_GUM_ASSERT_THROWS_NOTHING( seq<<2 );

      TS_GUM_ASSERT_THROWS_NOTHING( seq<<4 );

      TS_ASSERT_EQUALS( seq.size(),( gum::Size )5 );

      TS_ASSERT( ! seq.empty() );
    }

    void testCopy() {
      gum::Sequence<int> seq;
      TS_GUM_ASSERT_THROWS_NOTHING( seq<<1 );
      TS_GUM_ASSERT_THROWS_NOTHING( seq<<3 );
      TS_GUM_ASSERT_THROWS_NOTHING( seq<<5 );
      TS_GUM_ASSERT_THROWS_NOTHING( seq<<2 );
      TS_GUM_ASSERT_THROWS_NOTHING( seq<<4 );

      gum::Sequence<int>* p_seq2=NULL;
      TS_ASSERT_EQUALS( seq.toString(),"[0:1 - 1:3 - 2:5 - 3:2 - 4:4]" );
      TS_GUM_ASSERT_THROWS_NOTHING( p_seq2=new gum::Sequence<int>( seq ) );
      TS_ASSERT_EQUALS( p_seq2->toString(),"[0:1 - 1:3 - 2:5 - 3:2 - 4:4]" );

      TS_GUM_ASSERT_THROWS_NOTHING( if ( p_seq2 ) delete( p_seq2 ) );

      TS_GUM_ASSERT_THROWS_NOTHING( gum::Sequence<int> seq2( seq );if ( seq2.toString()!="[0:1 - 1:3 - 2:5 - 3:2 - 4:4]" ) throw( gum::Exception( "oups" ) ); );

      TS_GUM_ASSERT_THROWS_NOTHING( gum::Sequence<int> seq3=seq;if ( seq3.toString()!="[0:1 - 1:3 - 2:5 - 3:2 - 4:4]" ) throw( gum::Exception( "oups" ) ); );
    }

    void testEquality() {
      gum::Sequence<int> seq1,seq2;
      fill( seq1 );
      fill( seq2 );
      TS_ASSERT_EQUALS( seq1,seq2 );

      gum::Sequence<int> seq3;
      TS_ASSERT( seq1!=seq3 );
      seq3<<1;
      TS_ASSERT( seq1!=seq3 );
      seq3<<2<<3<<4<<5;
      TS_ASSERT( seq1!=seq3 );
    }

    void testGettersAndSetters() {
      gum::Sequence<std::string> seq;

      TS_GUM_ASSERT_THROWS_NOTHING( seq<<"aaaa"<<"b"<<"cc" );
      TS_GUM_ASSERT_THROWS_NOTHING( seq.insert( "ddd" ) );

      TS_ASSERT_THROWS( seq.insert( "ddd" ),gum::DuplicateElement );
      TS_ASSERT_THROWS( seq<<"ddd",gum::DuplicateElement );

      TS_ASSERT_EQUALS( seq.toString(),"[0:aaaa - 1:b - 2:cc - 3:ddd]" );

      TS_ASSERT_EQUALS( seq[1],"b" );
      TS_ASSERT_EQUALS( seq["ddd"],( gum::Idx ) 3 );

      TS_ASSERT( seq.exists( "b" ) );
      TS_ASSERT( ! seq.exists( "bb" ) );

      TS_ASSERT_THROWS( seq[15],gum::NotFound );
      TS_ASSERT_THROWS( seq["aaaaaaa"],gum::NotFound );

      TS_ASSERT_EQUALS( seq.toString(),"[0:aaaa - 1:b - 2:cc - 3:ddd]" );
      TS_GUM_ASSERT_THROWS_NOTHING( seq.setAtPos( 1,"boom" ) );
      TS_ASSERT_EQUALS( seq.toString(),"[0:aaaa - 1:boom - 2:cc - 3:ddd]" );
      TS_ASSERT_THROWS( seq.setAtPos( 15,"crach" ),gum::NotFound );
      TS_ASSERT_THROWS( seq.setAtPos( 1,"aaaa" ),gum::DuplicateElement );

      TS_ASSERT_EQUALS( seq.toString(),"[0:aaaa - 1:boom - 2:cc - 3:ddd]" );

      TS_GUM_ASSERT_THROWS_NOTHING( seq.swap( 0,2 ) );
      TS_ASSERT_EQUALS( seq.toString(),"[0:cc - 1:boom - 2:aaaa - 3:ddd]" );
      TS_GUM_ASSERT_THROWS_NOTHING( seq.swap( 0,3 ) );
      TS_ASSERT_EQUALS( seq.toString(),"[0:ddd - 1:boom - 2:aaaa - 3:cc]" );
      TS_ASSERT_THROWS( seq.swap( 1,10 ),gum::NotFound );
      TS_ASSERT_THROWS( seq.swap( 10,1 ),gum::NotFound );

      TS_ASSERT_EQUALS( seq.toString(),"[0:ddd - 1:boom - 2:aaaa - 3:cc]" );
    }

    void testRemove() {
      gum::Sequence<int> seq;
      fill( seq );

      TS_ASSERT_EQUALS( seq.toString(),"[0:1 - 1:3 - 2:5 - 3:2 - 4:4]" );
      TS_GUM_ASSERT_THROWS_NOTHING( seq>>2 );
      TS_ASSERT_EQUALS( seq.toString(),"[0:1 - 1:3 - 2:5 - 3:4]" );
      TS_GUM_ASSERT_THROWS_NOTHING( seq.erase( 1 ) );
      TS_ASSERT_EQUALS( seq.toString(),"[0:3 - 1:5 - 2:4]" );
      TS_GUM_ASSERT_THROWS_NOTHING( seq>>4 );
      TS_ASSERT_EQUALS( seq.toString(),"[0:3 - 1:5]" );
      TS_GUM_ASSERT_THROWS_NOTHING( seq>>4 );
      TS_ASSERT_EQUALS( seq.toString(),"[0:3 - 1:5]" );

      seq.clear();
      TS_ASSERT( seq.empty() );
      TS_ASSERT_EQUALS( seq.size(),( gum::Size )0 );
      TS_ASSERT_EQUALS( seq.toString(),"[]" );
    }

    void testIterators() {
      gum::Sequence<int> seq;

      int n;

      n=0;
      for ( gum::Sequence<int>::iterator it = seq.begin();
            it != seq.end(); ++it ) n++;
      TS_ASSERT_EQUALS( n,0 );


      n=0;
      for ( gum::Sequence<int>::iterator it = seq.rbegin();
            it != seq.rend(); --it ) n++;
      TS_ASSERT_EQUALS( n,0 );

      seq<<1<<3<<5<<2<<4;

      n=0;
      for ( gum::Sequence<int>::iterator it=seq.begin();it!=seq.end();++it ) {
        n*=10;n+=*it;
      }
      TS_ASSERT_EQUALS( n,13524 );

      n=0;
      for ( gum::Sequence<int>::iterator it=seq.rbegin();it!=seq.rend();--it ) {
        n*=10;n+=*it;
      }

      TS_ASSERT_EQUALS( n,42531 );
    }

  void testIdxSeq () {
    gum::Sequence<gum::Idx> seq;

    seq<<1<<3<<5<<2<<4;

    int n = 0;
    for ( gum::Sequence<gum::Idx>::iterator it=seq.rbegin();
          it!=seq.rend();--it ) {
      n*=10;n+=*it;
    }

    TS_ASSERT_EQUALS( n,42531 );

    gum::Sequence<gum::Idx> seq2;
    TS_ASSERT_THROWS( seq2.front(), gum::NotFound );
  }

  
  void testIntStarSeq () {
    gum::Sequence<int*> seq;

    int *a = 0, *b;
    b = a + 3;
    
    seq<<a<<b;

    int* n = 0;
    for ( gum::Sequence<int*>::iterator it=seq.rbegin();
          it!=seq.rend();--it ) {
      n=*it;
    }


    gum::Sequence<int*> seq2;
    TS_ASSERT_THROWS( seq2.front(), gum::NotFound );
  }
  
};


