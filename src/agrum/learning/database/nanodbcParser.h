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
/**
 * @file
 * @brief Class for parsing SQL results using Nanodbc.
 *
 * @author Lionel TORTI, Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifdef _ODBC

#ifndef GUM_NANODBC_PARSER_H
#define GUM_NANODBC_PARSER_H

#include <string>
#include <cstdlib>
#include <vector>
#include <stdexcept>
#include <sql.h>

#include <agrum/agrum.h>
#include <agrum/external/nanodbc/nanodbc.h>

namespace gum {

  namespace learning {

    /** @class NanodbcParser
     * @headerfile NanodbcParser.h <agrum/learning/database/NanodbcParser.h>
     * @brief Class for parsing SQL results using Nanodbc.
     *
     * This class should probably not be used by itself. It is essentially used
     * by Class DBInitializerFromSQL.
     * @ingroup learning_database
     */
    template <template<typename> class ALLOC = std::allocator>
    class NanodbcParser {
    public:

      /// type for the allocators passed in arguments of methods
      using allocator_type = ALLOC<std::string>;


      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// Default constructor: create a parser without being connected
      NanodbcParser( const ALLOC<std::string>& alloc = ALLOC<std::string>() );

      /// constructor that executes an SQL query if the connection is active
      /** @param connection a nanODBC connection to a SQL database
       * @param query a string containing an SQL query
       * @param alloc The allocator that will be used by all methods
       */
      NanodbcParser( nanodbc::connection&      connection,
                     const std::string&        query,
                     const ALLOC<std::string>& alloc = ALLOC<std::string> () );

      /// destructor
      virtual ~NanodbcParser();

      /// @}


      // ########################################################################
      /// @name Accessors / Modifiers
      // ########################################################################
      /// @{

      /// Gets the next line of the SQL stream and parses it.
      /** @return false if there is no next line. */
      bool next();

      /// returns the current parsed line.
      /** @throw NullElement is raised if there is no data. */
      const std::vector<std::string,ALLOC<std::string>>& current() const;

      /// returns the current line number within the query
      std::size_t nbLine() const;

      /// returns the number of columns in the query result
      std::size_t nbColumns () const;

      /// returns the name of the ith column
      std::string columnName ( const std::size_t i ) const;

      /// start a new query
      void useNewQuery ( nanodbc::connection& connexion,
                         const std::string&   query );

      /// @}


#ifndef DOXYGEN_SHOULD_SKIP_THIS

    private:

      // the result of the last SQL query performed
      nanodbc::result __result;

      // the line number within the current query
      std::size_t __nb_line { std::size_t(0) };

      // a vector that will contain the content of the current line of result
      std::vector<std::string,ALLOC<std::string>> __data;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

    };

  }  // namespace learning

}  // namespace gum

#include <agrum/learning/database/nanodbcParser_tpl.h>

#endif  // GUM_NANODBC_PARSER_H

#endif  // _ODBC
