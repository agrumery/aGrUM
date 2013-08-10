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

#include <agrum/config.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifndef NDEBUG
#include <map>
#endif // NDEBUG


namespace gum {

#ifndef NDEBUG

  namespace __debug__ {
    typedef std::map<std::string,int> DEBUG_MAP;

// this static hashtable only on debug mode.
    static DEBUG_MAP& __sizeof() {
#if defined(_MT) || defined(__MT__) || defined(_PTHREAD)
#warning "This function is not thread-safe ! (but only in debug mode)"
#endif
      static DEBUG_MAP* sizeOf=new DEBUG_MAP();
      return *sizeOf;
    }

// this static hashtable only on debug mode.
    static DEBUG_MAP& __creation() {
#if defined(_MT) || defined(__MT__) || defined(_PTHREAD)
#warning "This function is not thread-safe ! (but only in debug mode)"
#endif
      static DEBUG_MAP* creation=new DEBUG_MAP();
      return *creation;
    }

    static DEBUG_MAP& __deletion() {
#if defined(_MT) || defined(__MT__) || defined(_PTHREAD)
#warning "This function is not thread-safe ! (but only in debug mode)"
#endif
      static DEBUG_MAP* deletion=new DEBUG_MAP();
      return *deletion;
    }

    std::string __getFile ( const char* f ) {
      std::string s ( f );
      return s.erase ( 0,s.rfind ( "/" ) +1 );
    }

    void __show_trace ( const char *zeKey,const char *zeFile,long zeLine,const char *zeMsg,const void *zePtr ) {
#ifdef GUM_DEEP_TRACE_ON
      std::cerr <<std::setw ( 40 ) <<std::setfill ( ' ' ) <<__getFile ( zeFile ) <<"#"<<std::setfill ( '0' )  <<  std::setw ( 5 ) <<std::dec<<zeLine<<" : "<<zeMsg<<" <"<<zeKey<<"> ["<<std::hex<<zePtr<<"]"<<std::dec<<std::endl;
#endif //TRACE_CONSTRUCTION_ON
    }

    void __inc_creation ( const char *zeKey,const char *zeFile,long zeLine,const char *zeMsg,const void *zePtr,int zeSize ) {
      __show_trace ( zeKey,zeFile,zeLine,zeMsg,zePtr );

      __creation() [zeKey]++;
      __sizeof() [zeKey]=zeSize;
    }

// to handle static element of agrum library
    void __dec_creation ( const char *zeKey,const char *zeFile,long zeLine,const char *zeMsg,const void *zePtr ) {
      __show_trace ( zeKey,zeFile,zeLine,zeMsg,zePtr );
      __creation() [zeKey]--;
    }

    void __inc_deletion ( const char *zeKey,const char *zeFile,long zeLine,const char *zeMsg,const void *zePtr ) {
      __show_trace ( zeKey,zeFile,zeLine,zeMsg,zePtr );
      __deletion() [zeKey]++;
    }


    void __dumpObjects ( void ) {
      unsigned int nb_err=0;
      int total_size=0;

      std::cerr<<std::setfill ( '-' );
      std::cerr<<"|-"<<std::setw ( 50 ) <<""<<"-|-"<<std::setw ( 7 ) <<""<<"-|-"<<std::setw ( 8 ) <<""<<"-|-"<<std::setw ( 8 ) <<""<<"-|"<<std::endl;
      std::cerr<<std::setfill ( ' ' );
      std::cerr<<"| "<<std::setw ( 50 ) <<"   Class Name    "<<" | "<<std::setw ( 7 ) <<"Size"<<" | "<<std::setw ( 8 ) <<"#Const"<<" | "<<std::setw ( 8 ) <<"#Dest"<<" |"<<std::endl;
      std::cerr<<std::setfill ( '-' );
      std::cerr<<"|-"<<std::setw ( 50 ) <<""<<"-|-"<<std::setw ( 7 ) <<""<<"-|-"<<std::setw ( 8 ) <<""<<"-|-"<<std::setw ( 8 ) <<""<<"-|"<<std::endl;
      std::cerr<<std::setfill ( ' ' );
      // list of created objects
      std::vector<std::string> res;

      for ( DEBUG_MAP::const_iterator xx = __creation().begin(); xx != __creation().end(); ++xx ) {
        std::stringstream stream;
        int zeCreatedObjs=xx->second;
        int zeDeletedObjts=-1;
        int size=__sizeof() [xx->first];
        stream<<"| "<<std::setw ( 50 ) <<xx->first<<" | "<<std::setw ( 5 ) <<size<<" o | "<<std::setw ( 8 ) <<zeCreatedObjs<<" | ";

        if ( size>0 ) total_size+=zeCreatedObjs*size;

        try {
          zeDeletedObjts=__deletion() [xx->first];
          stream<<std::setw ( 8 ) <<zeDeletedObjts;
        }
        catch ( NotFound& ) {
          stream<<std::setw ( 8 ) <<"?????";
        }

        stream<<" |";;

        if ( zeCreatedObjs!=zeDeletedObjts ) {
          nb_err+=abs ( zeDeletedObjts-zeCreatedObjs );
          stream<<"<--- failed";
        }

        res.push_back ( stream.str() );
      }

      // list of deleted objects, but not created (?)
      for ( DEBUG_MAP::const_iterator xx = __deletion().begin(); xx != __deletion().end(); ++xx ) {
        try {
          __creation() [xx->first];
        }
        catch ( NotFound& ) {
          std::stringstream stream;
          stream<<"| "<<std::setw ( 50 ) <<xx->first<<" | "<<std::setw ( 7 ) <<__sizeof() [xx->first]<<" | "<<std::setw ( 8 ) <<"?????"<<" | "<<std::setw ( 8 ) <<xx->second<<" |<--- failed";
          res.push_back ( stream.str() );
          nb_err+=xx->second;
        }
      }

      //sort ( res.begin(),res.end() );

      for ( std::vector<std::string>::const_iterator iter=res.begin();
            iter!=res.end();
            ++iter ) {
        std::cerr<<*iter<<std::endl;
      }

      std::cerr<<std::setfill ( '-' );

      std::cerr<<"|-"<<std::setw ( 50 ) <<""<<"-|-"<<std::setw ( 7 ) <<""<<"-|-"<<std::setw ( 8 ) <<""<<"-|-"<<std::setw ( 8 ) <<""<<"-|"<<std::endl;

      std::cerr<<std::setfill ( ' ' );

      if ( nb_err==0 ) {
        std::cerr<<"| "<<std::setw ( 50 ) <<"NO MEMORY LEAK !"<<""<<" | "<<std::setw ( 31 ) <<"|"<<std::endl;
      }
      else {
        std::cerr<<"| "<<std::setw ( 50 ) <<"Memory leaks found "<<""<<" | "<<std::setw ( 16 ) <<nb_err<<" object(s) "<<std::setw ( 4 ) <<"|"<<std::endl;
      }

      std::cerr<<"| "<<std::setw ( 50 ) <<"total "<<""<<" | "<<std::setw ( 17 ) <<total_size<<" octet(s) "<<std::setw ( 4 ) <<"|"<<std::endl;

      std::cerr<<std::setfill ( '-' );

      std::cerr<<"|"<<std::setw ( 85 ) <<"|"<<std::endl;

    }

// take into account static objects in agrum (no called destructor before exit())
    void __staticCorrections() {
      __dec_creation ( "HashTableIterator"  ,"__hash_static_end",0,"static variable correction",0 );
      __dec_creation ( "SetIterator","__empty_edge_set",0,"static variable correction",0 );
      __dec_creation ( "BijectionIterator","__empty_bijection",0,"static variable correction",0 );
      __dec_creation ( "BijectionIterator","__empty_bijection_star",0,"static variable correction",0 );
      __dec_creation ( "Set","__empty_edge_set",0,"static variable correction",0 );
      __dec_creation ( "HashTable"  ,"__empty_edge_set",0,"static variable correction",0 );
    }

    void __atexit ( void ) {
      __staticCorrections();
      __dumpObjects();
      __creation().clear();
      __deletion().clear();
    }


  } /* namespace gum::__debug__ */

#endif //NDEBUG


} /* namespace gum */

#endif // DOXYGEN_SHOULD_SKIP_THIS
