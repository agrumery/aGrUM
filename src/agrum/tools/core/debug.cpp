
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
 *   info_at_agrum_dot_org
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


/**
 * @file
 * @brief debugging functions
 *
 * @author Pierre-Henri WUILLEMIN (@LIP6) and Christophe GONZALES (@AMU)
 */

#include <agrum/agrum.h>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <mutex>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <map>

namespace gum {

#  ifdef GUM_DEBUG_MODE

  namespace __debug__ {
    typedef std::map< std::string, int > DEBUG_MAP;

    static std::mutex debug_mutex;

    // this static hashtable only on debug mode.
    static DEBUG_MAP& __sizeof() {
      // This function is not thread-safe ! (but only in debug mode)
      static DEBUG_MAP* sizeOf = new DEBUG_MAP();
      return *sizeOf;
    }

    // this static hashtable only on debug mode.
    static DEBUG_MAP& __creation() {
      // @todo This function is not thread-safe ! (but only in debug mode)
      static DEBUG_MAP* creation = new DEBUG_MAP();
      return *creation;
    }

    static DEBUG_MAP& __deletion() {
      // @todo This function is not thread-safe ! (but only in debug mode)
      static DEBUG_MAP* deletion = new DEBUG_MAP();
      return *deletion;
    }

    std::string __getFile(const char* f) {
      std::string s(f);
      return s.erase(0, s.rfind("/") + 1);
    }

    void __show_trace(const char* zeKey,
                      const char* zeFile,
                      long        zeLine,
                      const char* zeMsg,
                      const void* zePtr) {
#    ifdef GUM_DEEP_TRACE_ON
      std::cerr << std::setw(40) << std::setfill(' ') << __getFile(zeFile) << "#"
                << std::setfill('0') << std::setw(5) << std::dec << zeLine << " : "
                << zeMsg << " <" << zeKey << "> [" << std::hex << zePtr << "]"
                << std::dec << std::endl;
#    endif   // TRACE_CONSTRUCTION_ON
    }

    void __inc_creation(const char* zeKey,
                        const char* zeFile,
                        long        zeLine,
                        const char* zeMsg,
                        const void* zePtr,
                        int         zeSize) {
      debug_mutex.lock();
      __show_trace(zeKey, zeFile, zeLine, zeMsg, zePtr);
      __creation()[zeKey]++;
      __sizeof()[zeKey] = zeSize;
      debug_mutex.unlock();
    }

    // to handle static element of agrum library
    void __dec_creation(const char* zeKey,
                        const char* zeFile,
                        long        zeLine,
                        const char* zeMsg,
                        const void* zePtr) {
      debug_mutex.lock();
      __show_trace(zeKey, zeFile, zeLine, zeMsg, zePtr);
      __creation()[zeKey]--;
      debug_mutex.unlock();
    }

    void __inc_deletion(const char* zeKey,
                        const char* zeFile,
                        long        zeLine,
                        const char* zeMsg,
                        const void* zePtr) {
      debug_mutex.lock();
      __show_trace(zeKey, zeFile, zeLine, zeMsg, zePtr);
      __deletion()[zeKey]++;
      debug_mutex.unlock();
    }

    void __dumpObjects() {
      Size   nb_err = 0;
      double total_size = 0.0;

      char fillChar = '_';
      int  widthColLibelle = 50;
      int  widthColSizeOf = 5;
      int  widthColItemsNumber = 8;

      std::cout << std::setfill('=') << "|" << std::setw(widthColLibelle + 2) << ""
                << "|" << std::setw(widthColSizeOf + 4) << ""
                << "|" << std::setw(widthColItemsNumber + 2) << ""
                << "|" << std::setw(widthColItemsNumber + 2) << ""
                << "|" << std::endl;
      std::cout << std::setfill(' ') << "| " << std::left
                << std::setw(widthColLibelle) << "Class Name" << std::right
                << " |   " << std::setw(widthColSizeOf) << "Size"
                << " | " << std::setw(widthColItemsNumber) << "#Const"
                << " | " << std::setw(widthColItemsNumber) << "#Dest"
                << " |" << std::endl;
      std::cout << std::setfill('-') << "|" << std::setw(widthColLibelle + 2) << ""
                << "|" << std::setw(widthColSizeOf + 4) << ""
                << "|" << std::setw(widthColItemsNumber + 2) << ""
                << "|" << std::setw(widthColItemsNumber + 2) << ""
                << "|" << std::endl;
      // list of created objects
      std::map< std::string, std::string > res;

      for (DEBUG_MAP::const_iterator xx = __creation().begin();
           xx != __creation().end();
           ++xx) {
        std::stringstream stream;
        int               zeCreatedObjs = xx->second;
        int               zeDeletedObjts = -1;
        int               size = __sizeof()[xx->first];

        stream << std::setfill(fillChar = (fillChar == '_') ? ' ' : '_') << "| "
               << std::setw(widthColLibelle) << std::left << xx->first << " | "
               << std::right << std::setw(widthColSizeOf) << size << " o | "
               << std::setw(widthColItemsNumber) << zeCreatedObjs << " | ";

        if (size > 0) total_size += zeCreatedObjs * (size / 1024.0);

        try {
          zeDeletedObjts = __deletion()[xx->first];
          stream << std::setfill(fillChar) << std::setw(widthColItemsNumber)
                 << zeDeletedObjts;
        } catch (NotFound&) {
          stream << std::setfill(fillChar) << std::setw(widthColItemsNumber)
                 << "?????";
        }

        stream << " |";
        ;

        if (zeCreatedObjs != zeDeletedObjts) {
          nb_err += std::abs(zeDeletedObjts - zeCreatedObjs);
          stream << "<--- failed";
        }

        res.insert(make_pair(xx->first, stream.str()));
        // res.push_back( stream.str() );
      }

      // list of deleted objects, but not created (?)
      for (DEBUG_MAP::const_iterator xx = __deletion().begin();
           xx != __deletion().end();
           ++xx) {
        try {
          __creation()[xx->first];
        } catch (NotFound&) {
          std::stringstream stream;
          fillChar = (fillChar == '_') ? ' ' : '_';
          stream << std::setfill(fillChar = (fillChar == '_') ? ' ' : '_') << "| "
                 << std::setw(widthColLibelle) << std::left << xx->first + " "
                 << " | " << std::right << std::setw(widthColSizeOf)
                 << __sizeof()[xx->first] << " o | "
                 << std::setw(widthColItemsNumber) << "?????"
                 << " | " << std::setw(widthColItemsNumber) << xx->second
                 << " |<--- failed";
          res.insert(make_pair(xx->first, stream.str()));
          // res.push_back( stream.str() );
          nb_err += xx->second;
        }
      }

      for (const auto iter: res) {
        std::cout << iter.second << std::endl;
      }

      std::cout << std::setfill('-');

      std::cout << "|-" << std::setw(widthColLibelle) << ""
                << "-|-" << std::setw(widthColSizeOf + 2) << ""
                << "-|-" << std::setw(widthColItemsNumber) << ""
                << "-|-" << std::setw(widthColItemsNumber) << ""
                << "-|" << std::endl;

      std::cout << std::setfill(' ');

      if (nb_err == 0) {
        std::cout << "| " << std::setw(widthColLibelle) << "NO MEMORY LEAK !"
                  << " | "
                  << std::setw(widthColSizeOf + widthColItemsNumber * 2 + 9) << ""
                  << "|" << std::endl;
      } else {
        std::cout << "| " << std::setw(widthColLibelle) << "Memory leaks found "
                  << ""
                  << " | "
                  << std::setw(widthColSizeOf + widthColItemsNumber * 2 - 6)
                  << nb_err << " object(s)     "
                  << "|" << std::endl;
      }

      std::cout << "| " << std::setw(widthColLibelle) << "total "
                << " | " << std::fixed
                << std::setw(widthColSizeOf + widthColItemsNumber * 2 - 4)
                << std::setprecision(2) << total_size << " Ko          "
                << "|" << std::endl;

      std::cout << std::setfill('=') << "|" << std::setw(widthColLibelle + 2) << ""
                << "|" << std::setw(widthColSizeOf + widthColItemsNumber * 2 + 10)
                << ""
                << "|" << std::endl;
    }

    // take into account static objects in agrum (no called destructor before
    // exit())
    void __staticCorrections() {}

    void __atexit() {
      __staticCorrections();
      __dumpObjects();
      __creation().clear();
      __deletion().clear();
    }

  }   // namespace __debug__

#  endif   // GUM_DEBUG_MODE

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS