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
 * @brief Headers of gum::prm::Instance
 *
 * @author Lionel TORTI
 */
// ============================================================================
#ifndef GUM_INSTANCE_H
#define GUM_INSTANCE_H
// ============================================================================
#include <utility>
#include <set>
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
 *
 * @brief An Instance is a Bayesian Network fragment defined by a Class and
 *        used in a System.
 *
 * Before using an Instance for inference YOU MUST call
 * Instance::instantiateSlotChains() and Instance::instantiate() on it before,
 * otherwise DiscreteVariable pointers will be inconsistent and inference will
 * be erroneous. Of course, this must be done once you have set all reference
 * in the current system.
 *
 * @see Class PRM ClassElement @ingroup prm_group
 */
class Instance: public PRMObject {

  public:
  // ========================================================================
  /// @name Constructors & private operators.
  // ========================================================================
    /// @{

    /**
     * @brief Default constructor of an Instance.
     *
     * When you create an Instance all gum::prm::Attribute in it's type are
     * added, except for gum::prm::Aggregate and parameters which are
     * instantiated.
     *
     * However neither of their children or parents are updated (i.e.
     * DiscreteVariable pointers are inconsistent).
     */
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
    virtual PRMObject::ObjectType obj_type() const;

    /**
     * Returns the type of this instance
     */
    Class& type();

    /**
     * Returns the type of this instance
     */
    const Class& type() const;

    /**
     * Returns true if id matches an Attribute in this Instance.
     */
    bool exists(NodeId id);

    /**
     * @brief Getter on an Attribute of this Instance.
     *
     * Recall that Aggregate are instantiated has Attribute
     * when an Instance is created. Thus any random variable contained
     * in this Instance is mapped to an Attribute.
     *
     * @param id The Attribute id.
     * @return Returns a constant reference on an Attribute.
     *
     * @throw NotFound Raised if no Attribute matches name.
     */
    Attribute& get(NodeId id);

    /**
     * @brief Given a name returns the related Attribute.
     *
     * @param n The Attribute's name.
     * @return Returns a reference over the Attribute.
     *
     * @throw NotFound Raised if there is no Attribute named n.
     */
    Attribute& get(const std::string& name);

    /**
     * @brief Getter on an Attribute of this Instance.
     *
     * Recall that Aggregate are instantiated has Attribute
     * when an Instance is created. Thus any random variable contained
     * in this Instance is mapped to an Attribute.
     *
     * @param id The Attribute id.
     * @return Returns a constant reference on an Attribute.
     *
     * @throw NotFound Raised if no Attribute matches name.
     */
    const Attribute& get(NodeId id) const;

    /**
     * @brief Given a name returns the related ClassElement.
     *
     * @param n The ClassElement's name.
     * @return Returns a constant reference over the ClassElement.
     *
     * @throw NotFound Raised if there is no ClassElement named n.
     */
    const Attribute& get(const std::string& name) const;


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
     * @param id The id of a parameter in this Instance.
     * @return true If id is initialized.
     *
     * @throw NotFound Raised if no gum::prm::Attribute matches id.
     * @throw WrongClassElement Raised if id is not a parameter.
     */
    bool isInitialised(NodeId id) const;

    /**
     * @brief Set the value of a parameter.
     *
     * Parameters are gum::prm::Attribute with no parents and for which a value
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
     * @param id The NodeId of the parameter.
     * @param value The MultiDim used to assign the value of the parameter.
     *
     * @throw NotFound Raised if no gum::prm::Attribute matches id.
     * @throw WrongClassElement Raised if id is not a parameter.
     * @throw OperationNotAllowed Raised if value is invalid.
     */
    void setParameterValue(NodeId id, const Potential<prm_float>& value);

    /// @}
  // ========================================================================
  /// @name Instantiation methods.
  // ========================================================================
    /// @{

    /**
     * @brief An instantiated attribute has a different DiscreteVariable than
     *        the Class level version of it and in other instance.
     *
     * MultiDims require distinctive DiscreteVariable pointers, thus
     * interface Attributes are always instantiated to prevent multiple
     * insertion of the same DiscreteVariable pointer in a MultiDim.
     *
     * Be careful ! This does not check if the attribute's parents are
     * instantiated. An attribute could not be instantiated and still have
     * instatitiated parents, thus it's CPF will not hold the same pointers
     * than it's class version.
     *
     * If the NodeId does not match an existing ClassElement in this Instance,
     * isInstantiated() returns false.
     *
     * @param NodeId The ClassElement on which we test if it is instantiated.
     * @return Returns true if the ClassElement is instantiated.
     */
    bool isInstantiated(NodeId) const;

    /**
     * @brief Instantiate the given node and update it's dependencies.
     *
     * This method can be used to instantiate manually nodes in this Instance.
     * It is also useful for inference: by instantiating queried nodes their
     * respective gum::prm::DiscreteVariable becomes unique and can safely be
     * used in MultiDims.
     *
     * MultiDims require distinctive gum::prm::DiscreteVariable pointers, thus
     * interface attributes are always instantiated to prevent multiple
     * insertion of the same gum::prm::DiscreteVariable pointer in a MultiDim.
     *
     * When an Attribute is instantiated it automatically adds
     * its gum::prm::DiscreteVariable and its equivalent Class level
     * gum::prm::DiscreteVariable to Instance::__bijection.
     *
     * @warning Be aware that this will make a call to instantiate(), thus
     *          deleting a large part of attribute's CPF. Furthermore, the
     *          instantiated attribute's type will change (but will not be
     *          deleted).
     *
     * @param NodeId The node to be instantiated.
     *
     * @throw NotFound Raised if id does not match any gum::prm::Attribute
     *                 in this.
     */
    void instantiate(NodeId id);

    /**
     * @brief Instantiate all nodes which requires it.
     *
     * MultiDims require distinctive gum::prm::DiscreteVariable pointers, thus
     * interface attributes are always instantiated to prevent multiple
     * insertion of the same gum::prm::DiscreteVariable pointer in a MultiDim.
     *
     * When an Attribute or an Aggregate is instantiated it automatically adds
     * its gum::prm::DiscreteVariable and its equivalent Class level
     * gum::prm::DiscreteVariable to Instance::__bijection.
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
     *        ones used in this Instance's Class.
     *
     * MultiDims require distinctive DiscreteVariable pointers, thus
     * interface attributes are always instantiated to prevent multiple
     * insertion of the same DiscreteVariable pointer in a MultiDim.
     *
     * @return Returns a bijection in which first elements are DiscreteVariable
     *         in this and the second are in this->type().
     */
    const Bijection<const DiscreteVariable*,
                    const DiscreteVariable*>& bijection() const;

    /// @}
  // ========================================================================
  /// @name Reference handling.
  // ========================================================================
    /// @{

    /**
     * @brief Add an Instance to a given ReferenceSlot, SlotChain or
     *        output node.
     *
     * Three type of nodes can be associated with an Instance. When an Instance
     * is associated with a ReferenceSlot, it represents an arc in the
     * relational skeleton. For a SlotChain, it is a placeholder of referenced
     * ClassElement by this Instance. Finally, for an output node it indicates
     * that an Instance has an ClassElement referring it.
     *
     * @param id The NodeId of a ReferenceSlot of this Instance.
     * @param instance The instance added as references by id in this Instance.
     *
     * @throw WrongClassElement Raised if id is not a valid ClassElement.
     * @throw OutOfUpperBound Raised if no more Instance can be added to id.
     * @throw TypeError Raised if instance's type isn't compatible with the
     *                  reference's type.
     * @throw NotFound If there is no ClassElement given id.
     */
    void add(NodeId id, Instance& instance);

    /**
     * @brief Fast access to the first instance in a ReferenceSlot or SlotChain.
     *
     * This is equivalent to **(this->getInstance(id).begin()) and should be use
     * when dealing with non multiple ReferenceSlot or SlotChain.
     *
     * @param id The NodeId of a ReferenceSlot or SlotChain in this Instance.
     *
     * @throw NotFound Raised if there is no ClassElement given id.
     * @throw UndefinedElement Raised if no Instance are referenced by id.
     */
    const Instance& getInstance(NodeId id) const;

    /**
     * @brief Returns the Set of Instance referenced by id.
     *
     * @param id The NodeId of a ReferenceSlot or SlotChain in this.
     * @return Returns the Set of Instance referenced by id.
     *
     * @throw NotFound If there is no ClassElement given id.
     */
    const Set<Instance*>& getInstances(NodeId id) const;

    /// @}
  // ========================================================================
  /// @name Iterators
  // ========================================================================
    /// @{

    /// Alias to iterate over the gum::prm::Attribute in this Instance.
    typedef Property<Attribute*>::onNodes::iterator iterator;

    /// Returns an iterator at the begining of the list of gum::prm::Attribute in
    /// this Instance.
    iterator begin();

    /// Returns a reference over the iterator at the end of the list of
    /// gum::prm::Attribute in this Instance.
    const iterator& end();

    /// Alias to iterate over the gum::prm::Attribute in this Instance.
    typedef Property<Attribute*>::onNodes::const_iterator const_iterator;

    /// Returns an iterator at the begining of the list of gum::prm::Attribute in
    /// this Instance.
    const_iterator begin() const;

    /// Returns a reference over the iterator at the end of the list of
    /// gum::prm::Attribute in this Instance.
    const const_iterator& end() const;

    /**
     * Nested class to iterate over ReferenceSlot and SlotChain
     * instantiations.
     */
    class RefIterator {

      public:
        RefIterator(const Set<Instance*>::iterator& iter) ;

        RefIterator( const RefIterator& from ) ;

        ~RefIterator() ;

        RefIterator& operator=( const RefIterator& from ) ;

        RefIterator& operator++() ;

        bool operator!=(const RefIterator& from) const;

        bool operator==(const RefIterator& from) const;

        Instance& operator*() const;
        Instance* operator->() const;

      private:
        Set<Instance*>::iterator __iter;
    };

    /**
     * Returns an iterator at the beginning of the set of Instance associated
     * to a given gum::prm::ReferenceSlot or gum::prm::SlotChain.
     *
     * @param id A gum::prm::ReferenceSlot or gum::prm::SlotChain in this
     *           Instance type.
     *
     * @throw NotFound Raised if no gum::prm::ClassElement in this Instance
     *                 type matches id.
     * @throw WrongClassElement Raised if id is neither a ReferenceSlot or
     *                          SlotChain.
     */
    RefIterator begin(NodeId id);

    /**
     * Returns an iterator at the end of the set of Instance associated
     * to a given gum::prm::ReferenceSlot or gum::prm::SlotChain.
     *
     * @param id A gum::prm::ReferenceSlot or gum::prm::SlotChain in this
     *           Instance type.
     *
     * @throw NotFound Raised if no gum::prm::ClassElement in this Instance
     * type matches id.
     * @throw WrongClassElement Raised if id is neither a ReferenceSlot or
     * SlotChain.
     */
    const RefIterator& end(NodeId id);

    /**
     * Nested class to iterate over ReferenceSlot and SlotChain
     * instantiations.
     */
    class ConstRefIterator {
      public:
        ConstRefIterator(const Set<Instance*>::const_iterator& iter);

        ConstRefIterator( const ConstRefIterator& from );

        ~ConstRefIterator();

        ConstRefIterator& operator=( const ConstRefIterator& from );

        ConstRefIterator& operator++();

        bool operator!=(const ConstRefIterator& from) const;

        bool operator==(const ConstRefIterator& from) const;

        const Instance& operator*() const;
        const Instance* operator->() const;

      private:
        Set<Instance*>::const_iterator __iter;
    };

    /**
     * Returns an iterator at the beginning of the set of Instance associated
     * to a given gum::prm::ReferenceSlot or gum::prm::SlotChain.
     *
     * @param id A gum::prm::ReferenceSlot or gum::prm::SlotChain in this
     *           Instance type.
     *
     * @throw NotFound Raised if no gum::prm::ClassElement in this Instance
     * type matches id.
     * @throw WrongClassElement Raised if id is neither a ReferenceSlot or
     * SlotChain.
     */
    ConstRefIterator begin(NodeId id) const;

    /**
     * Returns an iterator at the end of the set of Instance associated
     * to a given gum::prm::ReferenceSlot or gum::prm::SlotChain.
     *
     * @param id A gum::prm::ReferenceSlot or gum::prm::SlotChain in this
     *           Instance type.
     *
     * @throw NotFound Raised if no gum::prm::ClassElement in this Instance
     * type matches id.
     * @throw WrongClassElement Raised if id is neither a ReferenceSlot or
     * SlotChain.
     */
    const ConstRefIterator& end(NodeId id) const;

    /// @}
  private:
    /// Copy constructor.
    Instance(const Instance& source);

    /// Copy operator. Don't use it.
    Instance& operator=(const Class& from);

  // ========================================================================
  /// @name Private instantiation methods.
  // ========================================================================
    /// @{

    /// Used at construction to instantiate aggregates.
    void __copyAggregates(Aggregate* source);

    /// Used at construction to instantiate attributes.
    /// @param source An attribute in __type.
    void __copyAttribute(Attribute* source);

    /// Used at construction to instantiate parameters.
    /// @param source A parameter in __type.
    void __copyParameter(Attribute* source);

    /**
     * @brief Add i as the inverse instantiation of name.
     *
     * @param name Either an inverse ReferenceSlot or an inverse SlotChain.
     * @param i An inverse Instance added to name.
     *
     * @throw NotFound Raised if name does not match any ClassElement in this.
     * @throw WrongClassElement Raised if name is not a ReferenceSlot nor a
     *                          SlotChain.
     * @throw TypeError Raised if i is not a valid subtype for name.
     */
    void __addInverse(const std::string& name, Instance* i);

    /**
     * @brief This method is used to propagate instantiations between Instance
     *        sharing dependencies.
     *
     * @param visited A Set to prevent any unnecessary call to __instantiate().
     */
    void __instantiate(Set<Instance*> visited);

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
    void __instantiateSlotChain(SlotChain* sc);

    /// @brief Add this as a referring instance over the attribute pointed by sc
    ///        in i.
    /// @param sc A slot chain pointing over an attribute in i.
    /// @param i An instance holding an attribute pointed by sc.
    void __addReferingInstance(SlotChain* sc, Instance* i);

    /// @}
  // ========================================================================
  /// @name Private members.
  // ========================================================================
    /// @{

    /// The type of this Instance.
    Class* __type;

    /// The gum::prm::Attribute and gum::prm::Aggregate of this Instance.
    Property<Attribute*>::onNodes __nodeIdMap;

    /// Mapping between the gum::prm::ReferenceSlot and gum::prm::SlotChain in
    /// __type / and the Instance associated with it.
    Property< Set< Instance* >* >::onNodes __referenceMap;

    typedef std::pair<Instance*, std::string> pair;

    Property< std::vector<pair>* >::onNodes __referingAttr;

    /// gum::Set used to know which node is instantiated. Nodes with instantiated
    /// parents are not considerer as instantiated (they share the gum::prm::Type
    /// with their class-level equivalent).
    Set<NodeId> __instantiated_nodes;

    /// The gum::Set of initialised parameters.
    Set<NodeId>* __params;

    /// A bijection used for MultiDim handling.
    Bijection<const DiscreteVariable*, const DiscreteVariable*> __bijection;

    /// A Set over pointers to delete when gum::prm::~Instance() is called.
    Set<Attribute*> __trash;

    static Size __end_counter;

    static RefIterator* __end;

    static ConstRefIterator* __const_end;

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
