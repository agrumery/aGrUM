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
 * @brief Headers of gum::prm::Interface.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_INTERFACE_H
#define GUM_INTERFACE_H

#include <utility>
#include <set>

#include <agrum/core/set.h>
#include <agrum/core/sequence.h>

#include <agrum/PRM/elements/classElementContainer.h>

namespace gum {
  namespace prm {

    /**
     * @class Interface interface.h <agrum/PRM/interface.h>
     * @brief An Interface is implemented by a Class<GUM_SCALAR> and defines a set of
     *        ReferenceSlot<GUM_SCALAR> and Attribute<GUM_SCALAR> which the
     *implementing Class<GUM_SCALAR> must contain.
     *
     * @see PRM Class<GUM_SCALAR> ClassElement<GUM_SCALAR>
     */
    template <typename GUM_SCALAR>
    class Interface : public ClassElementContainer<GUM_SCALAR> {
      // ========================================================================
      friend class Class<GUM_SCALAR>;
      // ========================================================================
      public:
      // ========================================================================
      /// @name Constructor & destructor.
      // ========================================================================
      /// @{

      /**
       * Default constructor.
       * @param name The interface name.
       */
      Interface(const std::string &name);

      /**
       * Constructor for building a subclass of super.
       * @param name The sub-interface name.
       * @param super The super Interface of this.
       */
      Interface(const std::string &name, Interface &super);

      /// Copy constructor.
      Interface(const Interface &source);

      /// Destructor.
      virtual ~Interface();

      /// @}
      // ========================================================================
      /// @name Graphical model getters and setters
      // ========================================================================
      /// @{

      /// Implementation of pure virtual method of PRMObject.
      virtual typename PRMObject::PRMType obj_type() const;

      /// See gum::prm::ClassElementContainer<GUM_SCALAR>::get(NodeId).
      virtual ClassElement<GUM_SCALAR> &get(NodeId id);

      /// Se gum::prm::ClassElementContainer<GUM_SCALAR>::get(NodeId).
      virtual const ClassElement<GUM_SCALAR> &get(NodeId id) const;

      /**
       * @brief An Interfance doesn't have any arc, this will raise an
       *        OperationNotAllowed exception.
       *
       * @param tail The tail of the added arc.
       * @param head The head of the added arc.
       * @throw OperationNotAllowed Systematically throws an OperationNotAllowed.
       */
      GUM_DEPRECATED(void insertArc(const std::string &tail,
                                    const std::string &head));
      void addArc(const std::string &tail, const std::string &head);

      /// @}
      // ========================================================================
      /// @name ClassElement<GUM_SCALAR> getters and setters
      // ========================================================================
      /// @{

      /// See gum::prm::ClassElementContainer<GUM_SCALAR>::get(const std::string&).
      virtual ClassElement<GUM_SCALAR> &get(const std::string &name);

      /// See gum::prm::ClassElementContainer<GUM_SCALAR>::get(const std::string&).
      virtual const ClassElement<GUM_SCALAR> &get(const std::string &name) const;

      /**
       * Returns the set of Attribute<GUM_SCALAR> of this Class<GUM_SCALAR>.
       * @return Returns the set of Attribute<GUM_SCALAR> of this Class<GUM_SCALAR>.
       */
      const Set<Attribute<GUM_SCALAR> *> &attributes() const;

      /**
       * Returns the set of Aggregate of this Class<GUM_SCALAR>.
       * @return Returns the set of Aggregate of this Class<GUM_SCALAR>.
       */
      const Set<ReferenceSlot<GUM_SCALAR> *> &referenceSlots() const;

      /// See
      /// gum::prm::ClassElementContainer<GUM_SCALAR>::add(ClassElement<GUM_SCALAR>*).
      NodeId add(ClassElement<GUM_SCALAR> *elt);

      /**
       * @brief Add a new ClassElement<GUM_SCALAR> which overload an inherited
       *ClassElement<GUM_SCALAR>.
       *
       * The pointer is "given" to this class, which will delete it when
       * ~Class<GUM_SCALAR>() is called.
       *
       * The NodeId of elt is defined when it is added to this, discarding any
       * previous value. There is no garanty that elt will have the same NodeId
       * than the ClassElement<GUM_SCALAR> it overloaded.
       *
       * You can only overload inherited ClassElement<GUM_SCALAR> and only if elt is
       *a subtype
       * of the inherited ClassElement<GUM_SCALAR>. You do not define dependencies in
       *an Interface
       * so it is useless to overload an Attribute<GUM_SCALAR> with another sharing
       *the same type,
       * and if tried it will raise an OperationNotAllowed exception.
       *
       * @param elt The new ClassElement<GUM_SCALAR> overloading an inherited
       *ClassElement<GUM_SCALAR> in this.
       * @return the NodeId assigned to elt.
       * @throw NotFound Raised if no overloaded ClassElement<GUM_SCALAR> is found.
       * @throw OperationNotAllowed Raised if the overloading is impossible.
       */
      NodeId overload(ClassElement<GUM_SCALAR> *elt);

      // /**
      //  * @brief Remove a ClassElement<GUM_SCALAR> from this Class<GUM_SCALAR>.
      //  *
      //  * When removing a ClassElement<GUM_SCALAR> it will not be deleted by this
      //  Class<GUM_SCALAR>, thus
      //  * you should delete it yourself.
      //  *
      //  * All dependencies among the removed ClassElement<GUM_SCALAR> and
      //  ClassElement<GUM_SCALAR> defined
      //  * in and outside of this class are deleted also. You must update the
      //  * corresponding CPF yourself.
      //  *
      //  * Futhermore if there exists Instance of this Class<GUM_SCALAR> you should
      //  be very
      //  * careful at what you are doing (for instance do not delete the
      //  * ClassElement<GUM_SCALAR> before deleting the concerned Instance).
      //  *
      //  * @param id The ClassElement<GUM_SCALAR>'s NodeId.
      //  * @return the pointer over the removed ClassElement<GUM_SCALAR>.
      //  * @throw NotFound If no ClassElement<GUM_SCALAR> matches id.
      //  */
      // ClassElement<GUM_SCALAR>* remove(NodeId id);

      /// @}
      // ========================================================================
      /// @name Inheritance methods
      // ========================================================================
      /// @{

      /**
       * @brief Test if this Interface is a sub Interface of cec.
       *
       * Interface can not be a sub Interface of a Class<GUM_SCALAR>, so if cec is a
       *Class<GUM_SCALAR>
       * this method will return false.
       *
       * If cec is an Interface then this Interface is a sub Interface of cec if they
       *are
       * equal or there exists a super Interface of this Interface which is equal to
       *cec.
       *
       * @param cec The ClassElementContainer<GUM_SCALAR> for which we determine if
       *this
       *            Interface is a sub Interface of it.
       * @return Returns true if this Class<GUM_SCALAR> is a subclass of cec.
       */
      virtual bool isSubTypeOf(const ClassElementContainer<GUM_SCALAR> &cec) const;

      /**
       * @brief Returns the superInterface of this Interface.
       * @return Returns the super Interface of this Interface.
       * @throw NotFound Raised if this has no super Interface.
       */
      Interface &super();

      /**
       * @brief Returns the superInterface of this Interface.
       * @return Returns the super Interface of this Interface.
       * @throw NotFound Raised if this has no super Interface.
       */
      const Interface &super() const;

      /**
       * Returns the set of Class<GUM_SCALAR> implementing this Interface.
       */
      Set<Class<GUM_SCALAR> *> &implementations();

      /**
       * Returns the set of Class<GUM_SCALAR> implementing this Interface.
       */
      const Set<Class<GUM_SCALAR> *> &implementations() const;

      /// @}
      // ========================================================================
      /// @name Getters & setters operators
      // ========================================================================
      /// @{

      /// See gum::prm::ClassElementContainer<GUM_SCALAR>::operator[](NodeId).
      ClassElement<GUM_SCALAR> &operator[](NodeId id);

      /// See gum::prm::ClassElementContainer<GUM_SCALAR>::operator[](NodeId).
      const ClassElement<GUM_SCALAR> &operator[](NodeId id) const;

      /// See gum::prm::ClassElementContainer<GUM_SCALAR>::operator[](const
      /// std::string&).
      ClassElement<GUM_SCALAR> &operator[](const std::string &name);

      /// See gum::prm::ClassElementContainer<GUM_SCALAR>::operator[](const
      /// std::string&).
      const ClassElement<GUM_SCALAR> &operator[](const std::string &name) const;

      /// @}
      // ========================================================================
      /// @name Iterators
      // ========================================================================
      /// @{

      typedef typename NodeProperty<ClassElement<GUM_SCALAR> *>::iterator
          ClassEltIterator;
      ClassEltIterator begin();
      const ClassEltIterator &end();

      typedef typename NodeProperty<ClassElement<GUM_SCALAR> *>::const_iterator
          const_ClassEltIterator;
      const_ClassEltIterator begin() const;
      const const_ClassEltIterator &end() const;

      /// @}
      protected:
      /// Returns a constant reference over this Interface's DAG.
      const DAG &_dag() const;

      /// Returns a non constant reference over this Interface's DAG.
      DAG &_dag();

      /// Fills set with all the subtypes of this Interface, this includes extensions
      /// and implementations.
      void _findAllSubtypes(Set<ClassElementContainer<GUM_SCALAR> *> &set);

      /// See gum::prm::ClassElementContainer<GUM_SCALAR>(const
      /// ClassElement<GUM_SCALAR>&).
      void _updateDescendants(const ClassElement<GUM_SCALAR> &elt);

      private:
      /// Copy operator. Don't use it.
      Interface &operator=(const Interface &source);

      /// Proceed with the copy of i in this.
      void __inheritInterface(const Interface &i);

      // ========================================================================
      /// @name Graphical model members
      // ========================================================================
      /// @{

      /// The dag representing dependencies between formal attributes and
      /// slots.
      DAG __dag;

      /// Mapping between node's id and their name (being an attribute or a
      /// slot). Used for fast access to a member given it's node id.
      NodeProperty<ClassElement<GUM_SCALAR> *> __nodeIdMap;

      /// @}
      // ========================================================================
      /// @name ClassElement<GUM_SCALAR> members
      // ========================================================================
      /// @{

      /// Mapping between a member's name and itself.
      /// Used for fast access to a member given it's name.
      HashTable<std::string, ClassElement<GUM_SCALAR> *> __nameMap;

      /// The sequence of Attribute<GUM_SCALAR>s.
      Set<Attribute<GUM_SCALAR> *> __attributes;

      /// The sequence of ReferenceSlot<GUM_SCALAR>.
      Set<ReferenceSlot<GUM_SCALAR> *> __referenceSlots;

      /// @}
      // ========================================================================
      /// @name Inheritance members
      // ========================================================================
      /// @{

      /// @brief The alternate ClassElementContainer<GUM_SCALAR> searched for
      /// elements defined in
      ///        this.
      /// Note that this is first searched for gum::ClassElement<GUM_SCALAR>.
      Interface *__super;

      /// The set of Class<GUM_SCALAR> which implements this Interface.
      Set<Class<GUM_SCALAR> *> __implementations;

      /// Add an Class<GUM_SCALAR> to the set of Class<GUM_SCALAR> which implements
      /// this Interface.
      void __addImplementation(Class<GUM_SCALAR> *c);

      /// The set of Class<GUM_SCALAR> which implements this Interface.
      Set<Interface *> __extensions;

      /// Add an Class<GUM_SCALAR> to the set of Class<GUM_SCALAR> which implements
      /// this Interface.
      void __addExtension(Interface *c);

      void __checkOverloadLegality(const ClassElement<GUM_SCALAR> *overloaded,
                                   const ClassElement<GUM_SCALAR> *overloader);

      void __overloadAttribute(Attribute<GUM_SCALAR> *overloader,
                               Attribute<GUM_SCALAR> *overloaded);

      void __addCastDescendants(Attribute<GUM_SCALAR> *start,
                                Attribute<GUM_SCALAR> *end);

      void __swap_types(Attribute<GUM_SCALAR> *overloader,
                        Attribute<GUM_SCALAR> *overloaded);

      /// @}
    };

    extern template class Interface<double>;

  } /* namespace prm */
} // namespace gum

#include <agrum/PRM/elements/interface.tcc>

#endif /* GUM_INTERFACE_H */
