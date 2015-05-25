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
 * @brief Headers of gum::prm::Class<GUM_SCALAR>.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_CLASS_H
#define GUM_CLASS_H

#include <utility>
#include <set>

#include <agrum/core/sequence.h>
#include <agrum/core/bijection.h>
#include <agrum/multidim/multiDimSparse.h>
#include <agrum/PRM/elements/classElementContainer.h>

namespace gum {
  namespace prm {

    /**
     * @class Class<GUM_SCALAR> class.h <agrum/PRM/class.h>
     * @brief A Class<GUM_SCALAR> is an object of a PRM representing a fragment of a
     *Bayesian
     *        Network which can be instantiated in Instance.
     *
     *
     * @see PRM Class<GUM_SCALAR> ClassElement<GUM_SCALAR>
     * @ingroup prm_group
     */
    template <typename GUM_SCALAR>
    class Class : public ClassElementContainer<GUM_SCALAR> {

      friend class Interface<GUM_SCALAR>;

      public:
      /// @name Constructors & destructor.
      /// @{

      /**
       * Default constructor.
       * @param name The class name.
       */
      Class(const std::string &name);

      /**
       * Constructor for building a subclass of super.
       * @param name The subclass name.
       * @param super The super Class<GUM_SCALAR> of this.
       */
      Class(const std::string &name, Class<GUM_SCALAR> &super);

      /**
       * Constructor for building a Class<GUM_SCALAR> implementing several each
       * interface
       * in set.
       * @param name The sub class name.
       * @param set The Set of implemented interfaces.
       */
      Class(const std::string &name, const Set<Interface<GUM_SCALAR> *> &set);

      /**
       * Constructor for building a subclass of super and implementing each
       * interface in set.
       * @param name The sub class name.
       * @param super The super Class<GUM_SCALAR> of this.
       * @param set The Set of implemented interfaces.
       */
      Class(const std::string &name, Class<GUM_SCALAR> &super,
            const Set<Interface<GUM_SCALAR> *> &set);

      /// Copy constructor.
      Class(const Class<GUM_SCALAR> &source);

      /// Destructor.
      virtual ~Class();

      /// Implementation of pure virtual method of PRMObject.
      virtual PRMObject::PRMType obj_type() const;
      /// @}
      // ========================================================================
      /// @name Graphical operator
      // ========================================================================
      /// @{

      /// See gum::prm::ClassElementContainer<GUM_SCALAR>::get(NodeId).
      ClassElement<GUM_SCALAR> &get(NodeId id);

      /// See gum::prm::ClassElementContainer<GUM_SCALAR>::get(NodeId).
      const ClassElement<GUM_SCALAR> &get(NodeId id) const;

      /// @}
      // ========================================================================
      /// @name ClassElement<GUM_SCALAR> getters and setters
      // ========================================================================
      /// @{

      // /**
      //  * @brief returns the state of a parameter.
      //  *
      //  * Parameters are Attribute<GUM_SCALAR> without any dependencies and for
      //  which a value
      //  * (i.e. observation) must be assigned at instantiation if no default
      //  * value is defined.
      //  *
      //  * Default values are assigned using the parameter's CPF. If there is
      //  * no default values, it's CPF is filled with invasive nan (i.e. Not A
      //  * Number).
      //  *
      //  * @param elt A ClassElement<GUM_SCALAR>.
      //  * @return Returns 0 if n is not a parameter, 1 if it is a
      //  *         parameter and 2 if it haves a default value.
      //  */
      /**
       * @brief returns the state of a parameter.
       *
       * Parameters are Attribute<GUM_SCALAR> without any dependencies and for which
       *a value
       * (i.e. observation) must be assigned at instantiation if no default
       * value is defined.
       *
       * Default values are assigned using the parameter's CPF. If there is
       * no default values, it's CPF is filled with invasive nan (i.e. Not A
       * Number).
       *
       * @param elt A ClassElement<GUM_SCALAR>.
       * @return true if elt is a parameter.
       */
      bool isParameter(const ClassElement<GUM_SCALAR> &elt) const;

      /**
       * @brief Return true if the attribute named safe_name is a cast descendant.
       * Cast descendant are automatically added Attribute<GUM_SCALAR> for type
       * casting.
       * @param safe_name The safe name of an Attribute<GUM_SCALAR> of this class.
       * @return true if safe_name is a cast descendant.
       * @throw NotFound Raised if safe_name does not name an Attribute<GUM_SCALAR>
       * in this Class<GUM_SCALAR>.
       */
      bool isCastDescendant(const std::string &safe_name) const;

      /**
       * @brief Add a parameter to this Class<GUM_SCALAR>.
       *
       * Parameters are Attribute<GUM_SCALAR> without any dependencies and for which
       *a value
       * (i.e. observation) must be assigned at instantiation if no default
       * value is defined.
       *
       * Default values are assigned using the parameter's CPF. If there is
       * no default values, it's CPF is filled with invasive nan (i.e. Not A
       * Number).
       *
       * @param param The Attribute<GUM_SCALAR> added as a Parameter of this
       *Class<GUM_SCALAR>.
       * @param flag If true the initialization flag indicated that the given
       *             parameter is already initialized and does not require
       *             initialization at instantiation.
       * @return Returns the NodeId assigned to the added parameter.
       *
       * @throw DuplicateElement Raised if elt's name is already used in this class.
       */
      NodeId addParameter(Attribute<GUM_SCALAR> *param, bool flag);

      /// See gum::prm::ClassElementContainer<GUM_SCALAR>::get(const std::string&).
      ClassElement<GUM_SCALAR> &get(const std::string &name);

      /// See gum::prm::ClassElementContainer<GUM_SCALAR>::get(const std::string&).
      const ClassElement<GUM_SCALAR> &get(const std::string &name) const;

      /// See gum::prm::add(ClassElement<GUM_SCALAR>*).
      virtual NodeId add(ClassElement<GUM_SCALAR> *elt);

      /// See gum::prm::overload(ClassElement<GUM_SCALAR>*).
      virtual NodeId overload(ClassElement<GUM_SCALAR> *elt);

      /// See gum::prm::ClassElementContainer<GUM_SCALAR>::insertArc().
      GUM_DEPRECATED(void insertArc(const std::string &tail,
                                    const std::string &head));
      virtual void addArc(const std::string &tail, const std::string &head);

      /**
       * Returns the set of Attribute<GUM_SCALAR> of this Class<GUM_SCALAR>.
       * @return Returns the set of Attribute<GUM_SCALAR> of this Class<GUM_SCALAR>.
       */
      const Set<Attribute<GUM_SCALAR> *> &attributes() const;

      /**
       * Returns the set of parameters of this Class<GUM_SCALAR>.
       * @return Returns the set of parameters of this Class<GUM_SCALAR>.
       */
      const Set<Attribute<GUM_SCALAR> *> &parameters() const;

      /**
       * Returns the set of Aggregate<GUM_SCALAR> of this Class<GUM_SCALAR>.
       * @return Returns the set of Aggregate<GUM_SCALAR> of this Class<GUM_SCALAR>.
       */
      const Set<Aggregate<GUM_SCALAR> *> &aggregates() const;

      /**
       * Returns the set of ReferenceSlot<GUM_SCALAR> of this Class<GUM_SCALAR>.
       * @return Returns the set of ReferenceSlot<GUM_SCALAR> of this
       * Class<GUM_SCALAR>.
       */
      const Set<ReferenceSlot<GUM_SCALAR> *> &referenceSlots() const;

      /**
       * Returns the set of SlotChain<GUM_SCALAR> of this Class<GUM_SCALAR>.
       * @return Returns the set of SlotChain<GUM_SCALAR> of this Class<GUM_SCALAR>.
       */
      const Set<SlotChain<GUM_SCALAR> *> &slotChains() const;

      /**
       * Returns the Set of ClassElement<GUM_SCALAR>'s which must be instantiated
       * at Instance level.
       * @return the Set of ClassElement<GUM_SCALAR>'s which must be instantiated
       *        at Instance level.
       */
      const Sequence<NodeId> &toInstantiate() const;

      //    /**
      //     * @brief Remove a ClassElement<GUM_SCALAR> from this Class<GUM_SCALAR>.
      //     *
      //     * When removing a ClassElement<GUM_SCALAR> it will not be deleted by
      //     this Class<GUM_SCALAR>, thus
      //     * you should delete it yourself.
      //     *
      //     * All dependencies among the removed ClassElement<GUM_SCALAR> and
      //     ClassElement<GUM_SCALAR> defined
      //     * in and outside of this class are deleted also. You must update the
      //     * corresponding CPF yourself.
      //     *
      //     * Futhermore if there exists Instance of this Class<GUM_SCALAR> you
      //     should be very
      //     * careful at what you are doing (for instance do not delete the
      //     * ClassElement<GUM_SCALAR> before deleting the concerned Instance).
      //     *
      //     * @param id The ClassElement<GUM_SCALAR>'s NodeId.
      //     * @return the pointer over the removed ClassElement<GUM_SCALAR>.
      //     *
      //     * @throw NotFound If no ClassElement<GUM_SCALAR> matches id.
      //     */
      //    ClassElement<GUM_SCALAR>* remove(NodeId id);

      /// @}
      // ========================================================================
      /// @name Inheritance methods
      // ========================================================================
      /// @{

      /**
       * @brief Test if this Class<GUM_SCALAR> is a subclass of cec.
       *
       * If cec is a Class<GUM_SCALAR> then this Class<GUM_SCALAR> is a subclass of
       *cec if they are
       * equal or there exists a superclass of this Class<GUM_SCALAR> which is equal
       *to cec.
       *
       * If cec is an Interface<GUM_SCALAR> then this Class<GUM_SCALAR> is a subclass
       *of cec if it
       * implements cec or if there exists a superclass of this Class<GUM_SCALAR>
       *which
       * implements cec.
       *
       * @param cec The ClassElementContainer<GUM_SCALAR> for which we determine if
       *this
       *            Class<GUM_SCALAR> is a subclass of it.
       * @return Returns true if this Class<GUM_SCALAR> is a subclass of cec.
       */
      virtual bool isSubTypeOf(const ClassElementContainer<GUM_SCALAR> &cec) const;

      /**
       * @brief Returns the super Class<GUM_SCALAR> of this Class<GUM_SCALAR>.
       * @return Returns the super Class<GUM_SCALAR> of this Class<GUM_SCALAR>.
       * @throw NotFound Raised if this has no super Class<GUM_SCALAR>.
       */
      const Class<GUM_SCALAR> &super() const;

      /**
       * @brief Returns the Set of Interface<GUM_SCALAR> implemented by this
       * Class<GUM_SCALAR>.
       * @return Returns the Set of Interface<GUM_SCALAR> implemented by this
       * Class<GUM_SCALAR>.
       * @throw NotFound Raised if this Class<GUM_SCALAR> doesn't implement any
       * Interface<GUM_SCALAR>.
       */
      const Set<Interface<GUM_SCALAR> *> &implements() const;

      /// Returns the set of Class<GUM_SCALAR> which are direct sub-Class<GUM_SCALAR>
      /// of this Class<GUM_SCALAR>.
      const Set<Class<GUM_SCALAR> *> &extensions() const;

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
      protected:
      /// returns a constant reference over this interface's dag.
      virtual const DAG &_dag() const;

      /// Returns a non constant reference over this Interface<GUM_SCALAR>'s DAG.
      virtual DAG &_dag();

      /// Fills set with all the subtypes of this Class<GUM_SCALAR>.
      void _findAllSubtypes(Set<ClassElementContainer<GUM_SCALAR> *> &set);

      /// See gum::prm::ClassElementContainer<GUM_SCALAR>(const
      /// ClassElement<GUM_SCALAR>&).
      void _updateDescendants(const ClassElement<GUM_SCALAR> &elt);

      private:
      /// Copy operator. Don't use it.
      Class<GUM_SCALAR> &operator=(const Class<GUM_SCALAR> &source);

      /// Proceed with the copy of c in this.
      // void __copyClass<GUM_SCALAR>(const Class<GUM_SCALAR>& c);

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

      /// The sequence of aggregate.
      Set<Aggregate<GUM_SCALAR> *> __aggregates;

      /// The set of gum::SlotChain<GUM_SCALAR>s
      Set<SlotChain<GUM_SCALAR> *> __slotChains;

      /// Recursively adds cast descendant of attr in this Class<GUM_SCALAR>.
      void __addCastDescendants(Attribute<GUM_SCALAR> *attr);

      /// Recursively adds cast descendant from start to end in this
      /// Class<GUM_SCALAR>.
      void __addCastDescendants(Attribute<GUM_SCALAR> *start,
                                Attribute<GUM_SCALAR> *end);

      /// When an attribute or an aggregate is overloaded it's inherited counterpar
      /// is deleted. However we need to keep traof it's Type pointer. This method
      /// swaps the Type's between overloader and overloaded and ends by deleting
      /// overloaded.
      void __swap_types(ClassElement<GUM_SCALAR> *overloader,
                        ClassElement<GUM_SCALAR> *overloaded);

      /// @}
      // ========================================================================
      /// @name Inheritance members
      // ========================================================================
      /// @{

      /// @brief The alternate ClassElementContainer<GUM_SCALAR> searched for
      /// elements defined in
      ///        this.
      /// Note that this is first searched for gum::ClassElement<GUM_SCALAR>.
      Class<GUM_SCALAR> *__super;

      /// The Set of implemented interface of this.
      Set<Interface<GUM_SCALAR> *> *__implements;

      /// The set of Class<GUM_SCALAR> which are extension of this Class<GUM_SCALAR>
      /// (i.e. direct
      /// subtypes).
      Set<Class<GUM_SCALAR> *> __extensions;

      /// Proceed with the copy when this inherits c.
      void __inheritClass(const Class<GUM_SCALAR> &c);

      /// Check if elt is present in an implementation. If it is, its IO flags
      /// are updated.
      void __addIOInterfaceFlags(ClassElement<GUM_SCALAR> *elt);

      /// This method is called when a sub-Class<GUM_SCALAR> of this
      /// Class<GUM_SCALAR> is created.
      /// @param c The Class<GUM_SCALAR> added as a direct sub-Class<GUM_SCALAR> of
      /// this.
      /// @throw DuplicateElement Raised if c is already a sub-Class<GUM_SCALAR> of
      /// this.
      void __addExtension(Class<GUM_SCALAR> *c);

      /// Throws something if overload is illegal.
      void __checkOverloadLegality(const ClassElement<GUM_SCALAR> *overloaded,
                                   const ClassElement<GUM_SCALAR> *overloader);

      /// Overloads an attribute.
      void __overloadAttribute(Attribute<GUM_SCALAR> *overloader,
                               Attribute<GUM_SCALAR> *overloaded);

      /// Overloads an aggregate.
      void __overloadAggregate(Aggregate<GUM_SCALAR> *overloader,
                               ClassElement<GUM_SCALAR> *overloaded);

      /// Overloads a reference slot.
      void __overloadReference(ReferenceSlot<GUM_SCALAR> *overloader,
                               ReferenceSlot<GUM_SCALAR> *overloaded);

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

      /// The Set of parameters in this Class<GUM_SCALAR>.
      Set<Attribute<GUM_SCALAR> *> __parameters;

      /// The HashTable of parameters in this.
      /// The value correspond to the initialization flag.
      HashTable<const Attribute<GUM_SCALAR> *, bool> __paramValueFlags;

      /// @}
    };

    extern template class Class<double>;
  } /* namespace prm */
} // namespace gum

#include <agrum/PRM/elements/class.tcc>

#endif /* GUM_CLASS_H */
