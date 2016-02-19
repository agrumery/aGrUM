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

#include <agrum/PRM/newo3prm/errors.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      void O3PRM_TYPE_NOT_FOUND( const O3Label& val, ErrorsContainer& errors ) {
        auto pos = val.position();
        auto msg = std::stringstream();
        msg << "Type error : "
            << "Unknown type " << val.label();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_TYPE_AMBIGUOUS( const O3Label& val,
                                 const std::vector<std::string>& matches,
                                 ErrorsContainer& errors ) {
        const auto& pos = val.position();
        auto msg = std::stringstream();
        msg << "Type error : "
            << "Ambiguous name " << val.label()
            << ", found more than one elligible types: ";
        for ( auto i = 0; i < matches.size() - 1; ++i ) {
          msg << matches[i] << ", ";
        }
        msg << matches.back();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_TYPE_RESERVED( const O3Label& val, ErrorsContainer& errors ) {
        const auto& pos = val.position();
        auto msg = std::stringstream();
        msg << "Type error : "
            << "Type name " << val.label() << " is reserved";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_TYPE_DUPPLICATE( const O3Label& val,
                                  ErrorsContainer& errors ) {
        const auto& pos = val.position();
        auto msg = std::stringstream();
        msg << "Type error : "
            << "Type " << val.label() << " exists already";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_TYPE_CYCLIC_INHERITANCE( const O3Label& sub_type,
                                          const O3Label& super_type,
                                          ErrorsContainer& errors ) {
        const auto& pos = sub_type.position();
        auto msg = std::stringstream();
        msg << "Type error : "
            << "Cyclic inheritance between type " << sub_type.label()
            << " and type " << super_type.label();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_TYPE_UNKNOWN_LABEL( const O3Label& type,
                                     const O3Label& l,
                                     ErrorsContainer& errors ) {
        const auto& pos = l.position();
        auto msg = std::stringstream();
        msg << "Type error : "
            << "Unknown label " << l.label() << " in " << type.label();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_TYPE_INVALID_RANGE( const O3IntType& val,
                                     ErrorsContainer& errors ) {
        const auto& pos = val.name().position();
        auto msg = std::stringstream();
        msg << "Type error : "
            << "Invalid range " << val.start().value() << " -> "
            << val.end().value();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_CLASS_NOT_FOUND( const O3Label& val,
                                  ErrorsContainer& errors ) {
        const auto& pos = val.position();
        auto msg = std::stringstream();
        msg << "Class error : "
            << "Unknown class " << val.label();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_CLASS_AMBIGUOUS( const O3Label& val,
                                  const std::vector<std::string>& matches,
                                  ErrorsContainer& errors ) {
        const auto& pos = val.position();
        auto msg = std::stringstream();
        msg << "Class error : "
            << "Name " << val.label() << " is ambiguous: ";
        for ( auto i = 0; i < matches.size() - 1; ++i ) {
          msg << matches[i] << ", ";
        }
        msg << matches.back();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_INTERFACE_NOT_FOUND( const O3Label& val,
                                      ErrorsContainer& errors ) {
        const auto& pos = val.position();
        auto msg = std::stringstream();
        msg << "Interface error : "
            << "Interface " << val.label() << " not found";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_INTERFACE_AMBIGUOUS( const O3Label& val,
                                      const std::vector<std::string>& matches,
                                      ErrorsContainer& errors ) {
        const auto& pos = val.position();
        auto msg = std::stringstream();
        msg << "Inteface error : "
            << "Name " << val.label() << " is ambiguous: ";
        for ( auto i = 0; i < matches.size() - 1; ++i ) {
          msg << matches[i] << ", ";
        }
        msg << matches.back();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_INTERFACE_DUPLICATE( const O3Label& val,
                                      ErrorsContainer& errors ) {
        const auto& pos = val.position();
        auto msg = std::stringstream();
        msg << "Interface error : "
            << "Interface name " << val.label() << " exists already";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_INTERFACE_DUPLICATE_ELEMENT( const O3InterfaceElement& elt,
                                              ErrorsContainer& errors ) {
        const auto& pos = elt.type().position();
        auto msg = std::stringstream();
        msg << "Interface error : "
            << "Element " << elt.name().label() << " already exists";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_INTERFACE_CYCLIC_INHERITANCE( const O3Label& sub,
                                               const O3Label& super,
                                               ErrorsContainer& errors ) {
        const auto& pos = super.position();
        auto msg = std::stringstream();
        msg << "Interface error : "
            << "Cyclic inheritance between interface " << sub.label()
            << " and interface " << super.label();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_INTERFACE_SELF_REFERENCE( const O3Interface& i,
                                           const O3InterfaceElement& r,
                                           ErrorsContainer& errors ) {
        const auto& pos = r.type().position();
        auto msg = std::stringstream();
        msg << "Reference Slot error : "
            << "Interface " << i.name().label() << " cannot reference itself";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_INTERFACE_ILLEGAL_SUB_REFERENCE( const O3Interface& i,
                                                  const O3InterfaceElement& ref,
                                                  ErrorsContainer& errors ) {
        const auto& pos = ref.type().position();
        auto msg = std::stringstream();
        msg << "Reference Slot error : "
            << "Interface " << i.name().label()
            << " cannot reference subinterface " << ref.type().label();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_INTERFACE_ILLEGAL_SUPER_REFERENCE( const O3Interface& i,
                                                    const O3InterfaceElement& r,
                                                    ErrorsContainer& errors ) {
        const auto& pos = r.type().position();
        auto msg = std::stringstream();
        msg << "Reference Slot error : "
            << "Interface " << i.name().label()
            << " cannot reference super interface " << r.type().label();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_REFERENCE_NOT_FOUND( const O3Label& val,
                                      ErrorsContainer& errors ) {
        const auto& pos = val.position();
        auto msg = std::stringstream();
        msg << "Reference Slot error : "
            << "Reference Slot type " << val.label() << " not found";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_REFERENCE_AMBIGUOUS( const O3Label& val,
                                      const std::vector<std::string>& matches,
                                      ErrorsContainer& errors ) {
        const auto& pos = val.position();
        auto msg = std::stringstream();
        msg << "Reference Slot error : "
            << "Name " << val.label() << " is ambiguous: ";
        for ( auto i = 0; i < matches.size() - 1; ++i ) {
          msg << matches[i] << ", ";
        }
        msg << matches.back();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }


    }  // o3prm
  }    // prm
}  // gum

