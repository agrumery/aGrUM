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

#ifdef GUM_NO_INLINE
#include <agrum/learning/CSVParser/CSVParser.inl>
#endif /* GUM_NO_INLINE */

namespace gum {

  CSVParser::CSVParser( std::istream & in, const std::string& delimiter, const char  commentmarker ) :
      __line(),
      __delimiter( delimiter ),
      __spaces( " \t" ),
      __delimiterPlusSpaces( __delimiter+__spaces ),
      __commentMarker( commentmarker ),
      __in( in ),
      __data(),
      __emptyData( true ) {
    GUM_CONSTRUCTOR( CSVParser );
  }

  CSVParser::~CSVParser() {
    GUM_DESTRUCTOR( CSVParser );
  }

  void CSVParser::__tokenize( const std::string& s) {
    // removing comment part
    Size pos_truncate_comment = s.find_first_of( __commentMarker,0 );
    std::string str =s.substr(0,pos_truncate_comment);
    GUM_TRACE_VAR("<"+str+">");

    Size counter=0;

    Size last_pos = str.find_first_not_of( __delimiterPlusSpaces, 0 );

    Size pos     = str.find_first_of( __delimiter, last_pos );

    Size pos_w = str.find_last_not_of( __spaces,last_pos+1);

    while ( std::string::npos != pos || std::string::npos != last_pos ) {

      const Size fieldlength = pos == std::string::npos ?   pos_w + 1 - last_pos: pos_w -last_pos;

      if ( __data.size() < ++counter ) __data.resize( counter );

      __data[counter-1].resize( fieldlength );

      __data[counter-1].assign( str,last_pos, fieldlength );
      GUM_TRACE_VAR(pos);
      GUM_TRACE_VAR(last_pos);
      GUM_TRACE_VAR(pos_w);
      GUM_TRACE_VAR("<"+__data[counter-1]+">");
      GUM_TRACE_VAR(pos);

      last_pos = str.find_first_not_of( __delimiterPlusSpaces, pos );

      pos = str.find_first_of( __delimiter, last_pos );

      pos_w = str.find_last_not_of( __spaces,last_pos+1 );
    }

    __data.resize( counter );

    __emptyData=false;
  }


}
