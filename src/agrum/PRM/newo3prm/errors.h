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
 * @brief Macros for error messages.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 * @author Lionel TORTI
 */

#include <string>
#include <sstream>
#include <vector>

#include <agrum/core/errorsContainer.h>
#include <agrum/PRM/newo3prm/o3prm.h>

#ifndef GUM_PRM_O3PRM_ERRORS_H
#define GUM_PRM_O3PRM_ERRORS_H

namespace gum {
  namespace prm {
    namespace o3prm {


      void O3PRM_TYPE_NOT_FOUND( const O3Label& val, ErrorsContainer& errors );

      void O3PRM_TYPE_AMBIGUOUS( const O3Label& val,
                                 const std::vector<std::string>& matches,
                                 ErrorsContainer& errors );

      void O3PRM_TYPE_RESERVED( const O3Label& val, ErrorsContainer& errors );

      void O3PRM_TYPE_DUPPLICATE( const O3Label& val, ErrorsContainer& errors );

      void O3PRM_TYPE_CYCLIC_INHERITANCE( const O3Label& sub_type,
                                          const O3Label& super_type,
                                          ErrorsContainer& errors );

      void O3PRM_TYPE_UNKNOWN_LABEL( const O3Label& type,
                                     const O3Label& l,
                                     ErrorsContainer& errors );

      void O3PRM_TYPE_INVALID_RANGE( const O3IntType& val,
                                     ErrorsContainer& errors );

      void O3PRM_CLASS_NOT_FOUND( const O3Label& val, ErrorsContainer& errors );

      void O3PRM_CLASS_AMBIGUOUS( const O3Label& val,
                                  const std::vector<std::string>& matches,
                                  ErrorsContainer& errors );

      void O3PRM_INTERFACE_NOT_FOUND( const O3Label& val,
                                      ErrorsContainer& errors );

      void O3PRM_INTERFACE_AMBIGUOUS( const O3Label& val,
                                      const std::vector<std::string>& matches,
                                      ErrorsContainer& errors );

      void O3PRM_INTERFACE_DUPLICATE( const O3Label& val,
                                      ErrorsContainer& errors );

      void O3PRM_INTERFACE_DUPLICATE_ELEMENT( const O3InterfaceElement& elt,
                                              ErrorsContainer& errors );

      void O3PRM_INTERFACE_CYCLIC_INHERITANCE( const O3Label& sub,
                                               const O3Label& super,
                                               ErrorsContainer& errors );

      void O3PRM_INTERFACE_SELF_REFERENCE( const O3Interface& i,
                                           const O3InterfaceElement& r,
                                           ErrorsContainer& errors );

      void O3PRM_INTERFACE_ILLEGAL_SUB_REFERENCE( const O3Interface& i,
                                                  const O3InterfaceElement& ref,
                                                  ErrorsContainer& errors );

      void O3PRM_INTERFACE_ILLEGAL_SUPER_REFERENCE( const O3Interface& i,
                                                    const O3InterfaceElement& r,
                                                    ErrorsContainer& errors );

      void O3PRM_REFERENCE_NOT_FOUND( const O3Label& val,
                                      ErrorsContainer& errors );

      void O3PRM_REFERENCE_AMBIGUOUS( const O3Label& val,
                                      const std::vector<std::string>& matches,
                                      ErrorsContainer& errors );


    }  // o3prm
  }    // prm
}  // gum

#endif  // GUM_PRM_O3PRM_ERRORS_H

