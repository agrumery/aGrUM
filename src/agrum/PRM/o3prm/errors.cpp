/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Macros for error messages.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 * @author Lionel TORTI
 */

#include <agrum/PRM/o3prm/errors.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {
  namespace prm {
    namespace o3prm {

      void O3PRM_TYPE_NOT_FOUND(const O3Label& val, ErrorsContainer& errors) {
        auto pos = val.position();
        errors.addError(std::format("Error : Unknown type {}", val.label()),
                        pos.file(),
                        pos.line(),
                        pos.column());
      }

      void O3PRM_TYPE_AMBIGUOUS(const O3Label&                    val,
                                const std::vector< std::string >& matches,
                                ErrorsContainer&                  errors) {
        const auto& pos = val.position();
        std::string matchList;
        for (std::size_t i = 0; i < matches.size() - 1; ++i) {
          matchList += matches[i] + ", ";
        }
        matchList += matches.back();
        errors.addError(
            std::format("Error : Ambiguous name {}, found more than one elligible types: {}",
                        val.label(),
                        matchList),
            pos.file(),
            pos.line(),
            pos.column());
      }

      void O3PRM_TYPE_RESERVED(const O3Label& val, ErrorsContainer& errors) {
        const auto& pos = val.position();
        errors.addError(std::format("Error : Type name {} is reserved", val.label()),
                        pos.file(),
                        pos.line(),
                        pos.column());
      }

      void O3PRM_TYPE_DUPPLICATE(const O3Label& val, ErrorsContainer& errors) {
        const auto& pos = val.position();
        errors.addError(std::format("Error : Type {} exists already", val.label()),
                        pos.file(),
                        pos.line(),
                        pos.column());
      }

      void O3PRM_TYPE_CYCLIC_INHERITANCE(const O3Label&   sub_type,
                                         const O3Label&   super_type,
                                         ErrorsContainer& errors) {
        const auto& pos = sub_type.position();
        errors.addError(
            std::format("Error : Cyclic inheritance between type {} and type {}",
                        sub_type.label(),
                        super_type.label()),
            pos.file(),
            pos.line(),
            pos.column());
      }

      void
          O3PRM_TYPE_UNKNOWN_LABEL(const O3Label& type, const O3Label& l, ErrorsContainer& errors) {
        const auto& pos = l.position();
        errors.addError(std::format("Error : Unknown label {} in {}", l.label(), type.label()),
                        pos.file(),
                        pos.line(),
                        pos.column());
      }

      void O3PRM_TYPE_INVALID_RANGE(const O3IntType& val, ErrorsContainer& errors) {
        const auto& pos = val.position();
        errors.addError(
            std::format("Error : Invalid range {} -> {}", val.start().value(), val.end().value()),
            pos.file(),
            pos.line(),
            pos.column());
      }

      void O3PRM_TYPE_INVALID_RANGE(const O3RealType& val, ErrorsContainer& errors) {
        const auto& pos = val.position();
        errors.addError(
            std::format("Error : Found {} values in range expected at least 3",
                        val.values().size()),
            pos.file(),
            pos.line(),
            pos.column());
      }

      void O3PRM_CLASS_NOT_FOUND(const O3Label& val, ErrorsContainer& errors) {
        const auto& pos = val.position();
        const auto  msg = std::format("Error : Unknown class {}", val.label());
        errors.addError(msg, pos.file(), pos.line(), pos.column());
        GUM_ERROR(FatalError, msg)
      }

      void O3PRM_CLASS_AMBIGUOUS(const O3Label&                    val,
                                 const std::vector< std::string >& matches,
                                 ErrorsContainer&                  errors) {
        const auto& pos = val.position();
        std::string matchList;
        for (std::size_t i = 0; i < matches.size() - 1; ++i) {
          matchList += matches[i] + ", ";
        }
        matchList += matches.back();
        errors.addError(
            std::format("Error : Name {} is ambiguous: {}", val.label(), matchList),
            pos.file(),
            pos.line(),
            pos.column());
      }

      void O3PRM_CLASS_DUPLICATE(const O3Label& val, ErrorsContainer& errors) {
        // Raised if duplicate type names
        const auto& pos = val.position();
        errors.addError(std::format("Error : Class name {} exists already", val.label()),
                        pos.file(),
                        pos.line(),
                        pos.column());
      }

      void O3PRM_CLASS_CYLIC_INHERITANCE(const O3Label&   sub,
                                         const O3Label&   super,
                                         ErrorsContainer& errors) {
        // Cyclic inheritance
        const auto& pos = sub.position();
        errors.addError(
            std::format("Error : Cyclic inheritance between class {} and class {}",
                        sub.label(),
                        super.label()),
            pos.file(),
            pos.line(),
            pos.column());
      }

      void O3PRM_CLASS_ATTR_IMPLEMENTATION(const O3Label&   c,
                                           const O3Label&   i,
                                           const O3Label&   attr,
                                           ErrorsContainer& errors) {
        const auto& pos = attr.position();
        errors.addError(
            std::format("Error : Class {} attribute {} does not respect interface {}",
                        c.label(),
                        attr.label(),
                        i.label()),
            pos.file(),
            pos.line(),
            pos.column());
      }

      void O3PRM_CLASS_AGG_IMPLEMENTATION(const O3Label&   c,
                                          const O3Label&   i,
                                          const O3Label&   attr,
                                          ErrorsContainer& errors) {
        const auto& pos = attr.position();
        errors.addError(
            std::format("Error : Class {} aggregate {} does not respect interface {}",
                        c.label(),
                        attr.label(),
                        i.label()),
            pos.file(),
            pos.line(),
            pos.column());
      }

      void O3PRM_CLASS_REF_IMPLEMENTATION(const O3Label&   c,
                                          const O3Label&   i,
                                          const O3Label&   ref,
                                          ErrorsContainer& errors) {
        const auto& pos = ref.position();
        errors.addError(
            std::format("Error : Class {} reference {} does not respect interface {}",
                        c.label(),
                        ref.label(),
                        i.label()),
            pos.file(),
            pos.line(),
            pos.column());
      }

      void O3PRM_CLASS_MISSING_ATTRIBUTES(const O3Label&   c,
                                          const O3Label&   i,
                                          ErrorsContainer& errors) {
        const auto& pos = c.position();
        errors.addError(
            std::format("Error : Class {} does not implement all of interface {} attributes",
                        c.label(),
                        i.label()),
            pos.file(),
            pos.line(),
            pos.column());
      }

      void O3PRM_CLASS_DUPLICATE_REFERENCE(const O3Label& ref, ErrorsContainer& errors) {
        const auto& pos = ref.position();
        errors.addError(std::format("Error : Reference Slot name {} exists already", ref.label()),
                        pos.file(),
                        pos.line(),
                        pos.column());
      }

      void O3PRM_CLASS_SELF_REFERENCE(const O3Label&   c,
                                      const O3Label&   ref,
                                      ErrorsContainer& errors) {
        const auto& pos = ref.position();
        errors.addError(std::format("Error : Class {} cannot reference itself", c.label()),
                        pos.file(),
                        pos.line(),
                        pos.column());
      }

      void O3PRM_CLASS_ILLEGAL_SUB_REFERENCE(const O3Label&   c,
                                             const O3Label&   sub,
                                             ErrorsContainer& errors) {
        const auto& pos = sub.position();
        errors.addError(
            std::format("Error : Class {} cannot reference subclass {}", c.label(), sub.label()),
            pos.file(),
            pos.line(),
            pos.column());
      }

      void O3PRM_CLASS_PARENT_NOT_FOUND(const O3Label& parent, ErrorsContainer& errors) {
        const auto& pos = parent.position();
        errors.addError(std::format("Error : Parent {} not found", parent.label()),
                        pos.file(),
                        pos.line(),
                        pos.column());
      }

      void O3PRM_CLASS_ILLEGAL_PARENT(const O3Label& parent, ErrorsContainer& errors) {
        const auto& pos = parent.position();
        errors.addError(std::format("Error : Illegal parent {}", parent.label()),
                        pos.file(),
                        pos.line(),
                        pos.column());
      }

      void O3PRM_CLASS_LINK_NOT_FOUND(const O3Label&   chain,
                                      std::string_view s,
                                      ErrorsContainer& errors) {
        const auto& pos = chain.position();
        errors.addError(std::format("Error : Link {} in chain {} not found", s, chain.label()),
                        pos.file(),
                        pos.line(),
                        pos.column());
      }

      void O3PRM_CLASS_ILLEGAL_CPT_SIZE(std::string_view c,
                                        const O3Label&   attr,
                                        Size             found,
                                        Size             expected,
                                        ErrorsContainer& errors) {
        const auto& pos = attr.position();
        errors.addError(
            std::format("Error : Illegal CPT size, expected {} found {} for attribute {}.{}",
                        expected,
                        found,
                        c,
                        attr.label()),
            pos.file(),
            pos.line(),
            pos.column());
      }

      void O3PRM_CLASS_ILLEGAL_CPT_VALUE(std::string_view c,
                                         const O3Label&   attr,
                                         const O3Formula& f,
                                         ErrorsContainer& errors) {
        const auto& pos = f.position();
        auto        msg = std::format("Error : Illegal CPT value \"{}\" in attribute {}.{}",
                               f.formula().formula(),
                               c,
                               attr.label());
        try {
          auto result = f.formula().result();
          msg += std::format(", formula resolve to {}", result);
        } catch (...) {
          msg += std::format(", could not resolve the following formula: \"{}\"",
                             f.formula().formula());
        }
        errors.addError(msg, pos.file(), pos.line(), pos.column());
      }

      void O3PRM_CLASS_CPT_DOES_NOT_SUM_TO_1(std::string_view c,
                                             const O3Label&   attr,
                                             float            f,
                                             ErrorsContainer& errors) {
        const auto& pos = attr.position();
        errors.addError(
            std::format("Error : PRMAttribute {}.{} CPT does not sum to 1, found {}",
                        c,
                        attr.label(),
                        f),
            pos.file(),
            pos.line(),
            pos.column());
      }

      void O3PRM_CLASS_CPT_DOES_NOT_SUM_TO_1_WARNING(std::string_view c,
                                                     const O3Label&   attr,
                                                     float            f,
                                                     ErrorsContainer& errors) {
        const auto& pos = attr.position();
        errors.addWarning(
            std::format("Warning : PRMAttribute {}.{} CPT does not sum to 1, found {}",
                        c,
                        attr.label(),
                        f),
            pos.file(),
            pos.line(),
            pos.column());
      }

      void O3PRM_CLASS_ILLEGAL_RULE_SIZE(const O3RuleCPT::O3Rule& rule,
                                         size_t                   found,
                                         size_t                   expected,
                                         ErrorsContainer&         errors) {
        const auto& pos = rule.first.front().position();
        errors.addError(std::format("Error : Expected {} value(s), found {}", expected, found),
                        pos.file(),
                        pos.line(),
                        pos.column());
      }

      void O3PRM_CLASS_ILLEGAL_RULE_LABEL(const O3RuleCPT::O3Rule& rule,
                                          const O3Label&           label,
                                          const O3Label&           parent,
                                          ErrorsContainer&         errors) {
        const auto& pos = label.position();
        errors.addError(
            std::format("Error : Label {} is not part of {} domain",
                        label.label(),
                        parent.label()),
            pos.file(),
            pos.line(),
            pos.column());
      }

      void O3PRM_CLASS_WRONG_PARENT(const O3Label& prnt, ErrorsContainer& errors) {
        const auto& pos = prnt.position();
        errors.addError(std::format("Error : Illegal parent {}", prnt.label()),
                        pos.file(),
                        pos.line(),
                        pos.column());
      }

      void O3PRM_CLASS_WRONG_PARENT_TYPE(const O3Label&   prnt,
                                         std::string_view expected,
                                         std::string_view found,
                                         ErrorsContainer& errors) {
        const auto& pos = prnt.position();
        errors.addError(
            std::format("Error : Expected type {} for parent {}, found {}",
                        expected,
                        prnt.label(),
                        found),
            pos.file(),
            pos.line(),
            pos.column());
      }

      void O3PRM_CLASS_ILLEGAL_OVERLOAD(const O3Label&   elt,
                                        const O3Label&   c,
                                        ErrorsContainer& errors) {
        const auto& pos = elt.position();
        errors.addError(
            std::format("Error : Illegal overload of element {} from class {}",
                        elt.label(),
                        c.label()),
            pos.file(),
            pos.line(),
            pos.column());
      }

      void O3PRM_CLASS_AGG_PARAMETERS(const O3Label&   agg,
                                      Size             expected,
                                      Size             found,
                                      ErrorsContainer& errors) {
        const auto& pos = agg.position();
        errors.addError(
            std::format("Error : Expected {} parameters , found {}", expected, found),
            pos.file(),
            pos.line(),
            pos.column());
      }

      void O3PRM_CLASS_AGG_PARAMETER_NOT_FOUND(const O3Label&   agg,
                                               const O3Label&   param,
                                               ErrorsContainer& errors) {
        const auto& pos = param.position();
        errors.addError(
            std::format("Error : Parameter {} in aggregate {} does not match any expected values",
                        param.label(),
                        agg.label()),
            pos.file(),
            pos.line(),
            pos.column());
      }

      void O3PRM_INTERFACE_ILLEGAL_ARRAY(const O3Label& val, ErrorsContainer& errors) {
        const auto& pos = val.position();
        errors.addError(std::format("Error : PRMAttribute {} can not be an array", val.label()),
                        pos.file(),
                        pos.line(),
                        pos.column());
      }

      void O3PRM_INTERFACE_NOT_FOUND(const O3Label& val, ErrorsContainer& errors) {
        const auto& pos = val.position();
        errors.addError(std::format("Error : Interface {} not found", val.label()),
                        pos.file(),
                        pos.line(),
                        pos.column());
      }

      void O3PRM_INTERFACE_AMBIGUOUS(const O3Label&                    val,
                                     const std::vector< std::string >& matches,
                                     ErrorsContainer&                  errors) {
        const auto& pos = val.position();
        std::string matchList;
        for (std::size_t i = 0; i < matches.size() - 1; ++i) {
          matchList += matches[i] + ", ";
        }
        matchList += matches.back();
        errors.addError(
            std::format("Error : Name {} is ambiguous: {}", val.label(), matchList),
            pos.file(),
            pos.line(),
            pos.column());
      }

      void O3PRM_INTERFACE_DUPLICATE(const O3Label& val, ErrorsContainer& errors) {
        const auto& pos = val.position();
        errors.addError(std::format("Error : Interface name {} exists already", val.label()),
                        pos.file(),
                        pos.line(),
                        pos.column());
      }

      void O3PRM_INTERFACE_DUPLICATE_ELEMENT(const O3InterfaceElement& elt,
                                             ErrorsContainer&          errors) {
        const auto& pos = elt.type().position();
        errors.addError(std::format("Error : Element {} already exists", elt.name().label()),
                        pos.file(),
                        pos.line(),
                        pos.column());
      }

      void O3PRM_INTERFACE_CYCLIC_INHERITANCE(const O3Label&   sub,
                                              const O3Label&   super,
                                              ErrorsContainer& errors) {
        const auto& pos = super.position();
        errors.addError(
            std::format("Error : Cyclic inheritance between interface {} and interface {}",
                        sub.label(),
                        super.label()),
            pos.file(),
            pos.line(),
            pos.column());
      }

      void O3PRM_INTERFACE_SELF_REFERENCE(const O3Interface&        i,
                                          const O3InterfaceElement& r,
                                          ErrorsContainer&          errors) {
        const auto& pos = r.type().position();
        errors.addError(
            std::format("Error : Interface {} cannot reference itself", i.name().label()),
            pos.file(),
            pos.line(),
            pos.column());
      }

      void O3PRM_INTERFACE_ILLEGAL_SUB_REFERENCE(const O3Interface&        i,
                                                 const O3InterfaceElement& ref,
                                                 ErrorsContainer&          errors) {
        const auto& pos = ref.type().position();
        errors.addError(
            std::format("Error : Interface {} cannot reference subinterface {}",
                        i.name().label(),
                        ref.type().label()),
            pos.file(),
            pos.line(),
            pos.column());
      }

      void O3PRM_INTERFACE_ILLEGAL_OVERLOAD(const O3InterfaceElement& elt,
                                            ErrorsContainer&          errors) {
        const auto& pos = elt.type().position();
        errors.addError(
            std::format("Error : Illegal overload of element {}", elt.name().label()),
            pos.file(),
            pos.line(),
            pos.column());
      }

      void O3PRM_REFERENCE_NOT_FOUND(const O3Label& val, ErrorsContainer& errors) {
        const auto& pos = val.position();
        errors.addError(std::format("Error : Reference Slot type {} not found", val.label()),
                        pos.file(),
                        pos.line(),
                        pos.column());
      }

      void O3PRM_REFERENCE_AMBIGUOUS(const O3Label&                    val,
                                     const std::vector< std::string >& matches,
                                     ErrorsContainer&                  errors) {
        const auto& pos = val.position();
        std::string matchList;
        for (std::size_t i = 0; i < matches.size() - 1; ++i) {
          matchList += matches[i] + ", ";
        }
        matchList += matches.back();
        errors.addError(
            std::format("Error : Name {} is ambiguous: {}", val.label(), matchList),
            pos.file(),
            pos.line(),
            pos.column());
      }

      void O3PRM_SYSTEM_INSTANTIATION_FAILED(const O3System& sys, ErrorsContainer& errors) {
        const auto& pos = sys.name().position();
        errors.addError(
            std::format("Error : Could not instantiate the system, some reference slots must be "
                        "unassigned"),
            pos.file(),
            pos.line(),
            pos.column());
      }

      void O3PRM_SYSTEM_NOT_A_CLASS(const O3Instance& i, ErrorsContainer& errors) {
        const auto& pos = i.type().position();
        errors.addError(std::format("Error : {} is not a class", i.type().label()),
                        pos.file(),
                        pos.line(),
                        pos.column());
      }

      void O3PRM_SYSTEM_DUPLICATE_INSTANCE(const O3Instance& i, ErrorsContainer& errors) {
        const auto& pos = i.type().position();
        errors.addError(std::format("Error : Instance {} already exists", i.name().label()),
                        pos.file(),
                        pos.line(),
                        pos.column());
      }

      void O3PRM_SYSTEM_NOT_A_PARAMETER(const O3InstanceParameter& param, ErrorsContainer& errors) {
        const auto& pos = param.name().position();
        errors.addError(
            std::format("Instance error : {} is not a parameter", param.name().label()),
            pos.file(),
            pos.line(),
            pos.column());
      }

      void O3PRM_SYSTEM_PARAMETER_NOT_FOUND(const O3InstanceParameter& param,
                                            ErrorsContainer&           errors) {
        const auto& pos = param.name().position();
        errors.addError(std::format("Error : Parameter {} not found", param.name().label()),
                        pos.file(),
                        pos.line(),
                        pos.column());
      }

      void O3PRM_SYSTEM_PARAMETER_NOT_INT(const O3InstanceParameter& param,
                                          ErrorsContainer&           errors) {
        const auto& pos = param.value().position();
        errors.addError(std::format("Error : Parameter {} is an integer", param.name().label()),
                        pos.file(),
                        pos.line(),
                        pos.column());
      }

      void O3PRM_SYSTEM_PARAMETER_NOT_FLOAT(const O3InstanceParameter& param,
                                            ErrorsContainer&           errors) {
        const auto& pos = param.value().position();
        errors.addError(std::format("Error : Parameter {} is a float", param.name().label()),
                        pos.file(),
                        pos.line(),
                        pos.column());
      }

      void O3PRM_SYSTEM_INVALID_LEFT_VALUE(const O3Label& val, ErrorsContainer& errors) {
        const auto& pos = val.position();
        errors.addError(std::format("Error : Invalid left expression {}", val.label()),
                        pos.file(),
                        pos.line(),
                        pos.column());
      }

      void O3PRM_SYSTEM_INSTANCE_NOT_FOUND(const O3Label& i, ErrorsContainer& errors) {
        const auto& pos = i.position();
        errors.addError(std::format("Error : Instance {} not found", i.label()),
                        pos.file(),
                        pos.line(),
                        pos.column());
      }

      void O3PRM_SYSTEM_REFERENCE_NOT_FOUND(const O3Label&   ref,
                                            std::string_view type,
                                            ErrorsContainer& errors) {
        const auto& pos = ref.position();
        errors.addError(
            std::format("Error : Reference {} not found in class {}", ref.label(), type),
            pos.file(),
            pos.line(),
            pos.column());
      }

      void O3PRM_SYSTEM_NOT_AN_ARRAY(const O3Label& val, ErrorsContainer& errors) {
        const auto& pos = val.position();
        errors.addError(std::format("Error : {} is not an array", val.label()),
                        pos.file(),
                        pos.line(),
                        pos.column());
      }

      void O3PRM_DEPRECATED_TYPE_WARNING(const O3Label& val, ErrorsContainer& errors) {
        const auto& pos = val.position();
        errors.addWarning(
            std::format("Warning : {} is declared using a deprecated syntax.", val.label()),
            pos.file(),
            pos.line(),
            pos.column());
      }


    }   // namespace o3prm
  }   // namespace prm
}   // namespace gum

#endif   // DOXYGEN_SHOULD_SKIP_THIS
