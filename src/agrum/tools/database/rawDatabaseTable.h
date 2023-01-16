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
 * @brief The table containing the raw/original data of a database
 *
 * Class RawDatabaseTable is intended to store in RAM the raw/original data
 * of a database. Such raw data are not well suited for learning tasks
 * because they need to be interpreted by the learning algorithm, which would
 * incur a strong overhead. However, reading a CSV file and interpreting its
 * data in order to reshape them in a way that will allow fast parsing by
 * learning algorithms is also very time consuming. So, if you are unsure
 * about the correct interpretation and need to change it several times either
 * before processing the learning or during several learning phases, it is
 * efficient to first read the CSV file and store its useful data (removing
 * comments, for instance) into a first database table and, then, use this
 * preprocessed table to produce quickly the interpreted database table that
 * will subsequently be used by the learning. The purpose of the
 * RawDatabaseTable class is precisely to implement this preprocessed table.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_RAW_DATABASE_TABLE_H
#define GUM_RAW_DATABASE_TABLE_H


#include <agrum/agrum.h>
#include <agrum/tools/database/IDatabaseTable.h>

namespace gum {

  namespace learning {

    /** @class RawDatabaseTable
     * @brief The table containing the raw/original data of a database
     * @headerfile rawDatabaseTable.h <agrum/tools/databaseTable.h>
     * @ingroup learning_database
     *
     * Class RawDatabaseTable is intended to store in RAM the raw/original data
     * of a database. Such raw data are not well suited for learning tasks
     * because they need to be interpreted by the learning algorithm, which would
     * incur a strong overhead. However, reading a CSV file and interpreting its
     * data in order to reshape them in a way that will allow fast parsing by
     * learning algorithms is also very time consuming. So, if you are unsure
     * about the correct interpretation and need to change it several times either
     * before processing the learning or during several learning phases, it is
     * efficient to first read the CSV file and store its useful data (removing
     * comment, for instance) into a first database table and, then, use this
     * preprocessed table to produce quickly the interpreted database table that
     * will subsequently be used by the learning. The purpose of the
     * RawDatabaseTable class is precisely to implement this preprocessed table.
     *
     * @par Usage example:
     * @code
     * // create an empty database
     * gum::learning::RawDatabaseTable<> database;
     *
     * // create a new row with 3 DBCells containing integer 2
     * typename gum::learning::RawDatabaseTable<>::template
     * Row<gum::learning::DBCell> new_row ( 3, gum::learning::DBCell ( 2 ) );
     *
     * // add it into the database
     * database.insertRow ( new_row );
     * database.insertRow ( std::move ( new_row ) );
     *
     * // erase the first Row
     * database.eraseFirstRow ();
     *
     * // returns the content of the database
     * const auto& content = database.content ();
     *
     * // sets the names of the variables (the columns) of the database
     * std::vector<std::string> new_names { "col1", "col2", "col3" };
     * database.setVariableNames ( new_names );
     *
     * // print the names of the columns
     * std::cout << database.variableNames () << std::endl;
     *
     * // print all the records of the database
     * for ( auto row : database )
     *   std::cout << row << std::endl;
     *
     * // make the handler parse the 3rd record to the 5th record (included)
     * auto handler = database.handler ();
     * handler.setRange ( 2, 5 ); // 2 = 3rd record; 5 = 6th record (excluded)
     * while ( const auto& row : handler ) {
     *   std::cout << row << std::endl;
     * }
     *
     * // clears the content of the database and update the safe database's
     * // handlers
     * database.clear ();
     * @endcode
     */
    class RawDatabaseTable: public IDatabaseTable< DBCell > {
      public:
      /// the type for the vectors used in the RawDatabaseTable
      template < typename TX_DATA >
      using DBVector = std::vector< TX_DATA >;

      /// a row of the database
      template < typename TX_DATA >
      using Row = DBRow< TX_DATA >;

      /// the type for the matrices stored into the database
      template < typename TX_DATA >
      using Matrix = DBVector< Row< TX_DATA > >;

      using MissingValType = std::vector< std::string >;

      /// the unsafe handler type
      using Handler = typename IDatabaseTable< DBCell >::Handler;

      /// the safe handler type
      using HandlerSafe = typename IDatabaseTable< DBCell >::HandlerSafe;

      using IsMissing = typename IDatabaseTable< DBCell >::IsMissing;

      /// Types for STL compliance.
      /// @{
      using value_type      = Row< DBCell >;
      using reference       = value_type&;
      using const_reference = const value_type&;
      using pointer         = value_type*;
      using const_pointer   = const value_type*;
      using size_type       = std::size_t;
      using difference_type = std::ptrdiff_t;
      using iterator        = Handler;
      using iterator_safe   = HandlerSafe;
      /// @}


      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      RawDatabaseTable(const MissingValType&             missing_symbols,
                       const std::vector< std::string >& var_names);

      /// default constructor
      RawDatabaseTable(const MissingValType& missing_symbols);

      /// default constructor
      RawDatabaseTable();

      /// copy constructor
      RawDatabaseTable(const RawDatabaseTable& from);

      /// move constructor
      RawDatabaseTable(RawDatabaseTable&& from);

      /// virtual copy constructor
      virtual RawDatabaseTable* clone() const final;

      /// destructor
      virtual ~RawDatabaseTable();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      RawDatabaseTable& operator=(const RawDatabaseTable& from);

      /// move constructor
      RawDatabaseTable& operator=(RawDatabaseTable&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      using IDatabaseTable< DBCell >::setVariableNames;

      /// sets the names of the variables
      /** This method can be called in two different ways: either the names
       * correspond precisely to the columns stored into the database table
       * (in this case, parameter from_external_object is equal to false),
       * or they corresponds to the columns of an external database (e.g., a
       * CSV file) from which we potentially excluded some columns and,
       * consequently, these columns should not be taken into account (in this
       * case, parameter from_external_object is equal to true). As an
       * example, imagine that the database table is created from a CSV file
       * with 5 columns named X0, X1, X2, X3 and X4 respectivly. Suppose that
       * we asked the database table to ignore columns X1 and X3. Then
       * setVariableNames( { "X0", "X1", "X2", "X3", "X4" }, true ) will
       * set the columns of the database table as { "X0", "X2", "X4" }. The
       * same result could be obtained by executing
       * setVariableNames( { "X0", "X2", "X4" }, false ), which specifies
       * directly the set of names to retain in the database table.
       * @param names the names of all the columns, including the ignored
       * columns if from_external_object is set to true, else excluding
       * them (i.e., this should precisely correspond to the columns stored
       * into the database table).
       * @param from_external_object a Boolean indicating whether parameter
       * names includes the columns ignored by the database table (true) or
       * not (false).
       * @throw SizeError is raised if the names passed in arguments cannot be
       * assigned to the columns of the RawDatabaseTable because the size of their
       * vector is inadequate. */
      void setVariableNames(const std::vector< std::string >& names,
                            const bool                        from_external_object = true) final;

      /// makes the database table ignore from now on the kth column
      /** This method can be called in two different ways: either k refers to
       * the current kth column of the database table (in this case parameter
       * from_external_object is set to false), or k corresponds to the kth
       * column of an original database used to fill the database table
       * (in this case from_external_object is set to true). Depending on
       * from_external_object's value, the ignored columns may differ. As an
       * example, imagine that the database table is created from a CSV file
       * with 5 columns named X0, X1, X2, X3 and X4 respectivly. Then a call to
       * ignoreColumn ( 1, true ) will exclude column X1 from the database table.
       * As a result, the database table columns are X0, X2, X3 and X4.
       * Therefore, subsequently calling ignoreColumn ( 1, false ) will result
       * in excluding X2 since X2 is the 2nd column (columns are indexed
       * starting from 0). So, now the database table's columns are
       * X0, X3 and X4. If, now, we call ignoreColumn ( 3, true ), this will
       * remove column X3 because, in the original database, X3 was the 4th
       * column.
       *
       * @warning If the database table was not empty, then the kth column is
       * removed from all the rows currently stored.
       * @warning If the kth column does not exist (i.e., the original dataset
       * does not contain the kth column when from_external_object is set to
       * true, or the RawDatabaseTable has no kth column when from_external_object
       * is set to false), column k is marked as to be ignored and nothing is
       * done on the content of the RawDatabaseTable. No exception is raised.
       * @param k the column to remove. See Method setVariableNames for a
       * detailed description on how k is computed.
       * @param from_external_object indicates whether k refers to the kth
       * column of an original external database (true) or to the current kth
       * column of the RawDatabaseTable. */
      void ignoreColumn(const std::size_t k, const bool from_external_object = true) final;

      /// returns  the set of columns of the original dataset that are ignored
      const DBVector< std::size_t > ignoredColumns() const final;

      /** @brief returns the set of columns of the original dataset that are
       * present in the RawDatabaseTable */
      const DBVector< std::size_t > inputColumns() const final;

      using IDatabaseTable< DBCell >::insertRow;

      /// insert a new row at the end of the database
      /** The new_row passed in argument is supposed to come from an external
       * database. So it must contain data for the ignored columns.
       * @throw SizeError is raised if the vector of string cannot be inserted
       * in the RawDatabaseTable because its size does not allow a matching with
       * the columns of the RawDatabaseTable (taking into account the ignored
       * columns) */
      void insertRow(const std::vector< std::string >& new_row) final;

      /// erase the content of the database, including the names of the variables
      virtual void clear() final;

      /// @}


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      // the set of ignored columns, sorted by increasing order
      DBVector< std::size_t > _ignored_cols_;

      /// translates a string into a DBCell and returns it
      DBCell _convert_(const std::string& elt) const;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */

// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/tools/database/rawDatabaseTable_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_RAW_DATABASE_TABLE_H */
