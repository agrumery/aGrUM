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
 * @brief Headers of gum::ClassElement.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_REFERENCE_SLOT_H
#define GUM_REFERENCE_SLOT_H

#include <agrum/PRM/elements/classElement.h>

namespace gum {
  namespace prm {

    /**
     * @class ReferenceSlot referenceSlot.h <agrum/PRM/referenceSlot.h>
     * @brief A ReferenceSlot represent a relation between two
     *        ClassElementContainer.
     *
     * A reference slot is a relation between two ClassElementContainer in a PRM. A
     * ReferenceSlot is considered as an element of a ClassElementContainer which
     * slot type is a ClassElementContainer.
     *
     * A ReferenceSlot is defined by it's slot type (i.e. it's range), it's
     * ClassElementContainer (it's domain), it's name and if it is a multiple
     * reference (the isArray() flag).
     *
     * @see PRM PRMFactory ClassElementContainer ClassElement
     * @ingroup prm_group
     */
    // ==========================================================================
    template <typename GUM_SCALAR>
    class ReferenceSlot : public ClassElement<GUM_SCALAR> {
      public:
      // ========================================================================
      /// @name Protected constructors & destructor.
      // ========================================================================
      /// @{

      /**
       * Default constructor
       *
       * @param name The name of this element.
       * @param type The type of this reference slot.
       * @param isArray Determine if this reference slot is multiple or not.
       */
      ReferenceSlot(const std::string &name, ClassElementContainer<GUM_SCALAR> &type,
                    bool isArray = false);

      /**
       * Destructor.
       */
      virtual ~ReferenceSlot();

      /// @}
      // ========================================================================
      /// @name Getters & setters
      // ========================================================================
      /// @{

      /**
       * Implementation of the pure virtual method of PRMObject.
       */
      virtual typename ClassElement<GUM_SCALAR>::ClassElementType elt_type() const;

      /**
       * Returns the type of this slot, which is a ClassElementContainer
       * (it is not the type of PRMObject).
       */
      ClassElementContainer<GUM_SCALAR> &slotType();

      /**
       * Returns the type of this slot, which is a ClassElementContainer
       * (it is not the type of PRMObject).
       */
      const ClassElementContainer<GUM_SCALAR> &slotType() const;

      /**
       * Returns true if this reference slot is an array.
       */
      bool isArray() const;

      /// See gum::ClassElement::addParent().
      virtual void addParent(const ClassElement<GUM_SCALAR> &elt);

      /// See gum::ClassElement::addChild().
      virtual void addChild(const ClassElement<GUM_SCALAR> &elt);

      /// @}
      // ========================================================================
      /// @name Not allowed operations on a ReferenceSlot.
      // ========================================================================
      /// @{

      /// @brief Raise an OperationNotAllowed.
      /// See gum::ClassElement::type().
      virtual Type<GUM_SCALAR> &type();

      /// @brief Raise an OperationNotAllowed.
      /// See gum::ClassElement::type().
      virtual const Type<GUM_SCALAR> &type() const;

      /// @brief Raise an OperationNotAllowed.
      /// See gum::ClassElement::cpf().
      virtual Potential<GUM_SCALAR> &cpf();

      /// @brief Raise an OperationNotAllowed.
      /// See gum::ClassElement::cpf().
      virtual const Potential<GUM_SCALAR> &cpf() const;

      /// @}
      private:
      // ========================================================================
      /// @name Private methods
      // ========================================================================
      // @{

      /// Copy constructor. Don't use it.
      ReferenceSlot(const ReferenceSlot &source);

      /// Copy operator. Don't use it.
      ReferenceSlot &operator=(const ReferenceSlot &from);

      /// @}
      // ========================================================================
      /// @name Private members
      // ========================================================================
      // @{

      /// The type of this ReferenceSlot.
      ClassElementContainer<GUM_SCALAR> &__slotType;

      /// Flag indicating if this slot is an array.
      bool __isArray;

      /// @}
    };

    extern template class ReferenceSlot<double>;
  } /* namespace prm */
} // namespace gum

#include <agrum/PRM/elements/referenceSlot.tcc>

#endif /* GUM_CLASS_ELEMENT_H */
