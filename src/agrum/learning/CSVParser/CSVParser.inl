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

#include <agrum/learning/CSVParser/CSVParser.h>

namespace gum {

  INLINE bool CSVParser::next() {
    while (getline(__in,__line)) {
      if (__line.size() == 0) continue;

      // fast recognition of commented lines
      Size lastPos = __line.find_first_not_of( __delimiterPlusSpaces, 0 );
      if (__line.at(lastPos) == __commentMarker) continue;

      __tokenize(__line);
      return true;
    }

    return false;
  }

  INLINE const std::vector<std::string> &CSVParser::currentData()  const {
    if (__emptyData) GUM_ERROR(NullElement,"No parsed data");
    return __data;
  }
}
