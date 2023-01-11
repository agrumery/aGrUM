/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


/** @file
 * @brief The class for initializing DatabaseTable and RawDatabaseTable
 * instances from CSV files
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_DB_INITILIALIZER_FROM_CSV_H
#define GUM_LEARNING_DB_INITILIALIZER_FROM_CSV_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include <agrum/agrum.h>
#include <agrum/tools/database/IDBInitializer.h>
#include <agrum/tools/database/CSVParser.h>

namespace gum {

  namespace learning {

    /** @class DBInitializerFromCSV
     * @headerfile DBInitializerFromCSV.h <agrum/tools/database/DBInitializerFromCSV.h>
     * @ingroup learning_database
     * @brief The class for initializing DatabaseTable and RawDatabaseTable
     * instances from CSV files
     *
     * In aGrUM, the usual way to create DatabaseTable instances used by learning
     * algorithms is to use the 4-step process below:
     * -# Create an IDBInitializer instance (either a DBInitializerFromCSV or a
     *    DBInitializerFromSQL). This will enable to get the variables
     *    corresponding to the columns of the DatabaseTable.
     * -# Knowing these variables, create a DBTranslatorSet for encoding the
     *    lines of the CSV file or those of the SQL result into the appropriate
     *    values for the learning algorithms.
     * -# Create the DatabaseTable, passing it the DBTranslatorSet created
     *    in the preceding step. Use the IDBInitializer to provide the variables'
     *    names to the DatabaseTable.
     * -# Use the IDBInitializer to add the lines of the CSV file or those of the
     *    SQL result into the DatabaseTable.
     *
     * @par The following codes show the details of this process:
     * @code
     * // 1/ use the initializer to parse all the columns/rows of a CSV file
     * gum::learning::DBInitializerFromCSV initializer ( "asia.csv" );
     * const auto& var_names = initializer.variableNames ();
     * const std::size_t nb_vars = var_names.size ();
     *
     * // we create as many translators as there are variables
     * gum::learning::DBTranslator4LabelizedVariable translator;
     * gum::learning::DBTranslatorSet translator_set;
     * for ( std::size_t i = 0; i < nb_vars; ++i )
     *   translator_set.insertTranslator ( translator, i );

     * // create a DatabaseTable with these translators. For the moment, the
     * // DatabaseTable will be empty, i.e., it will contain no row
     * gum::learning::DatabaseTable database ( translator_set );
     * database.setVariableNames( initializer.variableNames () );
     *
     * // use the DBInitializerFromCSV to fill the rows:
     * initializer.fillDatabase ( database );
     * // now, the database contains all the content of the CSV file
     *
     *
     * // 2/ use an IDBInitializer to initialize a DatabaseTable, but ignore
     * // some columns.
     * gum::learning::DBInitializerFromCSV initializer2 ( "asia.csv" );
     * gum::learning::DatabaseTable database2; // empty database
     *
     * // indicate which columns of the CSV file should be read
     * database2.insertTranslator ( translator, 1 );
     * database2.insertTranslator ( translator, 3 );
     * database2.insertTranslator ( translator, 4 );
     *
     * // sets the names of the columns correctly
     * database2.setVariableNames( initializer2.variableNames () );
     *
     * // fill the rows:
     * initializer2.fillDatabase ( database2 );
     * // now all the rows of the CSV file have been transferred into database2,
     * // but only columns 1, 3 and 4 of the CSV file have been kept.
     *
     *
     * // 3/ another possibility to initialize a DatabaseTable, ignoring
     * // some columns:
     * gum::learning::DBInitializerFromCSV initializer3 ( "asia.csv" );
     * gum::learning::DatabaseTable database3 ( translator_set );
     * // here, database3 is an empty database but it contains already
     * // translators for all the columns of the CSV file. We shall now remove
     * // the columns/translators that are not wanted anymore
     * database3.ignoreColumn ( 0 );
     * database3.ignoreColumn ( 2 );
     * database3.ignoreColumn ( 5 );
     * database3.ignoreColumn ( 6 );
     * database3.ignoreColumn ( 7 );
     * // asia contains 8 columns. The above ignoreColumns keep only columns
     * // 1, 3 and 4.
     *
     * // sets the names of the columns correctly
     * database3.setVariableNames( initializer3.variableNames () );
     * // fill the rows:
     * initializer3.fillDatabase ( database3 );
     * // now all the rows of the CSV file have been transferred into database3,
     * // but only columns 1, 3 and 4 of the CSV file have been kept.
     * @endcode
     */
    class DBInitializerFromCSV: public IDBInitializer {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param filename the name of the CSV file
       * @param fileContainsNames a Boolean indicating whether the first line of
       * the CSV file contains the names of the columns
       * @param delimiter the character that acts as the column separator in
       * the CSV file
       * @param commentmarker the character that marks the beginning of a comment
       * @param quoteMarker the character that is used to quote the sentences
       * in the CSV file
       */
      DBInitializerFromCSV(const std::string filename,
                           bool              fileContainsNames = true,
                           const std::string delimiter         = ",",
                           const char        commentmarker     = '#',
                           const char        quoteMarker       = '"');

      /// copy constructor
      /** the new initializer points to the same file as from, but it reparses
       * it from scratch. */
      DBInitializerFromCSV(const DBInitializerFromCSV& from);

      /// move constructor
      DBInitializerFromCSV(DBInitializerFromCSV&& from);

      /// virtual copy constructor
      virtual DBInitializerFromCSV* clone() const;

      /// destructor
      virtual ~DBInitializerFromCSV();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      /** the initializer points to the same file as from, but it reparses
       * it from scratch. */
      DBInitializerFromCSV& operator=(const DBInitializerFromCSV& from);

      /// move operator
      /** the initializer points to the same file as from, but it reparses
       * it from scratch. */
      DBInitializerFromCSV& operator=(DBInitializerFromCSV&& from);

      /// @}


      protected:
      /// returns the names of the variables
      virtual std::vector< std::string > variableNames_() final;

      /// returns the content of the current row using strings
      virtual const std::vector< std::string >& currentStringRow_() final;

      /// indicates whether there is a next row to read (and point on it)
      virtual bool nextRow_() final;


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      // the filename used for parsing
      std::string _filename_;

      // indicates the delimiter used within the CSV
      std::string _delimiter_;

      // indicates which character is a comment symbol in the CSV
      char _comment_marker_;

      // indicates which character is a quote symbol in the CSV
      char _quote_marker_;

      // indicates whether the first row of the file contains the names
      bool _first_row_has_names_;

      // the input stream read by the parser
      std::ifstream _input_stream_;

      // the CSV parser used for the reading the CSV file
      CSVParser _parser_;

      // the variables names, if the first row has names
      std::vector< std::string > _var_names_;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/tools/database/DBInitializerFromCSV_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_DB_INITILIALIZER_FROM_CSV_H */
