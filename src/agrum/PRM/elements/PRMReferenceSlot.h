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

#ifndef GUM_REFERENCE_SLOT_H
#define GUM_REFERENCE_SLOT_H

#include <agrum/PRM/elements/PRMClassElement.h>
#include <agrum/PRM/elements/PRMClassElementContainer.h>

namespace gum {
  namespace prm {

    /**
     * @class PRMReferenceSlot
     * @headerfile referenceSlot.h <agrum/PRM/referenceSlot.h>
     * @brief A PRMReferenceSlot represent a relation between two
     *        PRMClassElementContainer.
     *
     * A reference slot is a relation between two PRMClassElementContainer in a
     *PRM. A
     * PRMReferenceSlot is considered as an element of a
     *PRMClassElementContainer
     *which
     * slot type is a PRMClassElementContainer.
     *
     * A PRMReferenceSlot is defined by it's slot type (i.e. it's range), it's
     * PRMClassElementContainer (it's domain), it's name and if it is a multiple
     * reference (the isArray() flag).
     *
     * @see PRM PRMFactory PRMClassElementContainer PRMClassElement
     * @ingroup prm_group
     */
    // ==========================================================================
    template < typename GUM_SCALAR >
    class PRMReferenceSlot: public PRMClassElement< GUM_SCALAR > {
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
      PRMReferenceSlot(const std::string&                      name,
                       PRMClassElementContainer< GUM_SCALAR >& type,
                       bool                                    isArray = false);

      /**
       * Destructor.
       */
      virtual ~PRMReferenceSlot();

      /// @}
      // ========================================================================
      /// @name Getters & setters
      // ========================================================================
      /// @{

      /**
       * Implementation of the pure virtual method of PRMObject.
       */
      virtual typename PRMClassElement< GUM_SCALAR >::ClassElementType elt_type() const;

      /**
       * Returns the type of this slot, which is a PRMClassElementContainer
       * (it is not the type of PRMObject).
       */
      PRMClassElementContainer< GUM_SCALAR >& slotType();

      /**
       * Returns the type of this slot, which is a PRMClassElementContainer
       * (it is not the type of PRMObject).
       */
      const PRMClassElementContainer< GUM_SCALAR >& slotType() const;

      /**
       * Returns true if this reference slot is an array.
       */
      bool isArray() const;

      /// See gum::PRMClassElement::addParent().
      virtual void addParent(const PRMClassElement< GUM_SCALAR >& elt);

      /// See gum::PRMClassElement::addChild().
      virtual void addChild(const PRMClassElement< GUM_SCALAR >& elt);

      /// @}
      // ========================================================================
      /// @name Not allowed operations on a PRMReferenceSlot.
      // ========================================================================
      /// @{

      /// @brief Raise an OperationNotAllowed.
      /// See gum::PRMClassElement::type().
      virtual PRMType& type();

      /// @brief Raise an OperationNotAllowed.
      /// See gum::PRMClassElement::type().
      virtual const PRMType& type() const;

      /// @brief Raise an OperationNotAllowed.
      /// See gum::PRMClassElement::cpf().
      virtual Potential< GUM_SCALAR >& cpf();

      /// @brief Raise an OperationNotAllowed.
      /// See gum::PRMClassElement::cpf().
      virtual const Potential< GUM_SCALAR >& cpf() const;

      /// @brief Raise an OperationNotAllowed.
      /// See gum::PRMClassElement::getCastDescendant().
      virtual PRMAttribute< GUM_SCALAR >* getCastDescendant() const;

      /// @}
      private:
      // ========================================================================
      /// @name Private methods
      // ========================================================================
      // @{

      /// Copy constructor. Don't use it.
      PRMReferenceSlot(const PRMReferenceSlot< GUM_SCALAR >& source);

      /// Copy operator. Don't use it.
      PRMReferenceSlot& operator=(const PRMReferenceSlot< GUM_SCALAR >& from);

      /// @}
      // ========================================================================
      /// @name Private members
      // ========================================================================
      // @{

      /// The type of this PRMReferenceSlot.
      PRMClassElementContainer< GUM_SCALAR >& _slotType_;

      /// Flag indicating if this slot is an array.
      bool _isArray_;

      /// @}
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class PRMReferenceSlot< double >;
#endif


  } /* namespace prm */
}   // namespace gum

#include <agrum/PRM/elements/PRMReferenceSlot_tpl.h>

#endif /* GUM_CLASS_ELEMENT_H */
