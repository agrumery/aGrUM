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
/** @file
 * @brief The class representing a tabular database created from a CSV
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <iostream> 
#include <fstream> 

#include <agrum/learning/CSVParser.h>
#include <agrum/learning/databaseFromCSV.h>

namespace gum {

  
  namespace learning {


    /// default constructor
    DatabaseFromCSV::DatabaseFromCSV
    ( const std::string filename,
      const DBTransform& transform,
      const std::string delimiter,
      const char commentmarker,
      const char quoteMarker,
      const std::vector<std::string> missingVal ) {
      // open the file and use the CSVParser to fill the vector of DBRows
      std::ifstream in ( filename, std::ifstream::in);
      CSVParser parser ( in, delimiter, commentmarker, quoteMarker );
      std::vector<DBRow>& vect = _content ();
      
      do {
        // read a new line in the input file and convert it into a DBRow
        const std::vector<std::string>& row = parser.current ();
        
        DBRow new_row ( row.size () );
        for ( unsigned int i = 0; i < row.size (); ++i ) {
          new_row[i].setBestTypeSafe ( row[i] );
        }
        
        // add the result into 
        vect.push_back ( new_row );
      } while ( parser.next () );
      
      // if we wish to apply a DBTransform to preprocess the database
      transform.transform ( vect, missingVal );

      // for debugging purposes
      GUM_CONSTRUCTOR ( DatabaseFromCSV );
    }

        
    /// copy constructor
    DatabaseFromCSV::DatabaseFromCSV ( const DatabaseFromCSV& from ) :
      DatabaseVectInRAM ( from ) {
      // for debugging purposes
      GUM_CONS_CPY ( DatabaseFromCSV );
    }
      

    /// move constructor
    DatabaseFromCSV::DatabaseFromCSV ( DatabaseFromCSV&& from ) :
      DatabaseVectInRAM ( std::move ( from ) ) {
      // for debugging purposes
      GUM_CONS_MOV ( DatabaseFromCSV );
    }
      

    /// destructor
    DatabaseFromCSV::~DatabaseFromCSV () {
      GUM_DESTRUCTOR ( DatabaseFromCSV );
    }
          

  } /* namespace learning */


} /* namespace gum */
