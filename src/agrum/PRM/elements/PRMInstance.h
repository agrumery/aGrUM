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
 * @brief Headers of gum::prm::PRMInstance<GUM_SCALAR>
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_INSTANCE_H
#define GUM_INSTANCE_H

#include <set>
#include <utility>


#include <agrum/PRM/elements/PRMClass.h>

namespace gum {
  namespace prm {


    /**
     * @class PRMInstance
     * @headerfile instance.h <agrum/PRM/elements/instance.h>
     *
     * @brief An PRMInstance is a Bayesian network fragment defined by a Class
     * and used in a PRMSystem.
     *
     * Before using an PRMInstance for inference YOU MUST call
     * PRMInstance::instantiateSlotChains() and PRMInstance::instantiate() on
     * it before, otherwise DiscreteVariable pointers will be inconsistent and
     * inference will be erroneous. Of course, this must be done once you have
     * set all reference in the current system.
     *
     * @see PRMClass PRM PRMClassElement
     *
     * @ingroup prm_group
     */
    template < typename GUM_SCALAR >
    class PRMInstance: public PRMObject {
      public:
      // ========================================================================
      /// @name Constructors & private operators.
      // ========================================================================
      /// @{

      /**
       * @brief Default constructor of an PRMInstance<GUM_SCALAR>.
       *
       * When you create an PRMInstance<GUM_SCALAR> all
       *gum::prm::PRMAttribute<GUM_SCALAR> in it's type are
       * added, except for gum::prm::PRMAggregate<GUM_SCALAR> and parameters
       *which
       *are
       * instantiated.
       *
       * However neither of their children or parents are updated (i.e.
       * DiscreteVariable pointers are inconsistent).
       */
      PRMInstance(const std::string& name, PRMClass< GUM_SCALAR >& type);

      /// Destructor.
      virtual ~PRMInstance();

      /// @}
      // ========================================================================
      /// @name Getters & setters.
      // ========================================================================
      /// @{

      /**
       * Returns the PRM type of this object.
       */
      virtual prm_type obj_type() const;

      /**
       * Returns the type of this instance
       */
      PRMClass< GUM_SCALAR >& type();

      /**
       * Returns the type of this instance
       */
      const PRMClass< GUM_SCALAR >& type() const;

      /**
       * Returns true if id matches an PRMAttribute<GUM_SCALAR> in this
       * PRMInstance<GUM_SCALAR>.
       */
      bool exists(NodeId id) const;

      /**
       * Returns true if name matches an PRMAttribute<GUM_SCALAR> in this
       * PRMInstance<GUM_SCALAR>.
       */
      bool exists(const std::string& name) const;

      /**
       * @brief Getter on an PRMAttribute<GUM_SCALAR> of this
       *PRMInstance<GUM_SCALAR>.
       *
       * Recall that PRMAggregate<GUM_SCALAR> are instantiated as
       *PRMAttribute<GUM_SCALAR>
       * when an PRMInstance<GUM_SCALAR> is created. Thus any random variable
       *contained
       * in this PRMInstance<GUM_SCALAR> is mapped to an
       *PRMAttribute<GUM_SCALAR>.
       *
       * @param id The PRMAttribute<GUM_SCALAR> id.
       * @return Returns a constant reference on an PRMAttribute<GUM_SCALAR>.
       *
       * @throw NotFound Raised if no PRMAttribute<GUM_SCALAR> matches name.
       */
      PRMAttribute< GUM_SCALAR >& get(NodeId id);

      /**
       * @brief Given a name returns the related PRMAttribute<GUM_SCALAR>.
       *
       * @param name The PRMAttribute<GUM_SCALAR>'s name.
       * @return Returns a reference over the PRMAttribute<GUM_SCALAR>.
       *
       * @throw NotFound Raised if there is no PRMAttribute<GUM_SCALAR> named n.
       */
      PRMAttribute< GUM_SCALAR >& get(const std::string& name);

      /**
       * @brief Getter on an PRMAttribute<GUM_SCALAR> of this
       *PRMInstance<GUM_SCALAR>.
       *
       * Recall that PRMAggregate<GUM_SCALAR> are instantiated has
       *PRMAttribute<GUM_SCALAR>
       * when an PRMInstance<GUM_SCALAR> is created. Thus any random variable
       *contained
       * in this PRMInstance<GUM_SCALAR> is mapped to an
       *PRMAttribute<GUM_SCALAR>.
       *
       * @param id The PRMAttribute<GUM_SCALAR> id.
       * @return Returns a constant reference on an PRMAttribute<GUM_SCALAR>.
       *
       * @throw NotFound Raised if no PRMAttribute<GUM_SCALAR> matches name.
       */
      const PRMAttribute< GUM_SCALAR >& get(NodeId id) const;

      /**
       * @brief Given a name returns the related PRMClassElement.
       *
       * @param name The PRMClassElement's name.
       * @return Returns a constant reference over the PRMClassElement.
       *
       * @throw NotFound Raised if there is no PRMClassElement named n.
       */
      const PRMAttribute< GUM_SCALAR >& get(const std::string& name) const;

      /**
       * Returns the number of attributes in this PRMInstance<GUM_SCALAR>.
       * @return the number of attributes in this PRMInstance<GUM_SCALAR>.
       */
      Size size() const;

      /// @}
      // ========================================================================
      /// @name Instantiation methods.
      // ========================================================================
      /// @{

      /**
       * @brief Instantiate all nodes which requires it.
       *
       * All attributes are now instantiated, which means they have
       * a different DiscreteVariable pointer. This methods however still
       * connects attributes of different instance together.
       *
       * @warning Be aware that this method will delete a large part of
       *          this instance attribute's CPF. Furthermore, the
       *          instantiated attribute's type will change (but will not be
       *          deleted).
       */
      void instantiate();

      /**
       * @brief Returns a mapping between DiscreteVariable used in this and the
       *        ones used in this PRMInstance<GUM_SCALAR>'s Class<GUM_SCALAR>.
       *
       * MultiDims require distinctive DiscreteVariable pointers, thus
       * interface attributes are always instantiated to prevent multiple
       * insertion of the same DiscreteVariable pointer in a MultiDim.
       *
       * @return Returns a bijection in which first elements are
       *DiscreteVariable
       *         in this->type() and the second are in this.
       */
      const Bijection< const DiscreteVariable*, const DiscreteVariable* >& bijection() const;

      /// @}
      // ========================================================================
      /// @name Reference handling.
      // ========================================================================
      /// @{

      /**
       * @brief Add an PRMInstance<GUM_SCALAR> to a given PRMReferenceSlot,
       *PRMSlotChain<GUM_SCALAR> or
       *        output node.
       *
       * Three type of nodes can be associated with an PRMInstance<GUM_SCALAR>.
       *When an PRMInstance<GUM_SCALAR>
       * is associated with a PRMReferenceSlot, it represents an arc in the
       * relational skeleton. For a PRMSlotChain<GUM_SCALAR>, it is a
       *placeholder
       *of referenced
       * PRMClassElement by this PRMInstance<GUM_SCALAR>. Finally, for an output
       *node
       *it indicates
       * that an PRMInstance<GUM_SCALAR> has an PRMClassElement referring it.
       *
       * @param id The NodeId of a PRMReferenceSlot of this
       *PRMInstance<GUM_SCALAR>.
       * @param instance The instance added as references by id in this
       *PRMInstance<GUM_SCALAR>.
       *
       * @throw WrongClassElement Raised if id is not a valid PRMClassElement.
       * @throw OutOfBounds Raised if no more PRMInstance<GUM_SCALAR> can be
       *added to id.
       * @throw PRMTypeError Raised if instance's type isn't compatible with the
       *                  reference's type.
       * @throw NotFound If there is no PRMClassElement given id.
       */
      void add(NodeId id, PRMInstance< GUM_SCALAR >& instance);

      /**
       * @brief Fast access to the first instance in a PRMReferenceSlot or
       *PRMSlotChain<GUM_SCALAR>.
       *
       * This is equivalent to **(this->getInstance(id).begin()) and should be
       *use
       * when dealing with non multiple PRMReferenceSlot or
       *PRMSlotChain<GUM_SCALAR>.
       *
       * @param id The NodeId of a PRMReferenceSlot or PRMSlotChain<GUM_SCALAR>
       *in
       *this PRMInstance<GUM_SCALAR>.
       *
       * @throw NotFound Raised if there is no PRMClassElement given id.
       * @throw UndefinedElement Raised if no PRMInstance<GUM_SCALAR> are
       *referenced by id.
       */
      const PRMInstance< GUM_SCALAR >& getInstance(NodeId id) const;

      /**
       * @brief Returns the Set of PRMInstance<GUM_SCALAR> referenced by id.
       *
       * @param id The NodeId of a PRMReferenceSlot or PRMSlotChain<GUM_SCALAR>
       *in
       *this.
       * @return Returns the Set of PRMInstance<GUM_SCALAR> referenced by id.
       *
       * @throw NotFound If there is no PRMClassElement given id.
       */
      const Set< PRMInstance< GUM_SCALAR >* >& getInstances(NodeId id) const;

      /**
       * Returns true if id has at least one referring PRMAttribute<GUM_SCALAR>.
       * @param id A NodeId.
       * @return returns true if is has at least one referring
       * PRMAttribute<GUM_SCALAR>.
       */
      bool hasRefAttr(NodeId id) const;

      /**
       * @brief Returns a vector of pairs of refering attributes of id.
       */
      std::vector< std::pair< PRMInstance< GUM_SCALAR >*, std::string > >& getRefAttr(NodeId id);

      /**
       * @brief Returns a vector of pairs of refering attributes of id.
       */
      const std::vector< std::pair< PRMInstance< GUM_SCALAR >*, std::string > >&
         getRefAttr(NodeId id) const;

      /// @}
      // ========================================================================
      /// @name Iterators
      // ========================================================================
      /// @{

      /// Alias to iterate over the gum::prm::PRMAttribute<GUM_SCALAR> in this
      /// PRMInstance<GUM_SCALAR>.
      using iterator = typename NodeProperty< PRMAttribute< GUM_SCALAR >* >::iterator;

      /// Returns an iterator at the begining of the list of
      /// gum::prm::PRMAttribute<GUM_SCALAR> in
      /// this PRMInstance<GUM_SCALAR>.
      iterator begin();

      /// Returns a reference over the iterator at the end of the list of
      /// gum::prm::PRMAttribute<GUM_SCALAR> in this PRMInstance<GUM_SCALAR>.
      const iterator& end();

      /// Alias to iterate over the gum::prm::PRMAttribute<GUM_SCALAR> in this
      /// PRMInstance<GUM_SCALAR>.
      using const_iterator = typename NodeProperty< PRMAttribute< GUM_SCALAR >* >::const_iterator;

      /// Returns an iterator at the beginning of the list of
      /// gum::prm::PRMAttribute<GUM_SCALAR> in
      /// this PRMInstance<GUM_SCALAR>.
      const_iterator begin() const;

      /// Returns a reference over the iterator at the end of the list of
      /// gum::prm::PRMAttribute<GUM_SCALAR> in this PRMInstance<GUM_SCALAR>.
      const const_iterator& end() const;

      /**
       * Nested class to iterate over PRMReferenceSlot and
       * PRMSlotChain<GUM_SCALAR>
       * instantiations.
       */
      class RefIterator {
        public:
        explicit RefIterator(Set< PRMInstance< GUM_SCALAR >* >& set);

        RefIterator(const RefIterator& from);

        ~RefIterator();

        RefIterator& operator=(const RefIterator& from);

        RefIterator& operator++();

        bool isEnd() const;

        bool operator!=(const RefIterator& from) const;

        bool operator==(const RefIterator& from) const;

        PRMInstance< GUM_SCALAR >& operator*() const;
        PRMInstance< GUM_SCALAR >* operator->() const;

        private:
        Set< PRMInstance< GUM_SCALAR >* >&                   _set_;
        typename Set< PRMInstance< GUM_SCALAR >* >::iterator _iter_;
      };

      /**
       * Returns an iterator at the beginning of the set of
       *PRMInstance<GUM_SCALAR>
       *associated
       * to a given gum::prm::PRMReferenceSlot or
       *gum::prm::PRMSlotChain<GUM_SCALAR>.
       *
       * @param id A gum::prm::PRMReferenceSlot or
       *gum::prm::PRMSlotChain<GUM_SCALAR>
       *in this
       *           PRMInstance<GUM_SCALAR> type.
       *
       * @throw NotFound Raised if no gum::prm::PRMClassElement in this
       *PRMInstance<GUM_SCALAR>
       *                 type matches id.
       * @throw WrongClassElement Raised if id is neither a PRMReferenceSlot or
       *                          PRMSlotChain<GUM_SCALAR>.
       */
      RefIterator begin(NodeId id);

      /**
       * Nested class to iterate over PRMReferenceSlot and
       * PRMSlotChain<GUM_SCALAR>
       * instantiations.
       */
      class RefConstIterator {
        public:
        explicit RefConstIterator(const Set< PRMInstance< GUM_SCALAR >* >& set);

        RefConstIterator(const RefConstIterator& from);

        ~RefConstIterator();

        RefConstIterator& operator=(const RefConstIterator& from);

        RefConstIterator& operator++();

        bool isEnd() const;

        bool operator!=(const RefConstIterator& from) const;

        bool operator==(const RefConstIterator& from) const;

        const PRMInstance< GUM_SCALAR >& operator*() const;
        const PRMInstance< GUM_SCALAR >* operator->() const;

        private:
        const Set< PRMInstance< GUM_SCALAR >* >&                   _set_;
        typename Set< PRMInstance< GUM_SCALAR >* >::const_iterator _iter_;
      };

      /**
       * Returns an iterator at the beginning of the set of
       *PRMInstance<GUM_SCALAR>
       *associated
       * to a given gum::prm::PRMReferenceSlot or
       *gum::prm::PRMSlotChain<GUM_SCALAR>.
       *
       * @param id A gum::prm::PRMReferenceSlot or
       *gum::prm::PRMSlotChain<GUM_SCALAR>
       *in this
       *           PRMInstance<GUM_SCALAR> type.
       *
       * @throw NotFound Raised if no gum::prm::PRMClassElement in this
       *PRMInstance<GUM_SCALAR>
       * type matches id.
       * @throw WrongClassElement Raised if id is neither a PRMReferenceSlot or
       * PRMSlotChain<GUM_SCALAR>.
       */
      RefConstIterator begin(NodeId id) const;

      using InvRefIterator = typename NodeProperty<
         std::vector< std::pair< PRMInstance< GUM_SCALAR >*, std::string > >* >::iterator;
      using InvRefConstIterator = typename NodeProperty<
         std::vector< std::pair< PRMInstance< GUM_SCALAR >*, std::string > >* >::const_iterator;

      InvRefIterator        beginInvRef();
      const InvRefIterator& endInvRef();

      InvRefConstIterator        beginInvRef() const;
      const InvRefConstIterator& endInvRef() const;

      /// @}
      private:
      /// Copy constructor.
      PRMInstance(const PRMInstance< GUM_SCALAR >& source);

      /// Copy operator. Don't use it.
      PRMInstance< GUM_SCALAR >& operator=(const PRMClass< GUM_SCALAR >& from);

      // ========================================================================
      /// @name Private instantiation methods.
      // ========================================================================
      /// @{

      /// Used at construction to instantiate aggregates.
      void _copyAggregates_(PRMAggregate< GUM_SCALAR >* source);

      /// Used at construction to instantiate attributes.
      /// @param source An attribute in  _type_.
      void _copyAttribute_(PRMAttribute< GUM_SCALAR >* source);

      /**
       * @brief Starts this instance instantiations.
       */
      void _doInstantiate_();

      /// @brief Retrieve all instances referred by sc.
      /// @param sc A slot chain of this instance's type.
      /// @throw NotFound Raised if a reference in sc is not instantiated.
      void _instantiateSlotChain_(PRMSlotChain< GUM_SCALAR >* sc);

      /// @brief Copy the content of an PRMAttribute<GUM_SCALAR> from its
      /// Class<GUM_SCALAR> counterpart.
      /// @param attr An PRMAttribute<GUM_SCALAR> of this
      /// PRMInstance<GUM_SCALAR>.
      /// @throw OperationNotAllowed If the MultiDimImplementation is of an
      /// unknown type.
      void _copyAttributeCPF_(PRMAttribute< GUM_SCALAR >* attr);

      /// @brief Add this as a referring instance over the attribute pointed by
      /// sc in i.
      /// @param sc A slot chain pointing over an attribute in i.
      /// @param i An instance holding an attribute pointed by sc.
      void _addReferingInstance_(PRMSlotChain< GUM_SCALAR >* sc, PRMInstance< GUM_SCALAR >* i);

      /// @}
      // ========================================================================
      /// @name Private members.
      // ========================================================================
      /// @{

      /// True if this instance has been instantiated
      bool _instantiated_;

      /// The type of this PRMInstance<GUM_SCALAR>.
      PRMClass< GUM_SCALAR >* _type_;

      /// The gum::prm::PRMAttribute<GUM_SCALAR> and
      /// gum::prm::PRMAggregate<GUM_SCALAR> of this PRMInstance<GUM_SCALAR>.
      NodeProperty< PRMAttribute< GUM_SCALAR >* > _nodeIdMap_;

      /// Mapping between the gum::prm::PRMReferenceSlot and
      /// gum::prm::PRMSlotChain<GUM_SCALAR> in
      ///  _type_ / and the PRMInstance<GUM_SCALAR> associated with it.
      NodeProperty< Set< PRMInstance< GUM_SCALAR >* >* > _referenceMap_;

      /// Code alias.
      using pair = std::pair< PRMInstance< GUM_SCALAR >*, std::string >;

      /// The set of pair (instance, attribute) referring an attribute of
      /// this instance.
      NodeProperty< std::vector< pair >* > _referingAttr_;

      /// A bijection used for MultiDim handling.
      Bijection< const DiscreteVariable*, const DiscreteVariable* > _bijection_;

      /// @}
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class PRMInstance< double >;
#endif


  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/elements/PRMInstance_tpl.h>

#endif /* GUM_INSTANCE_H */
