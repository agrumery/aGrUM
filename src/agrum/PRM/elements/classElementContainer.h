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
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_CLASS_ELEMENT_CONTAINER_H
#define GUM_CLASS_ELEMENT_CONTAINER_H

#include <agrum/core/set.h>
#include <agrum/core/hashTable.h>

#include <agrum/graphs/graphElements.h>
#include <agrum/graphs/DAG.h>

#include <agrum/PRM/utils_prm.h>

#include <agrum/PRM/elements/PRMObject.h>

namespace gum {
  namespace prm {

    template <typename GUM_SCALAR> class Class;
    template <typename GUM_SCALAR> class Interface;
    class ClassElementContainterIterator;
    class ClassElementContainterConstIterator;

    /**
     * @class ClassElementContainer classElementContainer.h
     *<agrum/PRM/classElementContainer.h>
     * @brief Abstract class for classes containing gum::ClassElement<GUM_SCALAR>.
     *
     * To print a ClassElementContainer you can use the following operator:
     * gum::operator<<(std::ostream&, const ClassElementContainer&) which print
     * the ClassElementContainer in the graphviz-dot format.
     *
     * @ingroup prm_group
     */
    template <typename GUM_SCALAR> class ClassElementContainer : public PRMObject {
      friend class ClassElementContainterIterator;
      friend class ClassElementContainterConstIterator;

      public:
      // ========================================================================
      /// @name Protected constructors & destructor.
      // ========================================================================
      /// @{

      /// Default constructor.
      ClassElementContainer(const std::string &name);

      /// Destructor.
      virtual ~ClassElementContainer();

      /// @}
      // ========================================================================
      /// @name Getters on the gum::ClassElement<GUM_SCALAR>.
      // ========================================================================
      /// @{

      /**
       * Returns true if elt belongs to this ClassElementContainer.
       * @param elt A ClassElement<GUM_SCALAR>.
       * @return true if elt beglons to this ClassElementContainer.
       */
      bool belongsTo(const ClassElement<GUM_SCALAR> &elt) const;

      /**
       * Returns true if a member with the given name exists in this
       * ClassElementContainer or
       * in the ClassElementContainer hierarchy.
       */
      bool exists(const std::string &name) const;

      /**
       * Getter on a member of this ClassElementContainer.
       * @param name The member's name.
       * @return Returns a constant reference on the member.
       * @throw NotFound Raised if no attribute matches name.
       */
      virtual ClassElement<GUM_SCALAR> &get(const std::string &name) = 0;

      /**
       * Constant getter on a member of this ClassElementContainer.
       * @param name The member's name.
       * @return Returns a constant reference on the member.
       * @throw NotFound Raised if no attribute matches name.
       */
      virtual const ClassElement<GUM_SCALAR> &get(const std::string &name) const = 0;

      /**
       * @brief Add a ClassElement<GUM_SCALAR> to this ClassElementContainer.
       *
       * The pointer is "given" to this class, which will delete it when
       * ~Class() is called.
       *
       * The NodeId of elt is defined when it is added to this, discarding any
       * previous value.
       *
       * If you want to overload an inherited ClassElement<GUM_SCALAR> call
       *Class::overload().
       *
       * When adding an Attribute or an Aggregate its type safe name is automatically
       * added, the syntax is \<type\>name. So you can either use its type safe name
       *or
       * its real name. See the \ref prm_typ_inh "type inheritance section" for
       * further details.
       *
       * @param elt The ClassElement<GUM_SCALAR> added to this Class.
       * @return Returns the NodeId assigned to elt.
       *
       * @throw DuplicateElement Raised if elt's name is already used in this class.
       */
      virtual NodeId add(ClassElement<GUM_SCALAR> *elt) = 0;

      /**
       * @brief Add a ClassElement<GUM_SCALAR> which overload an inherited
       *ClassElement<GUM_SCALAR>.
       *
       * The pointer is "given" to this class, which will delete it when
       * ~ClassElementContainer() is called.
       *
       * The NodeId of elt is defined when it is added to this, discarding any
       * previous value. There is no guaranty that elt will have the same NodeId
       * than the ClassElement<GUM_SCALAR> it overloaded.
       *
       * You can only overload inherited ClassElement<GUM_SCALAR> and only if elt is
       *a subtype
       * of the inherited ClassElement<GUM_SCALAR>. Thus you can only overload
       *ReferenceSlot and
       * Attribute. In the case of Attribute you can overload an inherited Attribute
       * even if they are of the same type: this is useful when you want to redefine
       * the dependencies of an Attribute or its CPF. You can also overload an
       * Attribute with an Aggregate, as long as their respective Type allow it.
       *
       * @param elt The ClassElement<GUM_SCALAR> overloading an inherited
       *ClassElement<GUM_SCALAR>
       *            in this ClassElementContainer.
       * @return the NodeId assigned to elt.
       *
       * @throw OperationNotAllowed Raised if there is no ClassElement<GUM_SCALAR> to
       *overload.
       * @throw WrongClassElement Raised if the overloading is illegal.
       * @throw TypeError Raised if elt isn't a legal subtype of
       *this->get(elt->name()).
       */
      virtual NodeId overload(ClassElement<GUM_SCALAR> *elt) = 0;

      /**
       * Add an arc between two ClassElement<GUM_SCALAR>.
       */
      GUM_DEPRECATED(virtual void insertArc(const std::string &tail,
                                            const std::string &head)) = 0;
      virtual void addArc(const std::string &tail, const std::string &head) = 0;

      /**
       * @brief Returns true if the node is an input node.
       *
       * Attribute or Aggregate can either be input, output, both
       * or internal nodes.
       *
       * @param elt A ClassElement<GUM_SCALAR>.
       * @return Returns true if id is an input node.
       */
      bool isInputNode(const ClassElement<GUM_SCALAR> &elt) const;

      /**
       * @brief Set the input flag value of id at b.
       *
       * Attribute or Aggregate can either be input, output, both
       * or internal nodes.
       *
       * @param elt A ClassElement<GUM_SCALAR>.
       * @param b The flag value.
       *
       * @throw NotFound Raised if id does'nt match any ClassElement<GUM_SCALAR> in
       *this.
       * @throw WrongClassElement Raised if NodeId is neither an Attribute nor
       *                          an Aggregate.
       */
      void setInputNode(const ClassElement<GUM_SCALAR> &elt, bool b);

      /**
       * @brief Returns true if the node is an output node.
       *
       * Attribute or Aggregate can either be input, output, both
       * or internal nodes.
       *
       * @param elt A ClassElement<GUM_SCALAR>.
       * @return Returns true if id is an input node.
       */
      bool isOutputNode(const ClassElement<GUM_SCALAR> &elt) const;

      /**
       * @brief Set the output flag value of id at b.
       *
       * Attribute or Aggregate can either be input, output, both
       * or internal nodes.
       *
       * @param elt A ClassElement<GUM_SCALAR>.
       * @param b The flag value.
       *
       * @throw NotFound Raised if id does'nt match any ClassElement<GUM_SCALAR> in
       *this.
       * @throw WrongClassElement Raised if NodeId is neither an Attribute nor
       *                          an Aggregate.
       */
      void setOutputNode(const ClassElement<GUM_SCALAR> &elt, bool b);

      /**
       * @brief Returns true if the node is an inner node.
       *
       * Attribute or Aggregate can either be input, output, both
       * or internal nodes.
       *
       * @param elt A ClassElement<GUM_SCALAR>.
       * @return true if elt is an input node.
       *
       * @throw NotFound Raised if NodeId does'nt match any ClassElement<GUM_SCALAR>
       *in this.
       * @throw WrongClassElement Raised if NodeId is neither an Attribute nor
       *                          an Aggregate.
       */
      bool isInnerNode(const ClassElement<GUM_SCALAR> &elt) const;

      /// @}
      // ========================================================================
      /// @name Graphical operator
      // ========================================================================
      /// @{

      /**
       * @brief Returns the gum::DAG of this ClassElementContainer.
       *
       * Be very careful when using NodeId with ClassElement<GUM_SCALAR>: there is no
       * guarantee that an inherited ClassElement<GUM_SCALAR> will have the same
       *NodeId
       * than its superclass counterpart.
       *
       * When dealing with different classes and interfaces ALWAYS use safe-name
       * as identifier.
       *
       * @return the DAG of this ClassElementContainer.
       */
      const DAG &dag() const;

      /**
       * Returns true if a member with the given id exists in this
       * ClassElementContainer or
       * in the ClassElementContainer hierarchy.
       * @param id A NodeId.
       * @return true if id matches a ClassElement<GUM_SCALAR>.
       */
      bool exists(NodeId id) const;

      /**
       * Getter on a member of this ClassElementContainer.
       * @param id The member's id.
       * @return Returns a constant reference on the member.
       * @throw NotFound Raised if no attribute matches name.
       */
      virtual ClassElement<GUM_SCALAR> &get(NodeId id) = 0;

      /**
       * Constant getter on a member of this ClassElementContainer.
       * @param id The member's id.
       * @return Returns a constant reference on the member.
       * @throw NotFound Raised if no attribute matches name.
       */
      virtual const ClassElement<GUM_SCALAR> &get(NodeId id) const = 0;

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
      virtual ClassElement<GUM_SCALAR> &operator[](NodeId id) = 0;

      /**
       * Constant getter on a member of this ClassElementContainer.
       * @param id The member's id.
       * @return Returns a constant reference on the member.
       * @throw NotFound Raised if no attribute matches name.
       */
      virtual const ClassElement<GUM_SCALAR> &operator[](NodeId id) const = 0;

      /**
       * Getter on a member of this ClassElementContainer.
       * @param name The member's name.
       * @return Returns a constant reference on the member.
       * @throw NotFound Raised if no attribute matches name.
       */
      virtual ClassElement<GUM_SCALAR> &operator[](const std::string &name) = 0;

      /**
       * Constant getter on a member of this ClassElementContainer.
       * @param name The member's name.
       * @return Returns a constant reference on the member.
       * @throw NotFound Raised if no attribute matches name.
       */
      virtual const ClassElement<GUM_SCALAR> &
      operator[](const std::string &name) const = 0;

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
      virtual bool isSubTypeOf(const ClassElementContainer &cec) const = 0;

      /**
       * @brief Test if this ClassElementContainer is a super type of cec.
       *
       * This returns cec.isSubTypeOf(*this).
       *
       * @param cec
       * @return return true if this ClassElementContainer is a super type of cec.
       */
      bool isSuperTypeOf(const ClassElementContainer &cec) const;

      /// @}

      protected:
      /// Copy operator. Don't use it.
      ClassElementContainer &operator=(const ClassElementContainer &source);

      /// Copy constructor. Don't use it.
      ClassElementContainer(const ClassElementContainer &source);

      virtual const DAG &_dag() const = 0;

      /// Returns a non constant reference over this ClassElementContainer's DAG.
      virtual DAG &_dag() = 0;

      /// Fills set with all the subtypes of this Interface, this includes extensions
      /// and implementations.
      virtual void _findAllSubtypes(Set<ClassElementContainer *> &set) = 0;

      /// Returns the IO flags of a ClassElement<GUM_SCALAR>.
      /// @param elt The ClassElement<GUM_SCALAR>.
      /// @return elt's IO flags.
      /// @throw NotFound Raised if elt does not have any IO flags.
      std::pair<bool, bool> &_getIOFlag(const ClassElement<GUM_SCALAR> &elt);

      /// Returns the IO flags of a ClassElement<GUM_SCALAR>.
      /// @param elt The ClassElement<GUM_SCALAR>.
      /// @return elt's IO flags.
      /// @throw NotFound Raised if elt does not have any IO flags.
      const std::pair<bool, bool> &
      _getIOFlag(const ClassElement<GUM_SCALAR> &elt) const;

      /// Defines the IO flags of a ClassElement<GUM_SCALAR>.
      /// @param elt The ClassElement<GUM_SCALAR>.
      /// @param flags The IO flags of elt. Overwrite any existing flags.
      /// @return elt's IO flags.
      /// @throw NotFound Raised if elt does not have any IO flags.
      void _setIOFlag(const ClassElement<GUM_SCALAR> &elt,
                      const std::pair<bool, bool> &flags);

      /// Copy the IO Flags of c in this ClassElementContainer.
      /// @param c A ClassElementContainer.
      void _copyIOFlags(const ClassElementContainer &c);

      /// When a ClassElement<GUM_SCALAR> becomes an Output node we must update any
      /// the IO
      /// flags of every descendant of this ClassElementContainer.
      /// Note that after its declaration, input flags can not be changed and
      /// output flags can only become true.
      /// @param elt A ClassElement<GUM_SCALAR>.
      virtual void _updateDescendants(const ClassElement<GUM_SCALAR> &elt) = 0;

      private:
      /// input / output flags, useful when inheriting or copying.
      HashTable<std::string, std::pair<bool, bool>> __IOFlags;
    };

    extern template class ClassElementContainer<double>;

  } /* namespace prm */
} // namespace gum

/// @brief An << operator for ClassElementContainer.
/// Output in the graphviz-dot format.
template <typename GUM_SCALAR>
std::ostream &
operator<<(std::ostream &output,
           const gum::prm::ClassElementContainer<GUM_SCALAR> &container);

#include <agrum/PRM/elements/classElementContainer.tcc>

#endif /* GUM_CLASS_ELEMENT_CONTAINER_H */
