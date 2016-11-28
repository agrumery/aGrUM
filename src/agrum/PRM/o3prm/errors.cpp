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

#include <agrum/PRM/o3prm/errors.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {
  namespace prm {
    namespace o3prm {

      void O3PRM_TYPE_NOT_FOUND( const O3Label& val, ErrorsContainer& errors ) {
        auto              pos = val.position();
        std::stringstream msg;
        msg << "Error : "
            << "Unknown type " << val.label();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_TYPE_AMBIGUOUS( const O3Label&                  val,
                                 const std::vector<std::string>& matches,
                                 ErrorsContainer&                errors ) {
        const auto&       pos = val.position();
        std::stringstream msg;
        msg << "Error : "
            << "Ambiguous name " << val.label()
            << ", found more than one elligible types: ";
        for ( std::size_t i = 0; i < matches.size() - 1; ++i ) {
          msg << matches[i] << ", ";
        }
        msg << matches.back();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_TYPE_RESERVED( const O3Label& val, ErrorsContainer& errors ) {
        const auto&       pos = val.position();
        std::stringstream msg;
        msg << "Error : "
            << "Type name " << val.label() << " is reserved";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_TYPE_DUPPLICATE( const O3Label& val, ErrorsContainer& errors ) {
        const auto&       pos = val.position();
        std::stringstream msg;
        msg << "Error : "
            << "Type " << val.label() << " exists already";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_TYPE_CYCLIC_INHERITANCE( const O3Label&   sub_type,
                                          const O3Label&   super_type,
                                          ErrorsContainer& errors ) {
        const auto&       pos = sub_type.position();
        std::stringstream msg;
        msg << "Error : "
            << "Cyclic inheritance between type " << sub_type.label()
            << " and type " << super_type.label();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_TYPE_UNKNOWN_LABEL( const O3Label&   type,
                                     const O3Label&   l,
                                     ErrorsContainer& errors ) {
        const auto&       pos = l.position();
        std::stringstream msg;
        msg << "Error : "
            << "Unknown label " << l.label() << " in " << type.label();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_TYPE_INVALID_RANGE( const O3IntType& val,
                                     ErrorsContainer& errors ) {
        const auto&       pos = val.position();
        std::stringstream msg;
        msg << "Error : "
            << "Invalid range " << val.start().value() << " -> "
            << val.end().value();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_TYPE_INVALID_RANGE( const O3RealType& val,
                                     ErrorsContainer&  errors ) {
        const auto&       pos = val.position();
        std::stringstream msg;
        msg << "Error : "
            << "Found " << val.values().size()
            << " values in range expected at least 3";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_CLASS_NOT_FOUND( const O3Label& val, ErrorsContainer& errors ) {
        const auto&       pos = val.position();
        std::stringstream msg;
        msg << "Error : "
            << "Unknown class " << val.label();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
        GUM_ERROR( FatalError, msg.str() );
      }

      void O3PRM_CLASS_AMBIGUOUS( const O3Label&                  val,
                                  const std::vector<std::string>& matches,
                                  ErrorsContainer&                errors ) {
        const auto&       pos = val.position();
        std::stringstream msg;
        msg << "Error : "
            << "Name " << val.label() << " is ambiguous: ";
        for ( std::size_t i = 0; i < matches.size() - 1; ++i ) {
          msg << matches[i] << ", ";
        }
        msg << matches.back();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_CLASS_DUPLICATE( const O3Label& val, ErrorsContainer& errors ) {
        // Raised if duplicate type names
        const auto&       pos = val.position();
        std::stringstream msg;
        msg << "Error : "
            << "Class name " << val.label() << " exists already";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_CLASS_CYLIC_INHERITANCE( const O3Label&   sub,
                                          const O3Label&   super,
                                          ErrorsContainer& errors ) {
        // Cyclic inheritance
        const auto&       pos = sub.position();
        std::stringstream msg;
        msg << "Error : "
            << "Cyclic inheritance between class " << sub.label() << " and class "
            << super.label();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }


      void O3PRM_CLASS_ATTR_IMPLEMENTATION( const O3Label&   c,
                                            const O3Label&   i,
                                            const O3Label&   attr,
                                            ErrorsContainer& errors ) {
        const auto&       pos = attr.position();
        std::stringstream msg;
        msg << "Error : "
            << "Class " << c.label() << " attribute " << attr.label()
            << " does not respect interface " << i.label();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_CLASS_AGG_IMPLEMENTATION( const O3Label&   c,
                                           const O3Label&   i,
                                           const O3Label&   attr,
                                           ErrorsContainer& errors ) {
        const auto&       pos = attr.position();
        std::stringstream msg;
        msg << "Error : "
            << "Class " << c.label() << " aggregate " << attr.label()
            << " does not respect interface " << i.label();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_CLASS_REF_IMPLEMENTATION( const O3Label&   c,
                                           const O3Label&   i,
                                           const O3Label&   ref,
                                           ErrorsContainer& errors ) {
        const auto&       pos = ref.position();
        std::stringstream msg;
        msg << "Error : "
            << "Class " << c.label() << " reference " << ref.label()
            << " does not respect interface " << i.label();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }


      void O3PRM_CLASS_MISSING_ATTRIBUTES( const O3Label&   c,
                                           const O3Label&   i,
                                           ErrorsContainer& errors ) {
        const auto&       pos = c.position();
        std::stringstream msg;
        msg << "Error : "
            << "Class " << c.label() << " does not implement all of interface "
            << i.label() << " attributes";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_CLASS_DUPLICATE_REFERENCE( const O3Label&   ref,
                                            ErrorsContainer& errors ) {
        const auto&       pos = ref.position();
        std::stringstream msg;
        msg << "Error : "
            << "Reference Slot name " << ref.label() << " exists already";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_CLASS_SELF_REFERENCE( const O3Label&   c,
                                       const O3Label&   ref,
                                       ErrorsContainer& errors ) {
        const auto&       pos = ref.position();
        std::stringstream msg;
        msg << "Error : "
            << "Class " << c.label() << " cannot reference itself";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_CLASS_ILLEGAL_SUB_REFERENCE( const O3Label&   c,
                                              const O3Label&   sub,
                                              ErrorsContainer& errors ) {
        const auto&       pos = sub.position();
        std::stringstream msg;
        msg << "Error : "
            << "Class " << c.label() << " cannot reference subclass "
            << sub.label();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_CLASS_PARENT_NOT_FOUND( const O3Label&   parent,
                                         ErrorsContainer& errors ) {
        const auto&       pos = parent.position();
        std::stringstream msg;
        msg << "Error : "
            << "Parent " << parent.label() << " not found";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_CLASS_ILLEGAL_PARENT( const O3Label&   parent,
                                       ErrorsContainer& errors ) {
        const auto&       pos = parent.position();
        std::stringstream msg;
        msg << "Error : "
            << "Illegal parent " << parent.label();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_CLASS_LINK_NOT_FOUND( const O3Label&     chain,
                                       const std::string& s,
                                       ErrorsContainer&   errors ) {
        const auto&       pos = chain.position();
        std::stringstream msg;
        msg << "Error : "
            << "Link " << s << " in chain " << chain.label() << " not found";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_CLASS_ILLEGAL_CPT_SIZE( const std::string& c,
                                         const O3Label&     attr,
                                         Size               found,
                                         Size               expected,
                                         ErrorsContainer&   errors ) {
        const auto&       pos = attr.position();
        std::stringstream msg;
        msg << "Error : "
            << "Illegal CPT size, expected " << expected << " found " << found
            << " for attribute " << c << "." << attr.label();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_CLASS_ILLEGAL_CPT_VALUE( const std::string& c,
                                          const O3Label&     attr,
                                          const O3Formula&   f,
                                          ErrorsContainer&   errors ) {
        const auto&       pos = f.position();
        std::stringstream msg;
        msg << "Error : "
            << "Illegal CPT value \"" << f.formula().formula()
            << "\" in attribute " << c << "." << attr.label();
        try {
          auto result = f.formula().result();
          msg << ", formula resolve to " << result;
        } catch ( ... ) {
          msg << ", could not resolve the following formula: "
              << "\"" << f.formula().formula() << "\"";
        }
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_CLASS_CPT_DOES_NOT_SUM_TO_1( const std::string& c,
                                              const O3Label&     attr,
                                              float              f,
                                              ErrorsContainer&   errors ) {
        const auto&       pos = attr.position();
        std::stringstream msg;
        msg << "Error : "
            << "PRMAttribute " << c << "." << attr.label()
            << " CPT does not sum to 1, found " << f;
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_CLASS_CPT_DOES_NOT_SUM_TO_1_WARNING( const std::string& c,
                                                      const O3Label&     attr,
                                                      float              f,
                                                      ErrorsContainer&   errors ) {
        const auto&       pos = attr.position();
        std::stringstream msg;
        msg << "Warning : "
            << "PRMAttribute " << c << "." << attr.label()
            << " CPT does not sum to 1, found " << f;
        errors.addWarning( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_CLASS_ILLEGAL_RULE_SIZE( const O3RuleCPT::O3Rule& rule,
                                          size_t                   found,
                                          size_t                   expected,
                                          ErrorsContainer&         errors ) {
        const auto&       pos = rule.first.front().position();
        std::stringstream msg;
        msg << "Error : "
            << "Expected " << expected << " value(s), found " << found;
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_CLASS_WRONG_PARENT( const O3Label&   prnt,
                                     ErrorsContainer& errors ) {
        const auto&       pos = prnt.position();
        std::stringstream msg;
        msg << "Error : "
            << "Illegal parent " << prnt;
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_CLASS_WRONG_PARENT_TYPE( const O3Label&     prnt,
                                          const std::string& expected,
                                          const std::string& found,
                                          ErrorsContainer&   errors ) {
        const auto&       pos = prnt.position();
        std::stringstream msg;
        msg << "Error : "
            << "Expected type " << expected << " for parent " << prnt.label()
            << ", found " << found;
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_CLASS_ILLEGAL_OVERLOAD( const O3Label&   elt,
                                         const O3Label&   c,
                                         ErrorsContainer& errors ) {
        const auto&       pos = elt.position();
        std::stringstream msg;
        msg << "Error : "
            << "Illegal overload of element " << elt.label() << " from class "
            << c.label();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_CLASS_AGG_PARAMETERS( const O3Label&   agg,
                                       Size             expected,
                                       Size             found,
                                       ErrorsContainer& errors ) {
        const auto&       pos = agg.position();
        std::stringstream msg;
        msg << "Error : "
            << "Expected " << expected << " parameters "
            << ", found " << found;
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_CLASS_AGG_PARAMETER_NOT_FOUND( const O3Label&   agg,
                                                const O3Label&   param,
                                                ErrorsContainer& errors ) {
        const auto&       pos = param.position();
        std::stringstream msg;
        msg << "Error : "
            << "Parameter " << param.label() << " in aggregate " << agg.label()
            << " does not match any expected values";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_INTERFACE_ILLEGAL_ARRAY( const O3Label&   val,
                                          ErrorsContainer& errors ) {
        const auto&       pos = val.position();
        std::stringstream msg;
        msg << "Error : "
            << "PRMAttribute " << val.label() << " can not be an array";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_INTERFACE_NOT_FOUND( const O3Label&   val,
                                      ErrorsContainer& errors ) {
        const auto&       pos = val.position();
        std::stringstream msg;
        msg << "Error : "
            << "Interface " << val.label() << " not found";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_INTERFACE_AMBIGUOUS( const O3Label&                  val,
                                      const std::vector<std::string>& matches,
                                      ErrorsContainer&                errors ) {
        const auto&       pos = val.position();
        std::stringstream msg;
        msg << "Error : "
            << "Name " << val.label() << " is ambiguous: ";
        for ( std::size_t i = 0; i < matches.size() - 1; ++i ) {
          msg << matches[i] << ", ";
        }
        msg << matches.back();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_INTERFACE_DUPLICATE( const O3Label&   val,
                                      ErrorsContainer& errors ) {
        const auto&       pos = val.position();
        std::stringstream msg;
        msg << "Error : "
            << "Interface name " << val.label() << " exists already";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_INTERFACE_DUPLICATE_ELEMENT( const O3InterfaceElement& elt,
                                              ErrorsContainer&          errors ) {
        const auto&       pos = elt.type().position();
        std::stringstream msg;
        msg << "Error : "
            << "Element " << elt.name().label() << " already exists";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_INTERFACE_CYCLIC_INHERITANCE( const O3Label&   sub,
                                               const O3Label&   super,
                                               ErrorsContainer& errors ) {
        const auto&       pos = super.position();
        std::stringstream msg;
        msg << "Error : "
            << "Cyclic inheritance between interface " << sub.label()
            << " and interface " << super.label();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_INTERFACE_SELF_REFERENCE( const O3Interface&        i,
                                           const O3InterfaceElement& r,
                                           ErrorsContainer&          errors ) {
        const auto&       pos = r.type().position();
        std::stringstream msg;
        msg << "Error : "
            << "Interface " << i.name().label() << " cannot reference itself";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_INTERFACE_ILLEGAL_SUB_REFERENCE( const O3Interface&        i,
                                                  const O3InterfaceElement& ref,
                                                  ErrorsContainer& errors ) {
        const auto&       pos = ref.type().position();
        std::stringstream msg;
        msg << "Error : "
            << "Interface " << i.name().label()
            << " cannot reference subinterface " << ref.type().label();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_INTERFACE_ILLEGAL_OVERLOAD( const O3InterfaceElement& elt,
                                             ErrorsContainer&          errors ) {
        const auto&       pos = elt.type().position();
        std::stringstream msg;
        msg << "Error : "
            << "Illegal overload of element " << elt.name().label();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_REFERENCE_NOT_FOUND( const O3Label&   val,
                                      ErrorsContainer& errors ) {
        const auto&       pos = val.position();
        std::stringstream msg;
        msg << "Error : "
            << "Reference Slot type " << val.label() << " not found";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_REFERENCE_AMBIGUOUS( const O3Label&                  val,
                                      const std::vector<std::string>& matches,
                                      ErrorsContainer&                errors ) {
        const auto&       pos = val.position();
        std::stringstream msg;
        msg << "Error : "
            << "Name " << val.label() << " is ambiguous: ";
        for ( std::size_t i = 0; i < matches.size() - 1; ++i ) {
          msg << matches[i] << ", ";
        }
        msg << matches.back();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_SYSTEM_INSTANTIATION_FAILED( const O3System&  sys,
                                              ErrorsContainer& errors ) {
        const auto&       pos = sys.name().position();
        std::stringstream msg;
        msg << "Error : "
            << "Could not instantiate the system, some reference slots must be "
               "unassigned";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_SYSTEM_NOT_A_CLASS( const O3Instance& i,
                                     ErrorsContainer&  errors ) {
        const auto&       pos = i.type().position();
        std::stringstream msg;
        msg << "Error : " << i.type().label() << " is not a class";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_SYSTEM_DUPLICATE_INSTANCE( const O3Instance& i,
                                            ErrorsContainer&  errors ) {
        const auto&       pos = i.type().position();
        std::stringstream msg;
        msg << "Error : "
            << "Instance " << i.name().label() << " already exists";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_SYSTEM_NOT_A_PARAMETER( const O3InstanceParameter& param,
                                         ErrorsContainer&           errors ) {
        const auto&       pos = param.name().position();
        std::stringstream msg;
        msg << "Instance error : " << param.name().label()
            << " is not a parameter";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_SYSTEM_PARAMETER_NOT_FOUND( const O3InstanceParameter& param,
                                             ErrorsContainer&           errors ) {
        const auto&       pos = param.name().position();
        std::stringstream msg;
        msg << "Error : "
            << "Parameter " << param.name().label() << " not found";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_SYSTEM_PARAMETER_NOT_INT( const O3InstanceParameter& param,
                                           ErrorsContainer&           errors ) {
        const auto&       pos = param.value().position();
        std::stringstream msg;
        msg << "Error : "
            << "Parameter " << param.name().label() << " is an integer";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_SYSTEM_PARAMETER_NOT_FLOAT( const O3InstanceParameter& param,
                                             ErrorsContainer&           errors ) {
        const auto&       pos = param.value().position();
        std::stringstream msg;
        msg << "Error : "
            << "Parameter " << param.name().label() << " is a float";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_SYSTEM_INVALID_LEFT_VALUE( const O3Label&   val,
                                            ErrorsContainer& errors ) {
        const auto&       pos = val.position();
        std::stringstream msg;
        msg << "Error : "
            << "Invalid left expression " << val.label();
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_SYSTEM_INSTANCE_NOT_FOUND( const O3Label&   i,
                                            ErrorsContainer& errors ) {
        const auto&       pos = i.position();
        std::stringstream msg;
        msg << "Error : "
            << "Instance " << i.label() << " not found";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_SYSTEM_REFERENCE_NOT_FOUND( const O3Label&     ref,
                                             const std::string& type,
                                             ErrorsContainer&   errors ) {
        const auto&       pos = ref.position();
        std::stringstream msg;
        msg << "Error : "
            << "Reference " << ref.label() << " not found in class " << type;
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }

      void O3PRM_SYSTEM_NOT_AN_ARRAY( const O3Label&   val,
                                      ErrorsContainer& errors ) {
        const auto&       pos = val.position();
        std::stringstream msg;
        msg << "Error : " << val.label() << " is not an array";
        errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
      }


    }  // o3prm
  }    // prm
}  // gum

#endif  // DOXYGEN_SHOULD_SKIP_THIS
