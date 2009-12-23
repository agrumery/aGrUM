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
 * @brief Headers of gum::ClassElementContainer.
 *
 * @author Lionel TORTI
 */
// ============================================================================
#ifndef GUM_CLASS_ELEMENT_CONTAINER_H
#define GUM_CLASS_ELEMENT_CONTAINER_H
// ============================================================================
#include <agrum/core/set.h>
#include <agrum/core/hashTable.h>
// ============================================================================
#include <agrum/graphs/graphElements.h>
#include <agrum/graphs/DAG.h>
// ============================================================================
#include <agrum/prm/utils_prm.h>
#include <agrum/prm/PRMObject.h>
#include <agrum/prm/classElement.h>
#include <agrum/prm/attribute.h>
#include <agrum/prm/referenceSlot.h>
#include <agrum/prm/slotChain.h>
#include <agrum/prm/aggregate.h>
// ============================================================================
namespace gum {
namespace prm {
// ============================================================================
/**
 * @class ClassElementContainer classElementContainer.h <agrum/prm/classElementContainer.h>
 * @brief Abstract class for classes containing gum::ClassElement.
 *
 * A ClassElementContainer is a mapping between nodes in gum::DAG and
 * gum::ClassElement.
 *
 * This ClassElementContainer handles basic adding and getting of
 * gum::ClassElementgiven their names and ids.
 *
 * Another particularity of the ClassElementContainer is to enable delagation
 * for containing gum::ClassElement: it is possible to define an instance
 * of ClassElementContainer to be another possible holder of ClassElement.
 *
 * This is useful in inheritance in gum::Class and instance redefinition in
 * gum::Instance.
 *
 * Another tweak possible is to prevent the creation of a gum::DAG for this
 * ClassElementContainer. This is useful when the subclass is read-only
 * on it's gum::DAG.
 *
 * To print a ClassElementContainer you can use the following operator:
 * gum::operator<<(std::ostream&, const ClassElementContainer&) which print
 * the ClassElementContainer in the graphviz-dot format.
 *
 */
class ClassElementContainer: public PRMObject {
  public:
  // ========================================================================
  /// @name Protected constructors & destructor.
  // ========================================================================
    /// @{

    /// Default constructor.
    ClassElementContainer(const std::string& name);

    /**
     * Constructor with alternate ClassElementContainer for searching
     * elements.
     *
     * @param name The name of this ClassElementContainer.
     * @param alternate The alternate ClassElementContainer.
     * @param delegateDAG If true, no gum::DAG is created for this
     *                    ClassElementContainer if will used the alternate's one.
     */
    ClassElementContainer(const std::string& name,
                          ClassElementContainer& alternate,
                          bool delegateDAG);

    /// Destructor.
    virtual ~ClassElementContainer();

    /// @}
  // ========================================================================
  /// @name Getters on the gum::ClassElement.
  // ========================================================================
    /// @{

    /// Returns the gum::DAG of this ClassElementContainer.
    const DAG& dag() const;

    /// Shortcut to dag().children(node).
    const ArcSet& children(NodeId node) const;

    /// Shortcut to dag().children(elt.id()).
    const ArcSet& children(const ClassElement& elt) const;

    /// Shortcut to dag().parents(node).
    const ArcSet& parents(NodeId node) const;

    /// Shortcut to dag().parents(elt.id()).
    const ArcSet& parents(const ClassElement& elt) const;

    /**
     * Returns true if a member with the given id exists in this ClassElementContainer or
     * in the ClassElementContainer hierarchy.
     */
    bool exists(NodeId id) const;

    /**
     * Returns true if a member with the given name exists in this ClassElementContainer or
     * in the ClassElementContainer hierarchy.
     */
    bool exists(const std::string& name) const;

    /**
     * Getter on a member of this ClassElementContainer.
     * @param id The member's id.
     * @return Returns a constant reference on the member.
     * @throw NotFound Raised if no attribute matches name.
     */
    ClassElement& get(NodeId id);

    /**
     * Constant getter on a member of this ClassElementContainer.
     * @param id The member's id.
     * @return Returns a constant reference on the member.
     * @throw NotFound Raised if no attribute matches name.
     */
    const ClassElement& get(NodeId id) const;

    /**
     * Getter on a member of this ClassElementContainer.
     * @param name The member's name.
     * @return Returns a constant reference on the member.
     * @throw NotFound Raised if no attribute matches name.
     */
    ClassElement& get(const std::string& name);

    /**
     * Constant getter on a member of this ClassElementContainer.
     * @param name The member's name.
     * @return Returns a constant reference on the member.
     * @throw NotFound Raised if no attribute matches name.
     */
    const ClassElement& get(const std::string& name) const;

    /// @return Returns the Set of Attribute in this ClassElementContainer.
    Set< Attribute* >& attributes();

    /// @return Returns the Set of Attribute in this ClassElementContainer.
    const Set< Attribute* >& attributes() const;

    /// @return Returns the Set of Aggregate in this ClassElementContainer.
    Set< Aggregate* >& aggregates();

    /// @return Returns the Set of Aggregate in this ClassElementContainer.
    const Set< Aggregate* >& aggregates() const;

    /// @return Returns the Set of ReferenceSlot in this ClassElementContainer.
    Set< ReferenceSlot* >& referenceSlots();

    /// @return Returns the Set of ReferenceSlot in this ClassElementContainer.
    const Set< ReferenceSlot* >& referenceSlots() const;

    /// @return Returns the Set of SlotChain in this ClassElementContainer.
    Set< SlotChain* >& slotChains();

    /// @return Returns the Set of SlotChain in this ClassElementContainer.
    const Set< SlotChain* >& slotChains() const;

    /// @}
  // ========================================================================
  /// @name Getters & setters operators
  // ========================================================================
    /// @{

    /**
     * Getter on a member of this ClassElementContainer.
     * @param id The member's id.
     * @return Returns a constant reference on the member.
     * @throw NotFound Raised if no attribute matches name.
     */
    ClassElement& operator[](NodeId id);

    /**
     * Constant getter on a member of this ClassElementContainer.
     * @param id The member's id.
     * @return Returns a constant reference on the member.
     * @throw NotFound Raised if no attribute matches name.
     */
    const ClassElement& operator[](NodeId id) const;

    /**
     * Getter on a member of this ClassElementContainer.
     * @param name The member's name.
     * @return Returns a constant reference on the member.
     * @throw NotFound Raised if no attribute matches name.
     */
    ClassElement& operator[](const std::string& name);

    /**
     * Constant getter on a member of this ClassElementContainer.
     * @param name The member's name.
     * @return Returns a constant reference on the member.
     * @throw NotFound Raised if no attribute matches name.
     */
    const ClassElement& operator[](const std::string& name) const;

    /// @}
  // ========================================================================
  /// @name Inheritance operators
  // ========================================================================
    /// @{


    /**
     * @brief Returns true if this shares cec's type.
     *
     * To share cec's type either this is of the same type, or there exists
     * a super type of this which is of the same type.
     *
     * this->isSubTypeOf(*this) returns true.
     *
     * @param cec The searched type.
     * @return true if this shares cec's type.
     */
    bool isSubTypeOf(const ClassElementContainer& cec) const;

    /**
     * @brief Returns true if this is a super type of cec.
     *
     * This is an equivalent call to cec.isSubTypeOf(*this).
     *
     * this->isSuperTypeOf(*this) returns true.
     *
     * @return true if this is a super type of cec.
     */
    bool isSuperTypeOf(const ClassElementContainer& cec) const;

    /// @}
  protected:
  // ========================================================================
  /// @name Protected methods.
  // ========================================================================
    /// @{

    /**
     * @brief Returns true if and only if this is a sub type of cec.
     *
     * Unlike isSubTypeOf(), this->_isSubTypeOf(*this) returns false.
     *
     * Since there can be different cases regarding inheritance
     * (specialisation vs implementation) each subclass of
     * ClassElementContainer must define this method.
     */
    virtual bool _isSubTypeOf(const ClassElementContainer& cec) const =0;

    /// Returns the alternate ClassElementContainer searched for elements
    /// defined in this.
    ClassElementContainer& _alternate();

    /// Returns the alternate ClassElementContainer searched for elements
    /// defined in this.
    const ClassElementContainer& _alternate() const;


    /// Returns true if id is defined here, return false even if
    /// _alternate().get(id) returns something.
    bool _exists(NodeId id) const;

    /**
     * @brief Add an attribute to this ClassElementContainer.
     *
     * The pointer is "given" to this ClassElementContainer, which will delete when
     * this->~Class() is called.
     *
     * If there is already an element in this ClassElementContainer with the same than attr,
     * then an DuplicateElement is raised and attr is not added to this ClassElementContainer
     * which implies that you should handle yourself it's memory deallocation.
     *
     * @throw DuplicateElement Raised if an element in this has the same name.
     */
    void _add(Attribute* attr);

    /**
     * @brief Add an attribute which will overload an existing one in __alternate.
     *
     * This is used mostly when a gum::Instance instantiate gum::Aggregate.
     * @param overload If true the in going arcs of attr are erased.
     * @throw OperationNotAllowed Raised if attr can not replace the ClassElement pointed by id.
     */
    void _add(Attribute* attr, NodeId id, bool overload = false);

    /**
     * Add an Attribute overloading an inherited Attribute or Aggregate.
     * @throw OperationNotAllowed Raised if the overload is invalid.
     */
    void _overload(Attribute* attr, ClassElement& elt);

    /**
     * @brief Add an aggregate to this ClassElementContainer.
     *
     * The pointer is "given" to this ClassElementContainer, which will delete when
     * this->~Class() is called.
     *
     * If there is already an element in this ClassElementContainer with the same than agg,
     * then an DuplicateElement is raised and agg is not added to this ClassElementContainer
     * which implies that you should handle yourself it's memory deallocation.
     *
     * @throw DuplicateElement Raised if an element in this has the same name.
     */
    void _add(Aggregate* agg);

    /**
     * @brief Add an Aggregate which will overload an existing one in __alternate.
     *
     * This is used mostly when a gum::Instance instantiate gum::Aggregate.
     * @param overload If true the in going arcs of attr are erased.
     * @throw OperationNotAllowed Raised if attr can not replace the ClassElement pointed by id.
     */
    void _add(Aggregate* agg, NodeId id, bool overload);

    /**
     * Add an Aggregate overloading an inherited Aggregate or Attribute.
     * @throw OperationNotAllowed Raised if the overload is invalid.
     */
    void _overload(Aggregate* agg, ClassElement& elt);

    /**
     * @brief Add a ReferenceSlot to this ClassElementContainer.
     *
     * The pointer is "given" to this ClassElementContainer, which will delete when
     * this->~Class() is called.
     *
     * @throw DuplicateElement Raised if an element in this has the same name.
     */
    void _add(ReferenceSlot* ref);

    /**
     * Add an reference overloading an inherited reference.
     * @throw OperationNotAllowed Raised if the overload is invalid.
     */
    void _overload(ReferenceSlot* ref, ClassElement& elt);

    /**
     * @brief Add a ReferenceSlot which will overloas an existing one in __alternate.
     * The given NodeId must either belong to a ReferenceSlot in __alternate or not be affected
     * to a ClassElement yet.
     * @throw OperationNotAllowed Raised if ref can not replace the ClassElement pointed by id.
     */
    void _add(ReferenceSlot* ref, NodeId id);

    /**
     * @brief Add gum::SlotChain to this class.
     *
     * A gum::SlotChain is a reference node, it reference one or several nodes defined
     * in another ClassElementContainer.
     */
    void _add(SlotChain* sc);

    /**
     * @brief Add a parent to an element in this ClassElementContainer.
     *
     * This method insert an arc in the ClassElementContainer's dag and do the proper
     * methods call to insert the parent in head.
     *
     * @throw OperationNotAllowed Raised if tail or head is a gum::ReferenceSlot, or
     *                            if head is a gum::SlotChain.
     */
    void _insertArc(const std::string& tail, const std::string& head);

    /// Copy operator. Don't use it.
    ClassElementContainer& operator=(const ClassElementContainer& source);

    /// Copy constructor. Don't use it.
    ClassElementContainer(const ClassElementContainer& source);

    /// @}
  private:
  // ========================================================================
  /// @name Private members
  // ========================================================================
    /// @{

    /// The dag representing dependencies between formal attributes and 
    /// slots.
    DAG* __dag;

    /// Mapping between node's id and their name (being an attribute or a 
    /// slot). Used for fast access to a member given it's node id.
    Property<ClassElement*>::onNodes __nodeIdMap;

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

    /// @brief The alternate ClassElementContainer searched for elements defined in
    ///        this.
    /// Note that this is first searched for gum::ClassElement.
    ClassElementContainer* __alternate;

    /// If true, then this->__dag is a pointer over this->__alternate->__dag;
    bool __delegateDAG;

    /// @}
  // ========================================================================
  /// @name Private methods
  // ========================================================================
    /// @{

    /// Generic processing when adding an element.
    void __add(ClassElement* elt);

    /// Pointer version of operator[]. We need this because of inheritance.
    /// For friends only.
    ClassElement* __get(const std::string& name);

    /// Constant version.
    /// @see Class::__get(const std::string&)
    const ClassElement* __get(const std::string& name) const;

    /// Pointer version of operator[]. We need this because of inheritance.
    /// For friends only.
    ClassElement* __get(NodeId id);

    /// Constant version.
    /// @see Class::__get(NodeId)
    const ClassElement* __get(NodeId id) const;

    /// @}
};

// ============================================================================
/// @brief An << operator for ClassElementContainer.
/// Output in the graphviz-dot format.
std::ostream& operator<<(std::ostream& output, const ClassElementContainer& container);
// ============================================================================

} /* namespace prm */
} // namespace gum
// ============================================================================
#ifndef GUM_NO_INLINE
#include <agrum/prm/classElementContainer.inl>
#endif // GUM_NO_INLINE
// ============================================================================
#endif /* GUM_CLASS_ELEMENT_CONTAINER_H */
// ============================================================================
