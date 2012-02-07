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

  CSVParser::CSVParser ( std::istream & in, const std::string& delimiter, const char  commentmarker ) :
    __line(),
    __delimiter ( delimiter ),
    __spaces ( " \t" ),
    __delimiterPlusSpaces ( __delimiter+__spaces ),
    __commentMarker ( commentmarker ),
    __in ( in ),
    __data(),
    __emptyData ( true ) {
    GUM_CONSTRUCTOR ( CSVParser );
  }

  CSVParser::~CSVParser() {
    GUM_DESTRUCTOR ( CSVParser );
  }


  char getTheChar(const std::string& str,Size pos) {
    return (pos<std::string::npos)?str.at(pos):'$';
  }
  void CSVParser::__getNextTriplet (const std::string& str,Size& first_letter_token, Size& next_token, Size& last_letter_token, Size from ) {
    first_letter_token= str.find_first_not_of ( __spaces, from );

    if (first_letter_token==std::string::npos) {
      next_token=last_letter_token=first_letter_token;
      return;
    }

    if (str.at(first_letter_token)=='"') {
      last_letter_token=str.find_first_of('"',first_letter_token+1);
      if (last_letter_token==std::string::npos) GUM_ERROR(FatalError,"String does not end");
      next_token=str.find_first_of(__delimiter,last_letter_token);
    } else {
      next_token= str.find_first_of ( __delimiter, first_letter_token );

      if (next_token==std::string::npos) {
        last_letter_token= str.find_last_not_of ( __spaces,next_token );
      } else {
        last_letter_token= str.find_last_not_of ( __delimiterPlusSpaces,next_token-1 );
      }
    }

    //GUM_TRACE ( str<<" : "<<first_letter_token<<"["<<getTheChar(str,first_letter_token)<<"]-"<<last_letter_token<<"["<<getTheChar(str,last_letter_token)<<"]-"<<next_token );
  }

  void CSVParser::__tokenize ( const std::string& s ) {
    // removing comment part
    std::string str =s.substr ( 0,s.find_first_of ( __commentMarker,0 ) );
    Size counter=0,first_letter_token,next_token,last_letter_token;


    __getNextTriplet ( str,first_letter_token,next_token,last_letter_token,0 );

    while ( std::string::npos != first_letter_token && std::string::npos != last_letter_token ) {
      if ( __data.size() < ++counter )
        __data.resize ( counter );


      if ( last_letter_token>=first_letter_token ) {
        const Size fieldlength = last_letter_token+ 1 - first_letter_token;
        __data[counter-1].resize ( fieldlength );
        __data[counter-1].assign ( str,first_letter_token, fieldlength );
      } else {
        __data[counter-1]="";
      }

      //GUM_TRACE ( "-----["<<__data[counter-1]<<"]" );

      if ( next_token==std::string::npos ) break;

      __getNextTriplet ( str,first_letter_token,next_token,last_letter_token,next_token+1 );
    }

    __data.resize ( counter );

    __emptyData=false;
  }


}
