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
 * @brief Headers of gum::Class
 *
 * @author Lionel TORTI
 */
// ============================================================================
#ifndef GUM_CLASS_H
#define GUM_CLASS_H
// ============================================================================
#include <utility>
#include <list>
// ============================================================================
#include <agrum/core/sequence.h>
// ============================================================================
#include <agrum/prm/classElementContainer.h>
// ============================================================================
namespace gum {
// ============================================================================
/**
 * @class Class class.h <agrum/prm/class.h>
 * @brief this class represents a class in a Probabilistic Relational Model.
 *
 * A class is a mapping between a set of attributes and slots and a DAG.
 *
 * Each attributes has a set of parents which are either attributes of the
 * same class (i.e. X.B), a slot chain, or an aggregate (which is a summary
 * of n-ary relation).
 *
 * Aggregate cannot be built at the class level, since the number of parents
 * of an Aggregate is known only when we create a Model.
 *
 * Attributes present in a slot chain must be have a specific Potential for
 * each instance of this class. This is necessary to enable evidence handling
 * since we need different pointers for different DiscreteVariable.
 *
 * @see PRM PRMObject Attribute ReferenceSlot Aggregate SlotChain
 */
class Class: public ClassElementContainer {
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
     * Constructor for building a sub class of mother.
     * @param name The sub class name.
     * @param mother The class which this is an extension.
     */
    Class(const std::string& name, Class& mother);

    /**
     * Destructor.
     */
    virtual ~Class();

    /// @}
  // ========================================================================
  /// @name Getters & setters
  // ========================================================================
    /// @{

    /**
     * Implementation of pure virtual method of PRMObject.
     */
    virtual ObjectType obj_type() const;

    /**
     * Returns the mother of this class.
     * @throw NotFound Raised if this has no mother.
     */
    Class& mother();

    /**
     * Returns the mother of this class.
     * @throw NotFound Raised if this has no mother.
     */
    const Class& mother() const;

    /**
     * @brief Add an attribute to this class.
     *
     * The pointer is "given" to this class, which will delete when
     * this->~Class() is called.
     *
     * If there is already an element in this class with the same than attr,
     * then an DuplicateElement is raised and attr is not added to this class
     * which implies that you should handle yourself it's memory deallocation.
     *
     * @throw DuplicateElement Raised if an element in this has the same name.
     */
    void add(Attribute* attr);

    /**
     * @brief Add an aggregate to this class.
     *
     * The pointer is "given" to this class, which will delete when
     * this->~Class() is called.
     *
     * If there is already an element in this class with the same than agg,
     * then an DuplicateElement is raised and agg is not added to this class
     * which implies that you should handle yourself it's memory deallocation.
     *
     * @throw DuplicateElement Raised if an element in this has the same name.
     */
    void add(Aggregate* agg);

    /**
     * @brief Add a ReferenceSlot to this class.
     *
     * The pointer is "given" to this class, which will delete when
     * this->~Class() is called.
     *
     * @throw DuplicateElement Raised if an element in this has the same name.
     */
    void add(ReferenceSlot* ref);

    /**
     * @brief Add gum::SlotChain to this class.
     *
     * A gum::SlotChain is a reference node, it reference one or several nodes
     * defined in another ClassElementContainer.
     */
    void add(SlotChain* sc);

    /**
     * @brief Add a parent to an element in this class.
     *
     * This method insert an arc in the class's dag and do the proper
     * methods call to insert the parent in head.
     *
     * @throw OperationNotAllowed Raised if tail or head is a
     *                            gum::ReferenceSlot, or if head is a
     *                            gum::SlotChain.
     */
    void insertArc(const std::string& tail, const std::string& head);

    /// Returns a constant reference over the sequence of nodes required to
    /// instantiated when an instance of this class is created.
    const Sequence<NodeId>& instantiations() const;

    /// Returns true if the node is an input node.
    bool isInputNode(NodeId id) const;

    /// Set the input flag value of id at b.
    void setInputNode(NodeId id, bool b);

    /// Returns true if the node is an output node.
    bool isOutputNode(NodeId id) const;

    /// Set the output flag value of id at b.
    void setOutputNode(NodeId id, bool b);

    /// Returns true if the node is an inner node.
    bool isInnerNode(NodeId id) const;

    /// This fills the set of attributes and aggregates which must be
    /// redefined at instance level. This is called by the PRMFactory when
    /// the class is fully filled.
    void buildInstantiationSequence() const;

    /// @}
  // ========================================================================
  /// @name Inheritance operators
  // ========================================================================
    /// @{

    /// Returns true if daughter is a sub class of this.
    bool operator>(const Class& daughter) const;

    /// Returns true if daughter is subclass of this or is this.
    bool operator>=(const Class& daughter) const;

    /// Returns true if this is a subclass of mother.
    bool operator<(const Class& mother) const;

    /// Returns true if this is a subclass of mother or is mother.
    bool operator<=(const Class& mother) const;

    /// @}

  private:
    /// Copy constructor. Don't use it.
    Class(const Class& source);

    /// Copy operator. Don't use it.
    Class& operator=(const Class& source);

    /// Add the super types of the given attribute.
    void __addSuperType(Attribute* attr);

    /// Add an attribute overloading an inherited attribute.
    /// @throw OperationNotAllowed Raised if the overload is invalid.
    void __overload(Attribute* attr, ClassElement& elt);

    /// Add an reference overloading an inherited reference.
    /// @throw OperationNotAllowed Raised if the overload is invalid.
    void __overload(ReferenceSlot* ref, ClassElement& elt);

  // ========================================================================
  /// @name Private members
  // ========================================================================
    /// @{

    /// The set of attributes and aggregates which must redefined at instance
    /// level.
    /// This concerns any output and input nodes.
    /// A sequence is used because the nodes are ordered given their
    /// topological order.
    /// This is mutable because it does not modify the class, only updates
    /// information on it.
    mutable Sequence<NodeId> __instantiations;

    /// This is used to remember if a ClassElement is an input or output node.
    /// first is for the input flag, second is for the output flag.
    Property<std::pair<bool, bool>*>::onNodes __IOFlags;

    /// @}
// ==========================================================================
};
} // namespace gum
// ==========================================================================
#ifndef GUM_NO_INLINE
#include <agrum/prm/class.inl>
#endif // GUM_NO_INLINE
// ==========================================================================
#endif /* GUM_CLASS_H */
// ==========================================================================
