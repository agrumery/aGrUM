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
 * @brief Headers of gum::PRMClassElement.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_CLASS_ELEMENT_H
#define GUM_CLASS_ELEMENT_H

#include <sstream>

#include <agrum/agrum.h>


#include <agrum/PRM/utils_prm.h>

namespace gum {
  namespace prm {
    template < typename GUM_SCALAR >
    class PRMAttribute;

    /**
     * @class PRMClassElement
     * @headerfile classElement.h <agrum/PRM/classElement.h>
     * @brief Abstract class representing an element of PRM class.
     *
     * All class elements are nodes in the class's DAG and a unique name in
     *their
     * class.
     *
     * @ingroup prm_group
     */
    template < typename GUM_SCALAR >
    class PRMClassElement: public PRMObject {
      public:
      // ========================================================================
      /// @name Constructor & Destructor.
      // ========================================================================
      /// @{

      /**
       * @brief Default constructor of a PRMClassElement.
       *
       * The PRMClassElement will automatically add itself to c.
       *
       * @param name The name of this element, must be unique in it's class.
       * @throw DupplicateElement Raised if c contains already an element with
       *                          the same name.
       */
      explicit PRMClassElement(const std::string& name);

      /**
       * Copy constructor.
       */
      PRMClassElement(const PRMClassElement< GUM_SCALAR >& source);

      /**
       * Destructor of this class.
       */
      virtual ~PRMClassElement();

      /// @}
      // ========================================================================
      /// @name Built-in type
      // ========================================================================
      /// @{

      enum ClassElementType {
        prm_attribute,
        prm_aggregate,
        prm_refslot,
        prm_slotchain,
        prm_parameter
      };

      static std::string enum2str(ClassElementType type) {
        switch (type) {
          case prm_attribute: return "prm_attribute";

          case prm_aggregate: return "prm_aggregate";

          case prm_refslot: return "prm_refslot";

          case prm_slotchain: return "prm_slotchain";

          case prm_parameter: return "prm_parameter";

          default: return "unknown";
        }
      }

      /// Returns true if obj_ptr is of type PRMReferenceSlot.
      static INLINE bool isReferenceSlot(const PRMClassElement< GUM_SCALAR >& elt) {
        return elt.elt_type() == prm_refslot;
      }

      /// Returns true if obj_ptr is of type PRMAttribute.
      static INLINE bool isAttribute(const PRMClassElement< GUM_SCALAR >& elt) {
        return elt.elt_type() == prm_attribute;
      }

      /// Return true if obj is of type PRMAggregate
      static INLINE bool isAggregate(const PRMClassElement< GUM_SCALAR >& elt) {
        return elt.elt_type() == prm_aggregate;
      }

      /// Return true if obj is of type PRMSlotChain
      static INLINE bool isSlotChain(const PRMClassElement< GUM_SCALAR >& elt) {
        return elt.elt_type() == prm_slotchain;
      }

      /// Return true if obj is of type PRMParameter
      static INLINE bool isParameter(const PRMClassElement< GUM_SCALAR >& elt) {
        return elt.elt_type() == prm_parameter;
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
      virtual void addParent(const PRMClassElement< GUM_SCALAR >& elt) = 0;

      /**
       * @brief Add a child to this element.
       *
       * This methos is called by gum::Class when a child is added
       * to this element.
       */
      virtual void addChild(const PRMClassElement< GUM_SCALAR >& elt) = 0;

      /// @see gum::PRMObject::obj_type().
      virtual typename PRMObject::prm_type obj_type() const;

      /// Return the type of class element this object is.
      virtual ClassElementType elt_type() const = 0;

      /// @}
      // ========================================================================
      /// @name Fast access to random variable's properties
      // ========================================================================
      /// @{

      /**
       * Return a reference over the gum::PRMType of this class element.
       * @throw OperationNotAllowed Raised if this class element doesn't have
       *                            any gum::Potential (like a
       * gum::PRMReferenceSlot).
       */
      virtual PRMType& type() = 0;

      /**
       * Return a constant reference over the gum::PRMType of this class
       * element.
       * @throw OperationNotAllowed Raised if this class element doesn't have
       *                            any gum::Potential (like a
       * gum::PRMReferenceSlot).
       */
      virtual const PRMType& type() const = 0;

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
       * @brief Returns the safe name of this PRMClassElement, if any.
       *
       * This will only work if this PRMClassElement is an PRMAttribute or an
       *PRMAggregate.
       * @return Returns the safe name of this PRMClassElement.
       *
       * @throw NotFound& Raised if this PRMClassElement does not have any safe
       *name.
       */
      const std::string& safeName() const;

      /**
       * @brief Returns the name of the cast descendant with PRMType t of this
       *        PRMClassElement.
       * @param t The type in which we want to cast this PRMClassElement.
       * @throw OperationNotAllowed If the cast is impossible.
       */
      virtual std::string cast(const PRMType& t) const;

      // /**
      //  * Return a reference over the gum::Potential of this class element.
      //  * @throw OperationNotAllowed Raised if this class element doesn't have
      //  *                            any gum::Potential (like a
      //  gum::PRMReferenceSlot).
      //  */
      // virtual Potential<GUM_SCALAR>& cpf() = 0;

      /**
       * Return a constant reference over the gum::Potential of this class
       * element.
       * @throw OperationNotAllowed Raised if the class element doesn't have
       *                            any gum::Potential (like a
       * gum::PRMReferenceSlot).
       */
      virtual const Potential< GUM_SCALAR >& cpf() const = 0;

      /// @}
      protected:
      /// The safe name of this PRMClassElement.
      std::string safeName_;

      private:
      /// The node's id of this element
      NodeId _id_;
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class PRMClassElement< double >;
#endif

  } /* namespace prm */
}   // namespace gum

#include <agrum/PRM/elements/PRMClassElement_tpl.h>

#endif /* GUM_CLASS_ELEMENT_H */
