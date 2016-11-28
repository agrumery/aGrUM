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
 * @brief The class representing a tabular database created from a SQL query.
 *
 * @author Lionel TORTI, Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifdef _ODBC

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <agrum/learning/database/databaseFromSQL.h>

namespace gum {

  namespace learning {

    /// copy constructor
    INLINE DatabaseFromSQL::DatabaseFromSQL( const DatabaseFromSQL& from )
        : DatabaseVectInRAM( from ) {
      // for debugging purposes
      GUM_CONS_CPY( DatabaseFromSQL );
    }

    /// move constructor
    INLINE DatabaseFromSQL::DatabaseFromSQL( DatabaseFromSQL&& from )
        : DatabaseVectInRAM( std::move( from ) ) {
      // for debugging purposes
      GUM_CONS_MOV( DatabaseFromSQL );
    }

    /// destructor
    INLINE DatabaseFromSQL::~DatabaseFromSQL() {
      GUM_DESTRUCTOR( DatabaseFromSQL );
    }

    /// copy operator
    INLINE DatabaseFromSQL& DatabaseFromSQL::
    operator=( const DatabaseFromSQL& from ) {
      DatabaseVectInRAM::operator=( from );
      return *this;
    }

    /// move constructor
    INLINE DatabaseFromSQL& DatabaseFromSQL::operator=( DatabaseFromSQL&& from ) {
      DatabaseVectInRAM::operator=( std::move( from ) );
      return *this;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

#endif  // _ODBC
