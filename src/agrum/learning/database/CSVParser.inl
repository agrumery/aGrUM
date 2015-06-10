/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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

#include <agrum/learning/database/CSVParser.h>

namespace gum {

  namespace learning {

    INLINE bool CSVParser::next() {
      while (getline(__in, __line)) {
        __noLine++;

        if (__line.size() == 0)
          continue;

        // fast recognition of commented or empty lines lines
        Size lastPos = __line.find_first_not_of(__spaces, 0);

        if (lastPos == std::string::npos)
          continue;

        if (__line.at(lastPos) == __commentMarker)
          continue;

        __tokenize(__line);
        return true;
      }

      return false;
    }

    // search for quote taking into account the '\'...
    INLINE Size CSVParser::__correspondingQuoteMarker(const std::string &str,
                                                      Size pos) const {
      Size res = pos, before;

      while (true) {
        res = str.find_first_of(__quoteMarker, res + 1);

        if (res == std::string::npos)
          return res; // no quote found

        before = str.find_last_not_of('\\', res - 1);

        if (before == std::string::npos)
          return res; // quote found, it is the good one

        if ((res - before) % 2 == 1)
          return res; // the quote is the good one, even if there are some '\' before
      }
    }

    INLINE const std::vector<std::string> &CSVParser::current() const {
      if (__emptyData)
        GUM_ERROR(NullElement, "No parsed data");

      return __data;
    }

    INLINE const Size CSVParser::noLine() const {
      if (__noLine == 0)
        GUM_ERROR(NullElement, "No parsed data");

      return __noLine;
    }

  } // namespace learning

} // namespace gum
