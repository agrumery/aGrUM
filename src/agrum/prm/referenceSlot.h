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
 * @author Lionel TORTI
 */
// ============================================================================
#ifndef GUM_REFERENCE_SLOT_H
#define GUM_REFERENCE_SLOT_H
// ============================================================================
#include <agrum/prm/classElement.h>
// ============================================================================
namespace gum {
namespace prm {
// ============================================================================
class Class;
// ============================================================================
/**
 * @class ReferenceSlot referenceSlot.h <agrum/prm/referenceSlot.h>
 * @brief Class used as a placeholder of properties on a slot.
 *
 * A reference slot is a relation between two classes in a PRM. A
 * ReferenceSlot is considered as an element of a class which
 * type is a class. It is the only class element wich does not represent
 * a random variable, which implies that any call to random variables related
 * methods will raise an OperationNotAllowed.
 *
 * Four type of information defines a ReferenceSlot: it's type, the class
 * in which it is defined, it's name in that class and finally if it represent
 * a multiple relationship.
 *
 * A ReferenceSlot is built by a gum::PRMFactory and is delted by it's
 * gum::Class.
 *
 * @see PRM PRMFactory Class ClassElement
 */
// ==========================================================================
class ReferenceSlot: public ClassElement {
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
    ReferenceSlot(const std::string& name, Class& type, bool isArray=false);

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
    virtual ClassElementType elt_type() const;

    /**
     * Returns the type of this slot, which is a Class (it is not the type
     * of PRMObject).
     */
    Class& slotType();

    /**
     * Returns the type of this slot, which is a Class (it is not the type
     * of PRMObject).
     */
    const Class& slotType() const;

    /**
     * Returns true if this reference slot is an array.
     */
    bool isArray() const;

    /// See gum::ClassElement::addParent().
    virtual void addParent(const ClassElement& elt);

    /// See gum::ClassElement::addChild().
    virtual void addChild(const ClassElement& elt);

    /// @}
  // ========================================================================
  /// @name Not allowed operations on a ReferenceSlot.
  // ========================================================================
    /// @{

    /// @brief Raise an OperationNotAllowed.
    /// See gum::ClassElement::type().
    virtual Type& type();

    /// @brief Raise an OperationNotAllowed.
    /// See gum::ClassElement::type().
    virtual const Type& type() const;

    /// @brief Raise an OperationNotAllowed.
    /// See gum::ClassElement::cpf().
    virtual Potential<prm_float>& cpf();

    /// @brief Raise an OperationNotAllowed.
    /// See gum::ClassElement::cpf().
    virtual const Potential<prm_float>& cpf() const;

    /// @}
  private:
  // ========================================================================
  /// @name Private methods
  // ========================================================================
    // @{

    /// Copy constructor. Don't use it.
    ReferenceSlot(const ReferenceSlot& source);

    /// Copy operator. Don't use it.
    ReferenceSlot& operator=(const ReferenceSlot& from);

    /// @}
  // ========================================================================
  /// @name Private members
  // ========================================================================
    // @{

    /// The type of this ReferenceSlot
    Class& __slotType;

    /// Flag indicating if this slot is an array
    bool __isArray;

    /// @}
};
// ============================================================================
} /* namespace prm */
} // namespace gum
// ============================================================================
#ifndef GUM_NO_INLINE
#include <agrum/prm/referenceSlot.inl>
#endif // GUM_NO_INLINE
// ============================================================================
#endif /* GUM_CLASS_ELEMENT_H */
// ============================================================================
