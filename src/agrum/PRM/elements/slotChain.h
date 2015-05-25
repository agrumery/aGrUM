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
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_SLOT_CHAIN_H
#define GUM_SLOT_CHAIN_H

#include <agrum/core/sequence.h>

#include <agrum/multidim/multiDimBijArray.h>
#include <agrum/multidim/ICIModels/multiDimNoisyORNet.h>
#include <agrum/multidim/ICIModels/multiDimNoisyORCompound.h>

#include <agrum/PRM/elements/classElement.h>

namespace gum {
  namespace prm {

    template <typename GUM_SCALAR> class Class;
    template <typename GUM_SCALAR> class Attribute;

    /**
     * @class SlotChain slotChain.h <agrum/PRM/slotChain.h>
     *
     * @brief A SlotChain represents a sequence of gum::prm::ClassElement<GUM_SCALAR>
     *where
     *        the n-1 first gum::prm::ClassElement<GUM_SCALAR> are
     *gum::prm::ReferenceSlot and
     *        the last gum::prm::ClassElement<GUM_SCALAR> an gum::prm::Attribute or
     *an
     *        gum::prm::Aggregate.
     *
     * A SlotChain behaves as an gum::prm::Attribute or an gum::prm::Aggregate
     * (depending the gum::prm::ClassElement<GUM_SCALAR> type of it's last element)
     *regarding
     * the following methods: gum::prm::ClassElement<GUM_SCALAR>::type() and
     * gum::prm::ClassElement<GUM_SCALAR>::cpf().
     *
     */
    // ==========================================================================
    template <typename GUM_SCALAR>
    class SlotChain : public ClassElement<GUM_SCALAR> {
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
       * @param chain The chain of gum::prm::ClassElement<GUM_SCALAR> in this
       *SlotChain.
       *
       * @throw OperationNotAllowed Raised if the chain contains less than two
       *        ClassElement<GUM_SCALAR>.
       * @throw WrongClassElement<GUM_SCALAR> Raised contains invalid
       *ClassElement<GUM_SCALAR>.
       */
      SlotChain(const std::string &name,
                const Sequence<ClassElement<GUM_SCALAR> *> &chain);

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
       *        ClassElement<GUM_SCALAR>.
       * @throw WrongClassElement<GUM_SCALAR> Raised contains invalid
       *ClassElement<GUM_SCALAR>.
       */
      SlotChain(Sequence<ClassElement<GUM_SCALAR> *> *chain,
                const std::string &name);

      /**
       * Copy constructor.
       *
       * This creates a copy of the slot chain.
       */
      SlotChain(const SlotChain &source);

      /// Destructor.
      virtual ~SlotChain();

      /// @}
      // ========================================================================
      /// @name Getters & setters
      // ========================================================================
      /// @{

      /// See gum::ClassElement<GUM_SCALAR>::elt_type().
      virtual typename ClassElement<GUM_SCALAR>::ClassElementType elt_type() const;

      /// This is similar to the following call: this->lastElt().type()
      virtual Type<GUM_SCALAR> &type();

      /// This is similar to the following call: this->lastElt().type()
      virtual const Type<GUM_SCALAR> &type() const;

      /// This is similar to the following call: this->lastElt().cpf()
      virtual Potential<GUM_SCALAR> &cpf();

      /// This is similar to the following call: this->lastElt().cpf()
      virtual const Potential<GUM_SCALAR> &cpf() const;

      /// Return true if this slot chain contains at least one multiple
      /// reference slot.
      bool isMultiple() const;

      /// Returns the ClassElement<GUM_SCALAR>Container over which this slot chain
      /// ends.
      ClassElementContainer<GUM_SCALAR> &end();

      /// Returns the ClassElement<GUM_SCALAR>Container over which this slot chain
      /// ends.
      const ClassElementContainer<GUM_SCALAR> &end() const;

      /// Returns the last element of the slot chain, typically this is an
      /// gum::Attribute or a gum::Aggregate.
      ClassElement<GUM_SCALAR> &lastElt();

      /// Returns the last element of the slot chain, typically this is an
      /// gum::Attribute or a gum::Aggregate.
      const ClassElement<GUM_SCALAR> &lastElt() const;

      /// Return the sequence representing the chain of elements in this
      /// SlotChain.
      Sequence<ClassElement<GUM_SCALAR> *> &chain();

      /// Return the sequence representing the chain of elements in this
      /// SlotChain.
      const Sequence<ClassElement<GUM_SCALAR> *> &chain() const;

      /// See gum::ClassElement<GUM_SCALAR>::_addParent().
      virtual void addParent(const ClassElement<GUM_SCALAR> &elt);

      /// See gum::ClassElement<GUM_SCALAR>::_addChild().
      virtual void addChild(const ClassElement<GUM_SCALAR> &elt);

      virtual std::string cast(const Type<GUM_SCALAR> &t) const {
        if (lastElt().type().isSubTypeOf(t)) {
          std::stringstream sBuff;

          for (Size i = 0; i < chain().size() - 1; ++i) {
            sBuff << chain().atPos(i) << ".";
          }

          sBuff << PRMObject::LEFT_CAST() << t.name() << PRMObject::RIGHT_CAST();
          sBuff << lastElt().name();
          return sBuff.str();
        } else {
          GUM_ERROR(OperationNotAllowed,
                    "no possible safe name for this ClassElement<GUM_SCALAR>");
        }
      }

      /// @}
      private:
      /// Copy operator. Don't use it.
      SlotChain &operator=(const SlotChain &source);

      // ========================================================================
      /// @name Private members of SlotChain.
      // ========================================================================
      /// @{

      /// The sequence of ClassElement<GUM_SCALAR> composing the slot chain
      Sequence<ClassElement<GUM_SCALAR> *> *__chain;

      /// Flag indicating if this slot chain is multiple or not.
      bool __isMultiple;

      /// Copy the last element, this prevents unwanted DuplicateElement exceptions.
      void __copyLastElt();

      /// @}
    };

    extern template class SlotChain<double>;
  } /* namespace prm */
} // namespace gum

#include <agrum/PRM/elements/slotChain.tcc>

#endif /* GUM_SLOT_CHAIN_H */
