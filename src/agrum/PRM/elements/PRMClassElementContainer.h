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
 * @brief Headers of gum::prm::PRMClassElementContainer.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_CLASS_ELEMENT_CONTAINER_H
#define GUM_CLASS_ELEMENT_CONTAINER_H


#include <agrum/PRM/utils_prm.h>


namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    class PRMClass;
    template < typename GUM_SCALAR >
    class PRMInterface;
    class PRMClassElementContainterIterator;
    class PRMClassElementContainterConstIterator;

    /**
     * @class PRMClassElementContainer classElementContainer.h
     *<agrum/PRM/classElementContainer.h>
     * @brief Abstract class for classes containing
     *gum::PRMClassElement<GUM_SCALAR>.
     *
     * To print a PRMClassElementContainer you can use the following operator:
     * gum::operator<<(std::ostream&, const PRMClassElementContainer&) which
     *print
     * the PRMClassElementContainer in the graphviz-dot format.
     *
     * @ingroup prm_group
     */
    template < typename GUM_SCALAR >
    class PRMClassElementContainer: public PRMObject {
      friend class PRMClassElementContainterIterator;
      friend class PRMClassElementContainterConstIterator;

      public:
      // ========================================================================
      /// @name Protected constructors & destructor.
      // ========================================================================
      /// @{

      /// Default constructor.
      PRMClassElementContainer(const std::string& name);

      /// Destructor.
      virtual ~PRMClassElementContainer();

      /// @}
      // ========================================================================
      /// @name Getters on the gum::PRMClassElement<GUM_SCALAR>.
      // ========================================================================
      /// @{

      /**
       * Returns true if elt belongs to this PRMClassElementContainer.
       * @param elt A PRMClassElement<GUM_SCALAR>.
       * @return true if elt beglons to this PRMClassElementContainer.
       */
      virtual bool belongsTo(const PRMClassElement< GUM_SCALAR >& elt) const;

      /**
       * Returns true if a member with the given name exists in this
       * PRMClassElementContainer or
       * in the PRMClassElementContainer hierarchy.
       */
      virtual bool exists(const std::string& name) const;

      /**
       * Getter on a member of this PRMClassElementContainer.
       * @param name The member's name.
       * @return Returns a constant reference on the member.
       * @throw NotFound Raised if no attribute matches name.
       */
      virtual PRMClassElement< GUM_SCALAR >& get(const std::string& name) = 0;

      /**
       * Constant getter on a member of this PRMClassElementContainer.
       * @param name The member's name.
       * @return Returns a constant reference on the member.
       * @throw NotFound Raised if no attribute matches name.
       */
      virtual const PRMClassElement< GUM_SCALAR >& get(const std::string& name) const = 0;

      /**
       * @brief Add a PRMClassElement<GUM_SCALAR> to this
       *PRMClassElementContainer.
       *
       * The pointer is "given" to this class, which will delete it when
       * ~Class() is called.
       *
       * The NodeId of elt is defined when it is added to this, discarding any
       * previous value.
       *
       * If you want to overload an inherited PRMClassElement<GUM_SCALAR> call
       * Class::overload().
       *
       * When adding an PRMAttribute or an PRMAggregate its type safe name is
       * automatically added, the syntax is \<type\>name. So you can either use
       * its type safe name or its real name. See the ref prm_typ_inh "type
       * inheritance section" for further details.
       *
       * @param elt The PRMClassElement<GUM_SCALAR> added to this Class.
       * @return Returns the NodeId assigned to elt.
       *
       * @throw DuplicateElement Raised if elt's name is already used in this
       *class.
       */
      virtual NodeId add(PRMClassElement< GUM_SCALAR >* elt) = 0;

      /**
       * @brief Add a PRMClassElement<GUM_SCALAR> which overload an inherited
       *PRMClassElement<GUM_SCALAR>.
       *
       * The pointer is "given" to this class, which will delete it when
       * ~PRMClassElementContainer() is called.
       *
       * The NodeId of elt is defined when it is added to this, discarding any
       * previous value. There is no guaranty that elt will have the same NodeId
       * than the PRMClassElement<GUM_SCALAR> it overloaded.
       *
       * You can only overload inherited PRMClassElement<GUM_SCALAR> and only if
       *elt is
       * a subtype of the inherited PRMClassElement<GUM_SCALAR>. Thus you can
       *only
       * overload PRMReferenceSlot and PRMAttribute. In the case of PRMAttribute
       *you can
       * overload an inherited PRMAttribute even if they are of the same type:
       *this
       *is
       * useful when you want to redefine the dependencies of an PRMAttribute or
       *its
       * CPF. You can also overload an PRMAttribute with an PRMAggregate, as
       *long as
       *their
       * respective PRMType allow it.
       *
       * @param elt The PRMClassElement<GUM_SCALAR> overloading an inherited
       *            PRMClassElement<GUM_SCALAR> in this
       *PRMClassElementContainer.
       * @return the NodeId assigned to elt.
       *
       * @throw OperationNotAllowed Raised if  overloading is illegal.
       */
      virtual NodeId overload(PRMClassElement< GUM_SCALAR >* elt) = 0;

      /**
       * Add an arc between two PRMClassElement<GUM_SCALAR>.
       */
      virtual void addArc(const std::string& tail, const std::string& head) = 0;

      /**
       * @brief Returns true if the node is an input node.
       *
       * PRMAttribute or PRMAggregate can either be input, output, both
       * or internal nodes.
       *
       * By defaut, attributes and aggregates are inner nodes.
       *
       * @param elt A PRMClassElement<GUM_SCALAR>.
       * @return Returns true if id is an input node.
       */
      virtual bool isInputNode(const PRMClassElement< GUM_SCALAR >& elt) const;

      /**
       * @brief Set the input flag value of id at b.
       *
       * PRMAttribute or PRMAggregate can either be input, output, both
       * or internal nodes.
       *
       * By defaut, attributes and aggregates are inner nodes.
       *
       * @param elt A PRMClassElement<GUM_SCALAR>.
       * @param b The flag value.
       *
       * @throw NotFound Raised if id does'nt match any
       *PRMClassElement<GUM_SCALAR>
       *in
       *this.
       * @throw WrongClassElement Raised if NodeId is neither an PRMAttribute
       *nor
       *                          an PRMAggregate.
       */
      virtual void setInputNode(const PRMClassElement< GUM_SCALAR >& elt, bool b);

      /**
       * @brief Returns true if the node is an output node.
       *
       * PRMAttribute or PRMAggregate can either be input, output, both
       * or internal nodes.
       *
       * By defaut, attributes and aggregates are inner nodes.
       *
       * @param elt A PRMClassElement<GUM_SCALAR>.
       * @return Returns true if id is an input node.
       */
      virtual bool isOutputNode(const PRMClassElement< GUM_SCALAR >& elt) const = 0;

      /**
       * @brief Set the output flag value of id at b.
       *
       * PRMAttribute or PRMAggregate can either be input, output, both
       * or internal nodes.
       *
       * By defaut, attributes and aggregates are inner nodes.
       *
       * @param elt A PRMClassElement<GUM_SCALAR>.
       * @param b The flag value.
       *
       * @throw NotFound Raised if id does'nt match any
       *PRMClassElement<GUM_SCALAR>
       *in
       *this.
       * @throw WrongClassElement Raised if NodeId is neither an PRMAttribute
       *nor
       *                          an PRMAggregate.
       */
      virtual void setOutputNode(const PRMClassElement< GUM_SCALAR >& elt, bool b);

      /**
       * @brief Returns true if the node is an inner node.
       *
       * PRMAttribute or PRMAggregate can either be input, output, both
       * or internal nodes.
       *
       * By defaut, attributes and aggregates are inner nodes.
       *
       * @param elt A PRMClassElement<GUM_SCALAR>.
       * @return true if elt is an inner node.
       *
       * @throw NotFound Raised if NodeId does'nt match any
       *PRMClassElement<GUM_SCALAR>
       *in this.
       * @throw WrongClassElement Raised if NodeId is neither an PRMAttribute
       *nor
       *                          an PRMAggregate.
       */
      virtual bool isInnerNode(const PRMClassElement< GUM_SCALAR >& elt) const;
      /// @}
      // ========================================================================
      /// @name Graphical operator
      // ========================================================================
      /// @{

      /**
       * @brief Returns the gum::DAG of this PRMClassElementContainer.
       *
       * Be very careful when using NodeId with PRMClassElement<GUM_SCALAR>:
       * there is no guarantee that an inherited PRMClassElement<GUM_SCALAR> will
       * have the same NodeId than its superclass counterpart.
       *
       * When dealing with different classes and interfaces ALWAYS use safe-name
       * as identifier.
       *
       * @return the DAG of this PRMClassElementContainer.
       */
      virtual const DAG& containerDag() const;

      /**
       * Returns true if a member with the given id exists in this
       * PRMClassElementContainer or
       * in the PRMClassElementContainer hierarchy.
       * @param id A NodeId.
       * @return true if id matches a PRMClassElement<GUM_SCALAR>.
       */
      virtual bool exists(NodeId id) const;

      /**
       * Getter on a member of this PRMClassElementContainer.
       * @param id The member's id.
       * @return Returns a constant reference on the member.
       * @throw NotFound Raised if no attribute matches name.
       */
      virtual PRMClassElement< GUM_SCALAR >& get(NodeId id) = 0;

      /**
       * Constant getter on a member of this PRMClassElementContainer.
       * @param id The member's id.
       * @return Returns a constant reference on the member.
       * @throw NotFound Raised if no attribute matches name.
       */
      virtual const PRMClassElement< GUM_SCALAR >& get(NodeId id) const = 0;

      /// @}
      // ========================================================================
      /// @name Getters & setters operators
      // ========================================================================
      /// @{

      /**
       * Getter on a member of this PRMClassElementContainer.
       * @param id The member's id.
       * @return Returns a constant reference on the member.
       * @throw NotFound Raised if no attribute matches name.
       */
      virtual PRMClassElement< GUM_SCALAR >& operator[](NodeId id) = 0;

      /**
       * Constant getter on a member of this PRMClassElementContainer.
       * @param id The member's id.
       * @return Returns a constant reference on the member.
       * @throw NotFound Raised if no attribute matches name.
       */
      virtual const PRMClassElement< GUM_SCALAR >& operator[](NodeId id) const = 0;

      /**
       * Getter on a member of this PRMClassElementContainer.
       * @param name The member's name.
       * @return Returns a constant reference on the member.
       * @throw NotFound Raised if no attribute matches name.
       */
      virtual PRMClassElement< GUM_SCALAR >& operator[](const std::string& name) = 0;

      /**
       * Constant getter on a member of this PRMClassElementContainer.
       * @param name The member's name.
       * @return Returns a constant reference on the member.
       * @throw NotFound Raised if no attribute matches name.
       */
      virtual const PRMClassElement< GUM_SCALAR >& operator[](const std::string& name) const = 0;

      /// @}
      // ========================================================================
      /// @name Inheritance getters and setters
      // ========================================================================
      /// @{

      /**
       * @brief Test if this PRMClassElementContainer is a subtype of cec.
       *
       * @param cec
       * @return return true if this PRMClassElementContainer is a subtype of
       * cec.
       */
      virtual bool isSubTypeOf(const PRMClassElementContainer< GUM_SCALAR >& cec) const = 0;

      /**
       * @brief Test if this PRMClassElementContainer is a super type of cec.
       *
       * This returns cec.isSubTypeOf(*this).
       *
       * @param cec
       * @return return true if this PRMClassElementContainer is a super type of
       *cec.
       */
      virtual bool isSuperTypeOf(const PRMClassElementContainer< GUM_SCALAR >& cec) const;

      /// @}

      protected:
      /// Copy operator. Don't use it.
      PRMClassElementContainer< GUM_SCALAR >&
         operator=(const PRMClassElementContainer< GUM_SCALAR >& source);

      /// Copy constructor. Don't use it.
      PRMClassElementContainer(const PRMClassElementContainer< GUM_SCALAR >& source);

      virtual const DAG& dag_() const = 0;

      /// Returns a non constant reference over this PRMClassElementContainer's
      /// DAG.
      virtual DAG& dag_() = 0;

      /// Fills set with all the subtypes of this PRMInterface, this includes
      /// extensions
      /// and implementations.
      virtual void findAllSubtypes_(Set< PRMClassElementContainer< GUM_SCALAR >* >& set) = 0;

      /// Returns the IO flags of a PRMClassElement<GUM_SCALAR>.
      /// @param elt The PRMClassElement<GUM_SCALAR>.
      /// @return elt's IO flags.
      /// @throw NotFound Raised if elt does not have any IO flags.
      virtual std::pair< bool, bool >& getIOFlag_(const PRMClassElement< GUM_SCALAR >& elt);

      /// Returns the IO flags of a PRMClassElement<GUM_SCALAR>.
      /// @param elt The PRMClassElement<GUM_SCALAR>.
      /// @return elt's IO flags.
      /// @throw NotFound Raised if elt does not have any IO flags.
      virtual const std::pair< bool, bool >&
         getIOFlag_(const PRMClassElement< GUM_SCALAR >& elt) const;

      /// Defines the IO flags of a PRMClassElement<GUM_SCALAR>.
      /// @param elt The PRMClassElement<GUM_SCALAR>.
      /// @param flags The IO flags of elt. Overwrite any existing flags.
      /// @return elt's IO flags.
      /// @throw NotFound Raised if elt does not have any IO flags.
      virtual void setIOFlag_(const PRMClassElement< GUM_SCALAR >& elt,
                              const std::pair< bool, bool >&       flags);

      /// Copy the IO Flags of c in this PRMClassElementContainer.
      /// @param c A PRMClassElementContainer.
      virtual void copyIOFlags_(const PRMClassElementContainer< GUM_SCALAR >& c);

      /// When a PRMClassElement<GUM_SCALAR> becomes an Output node we must
      /// update
      /// any the IO flags of every descendant of this PRMClassElementContainer.
      /// Note that after its declaration, input flags can not be changed and
      /// output flags can only become true.
      ///
      /// @param elt A PRMClassElement<GUM_SCALAR>.
      virtual void updateDescendants_(const PRMClassElement< GUM_SCALAR >& elt) = 0;

      private:
      /// input / output flags, useful when inheriting or copying.
      HashTable< std::string, std::pair< bool, bool > > _IOFlags_;
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class PRMClassElementContainer< double >;
#endif


  } /* namespace prm */

}   // namespace gum

/// @brief An << operator for PRMClassElementContainer.
/// Output in the graphviz-dot format.
template < typename GUM_SCALAR >
std::ostream& operator<<(std::ostream&                                           output,
                         const gum::prm::PRMClassElementContainer< GUM_SCALAR >& container);

#include <agrum/PRM/elements/PRMClassElementContainer_tpl.h>

#endif /* GUM_CLASS_ELEMENT_CONTAINER_H */
