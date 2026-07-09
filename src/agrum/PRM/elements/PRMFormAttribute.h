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
 * @brief Headers of gum::PRMFormAttribute.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_FORM_ATTRIBUTE_H
#define GUM_FORM_ATTRIBUTE_H


#include <agrum/PRM/elements/PRMClass.h>

#include <string_view>

namespace gum {
  namespace prm {

    /**
     * @class PRMFormAttribute formAttribute.h
     *<agrum/PRM/elements/formAttribute.h>
     * @brief PRMFormAttribute is a member of a Class in a PRM.
     *
     * @see PRM PRMFactory Class PRMClassElement PRMType<GUM_SCALAR> Tensor
     *PRMAttribute
     * @ingroup prm_group
     */
    template < GUM_Numeric GUM_SCALAR >
    class PRMFormAttribute: public PRMAttribute< GUM_SCALAR > {
      public:
      PRMFormAttribute(const PRMClass< GUM_SCALAR >&          c,
                       std::string_view                       name,
                       const PRMType&                         type,
                       MultiDimImplementation< std::string >* impl
                       = new MultiDimArray< std::string >());

      ~PRMFormAttribute() override;

      /// See gum::prm::PRMAttribute.
      PRMAttribute< GUM_SCALAR >* newFactory(const PRMClass< GUM_SCALAR >& c) const override;

      /// See gum::prm::PRMAttribute.
      PRMAttribute< GUM_SCALAR >*
          copy(Bijection< const DiscreteVariable*, const DiscreteVariable* > bij) const override;

      /// See gum::prm::PRMAttribute.
      void copyCpf(const Bijection< const DiscreteVariable*, const DiscreteVariable* >& bif,
                   const PRMAttribute< GUM_SCALAR >& source) override;

      /// See gum::prm::PRMAttribute.
      typename PRMClassElement< GUM_SCALAR >::ClassElementType elt_type() const override;

      /// See gum::prm::PRMAttribute.
      PRMType& type() override;

      /// See gum::prm::PRMAttribute.
      const PRMType& type() const override;

      /// See gum::prm::PRMAttribute.
      const Tensor< GUM_SCALAR >& cpf() const override;

      /// See gum::prm::PRMAttribute.
      void addParent(const PRMClassElement< GUM_SCALAR >& elt) override;

      /// See gum::prm::PRMAttribute.
      void addChild(const PRMClassElement< GUM_SCALAR >& elt) override;

      /// See gum::prm::PRMAttribute.
      PRMAttribute< GUM_SCALAR >* getCastDescendant() const override;

      /// See gum::prm::PRMAttribute.
      void setAsCastDescendant(PRMAttribute< GUM_SCALAR >* attr) override;
      void becomeCastDescendant(PRMType& subtype) override;

      virtual MultiDimImplementation< std::string >&       formulas();
      virtual const MultiDimImplementation< std::string >& formulas() const;

      /// Swap old_type with new_type in the PRMClassElement cpt.
      void swap(const PRMType& old_type, const PRMType& new_type) override;

      protected:
      PRMType* type_() override;
      void     type_(PRMType* t) override;

      private:
      PRMFormAttribute(const PRMFormAttribute& source);
      PRMFormAttribute& operator=(const PRMFormAttribute& source);

      /// The random variable type of this attribute
      PRMType* _type_;

      /// A pointer on the Tensor of this attribute
      mutable Tensor< GUM_SCALAR >* _cpf_;

      /// A pointer on the Tensor of this attribute
      MultiDimImplementation< std::string >* _formulas_;

      /// A pointe toward the class of this attribute
      const PRMClass< GUM_SCALAR >* _class_;

      void _fillCpf_() const;
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class PRMFormAttribute< double >;
#endif


  } /* namespace prm */
}   // namespace gum

#include <agrum/PRM/elements/PRMFormAttribute_tpl.h>

#endif /* GUM_ATTRIBUTE_H */
