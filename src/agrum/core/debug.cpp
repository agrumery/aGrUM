/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN and Christophe GONZALES  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief debugging functions
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 */
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>


#include <agrum/core/exceptions.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifndef NDEBUG
#include <agrum/core/hashTable.h>
#endif // NDEBUG


namespace gum {


  namespace debug {

#ifndef NDEBUG
    // this static hashtable only on debug mode.
    static HashTable<std::string,int>& __sizeof() {
#if defined(_MT) || defined(__MT__) || defined(_PTHREAD)
#warning "This function is not thread-safe ! (but only in debug mode)"
#endif
      static HashTable<std::string,int>* sizeOf=new HashTable<std::string,int>( "dummy constructor for debug purpose" );
      return *sizeOf;
    }

    // this static hashtable only on debug mode.
    static HashTable<std::string,int>& __creation() {
#if defined(_MT) || defined(__MT__) || defined(_PTHREAD)
#warning "This function is not thread-safe ! (but only in debug mode)"
#endif
      static HashTable<std::string,int>* creation=new HashTable<std::string,int>( "dummy constructor for debug purpose" );
      return *creation;
    }

    static HashTable<std::string,int>& __deletion() {
#if defined(_MT) || defined(__MT__) || defined(_PTHREAD)
#warning "This function is not thread-safe ! (but only in debug mode)"
#endif
      static HashTable<std::string,int>* deletion=new HashTable<std::string,int>( "dummy constructor for debug purpose" );
      return *deletion;
    }

#endif // NDEBUG

    std::string __getFile( const char* f ) {
      std::string s( f );
      return s.erase( 0,s.rfind( "/" )+1 );
    }

#ifdef TRACE_ON
    void __show_trace( const char *zeKey,const char *zeFile,long zeLine,const char *zeMsg,const void *zePtr ) {
      std::cerr <<std::setw( 20 )<<std::setfill( ' ' )<<__getFile( zeFile )<<"#"<<std::setfill( '0' )  <<  std::setw( 5 )<<std::dec<<zeLine<<" : "<<zeMsg<<" <"<<zeKey<<"> ["<<std::hex<<zePtr<<"]"<<std::dec<<std::endl;
    }

#else //TRACE_ON
    void __show_trace( const char *,const char *,long ,const char *,const void * ) { }

#endif //TRACE_ON

    void __inc_creation( const char *zeKey,const char *zeFile,long zeLine,const char *zeMsg,const void *zePtr,int zeSize ) {
#ifndef NDEBUG
      __show_trace( zeKey,zeFile,zeLine,zeMsg,zePtr );

      try {
        __creation()[zeKey]++;
      } catch ( NotFound& ) {
        __creation().insert( zeKey,1 );
        __deletion().insert( zeKey,0 );

        if ( zeSize!=-1 )
          if ( !__sizeof().exists( zeKey ) )
            __sizeof().insert( zeKey,zeSize );
      }

#endif //NDEBUG
    }

    // to handle static element of agrum library
    void __dec_creation( const char *zeKey,const char *zeFile,long zeLine,const char *zeMsg,const void *zePtr ) {
#ifndef NDEBUG
      __show_trace( zeKey,zeFile,zeLine,zeMsg,zePtr );
      __creation()[zeKey]--;
#endif //NDEBUG
    }

    void __inc_deletion( const char *zeKey,const char *zeFile,long zeLine,const char *zeMsg,const void *zePtr ) {
#ifndef NDEBUG
      __show_trace( zeKey,zeFile,zeLine,zeMsg,zePtr );

      try {
        __deletion()[zeKey]++;
      } catch ( NotFound& ) {
        __deletion().insert( zeKey,1 );
      }

#endif //NDEBUG
    }

    void __dumpObjects( void ) {
#ifndef NDEBUG
      unsigned int nb_err=0;
      int total_size=0;

      std::cerr<<std::setfill( '-' );
      std::cerr<<"|-"<<std::setw( 50 )<<""<<"-|-"<<std::setw( 7 )<<""<<"-|-"<<std::setw( 5 )<<""<<"-|-"<<std::setw( 5 )<<""<<"-|"<<std::endl;
      std::cerr<<std::setfill( ' ' );
      std::cerr<<"| "<<std::setw( 50 )<<"   Class Name    "<<" | "<<std::setw( 7 )<<"Size"<<" | "<<std::setw( 5 )<<"#Const"<<"| "<<std::setw( 5 )<<"#Dest"<<" |"<<std::endl;
      std::cerr<<std::setfill( '-' );
      std::cerr<<"|-"<<std::setw( 50 )<<""<<"-|-"<<std::setw( 7 )<<""<<"-|-"<<std::setw( 5 )<<""<<"-|-"<<std::setw( 5 )<<""<<"-|"<<std::endl;
      std::cerr<<std::setfill( ' ' );
      // list of created objects
      std::vector<std::string> res;

      for ( HashTable<std::string,int>::iterator xx = __creation().begin(); xx != __creation().end();++xx ) {
        std::stringstream stream;
        int zeCreatedObjs=*xx;
        int zeDeletedObjts=-1;
        int size=__sizeof().getWithDefault( xx.key(),-1 );
        stream<<"| "<<std::setw( 50 )<<xx.key()<<" | "<<std::setw( 5 )<<size<<" o | "<<std::setw( 5 )<<zeCreatedObjs<<" | ";

        if ( size>0 ) total_size+=zeCreatedObjs*size;

        try {
          zeDeletedObjts=__deletion()[xx.key()];
          stream<<std::setw( 5 )<<zeDeletedObjts;
        } catch ( NotFound& ) {
          stream<<std::setw( 5 )<<"?????";
        }

        stream<<" |";;

        if ( zeCreatedObjs!=zeDeletedObjts ) {nb_err+=abs(zeDeletedObjts-zeCreatedObjs);stream<<"<--- failed";}

        res.push_back( stream.str() );
      }

      // list of deleted objects, but not created (?)
      for ( HashTable<std::string,int>::iterator xx = __deletion().begin(); xx != __deletion().end();++xx ) {
        try {
          __creation()[xx.key()];
        } catch ( NotFound& ) {
          std::stringstream stream;
          stream<<"| "<<std::setw( 50 )<<xx.key()<<" | "<<std::setw( 7 )<<__sizeof()[xx.key()]<<" | "<<std::setw( 5 )<<"?????"<<" | "<<std::setw( 5 )<<*xx<<" |<--- failed";
          res.push_back( stream.str() );
          nb_err+=*xx;
        }
      }

      sort( res.begin(),res.end() );

      for ( std::vector<std::string>::iterator iter=res.begin();
            iter!=res.end();
            ++iter ) {
        std::cerr<<*iter<<std::endl;
      }

      std::cerr<<std::setfill( '-' );

      std::cerr<<"|-"<<std::setw( 50 )<<""<<"-|-"<<std::setw( 7 )<<""<<"-|-"<<std::setw( 5 )<<""<<"-|-"<<std::setw( 5 )<<""<<"-|"<<std::endl;

      std::cerr<<std::setfill( ' ' );

      if ( nb_err==0 ) {
        std::cerr<<"| "<<std::setw( 50 )<<"NO MEMORY LEAK !"<<""<<" | "<<std::setw(25)<<"|"<<std::endl;
      } else {
        std::cerr<<"| "<<std::setw( 50 )<<"Memory leaks found : "<<""<<" | "<<std::setw(11)<<nb_err<<" object(s) "<<std::setw(3)<<"|"<<std::endl;
      }
      std::cerr<<"| "<<std::setw( 50 )<<"total "<<""<<" | "<<std::setw(11)<<total_size<<" octet(s) "<<std::setw(4)<<"|"<<std::endl;

      std::cerr<<std::setfill( '-' );

      std::cerr<<"|"<<std::setw( 79 )<<"|"<<std::endl;


#endif //NDEBUG
    }

    // take into account static objects in agrum (no called destructor before exit())
    void staticCorrections() {
#ifndef NDEBUG
      __dec_creation( "Set"        ,"__empty_arc_set",0,"static variable correction",0 );
      __dec_creation( "HashTable"  ,"__empty_arc_set",0,"static variable correction",0 );
      __dec_creation( "SetIterator","__empty_arc_set",0,"static variable correction",0 );
      __dec_creation( "SetIterator","__empty_arc_set",0,"static variable correction",0 );
      __dec_creation( "Set"        ,"__empty_edge_set",0,"static variable correction",0 );
      __dec_creation( "HashTable"  ,"__empty_edge_set",0,"static variable correction",0 );
      __dec_creation( "SetIterator","__empty_edge_set",0,"static variable correction",0 );
      __dec_creation( "SetIterator","__empty_edge_set",0,"static variable correction",0 );
#endif //NDEBUG
    }

    void __atexit( void ) {

#ifndef NDEBUG
      staticCorrections();
      __dumpObjects();
      __creation().clear();
      __deletion().clear();
#endif //NDEBUG
    }


  } /* namespace gum::debug */


} /* namespace gum */

#endif // DOXYGEN_SHOULD_SKIP_THIS
