/**************************************************************************
*   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
 * @brief Headers for parsing function for the O3PRM language.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 * @author Lionel TORTI
 */

#include <string>
#include <vector>
#include <utility>

#include <agrum/config.h>
#include <agrum/core/hashTable.h>

#ifndef GUM_PRM_O3PRM_PRM_H
#define GUM_PRM_O3PRM_PRM_H

namespace gum {
  namespace prm {
    namespace o3prm {

      class Position {
        public:
        Position();
        Position( const std::string& filename, int line, int column );
        Position( const Position& src );
        Position( Position&& src );
        ~Position();
        const std::string& file() const;
        void file( const std::string& file );
        int line() const;
        void line( int line );
        int column() const;
        void column( int column );

        private:
        std::string __file;
        int __line;
        int __column;
      };

      class O3Type {
        public:
        using LabelMap = std::vector<std::pair<std::string, std::string>>;
        O3Type( Position pos, std::string name, std::string super, LabelMap& labels );
        O3Type( const O3Type& src );
        O3Type( O3Type&& src );
        ~O3Type();

        const std::string& name() const;
        const std::string& super() const;
        const LabelMap& labels() const;
        const Position& position() const;

        private:
        Position __position;
        std::string __name;
        std::string __super;
        LabelMap __labels;
      };

      class O3PRM {
        public:
        O3PRM();
        O3PRM( const O3PRM& src );
        O3PRM( O3PRM&& src );
        ~O3PRM();

        std::vector<O3Type>& types();
        const std::vector<O3Type>& types() const;

        private:
        std::vector<O3Type> __types;
      };

    } // o3prm
  } // prm
} // gum

#endif // GUM_PRM_O3PRM_PRM_H

