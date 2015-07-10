/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
 * @brief Headers of gum::Attribute.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_ATTRIBUTE_H
#define GUM_ATTRIBUTE_H

#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/PRM/elements/classElement.h>

namespace gum {
  namespace prm {

    /**
     * @class Attribute attribute.h <agrum/PRM/attribute.h>
     * @brief Attribute is a member of a Class in a PRM.
     *
     * A Attribute is defined by its name, its containing class, its type and
     * by a Conditional Probability Function (aka CPF but represented by a
     *Potential).
     *
     * An attribute in a PRM is the equivalent of a random variable in a
     *bayesian
     * network.
     *
     * This class is constructed by a gum::PRMFactory and is deleted by its
     * gum::Class.
     *
     * Built-in copies (copy constructor and copy operator) are illegal due to
     * various problems raised by redondant information.
     *
     * @see PRM PRMFactory Class ClassElement Type<GUM_SCALAR> Potential
     * @ingroup prm_group
     */
    template <typename GUM_SCALAR>
    class Attribute : public ClassElement<GUM_SCALAR> {

      public:
      // ========================================================================
      /// @name Constructors & destructor
      // ========================================================================
      /// @{
      Attribute(const std::string& name);

      /// Destructor.
      virtual ~Attribute();

      /// @}
      // ========================================================================
      /// @name Getters & setters
      // ========================================================================
      /// @{

      virtual Attribute* newFactory(const Class<GUM_SCALAR>& c) const = 0;
      virtual Attribute* copy(Bijection<const DiscreteVariable*,
                                        const DiscreteVariable*> bij) const = 0;

      virtual void copyCpf(const Bijection<const DiscreteVariable*,
                                           const DiscreteVariable*>& bif,
                           const Attribute<GUM_SCALAR>& source) = 0;

      /// See gum::ClassElement::elt_type().
      virtual typename ClassElement<GUM_SCALAR>::ClassElementType
      elt_type() const = 0;

      /// See gum::ClassElement::type().
      virtual Type<GUM_SCALAR>& type() = 0;

      /// See gum::ClassElement::type().
      virtual const Type<GUM_SCALAR>& type() const = 0;

      /// See gum::ClassElement::cpf().
      virtual const Potential<GUM_SCALAR>& cpf() const = 0;

      /// See gum::ClassElement::_addParent().
      virtual void addParent(const ClassElement<GUM_SCALAR>& elt) = 0;

      /// See gum::ClassElement::_addChild().
      virtual void addChild(const ClassElement<GUM_SCALAR>& elt) = 0;

      /**
       * @brief Returns a proper cast descendant of this Attribute.
       *
       * A cast descendant is an Attribute depending on this one which
       * cast it in this->type().super().
       *
       * The pointer is not deleted by this Attribute, so delete it yourself
       * after use.
       *
       * A new cast descendant is created for each call of this method.
       *
       * @return The cast descendant of this Attribute.
       *
       * @throw OperationNotAllowed Raised if it is not possible to create a
       *                            cast descendant for this Attribute.
       */
      virtual Attribute<GUM_SCALAR>* getCastDescendant() const = 0;

      /**
       * @brief Define attr as a cast descendant of this Attribute.
       *
       * When overloading an inherited Attribute using of subtype of it,
       * it is necessary to change the inherited Attribute CPF to make it
       * a proper cast descendant.
       *
       * Furthermore it is necessary to change the DiscreteVariable used
       * by this Attribute's super Type<GUM_SCALAR> in order to have the same
       *pointers in
       * both super Type<GUM_SCALAR> (i.e. this->type().super().variable()) and
       *the
       * cast descendant CPF (i.e. attr->cpf()).
       *
       * This can only be done if attr Type<GUM_SCALAR> is a direct subtype of
       *this
       * Attribute Type<GUM_SCALAR> (i.e. this->type().super() == attr->type()).
       *
       * @param attr The Attribute which is transformed to be this Attribute
       *             cast descendant.
       *
       * @throw OperationNotAllowed Raised if this Attribute can not have any
       *                            cast descendant.
       * @throw TypeError Raised if attr's Type<GUM_SCALAR> is not a direct
       *descendant of
       *                  this Attribute's Type<GUM_SCALAR>.
       */
      virtual void setAsCastDescendant(Attribute<GUM_SCALAR>* attr) = 0;

      /**
       * @brief Change this attribute to be a cast descendant of a an attribute
       * with type
       *        subtype.
       */
      virtual void becomeCastDescendant(Type<GUM_SCALAR>& subtype) = 0;

      /// Swap old_type with new_type in the ClassElement cpt.
      virtual void swap(const Type<GUM_SCALAR>& old_type,
                        const Type<GUM_SCALAR>& new_type) = 0;

      /// Set this as overload of source (necessayr to preserver internal
      /// pointers for MultiDims).
      virtual void overload(Attribute<GUM_SCALAR>* source);

      /// @}

      protected:
      Attribute(const Attribute<GUM_SCALAR>& source);

      virtual Type<GUM_SCALAR>* _type() = 0;
      virtual void _type(Type<GUM_SCALAR>* t) = 0;
    };

    extern template class Attribute<double>;

  } /* namespace prm */
}  // namespace gum

#include <agrum/PRM/elements/attribute.tcc>

#endif /* GUM_ATTRIBUTE_H */
