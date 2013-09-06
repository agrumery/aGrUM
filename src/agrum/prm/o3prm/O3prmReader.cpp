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
 * @brief Implementation of O3prmReader.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <agrum/prm/o3prm/O3prmReader.h>

#ifdef GUM_NO_INLINE
#include <agrum/prm/o3prm/O3prmReader.inl>
#endif // GUM_NO_INLINE


namespace gum {

  namespace prm {

    namespace o3prm {

      void
      O3prmReader::setClassPath( const std::string& class_path ) {
        size_t i = 0;
        size_t j = class_path.find( ';' );

        while ( j != std::string::npos ) {
          addClassPath( class_path.substr( i, j - i ) );
          i = j + 1;

          if ( i < class_path.length() )
            j = class_path.find( ';', i );
          else
            j = std::string::npos;
        }

        if ( i < class_path.length() )
          addClassPath( class_path.substr( i, std::string::npos ) );
      }

      void O3prmReader::addClassPath( const std::string& class_path ) {
        if ( class_path[class_path.size()-1] == '/' )
          __class_path.push_back( class_path );
        else
          __class_path.push_back( class_path + '/' );
      }

    } /* namespace o3prm */
  } /* namespace prm */
} /* namespace gum */

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
