/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
#ifndef GUM_DEBUG_H
#define GUM_DEBUG_H

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cassert>



#define GUM_CHECKPOINT {                                                \
    std::cerr << __FILE__ << ":" << __LINE__ <<": warning : aGrUM checkpoint"<< std::endl; }

#define GUM_TRACE(x) {                                                  \
    std::cerr << __FILE__ << ":" << __LINE__ <<": trace : "<<x << std::endl; }

#define GUM_TRACE_NEWLINE {std::cerr << std::endl;}
#define GUM_TRACE_VAR(x) {                                              \
    std::cerr << __FILE__ << ":" << __LINE__ <<": trace "<<#x<<": "<<x << std::endl; }

namespace gum {


  namespace debug {

    std::string __getFile( const char* f );


    /////////////////////////////////////////////////////////////
    void __show_trace( const char *zeKey, const char *zeFile, long zeLine,
                       const char *zeMsg, const void *zePtr );
    void __inc_creation( const char *zeKey, const char *zeFile, long zeLine,
                         const char *zeMsg, const void *zePtr,int zeSize=-1 );
    void __inc_deletion( const char *zeKey, const char *zeFile, long zeLine,
                         const char *zeMsg, const void *zePtr );
    void __dumpObjects( void );
    void __atexit( void );


#ifndef NDEBUG
    // FLAG : verbose mode
#undef TRACE_ON
//     #define TRACE_ON
#define GUM_ASSERT(condition) { assert(condition); }
#define GUM_DEBUG(x) {x}
#ifdef TRACE_ON
#define GUM_DEBUG_TRACE(x) {                                            \
      std::cerr << std::setw(20) << std::setfill(' ') << gum::debug::__getFile(__FILE__) << \
        "#" << std::setfill('0') << std::setw(5) << std::dec <<__LINE__ << " : " << \
        x << std::endl; }
#else
#define GUM_DEBUG_TRACE(x)
#endif

// FOR EXPANSION OF MACRO IN ARGS OF GUM_CONSTRUCTOR, WE NEED TO USE A 2-LEVEL DEFINITION OF GUM_CONSTRUCTOR
#define GUM_CONSTRUCTOR_BASIC(x) { GUM_DEBUG_TRACE("Création de " #x)                                           \
      gum::debug::__inc_creation (#x,__FILE__,__LINE__, "constructor of",(void *)this,sizeof(x)); }
#define GUM_CONSTRUCTOR(x) GUM_CONSTRUCTOR_BASIC(x)

// FOR EXPANSION OF MACRO IN ARGS OF GUM_DESTRUCTOR, WE NEED TO USE A 2-LEVEL DEFINITION OF GUM_DESTRUCTOR
#define GUM_DESTRUCTOR_BASIC(x) {                                             \
      gum::debug::__inc_deletion(#x,__FILE__,__LINE__,"destructor of",(void *)this); }
#define GUM_DESTRUCTOR(x) GUM_DESTRUCTOR_BASIC(x)

// FOR EXPANSION OF MACRO IN ARGS OF GUM_CONS_CPY, WE NEED TO USE A 2-LEVEL DEFINITION OF GUM_CONS_CPY
#define GUM_CONS_CPY_BASIC(x) {                                               \
      gum::debug::__inc_creation(#x,__FILE__,__LINE__,"copy constructor of",(void*)this,sizeof(x));}
#define GUM_CONS_CPY(x) GUM_CONS_CPY_BASIC(x)

// FOR EXPANSION OF MACRO IN ARGS OF GUM_CONSTRUCTOR, WE NEED TO USE A 2-LEVEL DEFINITION OF GUM_CONSTRUCTOR
#define GUM_OP_CPY_BASIC(x) {                                                 \
      gum::debug::__show_trace(#x,__FILE__,__LINE__,"copy operator of",(void *)this); }
#define GUM_OP_CPY(x) GUM_OP_CPY_BASIC(x)
    /////////////////////////////////////////////////////////////
#else //NDEBUG
    /////////////////////////////////////////////////////////////
#define GUM_ASSERT(condition)
#define GUM_DEBUG(x)
#define GUM_DEBUG_TRACE(x)
#define GUM_CONSTRUCTOR(x)
#define GUM_DESTRUCTOR(x)
#define GUM_CONS_CPY(x)
#define GUM_OP_CPY(x)
    /////////////////////////////////////////////////////////////
#endif //NDEBUG


  } /* namespace gum::debug */


  /* =========================================================================== */
  /* ===         A CLASS USED FOR MAKING VALGRIND HAPPY IN DEBUG MODE        === */
  /* =========================================================================== */
  class Debug : public std::string {
  public:
    Debug( const std::string& str ) : std::string( str ) {}

    Debug( const char* const str ) : std::string( str ) {}
  };


} /* namespace gum */


#endif // GUM_DEBUG_H
