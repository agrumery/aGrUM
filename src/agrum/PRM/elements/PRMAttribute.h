/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
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
 * @brief Headers of gum::PRMAttribute.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_ATTRIBUTE_H
#define GUM_ATTRIBUTE_H

#include <agrum/PRM/elements/PRMClassElement.h>

namespace gum {
  namespace prm {

    /**
     * @class PRMAttribute
     * @headerfile attribute.h <agrum/PRM/attribute.h>
     * @brief PRMAttribute is a member of a Class in a PRM.
     *
     * A PRMAttribute is defined by its name, its containing class, its type and
     * by a Conditional Probability Function (aka CPF but represented by a
     * Tensor).
     *
     * An attribute in a PRM is the equivalent of a random variable in a
     * Bayesian network.
     *
     * This class is constructed by a gum::PRMFactory and is deleted by its
     * gum::Class.
     *
     * Built-in copies (copy constructor and copy operator) are illegal due to
     * various problems raised by redondant information.
     *
     * @see PRM PRMFactory Class PRMClassElement PRMType Tensor
     * @ingroup prm_group
     */
    template < typename GUM_SCALAR >
    class PRMAttribute: public PRMClassElement< GUM_SCALAR > {
      public:
      // ========================================================================
      /// @name Constructors & destructor
      // ========================================================================
      /// @{
      explicit PRMAttribute(const std::string& name);

      /// Destructor.
      virtual ~PRMAttribute();

      /// @}
      // ========================================================================
      /// @name Getters & setters
      // ========================================================================
      /// @{

      virtual PRMAttribute< GUM_SCALAR >* newFactory(const PRMClass< GUM_SCALAR >& c) const = 0;
      virtual PRMAttribute< GUM_SCALAR >*
          copy(Bijection< const DiscreteVariable*, const DiscreteVariable* > bij) const
          = 0;

      virtual void copyCpf(const Bijection< const DiscreteVariable*, const DiscreteVariable* >& bif,
                           const PRMAttribute< GUM_SCALAR >& source)
          = 0;

      /// See gum::PRMClassElement::elt_type().
      virtual typename PRMClassElement< GUM_SCALAR >::ClassElementType elt_type() const = 0;

      /// See gum::PRMClassElement::type().
      virtual PRMType& type() = 0;

      /// See gum::PRMClassElement::type().
      virtual const PRMType& type() const = 0;

      /// See gum::PRMClassElement::cpf().
      virtual const Tensor< GUM_SCALAR >& cpf() const = 0;

      /// See gum::PRMClassElement::addParent_().
      virtual void addParent(const PRMClassElement< GUM_SCALAR >& elt) = 0;

      /// See gum::PRMClassElement::addChild_().
      virtual void addChild(const PRMClassElement< GUM_SCALAR >& elt) = 0;

      /**
       * @brief Returns a proper cast descendant of this PRMAttribute.
       *
       * A cast descendant is an PRMAttribute depending on this one which
       * cast it in this->type().super().
       *
       * The pointer is not deleted by this PRMAttribute, so delete it yourself
       * after use.
       *
       * A new cast descendant is created for each call of this method.
       *
       * @return The cast descendant of this PRMAttribute.
       *
       * @throw OperationNotAllowed Raised if it is not possible to create a
       *                            cast descendant for this PRMAttribute.
       */
      virtual PRMAttribute< GUM_SCALAR >* getCastDescendant() const = 0;

      /**
       * @brief Define attr as a cast descendant of this PRMAttribute.
       *
       * When overloading an inherited PRMAttribute using of subtype of it,
       * it is necessary to change the inherited PRMAttribute CPF to make it
       * a proper cast descendant.
       *
       * Furthermore it is necessary to change the DiscreteVariable used
       * by this PRMAttribute's super PRMType in order to have the
       *same
       *pointers in
       * both super PRMType (i.e. this->type().super().variable())
       *and
       *the
       * cast descendant CPF (i.e. attr->cpf()).
       *
       * This can only be done if attr PRMType is a direct subtype
       *of
       *this
       * PRMAttribute PRMType (i.e. this->type().super() ==
       *attr->type()).
       *
       * @param attr The PRMAttribute which is transformed to be this
       *PRMAttribute
       *             cast descendant.
       *
       * @throw OperationNotAllowed Raised if this PRMAttribute can not have any
       *                            cast descendant.
       * @throw PRMTypeError Raised if attr's PRMType is not a direct
       *descendant of
       *                  this PRMAttribute's PRMType.
       */
      virtual void setAsCastDescendant(PRMAttribute< GUM_SCALAR >* attr) = 0;

      /**
       * @brief Change this attribute to be a cast descendant of a an attribute
       * with type
       *        subtype.
       */
      virtual void becomeCastDescendant(PRMType& subtype) = 0;

      /// Swap old_type with new_type in the PRMClassElement cpt.
      virtual void swap(const PRMType& old_type, const PRMType& new_type) = 0;

      /// Set this as overload of source (necessayr to preserver internal
      /// pointers for MultiDims).
      virtual void overload(PRMAttribute< GUM_SCALAR >* source);

      /// @}

      protected:
      PRMAttribute(const PRMAttribute< GUM_SCALAR >& source);

      virtual PRMType* type_()           = 0;
      virtual void     type_(PRMType* t) = 0;
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class PRMAttribute< double >;
#endif


  } /* namespace prm */
}   // namespace gum

#include <agrum/PRM/elements/PRMAttribute_tpl.h>

#endif /* GUM_ATTRIBUTE_H */
