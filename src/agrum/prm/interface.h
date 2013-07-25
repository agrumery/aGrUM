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
 * @author Lionel TORTI
 */

#ifndef GUM_INTERFACE_H
#define GUM_INTERFACE_H

#include <utility>
#include <set>

#include <agrum/core/set.h>
#include <agrum/core/sequence.h>

#include <agrum/prm/classElementContainer.h>
#include <agrum/prm/attribute.h>
#include <agrum/prm/referenceSlot.h>
#include <agrum/prm/aggregate.h>
#include <agrum/prm/slotChain.h>

namespace gum {
  namespace prm {

    class Class;

    /**
     * @class Interface interface.h <agrum/prm/interface.h>
     * @brief An Interface is implemented by a Class and defines a set of
     *        ReferenceSlot and Attribute which the implementing Class must contain.
     *
     * @see PRM Class ClassElement
     */
    class Interface: public ClassElementContainer {
        // ========================================================================
        friend class Class;
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
        Interface( const std::string& name );

        /**
         * Constructor for building a subclass of super.
         * @param name The sub-interface name.
         * @param super The super Interface of this.
         */
        Interface( const std::string& name, Interface& super );

        /// Copy constructor.
        Interface( const Interface& source );

        /// Destructor.
        virtual ~Interface();

        /// @}
        // ========================================================================
        /// @name Graphical model getters and setters
        // ========================================================================
        /// @{

        /// Implementation of pure virtual method of PRMObject.
        virtual ObjectType obj_type() const;

        /// See gum::prm::ClassElementContainer::get(NodeId).
        virtual ClassElement& get( NodeId id );

        /// Se gum::prm::ClassElementContainer::get(NodeId).
        virtual const ClassElement& get( NodeId id ) const;

        /**
         * @brief An Interfance doesn't have any arc, this will raise an
         *        OperationNotAllowed exception.
         *
         * @param tail The tail of the added arc.
         * @param head The head of the added arc.
         * @throw OperationNotAllowed Systematically throws an OperationNotAllowed.
         */
        void insertArc( const std::string& tail, const std::string& head );

        /// @}
        // ========================================================================
        /// @name ClassElement getters and setters
        // ========================================================================
        /// @{

        /// See gum::prm::ClassElementContainer::get(const std::string&).
        virtual ClassElement& get( const std::string& name );

        /// See gum::prm::ClassElementContainer::get(const std::string&).
        virtual const ClassElement& get( const std::string& name ) const;

        /**
         * Returns the set of Attribute of this Class.
         * @return Returns the set of Attribute of this Class.
         */
        const Set< Attribute* >& attributes() const;

        /**
         * Returns the set of Aggregate of this Class.
         * @return Returns the set of Aggregate of this Class.
         */
        const Set< ReferenceSlot* >& referenceSlots() const;

        /// See gum::prm::ClassElementContainer::add(ClassElement*).
        NodeId add( ClassElement* elt );

        /**
         * @brief Add a new ClassElement which overload an inherited ClassElement.
         *
         * The pointer is "given" to this class, which will delete it when
         * ~Class() is called.
         *
         * The NodeId of elt is defined when it is added to this, discarding any
         * previous value. There is no garanty that elt will have the same NodeId
         * than the ClassElement it overloaded.
         *
         * You can only overload inherited ClassElement and only if elt is a subtype
         * of the inherited ClassElement. You do not define dependencies in an Interface
         * so it is useless to overload an Attribute with another sharing the same type,
         * and if tried it will raise an OperationNotAllowed exception.
         *
         * @param elt The new ClassElement overloading an inherited ClassElement in this.
         * @return the NodeId assigned to elt.
         * @throw NotFound Raised if no overloaded ClassElement is found.
         * @throw OperationNotAllowed Raised if the overloading is impossible.
         */
        NodeId overload( ClassElement* elt );

        // /**
        //  * @brief Remove a ClassElement from this Class.
        //  *
        //  * When removing a ClassElement it will not be deleted by this Class, thus
        //  * you should delete it yourself.
        //  *
        //  * All dependencies among the removed ClassElement and ClassElement defined
        //  * in and outside of this class are deleted also. You must update the
        //  * corresponding CPF yourself.
        //  *
        //  * Futhermore if there exists Instance of this Class you should be very
        //  * careful at what you are doing (for instance do not delete the
        //  * ClassElement before deleting the concerned Instance).
        //  *
        //  * @param id The ClassElement's NodeId.
        //  * @return the pointer over the removed ClassElement.
        //  * @throw NotFound If no ClassElement matches id.
        //  */
        // ClassElement* remove(NodeId id);

        /// @}
        // ========================================================================
        /// @name Inheritance methods
        // ========================================================================
        /// @{

        /**
         * @brief Test if this Interface is a sub Interface of cec.
         *
         * Interface can not be a sub Interface of a Class, so if cec is a Class
         * this method will return false.
         *
         * If cec is an Interface then this Interface is a sub Interface of cec if they are
         * equal or there exists a super Interface of this Interface which is equal to cec.
         *
         * @param cec The ClassElementContainer for which we determine if this
         *            Interface is a sub Interface of it.
         * @return Returns true if this Class is a subclass of cec.
         */
        virtual bool isSubTypeOf( const ClassElementContainer& cec ) const;

        /**
         * @brief Returns the superInterface of this Interface.
         * @return Returns the super Interface of this Interface.
         * @throw NotFound Raised if this has no super Interface.
         */
        Interface& super();

        /**
         * @brief Returns the superInterface of this Interface.
         * @return Returns the super Interface of this Interface.
         * @throw NotFound Raised if this has no super Interface.
         */
        const Interface& super() const;

        /**
         * Returns the set of Class implementing this Interface.
         */
        Set<Class*>& implementations();

        /**
         * Returns the set of Class implementing this Interface.
         */
        const Set<Class*>& implementations() const;

        /// @}
        // ========================================================================
        /// @name Getters & setters operators
        // ========================================================================
        /// @{

        /// See gum::prm::ClassElementContainer::operator[](NodeId).
        ClassElement& operator[]( NodeId id );

        /// See gum::prm::ClassElementContainer::operator[](NodeId).
        const ClassElement& operator[]( NodeId id ) const;

        /// See gum::prm::ClassElementContainer::operator[](const std::string&).
        ClassElement& operator[]( const std::string& name );

        /// See gum::prm::ClassElementContainer::operator[](const std::string&).
        const ClassElement& operator[]( const std::string& name ) const;

        /// @}
        // ========================================================================
        /// @name Iterators
        // ========================================================================
        /// @{

        typedef Property<ClassElement*>::onNodes::iterator ClassEltIterator;
        ClassEltIterator begin();
        const ClassEltIterator& end();

        typedef Property<ClassElement*>::onNodes::const_iterator const_ClassEltIterator;
        const_ClassEltIterator begin() const;
        const const_ClassEltIterator& end() const;

        /// @}
      protected:

        /// Returns a constant reference over this Interface's DAG.
        const DAG& _dag() const;

        /// Returns a non constant reference over this Interface's DAG.
        DAG& _dag();

        /// Fills set with all the subtypes of this Interface, this includes extensions
        /// and implementations.
        void _findAllSubtypes( Set<ClassElementContainer*>& set );

        /// See gum::prm::ClassElementContainer(const ClassElement&).
        void _updateDescendants( const ClassElement& elt );

      private:

        /// Copy operator. Don't use it.
        Interface& operator=( const Interface& source );

        /// Proceed with the copy of i in this.
        void __inheritInterface( const Interface& i );

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

        /// @}
        // ========================================================================
        /// @name Inheritance members
        // ========================================================================
        /// @{

        /// @brief The alternate ClassElementContainer searched for elements defined in
        ///        this.
        /// Note that this is first searched for gum::ClassElement.
        Interface* __super;

        /// The set of Class which implements this Interface.
        Set<Class*> __implementations;

        /// Add an Class to the set of Class which implements this Interface.
        void __addImplementation( Class* c );

        /// The set of Class which implements this Interface.
        Set<Interface*> __extensions;

        /// Add an Class to the set of Class which implements this Interface.
        void __addExtension( Interface* c );

        void __checkOverloadLegality( const ClassElement* overloaded,
                                      const ClassElement* overloader );

        void __overloadAttribute( Attribute* overloader, Attribute* overloaded );

        void __addCastDescendants( Attribute* start, Attribute* end );

        void __swap_types( Attribute* overloader, Attribute* overloaded );

        /// @}
    };

  } /* namespace prm */
} // namespace gum
// ==========================================================================
#ifndef GUM_NO_INLINE
#include <agrum/prm/interface.inl>
#endif // GUM_NO_INLINE
// ==========================================================================
#endif /* GUM_INTERFACE_H */
// ==========================================================================
