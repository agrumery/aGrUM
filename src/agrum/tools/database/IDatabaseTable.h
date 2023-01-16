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
 * @brief The common class for the tabular database tables
 *
 * IDatabases are not intended to be created as is but should be created through
 * the RawDatabaseTable and DatabaseTable classes.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_IDATABASE_TABLE_H
#define GUM_IDATABASE_TABLE_H

#include <cstddef>
#include <utility>
#include <string>
#include <cstring>
#include <memory>
#include <vector>
#include <mutex>

#include <agrum/agrum.h>
#include <agrum/tools/core/threads.h>
#include <agrum/tools/core/threadExecutor.h>
#include <agrum/tools/database/DBCell.h>
#include <agrum/tools/database/DBHandler.h>
#include <agrum/tools/database/DBTranslator.h>


namespace gum {

  namespace learning {

    template < bool ENABLE_INSERT >
    struct IDatabaseTableInsert4DBCell;

    template <>
    struct IDatabaseTableInsert4DBCell< true > {
      template < typename TX_DATA >
      using DBVector = std::vector< TX_DATA >;

      template < typename TX_DATA >
      using Row = DBRow< TX_DATA >;

      template < typename TX_DATA >
      using Matrix = std::vector< DBRow< TX_DATA > >;


      /// insert a new DBRow at the end of the database
      /** The new row passed in argument is supposed to come from an external
       * database. So it must contain data for the ignored columns. */
      virtual void insertRow(Row< DBCell >&& new_row) = 0;

      /// insert a new row at the end of the database
      /** The new row passed in argument is supposed to come from an external
       * database. So it must contain data for the ignored columns. */
      virtual void insertRow(const Row< DBCell >& new_row) = 0;

      /// insert a set of new DBRows at the end of the database
      /** The new rows passed in argument are supposed to come from an external
       * database. So they must contain data for the ignored columns. */
      virtual void insertRows(Matrix< DBCell >&& new_rows) = 0;

      /// insert a set of new DBRows at the end of the database
      /** The new rows passed in argument are supposed to come from an external
       * database. So they must contain data for the ignored columns. */
      virtual void insertRows(const Matrix< DBCell >& new_rows) = 0;

      /// insert a new row at the end of the database
      /** The new row passed in argument is supposed to come from an external
       * database. So it must contain data for the ignored columns. */
      virtual void insertRow(const std::vector< std::string >& new_row) = 0;

      /// insert new rows at the end of the database
      /** The new rows passed in argument are supposed to come from an external
       * database. So they must contain data for the ignored columns. */
      virtual void insertRows(const DBVector< DBVector< std::string > >& new_rows);
    };


    template <>
    struct IDatabaseTableInsert4DBCell< false > {
      template < typename TX_DATA >
      using DBVector = std::vector< TX_DATA >;

      template < typename TX_DATA >
      using Row = DBRow< TX_DATA >;

      template < typename TX_DATA >
      using Matrix = std::vector< DBRow< TX_DATA > >;

      /// insert a new row at the end of the database
      /** The new row passed in argument is supposed to come from an external
       * database. So it must contain data for the ignored columns. */
      virtual void insertRow(const std::vector< std::string >& new_row) = 0;

      /// insert new rows at the end of the database
      /** The new rows passed in argument are supposed to come from an external
       * database. So they must contain data for the ignored columns. */
      virtual void insertRows(const DBVector< DBVector< std::string > >& new_rows);
    };


    /** @class IDatabaseTable
     * @headerfile IDatabaseTable.h <agrum/BN/learning/IDatabaseTable.h>
     * @brief The common class for the tabular database tables
     *
     * IDatabases are not intended to be created as is but should be created
     * through the RawDatabaseTable and DatabaseTable classes. They represent
     * the structures shared by these latter classes.
     *
     * Here is an example of how to use the class, illustrated with the
     * DatabaseTable class (in this case, the T_DATA type is just equal to
     * DBTranslatedValue):
     * @code
     * // create the database from a CSV. This is not compulsory for
     * // IDatabaseTable instances, but this is how we usually create
     * // DatabaseTable instances
     * gum::learning::DBInitializerFromCSV<> initializer ( "asia.csv" );
     * const auto& var_names = initializer.variableNames ();
     * gum::learning::DBTranslatorSet<> translator_set;
     * gum::learning::DBTranslator4LabelizedVariable<> translator;
     * for ( std::size_t i = 0; i < var_names.size(); ++i )
     *   translator_set.insertTranslator ( translator, i );
     * gum::learning::DatabaseTable<> database ( translator_set );
     * database.setVariableNames( initializer.variableNames () );
     *
     * // here, database contains the content of the asia.csv file.
     * // determine how many columns and rows the database contains
     * std::size_t nb_rows = database.nbRows();
     * std::size_t nb_cols = database.nbVariables ();
     *
     * // manually add a new row into the database
     * std::vector<std::string> row( 8, "toto" ); // asia has 8 columns
     * database.insertRow ( row );
     * gum::learning::DBRow<gum::learning::DBTranslatedValue>
     *   dbrow ( 8, gum::learning::DBTranslatedValue { std::size_t(0) } );
     * database.insertRow ( dbrow );
     * // insert 4 rows in a single call
     * database.insertRows(
     *   std::vector<gum::learning::DBRow<gum::learning::DBTranslatedValue>>
     *   ( 4, dbrow ) );
     *
     * // erase some rows
     * database.eraseRow ( 12 );      // erase the 13th row of the database
     * database.eraseFirstRow ();     // erase the first row of the database
     * database.eraseLastRow ();      // erase the last row of the database
     * database.eraseFirstRows ( 2 ); // erase the first two rows
     * database.eraseLastRows ( 3 );  // erase the last three rows
     * database.eraseRows ( 2,4 );    // erase rows indexed from 2 to 4 (excluded)
     *
     * // parse the content of the database, the usual way
     * for ( const auto& dbrow : database )
     *   std::cout << dbrow.row() << "  weight: " << dbrow.weight() << std::endl;
     *
     * // ignore some columns of the database, i.e., remove them
     * database.ignoreColumn ( 3 ); // remove the column X3 of the CSV file
     * // now, the database contains columns 0, 1, 2, 4, 5, 6, 7 of the
     * // CSV file. If we wish to remove Column X5 of the CSV file:
     * database.ignoreColumn ( 5 ); // remove the column X5 of the CSV file
     * // now, the database contains columns 0, 1, 2, 4, 6, 7 of the CSV file.
     * // if we wish to remove the 5th column of the IDatabaseTable, i.e.,
     * // column #4 of the CSV, either we determine that this actually correspond
     * // to column X6 of the CSV and we use database.ignoreColumn ( 6 ) or
     * // we call:
     * database.ignoreColumn ( 4, false ); // false => 4 = the 5th column of
     * // the IDatabaseTable, not the 5th column/variable of the CSV file
     * // (remember that all column numbers start from 0).
     *
     * // display the columns of the CSV that were ignored and those that
     * // were kept:
     * std::vector<std::size_t> ignored_cols = database.ignoredColumns ();
     * std::vector<std::size_t> kept_cols = database.inputColumns ();
     *
     * // parse the content of the database using handlers
     * typename gum::learning::DatabaseTable<>::HandlerSafe handler( database );
     * typename gum::learning::DatabaseTable<>::Handler uhandler( database );
     * // by default, the handlers range over the whole database
     *
     * // change the range of rows handled by the DBHandler
     * std::cout << handler.setRange ( 1, 40 ); // now parses rows [1,40)
     * std::cout << handler.size ();   // displays 39: rows 1,...,39
     * std::cout << handler.DBSize (); // shows the number of rows in the database
     * std::cout << handler.numRow (); // displays 0: the handler currently
     * // points on the first row of its managed area [1,40)
     *
     * // move the handler to the next row
     * handler.nextRow();
     * std::cout << handler.numRow (); // displays 1: the handler points now
     * // on the second row of its managed area. This corresponds to the third
     * // DBRow of the database since the range of handler is [1,40)
     * ++handler; // move again to the next row
     * std::cout << handler.numRow (); // displays 2
     * handler += 4; // advances the pointer by 4 rows
     * std::cout << handler.numRow (); // displays 6
     *
     * // get the DBRow pointed to by the handler: this is the 7th DBRow
     * // of the database
     * const auto& xrow7 = handler.row ();     // get the DBRow, unsafe version
     * const auto& yrow7 = handler.rowSafe (); // get the DBRow, safe version
     * const std::vector<gum::learning::DBCell>& xrow = xrow7.row ();
     * const double xweight = xrow27.weight ();
     *
     * // another way to access the row
     * const auto& zrow7 = *handler; // get the DBRow, unsafe version
     *
     * // check whether there exist other rows managed by the handler after
     * // the current row
     * bool has_rows = handler.hasRows (); // true: there remains 33 rows
     *
     * // makes the handler point again on the 2nd row of the database
     * handler.reset (); // the handler points to the beginning of its area
     * std::cout << handler.numRow (); // displays 0: the handler currently
     * // points on the first row of its managed area [1,40)
     *
     * // see the variables' names, i.e., the names of the database's columns
     * const auto& vars = handler.variableNames();
     *
     * // parse all the rows managed
     * handler.reset ();
     * for ( auto end = handler.end (); handler != end; ++handler )
     *   std::cout << handler.row ().weight () << std::endl;
     *
     * // another possibility:
     * for ( const auto& row : handler )
     *   std::cout << row.weight () << std::endl;
     * @endcode
     * @ingroup learning_database
     */
    template < typename T_DATA >
    class IDatabaseTable:
        public IDatabaseTableInsert4DBCell< !std::is_same< T_DATA, DBCell >::value > {
      public:
      /// the type for the vectors used in the IDatabaseTable
      template < typename TX_DATA >
      using DBVector = std::vector< TX_DATA >;

      /// a row of the database
      template < typename TX_DATA >
      using Row = DBRow< TX_DATA >;

      /// the type for the matrices stored into the database
      template < typename TX_DATA >
      using Matrix = std::vector< DBRow< TX_DATA > >;

      using MissingValType = std::vector< std::string >;

      enum IsMissing : char {
        False,
        True
      };


      /** @class Handler
       * @headerfile IDatabaseTable.h <agrum/BN/learning/IDatabaseTable.h>
       * @brief the (unsafe) handler for the tabular databases
       *
       * The IDatabaseTable class is provided with two types of handlers: unsafe
       * handlers and safe ones. Compared to the former, the safe handlers
       * incur a small overhead during their creation. But safe handlers
       * are informed by their associated database when the structure of
       * this one changes, i.e., when the number of rows/columns changes or
       * when rows are added/removed, whereas unsafe handlers are not aware
       * of such changes. For databases that are not affected by this kind of
       * change, unsafe handlers should be used instead of safe ones because
       * they are slightly faster. Both types of handlers are designed to be
       * created in parallel by several threads.
       *
       * Here is an example of how to use this class, illustrated on handlers
       * for a RawDatabaseTable:
       * @code
       * // create the database
       * gum::learning::RawDatabaseTable<> database;
       * database.setVariableNames( std::vector<std::string> {"v1","v2","v3"} );
       *
       * // add one row to the database
       * gum::learning::DBRow<gum::learning::DBCell>
       *   row( 3, gum::learning::DBCell(2) );
       * database.insertRow( row );
       *
       * // create a handler.
       * typename gum::learning::RawDatabaseTable<>::Handler handler( database );
       * // by default, the handlers range over the whole database, which
       * // currently contains only one row
       *
       * // here, we add 95 new rows into the database
       * for ( int i = 0; i < 95; ++i ) database.insertRow( row );
       *
       * // due to the addition of the rows, the (unsafe) handler still thinks
       * // there is only one row
       * std::cout << handler.size ();   // displays 1  (handler's range)
       * std::cout << handler.DBSize (); // displays 96 (database's size)
       *
       * // change the range of rows handled by the DBHandler
       * std::cout << handler.setRange ( 1, 40 ); // now parses rows [1,40)
       * std::cout << handler.size ();   // displays 39: rows 1,...,39
       * std::cout << handler.DBSize (); // displays 96: database's size
       * std::cout << handler.numRow (); // displays 0: the handler currently
       * // points on the first row of its managed area [1,40)
       *
       * // move the handler to the next row
       * handler.nextRow();
       * std::cout << handler.numRow (); // displays 1: the handler points now
       * // on the second row of its managed area. This corresponds to the third
       * // DBRow of the database since the range of handler is [1,40)
       * ++handler; // move again to the next row
       * std::cout << handler.numRow (); // displays 2
       * handler += 4; // advances the pointer by 4 rows
       * std::cout << handler.numRow (); // displays 6
       *
       * // get the DBRow pointed to by the handler: this is the 7th DBRow
       * // of the database
       * const auto& xrow7 = handler.row ();     // get the DBRow, unsafe version
       * const auto& yrow7 = handler.rowSafe (); // get the DBRow, safe version
       * const std::vector<gum::learning::DBCell>& xrow = xrow7.row ();
       * const double xweight = xrow27.weight ();
       *
       * // another way to access the row
       * const auto& zrow7 = *handler; // get the DBRow, unsafe version
       *
       * // check whether there exist other rows managed by the handler after
       * // the current row
       * bool has_rows = handler.hasRows (); // true: there remains 33 rows
       *
       * // makes the handler point again on the 2nd row of the database
       * handler.reset (); // the handler points to the beginning of its area
       * std::cout << handler.numRow (); // displays 0: the handler currently
       * // points on the first row of its managed area [1,40)
       *
       * // see the variables' names, i.e., the names of the database's columns
       * const auto& vars = handler.variableNames();
       *
       * // parse all the rows managed
       * handler.reset ();
       * for ( auto end = handler.end (); handler != end; ++handler )
       *   std::cout << handler.row ().weight () << std::endl;
       *
       * // another possibility:
       * for ( const auto& row : handler )
       *   std::cout << row.weight () << std::endl;
       * @endcode
       *
       * @ingroup learning_database
       */
      class Handler: public DBHandler< T_DATA > {
        public:
        /// Types for STL compliance.
        /// @{
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = typename DBHandler< T_DATA >::value_type;
        using reference         = value_type&;
        using const_reference   = const value_type&;
        using pointer           = value_type*;
        using const_pointer     = const value_type*;
        using difference_type   = std::ptrdiff_t;
        /// @}

        template < typename TX_DATA >
        using DBVector = std::vector< TX_DATA >;

        template < typename TX_DATA >
        using Row = DBRow< TX_DATA >;

        template < typename TX_DATA >
        using Matrix = std::vector< DBRow< TX_DATA > >;


        // ########################################################################
        /// @name Constructors / Destructors
        // ########################################################################
        /// @{

        /// default constructor
        /** @param db the database on which the handler will point to.
         * By default, the range of the handler is the whole database. */
        Handler(const IDatabaseTable< T_DATA >& db);

        /// copy constructor
        /** @param h the handler we wish to copy */
        Handler(const Handler& h);

        /// move constructor
        /** @param h the handler we wish to move */
        Handler(Handler&& h);

        /// destructor
        virtual ~Handler();

        /// @}

        // ########################################################################
        /// @name Operators
        // ########################################################################
        /// @{

        /// copy operator
        virtual Handler& operator=(const Handler&);

        /// move operator
        virtual Handler& operator=(Handler&&);

        /// makes the operator point to the next row in the database
        /** if the pointer has already reached the end of the area managed by the
         * handler, nothing happens. In particular, no exception is raised */
        virtual Handler& operator++() final;

        /// makes the operator point to the previous row in the database
        /** if the pointer is already at the beginning of the area managed
         * by the handler, nothing happens. In particular, no exception
         * is raised */
        virtual Handler& operator--() final;

        /// advances the handler by i rows in the database
        /** if, applying this move would make the handler reach the end of
         * the area managed by the handler, then the handler is kept at the
         * end of the area, i.e., after the last element of the area. */
        virtual Handler& operator+=(const std::size_t i) final;

        /// moves back the handler by i rows in the database
        /** if, applying this move would make the handler reach the beginning of
         * the area managed by the handler, then the handler is kept at the
         * beginning of the area, i.e., at the first element of the area. */
        virtual Handler& operator-=(const std::size_t i) final;

        /// checks whether two handlers point to the same row in the database
        virtual bool operator==(const Handler& handler) const final;

        /// checks whether two handlers point to different rows in the database
        virtual bool operator!=(const Handler& handler) const final;

        /// returns the current row pointed to by the handler (unsafe version)
        /** @warning The method does not check whether the handler already
         * points to the end of the area it manages. It is thus faster than
         * method rowSafe () but, when you call it, you must be sure that the row
         * actually exists, i.e., that the handler has not reached its end. */
        virtual const_reference operator*() const final;

        /// Dereferences the value pointed to by the handler (unsafe version)
        /** @warning The method does not check whether the handler already
         * points to the end of its area. It is thus faster than method
         * rowSafe () but, when you call it, you must be sure that the row
         * actually exists, i.e., that the handler has not reached its end. */
        virtual const_pointer operator->() const final;

        /// @}


        // ########################################################################
        /// @name Accessors / Modifiers
        // ########################################################################
        /// @{

        /// returns the number of rows managed by the handler
        /** A handler needs not necessarily handle all the rows of the database.
         * For instance, RecordCounters cut the database into several pieces and
         * assign each piece to a handler. Then each handler is used in parallel
         * to perform counts only on their subset of the database. The size
         * reported by method "size" is therefore the number of rows managed
         * by the handler. If you wish to retrieve the size of the whole database,
         * then use method DBSize instead. */
        virtual std::size_t size() const final;

        /// returns the number of rows of the whole database
        virtual std::size_t DBSize() const final;

        /// returns the current row pointed to by the handler (safe version)
        /** @throws OutOfBounds if the handler points to the end of its area */
        virtual const_reference rowSafe() const final;

        /// returns the current row pointed to by the handler (safe version)
        /** @throws OutOfBounds if the handler points to the end of its area */
        virtual reference rowSafe() final;

        /// returns the current row pointed to by the handler (unsafe version)
        /** @warning The method does not check whether the handler already
         * points to the end of its area. It is thus faster than method
         * rowSafe () but, when you call it, you must be sure that the row
         * actually exists, i.e., that the handler has not reached its end. */
        virtual const_reference row() const final;

        /// returns the current row pointed to by the handler (unsafe version)
        /** @warning The method does not check whether the handler already
         * points to the end of its area. It is thus faster than method
         * rowSafe () but, when you call it, you must be sure that the row
         * actually exists, i.e., that the handler has not reached its end. */
        virtual reference row() final;

        /// makes the handler point to the next row, equivalent to operator++
        virtual void nextRow() final;

        /// the number of the current row (0 = the 1st row managed by the handler)
        virtual std::size_t numRow() const final;

        /// indicates whether the handler has reached its end or not
        virtual bool hasRows() const final;

        /// puts the handler to the beginning of the database's area it handles
        virtual void reset() final;

        /** @brief returns a new handler that points to the beginning of the
         * database's area of the current handler
         *
         * @warning The handler returned manages precisely the same area
         * as the handler on which begin() is called. */
        virtual Handler begin() const;

        /** @brief returns a new handler that points to the end of the
         * database's area of the current handler
         *
         * @warning The handler returned manages precisely the same area
         * as the handler on which end() is called. */
        virtual Handler end() const;

        /// sets the area in the database the handler will handle
        /** In addition to setting the area that will be parsed by the handler,
         * this method makes the handler point to the beginning of the area.
         * @param first the first row to be handled
         * @param last the handler handles rows in interval [first,last). Thus,
         * the endth row is not included in the set of rows handled.
         * @warning if first is greater than last, these values are swapped.
         * @throw NullElement is raised if the handler does not point to
         * any database
         * @throw SizeError is raised if end is greater than the number of
         * rows of the database */
        virtual void setRange(std::size_t first, std::size_t last) final;

        /// returns the current range of the handler [begin,end)
        virtual std::pair< std::size_t, std::size_t > range() const final;

        /// returns the names of the variables
        virtual const DBVector< std::string >& variableNames() const final;

        /// returns the number of variables (columns) of the database
        virtual std::size_t nbVariables() const final;

        /// returns a pointer on the database
        /** @throw NullElement is raised if the handler does not point toward
         * any database. */
        virtual const IDatabaseTable< T_DATA >& database() const;

        /// @}


#ifndef DOXYGEN_SHOULD_SKIP_THIS

        protected:
        /// a reference to the whole database, including variable names
        const IDatabaseTable< T_DATA >* _db_;

        /// a reference on the database's records pointed to by the handler
        /** this data could be retrieved from  _db_ but we prefer using a
         * specific variable here for speed-up reasons. */
        const Matrix< T_DATA >* _row_;

        /// the index of the row currently pointed to by the handler
        std::size_t _index_{std::size_t(0)};

        /// the first row managed by the handler
        std::size_t _begin_index_{std::size_t(0)};

        /// the row just after the last one managed by the handler
        std::size_t _end_index_{std::size_t(0)};

        friend class IDatabaseTable< T_DATA >;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
      };


      /** @class HandlerSafe
       * @headerfile IDatabaseTable.h <agrum/BN/learning/IDatabaseTable.h>
       * @brief the safe handler of the tabular databases
       *
       * The IDatabaseTable class is provided with two types of handlers: unsafe
       * handlers and safe ones. Compared to the former, the safe handlers
       * incur a small overhead during their creation. But safe handlers
       * are informed by their associated database when the structure of
       * this one changes, i.e., when the number of rows/columns changes or
       * when rows are added/removed, whereas unsafe handlers are not aware
       * of such changes. For databases that are not affected by this kind of
       * change, unsafe handlers should be used instead of safe ones because
       * they are slightly faster. Both types of handlers are designed to be
       * created in parallel by several threads.
       *
       * Here is an example of how to use this class, illustrated on handlers
       * for a RawDatabaseTable:
       * @code
       * // create the database
       * gum::learning::RawDatabaseTable<> database;
       * database.setVariableNames( std::vector<std::string> {"v1","v2","v3"} );
       *
       * // add one row to the database
       * gum::learning::DBRow<gum::learning::DBCell>
       *   row( 3, gum::learning::DBCell(2) );
       * database.insertRow( row );
       *
       * // create a handler.
       * typename gum::learning::RawDatabaseTable<>::HandlerSafe handler(database);
       * // by default, the handlers range over the whole database, which
       * // currently contains only one row
       *
       * // here, we add 95 new rows into the database
       * for ( int i = 0; i < 95; ++i ) database.insertRow( row );
       *
       * // due to the addition of the rows, the safe handler updates its range
       * // and its area is now [0,96)
       * std::cout << handler.size ();   // displays 96 (handler's range)
       * std::cout << handler.DBSize (); // displays 96 (database's size)
       *
       * // change the range of rows handled by the DBHandler
       * std::cout << handler.setRange ( 1, 40 ); // now parses rows [1,40)
       * std::cout << handler.size ();   // displays 39: rows 1,...,39
       * std::cout << handler.DBSize (); // displays 96: database's size
       * std::cout << handler.numRow (); // displays 0: the handler currently
       * // points on the first row of its managed area [1,40)
       *
       * // move the handler to the next row
       * handler.nextRow();
       * std::cout << handler.numRow (); // displays 1: the handler points now
       * // on the second row of its managed area. This corresponds to the third
       * // DBRow of the database since the range of handler is [1,40)
       * ++handler; // move again to the next row
       * std::cout << handler.numRow (); // displays 2
       * handler += 4; // advances the pointer by 4 rows
       * std::cout << handler.numRow (); // displays 6
       *
       * // get the DBRow pointed to by the handler: this is the 7th DBRow
       * // of the database
       * const auto& xrow7 = handler.row ();     // get the DBRow, unsafe version
       * const auto& yrow7 = handler.rowSafe (); // get the DBRow, safe version
       * const std::vector<gum::learning::DBCell>& xrow = xrow7.row ();
       * const double xweight = xrow27.weight ();
       *
       * // another way to access the row
       * const auto& zrow7 = *handler; // get the DBRow, unsafe version
       *
       * // check whether there exist other rows managed by the handler after
       * // the current row
       * bool has_rows = handler.hasRows (); // true: there remains 33 rows
       *
       * // makes the handler point again on the 2nd row of the database
       * handler.reset (); // the handler points to the beginning of its area
       * std::cout << handler.numRow (); // displays 0: the handler currently
       * // points on the first row of its managed area [1,40)
       *
       * // see the variables' names, i.e., the names of the database's columns
       * const auto& vars = handler.variableNames();
       *
       * // parse all the rows managed
       * handler.reset ();
       * for ( auto end = handler.end (); handler != end; ++handler )
       *   std::cout << handler.row ().weight () << std::endl;
       *
       * // another possibility:
       * for ( const auto& row : handler )
       *   std::cout << row.weight () << std::endl;
       * @endcode
       *
       * @ingroup learning_database
       */
      class HandlerSafe: public Handler {
        public:
        /// Types for STL compliance.
        /// @{
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = typename Handler::value_type;
        using reference         = value_type&;
        using const_reference   = const value_type&;
        using pointer           = value_type*;
        using const_pointer     = const value_type*;
        using difference_type   = std::ptrdiff_t;
        /// @}

        // ########################################################################
        /// @name Constructors / Destructors
        // ########################################################################
        /// @{

        /// default constructor
        /** @param db the database on which the handler will point to.
         * By default, the range of the handler is the whole database. */
        HandlerSafe(const IDatabaseTable< T_DATA >& db);

        /// copy constructor
        HandlerSafe(const HandlerSafe& h);

        /// move constructor
        HandlerSafe(HandlerSafe&& h);

        /// destructor
        virtual ~HandlerSafe();

        /// @}

        // ########################################################################
        /// @name Operators
        // ########################################################################
        /// @{

        /// copy operator
        virtual HandlerSafe& operator=(const HandlerSafe&);

        /// copy operator
        virtual HandlerSafe& operator=(const Handler&);

        /// move operator
        virtual HandlerSafe& operator=(HandlerSafe&&);

        /// move operator
        virtual HandlerSafe& operator=(Handler&&);

        /// @}


#ifndef DOXYGEN_SHOULD_SKIP_THIS

        private:
        /// attach a new handler to the database
        void _attachHandler_();

        /// detach a handler
        void _detachHandler_();

        friend class IDatabaseTable< T_DATA >;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
      };


      /// Types for STL compliance.
      /// @{
      using value_type          = Row< T_DATA >;
      using reference           = value_type&;
      using const_reference     = const value_type&;
      using pointer             = value_type*;
      using const_pointer       = const value_type*;
      using size_type           = std::size_t;
      using difference_type     = std::ptrdiff_t;
      using iterator            = Handler;
      using iterator_safe       = HandlerSafe;
      using const_iterator      = const Handler;
      using const_iterator_safe = const HandlerSafe;
      /// @}


      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      IDatabaseTable(const MissingValType&             missing_symbols,
                     const std::vector< std::string >& var_names);

      /// copy constructor
      IDatabaseTable(const IDatabaseTable< T_DATA >& from);

      /// move constructor
      IDatabaseTable(IDatabaseTable< T_DATA >&& from);

      /// virtual copy constructor
      virtual IDatabaseTable< T_DATA >* clone() const = 0;

      /// destructor
      virtual ~IDatabaseTable();

      /// @}


      // ##########################################################################
      /// @name Iterators
      // ##########################################################################
      /// @{

      /// returns a new unsafe handler pointing to the beginning of the database
      iterator begin() const;

      /// returns a new safe handler pointing to the beginning of the database
      iterator_safe beginSafe() const;

      /// returns a new unsafe handler pointing to the end of the database
      const iterator& end() const noexcept;

      /// returns a new safe handler pointing to the end of the database
      const iterator_safe& endSafe() const noexcept;

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// returns the content (the records) of the database
      const Matrix< T_DATA >& content() const noexcept;

      /// returns a new unsafe handler pointing to the 1st record of the database
      iterator handler() const;

      /// returns a new safe handler pointing to the 1st record of the database
      iterator_safe handlerSafe() const;

      /// returns the variable names for all the columns of the database
      /** The names do not include the ignored columns. */
      const DBVector< std::string >& variableNames() const noexcept;

      /// sets the names of the variables
      /** This method can be called in two different ways: either the names
       * correspond precisely to the columns stored into the database table
       * (in this case, parameter from_external_object is equal to false),
       * or they corresponds to the columns of an external database (e.g., a
       * CSV file) from which we potentially excluded some columns and,
       * consequently, the latter should not be taken into account (in this
       * case, parameter from_external_object is equal to true). As an
       * example, imagine that the database table is created from a CSV file
       * with 5 columns named X0, X1, X2, X3 and X4 respectively. Suppose that
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
       * assigned to the columns of the IDatabaseTable because the size of their
       * vector is inadequate. */
      virtual void setVariableNames(const std::vector< std::string >& names,
                                    const bool                        from_external_object = true)
         = 0;

      /// returns the name of the kth column of the IDatabaseTable
      /** @throw OutOfBounds is raised if the IDatabaseTable contains fewer
       * than k columns. */
      const std::string& variableName(const std::size_t k) const;

      /// returns the index of the column whose name is passed in argument
      /** @warning If several columns correspond to the name, only the
       * column with the lowest index is returned. If you wish to retrieve all
       * the columns, use method columnsFromVariableName
       * @throw UndefinedElement is raised if there exists no column with
       * the given name*/
      std::size_t columnFromVariableName(const std::string& name) const;

      /// returns the indices of all the columns whose name is passed in argument
      /** It may happen that several columns correspond to a given variable
       * name. In this case, the function returns the indices of all the
       * columns of the IDatabase that match the name. */
      DBVector< std::size_t > columnsFromVariableName(const std::string& name) const;

      /// returns the number of variables (columns) of the database
      std::size_t nbVariables() const noexcept;

      /// returns the number of records (rows) in the database
      std::size_t nbRows() const noexcept;

      /// returns the number of records (rows) in the database
      std::size_t size() const noexcept;

      /// indicates whether the database contains some records or not
      bool empty() const noexcept;

      /// makes the database table ignore from now on the kth column
      /** This method can be called in two different ways: either k refers to
       * the current kth column of the database table (in this case, parameter
       * from_external_object is set to false), or k corresponds to the kth
       * column of an original dataset used to fill the database table
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
       * true, or the IDatabaseTable has no kth column when from_external_object
       * is set to false), column k is marked as to be ignored and nothing is
       * done on the content of the IDatabaseTable. No exception is raised.
       * @param k the column to remove. See the above detailed description on
       * how k is computed.
       * @param from_external_object indicates whether k refers to the kth
       * column of an original external database or to the current kth column
       * of the database table. */
      virtual void ignoreColumn(const std::size_t k, const bool from_external_object = true) = 0;

      /// returns  the set of columns of the original dataset that are ignored
      virtual const DBVector< std::size_t > ignoredColumns() const = 0;

      /** @brief returns the set of columns of the original dataset that are
       * present in the IDatabaseTable */
      virtual const DBVector< std::size_t > inputColumns() const = 0;

      using IDatabaseTableInsert4DBCell< !std::is_same< T_DATA, DBCell >::value >::insertRow;

      /// insert a new row at the end of the database
      /** The new_row passed in argument is supposed to come from an external
       * database. So it must contain data for the ignored columns.
       * @throw SizeError is raised if the vector of string cannot be inserted
       * in the IDatabaseTable because its size does not allow a matching with the
       * columns of the IDatabaseTable (taking into account the ignored columns) */
      virtual void insertRow(const std::vector< std::string >& new_row) = 0;

      /// insert a new DBRow at the end of the database
      /** Unlike methods insertRow for data whose type is different from T_DATA,
       * this method assumes that the new row passed in argument does not contain
       * any data of the ignored columns. So, basically, it could be copied
       * as is into the database table.
       * @throw SizeError is raised if the size of the new_row is not equal to
       * the number of columns retained in the IDatabaseTable */
      virtual void insertRow(Row< T_DATA >&& new_row, const IsMissing contains_missing_data);

      /// insert a new row at the end of the database
      /** Unlike methods insertRow for data whose type is different from T_DATA,
       * this method assumes that the new row passed in argument does not contain
       * any data of the ignored columns. So, basically, it could be copied
       * as is into the database table.
       * @throw SizeError is raised if the size of the new_row is not equal to
       * the number of columns retained in the IDatabaseTable */
      virtual void insertRow(const Row< T_DATA >& new_row, const IsMissing contains_missing_data);

      using IDatabaseTableInsert4DBCell< !std::is_same< T_DATA, DBCell >::value >::insertRows;

      /// insert a set of new DBRows at the end of the database
      /** Unlike methods insertRows for data whose type is different from T_DATA,
       * this method assumes that the new rows passed in argument do not contain
       * any data of the ignored columns. So, basically, these rows could be
       * copied as is into the database table.
       * @param new_rows the new set of rows to be copied as is
       * @param rows_have_missing_vals a vector of the same size as new_rows
       * that indicates, for each new row, whether it contains some missing
       * value or not
       * @throw SizeError is raised if the size of at least one row in new_rows
       * is not equal to the number of columns retained in the IDatabaseTable.
       * A SizeError exception will also be raised if the number of new rows
       * is not identical to the size of vector rows_have_missing_vals. */
      virtual void insertRows(Matrix< T_DATA >&&           new_rows,
                              const DBVector< IsMissing >& rows_have_missing_vals);

      /// insert a set of new DBRows at the end of the database
      /** Unlike methods insertRows for data whose type is different from T_DATA,
       * this method assumes that the new rows passed in argument do not contain
       * any data of the ignored columns. So, basically, these rows could be
       * copied as is into the database table.
       * @param new_rows the new set of rows to be copied as is
       * @param rows_have_missing_vals a vector of the same size as new_rows
       * that indicates, for each new row, whether it contains some missing
       * value or not
       * @throw SizeError is raised if the size of at least one row in new_rows
       * is not equal to the number of columns retained in the IDatabaseTable.
       * A SizeError exception will also be raised if the number of new rows
       * is not identical to the size of vector rows_have_missing_vals. */
      virtual void insertRows(const Matrix< T_DATA >&      new_rows,
                              const DBVector< IsMissing >& rows_have_missing_vals);

      /// erase a given row specified by its index in the table
      /** In the database, rows are indexed, starting from 0.
       * @warning If the row does not exist, nothing is done. In particular,
       * no exception is raised. */
      void eraseRow(std::size_t index);

      /// erase the first row
      /** @warning if the row does not exist, nothing is done. In particular, no
       * exception is raised. */
      void eraseFirstRow();

      /// erase the last row
      /** @warning if the row does not exist, nothing is done. In particular, no
       * exception is raised. */
      void eraseLastRow();

      /// erase the k first rows
      /** @warning if there are fewer than k rows in the database, the database is
       * completely emptied */
      void eraseFirstRows(const std::size_t k);

      /// erase the k last rows
      /** @warning if there are fewer than k rows in the database, the database is
       * completely emptied */
      void eraseLastRows(const std::size_t k);

      /// erase the rows from the debth to the endth (not included)
      /** In the database, rows are indexed, starting from 0. */
      void eraseRows(std::size_t deb, std::size_t end);

      /// erase all the rows
      void eraseAllRows();

      /// erase the content of the database, including the names of the variables
      virtual void clear();

      /// returns the set of missing symbols
      const DBVector< std::string >& missingSymbols() const;

      /// indicates whether the database contains some missing values
      bool hasMissingValues() const;

      /// indicates whether the kth row contains some missing values
      bool hasMissingValues(const std::size_t k) const;

      /// changes the max number of threads that a database can use
      /** Within databases, some methods can be processed in a parallel fashion.
       * This methods indicates the maximum number of threads that can be run
       * in parallel at the same time. */
      void setMaxNbThreads(const std::size_t nb) const;

      /// returns the number of threads used to parse the database
      std::size_t nbThreads() const;

      /** @brief changes the number min of rows a thread should process in a
       * multithreading context
       *
       * When a method executes several threads to perform actions on the rows
       * of the database, the MinNbRowsPerThread indicates how many rows each
       * thread should at least process. This is used to compute the number of
       * threads actually run. This number is equal to the min between the max
       * number of threads allowed and the number of records in the database
       * divided by nb. */
      void setMinNbRowsPerThread(const std::size_t nb) const;

      /// returns the minimum of rows that each thread should process
      std::size_t minNbRowsPerThread() const;

      /// assign a given weight to all the rows of the database
      void setAllRowsWeight(const double new_weight);

      /// assigns a given weight to the ith row of the database
      /** @throws OutOfBounds if i is outside the set of indices of the
       * records or if the weight is negative */
      void setWeight(const std::size_t i, const double weight);

      /// returns the weight of the ith record
      /** @throws OutOfBounds if i is outside the set of indices of the
       * records */
      double weight(const std::size_t i) const;

      /// returns the weight of the whole database
      double weight() const;

      /// @}


      protected:
      /// the names of the variables for each column
      DBVector< std::string > variable_names_;

      // the vector of DBRows containing all the raw data
      Matrix< T_DATA > rows_;

      // the set of string corresponding to missing values
      DBVector< std::string > missing_symbols_;

      // a vector indicating which rows have missing values (char != 0)
      DBVector< IsMissing > has_row_missing_val_;

      // the maximal number of threads that the database can use
      mutable std::size_t max_nb_threads_{std::size_t(gum::getNumberOfThreads())};

      // the min number of rows that a thread should process in a
      // multithreading context
      mutable std::size_t min_nb_rows_per_thread_{100};


      /** @brief checks whether a size corresponds to the number of columns
       * of the database */
      bool isRowSizeOK_(const std::size_t size) const;

      /// returns the number of threads used to process the current database content
      std::size_t nbProcessingThreads_() const;

      /// returns the ranges that threads should process
      std::vector< std::pair< std::size_t, std::size_t > >
         rangesProcessingThreads_(const std::size_t nb_threads) const;

      /// copy operator
      IDatabaseTable< T_DATA >& operator=(const IDatabaseTable< T_DATA >& from);

      /// move operator
      IDatabaseTable< T_DATA >& operator=(IDatabaseTable< T_DATA >&& from);


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      // the list of handlers currently attached to the database
      /* this is useful when the database is resized */
      mutable DBVector< HandlerSafe* > _list_of_safe_handlers_;

      // a mutex to safely add/remove handlers in  _list_of_safe_handlers_
      mutable std::mutex _safe_handlers_mutex_;

      // the end iterator for the database
      Handler* _end_{nullptr};

      // the safe end iterator for the database
      iterator_safe* _end_safe_{nullptr};

      /// add a new safe handler to the list of attached handlers
      void _attachHandler_(HandlerSafe* handler) const;

      /// detach a safe handler from the list of attached handlers
      void _detachHandler_(HandlerSafe* handler) const;

      /// update the handlers when the size of the database changes
      void _updateHandlers_(std::size_t new_size) const;

      // create the end iterators
      void _createEndIterators_();

#endif /* DOXYGEN_SHOULD_SKIP_THIS */


      /// allow the handlers to access the database directly
      friend class Handler;
      friend class HandlerSafe;
    };

  } /* namespace learning */

} /* namespace gum */

/// always include the templated implementations
#include <agrum/tools/database/IDatabaseTable_tpl.h>

#endif /* GUM_IDATABASE_TABLE_H */
