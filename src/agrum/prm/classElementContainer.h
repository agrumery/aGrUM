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
 * @brief Headers of gum::prm::ClassElementContainer.
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
#include <agrum/prm/referenceSlot.h>
#include <agrum/prm/slotChain.h>
// ============================================================================
namespace gum {
namespace prm {
// ============================================================================
class Class;
class Interface;
// ============================================================================
/**
 * @class ClassElementContainer classElementContainer.h <agrum/prm/classElementContainer.h>
 * @brief Abstract class for classes containing gum::ClassElement.
 *
 * To print a ClassElementContainer you can use the following operator:
 * gum::operator<<(std::ostream&, const ClassElementContainer&) which print
 * the ClassElementContainer in the graphviz-dot format.
 *
 * @ingroup prm_group
 */
class ClassElementContainer: public PRMObject {
  friend class Class;
  friend class Interface;
  public:
  // ========================================================================
  /// @name Protected constructors & destructor.
  // ========================================================================
    /// @{

    /// Default constructor.
    ClassElementContainer(const std::string& name);

    /// Destructor.
    virtual ~ClassElementContainer();

    /// @}
  // ========================================================================
  /// @name Getters on the gum::ClassElement.
  // ========================================================================
    /// @{

    /// Returns the gum::DAG of this ClassElementContainer.
    virtual const DAG& dag() const =0;

    /**
     * Returns true if a member with the given id exists in this ClassElementContainer or
     * in the ClassElementContainer hierarchy.
     */
    virtual bool exists(NodeId id) const =0;

    /**
     * Returns true if a member with the given name exists in this ClassElementContainer or
     * in the ClassElementContainer hierarchy.
     */
    virtual bool exists(const std::string& name) const =0;

    /**
     * Getter on a member of this ClassElementContainer.
     * @param id The member's id.
     * @return Returns a constant reference on the member.
     * @throw NotFound Raised if no attribute matches name.
     */
    virtual ClassElement& get(NodeId id) =0;

    /**
     * Constant getter on a member of this ClassElementContainer.
     * @param id The member's id.
     * @return Returns a constant reference on the member.
     * @throw NotFound Raised if no attribute matches name.
     */
    virtual const ClassElement& get(NodeId id) const =0;

    /**
     * Getter on a member of this ClassElementContainer.
     * @param name The member's name.
     * @return Returns a constant reference on the member.
     * @throw NotFound Raised if no attribute matches name.
     */
    virtual ClassElement& get(const std::string& name) =0;

    /**
     * Constant getter on a member of this ClassElementContainer.
     * @param name The member's name.
     * @return Returns a constant reference on the member.
     * @throw NotFound Raised if no attribute matches name.
     */
    virtual const ClassElement& get(const std::string& name) const =0;

    /**
     * Add a ClassElement to this ClassElementContainer.
     *
     * @param elt The ClassElement added.
     * @return Returns the NodeId given to elt.
     */
    virtual NodeId add(ClassElement* elt) =0;

    /**
     * Overload an inherited ClassElement with elt.
     * 
     * @param elt The ClassElement overloading an inherited ClassElement
     *            sharing the same name.
     * @return Returns the NodeId given to elt.
     */
    virtual NodeId overload(ClassElement* elt) =0;

    /**
     * @brief Returns true if the node is an input node.
     *
     * Attribute or Aggregate can either be input, output, both
     * or internal nodes.
     *
     * @param id The NodeId of a ClassElement in this ClassElementContainer.
     * @return Returns true if id is an input node.
     *
     * @throw NotFound Raised if NodeId does'nt match any ClassElement in this.
     * @throw WrongClassElement Raised if id is neither an Attribute nor
     *                          an Aggregate.
     */
    bool isInputNode(NodeId id) const;

    /**
     * @brief Set the input flag value of id at b.
     *
     * Attribute or Aggregate can either be input, output, both
     * or internal nodes.
     *
     * @param id The NodeId of a ClassElement in this ClassElementContainer.
     * @param b The flag value.
     *
     * @throw NotFound Raised if id does'nt match any ClassElement in this.
     * @throw WrongClassElement Raised if NodeId is neither an Attribute nor
     *                          an Aggregate.
     */
    void setInputNode(NodeId id, bool b);

    /**
     * @brief Returns true if the node is an output node.
     *
     * Attribute or Aggregate can either be input, output, both
     * or internal nodes.
     *
     * @param id The NodeId of a ClassElement in this ClassElementContainer.
     * @return Returns true if id is an input node.
     *
     * @throw NotFound Raised if id does'nt match any ClassElement in this.
     * @throw WrongClassElement Raised if NodeId is neither an Attribute nor
     *                          an Aggregate.
     */
    bool isOutputNode(NodeId id) const;

    /**
     * @brief Set the output flag value of id at b.
     *
     * Attribute or Aggregate can either be input, output, both
     * or internal nodes.
     *
     * @param id The NodeId of a ClassElement in this ClassElementContainer.
     * @param b The flag value.
     *
     * @throw NotFound Raised if id does'nt match any ClassElement in this.
     * @throw WrongClassElement Raised if NodeId is neither an Attribute nor
     *                          an Aggregate.
     */
    void setOutputNode(NodeId id, bool b);

    /**
     * @brief Returns true if the node is an inner node.
     *
     * Attribute or Aggregate can either be input, output, both
     * or internal nodes.
     *
     * @param id The NodeId of a ClassElement in this ClassElementContainer.
     * @return Returns true if id is an input node.
     *
     * @throw NotFound Raised if NodeId does'nt match any ClassElement in this.
     * @throw WrongClassElement Raised if NodeId is neither an Attribute nor
     *                          an Aggregate.
     */
    bool isInnerNode(NodeId id) const;

    /**
     * Add an arc between two ClassElement.
     */
    virtual void insertArc(const std::string& tail, const std::string& head) =0;

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
    virtual ClassElement& operator[](NodeId id) =0;

    /**
     * Constant getter on a member of this ClassElementContainer.
     * @param id The member's id.
     * @return Returns a constant reference on the member.
     * @throw NotFound Raised if no attribute matches name.
     */
    virtual const ClassElement& operator[](NodeId id) const =0;

    /**
     * Getter on a member of this ClassElementContainer.
     * @param name The member's name.
     * @return Returns a constant reference on the member.
     * @throw NotFound Raised if no attribute matches name.
     */
    virtual ClassElement& operator[](const std::string& name) =0;

    /**
     * Constant getter on a member of this ClassElementContainer.
     * @param name The member's name.
     * @return Returns a constant reference on the member.
     * @throw NotFound Raised if no attribute matches name.
     */
    virtual const ClassElement& operator[](const std::string& name) const =0;

    /// @}
  // ========================================================================
  /// @name Inheritance getters and setters
  // ========================================================================
    /// @{

    /**
     * @brief Test if this ClassElementContainer is a subtype of cec.
     *
     * @param cec
     * @return return true if this ClassElementContainer is a subtype of cec.
     */
    virtual bool isSubTypeOf(const ClassElementContainer& cec) const =0;

    /**
     * @brief Test if this ClassElementContainer is a super type of cec.
     *
     * This returns cec.isSubTypeOf(*this).
     *
     * @param cec
     * @return return true if this ClassElementContainer is a super type of cec.
     */
    bool isSuperTypeOf(const ClassElementContainer& cec) const;

    /// @}
  protected:
    /// Copy operator. Don't use it.
    ClassElementContainer& operator=(const ClassElementContainer& source);

    /// Copy constructor. Don't use it.
    ClassElementContainer(const ClassElementContainer& source);

    /// Returns a non constant reference over this ClassElementContainer's DAG.
    virtual DAG& _dag() =0;

    /// This method finds an unused NodeId in this ClassElementContainer and all
    /// of it's subtypes (returned by ClassElementContainer::_findAllSubtypes()).
    /// The NodeId is added to this ClassElementContainer's DAG.
    ///
    /// @return The NodeId founded and added to this ClassElementContainer's DAG.
    ///
    /// @throw FatalError If no common NodeId could be found.
    NodeId _findNodeId();

    /// Fills set with all the subtypes of this Interface, this includes extensions
    /// and implementations.
    virtual void _findAllSubtypes(Set<ClassElementContainer*>& set) =0;

    /// Returns the input / output flags, useful when inheriting or copying.
    Property<std::pair<bool, bool>*>::onNodes& _IOFlags();

    /// Returns the input / output flags, useful when inheriting or copying.
    const Property<std::pair<bool, bool>*>::onNodes& _IOFlags() const;

  private:
    /// This is used to remember if a ClassElement is an input or output node.
    /// first is for the input flag, second is for the output flag.
    Property<std::pair<bool, bool>*>::onNodes __IOFlags;

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
