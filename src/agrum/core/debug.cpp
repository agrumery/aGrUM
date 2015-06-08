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

#include <map>

namespace gum {

#ifndef NDEBUG

  namespace __debug__ {
    typedef std::map<std::string, int> DEBUG_MAP;

    // this static hashtable only on debug mode.
    static DEBUG_MAP &__sizeof() {
#if defined(_MT) || defined(__MT__) || defined(_PTHREAD)
#warning "This function is not thread-safe ! (but only in debug mode)"
#endif
      static DEBUG_MAP *sizeOf = new DEBUG_MAP();
      return *sizeOf;
    }

    // this static hashtable only on debug mode.
    static DEBUG_MAP &__creation() {
#if defined(_MT) || defined(__MT__) || defined(_PTHREAD)
#warning "This function is not thread-safe ! (but only in debug mode)"
#endif
      static DEBUG_MAP *creation = new DEBUG_MAP();
      return *creation;
    }

    static DEBUG_MAP &__deletion() {
#if defined(_MT) || defined(__MT__) || defined(_PTHREAD)
#warning "This function is not thread-safe ! (but only in debug mode)"
#endif
      static DEBUG_MAP *deletion = new DEBUG_MAP();
      return *deletion;
    }

    std::string __getFile(const char *f) {
      std::string s(f);
      return s.erase(0, s.rfind("/") + 1);
    }

    void __show_trace(const char *zeKey, const char *zeFile, long zeLine,
                      const char *zeMsg, const void *zePtr) {
#ifdef GUM_DEEP_TRACE_ON
      std::cerr << std::setw(40) << std::setfill(' ') << __getFile(zeFile) << "#"
                << std::setfill('0') << std::setw(5) << std::dec << zeLine << " : "
                << zeMsg << " <" << zeKey << "> [" << std::hex << zePtr << "]"
                << std::dec << std::endl;
#endif // TRACE_CONSTRUCTION_ON
    }

    void __inc_creation(const char *zeKey, const char *zeFile, long zeLine,
                        const char *zeMsg, const void *zePtr, int zeSize) {
      __show_trace(zeKey, zeFile, zeLine, zeMsg, zePtr);

      __creation()[zeKey]++;
      __sizeof()[zeKey] = zeSize;
    }

    // to handle static element of agrum library
    void __dec_creation(const char *zeKey, const char *zeFile, long zeLine,
                        const char *zeMsg, const void *zePtr) {
      __show_trace(zeKey, zeFile, zeLine, zeMsg, zePtr);
      __creation()[zeKey]--;
    }

    void __inc_deletion(const char *zeKey, const char *zeFile, long zeLine,
                        const char *zeMsg, const void *zePtr) {
      __show_trace(zeKey, zeFile, zeLine, zeMsg, zePtr);
      __deletion()[zeKey]++;
    }

    void __dumpObjects(void) {
      unsigned int nb_err = 0;
      int total_size = 0;

      char fillChar = '_';
      int widthColLibelle = 50;
      int widthColSizeOf = 5;
      int widthColItemsNumber = 8;

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
      std::map<std::string, std::string> res;

      for (DEBUG_MAP::const_iterator xx = __creation().begin();
           xx != __creation().end(); ++xx) {
        std::stringstream stream;
        int zeCreatedObjs = xx->second;
        int zeDeletedObjts = -1;
        int size = __sizeof()[xx->first];

        stream << std::setfill(fillChar = (fillChar == '_') ? ' ' : '_') << "| "
               << std::setw(widthColLibelle) << std::left << xx->first << " | "
               << std::right << std::setw(widthColSizeOf) << size << " o | "
               << std::setw(widthColItemsNumber) << zeCreatedObjs << " | ";

        if (size > 0)
          total_size += zeCreatedObjs * size;

        try {
          zeDeletedObjts = __deletion()[xx->first];
          stream << std::setfill(fillChar) << std::setw(widthColItemsNumber)
                 << zeDeletedObjts;
        } catch (NotFound &) {
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
           xx != __deletion().end(); ++xx) {
        try {
          __creation()[xx->first];
        } catch (NotFound &) {
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

      for (const auto iter : res) {
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
                  << " | " << std::setw(widthColSizeOf + widthColItemsNumber * 2 + 9)
                  << ""
                  << "|" << std::endl;
      } else {
        std::cout << "| " << std::setw(widthColLibelle) << "Memory leaks found "
                  << ""
                  << " | " << std::setw(widthColSizeOf + widthColItemsNumber * 2 - 6)
                  << nb_err << " object(s)     "
                  << "|" << std::endl;
      }

      std::cout << "| " << std::setw(widthColLibelle) << "total "
                << " | " << std::setw(widthColSizeOf + widthColItemsNumber * 2 - 4)
                << total_size << " octet(s)    "
                << "|" << std::endl;

      std::cout << std::setfill('=') << "|" << std::setw(widthColLibelle + 2) << ""
                << "|" << std::setw(widthColSizeOf + widthColItemsNumber * 2 + 10)
                << ""
                << "|" << std::endl;
    }

    // take into account static objects in agrum (no called destructor before exit())
    void __staticCorrections() { }

    void __atexit(void) {
      __staticCorrections();
      __dumpObjects();
      __creation().clear();
      __deletion().clear();
    }

  } /* namespace gum::__debug__ */

#endif // NDEBUG

} /* namespace gum */

#endif // DOXYGEN_SHOULD_SKIP_THIS
