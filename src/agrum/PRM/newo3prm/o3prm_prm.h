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

#include <agrum/config.h>

#ifndef GUM_PRM_O3PRM_PRM_H
#define GUM_PRM_O3PRM_PRM_H

namespace gum {
  namespace prm {
    namespace o3prm {

      class o3prm_type {
        public:
        o3prm_type( std::string name, std::vector<std::string>& labels )
            : __name( name )
            , __labels( labels ) {
          GUM_CONSTRUCTOR( o3prm_type );
        }
        ~o3prm_type() { GUM_DESTRUCTOR( o3prm_type ); }

        const std::string& name() { return __name; } 
        const std::vector<std::string>& labels() { return __labels; } 


        private:
        std::string __name;
        std::vector<std::string> __labels;
      };

      class o3prm_prm {
        public:
        o3prm_prm() { GUM_CONSTRUCTOR( o3prm_prm ); }
        ~o3prm_prm() { GUM_DESTRUCTOR( o3prm_prm ); }

        std::vector<o3prm_type>& types() { return __types; }

        private:
        std::vector<o3prm_type> __types;
      };

    } // o3prm
  } // prm
} // gum

#endif // GUM_PRM_O3PRM_PRM_H

