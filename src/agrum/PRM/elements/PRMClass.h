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

#include <set>
#include <utility>

#include <agrum/PRM/elements/PRMAggregate.h>
#include <agrum/PRM/elements/PRMAttribute.h>
#include <agrum/PRM/elements/PRMClassElementContainer.h>
#include <agrum/PRM/elements/PRMParameter.h>
#include <agrum/PRM/elements/PRMReferenceSlot.h>
#include <agrum/PRM/elements/PRMSlotChain.h>
#include <agrum/core/bijection.h>
#include <agrum/core/sequence.h>
#include <agrum/core/set.h>
#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/multidim/multiDimSparse.h>

namespace gum {
  namespace prm {

    /**
     * @class Class<GUM_SCALAR> class.h <agrum/PRM/class.h>
     * @brief A Class<GUM_SCALAR> is an object of a PRM representing a fragment
     *of a
     *        Bayesian Network which can be instantiated in PRMInstance.
     *
     * @see PRM Class<GUM_SCALAR> PRMClassElement<GUM_SCALAR>
     * @ingroup prm_group
     */
    template <typename GUM_SCALAR>
    class PRMClass : public PRMClassElementContainer<GUM_SCALAR> {

      friend class PRMInterface<GUM_SCALAR>;

      public:
      /// @name Constructors & destructor.
      /// @{

      /**
       * Default constructor.
       * @param name The class name.
       */
      PRMClass( const std::string& name );

      /**
       * Constructor for building a subclass of super.
       * @param name The subclass name.
       * @param super The super Class<GUM_SCALAR> of this.
       * @param delayInheritance If true, inheritance will be delayed.
       */
      PRMClass( const std::string&    name,
                PRMClass<GUM_SCALAR>& super,
                bool                  delayInheritance = false );

      /**
       * Constructor for building a Class<GUM_SCALAR> implementing several each
       * interface
       * in set.
       * @param name The sub class name.
       * @param set The Set of implemented interfaces.
       * @param delayInheritance If true, inheritance will be delayed.
       */
      PRMClass( const std::string&                    name,
                const Set<PRMInterface<GUM_SCALAR>*>& set,
                bool                                  delayInheritance = false );

      /**
       * Constructor for building a subclass of super and implementing each
       * interface in set.
       * @param name The sub class name.
       * @param super The super Class<GUM_SCALAR> of this.
       * @param set The Set of implemented interfaces.
       * @param delayInheritance If true, inheritance will be delayed.
       */
      PRMClass( const std::string&                    name,
                PRMClass<GUM_SCALAR>&                 super,
                const Set<PRMInterface<GUM_SCALAR>*>& set,
                bool                                  delayInheritance = false );

      /// Copy constructor.
      PRMClass( const PRMClass<GUM_SCALAR>& source ) = delete;

      /// Move constructor.
      PRMClass( const PRMClass<GUM_SCALAR>&& source ) = delete;

      /// Copy operator. Don't use it.
      PRMClass<GUM_SCALAR>&
      operator=( const PRMClass<GUM_SCALAR>& source ) = delete;

      /// Move operator. Don't use it.
      PRMClass<GUM_SCALAR>&
      operator=( const PRMClass<GUM_SCALAR>&& source ) = delete;

      /// Destructor.
      virtual ~PRMClass();

      /// Implementation of pure virtual method of PRMObject.
      virtual PRMObject::prm_type obj_type() const;
      /// @}
      // ========================================================================
      /// @name Graphical operator
      // ========================================================================
      /// @{

      /// See gum::prm::PRMClassElementContainer<GUM_SCALAR>::get(NodeId).
      PRMClassElement<GUM_SCALAR>& get( NodeId id );

      /// See gum::prm::PRMClassElementContainer<GUM_SCALAR>::get(NodeId).
      const PRMClassElement<GUM_SCALAR>& get( NodeId id ) const;

      /// @}
      // ========================================================================
      /// @name PRMClassElement<GUM_SCALAR> getters and setters
      // ========================================================================
      /// @{

      /**
       * @brief Returns true if elt is an output node.
       * @param elt A node of this class.
       * @return Returns true if elt is an output node.
       */
      virtual bool isOutputNode( const PRMClassElement<GUM_SCALAR>& elt ) const;

      /**
       * @brief Return true if the attribute named safe_name is a cast
       * descendant.
       * Cast descendant are automatically added PRMAttribute<GUM_SCALAR> for
       * type
       * casting.
       * @param safe_name The safe name of an PRMAttribute<GUM_SCALAR> of this
       * class.
       * @return true if safe_name is a cast descendant.
       * @throw NotFound Raised if safe_name does not name an
       * PRMAttribute<GUM_SCALAR> in this Class<GUM_SCALAR>.
       */
      bool isCastDescendant( const std::string& safe_name ) const;

      /// See gum::prm::PRMClassElementContainer<GUM_SCALAR>::get(const
      /// std::string&).
      PRMClassElement<GUM_SCALAR>& get( const std::string& name );

      /// See gum::prm::PRMClassElementContainer<GUM_SCALAR>::get(const
      /// std::string&).
      const PRMClassElement<GUM_SCALAR>& get( const std::string& name ) const;

      /// See gum::prm::add(PRMClassElement<GUM_SCALAR>*).
      virtual NodeId add( PRMClassElement<GUM_SCALAR>* elt );

      /// See gum::prm::overload(PRMClassElement<GUM_SCALAR>*).
      virtual NodeId overload( PRMClassElement<GUM_SCALAR>* elt );

      /// See gum::prm::PRMClassElementContainer<GUM_SCALAR>::addArc().
      virtual void addArc( const std::string& tail, const std::string& head );

      /**
       * Returns the set of PRMAttribute<GUM_SCALAR> of this Class<GUM_SCALAR>.
       * @return Returns the set of PRMAttribute<GUM_SCALAR> of this
       * Class<GUM_SCALAR>.
       */
      const Set<PRMAttribute<GUM_SCALAR>*>& attributes() const;

      /**
       * Returns the set of parameters of this Class<GUM_SCALAR>.
       * @return Returns the set of parameters of this Class<GUM_SCALAR>.
       */
      const Set<PRMParameter<GUM_SCALAR>*>& parameters() const;

      /**
       * Returns all the parameters in the scope of this class.
       * &return all the parameters in the scope of this class.
       */
      HashTable<std::string, const PRMParameter<GUM_SCALAR>*> scope() const;

      /**
       * Returns the set of PRMAggregate<GUM_SCALAR> of this Class<GUM_SCALAR>.
       * @return Returns the set of PRMAggregate<GUM_SCALAR> of this
       * Class<GUM_SCALAR>.
       */
      const Set<PRMAggregate<GUM_SCALAR>*>& aggregates() const;

      /**
       * Returns the set of PRMReferenceSlot<GUM_SCALAR> of this
       * Class<GUM_SCALAR>.
       * @return Returns the set of PRMReferenceSlot<GUM_SCALAR> of this
       * Class<GUM_SCALAR>.
       */
      const Set<PRMReferenceSlot<GUM_SCALAR>*>& referenceSlots() const;

      /**
       * Returns the set of PRMSlotChain<GUM_SCALAR> of this Class<GUM_SCALAR>.
       * @return Returns the set of PRMSlotChain<GUM_SCALAR> of this
       * Class<GUM_SCALAR>.
       */
      const Set<PRMSlotChain<GUM_SCALAR>*>& slotChains() const;

      /// @}
      // ========================================================================
      /// @name Inheritance methods
      // ========================================================================
      /// @{

      /**
       * @brief Test if this Class<GUM_SCALAR> is a subclass of cec.
       *
       * If cec is a Class<GUM_SCALAR> then this Class<GUM_SCALAR> is a subclass
       *of cec if they are
       * equal or there exists a superclass of this Class<GUM_SCALAR> which is
       *equal to cec.
       *
       * If cec is an PRMInterface<GUM_SCALAR> then this Class<GUM_SCALAR> is a
       *subclass of cec if it
       * implements cec or if there exists a superclass of this
       *Class<GUM_SCALAR> which
       * implements cec.
       *
       * @param cec The PRMClassElementContainer<GUM_SCALAR> for which we
       *determine
       *if this
       *            Class<GUM_SCALAR> is a subclass of it.
       * @return Returns true if this Class<GUM_SCALAR> is a subclass of cec.
       */
      virtual bool
      isSubTypeOf( const PRMClassElementContainer<GUM_SCALAR>& cec ) const;

      /**
       * @brief Returns the super Class<GUM_SCALAR> of this Class<GUM_SCALAR>.
       * @return Returns the super Class<GUM_SCALAR> of this Class<GUM_SCALAR>.
       * @throw NotFound Raised if this has no super Class<GUM_SCALAR>.
       */
      const PRMClass<GUM_SCALAR>& super() const;

      /**
       * @brief Returns the Set of PRMInterface<GUM_SCALAR> implemented by this
       * Class<GUM_SCALAR>.
       * @return Returns the Set of PRMInterface<GUM_SCALAR> implemented by this
       * Class<GUM_SCALAR>.
       * @throw NotFound Raised if this Class<GUM_SCALAR> doesn't implement any
       * PRMInterface<GUM_SCALAR>.
       */
      const Set<PRMInterface<GUM_SCALAR>*>& implements() const;

      /// Returns the set of Class@<GUM_SCALAR@> which are direct
      /// sub-Class@<GUM_SCALAR@> of this Class@<GUM_SCALAR@>.
      const Set<PRMClass<GUM_SCALAR>*>& extensions() const;

      /// @}
      // ========================================================================
      /// @name Getters & setters operators
      // ========================================================================
      /// @{

      /// See
      /// gum::prm::PRMClassElementContainer<GUM_SCALAR>::operator[](NodeId).
      PRMClassElement<GUM_SCALAR>& operator[]( NodeId id );

      /// See
      /// gum::prm::PRMClassElementContainer<GUM_SCALAR>::operator[](NodeId).
      const PRMClassElement<GUM_SCALAR>& operator[]( NodeId id ) const;

      /// See gum::prm::PRMClassElementContainer<GUM_SCALAR>::operator[](const
      /// std::string&).
      PRMClassElement<GUM_SCALAR>& operator[]( const std::string& name );

      /// See gum::prm::PRMClassElementContainer<GUM_SCALAR>::operator[](const
      /// std::string&).
      const PRMClassElement<GUM_SCALAR>&
      operator[]( const std::string& name ) const;

      /// @}

      /// @name For custom inheritance
      /// @{
      void inheritReferenceSlots();
      void inheritParameters();
      void inheritAttributes();
      void inheritAggregates();
      void inheritSlotChains();
      void initializeInheritance();
      void completeInheritance( const std::string& attr );
      /// @}

      protected:
      /// returns a constant reference over this interface's dag.
      virtual const DAG& _dag() const;

      /// Returns a non constant reference over this PRMInterface<GUM_SCALAR>'s
      /// DAG.
      virtual DAG& _dag();

      /// Fills set with all the subtypes of this Class<GUM_SCALAR>.
      void _findAllSubtypes( Set<PRMClassElementContainer<GUM_SCALAR>*>& set );

      /// See gum::prm::PRMClassElementContainer<GUM_SCALAR>(const
      /// PRMClassElement<GUM_SCALAR>&).
      void _updateDescendants( const PRMClassElement<GUM_SCALAR>& elt );

      private:
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
      NodeProperty<PRMClassElement<GUM_SCALAR>*> __nodeIdMap;

      /// @}
      // ========================================================================
      /// @name PRMClassElement<GUM_SCALAR> members
      // ========================================================================
      /// @{

      /// Mapping between a member's name and itself.
      /// Used for fast access to a member given it's name
      HashTable<std::string, PRMClassElement<GUM_SCALAR>*> __nameMap;

      /// The sequence of PRMAttribute<GUM_SCALAR>s.
      Set<PRMAttribute<GUM_SCALAR>*> __attributes;

      /// The sequence of PRMReferenceSlot<GUM_SCALAR>.
      Set<PRMReferenceSlot<GUM_SCALAR>*> __referenceSlots;

      /// The sequence of aggregate.
      Set<PRMAggregate<GUM_SCALAR>*> __aggregates;

      /// The set of gum::PRMSlotChain<GUM_SCALAR>s
      Set<PRMSlotChain<GUM_SCALAR>*> __slotChains;

      /// The Set of parameters in this Class<GUM_SCALAR>.
      Set<PRMParameter<GUM_SCALAR>*> __parameters;

      /// Recursively adds cast descendant of attr in this Class<GUM_SCALAR>.
      void __addCastDescendants( PRMClassElement<GUM_SCALAR>* attr );

      /// Recursively adds cast descendant from start to end in this
      /// Class<GUM_SCALAR>.
      void __addCastDescendants( PRMAttribute<GUM_SCALAR>* start,
                                 PRMAttribute<GUM_SCALAR>* end );

      /// @}
      // ========================================================================
      /// @name Inheritance members
      // ========================================================================
      /// @{

      /// @brief The alternate PRMClassElementContainer<GUM_SCALAR> searched for
      /// elements defined in
      ///        this.
      /// Note that this is first searched for gum::PRMClassElement<GUM_SCALAR>.
      PRMClass<GUM_SCALAR>* __superClass;

      /// The Set of implemented interface of this.
      Set<PRMInterface<GUM_SCALAR>*>* __implements;

      /// The set of Class<GUM_SCALAR> which are extension of this
      /// Class<GUM_SCALAR> (i.e. direct
      /// subtypes).
      Set<PRMClass<GUM_SCALAR>*> __extensions;

      /// The bijection between variables in super and variables in this
      /// The bijection's firsts are attributes in this and its seconds are
      /// attributes in c.
      Bijection<const DiscreteVariable*, const DiscreteVariable*>* __bijection;

      /// Proceed with the copy when this inherits c.
      void __inheritClass( const PRMClass<GUM_SCALAR>& c );

      /// Proceed with the implementation of interfaces
      void __implementInterfaces( bool delayInheritance );

      /// Check that a given element respects all the class interfaces
      void __checkInterfaces( PRMClassElement<GUM_SCALAR>* elt );

      /// Check that a given element respects a specific interface
      void __checkInterface( PRMClassElement<GUM_SCALAR>* elt,
                             PRMInterface<GUM_SCALAR>*    i );

      /// Check that a given element respects all the class interfaces
      void __checkRefInterfaces( PRMReferenceSlot<GUM_SCALAR>* elt );

      /// Check that a given element respects a specific interface
      void __checkRefInterface( PRMReferenceSlot<GUM_SCALAR>* elt,
                                PRMInterface<GUM_SCALAR>*     i );

      /// Check if elt is present in an implementation. If it is, its IO flags
      /// are updated.
      void __addIOInterfaceFlags( PRMClassElement<GUM_SCALAR>* elt );

      /// This method is called when a sub-Class@<GUM_SCALAR@> of this
      /// Class@<GUM_SCALAR@> is created.
      /// @param c The Class@<GUM_SCALAR@> added as a direct
      /// sub-Class@<GUM_SCALAR@>
      /// of this.
      /// @throw DuplicateElement Raised if c is already a
      /// sub-Class@<GUM_SCALAR@>
      /// of this.
      void __addExtension( PRMClass<GUM_SCALAR>* c );

      /// Return true of overloaded can be overload by overloader.
      bool
      __checkOverloadLegality( const PRMClassElement<GUM_SCALAR>* overloaded,
                               const PRMClassElement<GUM_SCALAR>* overloader );

      /// Overloads an attribute.
      void __overloadAttribute( PRMAttribute<GUM_SCALAR>* overloader,
                                PRMAttribute<GUM_SCALAR>* overloaded );

      /// Overloads an aggregate.
      void __overloadAggregate( PRMAggregate<GUM_SCALAR>*    overloader,
                                PRMClassElement<GUM_SCALAR>* overloaded );

      /// Overloads a reference slot.
      void __overloadReference( PRMReferenceSlot<GUM_SCALAR>* overloader,
                                PRMReferenceSlot<GUM_SCALAR>* overloaded );

      /// Overloads a parameter.
      void __overloadParameter( PRMParameter<GUM_SCALAR>* overloader,
                                PRMParameter<GUM_SCALAR>* overloaded );

      /// @}
    };


    extern template class PRMClass<float>;
    extern template class PRMClass<double>;

  } /* namespace prm */
}  // namespace gum

#include <agrum/PRM/elements/PRMClass_tpl.h>

#endif /* GUM_CLASS_H */
