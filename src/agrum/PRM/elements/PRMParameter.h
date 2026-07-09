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
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
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
 * @brief Headers of gum::PRMParameter.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_PARAMETER_H
#define GUM_PARAMETER_H


#include <agrum/PRM/elements/PRMAttribute.h>
#include <agrum/PRM/elements/PRMType.h>

namespace gum {
  namespace prm {

    /**
     * @class PRMParameter
     * @headerfile parameter.h <agrum/PRM/parameter.h>
     * @brief PRMParameter is a member of a Class in a PRM.
     *
     * @see PRM PRMFactory Class PRMClassElement
     * @ingroup prm_group
     */
    template < GUM_Numeric GUM_SCALAR >
    class PRMParameter: public PRMClassElement< GUM_SCALAR > {
      public:
      enum ParameterType { INT, REAL };

      // ========================================================================
      /// @name Constructor & destructor.
      // ========================================================================
      /// @{

      /**
       * @brief Constructor used by gum::Class.
       *
       * @param name
       * @param type
       * @param value
       */
      PRMParameter(std::string_view name, ParameterType type, GUM_SCALAR value);

      /// Destructor.
      ~PRMParameter() override;

      /// @}
      // ========================================================================
      /// @name Getters & setters
      // ========================================================================
      /// @{

      /// See gum::PRMClassElement::elt_type().
      typename PRMClassElement< GUM_SCALAR >::ClassElementType elt_type() const override;

      GUM_SCALAR value() const;

      void value(GUM_SCALAR value);

      ParameterType valueType() const;

      /// See gum::PRMClassElement::type().
      PRMType& type() override;

      /// See gum::PRMClassElement::type().
      const PRMType& type() const override;

      /// See gum::PRMClassElement::cpf().
      virtual Tensor< GUM_SCALAR >& cpf();

      /// See gum::PRMClassElement::cpf().
      const Tensor< GUM_SCALAR >& cpf() const override;

      /// See gum::PRMClassElement::addParent().
      void addParent(const gum::prm::PRMClassElement< GUM_SCALAR >&) override;

      /// See gum::PRMClassElement::addChild().
      void addChild(const gum::prm::PRMClassElement< GUM_SCALAR >&) override;

      /// @brief Raise a gum::OperationNotAllowed
      /// See gum::PRMClassElement::getCastDescendant()
      PRMAttribute< GUM_SCALAR >* getCastDescendant() const override;

      /// @}

      protected:
      /// Copy constructor. Don't use it.
      PRMParameter(const PRMParameter< GUM_SCALAR >& source);

      /// Copy operator. Don't use it.
      PRMParameter< GUM_SCALAR >& operator=(const PRMParameter< GUM_SCALAR >& from);

      private:
      // ========================================================================
      /// @name Private members
      // ========================================================================
      /// @{

      ParameterType _type_;

      GUM_SCALAR _value_;

      /// @}
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class PRMParameter< double >;
#endif


  } /* namespace prm */
}   // namespace gum

#include <agrum/PRM/elements/PRMParameter_tpl.h>

#endif /* GUM_PARAMETER_H */
