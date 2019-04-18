
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#ifndef GUM_DEBUG_H
#define GUM_DEBUG_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// WARNING : Do not include this file directlty : instead include
// <agrum/config.h>

#  include <algorithm>
#  include <iomanip>
#  include <iostream>
#  include <sstream>
#  include <string>
#  include <vector>


#  ifdef GUM_DEBUG_MODE
#    ifndef GUM_TRACE_ON
#      define GUM_TRACE_ON   // in DEBUG MODE we force TRACE to be ON
#    else   // GUM_TRACE_ON on mode debug add  TRACE_CONSTRUCTION_ON (tracing
// construction/destruction of object)
#      define GUM_DEEP_TRACE_ON
#    endif   // GUM_TRACE_ON

#    define GUM_ASSERT(condition)                                         \
      do {                                                                \
        if (!(condition)) {                                               \
          std::cout << std::endl                                          \
                    << __FILE__ << ":" << __LINE__ << " [aGrUM] assert (" \
                    << #condition << ") failed" << std::endl;             \
          std::abort();                                                   \
        }                                                                 \
      } while (0)

#    define GUM_DEBUG_ONLY(x) \
      { x }

// FOR EXPANSION OF MACRO IN ARGS OF GUM_CONSTRUCTOR, WE NEED TO USE A 2-LEVEL
// DEFINITION OF GUM_CONSTRUCTOR
#    define GUM_CONSTRUCTOR_BASIC(x)                                          \
      {                                                                       \
        gum::__debug__::__inc_creation(                                       \
           #x, __FILE__, __LINE__, "constructor of", (void*)this, sizeof(x)); \
      }
#    define GUM_CONSTRUCTOR(x) GUM_CONSTRUCTOR_BASIC(x)

// FOR EXPANSION OF MACRO IN ARGS OF GUM_DESTRUCTOR, WE NEED TO USE A 2-LEVEL
// DEFINITION OF GUM_DESTRUCTOR
#    define GUM_DESTRUCTOR_BASIC(x)                               \
      {                                                           \
        gum::__debug__::__inc_deletion(                           \
           #x, __FILE__, __LINE__, "destructor of", (void*)this); \
      }
#    define GUM_DESTRUCTOR(x) GUM_DESTRUCTOR_BASIC(x)

// FOR EXPANSION OF MACRO IN ARGS OF GUM_CONS_CPY, WE NEED TO USE A 2-LEVEL
// DEFINITION OF GUM_CONS_CPY
#    define GUM_CONS_CPY_BASIC(x)                             \
      {                                                       \
        gum::__debug__::__inc_creation(#x,                    \
                                       __FILE__,              \
                                       __LINE__,              \
                                       "copy constructor of", \
                                       (void*)this,           \
                                       sizeof(x));            \
      }
#    define GUM_CONS_CPY(x) GUM_CONS_CPY_BASIC(x)

// FOR EXPANSION OF MACRO IN ARGS OF GUM_CONS_MOV, WE NEED TO USE A 2-LEVEL
// DEFINITION OF GUM_CONS_MOV
#    define GUM_CONS_MOV_BASIC(x)                             \
      {                                                       \
        gum::__debug__::__inc_creation(#x,                    \
                                       __FILE__,              \
                                       __LINE__,              \
                                       "move constructor of", \
                                       (void*)this,           \
                                       sizeof(x));            \
      }
#    define GUM_CONS_MOV(x) GUM_CONS_MOV_BASIC(x)

// FOR EXPANSION OF MACRO IN ARGS OF GUM_CONSTRUCTOR, WE NEED TO USE A 2-LEVEL
// DEFINITION OF GUM_CONSTRUCTOR
#    define GUM_OP_CPY_BASIC(x)                                      \
      {                                                              \
        gum::__debug__::__show_trace(                                \
           #x, __FILE__, __LINE__, "copy operator of", (void*)this); \
      }
#    define GUM_OP_CPY(x) GUM_OP_CPY_BASIC(x)
// FOR EXPANSION OF MACRO IN ARGS OF GUM_CONSTRUCTOR, WE NEED TO USE A 2-LEVEL
// DEFINITION OF GUM_CONSTRUCTOR
#    define GUM_OP_MOV_BASIC(x)                                      \
      {                                                              \
        gum::__debug__::__show_trace(                                \
           #x, __FILE__, __LINE__, "move operator of", (void*)this); \
      }
#    define GUM_OP_MOV(x) GUM_OP_MOV_BASIC(x)
/////////////////////////////////////////////////////////////
#  else   // GUM_DEBUG_MODE
/////////////////////////////////////////////////////////////
#    define GUM_ASSERT(condition) ((void)0)
#    define GUM_CONSTRUCTOR(x) ((void)0)
#    define GUM_DESTRUCTOR(x) ((void)0)
#    define GUM_CONS_CPY(x) ((void)0)
#    define GUM_CONS_MOV(x) ((void)0)
#    define GUM_OP_CPY(x) ((void)0)
#    define GUM_OP_MOV(x) ((void)0)

#    define GUM_DEBUG_ONLY(x)
/////////////////////////////////////////////////////////////
#  endif   // GUM_DEBUG_MODE

#  ifdef GUM_TRACE_ON
#    define GUM__PRINT(file, line, x) \
      { std::cout << std::endl << file << ":" << line << x << std::endl; }

#    define GUM_CHECKPOINT GUM__PRINT(__FILE__, __LINE__, " GUMcheckpoint")
#    define GUM_TRACE(x) \
      GUM__PRINT(__FILE__, __LINE__, std::endl << "   --> GUMtrace: " << x)
#    define GUM_TRACE_VAR(x) \
      GUM__PRINT(__FILE__,   \
                 __LINE__,   \
                 std::endl   \
                    << "   --> GUMtrace of <" << #x << ">: " << x)

#    define GUM_TRACE_NEWLINE \
      { std::cout << std::endl; }
#  else   // GUM_TRACE_ON
#    define GUM__PRINT(line, file, x)
#    define GUM_CHECKPOINT
#    define GUM_TRACE(x)
#    define GUM_TRACE_VAR(x)
#    define GUM_TRACE_NEWLINE
#  endif   // GUM_TRACE_ON

namespace gum {

#  ifdef GUM_DEBUG_MODE

  namespace __debug__ {

    std::string __getFile(const char* f);

    void __show_trace(const char* zeKey,
                      const char* zeFile,
                      long        zeLine,
                      const char* zeMsg,
                      const void* zePtr);
    void __inc_creation(const char* zeKey,
                        const char* zeFile,
                        long        zeLine,
                        const char* zeMsg,
                        const void* zePtr,
                        int         zeSize = -1);
    void __inc_deletion(const char* zeKey,
                        const char* zeFile,
                        long        zeLine,
                        const char* zeMsg,
                        const void* zePtr);
    void __dec_creation(const char* zeKey,
                        const char* zeFile,
                        long        zeLine,
                        const char* zeMsg,
                        const void* zePtr);
    void __dumpObjects();
    void __atexit();

  }   // namespace __debug__

#  endif   // GUM_DEBUG_MODE

  // ===========================================================================
  // ===         A CLASS USED FOR MAKING VALGRIND HAPPY IN DEBUG MODE        ===
  // ===========================================================================

  class Debug : public std::string {
    public:
    Debug(const std::string& str) : std::string(str) {}

    Debug(const char* const str) : std::string(str) {}
  };

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS

#endif   // GUM_DEBUG_H
