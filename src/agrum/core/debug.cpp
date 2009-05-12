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

    void __inc_creation( const char *zeKey,const char *zeFile,long zeLine,const char *zeMsg,const void *zePtr ) {
#ifndef NDEBUG
      __show_trace( zeKey,zeFile,zeLine,zeMsg,zePtr );

      try {
        __creation()[zeKey]++;
      } catch ( NotFound& ) {
        __creation().insert( zeKey,1 );
        __deletion().insert( zeKey,0 );
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
      std::cerr<<std::setfill( '-' );
      std::cerr<<"|-"<<std::setw( 50 )<<""<<"-|-"<<std::setw( 5 )<<""<<"-|-"<<std::setw( 5 )<<""<<"-|"<<std::endl;
      std::cerr<<std::setfill( ' ' );
      std::cerr<<"| "<<std::setw( 50 )<<"   Class Name    "<<" | "<<std::setw( 5 )<<"Const"<<" | "<<std::setw( 5 )<<"Dest"<<" |"<<std::endl;
      std::cerr<<std::setfill( '-' );
      std::cerr<<"|-"<<std::setw( 50 )<<""<<"-|-"<<std::setw( 5 )<<""<<"-|-"<<std::setw( 5 )<<""<<"-|"<<std::endl;
      std::cerr<<std::setfill( ' ' );
      // list of created objects
      std::vector<std::string> res;

      for ( HashTable<std::string,int>::iterator xx = __creation().begin(); xx != __creation().end();++xx ) {
        std::stringstream stream;
        int zeCreatedObjs=*xx;
        int zeDeletedObjts=-1;
        stream<<"| "<<std::setw( 50 )<<xx.key()<<" | "<<std::setw( 5 )<<zeCreatedObjs<<" | ";

        try {
          zeDeletedObjts=__deletion()[xx.key()];
          stream<<std::setw( 5 )<<zeDeletedObjts;
        } catch ( NotFound& ) {
          stream<<std::setw( 5 )<<"?????";
        }

        stream<<" |";;

        if ( zeCreatedObjs!=zeDeletedObjts ) stream<<"<--- failed";

        res.push_back( stream.str() );
      }

      // list of deleted objects, but not created (?)
      for ( HashTable<std::string,int>::iterator xx = __deletion().begin(); xx != __deletion().end();++xx ) {
        try {
          __creation()[xx.key()];
        } catch ( NotFound& ) {
          std::stringstream stream;
          stream<<"| "<<std::setw( 50 )<<xx.key()<<" | "<<std::setw( 5 )<<"?????"<<" | "<<std::setw( 5 )<<*xx<<" |<--- failed";
          res.push_back( stream.str() );
        }
      }

      sort( res.begin(),res.end() );

      for ( std::vector<std::string>::iterator iter=res.begin();
            iter!=res.end();
            ++iter ) {
        std::cerr<<*iter<<std::endl;
      }

      std::cerr<<std::setfill( '-' );

      std::cerr<<"|-"<<std::setw( 50 )<<""<<"-|-"<<std::setw( 5 )<<""<<"-|-"<<std::setw( 5 )<<""<<"-|"<<std::endl;
#endif //NDEBUG
    }

    // take into account static objects in agrum (no called destructor before exit())
    void staticCorrections() {
#ifndef NDEBUG
      __dec_creation( "Set"        ,"__empty_arc_set",0,"static variable correction",0 );
      __dec_creation( "HashTable"  ,"__empty_arc_set",0,"static variable correction",0 );
      __dec_creation( "SetIterator","__empty_arc_set",0,"static variable correction",0 );
      __dec_creation( "Set"        ,"__empty_edge_set",0,"static variable correction",0 );
      __dec_creation( "HashTable"  ,"__empty_edge_set",0,"static variable correction",0 );
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
