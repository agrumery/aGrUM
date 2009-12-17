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
 * @brief Headers of gum::SlotChain.
 *
 * @author Lionel TORTI
 */
// ============================================================================
#ifndef GUM_SLOT_CHAIN_H
#define GUM_SLOT_CHAIN_H
// ============================================================================
#include <agrum/core/sequence.h>
// ============================================================================
#include <agrum/prm/classElement.h>
#include <agrum/prm/attribute.h>
#include <agrum/prm/referenceSlot.h>
// ============================================================================
namespace gum {
// ============================================================================
class Class;
// ============================================================================
/**
 * @class SlotChain slotChain.h <agrum/prm/slotChain.h>
 * @brief SlotChain represent a path between two formal attributes.
 *
 * A formal attribute is either an Attribute or an Aggregate.
 *
 * A SlotChain delegates it's ClassElement's method to the formal attribute pointed
 * by the slot chain. However be careful to remember that the formal attribute used
 * is always at Class-level: in an Instance formal attributes with SlotChains
 * parents hold in their Potential pointers on the instantiated formal attributes
 * DiscreteVariable. The good pointers are found when the Model is created and
 * helps with the fact that MultiDim don't allow duplicated variables.
 *
 * @see Class Instance Attribute ReferenceSlot Aggregate
 */
// ==========================================================================
class SlotChain: public ClassElement
{
  public:
  // ========================================================================
  /// @name Constructors & destructor
  // ========================================================================
    /// @{

    /**
     * Default constructor.
     *
     * @param name The name of this SlotChain.
     * @param start The Class where this SlotChain starts.
     * @param end The Class where this SlotChain ends.
     * @param chain The chain of ClassElement in the SlotChain.
     * @param lastElt The element on which this SlotChain points.
     */
    SlotChain(const std::string& name, Class& start, Class& end,
              Sequence< ReferenceSlot* >& chain, ClassElement& lastElt);

    /// Destructor.
    virtual ~SlotChain();

    /// @}
  // ========================================================================
  /// @name Getters & setters
  // ========================================================================
    /// @{

    /// See gum::ClassElement::elt_type().
    virtual ClassElementType elt_type() const;

    /// This is similar to the following call: this->lastElt().type()
    virtual Type& type();

    /// This is similar to the following call: this->lastElt().type()
    virtual const Type& type() const;

    /// This is similar to the following call: this->lastElt().cpf()
    virtual Potential<prm_float>& cpf();

    /// This is similar to the following call: this->lastElt().cpf()
    virtual const Potential<prm_float>& cpf() const;

    /// Return true if this slot chain contains at least one multiple
    /// reference slot.
    bool isMultiple() const;

    /// Returns the T_DATA over which this slot chain starts.
    Class& start();

    /// Returns the T_DATA over which this slot chain starts.
    const Class& start() const;

    /// Returns the T_DATA over which this slot chain ends.
    Class& end();

    /// Returns the T_DATA over which this slot chain ends.
    const Class& end() const;

    /// Returns the last element of the slot chain, typically this is an
    /// gum::Attribute or a gum::Aggregate.
    ClassElement& lastElt();

    /// Returns the last element of the slot chain, typically this is an
    /// gum::Attribute or a gum::Aggregate.
    const ClassElement& lastElt() const;

    /// Return the sequence representing the chain of elements in this
    /// SlotChain.
    const Sequence<ReferenceSlot*>& chain() const;

    /// See gum::ClassElement::_addParent().
    virtual void addParent(const ClassElement& elt);

    /// See gum::ClassElement::_addChild().
    virtual void addChild(const ClassElement& elt);

    /// @}
  private:

    /// Copy constructor. Don't use it.
    SlotChain(const SlotChain& source);

    /// Copy operator. Don't use it.
    SlotChain& operator=(const SlotChain& source);

  // ========================================================================
  /// @name Private members of SlotChain.
  // ========================================================================
    /// @{

    /// The T_DATA over which the SlotChain starts.
    Class& __start;

    /// The T_DATA over which the SlotChain ends.
    Class& __end;

    /// The sequence of ClassElement composing the slot chain
    Sequence<ReferenceSlot*> __seq;

    /// The element pointed by the slot chain.
    ClassElement& __lastElt;

    /// Flag indicating if this slot chain is multiple or not.
    bool __isMultiple;

    /// @}
};
} // namespace gum
// ============================================================================
#ifndef GUM_NO_INLINE
#include <agrum/prm/slotChain.inl>
#endif // GUM_NO_INLINE
// ============================================================================
#endif /* GUM_SLOT_CHAIN_H */
// ============================================================================
