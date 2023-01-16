/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Headers of gum::prm::PRMInterface.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_INTERFACE_H
#define GUM_INTERFACE_H

#include <set>
#include <utility>
#include <string>

#include <agrum/agrum.h>

#include <agrum/PRM/elements/PRMReferenceSlot.h>
#include <agrum/PRM/elements/PRMScalarAttribute.h>


namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    class PRMClass;

    /**
     * @class PRMInterface
     * @headerfile interface.h <agrum/PRM/interface.h>
     * @brief An PRMInterface is implemented by a Class<GUM_SCALAR> and defines
     *a
     *set of
     *        PRMReferenceSlot<GUM_SCALAR> and PRMAttribute<GUM_SCALAR> which
     *the
     *implementing Class<GUM_SCALAR> must contain.
     *
     * @see PRM Class<GUM_SCALAR> PRMClassElement<GUM_SCALAR>
     */
    template < typename GUM_SCALAR >
    class PRMInterface: public PRMClassElementContainer< GUM_SCALAR > {
      // ========================================================================
      friend class PRMClass< GUM_SCALAR >;
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
      explicit PRMInterface(const std::string& name);

      /**
       * Constructor for building a subclass of super.
       * @param name The sub-interface name.
       * @param super The super PRMInterface of this.
       * @param delayInheritance If true, inheritance will be delayed.
       */
      PRMInterface(const std::string&          name,
                   PRMInterface< GUM_SCALAR >& super,
                   bool                        delayInheritance = false);

      /// Copy constructor.
      PRMInterface(const PRMInterface< GUM_SCALAR >& source);

      /// Destructor.
      virtual ~PRMInterface();

      /// @}
      // ========================================================================
      /// @name Graphical model getters and setters
      // ========================================================================
      /// @{

      /// Implementation of pure virtual method of PRMObject.
      virtual typename PRMObject::prm_type obj_type() const;

      /// See gum::prm::PRMClassElementContainer<GUM_SCALAR>::get(NodeId).
      virtual PRMClassElement< GUM_SCALAR >& get(NodeId id);

      /// Se gum::prm::PRMClassElementContainer<GUM_SCALAR>::get(NodeId).
      virtual const PRMClassElement< GUM_SCALAR >& get(NodeId id) const;

      /**
       * @brief An Interfance doesn't have any arc, this will raise an
       *        OperationNotAllowed exception.
       *
       * @param tail The tail of the added arc.
       * @param head The head of the added arc.
       * @throw OperationNotAllowed Systematically throws an
       *OperationNotAllowed.
       */
      void addArc(const std::string& tail, const std::string& head);

      /// @}
      // ========================================================================
      /// @name PRMClassElement<GUM_SCALAR> getters and setters
      // ========================================================================
      /// @{

      virtual bool isOutputNode(const PRMClassElement< GUM_SCALAR >& elt) const;

      /// See gum::prm::PRMClassElementContainer<GUM_SCALAR>::get(const
      /// std::string&).
      virtual PRMClassElement< GUM_SCALAR >& get(const std::string& name);

      /// See gum::prm::PRMClassElementContainer<GUM_SCALAR>::get(const
      /// std::string&).
      virtual const PRMClassElement< GUM_SCALAR >& get(const std::string& name) const;

      /**
       * Returns the set of PRMAttribute<GUM_SCALAR> of this Class<GUM_SCALAR>.
       * @return Returns the set of PRMAttribute<GUM_SCALAR> of this
       * Class<GUM_SCALAR>.
       */
      const Set< PRMAttribute< GUM_SCALAR >* >& attributes() const;

      /**
       * Returns the set of PRMAggregate of this Class<GUM_SCALAR>.
       * @return Returns the set of PRMAggregate of this Class<GUM_SCALAR>.
       */
      const Set< PRMReferenceSlot< GUM_SCALAR >* >& referenceSlots() const;

      /// See
      /// gum::prm::PRMClassElementContainer<GUM_SCALAR>::add(PRMClassElement<GUM_SCALAR>*).
      NodeId add(PRMClassElement< GUM_SCALAR >* elt);

      /**
       * @brief Add a new PRMClassElement<GUM_SCALAR> which overload an
       *inherited
       *PRMClassElement<GUM_SCALAR>.
       *
       * The pointer is "given" to this class, which will delete it when
       * ~Class<GUM_SCALAR>() is called.
       *
       * The NodeId of elt is defined when it is added to this, discarding any
       * previous value. There is no garanty that elt will have the same NodeId
       * than the PRMClassElement<GUM_SCALAR> it overloaded.
       *
       * You can only overload inherited PRMClassElement<GUM_SCALAR> and only if
       *elt is
       *a subtype
       * of the inherited PRMClassElement<GUM_SCALAR>. You do not define
       *dependencies in
       *an PRMInterface
       * so it is useless to overload an PRMAttribute<GUM_SCALAR> with another
       *sharing
       *the same type,
       * and if tried it will raise an OperationNotAllowed exception.
       *
       * @param elt The new PRMClassElement<GUM_SCALAR> overloading an inherited
       *PRMClassElement<GUM_SCALAR> in this.
       * @return the NodeId assigned to elt.
       * @throw NotFound Raised if no overloaded PRMClassElement<GUM_SCALAR> is
       *found.
       * @throw OperationNotAllowed Raised if the overloading is impossible.
       */
      NodeId overload(PRMClassElement< GUM_SCALAR >* elt);

      /// @}
      // ========================================================================
      /// @name Inheritance methods
      // ========================================================================
      /// @{

      /**
       * @brief Test if this PRMInterface is a sub PRMInterface of cec.
       *
       * PRMInterface can not be a sub PRMInterface of a Class<GUM_SCALAR>, so
       *if cec
       *is a
       *Class<GUM_SCALAR>
       * this method will return false.
       *
       * If cec is an PRMInterface then this PRMInterface is a sub PRMInterface
       *of cec if
       *they
       *are
       * equal or there exists a super PRMInterface of this PRMInterface which
       *is
       *equal to
       *cec.
       *
       * @param cec The PRMClassElementContainer<GUM_SCALAR> for which we
       *determine
       *if
       *this
       *            PRMInterface is a sub PRMInterface of it.
       * @return Returns true if this Class<GUM_SCALAR> is a subclass of cec.
       */
      virtual bool isSubTypeOf(const PRMClassElementContainer< GUM_SCALAR >& cec) const;

      /**
       * @brief Returns the superInterface of this PRMInterface.
       * @return Returns the super PRMInterface of this PRMInterface.
       * @throw NotFound Raised if this has no super PRMInterface.
       */
      PRMInterface< GUM_SCALAR >& super();

      /**
       * @brief Returns the superInterface of this PRMInterface.
       * @return Returns the super PRMInterface of this PRMInterface.
       * @throw NotFound Raised if this has no super PRMInterface.
       */
      const PRMInterface< GUM_SCALAR >& super() const;

      /**
       * Returns the set of Class<GUM_SCALAR> implementing this PRMInterface.
       */
      Set< PRMClass< GUM_SCALAR >* >& implementations();

      /**
       * Returns the set of Class<GUM_SCALAR> implementing this PRMInterface.
       */
      const Set< PRMClass< GUM_SCALAR >* >& implementations() const;

      /**
       * Inherits from this interface super interface, this should only be done
       * when this inteface inheritance was delayed.
       */
      void inheritInterface();

      /// @}
      // ========================================================================
      /// @name Getters & setters operators
      // ========================================================================
      /// @{

      /// See
      /// gum::prm::PRMClassElementContainer<GUM_SCALAR>::operator[](NodeId).
      PRMClassElement< GUM_SCALAR >& operator[](NodeId id);

      /// See
      /// gum::prm::PRMClassElementContainer<GUM_SCALAR>::operator[](NodeId).
      const PRMClassElement< GUM_SCALAR >& operator[](NodeId id) const;

      /// See gum::prm::PRMClassElementContainer<GUM_SCALAR>::operator[](const
      /// std::string&).
      PRMClassElement< GUM_SCALAR >& operator[](const std::string& name);

      /// See gum::prm::PRMClassElementContainer<GUM_SCALAR>::operator[](const
      /// std::string&).
      const PRMClassElement< GUM_SCALAR >& operator[](const std::string& name) const;

      /// @}
      // ========================================================================
      /// @name Iterators
      // ========================================================================
      /// @{

      using ClassEltIterator = typename NodeProperty< PRMClassElement< GUM_SCALAR >* >::iterator;
      ClassEltIterator        begin();
      const ClassEltIterator& end();

      using const_ClassEltIterator =
         typename NodeProperty< PRMClassElement< GUM_SCALAR >* >::const_iterator;
      const_ClassEltIterator        begin() const;
      const const_ClassEltIterator& end() const;

      /// @}
      protected:
      /// Returns a constant reference over this PRMInterface's DAG.
      const DAG& dag_() const;

      /// Returns a non constant reference over this PRMInterface's DAG.
      DAG& dag_();

      /// Fills set with all the subtypes of this PRMInterface, this includes
      /// extensions
      /// and implementations.
      void findAllSubtypes_(Set< PRMClassElementContainer< GUM_SCALAR >* >& set);

      /// See gum::prm::PRMClassElementContainer<GUM_SCALAR>(const
      /// PRMClassElement<GUM_SCALAR>&).
      void updateDescendants_(const PRMClassElement< GUM_SCALAR >& elt);

      private:
      /// Copy operator. Don't use it.
      PRMInterface< GUM_SCALAR >& operator=(const PRMInterface< GUM_SCALAR >& source);

      /// Proceed with the copy of i in this.
      void _inheritInterface_(const PRMInterface< GUM_SCALAR >& i);

      // ========================================================================
      /// @name Graphical model members
      // ========================================================================
      /// @{

      /// The dag representing dependencies between formal attributes and
      /// slots.
      DAG _dag_;

      /// Mapping between node's id and their name (being an attribute or a
      /// slot). Used for fast access to a member given it's node id.
      NodeProperty< PRMClassElement< GUM_SCALAR >* > _nodeIdMap_;

      /// @}
      // ========================================================================
      /// @name PRMClassElement<GUM_SCALAR> members
      // ========================================================================
      /// @{

      /// Mapping between a member's name and itself.
      /// Used for fast access to a member given it's name.
      HashTable< std::string, PRMClassElement< GUM_SCALAR >* > _nameMap_;

      /// The sequence of PRMAttribute<GUM_SCALAR>s.
      Set< PRMAttribute< GUM_SCALAR >* > _attributes_;

      /// The sequence of PRMReferenceSlot<GUM_SCALAR>.
      Set< PRMReferenceSlot< GUM_SCALAR >* > _referenceSlots_;

      /// @}
      // ========================================================================
      /// @name Inheritance members
      // ========================================================================
      /// @{

      /// @brief The alternate PRMClassElementContainer<GUM_SCALAR> searched for
      /// elements defined in
      ///        this.
      /// Note that this is first searched for gum::PRMClassElement<GUM_SCALAR>.
      PRMInterface< GUM_SCALAR >* _superInterface_;

      /// The set of Class<GUM_SCALAR> which implements this PRMInterface.
      Set< PRMClass< GUM_SCALAR >* > _implementations_;

      /// Add an Class<GUM_SCALAR> to the set of Class<GUM_SCALAR> which
      /// implements
      /// this PRMInterface.
      void _addImplementation_(PRMClass< GUM_SCALAR >* c);

      /// The set of Class<GUM_SCALAR> which implements this PRMInterface.
      Set< PRMInterface< GUM_SCALAR >* > _extensions_;

      /// Add an Class<GUM_SCALAR> to the set of Class<GUM_SCALAR> which
      /// implements
      /// this PRMInterface.
      void _addExtension_(PRMInterface< GUM_SCALAR >* c);

      bool _checkOverloadLegality_(const PRMClassElement< GUM_SCALAR >* overloaded,
                                   const PRMClassElement< GUM_SCALAR >* overloader);

      void _overloadAttribute_(PRMAttribute< GUM_SCALAR >* overloader,
                               PRMAttribute< GUM_SCALAR >* overloaded);

      void _overloadReferenceSlot_(PRMReferenceSlot< GUM_SCALAR >* overloader,
                                   PRMReferenceSlot< GUM_SCALAR >* overloaded);

      void _addCastDescendants_(PRMAttribute< GUM_SCALAR >* start, PRMAttribute< GUM_SCALAR >* end);

      /// @}
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class PRMInterface< double >;
#endif


  } /* namespace prm */
}   // namespace gum

#include <agrum/PRM/elements/PRMInterface_tpl.h>

#endif /* GUM_INTERFACE_H */
