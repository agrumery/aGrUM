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
 * @brief Headers of gum::prm::Instance<GUM_SCALAR>
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_INSTANCE_H
#define GUM_INSTANCE_H

#include <utility>
#include <set>

#include <agrum/core/bijection.h>

#include <agrum/PRM/elements/classElementContainer.h>
#include <agrum/PRM/elements/class.h>

namespace gum {
  namespace prm {

    /**
     * @class Instance<GUM_SCALAR> instance.h <agrum/PRM/elements/instance.h>
     *
     * @brief An Instance<GUM_SCALAR> is a Bayesian Network fragment defined by a
     *Class<GUM_SCALAR> and
     *        used in a System.
     *
     * Before using an Instance<GUM_SCALAR> for inference YOU MUST call
     * Instance<GUM_SCALAR>::instantiateSlotChains() and
     *Instance<GUM_SCALAR>::instantiate() on it before,
     * otherwise DiscreteVariable pointers will be inconsistent and inference will
     * be erroneous. Of course, this must be done once you have set all reference
     * in the current system.
     *
     * @see Class<GUM_SCALAR> PRM ClassElement @ingroup prm_group
     */
    template <typename GUM_SCALAR> class Instance : public PRMObject {

      public:
      // ========================================================================
      /// @name Constructors & private operators.
      // ========================================================================
      /// @{

      /**
       * @brief Default constructor of an Instance<GUM_SCALAR>.
       *
       * When you create an Instance<GUM_SCALAR> all gum::prm::Attribute<GUM_SCALAR>
       *in it's type are
       * added, except for gum::prm::Aggregate<GUM_SCALAR> and parameters which are
       * instantiated.
       *
       * However neither of their children or parents are updated (i.e.
       * DiscreteVariable pointers are inconsistent).
       */
      Instance<GUM_SCALAR>(const std::string &name, Class<GUM_SCALAR> &type);

      /// Destructor.
      virtual ~Instance<GUM_SCALAR>();

      /// @}
      // ========================================================================
      /// @name Getters & setters.
      // ========================================================================
      /// @{

      /**
       * Returns the PRM type of this object.
       */
      virtual PRMType obj_type() const;

      /**
       * Returns the type of this instance
       */
      Class<GUM_SCALAR> &type();

      /**
       * Returns the type of this instance
       */
      const Class<GUM_SCALAR> &type() const;

      /**
       * Returns true if id matches an Attribute<GUM_SCALAR> in this
       * Instance<GUM_SCALAR>.
       */
      bool exists(NodeId id) const;

      /**
       * Returns true if name matches an Attribute<GUM_SCALAR> in this
       * Instance<GUM_SCALAR>.
       */
      bool exists(const std::string &name) const;

      /**
       * @brief Getter on an Attribute<GUM_SCALAR> of this Instance<GUM_SCALAR>.
       *
       * Recall that Aggregate<GUM_SCALAR> are instantiated has Attribute<GUM_SCALAR>
       * when an Instance<GUM_SCALAR> is created. Thus any random variable contained
       * in this Instance<GUM_SCALAR> is mapped to an Attribute<GUM_SCALAR>.
       *
       * @param id The Attribute<GUM_SCALAR> id.
       * @return Returns a constant reference on an Attribute<GUM_SCALAR>.
       *
       * @throw NotFound Raised if no Attribute<GUM_SCALAR> matches name.
       */
      Attribute<GUM_SCALAR> &get(NodeId id);

      /**
       * @brief Given a name returns the related Attribute<GUM_SCALAR>.
       *
       * @param name The Attribute<GUM_SCALAR>'s name.
       * @return Returns a reference over the Attribute<GUM_SCALAR>.
       *
       * @throw NotFound Raised if there is no Attribute<GUM_SCALAR> named n.
       */
      Attribute<GUM_SCALAR> &get(const std::string &name);

      /**
       * @brief Getter on an Attribute<GUM_SCALAR> of this Instance<GUM_SCALAR>.
       *
       * Recall that Aggregate<GUM_SCALAR> are instantiated has Attribute<GUM_SCALAR>
       * when an Instance<GUM_SCALAR> is created. Thus any random variable contained
       * in this Instance<GUM_SCALAR> is mapped to an Attribute<GUM_SCALAR>.
       *
       * @param id The Attribute<GUM_SCALAR> id.
       * @return Returns a constant reference on an Attribute<GUM_SCALAR>.
       *
       * @throw NotFound Raised if no Attribute<GUM_SCALAR> matches name.
       */
      const Attribute<GUM_SCALAR> &get(NodeId id) const;

      /**
       * @brief Given a name returns the related ClassElement.
       *
       * @param name The ClassElement's name.
       * @return Returns a constant reference over the ClassElement.
       *
       * @throw NotFound Raised if there is no ClassElement named n.
       */
      const Attribute<GUM_SCALAR> &get(const std::string &name) const;

      /**
       * Returns the number of attributes in this Instance<GUM_SCALAR>.
       * @return the number of attributes in this Instance<GUM_SCALAR>.
       */
      Size size() const;

      /// @}
      // ========================================================================
      /// @name Parameter initialisation methods
      // ========================================================================
      /// @{

      /**
       * @brief Returns true if the given id is a parameter and is properly
       *        initialized.
       *
       * This includes parameters with default value.
       *
       * @param id The id of a parameter in this Instance<GUM_SCALAR>.
       * @return true If id is initialized.
       *
       * @throw NotFound Raised if no gum::prm::Attribute<GUM_SCALAR> matches id.
       * @throw WrongClassElement Raised if id is not a parameter.
       */
      bool isInitialised(NodeId id) const;

      /**
       * @brief Set the value of a parameter.
       *
       * Parameters are gum::prm::Attribute<GUM_SCALAR> with no parents and for which
       *a value
       * (i.e. observation) must be assigned at instantiation if no default
       * value is defined. Parameters are always instantiated so you do not need
       * to instantiate them.
       *
       * Default values are assigned using the parameter's CPF. If there is
       * no default values, it's CPF is filled with invasive nan (i.e. Not A
       * Number).
       *
       * The Potential used to assign the value to id must contain one
       * DiscreteVariable, which must be identical to get(id).type().variable().
       * it must also contain k-1 0 and one 1, where k is the domain of id's
       * DiscreteVariable.
       *
       * @param name The Attribute<GUM_SCALAR>'s name.
       * @param value The MultiDim used to assign the value of the parameter.
       *
       * @throw NotFound Raised if no gum::prm::Attribute<GUM_SCALAR> matches id.
       * @throw WrongClassElement Raised if id is not a parameter.
       * @throw OperationNotAllowed Raised if value is invalid.
       */
      void setParameterValue(const std::string &name,
                             const Potential<GUM_SCALAR> &value);

      /// @}
      // ========================================================================
      /// @name Instantiation methods.
      // ========================================================================
      /// @{

      /**
       * Deprecated: all attribute are now instantiated.
       * TODO remove this one day.
       */
      bool isInstantiated(NodeId) const;

      /**
       * Deprecated: all attribute are now instantiated.
       * TODO remove this one day.
       */
      void instantiate(NodeId id);

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
       *
       *
       */
      void instantiate();

      /**
       * @brief Returns a mapping between DiscreteVariable used in this and the
       *        ones used in this Instance<GUM_SCALAR>'s Class<GUM_SCALAR>.
       *
       * MultiDims require distinctive DiscreteVariable pointers, thus
       * interface attributes are always instantiated to prevent multiple
       * insertion of the same DiscreteVariable pointer in a MultiDim.
       *
       * @return Returns a bijection in which first elements are DiscreteVariable
       *         in this->type() and the second are in this.
       */
      const Bijection<const DiscreteVariable *, const DiscreteVariable *> &
      bijection() const;

      /// @}
      // ========================================================================
      /// @name Reference handling.
      // ========================================================================
      /// @{

      /**
       * @brief Add an Instance<GUM_SCALAR> to a given ReferenceSlot,
       *SlotChain<GUM_SCALAR> or
       *        output node.
       *
       * Three type of nodes can be associated with an Instance<GUM_SCALAR>. When an
       *Instance<GUM_SCALAR>
       * is associated with a ReferenceSlot, it represents an arc in the
       * relational skeleton. For a SlotChain<GUM_SCALAR>, it is a placeholder of
       *referenced
       * ClassElement by this Instance<GUM_SCALAR>. Finally, for an output node it
       *indicates
       * that an Instance<GUM_SCALAR> has an ClassElement referring it.
       *
       * @param id The NodeId of a ReferenceSlot of this Instance<GUM_SCALAR>.
       * @param instance The instance added as references by id in this
       *Instance<GUM_SCALAR>.
       *
       * @throw WrongClassElement Raised if id is not a valid ClassElement.
       * @throw OutOfUpperBound Raised if no more Instance<GUM_SCALAR> can be added
       *to id.
       * @throw TypeError Raised if instance's type isn't compatible with the
       *                  reference's type.
       * @throw NotFound If there is no ClassElement given id.
       */
      void add(NodeId id, Instance<GUM_SCALAR> &instance);

      /**
       * @brief Fast access to the first instance in a ReferenceSlot or
       *SlotChain<GUM_SCALAR>.
       *
       * This is equivalent to **(this->getInstance(id).begin()) and should be use
       * when dealing with non multiple ReferenceSlot or SlotChain<GUM_SCALAR>.
       *
       * @param id The NodeId of a ReferenceSlot or SlotChain<GUM_SCALAR> in this
       *Instance<GUM_SCALAR>.
       *
       * @throw NotFound Raised if there is no ClassElement given id.
       * @throw UndefinedElement Raised if no Instance<GUM_SCALAR> are referenced by
       *id.
       */
      const Instance<GUM_SCALAR> &getInstance(NodeId id) const;

      /**
       * @brief Returns the Set of Instance<GUM_SCALAR> referenced by id.
       *
       * @param id The NodeId of a ReferenceSlot or SlotChain<GUM_SCALAR> in this.
       * @return Returns the Set of Instance<GUM_SCALAR> referenced by id.
       *
       * @throw NotFound If there is no ClassElement given id.
       */
      const Set<Instance<GUM_SCALAR> *> &getInstances(NodeId id) const;

      /**
       * Returns true if id has at least one referring Attribute<GUM_SCALAR>.
       * @param id A NodeId.
       * @return returns true if is has at least one referring Attribute<GUM_SCALAR>.
       */
      bool hasRefAttr(NodeId id) const;

      /**
       * @brief Returns a vector of pairs of refering attributes of id.
       */
      std::vector<std::pair<Instance<GUM_SCALAR> *, std::string>> &
      getRefAttr(NodeId id);

      /**
       * @brief Returns a vector of pairs of refering attributes of id.
       */
      const std::vector<std::pair<Instance<GUM_SCALAR> *, std::string>> &
      getRefAttr(NodeId id) const;

      /// @}
      // ========================================================================
      /// @name Iterators
      // ========================================================================
      /// @{

      /// Alias to iterate over the gum::prm::Attribute<GUM_SCALAR> in this
      /// Instance<GUM_SCALAR>.
      typedef typename NodeProperty<Attribute<GUM_SCALAR> *>::iterator iterator;

      /// Returns an iterator at the begining of the list of
      /// gum::prm::Attribute<GUM_SCALAR> in
      /// this Instance<GUM_SCALAR>.
      iterator begin();

      /// Returns a reference over the iterator at the end of the list of
      /// gum::prm::Attribute<GUM_SCALAR> in this Instance<GUM_SCALAR>.
      const iterator &end();

      /// Alias to iterate over the gum::prm::Attribute<GUM_SCALAR> in this
      /// Instance<GUM_SCALAR>.
      typedef typename NodeProperty<Attribute<GUM_SCALAR> *>::const_iterator
          const_iterator;

      /// Returns an iterator at the beginning of the list of
      /// gum::prm::Attribute<GUM_SCALAR> in
      /// this Instance<GUM_SCALAR>.
      const_iterator begin() const;

      /// Returns a reference over the iterator at the end of the list of
      /// gum::prm::Attribute<GUM_SCALAR> in this Instance<GUM_SCALAR>.
      const const_iterator &end() const;

      /**
       * Nested class to iterate over ReferenceSlot and SlotChain<GUM_SCALAR>
       * instantiations.
       */
      class RefIterator {

        public:
        RefIterator(Set<Instance<GUM_SCALAR> *> &set);

        RefIterator(const RefIterator &from);

        ~RefIterator();

        RefIterator &operator=(const RefIterator &from);

        RefIterator &operator++();

        bool isEnd() const;

        bool operator!=(const RefIterator &from) const;

        bool operator==(const RefIterator &from) const;

        Instance<GUM_SCALAR> &operator*() const;
        Instance<GUM_SCALAR> *operator->() const;

        private:
        Set<Instance<GUM_SCALAR> *> &__set;
        typename Set<Instance<GUM_SCALAR> *>::iterator __iter;
      };

      /**
       * Returns an iterator at the beginning of the set of Instance<GUM_SCALAR>
       *associated
       * to a given gum::prm::ReferenceSlot or gum::prm::SlotChain<GUM_SCALAR>.
       *
       * @param id A gum::prm::ReferenceSlot or gum::prm::SlotChain<GUM_SCALAR> in
       *this
       *           Instance<GUM_SCALAR> type.
       *
       * @throw NotFound Raised if no gum::prm::ClassElement in this
       *Instance<GUM_SCALAR>
       *                 type matches id.
       * @throw WrongClassElement Raised if id is neither a ReferenceSlot or
       *                          SlotChain<GUM_SCALAR>.
       */
      RefIterator begin(NodeId id);

      /**
       * Nested class to iterate over ReferenceSlot and SlotChain<GUM_SCALAR>
       * instantiations.
       */
      class RefConstIterator {
        public:
        RefConstIterator(const Set<Instance<GUM_SCALAR> *> &set);

        RefConstIterator(const RefConstIterator &from);

        ~RefConstIterator();

        RefConstIterator &operator=(const RefConstIterator &from);

        RefConstIterator &operator++();

        bool isEnd() const;

        bool operator!=(const RefConstIterator &from) const;

        bool operator==(const RefConstIterator &from) const;

        const Instance<GUM_SCALAR> &operator*() const;
        const Instance<GUM_SCALAR> *operator->() const;

        private:
        const Set<Instance<GUM_SCALAR> *> &__set;
        typename Set<Instance<GUM_SCALAR> *>::const_iterator __iter;
      };

      /**
       * Returns an iterator at the beginning of the set of Instance<GUM_SCALAR>
       *associated
       * to a given gum::prm::ReferenceSlot or gum::prm::SlotChain<GUM_SCALAR>.
       *
       * @param id A gum::prm::ReferenceSlot or gum::prm::SlotChain<GUM_SCALAR> in
       *this
       *           Instance<GUM_SCALAR> type.
       *
       * @throw NotFound Raised if no gum::prm::ClassElement in this
       *Instance<GUM_SCALAR>
       * type matches id.
       * @throw WrongClassElement Raised if id is neither a ReferenceSlot or
       * SlotChain<GUM_SCALAR>.
       */
      RefConstIterator begin(NodeId id) const;

      typedef typename NodeProperty<
          std::vector<std::pair<Instance<GUM_SCALAR> *, std::string>> *>::iterator
          InvRefIterator;
      typedef typename NodeProperty<std::vector<
          std::pair<Instance<GUM_SCALAR> *, std::string>> *>::const_iterator
          InvRefConstIterator;

      InvRefIterator beginInvRef();
      const InvRefIterator &endInvRef();

      InvRefConstIterator beginInvRef() const;
      const InvRefConstIterator &endInvRef() const;

      /// @}
      private:
      /// Copy constructor.
      Instance<GUM_SCALAR>(const Instance<GUM_SCALAR> &source);

      /// Copy operator. Don't use it.
      Instance<GUM_SCALAR> &operator=(const Class<GUM_SCALAR> &from);

      // ========================================================================
      /// @name Private instantiation methods.
      // ========================================================================
      /// @{

      /// Used at construction to instantiate aggregates.
      void __copyAggregates(Aggregate<GUM_SCALAR> *source);

      /// Used at construction to instantiate attributes.
      /// @param source An attribute in __type.
      void __copyAttribute(Attribute<GUM_SCALAR> *source);

      /// Used at construction to instantiate parameters.
      /// @param source A parameter in __type.
      void __copyParameter(Attribute<GUM_SCALAR> *source);

      /**
       * @brief Add i as the inverse instantiation of name.
       *
       * @param name Either an inverse ReferenceSlot or an inverse
       *SlotChain<GUM_SCALAR>.
       * @param i An inverse Instance<GUM_SCALAR> added to name.
       *
       * @throw NotFound Raised if name does not match any ClassElement in this.
       * @throw WrongClassElement Raised if name is not a ReferenceSlot nor a
       *                          SlotChain<GUM_SCALAR>.
       * @throw TypeError Raised if i is not a valid subtype for name.
       */
      void __addInverse(const std::string &name, Instance<GUM_SCALAR> *i);

      /**
       * @brief This method is used to propagate instantiations between
       *Instance<GUM_SCALAR>
       *        sharing dependencies.
       *
       * @param visited A Set to prevent any unnecessary call to __instantiate().
       */
      void __instantiate(Set<Instance<GUM_SCALAR> *> visited);

      /// This instantiate the corresponding aggregate.
      void __instantiateAggregate(NodeId id);

      /// This instantiate the corresponding attribute.
      void __instantiateAttribute(NodeId id);

      /// This instantiate a parent of given node.
      void __instantiateParent(NodeId child, NodeId parent);

      /// This instantiate a child of given node.
      void __instantiateChild(NodeId child, NodeId parent);

      /// @brief Retrieve all instances referred by sc.
      /// @param sc A slot chain of this instance's type.
      /// @throw NotFound Raised if a reference in sc is not instantiated.
      void __instantiateSlotChain(SlotChain<GUM_SCALAR> *sc);

      /// @brief Copy the content of an Attribute<GUM_SCALAR> from its
      /// Class<GUM_SCALAR> counterpart.
      /// @param attr An Attribute<GUM_SCALAR> of this Instance<GUM_SCALAR>.
      /// @throw OperationNotAllowed If the MultiDimImplementation is of an unknown
      /// type.
      void __copyAttributeCPF(Attribute<GUM_SCALAR> *attr);

      /// @brief Add this as a referring instance over the attribute pointed by sc
      ///        in i.
      /// @param sc A slot chain pointing over an attribute in i.
      /// @param i An instance holding an attribute pointed by sc.
      void __addReferingInstance(SlotChain<GUM_SCALAR> *sc, Instance<GUM_SCALAR> *i);

      /// @}
      // ========================================================================
      /// @name Private members.
      // ========================================================================
      /// @{

      /// The type of this Instance<GUM_SCALAR>.
      Class<GUM_SCALAR> *__type;

      /// The gum::prm::Attribute<GUM_SCALAR> and gum::prm::Aggregate<GUM_SCALAR> of
      /// this Instance<GUM_SCALAR>.
      NodeProperty<Attribute<GUM_SCALAR> *> __nodeIdMap;

      /// Mapping between the gum::prm::ReferenceSlot and
      /// gum::prm::SlotChain<GUM_SCALAR> in
      /// __type / and the Instance<GUM_SCALAR> associated with it.
      NodeProperty<Set<Instance<GUM_SCALAR> *> *> __referenceMap;

      /// Code alias.
      typedef std::pair<Instance<GUM_SCALAR> *, std::string> pair;

      /// The set of pair (instance, attribute) referring an attribute of
      /// this instance.
      NodeProperty<std::vector<pair> *> __referingAttr;

      /// The gum::Set of initialised parameters.
      Set<NodeId> *__params;

      /// A bijection used for MultiDim handling.
      Bijection<const DiscreteVariable *, const DiscreteVariable *> __bijection;

      /// @}
    };

    extern template class Instance<double>;

  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/elements/instance.tcc>

#endif /* GUM_INSTANCE_H */
