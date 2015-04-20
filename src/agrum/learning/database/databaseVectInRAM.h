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
 * @brief The class representing a tabular database as a vector of DBRows stored
 * in RAM
 *
 * Class DatabaseVectInRAM represents a tabular database that stores in the
 * computer's random access memory (RAM) its content as a vector a DBRows.
 * Usually, users will not create directly DatabaseVectInRAM and fill them
 * manually but rather will use calles like DatabaseFromCSV that read data from
 * files and store them into a DatabaseVectInRAM.
 * @par Usage example:
 * @code
 * // create an empty database
 * DatabaseVectInRAM database;
 *
 * // create a new row with 3 DBCells containing integer 2
 * DBRow new_row ( 3, DBCell ( 2 ) );
 *
 * // add it into the database
 * database.insertDBRow ( row );
 * database.insertDBRow ( std::move ( row ) );
 *
 * // erase the first DBRow
 * database.eraseFirstDBRow ();
 *
 * returns the content of the database
 * const std::vector<DBRow>& content = database.content ();
 *
 * // sets the names of the variables (the columns) of the database
 * std::vector<std::string> new_names { "col1", "col2", "col3" };
 * database.setVariableNames ( new_names );
 *
 * // print the names of the columns
 * std::cout << database.variableNames () << std::endl;
 *
 * // print all the records of the database
 * for ( auto handler = database.handler ();
 *       handler.hasRows (); handler.nextRow () )
 *   std::cout << handler.row ();
 *
 * // make the handler parse the 3rd record to the 5th records
 * auto handler = database.handler ();
 * handler.setRange ( 2, 5 ); // 2 = 3rd record; 5 = 6th record (not included)
 * while ( handler.hasRows () ) {
 *   std::cout << handler.row ();
 *   handler.nextRow ();
 * }
 *
 * // clear the content of the database and update the database's handlers
 * database.clear ();
 * @endcode
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_DATABASE_VECT_IN_RAM_H
#define GUM_DATABASE_VECT_IN_RAM_H

#include <vector>
#include <string>

#include <agrum/config.h>
#include <agrum/learning/database/DBRow.h>
#include <agrum/learning/database/DBHandler.h>

namespace gum {

  namespace learning {

    /** @class DatabaseVectInRAM
     * @brief The class representing a tabular database as a vector of DBRows
     * stored in RAM
     * @ingroup learning_group
     * Class DatabaseVectInRAM represents a tabular database that stores in the
     * computer's random access memory (RAM) its content as a vector a DBRows.
     * Usually, users will not create directly DatabaseVectInRAM and fill them
     * manually but rather will use calles like DatabaseFromCSV that read data
     * from files and store them into a DatabaseVectInRAM.
     * @par Usage example:
     * @code
     * // create an empty database
     * DatabaseVectInRAM database;
     *
     * // create a new row with 3 DBCells containing integer 2
     * DBRow new_row ( 3, DBCell ( 2 ) );
     *
     * // add it into the database
     * database.insertDBRow ( row );
     * database.insertDBRow ( std::move ( row ) );
     *
     * // erase the first DBRow
     * database.eraseFirstDBRow ();
     *
     * returns the content of the database
     * const std::vector<DBRow>& content = database.content ();
     *
     * // sets the names of the variables (the columns) of the database
     * std::vector<std::string> new_names { "col1", "col2", "col3" };
     * database.setVariableNames ( new_names );
     *
     * // print the names of the columns
     * std::cout << database.variableNames () << std::endl;
     *
     * // print all the records of the database
     * for ( auto handler = database.handler ();
     *       handler.hasRows (); handler.nextRow () )
     *   std::cout << handler.row ();
     *
     * // make the handler parse the 3rd record to the 5th records
     * auto handler = database.handler ();
     * handler.setRange ( 2, 5 ); // 2 = 3rd record; 5 = 6th record (not included)
     * while ( handler.hasRows () ) {
     *   std::cout << handler.row ();
     *   handler.nextRow ();
     * }
     *
     * // clear the content of the database and update the database's handlers
     * database.clear ();
     * @endcode
     */
    class DatabaseVectInRAM {
      public:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
      /** @class DatabaseVectInRAM::Handler
       * @brief the handler of tabular databases represented as vectors of DBRows
       * @ingroup learning_group
       */
      class Handler : public DBHandler {
        public:
        // ########################################################################
        /// @name Constructors / Destructors
        // ########################################################################
        /// @{

        /// default constructor
        Handler(const DatabaseVectInRAM &db);

        /// copy constructor
        Handler(const Handler &h);

        /// move constructor
        Handler(Handler &&h);

        /// destructor
        virtual ~Handler();

        /// @}

        // ########################################################################
        /// @name Operators
        // ########################################################################
        /// @{

        /// copy operator
        Handler &operator=(const Handler &);

        /// move operator
        Handler &operator=(Handler &&);

        /// @}

        // ########################################################################
        /// @name Accessors / Modifiers
        // ########################################################################
        /// @{

        /// returns the number of rows managed by the handler
        /** A handler needs not necessarily handle all the rows of the database.
         * For instance, RecordCounters cut the database into several pieces and
         * assign each piece to a handler. Then each handler is used to perform
         * countings only on a subset of the database */
        unsigned long size() const noexcept;

        /// return the number of rows of the whole database
        unsigned long DBSize() const noexcept;

        /// returns the current row pointed to by the handler (safe version)
        /** @throws OutOfBounds if the handler points to the end of its area */
        const DBRow &rowSafe() const;

        /// returns the current row pointed to by the handler (safe version)
        /** @throws OutOfBounds if the handler points to the end of its area */
        DBRow &rowSafe();

        /// returns the current row pointed to by the handler (unsafe version)
        /** @warning The method does not check whether the handler already points
         * to the end of its area. It is thus faster than method rowSafe () but,
         * when you call it, you must be sure that the row actually exists, i.e.,
         * that the handler has not reached its end. */
        const DBRow &row() const;

        /// returns the current row pointed to by the handler (unsafe version)
        /** @warning The method does not check whether the handler already points
         * to the end of its area. It is thus faster than method rowSafe () but,
         * when you call it, you must be sure that the row actually exists, i.e.,
         * that the handler has not reached its end. */
        DBRow &row();

        /// makes the handler point to the next row
        void nextRow() noexcept;

        /// number of row
        Idx numRow() const noexcept;

        /// indicates whether the handler has reached its end or not
        bool hasRows() const noexcept;

        /// puts the handler to the beginning of the database area it handles
        void reset() noexcept;

        /// sets the area in the database the handler will handle
        /** @param begin the first row to be handled
         * @end the handler handles rows in interval [begin,end). Thus, the endth
         * row is not included in the set of rows handled. g*/
        void setRange(unsigned long begin, unsigned long end) noexcept;

        /// returns the current range of the handler [begin,end)
        std::pair<unsigned long, unsigned long> range() const noexcept;

        /// returns the names of the variables
        const std::vector<std::string> &variableNames() const noexcept;

        /// returns the number of variables (columns) of the database
        unsigned int nbVariables() const noexcept;

        /// @}

        private:
        /// a reference on the whole database, including variable names
        const DatabaseVectInRAM *__db;

        /// a reference on the database
        const std::vector<DBRow> *__row;

        /// the index of the row currently pointed to by the handler
        unsigned long __index{0};

        /// the first row managed by the handler
        unsigned long __begin_index{0};

        /// the row just after the last one managed by the handler
        unsigned long __end_index{0};

        /// attach a new handler to the database
        void __attachHandler();

        /// detach a handler
        void __detachHandler();

        friend class DatabaseVectInRAM;
      };

#endif // DOXYGEN_SHOULD_SKIP_THIS

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      DatabaseVectInRAM();

      /// copy constructor
      DatabaseVectInRAM(const DatabaseVectInRAM &);

      /// move constructor
      DatabaseVectInRAM(DatabaseVectInRAM &&);

      /// destructor
      virtual ~DatabaseVectInRAM();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      DatabaseVectInRAM &operator=(const DatabaseVectInRAM &from);

      /// move constructor
      DatabaseVectInRAM &operator=(DatabaseVectInRAM &&from);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// returns the content of the database
      const std::vector<DBRow> &content() const noexcept;

      /// returns a new handler on the database
      Handler handler() const;

      /// returns the variable names for all the columns
      const std::vector<std::string> &variableNames() const noexcept;

      /// sets the names of the variables
      void setVariableNames(const std::vector<std::string> &names);

      /// returns the number of variables (columns) of the database
      unsigned int nbVariables() const noexcept;

      /// insert a new DBRow at the end of the database
      void insertDBRow(const DBRow &new_row);

      /// insert a new DBRow at the end of the database
      void insertDBRow(DBRow &&new_row);

      /// insert a set of new DBRow at the end of the database
      void insertDBRows(const std::vector<DBRow> &new_rows);

      /// insert a set of new DBRows at the end of the database
      void insertDBRows(std::vector<DBRow> &&new_rows);

      /// erase a given row
      /** if the row does not exist, nothing is done. In particular, no
       * exception is raised. */
      void eraseDBRow(unsigned long index);

      /// erase the first row
      /** if the row does not exist, nothing is done. In particular, no
       * exception is raised. */
      void eraseFirstDBRow();

      /// erase the last row
      /** if the row does not exist, nothing is done. In particular, no
       * exception is raised. */
      void eraseLastDBRow();

      /// erase the k first rows
      /** if there are fewer than k rows in the database, the database is
       * completely emptied */
      void eraseFirstDBRows(unsigned long nb_rows);

      /// erase the k last rows
      /** if there are fewer than k rows in the database, the database is
       * completely emptied */
      void eraseLastDBRows(unsigned long nb_rows);

      /// erase the rows from the debth to the endth (not included)
      void eraseDBRows(unsigned long deb, unsigned long end);

      /// erase all the rows
      void eraseAllDBRows();

      /// erase the content of the database, including the names of the variables
      void clear();

      /// @}

      protected:
      /// returns the content of the database
      std::vector<DBRow> &_content() noexcept;

      /// returns the variable names for all the columns
      std::vector<std::string> &_variableNames() noexcept;

      private:
      /// the vector of DBRows
      std::vector<DBRow> __data;

      /// the names of the variables for each column
      std::vector<std::string> __variable_names;

      /// the list of handlers currently attached to the database
      /** this is useful when the database is resized */
      mutable std::vector<Handler *> __list_of_handlers;

      /// update the handlers when the size of the database changes
      void __updateHandlers(unsigned long new_size);

      /// allow the handlers to access the database directly
      friend class Handler;
    };

  } /* namespace learning */

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/database/databaseVectInRAM.inl>
#endif /* GUM_NO_INLINE */

#endif /* GUM_DATABASE_VECT_IN_RAM_H */
