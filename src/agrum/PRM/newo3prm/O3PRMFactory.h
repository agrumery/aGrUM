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
 * @brief Implementation for the O3TypeFactory class.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 * @author Lionel TORTI
 */

#ifndef GUM_PRM_O3PRM_O3PRM_FACGTORY_H
#define GUM_PRM_O3PRM_O3PRM_FACGTORY_H

#include <regex>
#include <string>
#include <sstream>

#include <agrum/PRM/PRM.h>
#include <agrum/PRM/newO3prm/cocoR/Parser.h>
#include <agrum/PRM/newO3prm/cocoR/Scanner.h>
#include <agrum/PRM/newo3prm/o3prm.h>
#include <agrum/PRM/newo3prm/O3TypeFactory.h>
#include <agrum/PRM/newo3prm/O3InterfaceFactory.h>
#include <agrum/PRM/newo3prm/O3ClassFactory.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      void parse_stream( gum::prm::PRM<double>& prm,
                         std::stringstream& input,
                         std::stringstream& output );

    }
  }
}

#endif // GUM_PRM_O3PRM_O3PRM_FACGTORY_H

