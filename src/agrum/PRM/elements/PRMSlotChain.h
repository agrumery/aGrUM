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
 * @brief Headers of gum::PRMSlotChain.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_SLOT_CHAIN_H
#define GUM_SLOT_CHAIN_H

#include <string>


#include <agrum/PRM/elements/PRMAggregate.h>
#include <agrum/PRM/elements/PRMReferenceSlot.h>

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    class PRMClass;
    template < typename GUM_SCALAR >
    class PRMAttribute;

    /**
     * @class PRMSlotChain
     * @headerfile slotChain.h <agrum/PRM/slotChain.h>
     *
     * @brief A PRMSlotChain represents a sequence of
     *gum::prm::PRMClassElement<GUM_SCALAR>
     *where
     *        the n-1 first gum::prm::PRMClassElement<GUM_SCALAR> are
     *gum::prm::PRMReferenceSlot and
     *        the last gum::prm::PRMClassElement<GUM_SCALAR> an
     *gum::prm::PRMAttribute
     *or
     *an
     *        gum::prm::PRMAggregate.
     *
     * A PRMSlotChain behaves as an gum::prm::PRMAttribute or an
     *gum::prm::PRMAggregate
     * (depending the gum::prm::PRMClassElement<GUM_SCALAR> type of it's last
     *element)
     *regarding
     * the following methods: gum::prm::PRMClassElement<GUM_SCALAR>::type() and
     * gum::prm::PRMClassElement<GUM_SCALAR>::cpf().
     *
     */
    // ==========================================================================
    template < typename GUM_SCALAR >
    class PRMSlotChain: public PRMClassElement< GUM_SCALAR > {
      public:
      // ========================================================================
      /// @name Constructors & destructor
      // ========================================================================
      /// @{

      /** @brief Default constructor.
       *
       * Chain's n-1 first elements must be PRMReferenceSlot and the last
       *element
       * must either be an PRMAttribute or an PRMAggregate.
       *
       * Warning: the last element in chain is copied !
       *
       * @param name The name of this PRMSlotChain.
       * @param chain The chain of gum::prm::PRMClassElement<GUM_SCALAR> in this
       *PRMSlotChain.
       *
       * @throw OperationNotAllowed Raised if the chain contains less than two
       *        PRMClassElement<GUM_SCALAR>.
       * @throw WrongClassElement<GUM_SCALAR> Raised contains invalid
       *PRMClassElement<GUM_SCALAR>.
       */
      PRMSlotChain(const std::string&                                name,
                   const Sequence< PRMClassElement< GUM_SCALAR >* >& chain);

      /** @brief Tweak constructor.
       *
       * Chain's n-1 first elements must be PRMReferenceSlot and the last
       *element
       * must either be an PRMAttribute or an PRMAggregate.
       *
       * Warning: the last element in chain is copied !
       *
       * @param name The name of this PRMSlotChain.
       * @param chain The chain given to this PRMSlotChain, it is deleted
       *              when PRMSlotChain::~PRMSlotChain() is called.
       *
       * @throw OperationNotAllowed Raised if the chain contains less than two
       *        PRMClassElement<GUM_SCALAR>.
       * @throw WrongClassElement<GUM_SCALAR> Raised contains invalid
       *PRMClassElement<GUM_SCALAR>.
       */
      PRMSlotChain(Sequence< PRMClassElement< GUM_SCALAR >* >* chain, const std::string& name);

      /**
       * Copy constructor.
       *
       * This creates a copy of the slot chain.
       */
      PRMSlotChain(const PRMSlotChain< GUM_SCALAR >& source);

      /// Destructor.
      virtual ~PRMSlotChain();

      /// @}
      // ========================================================================
      /// @name Getters & setters
      // ========================================================================
      /// @{

      /// See gum::PRMClassElement<GUM_SCALAR>::elt_type().
      virtual typename PRMClassElement< GUM_SCALAR >::ClassElementType elt_type() const;

      /// This is similar to the following call: this->lastElt().type()
      virtual PRMType& type();

      /// This is similar to the following call: this->lastElt().type()
      virtual const PRMType& type() const;

      // /// This is similar to the following call: this->lastElt().cpf()
      // virtual Potential<GUM_SCALAR>& cpf();

      /// This is similar to the following call: this->lastElt().cpf()
      virtual const Potential< GUM_SCALAR >& cpf() const;

      /// Return true if this slot chain contains at least one multiple
      /// reference slot.
      bool isMultiple() const;

      /// Returns the PRMClassElement<GUM_SCALAR>Container over which this slot
      /// chain ends.
      PRMClassElementContainer< GUM_SCALAR >& end();

      /// Returns the PRMClassElement<GUM_SCALAR>Container over which this slot
      /// chain ends.
      const PRMClassElementContainer< GUM_SCALAR >& end() const;

      /// Returns the last element of the slot chain, typically this is an
      /// gum::PRMAttribute or a gum::PRMAggregate.
      PRMClassElement< GUM_SCALAR >& lastElt();

      /// Returns the last element of the slot chain, typically this is an
      /// gum::PRMAttribute or a gum::PRMAggregate.
      const PRMClassElement< GUM_SCALAR >& lastElt() const;

      /// Return the sequence representing the chain of elements in this
      /// PRMSlotChain.
      Sequence< PRMClassElement< GUM_SCALAR >* >& chain();

      /// Return the sequence representing the chain of elements in this
      /// PRMSlotChain.
      const Sequence< PRMClassElement< GUM_SCALAR >* >& chain() const;

      /// See gum::PRMClassElement<GUM_SCALAR>::addParent_().
      virtual void addParent(const PRMClassElement< GUM_SCALAR >& elt);

      /// See gum::PRMClassElement<GUM_SCALAR>::addChild_().
      virtual void addChild(const PRMClassElement< GUM_SCALAR >& elt);

      /// @brief Raise a gum::OperationNotAllowed exception
      /// See gum::PRMClassElement<GUM_SCALAR>::getCastDescendant()
      virtual PRMAttribute< GUM_SCALAR >* getCastDescendant() const;
      /// @}
      private:
      /// Copy operator. Don't use it.
      PRMSlotChain& operator=(const PRMSlotChain& source);

      // ========================================================================
      /// @name Private members of PRMSlotChain.
      // ========================================================================
      /// @{

      /// The sequence of PRMClassElement<GUM_SCALAR> composing the slot chain
      Sequence< PRMClassElement< GUM_SCALAR >* >* _chain_;

      /// Flag indicating if this slot chain is multiple or not.
      bool _isMultiple_;

      /// Copy the last element, this prevents unwanted DuplicateElement
      /// exceptions.
      void _copyLastElt_();

      /// @}
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class PRMSlotChain< double >;
#endif


  } /* namespace prm */
}   // namespace gum

#include <agrum/PRM/elements/PRMSlotChain_tpl.h>

#endif /* GUM_SLOT_CHAIN_H */
