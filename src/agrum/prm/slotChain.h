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
#include <agrum/multidim/multiDimBijArray.h>
#include <agrum/multidim/multiDimNoisyORNet.h>
#include <agrum/multidim/multiDimNoisyORCompound.h>
// ============================================================================
#include <agrum/prm/classElement.h>
#include <agrum/prm/attribute.h>
#include <agrum/prm/aggregate.h>
#include <agrum/prm/referenceSlot.h>
// ============================================================================
namespace gum {
namespace prm {
// ============================================================================
class Class;
// ============================================================================

/**
 * @class SlotChain slotChain.h <agrum/prm/slotChain.h>
 *
 * @brief A SlotChain represents a sequence of gum::prm::ClassElement where
 *        the n-1 first gum::prm::ClassElement are gum::prm::ReferenceSlot and
 *        the last gum::prm::ClassElement an gum::prm::Attribute or an
 *        gum::prm::Aggregate.
 *
 * A SlotChain behaves as an gum::prm::Attribute or an gum::prm::Aggregate
 * (depending the gum::prm::ClassElement type of it's last element) regarding
 * the following methods: gum::prm::ClassElement::type() and
 * gum::prm::ClassElement::cpf().
 *
 */
// ==========================================================================
class SlotChain: public ClassElement
{
  public:
  // ========================================================================
  /// @name Constructors & destructor
  // ========================================================================
    /// @{

    /** @brief Default constructor.
     *
     * Chain's n-1 first elements must be ReferenceSlot and the last element
     * must either be an Attribute or an Aggregate.
     *
     * Warning: the last element in chain is copied !
     *
     * @param name The name of this SlotChain.
     * @param chain The chain of gum::prm::ClassElement in this SlotChain.
     *
     * @throw OperationNotAllowed Raised if the chain contains less than two
     *        ClassElement.
     * @throw WrongClassElement Raised contains invalid ClassElement.
     */
    SlotChain(const std::string& name, const Sequence< ClassElement* >& chain);

    /** @brief Tweak constructor.
     *
     * Chain's n-1 first elements must be ReferenceSlot and the last element
     * must either be an Attribute or an Aggregate.
     *
     * Warning: the last element in chain is copied !
     *
     * @param name The name of this SlotChain.
     * @param chain The chain given to this SlotChain, it is deleted
     *              when SlotChain::~SlotChain() is called.
     *
     * @throw OperationNotAllowed Raised if the chain contains less than two
     *        ClassElement.
     * @throw WrongClassElement Raised contains invalid ClassElement.
     */
    SlotChain(Sequence<ClassElement*>* chain, const std::string& name);

    /**
     * Copy constructor.
     *
     * This creates a copy of the slot chain.
     */
    SlotChain(const SlotChain& source);

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

    /// Returns the ClassElementContainer over which this slot chain ends.
    ClassElementContainer& end();

    /// Returns the ClassElementContainer over which this slot chain ends.
    const ClassElementContainer& end() const;

    /// Returns the last element of the slot chain, typically this is an
    /// gum::Attribute or a gum::Aggregate.
    ClassElement& lastElt();

    /// Returns the last element of the slot chain, typically this is an
    /// gum::Attribute or a gum::Aggregate.
    const ClassElement& lastElt() const;

    /// Return the sequence representing the chain of elements in this
    /// SlotChain.
    Sequence<ClassElement*>& chain();

    /// Return the sequence representing the chain of elements in this
    /// SlotChain.
    const Sequence<ClassElement*>& chain() const;

    /// See gum::ClassElement::_addParent().
    virtual void addParent(const ClassElement& elt);

    /// See gum::ClassElement::_addChild().
    virtual void addChild(const ClassElement& elt);

    virtual std::string cast(const Type& t) {
      if (lastElt().type().isSubTypeOf(t)) {
        std::stringstream sBuff;
        for (Size i = 0; i < chain().size() - 1; ++i) {
          sBuff << chain().atPos(i) << ".";
        }
        sBuff << ClassElement::LEFT_CAST() << t.name() << ClassElement::RIGHT_CAST();
        sBuff << lastElt().name();
        return sBuff.str();
      } else {
        GUM_ERROR(OperationNotAllowed, "no possible safe name for this ClassElement");
      }
    }

    /// @}
  private:

    /// Copy operator. Don't use it.
    SlotChain& operator=(const SlotChain& source);

  // ========================================================================
  /// @name Private members of SlotChain.
  // ========================================================================
    /// @{

    /// The sequence of ClassElement composing the slot chain
    Sequence<ClassElement*>* __chain;

    /// Flag indicating if this slot chain is multiple or not.
    bool __isMultiple;

    /// Copy the last element, this prevents unwanted DuplicateElement exceptions.
    void __copyLastElt();

    /// @}
};
} /* namespace prm */
} // namespace gum
// ============================================================================
#ifndef GUM_NO_INLINE
#include <agrum/prm/slotChain.inl>
#endif // GUM_NO_INLINE
// ============================================================================
#endif /* GUM_SLOT_CHAIN_H */
// ============================================================================
