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
 * @brief The base class for all database handlers
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_DB_HANDLER_H
#define GUM_LEARNING_DB_HANDLER_H

#include <cstddef>
#include <utility>
#include <memory>
#include <vector>
#include <string>

#include <agrum/agrum.h>
#include <agrum/tools/database/DBRow.h>

namespace gum {

  namespace learning {

    /** @class DBHandler
     * @brief The base class for all database handlers
     * @headerfile DBHandler.h <agrum/tools/database/DBHandler.h>
     *
     * Here is an example of how to use this class, illustrated on handlers
     * for a RawDatabaseTable:
     * @code
     * // create the database
     * gum::learning::RawDatabaseTable database;
     * database.setVariableNames( std::vector<std::string> { "v1", "v2", "v3" } );
     *
     * // add one row to the database
     * gum::learning::DBRow<gum::learning::DBCell>
     *   row( 3, gum::learning::DBCell(2) );
     * database.insertRow( row );
     *
     * // create a safe and an unsafe handler. Those inherit from DBHandler
     * typename gum::learning::RawDatabaseTable::HandlerSafe handler( database );
     * typename gum::learning::RawDatabaseTable::Handler uhandler( database );
     * // by default, the handlers range over the whole database, which
     * // currently contains only one row
     *
     * // here, we add 5 new rows into the database
     * for ( int i = 0; i < 5; ++i ) database.insertRow( row );
     *
     * // due to the addition of the rows, the safe handler is aware that there
     * // are now 6 rows. The unsafe handler still thinks there is only one row
     * std::cout << handler.range().second; // displays 6 (the last area's element)
     * std::cout << handler.size ();        // displays 6 (handler's range)
     * std::cout << handler.DBSize ();      // displays 6 (database's size)
     * std::cout << uhandler.size ();       // displays 1 (handler's range)
     * std::cout << uhandler.DBSize ();     // displays 6 (database's size)
     *
     * // change the range of rows handled by the DBHandler
     * std::cout << handler.setRange ( 1, 4 ); // now parses rows [1,4)
     * std::cout << handler.size ();   // displays 3: rows 1, 2, and 3
     * std::cout << handler.DBSize (); // displays 6: database's size
     * std::cout << handler.numRow (); // displays 0: the handler currently
     * // points on the first row of its managed area [1,4)
     *
     * uhandler.setRange ( 1, 4 ); // uhandler now parsed rows [1,4)
     * std::cout << uhandler.size ();   // displays 3: rows 1, 2, and 3
     * std::cout << uhandler.DBSize (); // displays 6: database's size
     * std::cout << uhandler.numRow (); // displays 0: the handler currently
     * // points on the first row of its managed area [1,4)
     *
     * // move the handler to the next row
     * handler.nextRow();
     * std::cout << handler.numRow (); // displays 1: the handler points now
     * // on the second row of its managed area. This corresponds to the third
     * // DBRow of the database since the range of handler is [1,4)
     *
     * // get the DBRow pointed to by the handler: this is the 3rd DBRow
     * // of the database
     * auto& xrow2 = handler.row ();     // get the DBRow, unsafe version
     * auto& yrow2 = handler.rowSafe (); // get the DBRow, safe version
     * const std::vector<gum::learning::DBCell>& xrow = xrow2.row ();
     * const double xweight = xrow2.weight ();
     *
     * // check whether there exist other rows managed by the handler after
     * // the current row
     * bool has_rows = handler.hasRows (); // true: there is still the 4th row
     * handler.nextRow();
     * bool has_rows2 = handler.hasRows (); // false: the 4th row is the last one
     *
     * // makes the handler point again on the 2nd row of the database
     * handler.reset ();
     * std::cout << handler.numRow (); // displays 0: the handler currently
     * // points on the first row of its managed area [1,4)
     *
     * // see the variables' names, i.e., the names of the database's columns
     * const auto& vars = handler.variableNames();
     * @endcode
     * @ingroup learning_database
     */
    template < typename T_DATA >
    class DBHandler {
      public:
      /// Types for STL compliance.
      /// @{
      using iterator_category = std::random_access_iterator_tag;
      using value_type        = DBRow< T_DATA >;
      using reference         = value_type&;
      using const_reference   = const value_type&;
      using pointer           = value_type*;
      using const_pointer     = const value_type*;
      using size_type         = std::size_t;
      using difference_type   = std::ptrdiff_t;
      /// @}

      /// the type for the allocated vectors in IDatabases
      template < typename TX_DATA >
      using DBVector = std::vector< TX_DATA >;


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// returns the number of rows managed by the handler
      /** A handler needs not necessarily handle all the rows of the database.
       * For instance, RecordCounters cut the database into several pieces and
       * assign each piece to a handler. Then each handler is used in parallel
       * to perform counts only on a subset of the database */
      virtual std::size_t size() const = 0;

      /// the number of rows in the whole database
      virtual std::size_t DBSize() const = 0;

      /// returns the current row of the database (safe version)
      /** @throws OutOfBounds is raised if the handler points outside of
       * its area */
      virtual const_reference rowSafe() const = 0;

      /// returns the current row of the database (safe version)
      /** @throws OutOfBounds is raised if the handler points outside of
       * its area */
      virtual reference rowSafe() = 0;

      /// returns the current row pointed to by the handler (unsafe version)
      /** @warning The method does not check whether the handler already points
       * to the end of its area. It is thus faster than method rowSafe () but,
       * when you call it, you must be sure that the row actually exists, i.e.,
       * that the handler has not reached its end. */
      virtual const_reference row() const = 0;

      /// returns the current row pointed to by the handler (unsafe version)
      /** @warning The method does not check whether the handler already points
       * to the end of its area. It is thus faster than method rowSafe () but,
       * when you call it, you must be sure that the row actually exists, i.e.,
       * that the handler has not reached its end. */
      virtual reference row() = 0;

      /// go to the next row in the database
      /** @warning If there is no more row, i.e., you are already on the last
       * DBRow managed or you point already to the end of the area, then the
       * handler will point on the end of the area. In particular, this will
       * not raise any exception. */
      virtual void nextRow() = 0;

      /// number of row the handler points to (from the beginning of the area)
      /** This method assigns 0 to the first row in the area handled by the
       * handler. So the number returned is the number of rows between the
       * currently pointed one to the beginning of the area handled. */
      virtual std::size_t numRow() const = 0;

      /// indicates wether there are still rows to parse in the database
      /** Remember that the handler manages only a specified area of the
       * database, so the method just indicates whether there still remains
       * rows in the area. */
      virtual bool hasRows() const = 0;

      /// puts the handler to the beginning of the database area it handles
      virtual void reset() = 0;

      /// sets the range of rows in the database that the handler will parse
      /** The range provided in arguments specifies that area [begin,end) is
       * the one managed by the DBHandler, i.e., the first row of the area
       * has index begin in the whole database, and the last row of the area
       * has index end-1 in the whole database. The endth row is the first one
       * outside the area.
       * @param begin the number of the row in the whole database that will
       * be the first one in the area managed by the DBHandler.
       * @param end the number of the row in the whole database from which
       * the DBHandler considers it is outside of its area. */
      virtual void setRange(std::size_t begin, std::size_t end) = 0;

      /// returns the current range of rows of the handler
      /** The range returned is of type [begin,end), i.e., the first row of the
       * range managed by the DBHandler has index begin in the whole database,
       * and the last row of the range has index end-1 in the whole database.
       * The endth row is therefore the first one outside the range.*/
      virtual std::pair< std::size_t, std::size_t > range() const = 0;

      /// returns the names of the variables corresponding to the rows
      virtual const DBVector< std::string >& variableNames() const = 0;

      /// returns the number of variables (columns) of the database
      virtual std::size_t nbVariables() const = 0;

      /// @}


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      protected:
      // the cache used to avoid cacheline problems due to parallelism
      static constexpr int cache_size_{128};

      // a buffer to avoid cacheline problems due to parallelism
      char align_[cache_size_];

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */

#endif /* GUM_LEARNING_DB_HANDLER_H */
