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
 * @brief Headers of gum::Instance
 *
 * @author Lionel TORTI
 */
// ============================================================================
#ifndef GUM_INSTANCE_H
#define GUM_INSTANCE_H
// ============================================================================
#include <utility>
// ============================================================================
#include <agrum/core/bijection.h>
// ============================================================================
#include <agrum/multidim/multiDimBijArray.h>
// ============================================================================
#include <agrum/prm/classElementContainer.h>
#include <agrum/prm/class.h>
// ============================================================================
namespace gum {
namespace prm {
// ============================================================================
/**
 * @class Instance instance.h <agrum/prm/instance.h>
 * An instance is design an instantiation of a Class in a Model.
 */
class Instance: public ClassElementContainer {
  public:
  // ========================================================================
  /// @name Constructors & private operators.
  // ========================================================================
    /// @{

    /// Default constructor.
    Instance(const std::string& name, Class& type);

    /// Destructor.
    virtual ~Instance();

    /// @}
  // ========================================================================
  /// @name Getters & setters.
  // ========================================================================
    /// @{

    /**
     * Returns the PRM type of this object.
     */
    virtual ObjectType obj_type() const;

    /**
     * Returns the type of this instance
     */
    Class& type();

    /**
     * Returns the type of this instance
     */
    const Class& type() const;

    /// Returns true if the given ClassElement is instantiated in this
    /// Instance.
    bool isInstantiated(NodeId) const;

    /// Instantiate the given node and all it's dependencies.
    /// @throw OperationNotAllowed Raised if the given can't be instantiated.
    void instantiate(NodeId id);

    /// Returns bijection between variables in this Instance and it's type.
    const Bijection<const DiscreteVariable*, const DiscreteVariable*>& bijection() const;

    /// Initialize a mutable Attribute
    /// @throw OperationNotAllowed Raised if a is not mutable.
    void initialize(NodeId id, const Potential<prm_float>& value);

    /// Returns the Set of initialized mutable Attribute in this.
    /// @throw NotFound Raised if there is no mutable Attribute in this.
    const Set<const Attribute*>& mutables() const;

    /// @}
  // ========================================================================
  /// @name Reference handling.
  // ========================================================================
    /// @{

    /**
     * @brief Add an gum::Instance to a given gum::ReferenceSlot.
     *
     * throw OperationNotAllowed Raised if the gum::ReferenceSlot cannot
     *                           receive another Instance.
     * @throw OperationNotAllowed Raised if instance's type isn't compatible
     *                            with the reference's type.
     * @throw NotFound If no gum::ReferenceSlot matches id.
     */
    void add(NodeId id, Instance& instance);

    /**
     * Returns the instantiation of a given ReferenceSlot or
     * SlotChain.
     * @throw NotFound Raised if no instantiation are associated to ref.
     * @throw OperationNotAllowed Raised if the SlotChain or ReferenceSlot
     *        is multiple
     */
    const Instance& getInstance(NodeId id) const;

    /**
     * Returns the instantiation of a given ReferenceSlot or
     * SlotChain.
     * @throw NotFound Raised if no instantiation are associated to ref.
     * @throw OperationNotAllowed Raised if the SlotChain or ReferenceSlot
     *        is multiple
     */
    Instance& getInstance(NodeId id);

    /**
     * Returns the sequence of instantiation of a given ReferenceSlot or
     * SlotChain.
     * @throw NotFound Raised if no instantiation are associated to ref.
     */
    const Set<Instance*>& getInstances(NodeId id) const;

    /// The firs element is an instance with the second element being a
    /// SlotChain pointing on this.
    typedef std::pair<Instance*, NodeId> InverseSC;

    /**
     * Return the Set of inverse slot chains for the given node.
     * Note that only output nodes have inverse slot chains.
     * @throw NotFound If the given node has no inverse slot chain.
     */
    const Set< InverseSC* >& inverseSC(NodeId id) const;

    typedef Property< Set< InverseSC* >* >::onNodes::const_iterator PSISCIterator;

    PSISCIterator beginPSISC() const;
    const PSISCIterator& endPSISC() const;

    /// @}
  protected:

    /// See ClassElementContainer::_isSubTypeOf().
    virtual bool _isSubTypeOf(const ClassElementContainer& cec) const;

  private:
    /// Copy constructor. Don't use it.
    Instance(const Instance& from);

    /// Copy operator. Don't use it.
    Instance& operator=(const Class& from);

  // ========================================================================
  /// @name Instantiation methods.
  // ========================================================================
    /// @{

    /// This instantiate the corresponding aggregate.
    void __instantiateAggregate(NodeId id);

    /// This instantiate the corresponding attribute.
    void __instantiateAttribute(NodeId id);

    /// This instantiate a parent of given node.
    void __instantiateParent(NodeId child, NodeId parent);

    /// This instantiate a child of given node.
    void __instantiateChildren(NodeId child, NodeId parent);

    /// Instantiate chain starting from instance. This will add all resulting
    /// Instance in the Set associated with chain and instantiate all nodes
    /// pointed by the chain.
    /// The private member __slotChainMap must be correctly initialized.
    void __instantiateSlotChain(NodeId id);

    /// Add the given instance in the set of instances belong to the inverse
    /// slot chain of one existing between i and this.
    /// @param id The id of the node pointed by the slot chain in i.
    /// @param i An instance for which a slot chain points on this->get(id).
    /// @param sc_id The NodeId of the slot chain in i pointing on id.
    void __addAsInverseSC(NodeId id, Instance* i, NodeId sc_id);

    /// Initialization the bijection of variables between this Instance and
    /// it's type.
    void __init_bijection() const;

    /// @}
  // ========================================================================
  /// @name Private members.
  // ========================================================================
    /// @{

    /// Mapping between the gum::ReferenceSlot in __type
    /// and the gum::Instance associated with it.
    Property< Set< Instance* >* >::onNodes __referenceMap;

    /// Code alias
    typedef HashTable<Instance*, NodeId> INMap;

    /// Mapping between the gum::SlotChain in __type
    /// and the gum::Instance associated with it.
    Property< INMap* >::onNodes __slotChainMap;

    /// Set used to know which node has been instantiated
    Set<NodeId> __instantiated_nodes;

    /// Mapping between nodes and there inverse slot chains instantiations.
    Property< Set< InverseSC* >* >::onNodes __inverseSC;

    /// A bijection used for MultiDim handling.
    mutable Bijection<const DiscreteVariable*, const DiscreteVariable*>* __bijection;

    /// The Set of initialized mutable Attribute
    Set<const Attribute*>* __mutables;

    /// @}
};
// ============================================================================
} /* namespace prm */
} /* namespace gum */
// ============================================================================
#ifndef GUM_NO_INLINE
#include <agrum/prm/instance.inl>
#endif // GUM_NO_INLINE
// ============================================================================
#endif /* GUM_INSTANCE_H */
// ============================================================================
