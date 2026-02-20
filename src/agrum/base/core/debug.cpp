/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief debugging functions
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>

#include <agrum/agrum.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <map>

namespace gum {

#  ifdef GUM_DEBUG_MODE

  namespace __debug__ {
    using DEBUG_MAP = std::map< std::string, int >;

    static std::mutex& _debug_mutex_() {
      // Here, this initialization is thread-safe due to Meyer’s Singleton property
      static std::mutex debug_mutex;
      return debug_mutex;
    }

    // this static hashtable only on debug mode.
    static DEBUG_MAP& _sizeof_() {
      // Here, this initialization is thread-safe due to Meyer’s Singleton property
      static DEBUG_MAP sizeOf;
      return sizeOf;
    }

    // this static hashtable only on debug mode.
    static DEBUG_MAP& _creation_() {
      // Here, this initialization is thread-safe due to Meyer’s Singleton property
      static DEBUG_MAP creation;
      return creation;
    }

    static DEBUG_MAP& _deletion_() {
      // Here, this initialization is thread-safe due to Meyer’s Singleton property
      static DEBUG_MAP deletion;
      return deletion;
    }

    std::string _getFile_(const char* f) {
      std::string s(f);
      return s.erase(0, s.rfind('/') + 1);
    }

    void _show_trace_(const char* zeKey,
                      const char* zeFile,
                      long        zeLine,
                      const char* zeMsg,
                      const void* zePtr) {
#    ifdef GUM_DEEP_TRACE_ON
      std::cerr << std::setw(40) << std::setfill(' ') << _getFile_(zeFile) << "#"
                << std::setfill('0') << std::setw(5) << std::dec << zeLine << " : " << zeMsg << " <"
                << zeKey << "> [" << std::hex << zePtr << "]" << std::dec << std::endl;
#    endif   // TRACE_CONSTRUCTION_ON
    }

    void _inc_creation_(const char* zeKey,
                        const char* zeFile,
                        long        zeLine,
                        const char* zeMsg,
                        const void* zePtr,
                        int         zeSize) {
      static DEBUG_MAP& creation = _creation_();
      static DEBUG_MAP& size     = _sizeof_();
      std::unique_lock  lck{_debug_mutex_()};

      _show_trace_(zeKey, zeFile, zeLine, zeMsg, zePtr);
      creation[zeKey]++;
      size[zeKey] = zeSize;
    }

    // to handle static element of agrum library
    void _dec_creation_(const char* zeKey,
                        const char* zeFile,
                        long        zeLine,
                        const char* zeMsg,
                        const void* zePtr) {
      static DEBUG_MAP& creation = _creation_();
      std::unique_lock  lck{_debug_mutex_()};

      _show_trace_(zeKey, zeFile, zeLine, zeMsg, zePtr);
      creation[zeKey]--;
    }

    void _inc_deletion_(const char* zeKey,
                        const char* zeFile,
                        long        zeLine,
                        const char* zeMsg,
                        const void* zePtr) {
      static DEBUG_MAP& deletion = _deletion_();
      std::unique_lock  lck{_debug_mutex_()};

      _show_trace_(zeKey, zeFile, zeLine, zeMsg, zePtr);
      deletion[zeKey]++;
    }

    void _dumpObjects_() {
      DEBUG_MAP& creation = _creation_();
      DEBUG_MAP& deletion = _deletion_();
      DEBUG_MAP& sizeOf   = _sizeof_();

      Size   nb_err     = 0;
      double total_size = 0.0;

      char fillChar            = '_';
      int  widthColLibelle     = 50;
      int  widthColSizeOf      = 5;
      int  widthColItemsNumber = 8;

      std::cout << std::setfill('=') << "|" << std::setw(widthColLibelle + 2) << ""
                << "|" << std::setw(widthColSizeOf + 4) << ""
                << "|" << std::setw(widthColItemsNumber + 2) << ""
                << "|" << std::setw(widthColItemsNumber + 2) << ""
                << "|" << std::endl;
      std::cout << std::setfill(' ') << "| " << std::left << std::setw(widthColLibelle)
                << "Class Name" << std::right << " |   " << std::setw(widthColSizeOf) << "Size"
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

      for (DEBUG_MAP::const_iterator xx = creation.begin(); xx != creation.end(); ++xx) {
        std::stringstream stream;
        int               zeCreatedObjs  = xx->second;
        int               zeDeletedObjts = -1;
        int               size           = sizeOf[xx->first];

        stream << std::setfill(fillChar = (fillChar == '_') ? ' ' : '_') << "| "
               << std::setw(widthColLibelle) << std::left << xx->first << " | " << std::right
               << std::setw(widthColSizeOf) << size << " o | " << std::setw(widthColItemsNumber)
               << zeCreatedObjs << " | ";

        if (size > 0) total_size += zeCreatedObjs * (size / 1024.0);

        // check if xx->first exists as key
        if (auto zeDeletedObjtsEntry = deletion.find(xx->first);
            zeDeletedObjtsEntry != deletion.end()) {
          zeDeletedObjts = zeDeletedObjtsEntry->second;
          stream << std::setfill(fillChar) << std::setw(widthColItemsNumber) << zeDeletedObjts;
        } else {
          stream << std::setfill(fillChar) << std::setw(widthColItemsNumber) << "?????";
        }

        stream << " |";

        if (zeCreatedObjs != zeDeletedObjts) {
          nb_err += std::abs(zeDeletedObjts - zeCreatedObjs);
          stream << "<--- failed";
        }

        res.insert(make_pair(xx->first, stream.str()));
      }

      // list of deleted objects, but not created (?)
      for (DEBUG_MAP::const_iterator xx = deletion.begin(); xx != deletion.end(); ++xx) {
        if (auto creationValAtKey = creation.find(xx->first);
            creationValAtKey != creation.end()) {
          std::stringstream stream;
          fillChar = (fillChar == '_') ? ' ' : '_';
          stream << std::setfill(fillChar = (fillChar == '_') ? ' ' : '_') << "| "
                 << std::setw(widthColLibelle) << std::left << xx->first + " " << " | "
                 << std::right << std::setw(widthColSizeOf) << sizeOf[xx->first] << " o | "
                 << std::setw(widthColItemsNumber) << "?????"
                 << " | " << std::setw(widthColItemsNumber) << xx->second << " |<--- failed";
          res.insert(make_pair(xx->first, stream.str()));

          nb_err += xx->second;
        }
      }

      for (const auto& [fisrt, second]: res)
        std::cout << second << std::endl;


      std::cout << std::setfill('-');

      std::cout << "|-" << std::setw(widthColLibelle) << ""
                << "-|-" << std::setw(widthColSizeOf + 2) << ""
                << "-|-" << std::setw(widthColItemsNumber) << ""
                << "-|-" << std::setw(widthColItemsNumber) << ""
                << "-|" << std::endl;

      std::cout << std::setfill(' ');

      if (nb_err == 0) {
        std::cout << "| " << std::setw(widthColLibelle) << "NO MEMORY LEAK !"
                  << " | " << std::setw(widthColSizeOf + widthColItemsNumber * 2 + 9) << ""
                  << "|" << std::endl;
      } else {
        std::cout << "| " << std::setw(widthColLibelle) << "Memory leaks found "
                  << ""
                  << " | " << std::setw(widthColSizeOf + widthColItemsNumber * 2 - 6) << nb_err
                  << " object(s)     "
                  << "|" << std::endl;
      }

      std::cout << "| " << std::setw(widthColLibelle) << "total "
                << " | " << std::fixed << std::setw(widthColSizeOf + widthColItemsNumber * 2 - 4)
                << std::setprecision(2) << total_size << " Ko          "
                << "|" << std::endl;

      std::cout << std::setfill('=') << "|" << std::setw(widthColLibelle + 2) << ""
                << "|" << std::setw(widthColSizeOf + widthColItemsNumber * 2 + 10) << ""
                << "|" << std::endl;
    }

    // take into account static objects in agrum (no called destructor before
    // exit())
    void _staticCorrections_() {
      /// for emptyNodeSet
      _inc_deletion_("HashTable", "GraphElements.cpp", 40, "destructor of", nullptr);
      _inc_deletion_("Set", "GraphElements.cpp", 40, "destructor of", nullptr);
    }

    void _atexit_() {
      _staticCorrections_();
      _dumpObjects_();
      _creation_().clear();
      _deletion_().clear();
    }

  }   // namespace  __debug__

#  endif   // GUM_DEBUG_MODE

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
