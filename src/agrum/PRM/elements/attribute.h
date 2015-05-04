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
     * An attribute in a PRM is the equivalent of a random variable in a bayesian
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
      // ========================================================================
      friend class Class<GUM_SCALAR>;
      friend class Interface<GUM_SCALAR>;
      friend class Instance<GUM_SCALAR>;
      // ========================================================================
      public:
      // ========================================================================
      /// @name Constructor & destructor.
      // ========================================================================
      /// @{

      /**
       * @brief Constructor used by gum::Class.
       * This will create an Attribute with only one variable: type and with the
       * given
       * implementation.
       * @param name The name of this Attribute.
       * @param type The type of this Attribute, it is copied.
       * @param impl The MultiDimImplementation used by the internal Potential of
       * this Attribute.
       *             it will be deleted after the call of ~Attribute.
       */
      Attribute(const std::string &name, const Type<GUM_SCALAR> &type,
                MultiDimImplementation<GUM_SCALAR> *impl =
                    new MultiDimArray<GUM_SCALAR>());

      /**
       * @brief Constructor used by gum::Instance.
       * This will create an Attribute with a ready Potential, however it will check
       * the existence
       * of type in cpf and raise an exception if it is not found.
       * @param type The type of this attribute, it will be deleted after a call to
       * ~Attribute.
       * @param name The name of this Attribute.
       * @param cpf The Potential of this Attribute, it will be deleted after the
       * call of
       *            ~Attribute.
       * @param delete_type If true, the type is deleted with this instance.
       */
      Attribute(const std::string &name, Type<GUM_SCALAR> *type,
                Potential<GUM_SCALAR> *cpf, bool delete_type);

      /// Destructor.
      virtual ~Attribute();

      /// @}
      // ========================================================================
      /// @name Getters & setters
      // ========================================================================
      /// @{

      /// See gum::ClassElement::elt_type().
      virtual typename ClassElement<GUM_SCALAR>::ClassElementType elt_type() const;

      /// See gum::ClassElement::type().
      virtual Type<GUM_SCALAR> &type();

      /// See gum::ClassElement::type().
      virtual const Type<GUM_SCALAR> &type() const;

      /// See gum::ClassElement::cpf().
      virtual Potential<GUM_SCALAR> &cpf();

      /// See gum::ClassElement::cpf().
      virtual const Potential<GUM_SCALAR> &cpf() const;

      /// See gum::ClassElement::_addParent().
      virtual void addParent(const ClassElement<GUM_SCALAR> &elt);

      /// See gum::ClassElement::_addChild().
      virtual void addChild(const ClassElement<GUM_SCALAR> &elt);

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
      Attribute *getCastDescendant() const;

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
       * both super Type<GUM_SCALAR> (i.e. this->type().super().variable()) and the
       * cast descendant CPF (i.e. attr->cpf()).
       *
       * This can only be done if attr Type<GUM_SCALAR> is a direct subtype of this
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
      void setAsCastDescendant(Attribute *attr);

      /// @}
      protected:
      /// Copy constructor. Don't use it.
      Attribute(const Attribute &source);

      /// Copy operator. Don't use it.
      Attribute &operator=(const Attribute &from);

      private:
      // ========================================================================
      /// @name Private members
      // ========================================================================
      /// @{

      /// The random variable type of this attribute
      Type<GUM_SCALAR> *__type;

      /// A pointer on the Potential of this attribute
      Potential<GUM_SCALAR> *__cpf;

      /// Flag to know if we can delete type.
      bool __delete_type;

      /// @}
    };

    /**
     * @class FuncAttribute attribute.h <agrum/PRM/attribute.h>
     * @brief This class overload gum::prm::ClassElement::_addParent() and
     *        gum::prm::ClassElement::_addChild with empty methods.
     *
     * This class should be used when dealing with functions such as Noisy-Or which
     * require special method calls to add parents. See
     * gum::prm::PRMFactory::addAttribute(Attribute*) for more details.
     */
    template <typename GUM_SCALAR>
    class FuncAttribute : public Attribute<GUM_SCALAR> {
      public:
      /**
       * @brief Constructor used by gum::Class.
       * This will create an FuncAttribute with only one variable: type and with the
       *given
       * implementation.
       *
       * @param name The name of this FuncAttribute.
       * @param type The type of this FuncAttribute, it is copied.
       * @param impl The MultiDimImplementation used by the internal Potential of
       *this FuncAttribute.
       *             it will be deleted after the call of ~FuncAttribute.
       */
      FuncAttribute(const std::string &name, const Type<GUM_SCALAR> &type,
                    MultiDimImplementation<GUM_SCALAR> *impl =
                        new MultiDimArray<GUM_SCALAR>());

      /**
       * @brief Constructor used by gum::Instance.
       * This will create an FuncAttribute with a ready Potential, however it will
       *check the existence
       * of type in cpf and raise an exception if it is not found.
       *
       * @param type The type of this attribute, it will be deleted after a call to
       *~FuncAttribute.
       * @param name The name of this FuncAttribute.
       * @param cpf The Potential of this FuncAttribute, it will be deleted after the
       *call of
       *            ~FuncAttribute.
       * @param delete_type If true, the type is deleted with this instance.
       */
      FuncAttribute(const std::string &name, Type<GUM_SCALAR> *type,
                    Potential<GUM_SCALAR> *cpf, bool delete_type);

      /// Destructor.
      virtual ~FuncAttribute();

      /// See gum::ClassElement::_addParent().
      virtual void addParent(const ClassElement<GUM_SCALAR> &elt);

      /// See gum::ClassElement::_addChild().
      virtual void addChild(const ClassElement<GUM_SCALAR> &elt);

      private:
      /// Copy constructor. Don't use it.
      FuncAttribute(const FuncAttribute &source);

      /// Copy operator. Don't use it.
      FuncAttribute &operator=(const FuncAttribute &from);
    };

    extern template class Attribute<double>;
    extern template class FuncAttribute<double>;
  } /* namespace prm */
} // namespace gum

#include <agrum/PRM/elements/attribute.tcc>

#endif /* GUM_ATTRIBUTE_H */
