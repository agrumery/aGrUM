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
 * @brief The base class for initializing DatabaseTable and RawDatabaseTable
 * instances from CSV files or SQL databases
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_IDB_INITILIALIZER_H
#define GUM_LEARNING_IDB_INITILIALIZER_H

#include <vector>
#include <string>

#include <agrum/agrum.h>
#include <agrum/learning/database/DBCell.h>
#include <agrum/learning/database/DBRow.h>


namespace gum {

  namespace learning {

    /** @class IDBInitializer
     * @headerfile IDBInitializer.h <agrum/learning/database/IDBInitializer.h>
     * @ingroup learning_database
     * @brief The base class for initializing DatabaseTable and
     * RawDatabaseTable instances from CSV files or SQL databases
     *
     * @par Usage example:
     * @code
     * // 1/ use the initializer to parse all the columns/rows of a CSV file
     * // the DBInitializerFromCSV class inherits from IDBInitializer<>
     * gum::learning::DBInitializerFromCSV<> initializer ( "asia.csv" );
     * const auto& var_names = initializer.variableNames ();
     * const std::size_t nb_vars = var_names.size ();
     *
     * // we create as many translators as there are variables
     * gum::learning::DBTranslator4LabelizedVariable<> translator;
     * gum::learning::DBTranslatorSet<> translator_set;
     * for ( std::size_t i = 0; i < nb_vars; ++i )
     *   translator_set.insertTranslator ( translator, i );

     * // create a DatabaseTable with these translators. For the moment, the
     * // DatabaseTable will be empty, i.e., it will contain no row
     * gum::learning::DatabaseTable<> database ( translator_set );
     * database.setVariableNames( initializer.variableNames () );
     *
     * // use the DBInitializerFromCSV to fill the rows:
     * initializer.fillDatabase ( database );
     * // now, the database contains all the content of the CSV file
     *
     *
     * // 2/ use an IDBInitializer to initialize a DatabaseTable, but ignore
     * // some columns.
     * gum::learning::DBInitializerFromCSV<> initializer2 ( "asia.csv" );
     * gum::learning::DatabaseTable<> database2; // empty database
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
     * gum::learning::DBInitializerFromCSV<> initializer3 ( "asia.csv" );
     * gum::learning::DatabaseTable<> database3 ( translator_set );
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
    template < template < typename > class ALLOC >
    class IDBInitializer {
      public:
      /** @brief the enumeration indicating the type of the data the
       * IDBInitializer expects as input data */
      enum class InputType : char { STRING, DBCELL };

      /// type for the allocators passed in arguments of methods
      using allocator_type = ALLOC< std::string >;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param type indicates what type of data will be read by the
       * IDBInitializer when it will try to fill the database.
       * @param alloc The allocator that will be used by all methods */
      IDBInitializer(const InputType type, const allocator_type& alloc);

      /// copy constructor
      IDBInitializer(const IDBInitializer< ALLOC >& from);

      /// copy constructor with a given allocator
      IDBInitializer(const IDBInitializer< ALLOC >& from,
                     const allocator_type&          alloc);

      /// move constructor
      IDBInitializer(IDBInitializer< ALLOC >&& from);

      /// move constructor with a given allocator
      IDBInitializer(IDBInitializer< ALLOC >&& from, const allocator_type& alloc);

      /// virtual copy constructor
      virtual IDBInitializer< ALLOC >* clone() const = 0;

      /// virtual copy constructor with a given allocator
      virtual IDBInitializer< ALLOC >*
        clone(const allocator_type& alloc) const = 0;

      /// destructor
      virtual ~IDBInitializer();

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// returns the names of the variables in the input dataset
      const std::vector< std::string, ALLOC< std::string > >& variableNames();

      /// fills the rows of the database table
      /** This method may raise exceptions when trying to insert new rows
       * into the database table. See Method insertRow() of the database table. */
      template < template < template < typename > class > class DATABASE >
      void fillDatabase(DATABASE< ALLOC >& database,
                        const bool         retry_insertion = false);

      /** @brief This method indicates which column filling raised an exception,
       * if any, during the execution of fillDatabase */
      std::size_t throwingColumn() const;

      /// returns the allocator used
      allocator_type getAllocator() const;

      /// @}


      protected:
      /// copy operator
      IDBInitializer< ALLOC >& operator=(const IDBInitializer< ALLOC >& from);

      /// move operator
      IDBInitializer< ALLOC >& operator=(IDBInitializer< ALLOC >&& from);

      /// ask the child class for the names of the variables
      virtual std::vector< std::string, ALLOC< std::string > >
        _variableNames() = 0;

      /// asks the child class for the content of the current row using strings
      /** If the child class parses strings, this method should be overloaded */
      virtual const std::vector< std::string, ALLOC< std::string > >&
        _currentStringRow();

      /// asks the child class for the content of the current row using dbcells
      /** If the child class parses DBRows, this method should be overloaded */
      virtual const DBRow< DBCell, ALLOC >& _currentDBCellRow();

      /// indicates whether there is a next row to read (and point on it)
      virtual bool _nextRow() = 0;


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      // the names of the variables
      std::vector< std::string, ALLOC< std::string > > __var_names;

      // the types of the input data read to fill the database
      InputType __input_type;

      // indicates whether an exception was raised when adding the last row
      // into the database. If so, when filling again the database, we may
      // try to insert again the same row
      bool __last_insertion_failed{false};


      /// fills the rows of the database using string inputs
      template < template < template < typename > class > class DATABASE >
      void __fillDatabaseFromStrings(DATABASE< ALLOC >& database,
                                     const bool         retry_insertion);

      /// fills the rows of the database using DBCell inputs
      template < template < template < typename > class > class DATABASE >
      void __fillDatabaseFromDBCells(DATABASE< ALLOC >& database,
                                     const bool         retry_insertion);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */

// always include the template implementation
#include <agrum/learning/database/IDBInitializer_tpl.h>

#endif /* GUM_LEARNING_IDB_INITILIALIZER_H */