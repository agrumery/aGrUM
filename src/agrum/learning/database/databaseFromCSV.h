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
 * Class DatabaseFromCSV first reads a database from a CSV file and stores
 * its content into a DatabaseVectInRAM. Then, the database is accessed as
 * a standard DatabaseVectInRAM. The CSV file may contain missing data, in
 * which case, we just indicate in the arguments passed to the constructor
 * which strings represent these missing data. If the CSV file does not
 * contain names for its columns, the DatabaseFromCSV creates some (those
 * are "node X", where X is the number of the column, the first column having
 * number 0).
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_DATABASE_FROM_CSV_H
#define GUM_DATABASE_FROM_CSV_H

#include <string>
#include <vector>

#include <agrum/learning/database/databaseVectInRAM.h>
#include <agrum/learning/database/DBTransform.h>
#include <agrum/learning/database/DBTransformIdentity.h>

namespace gum {

  namespace learning {

    /** @class DatabaseFromCSV
     * @brief The class representing a tabular database created from a CSV
     * @ingroup learning_group
     * Class DatabaseFromCSV first reads a database from a CSV file and stores
     * its content into a DatabaseVectInRAM. Then, the database is accessed as
     * a standard DatabaseVectInRAM. The CSV file may contain missing data, in
     * which case, we just indicate in the arguments passed to the constructor
     * which strings represent these missing data. If the CSV file does not
     * contain names for its columns, the DatabaseFromCSV creates some (those
     * are "node X", where X is the number of the column, the first column having
     * number 0).
     */
    class DatabaseFromCSV : public DatabaseVectInRAM {
      public:
      /// the handler used for parsing the database
      using Handler = typename DatabaseVectInRAM::Handler;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      /** Class DatabaseFromCSV first reads a database from a CSV file and stores
       * its content into a DatabaseVectInRAM. Then, the database is accessed as
       * a standard DatabaseVectInRAM. The CSV file may contain missing data, in
       * which case, we just indicate in the arguments passed to the constructor
       * which strings represent these missing data. If the CSV file does not
       * contain names for its columns, the DatabaseFromCSV creates some (those
       * are "node X", where X is the number of the column, the first column having
       * number 0). */
      DatabaseFromCSV(const std::string filename, bool fileContainsNames = true,
                      const DBTransform &transform = DBTransformIdentity(),
                      const std::string delimiter = ",",
                      const char commentmarker = '#', const char quoteMarker = '"',
                      const std::vector<std::string> missingVal = {"?", "N/A",
                                                                   "n/a"});

      /// copy constructor
      DatabaseFromCSV(const DatabaseFromCSV &from);

      /// move constructor
      DatabaseFromCSV(DatabaseFromCSV &&from);

      /// destructor
      virtual ~DatabaseFromCSV();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      DatabaseFromCSV &operator=(const DatabaseFromCSV &from);

      /// move constructor
      DatabaseFromCSV &operator=(DatabaseFromCSV &&from);

      /// @}
    };

  } /* namespace learning */

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/database/databaseFromCSV.inl>
#endif /* GUM_NO_INLINE */

#endif /* GUM_DATABASE_FROM_CSV_H */
