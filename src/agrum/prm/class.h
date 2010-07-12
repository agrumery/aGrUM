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
 * @brief Headers of gum::prm::Class.
 *
 * @author Lionel TORTI
 */
// ============================================================================
#ifndef GUM_CLASS_H
#define GUM_CLASS_H
// ============================================================================
#include <utility>
#include <set>
// ============================================================================
#include <agrum/core/sequence.h>
// ============================================================================
#include <agrum/core/bijection.h>
// ============================================================================
#include <agrum/prm/classElementContainer.h>
#include <agrum/prm/interface.h>
#include <agrum/prm/attribute.h>
#include <agrum/prm/referenceSlot.h>
#include <agrum/prm/aggregate.h>
#include <agrum/prm/slotChain.h>
// ============================================================================
namespace gum {
namespace prm {
// ============================================================================
/**
 * @class Class class.h <agrum/prm/class.h>
 * @brief A Class is an object of a PRM representing a fragment of a Bayesian
 *        Network which can be instantiated in Instance.
 *
 *
 * @see PRM Class ClassElement
 * @ingroup prm_group
 */
class Class: public ClassElementContainer {
  // ========================================================================
  friend class Interface;
  // ========================================================================
  public:
  // ========================================================================
  /// @name Constructor & destructor.
  // ========================================================================
    /// @{

    /**
     * Default constructor.
     * @param name The class name.
     */
    Class(const std::string& name);

    /**
     * Constructor for building a subclass of super.
     * @param name The subclass name.
     * @param super The super Class of this.
     */
    Class(const std::string& name, Class& super);

    /**
     * Constructor for building a Class implementing several each interface
     * in set.
     * @param name The sub class name.
     * @param set The Set of implemented interfaces.
     */
    Class(const std::string& name, const Set<Interface*>& set);

    /**
     * Constructor for building a subclass of super and implementing each
     * interface in set.
     * @param name The sub class name.
     * @param super The super Class of this.
     * @param set The Set of implemented interfaces.
     */
    Class(const std::string& name, Class& super, const Set<Interface*>& set);

    /// Copy constructor.
    Class(const Class& source);

    /// Destructor.
    virtual ~Class();

    /// Implementation of pure virtual method of PRMObject.
    virtual ObjectType obj_type() const;
    /// @}
  // ========================================================================
  /// @name Graphical operator
  // ========================================================================
    /// @{

    /// See gum::prm::ClassElementContainer::get(NodeId).
    ClassElement& get(NodeId id);

    /// See gum::prm::ClassElementContainer::get(NodeId).
    const ClassElement& get(NodeId id) const;

    /// @}
  // ========================================================================
  /// @name ClassElement getters and setters
  // ========================================================================
    /// @{

    // /**
    //  * @brief returns the state of a parameter.
    //  *
    //  * Parameters are Attribute without any dependencies and for which a value
    //  * (i.e. observation) must be assigned at instantiation if no default
    //  * value is defined.
    //  *
    //  * Default values are assigned using the parameter's CPF. If there is
    //  * no default values, it's CPF is filled with invasive nan (i.e. Not A
    //  * Number).
    //  *
    //  * @param elt A ClassElement.
    //  * @return Returns 0 if n is not a parameter, 1 if it is a
    //  *         parameter and 2 if it haves a default value.
    //  */
    /**
     * @brief returns the state of a parameter.
     *
     * Parameters are Attribute without any dependencies and for which a value
     * (i.e. observation) must be assigned at instantiation if no default
     * value is defined.
     *
     * Default values are assigned using the parameter's CPF. If there is
     * no default values, it's CPF is filled with invasive nan (i.e. Not A
     * Number).
     *
     * @param elt A ClassElement.
     * @return true if elt is a parameter.
     */
    bool isParameter(const ClassElement& elt) const;

    /**
     * @brief Add a parameter to this Class.
     *
     * Parameters are Attribute without any dependencies and for which a value
     * (i.e. observation) must be assigned at instantiation if no default
     * value is defined.
     *
     * Default values are assigned using the parameter's CPF. If there is
     * no default values, it's CPF is filled with invasive nan (i.e. Not A
     * Number).
     *
     * @param param The Attribute added as a Parameter of this Class.
     * @param flag If true the initialization flag indicated that the given
     *             parameter is already initialized and does not require
     *             initialization at instantiation.
     * @return Returns the NodeId assigned to the added parameter.
     *
     * @throw DuplicateElement Raised if elt's name is already used in this class.
     */
    NodeId addParameter(Attribute* param, bool flag);

    /// See gum::prm::ClassElementContainer::get(const std::string&).
    ClassElement& get(const std::string& name);

    /// See gum::prm::ClassElementContainer::get(const std::string&).
    const ClassElement& get(const std::string& name) const;

    /// See gum::prm::add(ClassElement*).
    virtual NodeId add(ClassElement* elt);

    /// See gum::prm::overload(ClassElement*).
    virtual NodeId overload(ClassElement* elt);

    /// See gum::prm::ClassElementContainer::insertArc().
    virtual void insertArc(const std::string& tail, const std::string& head);

    /**
     * Returns the set of Attribute of this Class.
     * @return Returns the set of Attribute of this Class.
     */
    const Set< Attribute* >& attributes() const;

    /**
     * Returns the set of parameters of this Class.
     * @return Returns the set of parameters of this Class.
     */
    const Set< Attribute* >& parameters() const;

    /**
     * Returns the set of Aggregate of this Class.
     * @return Returns the set of Aggregate of this Class.
     */
    const Set< Aggregate* >& aggregates() const;

    /**
     * Returns the set of ReferenceSlot of this Class.
     * @return Returns the set of ReferenceSlot of this Class.
     */
    const Set< ReferenceSlot* >& referenceSlots() const;

    /**
     * Returns the set of SlotChain of this Class.
     * @return Returns the set of SlotChain of this Class.
     */
    const Set< SlotChain* >& slotChains() const;

    /**
     * Returns the Set of ClassElement's which must be instantiated
     * at Instance level.
     * @return the Set of ClassElement's which must be instantiated
     *        at Instance level.
     */
    const Sequence<NodeId>& toInstantiate() const;

//    /**
//     * @brief Remove a ClassElement from this Class.
//     *
//     * When removing a ClassElement it will not be deleted by this Class, thus
//     * you should delete it yourself.
//     *
//     * All dependencies among the removed ClassElement and ClassElement defined
//     * in and outside of this class are deleted also. You must update the
//     * corresponding CPF yourself.
//     *
//     * Futhermore if there exists Instance of this Class you should be very
//     * careful at what you are doing (for instance do not delete the
//     * ClassElement before deleting the concerned Instance).
//     *
//     * @param id The ClassElement's NodeId.
//     * @return the pointer over the removed ClassElement.
//     *
//     * @throw NotFound If no ClassElement matches id.
//     */
//    ClassElement* remove(NodeId id);

    /// @}
  // ========================================================================
  /// @name Inheritance methods
  // ========================================================================
    /// @{

    /**
     * @brief Test if this Class is a subclass of cec.
     *
     * If cec is a Class then this Class is a subclass of cec if they are
     * equal or there exists a superclass of this Class which is equal to cec.
     *
     * If cec is an Interface then this Class is a subclass of cec if it
     * implements cec or if there exists a superclass of this Class which
     * implements cec.
     *
     * @param cec The ClassElementContainer for which we determine if this
     *            Class is a subclass of it.
     * @return Returns true if this Class is a subclass of cec.
     */
    virtual bool isSubTypeOf(const ClassElementContainer& cec) const;

    /**
     * @brief Returns the super Class of this Class.
     * @return Returns the super Class of this Class.
     * @throw NotFound Raised if this has no super Class.
     */
    const Class& super() const;

    /**
     * @brief Returns the Set of Interface implemented by this Class.
     * @return Returns the Set of Interface implemented by this Class.
     * @throw NotFound Raised if this Class doesn't implement any Interface.
     */
    const Set<Interface*>& implements() const;

    /// Returns the set of Class which are direct sub-Class of this Class.
    const Set<Class*>& extensions() const;

    /// @}
  // ========================================================================
  /// @name Getters & setters operators
  // ========================================================================
    /// @{

    /// See gum::prm::ClassElementContainer::operator[](NodeId).
    ClassElement& operator[](NodeId id);

    /// See gum::prm::ClassElementContainer::operator[](NodeId).
    const ClassElement& operator[](NodeId id) const;

    /// See gum::prm::ClassElementContainer::operator[](const std::string&).
    ClassElement& operator[](const std::string& name);

    /// See gum::prm::ClassElementContainer::operator[](const std::string&).
    const ClassElement& operator[](const std::string& name) const;

    /// @}
  // ========================================================================
  /// @name Iterators
  // ========================================================================
    /// @{

    /// Code alias for iterators over ClassElement in this Class.
    typedef Property<ClassElement*>::onNodes::iterator ClassEltIterator;

    /// Returns an iterator at the begin of the node map of the ClassElement
    /// in this Class
    ClassEltIterator begin();

    /// Returns an interator at the end of the node map of the ClassElement
    /// in this Class
    const ClassEltIterator& end();

    /// Code alias for constant iterators over ClassElement in this Class.
    typedef Property<ClassElement*>::onNodes::const_iterator const_ClassEltIterator;

    /// Returns a constant iterator at the begin of the node map of
    /// ClassElement in this Class
    const_ClassEltIterator begin() const;

    /// Returns an interator at the end of the node map of the ClassElement
    /// in this Class
    const const_ClassEltIterator& end() const;

    /// @}
  protected:

    /// returns a constant reference over this interface's dag.
    virtual const DAG& _dag() const;

    /// Returns a non constant reference over this Interface's DAG.
    virtual DAG& _dag();

    /// Fills set with all the subtypes of this Class.
    void _findAllSubtypes(Set<ClassElementContainer*>& set);

    /// See gum::prm::ClassElementContainer(const ClassElement&).
    void _updateDescendants(const ClassElement& elt);

  private:
    /// Copy operator. Don't use it.
    Class& operator=(const Class& source);

    /// Proceed with the copy of c in this.
    // void __copyClass(const Class& c);

  // ========================================================================
  /// @name Graphical model members
  // ========================================================================
    /// @{

    /// The dag representing dependencies between formal attributes and 
    /// slots.
    DAG __dag;

    /// Mapping between node's id and their name (being an attribute or a 
    /// slot). Used for fast access to a member given it's node id.
    Property<ClassElement*>::onNodes __nodeIdMap;

    /// @}
  // ========================================================================
  /// @name ClassElement members
  // ========================================================================
    /// @{

    /// Mapping between a member's name and itself.
    /// Used for fast access to a member given it's name.
    HashTable<std::string, ClassElement*> __nameMap;

    /// The sequence of Attributes.
    Set<Attribute*> __attributes;

    /// The sequence of ReferenceSlot.
    Set<ReferenceSlot*> __referenceSlots;

    /// The sequence of aggregate.
    Set<Aggregate*> __aggregates;

    /// The set of gum::SlotChains
    Set<SlotChain*> __slotChains;

    /// Recursively adds cast descendant of attr in this Class.
    void __addCastDescendants(Attribute* attr);

    /// Recursively adds cast descendant from start to end in this Class.
    void __addCastDescendants(Attribute* start, Attribute* end);

    /// When an attribute or an aggregate is overloaded it's inherited counterpar
    /// is deleted. However we need to keep traof it's Type pointer. This method
    /// swaps the Type's between overloader and overloaded and ends by deleting
    /// overloaded.
    void __swap_types(ClassElement* overloader, ClassElement* overloaded);

    /// @}
  // ========================================================================
  /// @name Inheritance members
  // ========================================================================
    /// @{

    /// @brief The alternate ClassElementContainer searched for elements defined in
    ///        this.
    /// Note that this is first searched for gum::ClassElement.
    Class* __super;

    /// The Set of implemented interface of this.
    Set<Interface*>* __implements;

    /// The set of Class which are extension of this Class (i.e. direct
    /// subtypes).
    Set<Class*> __extensions;

    /// Proceed with the copy when this inherits c.
    void __inheritClass(const Class& c);

    /// Check if elt is present in an implementation. If it is, its IO flags
    /// are updated.
    void __addIOInterfaceFlags(ClassElement* elt);

    /// This method is called when a sub-Class of this Class is created.
    /// @param c The Class added as a direct sub-Class of this.
    /// @throw DuplicateElement Raised if c is already a sub-Class of this.
    void __addExtension(Class* c);

    /// Throws something if overload is illegal.
    void __checkOverloadLegality(const ClassElement* overloaded,
                                 const ClassElement* overloader);

    /// Overloads an attribute.
    void __overloadAttribute(Attribute* overloader, Attribute* overloaded);

    /// Overloads an aggregate.
    void __overloadAggregate(Aggregate* overloader, ClassElement* overloaded);

    /// Overloads a reference slot.
    void __overloadReference(ReferenceSlot* overloader, ReferenceSlot* overloaded);

    /// @}
  // ========================================================================
  /// @name Initialisation members
  // ========================================================================
    /// @{

    /// The set of attributes and aggregates which must redefined at instance
    /// level.
    /// This concerns any output and input nodes.
    /// A sequence is used because the nodes are ordered given their
    /// topological order.
    /// This is mutable because it does not modify the class, only updates
    /// information on it.
    Sequence<NodeId> __instantiations;

    /// The Set of parameters in this Class.
    Set< Attribute* > __parameters;

    /// The HashTable of parameters in this.
    /// The value correspond to the initialization flag.
    HashTable<const Attribute*, bool> __paramValueFlags;

    /// @}
// ==========================================================================
};

} /* namespace prm */
} // namespace gum
// ==========================================================================
#ifndef GUM_NO_INLINE
#include <agrum/prm/class.inl>
#endif // GUM_NO_INLINE
// ==========================================================================
#endif /* GUM_CLASS_H */
// ==========================================================================
