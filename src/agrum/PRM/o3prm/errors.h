/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Functions for error messages.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 * @author Lionel TORTI
 */

#include <sstream>
#include <string>
#include <vector>

#include <agrum/PRM/o3prm/O3prm.h>
#include <agrum/tools/core/errorsContainer.h>

#ifndef GUM_PRM_O3PRM_ERRORS_H
#  define GUM_PRM_O3PRM_ERRORS_H

#  ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {
  namespace prm {
    namespace o3prm {


      void O3PRM_TYPE_NOT_FOUND(const O3Label& val, ErrorsContainer& errors);

      void O3PRM_TYPE_AMBIGUOUS(const O3Label&                    val,
                                const std::vector< std::string >& matches,
                                ErrorsContainer&                  errors);

      void O3PRM_TYPE_RESERVED(const O3Label& val, ErrorsContainer& errors);

      void O3PRM_TYPE_DUPPLICATE(const O3Label& val, ErrorsContainer& errors);

      void O3PRM_TYPE_CYCLIC_INHERITANCE(const O3Label&   sub_type,
                                         const O3Label&   super_type,
                                         ErrorsContainer& errors);

      void O3PRM_TYPE_UNKNOWN_LABEL(const O3Label& type, const O3Label& l, ErrorsContainer& errors);

      void O3PRM_TYPE_INVALID_RANGE(const O3IntType& val, ErrorsContainer& errors);

      void O3PRM_TYPE_INVALID_RANGE(const O3RealType& val, ErrorsContainer& errors);

      void O3PRM_CLASS_NOT_FOUND(const O3Label& val, ErrorsContainer& errors);

      void O3PRM_CLASS_AMBIGUOUS(const O3Label&                    val,
                                 const std::vector< std::string >& matches,
                                 ErrorsContainer&                  errors);

      void O3PRM_CLASS_DUPLICATE(const O3Label& val, ErrorsContainer& errors);

      void O3PRM_CLASS_CYLIC_INHERITANCE(const O3Label&   sub,
                                         const O3Label&   super,
                                         ErrorsContainer& errors);

      void O3PRM_CLASS_ATTR_IMPLEMENTATION(const O3Label&   c,
                                           const O3Label&   i,
                                           const O3Label&   attr,
                                           ErrorsContainer& errors);

      void O3PRM_CLASS_AGG_IMPLEMENTATION(const O3Label&   c,
                                          const O3Label&   i,
                                          const O3Label&   attr,
                                          ErrorsContainer& errors);

      void O3PRM_CLASS_REF_IMPLEMENTATION(const O3Label&   c,
                                          const O3Label&   i,
                                          const O3Label&   ref,
                                          ErrorsContainer& errors);

      void O3PRM_CLASS_MISSING_ATTRIBUTES(const O3Label&   c,
                                          const O3Label&   i,
                                          ErrorsContainer& errors);

      void O3PRM_CLASS_DUPLICATE_REFERENCE(const O3Label& ref, ErrorsContainer& errors);

      void
         O3PRM_CLASS_SELF_REFERENCE(const O3Label& c, const O3Label& ref, ErrorsContainer& errors);

      void O3PRM_CLASS_ILLEGAL_SUB_REFERENCE(const O3Label&   c,
                                             const O3Label&   sub,
                                             ErrorsContainer& errors);

      void O3PRM_CLASS_PARENT_NOT_FOUND(const O3Label& parent, ErrorsContainer& errors);

      void O3PRM_CLASS_ILLEGAL_PARENT(const O3Label& parent, ErrorsContainer& errors);

      void O3PRM_CLASS_LINK_NOT_FOUND(const O3Label&     chain,
                                      const std::string& s,
                                      ErrorsContainer&   errors);

      void O3PRM_CLASS_ILLEGAL_CPT_SIZE(const std::string& c,
                                        const O3Label&     attr,
                                        Size               found,
                                        Size               expected,
                                        ErrorsContainer&   errors);

      void O3PRM_CLASS_ILLEGAL_CPT_VALUE(const std::string& c,
                                         const O3Label&     attr,
                                         const O3Formula&   f,
                                         ErrorsContainer&   errors);

      void O3PRM_CLASS_CPT_DOES_NOT_SUM_TO_1(const std::string& c,
                                             const O3Label&     attr,
                                             float              f,
                                             ErrorsContainer&   errors);

      void O3PRM_CLASS_CPT_DOES_NOT_SUM_TO_1_WARNING(const std::string& c,
                                                     const O3Label&     attr,
                                                     float              f,
                                                     ErrorsContainer&   errors);


      void O3PRM_CLASS_ILLEGAL_RULE_SIZE(const O3RuleCPT::O3Rule& rule,
                                         size_t                   found,
                                         size_t                   expected,
                                         ErrorsContainer&         errors);

      void O3PRM_CLASS_ILLEGAL_RULE_LABEL(const O3RuleCPT::O3Rule& rule,
                                          const O3Label&           label,
                                          const O3Label&           parent,
                                          ErrorsContainer&         errors);

      void O3PRM_CLASS_WRONG_PARENT(const O3Label& prnt, ErrorsContainer& errors);

      void O3PRM_CLASS_WRONG_PARENT_TYPE(const O3Label&     prnt,
                                         const std::string& expected,
                                         const std::string& found,
                                         ErrorsContainer&   errors);

      void O3PRM_CLASS_ILLEGAL_OVERLOAD(const O3Label&   elt,
                                        const O3Label&   super,
                                        ErrorsContainer& errors);

      void O3PRM_CLASS_AGG_PARAMETERS(const O3Label&   agg,
                                      Size             expected,
                                      Size             found,
                                      ErrorsContainer& errors);

      void O3PRM_CLASS_AGG_PARAMETER_NOT_FOUND(const O3Label&   agg,
                                               const O3Label&   param,
                                               ErrorsContainer& errors);

      void O3PRM_INTERFACE_ILLEGAL_ARRAY(const O3Label& val, ErrorsContainer& errors);

      void O3PRM_INTERFACE_NOT_FOUND(const O3Label& val, ErrorsContainer& errors);

      void O3PRM_INTERFACE_AMBIGUOUS(const O3Label&                    val,
                                     const std::vector< std::string >& matches,
                                     ErrorsContainer&                  errors);

      void O3PRM_INTERFACE_DUPLICATE(const O3Label& val, ErrorsContainer& errors);

      void O3PRM_INTERFACE_DUPLICATE_ELEMENT(const O3InterfaceElement& elt,
                                             ErrorsContainer&          errors);

      void O3PRM_INTERFACE_CYCLIC_INHERITANCE(const O3Label&   sub,
                                              const O3Label&   super,
                                              ErrorsContainer& errors);

      void O3PRM_INTERFACE_SELF_REFERENCE(const O3Interface&        i,
                                          const O3InterfaceElement& r,
                                          ErrorsContainer&          errors);

      void O3PRM_INTERFACE_ILLEGAL_SUB_REFERENCE(const O3Interface&        i,
                                                 const O3InterfaceElement& ref,
                                                 ErrorsContainer&          errors);

      void O3PRM_INTERFACE_ILLEGAL_OVERLOAD(const O3InterfaceElement& elt, ErrorsContainer& errors);

      void O3PRM_REFERENCE_NOT_FOUND(const O3Label& val, ErrorsContainer& errors);

      void O3PRM_REFERENCE_AMBIGUOUS(const O3Label&                    val,
                                     const std::vector< std::string >& matches,
                                     ErrorsContainer&                  errors);

      void O3PRM_SYSTEM_INSTANTIATION_FAILED(const O3System& sys, ErrorsContainer& errors);

      void O3PRM_SYSTEM_NOT_A_CLASS(const O3Instance& i, ErrorsContainer& errors);

      void O3PRM_SYSTEM_DUPLICATE_INSTANCE(const O3Instance& i, ErrorsContainer& errors);

      void O3PRM_SYSTEM_NOT_A_PARAMETER(const O3InstanceParameter& param, ErrorsContainer& errors);

      void O3PRM_SYSTEM_PARAMETER_NOT_FOUND(const O3InstanceParameter& param,
                                            ErrorsContainer&           errors);

      void O3PRM_SYSTEM_PARAMETER_NOT_INT(const O3InstanceParameter& param,
                                          ErrorsContainer&           errors);

      void O3PRM_SYSTEM_PARAMETER_NOT_FLOAT(const O3InstanceParameter& param,
                                            ErrorsContainer&           errors);

      void O3PRM_SYSTEM_INVALID_LEFT_VALUE(const O3Label& val, ErrorsContainer& errors);

      void O3PRM_SYSTEM_INSTANCE_NOT_FOUND(const O3Label& i, ErrorsContainer& errors);

      void O3PRM_SYSTEM_REFERENCE_NOT_FOUND(const O3Label&     ref,
                                            const std::string& type,
                                            ErrorsContainer&   errors);

      void O3PRM_SYSTEM_NOT_AN_ARRAY(const O3Label& val, ErrorsContainer& errors);

      void O3PRM_DEPRECATED_TYPE_WARNING(const O3Label& type, ErrorsContainer& errors);

    }   // namespace o3prm
  }     // namespace prm
}   // namespace gum

#  endif   // DOXYGEN_SHOULD_SKIP_THIS

#endif   // GUM_PRM_O3PRM_ERRORS_H
