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
// WARNING : Do not include this file directlty : instead include <agrum/config.h>

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cassert>

#ifndef NDEBUG

#ifndef GUM_TRACE_ON
#define GUM_TRACE_ON // in DEBUG MODE we force TRACE to be ON
#else                // GUM_TRACE_ON on mode debug add  TRACE_CONSTRUCTION_ON (tracing
// construction/destruction of object)
#define GUM_DEEP_TRACE_ON
#endif // GUM_TRACE_ON

#define GUM_ASSERT(condition)                                                       \
  { assert(condition); }
#define GUM_DEBUG_ONLY(x)                                                           \
  { x }

// FOR EXPANSION OF MACRO IN ARGS OF GUM_CONSTRUCTOR, WE NEED TO USE A 2-LEVEL
// DEFINITION OF GUM_CONSTRUCTOR
#define GUM_CONSTRUCTOR_BASIC(x)                                                    \
  {                                                                                 \
    gum::__debug__::__inc_creation(#x, __FILE__, __LINE__, "constructor of",        \
                                   (void *) this, sizeof(x));                       \
  }
#define GUM_CONSTRUCTOR(x) GUM_CONSTRUCTOR_BASIC(x)

// FOR EXPANSION OF MACRO IN ARGS OF GUM_DESTRUCTOR, WE NEED TO USE A 2-LEVEL
// DEFINITION OF GUM_DESTRUCTOR
#define GUM_DESTRUCTOR_BASIC(x)                                                     \
  {                                                                                 \
    gum::__debug__::__inc_deletion(#x, __FILE__, __LINE__, "destructor of",         \
                                   (void *) this);                                  \
  }
#define GUM_DESTRUCTOR(x) GUM_DESTRUCTOR_BASIC(x)

// FOR EXPANSION OF MACRO IN ARGS OF GUM_CONS_CPY, WE NEED TO USE A 2-LEVEL
// DEFINITION OF GUM_CONS_CPY
#define GUM_CONS_CPY_BASIC(x)                                                       \
  {                                                                                 \
    gum::__debug__::__inc_creation(#x, __FILE__, __LINE__, "copy constructor of",   \
                                   (void *) this, sizeof(x));                       \
  }
#define GUM_CONS_CPY(x) GUM_CONS_CPY_BASIC(x)

// FOR EXPANSION OF MACRO IN ARGS OF GUM_CONS_MOV, WE NEED TO USE A 2-LEVEL
// DEFINITION OF GUM_CONS_MOV
#define GUM_CONS_MOV_BASIC(x)                                                       \
  {                                                                                 \
    gum::__debug__::__inc_creation(#x, __FILE__, __LINE__, "move constructor of",   \
                                   (void *) this, sizeof(x));                       \
  }
#define GUM_CONS_MOV(x) GUM_CONS_MOV_BASIC(x)

// FOR EXPANSION OF MACRO IN ARGS OF GUM_CONSTRUCTOR, WE NEED TO USE A 2-LEVEL
// DEFINITION OF GUM_CONSTRUCTOR
#define GUM_OP_CPY_BASIC(x)                                                         \
  {                                                                                 \
    gum::__debug__::__show_trace(#x, __FILE__, __LINE__, "copy operator of",        \
                                 (void *) this);                                    \
  }
#define GUM_OP_CPY(x) GUM_OP_CPY_BASIC(x)
// FOR EXPANSION OF MACRO IN ARGS OF GUM_CONSTRUCTOR, WE NEED TO USE A 2-LEVEL
// DEFINITION OF GUM_CONSTRUCTOR
#define GUM_OP_MOV_BASIC(x)                                                         \
  {                                                                                 \
    gum::__debug__::__show_trace(#x, __FILE__, __LINE__, "move operator of",        \
                                 (void *) this);                                    \
  }
#define GUM_OP_MOV(x) GUM_OP_MOV_BASIC(x)
/////////////////////////////////////////////////////////////
#else // NDEBUG
/////////////////////////////////////////////////////////////
#define GUM_ASSERT(condition)
#define GUM_DEBUG_ONLY(x)
#define GUM_CONSTRUCTOR(x)
#define GUM_DESTRUCTOR(x)
#define GUM_CONS_CPY(x)
#define GUM_CONS_MOV(x)
#define GUM_OP_CPY(x)
#define GUM_OP_MOV(x)
/////////////////////////////////////////////////////////////
#endif // NDEBUG

#ifdef GUM_TRACE_ON
#define GUM_CHECKPOINT                                                              \
  {                                                                                 \
    std::cerr << std::endl << __FILE__ << ":" << __LINE__                           \
              << ": warning : aGrUM checkpoint" << std::endl;                       \
  }

#define GUM_TRACE(x)                                                                \
  {                                                                                 \
    std::cerr << std::endl << __FILE__ << ":" << __LINE__ << ": trace : " << x      \
              << std::endl;                                                         \
  }
#define GUM_TRACE_VAR(x)                                                            \
  {                                                                                 \
    std::cerr << std::endl << __FILE__ << ":" << __LINE__ << ": trace " << #x       \
              << ": " << x << std::endl;                                            \
  }

#define GUM_TRACE_NEWLINE                                                           \
  { std::cerr << std::endl; }
#else // GUM_TRACE_ON
#define GUM_CHECKPOINT
#define GUM_TRACE(x)
#define GUM_TRACE_VAR(x)
#define GUM_TRACE_NEWLINE
#endif // GUM_TRACE_ON

namespace gum {

#ifndef NDEBUG

  namespace __debug__ {

    std::string __getFile(const char *f);

    /////////////////////////////////////////////////////////////
    void __show_trace(const char *zeKey, const char *zeFile, long zeLine,
                      const char *zeMsg, const void *zePtr);
    void __inc_creation(const char *zeKey, const char *zeFile, long zeLine,
                        const char *zeMsg, const void *zePtr, int zeSize = -1);
    void __inc_deletion(const char *zeKey, const char *zeFile, long zeLine,
                        const char *zeMsg, const void *zePtr);
    void __dec_creation(const char *zeKey, const char *zeFile, long zeLine,
                        const char *zeMsg, const void *zePtr);
    void __dumpObjects(void);
    void __atexit(void);

  } /* namespace gum::__debug__ */

#endif // NDEBUG

  /* =========================================================================== */
  /* ===         A CLASS USED FOR MAKING VALGRIND HAPPY IN DEBUG MODE        === */
  /* =========================================================================== */

  class Debug : public std::string {
    public:
    Debug(const std::string &str) : std::string(str) {}

    Debug(const char *const str) : std::string(str) {}
  };

} /* namespace gum */

#endif // GUM_DEBUG_H
