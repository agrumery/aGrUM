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

#ifndef GUM_CLASS_ELEMENT_H
#define GUM_CLASS_ELEMENT_H

#include <sstream>

#include <agrum/graphs/graphElements.h>

#include <agrum/multidim/potential.h>

#include <agrum/PRM/utils_prm.h>
#include <agrum/PRM/elements/PRMObject.h>

namespace gum {
  namespace prm {
    /**
     * @class ClassElement classElement.h <agrum/PRM/classElement.h>
     * @brief Abstract class representing an element of PRM class.
     *
     * All class elements are nodes in the class's DAG and a unique name in their
     * class.
     *
     * @ingroup prm_group
     */
    template <typename GUM_SCALAR> class ClassElement : public PRMObject {
      public:
      // ========================================================================
      /// @name Constructor & Destructor.
      // ========================================================================
      /// @{

      /**
       * @brief Default constructor of a ClassElement.
       *
       * The ClassElement will automatically add itself to c.
       *
       * @param name The name of this element, must be unique in it's class.
       * @throw DupplicateElement Raised if c contains already an element with
       *                          the same name.
       */
      ClassElement(const std::string &name);

      /**
       * Copy constructor.
       */
      ClassElement(const ClassElement &source);

      /**
       * Destructor of this class.
       */
      virtual ~ClassElement();

      /// @}
      // ========================================================================
      /// @name Built-in type
      // ========================================================================
      /// @{

      enum ClassElementType {
        prm_attribute,
        prm_aggregate,
        prm_refslot,
        prm_slotchain
      };

      static std::string enum2str(ClassElementType type) {
        switch (type) {
          case prm_attribute:
            return "prm_attribute";

          case prm_aggregate:
            return "prm_aggregate";

          case prm_refslot:
            return "prm_refslot";

          case prm_slotchain:
            return "prm_slotchain";

          default:
            return "unknown";
        }
      }

      /// Returns true if obj_ptr is of type ReferenceSlot.
      static INLINE bool isReferenceSlot(const ClassElement &elt) {
        return elt.elt_type() == prm_refslot;
      }

      /// Returns true if obj_ptr is of type Attribute.
      static INLINE bool isAttribute(const ClassElement &elt) {
        return elt.elt_type() == prm_attribute;
      }

      /// Return true if obj is of type Aggregate
      static INLINE bool isAggregate(const ClassElement &elt) {
        return elt.elt_type() == prm_aggregate;
      }

      /// Return true if obj is of type SlotChain
      static INLINE bool isSlotChain(const ClassElement &elt) {
        return elt.elt_type() == prm_slotchain;
      }

      /// @}
      // ========================================================================
      /// @name Getters & setters
      // ========================================================================
      /// @{

      /// Returns the NodeId of this element in it's class DAG.
      NodeId id() const;

      /// Used to assign the id of this element.
      virtual void setId(NodeId id);

      /**
       * @brief Add a parent to this element.
       *
       * This method is called by gum::Class when en parent is added
       * to this elememnt.
       */
      virtual void addParent(const ClassElement &elt) = 0;

      /**
       * @brief Add a child to this element.
       *
       * This methos is called by gum::Class when a child is added
       * to this element.
       */
      virtual void addChild(const ClassElement &elt) = 0;

      /// @see gum::PRMObject::obj_type().
      virtual typename PRMObject::PRMType obj_type() const;

      /// Return the type of class element this object is.
      virtual ClassElementType elt_type() const = 0;

      /// @}
      // ========================================================================
      /// @name Fast access to random variable's properties
      // ========================================================================
      /// @{

      /**
       * Return a reference over the gum::Type of this class element.
       * @throw OperationNotAllowed Raised if this class element doesn't have
       *                            any gum::Potential (like a gum::ReferenceSlot).
       */
      virtual Type<GUM_SCALAR> &type() = 0;

      /**
       * Return a constant reference over the gum::Type of this class element.
       * @throw OperationNotAllowed Raised if this class element doesn't have
       *                            any gum::Potential (like a gum::ReferenceSlot).
       */
      virtual const Type<GUM_SCALAR> &type() const = 0;

      /**
       * @brief Returns the safe name of this ClassElement, if any.
       *
       * This will only work if this ClassElement is an Attribute or an Aggregate.
       * @return Returns the safe name of this ClassElement.
       *
       * @throw NotFound& Raised if this ClassElement does not have any safe name.
       */
      const std::string &safeName() const;

      /**
       * @brief Returns the name of the cast descendant with Type t of this
       *        ClassElement.
       * @param t The type in which we want to cast this ClassElement.
       * @throw OperationNotAllowed If the cast is impossible.
       */
      virtual std::string cast(const Type<GUM_SCALAR> &t) const;

      /**
       * Return a reference over the gum::Potential of this class element.
       * @throw OperationNotAllowed Raised if this class element doesn't have
       *                            any gum::Potential (like a gum::ReferenceSlot).
       */
      virtual Potential<GUM_SCALAR> &cpf() = 0;

      /**
       * Return a constant reference over the gum::Potential of this class element.
       * @throw OperationNotAllowed Raised if the class element doesn't have
       *                            any gum::Potential (like a gum::ReferenceSlot).
       */
      virtual const Potential<GUM_SCALAR> &cpf() const = 0;

      /// @}
      protected:
      /// The safe name of this ClassElement.
      std::string _safeName;

      private:
      /// The node's id of this element
      NodeId __id;
    };

    extern template class ClassElement<double>;
  } /* namespace prm */
} // namespace gum

#include <agrum/PRM/elements/classElement.tcc>

#endif /* GUM_CLASS_ELEMENT_H */
