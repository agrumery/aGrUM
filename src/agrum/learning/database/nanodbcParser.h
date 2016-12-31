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

#ifndef GUM_NANODBC_H
#define GUM_NANODBC_H

#include <string>
#include <vector>

#include <agrum/config.h>
#include <agrum/learning/database/DBRow.h>

#include <agrum/learning/database/nanodbc/nanodbc.h>

namespace gum {

  namespace learning {

    class NanodbcParser {
      public:
      /**
       * @brief Default constructor.
       * @param result An SQL query result.
       */
      NanodbcParser( nanodbc::result& result );

      /**
       * @brief Class destructor.
       */
      virtual ~NanodbcParser();

      /**
       * @brief Gets the next line of the SQL stream and parses it.
       * @return Returns false if there is no next line.
       */
      bool next();

      /**
       * @return Returns the current parsed line.
       * @throw Raise gum::NullElement if there is no data.
       */
      const std::vector<std::string>& current() const;

      /**
       * Return the current noLine of parser line.
       */
      const Size noLine() const;

      private:
      nanodbc::result&         __result;
      std::vector<std::string> __data;
    };

  }  // namespace learning

}  // namespace gum

#ifndef GUM_NO_INLINE
#include <agrum/learning/database/nanodbcParser_inl.h>
#endif /* GUM_NO_INLINE */

#endif  // GUM_NANODBC_H

#endif  // _ODBC
