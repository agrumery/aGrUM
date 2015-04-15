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
#include <sstream>

#include <agrum/learning/database/CSVParser.h>
#include <agrum/learning/database/databaseFromCSV.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#include <agrum/learning/database/databaseFromCSV.inl>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    DatabaseFromCSV::DatabaseFromCSV(const std::string filename,
                                     bool fileContainsNames,
                                     const DBTransform &transform,
                                     const std::string delimiter,
                                     const char commentmarker,
                                     const char quoteMarker,
                                     const std::vector<std::string> missingVal) {
      // open the file and create the CSVParser that will parse it
      std::ifstream in(filename, std::ifstream::in);

      if ((in.rdstate() & std::ifstream::failbit) != 0) {
        GUM_ERROR(IOError, "File " << filename << " not found");
      }

      CSVParser parser(in, delimiter, commentmarker, quoteMarker);

      // if the first line contains names, store them
      if (fileContainsNames) {
        parser.next();
        _variableNames() = parser.current();
      }

      // use the CSVParser to fill the vector of DBRows
      std::vector<DBRow> &vect = _content();

      while (parser.next()) {
        // read a new line in the input file and convert it into a DBRow
        const std::vector<std::string> &row = parser.current();

        DBRow new_row(row.size());

        for (unsigned int i = 0; i < row.size(); ++i) {
          new_row[i].setBestTypeSafe(row[i]);
        }

        // add the result into
        vect.push_back(new_row);
      }

      // if we wish to apply a DBTransform to preprocess the database
      transform.transform(vect, missingVal);

      // if the names of the variables have not been set yet, do it
      if (!fileContainsNames && vect.size() && vect[0].size()) {
        std::vector<std::string> &names = _variableNames();
        names.resize(vect[0].size());

        for (unsigned int i = 0; i < names.size(); ++i) {
          std::stringstream s;
          s << "node " << i;
          names[i] = s.str();
        }
      }

      // for debugging purposes
      GUM_CONSTRUCTOR(DatabaseFromCSV);
    }

  } /* namespace learning */

} /* namespace gum */
