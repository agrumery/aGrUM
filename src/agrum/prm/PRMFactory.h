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
 * @brief Headers of PRMFactory.
 *
 * @author Lionel TORTI
 */
// ============================================================================
#ifndef GUM_PRM_FACTORY_H
#define GUM_PRM_FACTORY_H
// ============================================================================
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <limits>
// ============================================================================
#include <agrum/core/utils.h>
#include <agrum/core/exceptions.h>
// ============================================================================
#include <agrum/multidim/discreteVariable.h>
#include <agrum/multidim/multiDimSparse.h>
// ============================================================================
#include <agrum/prm/utils_prm.h>
#include <agrum/prm/PRMObject.h>
#include <agrum/prm/system.h>
#include <agrum/prm/PRM.h>
#include <agrum/prm/classElementContainer.h>
// ============================================================================
namespace gum {
namespace prm {
/**
 * @class PRMFactory
 * @brief Factory which builds a PRM.
 *
 * The Factory always create one PRM which is not deleted with the factory, so
 * be very careful to delete the PRM built by this factory.
 *
 * The factory does not allow any sequence of calls, if you call a method when
 * the factory is in an incoherent state it will raise a FactoryInvalidState
 * exception.
 *
 * Keep in mind that most methods could raise gum::FatalError if something
 * unexpected happens, since the framework is meant to evolve it is possible.
 *
 * @see PRM PRMObject
 * @ingroup prm_group
 */
// ==========================================================================
class PRMFactory {
  public:
    // ======================================================================
    /// @name Constructors & destructor
    // ======================================================================
    /// @{

    /**
     * Default constructor.
     */
    PRMFactory();

    /**
     * Destructor.
     */
    ~PRMFactory();

    /// @}
    // ========================================================================
    /// @name Getters & Setters
    // ========================================================================
    /// @{

    /**
     * Returns a pointer on the PRM created by this factory.
     * This pointer will not be deleted when the factory is destroyed.
     *
     * @return Returns the PRM created by this factory.
     */
    PRM* prm() const;

    /**
     * @return Returns the PRMObject type of the object begin built.
     * @throw NotFound if no type is being built.
     */
    PRMObject::ObjectType currentType() const;

    /**
     * @return the current PRMObject being built by this factory.
     * @throw NotFound if nothing is being built.
     */
    PRMObject* getCurrent();

    /**
     * @brief Close current object being built.
     *
     * Nothing is done in particular except removing the current object from
     * the factory's stack. If the object was not added to a container it will
     * not be deleted properly, so you might want to delete it yourself (at
     * your own risks!).
     *
     * @return Returns the top most object removed in this factory's stack
     *         before. If their is no such object, 0 is returned.
     */
    PRMObject* closeCurrent();

    /**
     * @return Returns the current package name.
     */
    std::string currentPackage() const;

    /**
     * @brief Returns a reference over a Class given its name.
     *
     * This methods adds if necessary the current package as a prefix to name.
     * @param name The name of the Class.
     * @return the Class with the given name.
     * @throw NotFound if no Class matches the given name.
     */
    Class& retrieveClass(const std::string& name);

    /**
     * @brief Returns a reference over a Type given its name.
     *
     * This methods adds if necessary the current package as a prefix to name.
     * @param name The name of the Type.
     * @return the Type with the given name.
     * @throw NotFound if no Type matches the given name.
     */
    Type& retrieveType(const std::string& name);


    /* @brief Retrieve the common Type of a vector of ClassElement.
     *
     * The vector elts must only contains ClassElement with a Type, i.e.
     * Attribute, Aggregate and SlotChain. If not, a WrongClassElement is
     * raised.
     *
     * A common Type is Type t such as t.isSuperTypeOf(elts[i]) for
     * 0 < i < elts.size(), where elts is a Type container.
     *
     * @param elts A vector of ClassElement.
     * @return the common super Type of all ClassElement un elts.
     *
     * @throw WrongClassElement Raised if elts contains a ClassElement
     *                          without a Type.
     * @throw NotFound Raised if there exists no common super type of all
     *                 ClassElement in elts.
     */
    Type& retrieveCommonType(const std::vector<ClassElement*>& elts);

    ///@}
    // ======================================================================
    /// @name Package construction methods.
    // ======================================================================
    /// @{

    /**
     * @brief Define the current package.
     *
     * A stack is used to keep track of packages given the different
     * imports.
     *
     * @param name The name of the package for all further objects.
     */
    void pushPackage(const std::string& name);

    /**
     * @brief Pop the current package from the package stack.
     * @return the popped package or an empty string if there was
     *         nothing to pop.
     */
    std::string popPackage();

    /// @}
    // ======================================================================
    /// @name Type construction methods.
    // ======================================================================
    /// @{

    /**
     * Start a discrete subtype declaration.
     *
     * @param name The discrete type's name.
     * @param super The super type of the declared discrete type.
     *
     * @throw DuplicateElement Raised if an object with the same name
     *                         already exists.
     */
    void startDiscreteType(const std::string& name, std::string super="");

    /**
     * Add a label to the current discrete type.
     *
     * @param l The label value.
     * @param extends The super type label for which l is an extension.
     *
     * @throw OperationNotAllowed Raised if the current type does not have a super
     *                            type, see PRMFactory::addLabel(const std::string&).
     * @throw DuplicateElement Raised if a label with the same value
     *                         already exists.
     * @throw NotFound Raised if extends does not match any label in the current
     *                 type's super type.
     */
    void addLabel(const std::string& l, std::string extends="");

    /**
     * End the current discrete type declaration.
     *
     * @throw OperationNotAllowed Raised if the current type is not a valid
     *                            discrete type.
     */
    void endDiscreteType();

    /// @}
    // ======================================================================
    /// @name Class construction models.
    // ======================================================================
    /// @{

    /**
     * @brief Tells the factory that we start a class declaration.
     *
     * Use the default values if you do not want c to be a subclass or to
     * implement interfaces.
     *
     * @param c The class name.
     * @param extends The name of the super class of c.
     * @param implements The list of interface implemented by c.
     *
     * @throw OperationNotAllowed Raised if the given operation is illegal.
     */
    void startClass(const std::string& c, const std::string& extends="",
                    const Set<std::string>* implements=0);

    /**
     * Tells the factory that we finished a class declaration.
     *
     * @throw TypeError Raised if the current Class does not respect one of
     *                  it's Interface.
     */
    void endClass();

    /// @}
    // ======================================================================
    /// @name Interface construction models.
    // ======================================================================
    /// @{

    /**
     * @brief Tells the factory that we start an interface declaration.
     *
     * Use the default values if you do not want c to be a subclass or to
     * implement interfaces.
     *
     * @param i The interface name.
     * @param extends The name of the super interface of i.
     *
     * @throw NotFound Raised if extends does not match any declared
     *                 Interface.
     */
    void startInterface(const std::string& i, const std::string& extends="");

    /**
     * @brief Add an attribute to an interface.
     *
     */
    void addAttribute(const std::string& type, const std::string& name);

    /**
     * Tells the factory that we finished an interface declaration.
     */
    void endInterface();

    /// @}
    // ======================================================================
    /// @name  Attributes construction methods.
    // ======================================================================
    /// @{

    /** @brief Add an already created attribute to the current class.
     *
     * Use this method when you must add functions, such as Noisy-Or.
     *
     * Use this method when you need to add functions, such as Noisy-Or.
     * The attribute CPT is checked for parents and arcs will be added using
     * the DiscreteVariable pointers, thus be careful to use those of the
     * attributes, aggregates and slotchains of the current class.
     * gum::prm::Class::insertArc() will be called for each found parent of
     * attr, so you should overload gum::prm::Attribute::addParent() to prevent
     * duplication errors. Such class exists: gum::prm::FuncAttribute .
     *
     * The pointer is given to the class, so do not worry about deleting it.
     *
     * @param attr The attribute added to the current class.
     *
     * @throw NotFound Raised if one of the DiscreteVariable in attr CPT does
     *                 not match any ClassElement in this.
     */
    void addAttribute(Attribute* attr);

    /**
     * Tells the factory that we start an attribute declaration.
     * @throw FatalError Raised if the given operation is illegal.
     */
    void startAttribute(const std::string& type, const std::string& name);

    /**
     * Tells the factory that we add a parent to the current declared 
     * attribute.
     * The name can refer to an already declared attribute or aggregate of the
     * same class. It can also be a slot chain only if there is no multiple
     * reference in it.
     * @throw OperationNotAllowed Raised if the given operation is illegal.
     */
    void addParent(const std::string& name);

    /**
     * Gives the factory the CPF in its raw form.
     *
     * The creation of the CPF is left to the factory because we do not know
     * what level of complexity for CPF implementation can be handled by the
     * PRM implementation.
     *
     * How to fill a CPT? If you want to fill the CPT of P(A|B,C)
     * with A, B and C boolean variables ( {f, t}, the order is
     * important), then the following array is valid:
     * @code
     * [0.20, 0.80, // P(f|f, f) = 0.20 and P(t|f, f) = 0.80
     *  0.50, 0.50, // P(f|t, f) = 0.50 and P(t|t, f) = 0.50
     *  0.70, 0.30, // P(f|f, t) = 0.70 and P(t|f, t) = 0.30
     *  0.01, 0.99] // P(f|t, t) = 0.01 and P(t|t, t) = 0.99
     * @endcode
     *
     * @throw OperationNotAllowed Raised if the given operation is illegal.
     */
    void setRawCPFByLines(const std::vector<prm_float>& array);

    /**
     * @brief Not implemented!
     *
     * Gives the factory the CPF in its raw form.
     *
     * The creation of the CPF is left to the factory because we do not know 
     * what level of complexity for CPF implementation can be handled by the
     * PRM implementation.
     *
     * How to fill a CPT? If you want to fill the CPT of P(A|B,C)
     * with A, B and C boolean variables ( {f, t}, the order is
     * important), then the following array is valid:
     * @code
     * //P(A|f,f),P(A|f,t),P(A|t,f),P(A|t,t)
     * [ 0.2,     0.7,     0.5,     0.01,
     *   0.8,     0.3,     0.5,     0.99]
     * @endcode
     *
     * See PRMFactory::setRawCPFByLines() for more details.
     *
     * @throw OperationNotAllowed Raised if the given operation is illegal.
     */
    void setRawCPFByColumns(const std::vector<prm_float>& array);

    /**
     * Fills the CPF using a rule.
     *
     * The labels vector is filled with one of each parent's labels or
     * with a wildcard ("*"). If a wildcard is used then all values of the
     * corresponding parents are used. The sequence of parents must be the
     * declaration order used when adding the current attribute's parents.
     * 
     * @param labels The value of each parents.
     * @param values The probability values of the current attribute given
     *               the values in parenst.
     */
    void setCPFByRule(const std::vector<std::string>& labels,
                      const std::vector<prm_float>& values);

    /**
     * Tells the factory that we finished declaring an attribute.
     * @throw OperationNotAllowed Raised if the given operation is illegal.
     */
    void endAttribute();

    /// @}
    // ======================================================================
    /// @name Parameters construction methods.
    // ======================================================================
    /// @{

    /**
     * Add a parameter to the current class with a default value.
     *
     * A parameter with a default value is an Attribute (aka mutable Attribute)
     * with no parents and with a CPF filled with 1.
     *
     * @param type The type of this parameter.
     * @param name The name of this parameter.
     * @param value The label used as default value for this parameter.
     */
    void addParameter(const std::string& type,
                      const std::string& name,
                      std::string value="");

    /// @}
    // ======================================================================
    /// @name Aggregator and function construction methods.
    // ======================================================================
    /// @{

    /**
     * @brief Add an aggregator in the current declared class.
     *
     * If at least one parent of an aggregator is a SlotChain, then all of
     * it's parents must be SlotChain. When an aggregator parents are only
     * composed of Attribute and Aggregate, then it is directly added as an
     * Attribute to it's Class.
     *
     * @param name The name of this aggregator.
     * @param agg_type The name of the aggregator type of this aggregator.
     * @param chains The set of chains on which this aggregate applies.
     * @param params The list of parameters for this aggregator.
     *
     * @throw OperationNotAllowed Raised if one or more parameters misses or are not
     *                            correct.
     * @throw WrongType Raised if the aggregator's type or one or more of the chains
     *                  are not of the good type.
     */
    void addAggregator(const std::string& name,
                       const std::string& agg_type,
                       const std::vector<std::string>& chains,
                       const std::vector<std::string>& params);

    /**
     * @brief Add a compound noisy-or as an Attribute to the current Class.
     *
     * The type of a noisy-or must be a boolean.
     *
     * @param name the name of the Attribute added as a noisy-or.
     * @param chains the list of parents of the noisy-or.
     * @param numbers the list of weights for each parent. Can consist of only
     *                one value which will be applied to all the parents.
     * @param label the label on which the noisy-or applies, can be an empty
     *              string (the noisy-or will behave as if chains are all booleans).
     *
     * @throw NotFound Raised if one of the chains or the label is not found.
     * @throw FactoryInvalidState Raised if a Class is not the current declared
     *                            PRMObject.
     * @throw OperationNotAllowed Raised if for some reasons the parameters are
     *                            invalid.
     */
    void addNoisyOrCompound(const std::string& name,
                    const std::vector<std::string>& chains,
                    const std::vector<float>& numbers, float leak,
                    const std::vector<std::string>& label);
    /// @}
    // ======================================================================
    /// @name ReferenceSlot construction methods.
    // ======================================================================
    /// @{

    /**
     * Tells the factory that we started declaring a slot.
     * @throw OperationNotAllowed Raised if the given operation is illegal.
     */
    void addReferenceSlot(const std::string& type,
                          const std::string& name,
                          bool isArray);

    /// @}
    // ======================================================================
    /// @name Systems constructions methods.
    // ======================================================================
    /// @{

    /**
     * Tells the factory that we started declaring a model.
     * @throw OperationNotAllowed Raised if the given operation is illegal.
     */
    void startSystem(const std::string& name);

    /**
     * Tells the factory that we finished declaring a model.
     * @throw OperationNotAllowed Raised if the given operation is illegal.
     */
    void endSystem();

    /**
     * Add an instance to the model.
     */
    void addInstance(const std::string& type, const std::string& name);

    /**
     * Creates an array with the given number of instances of the given type.
     * Instance are name using "name" as prefix and adding the suffix "[i]",
     * with "i" being the position of the instance in the array.
     */
    void addArray(const std::string& type, const std::string& name, Size size);

    /**
     * Add an instance to an array.
     */
    void incArray(const std::string& l_i, const std::string& r_i);

    /**
     * @brief Instantiate a reference in the current model.
     *
     * This is equivalent to the following SKOOL instruction:
     * left_instance.left_reference = right_instance;
     *
     * right_instance's type must be compatible with the slot type of
     * left_reference.
     *
     * @param left_instance The name of an instance in the model.
     * @param left_reference The name of a reference of left_instance.
     * @param right_instance The name of an instance or an array of instances
     *                       in the model.
     */
    void setReferenceSlot(const std::string& left_instance,
                          const std::string& left_reference,
                          const std::string& right_instance);

    /**
     * @brief Instantiate a reference in the current model.
     *
     * This is equivalent to the following SKOOL instruction:
     * left_instance.left_reference = right_instance;
     *
     * right_instance's type must be compatible with the slot type of
     * left_reference.
     *
     * @param l_i A slot chain starting from an instance and ending over a
     *            reference slot.
     * @param r_i The name of an instance or an array of instances in the model.
     */
    void setReferenceSlot(const std::string& l_i, const std::string& r_i);

    /**
     * @brief define the value of a parameter.
     *
     * @param i An instance defined in the current system.
     * @param p A parameter of instance.
     * @param v A label of p's type.
     *
     * @throw NotFound Raised if i, p or v is not found.
     */
    void setParameter(const std::string& i, const std::string& p, const std::string& v);

    /// @}
  private:
    /// Copy constructor. Don't use it.
    PRMFactory(const PRMFactory& from);

    /// Copy operator. Don't use it.
    PRMFactory& operator=(const PRMFactory& from);

    /// Decompose a string in v using dots ('.') as delimiters.
    void __fill_sc_in_vector(std::vector<std::string>& v, const std::string& str);

    // ======================================================================
    ///  @name Private methods for type checking and exception handling.
    // ======================================================================
    /// @{

    /// Adds __prefix to str iff __prefix != ""
    std::string __addPrefix(const std::string& str) const;

    /// Raise a DuplicateElement if the name already exists.
    void __checkDuplicateName(const std::string& name);

    /// Return a pointer on a PRMObject at __stack.size() - i position after
    /// checking the type of the object given obj_type.
    /// @throw FactoryInvalidState Raised if the stack isn't consistent with the 
    //                             current declaration.
    PRMObject* __checkStack(Idx i, PRMObject::ObjectType obj_type);

    ClassElement* __checkStack(Idx i, ClassElement::ClassElementType obj_type);

    ClassElementContainer* __checkStackContainter(Idx i);

    /// @}
    // ======================================================================
    ///  @name Private getters
    // ======================================================================
    /// @{

    /// Returns a pointer on a Type given it's name. Since the type can
    /// be given either with it's local name (without the prefix), full name
    /// (with the prefix) or can come from an import unit, or maybe one day
    /// with a using declaration we need some processing to find it.
    /// 
    /// The following precedences on finding the name is used:
    ///   * name
    ///   * prefix.name
    ///   * import_1.name
    ///   * import_2.name
    ///   * ...
    ///   * import_N.name
    /// In the case a local name is used multiple time, it's preferable to
    /// use it's full name.
    /// @throw OperationNotAllowed If the type is undeclared.
    Type* __retrieveType(const std::string& name) const;

    /// Returns a pointer on a class given it's name. Used when building 
    /// models, meaning that the class name can either be local (need to
    /// add the current prefix) or global (no prefix needed).
    /// @throw NotFound If no class matching the name is found.
    /// @see PRMFactory::__retrieveType 
    Class* __retrieveClass(const std::string& name) const;

    /// Returns a pointer on an interface given it's name. Used when building 
    /// models, meaning that the interface name can either be local (need to
    /// add the current prefix) or global (no prefix needed).
    /// @throw NotFound If no class matching the name is found.
    /// @see PRMFactory::__retrieveType 
    Interface* __retrieveInterface(const std::string& name) const;

    /// @}
    // ======================================================================
    ///  @name Private methods handling Class and ClassElement creation.
    // ======================================================================
    /// @{

    /// This methods build a SlotChain given a starting element and a string.
    ///
    /// @return Return a pointer over a SlotChain or 0 if no SlotChain could
    ///         be built.
    ///
    SlotChain* __buildSlotChain(ClassElementContainer* start, const std::string& name);

    /// @brief Retrieve inputs for an Aggregate.
    ///
    /// The vector chains contains names of the Aggregate inputs. If a name
    /// does not match an existing ClassElement in c, then a call to
    /// PRMFactory::__buildSlotChains() is made. Such created SlotChain are
    /// added to c.
    ///
    /// @param c The class in which the Aggregate is defined.
    /// @param chains Vector of the Aggregate inputs names.
    /// @param inputs Vector filled with the ClassElement matching the names
    ///               in chains.
    /// @return true if there was at least one slotchain in chains.
    ///
    /// @throw NotFound Raised if a name in chains does not match a legal
    ///                 SlotChain or an existing ClassElement in c.
    bool __retrieveInputs(Class* c, const std::vector<std::string>& chains,
                          std::vector<ClassElement*>& inputs);

    /// @brief Retrieve the common Type of a vector of ClassElement.
    ///
    /// The vector elts must only contains ClassElement with a Type, i.e.
    /// Attribute, Aggregate and SlotChain. If not a WrongClassElement is
    /// raised.
    ///
    /// A common Type is Type t such as t.isSuperTypeOf(elts[i]) for
    /// 0 < i < elts.size(), where elts is a Type container.
    ///
    /// @param elts A vector of ClassElement.
    /// @return Returns the common super Type of all ClassElement un elts.
    //
    /// @throw WrongClassElement Raised if elts contains a ClassElement
    ///                          without a Type.
    /// @throw NotFound Raised if there exists no common super type of all
    ///                 ClassElement in elts.
    Type* __retrieveCommonType(const std::vector<ClassElement*>& elts);

    /// @brief Returns the inheritance depth of a Type.
    ///
    /// This used by PRMFactory::__retrieveCommonType.
    /// This returns 0 if t does not have a super type.
    ///
    /// @param t The Type for which we compute its depth.
    /// @return Returns the depth of t.
    int __typeDepth(const Type* t);

    /// @}
    // ======================================================================
    ///  @name Private methods handling System and Instance creation.
    // ======================================================================
    /// @{

    /// Builds all SlotChain<Instance> in the given model.
    /// @throw OperationNotAllowed If reference slots are left un affected
    void __buildSlotChains(System* model);

    /// Builds all Aggregates CPF in the given model.
    /// This must be called after all the slot chains have been generated.
    void __buildAggregateCPF(System* model);

    /// Instantiate a slot chain in the given instance
    void __instantiateSlotChain(System* model,
                                Instance* inst,
                                ReferenceSlot* ref,
                                SlotChain* sc);

    /// Fill seq with the sequence of instance build using inst as the 
    /// instantiation of sc->__class and seeking each instantiation of 
    /// reference in sc.
    /// @¶eturn Returns the name of the corresponding SlotChain.
    std::string __retrieveInstanceSequence(Instance* inst,
                                           Sequence<Instance*>& seq,
                                           SlotChain* sc);

    /// @}
    // ======================================================================
    /// @name Private members.
    // ======================================================================
    /// @{

    /// The prefix used for classes and types names. It is normally the
    /// namespace of the corresponding compilation unit.
    std::vector<std::string> __packages;

    /// Set of all declared namespaces.
    Set<std::string> __namespaces;

    /// The pointer on the PRM built by this factory.
    PRM* __prm;

    /// A stack used to keep track of created PRMObject.
    std::vector<PRMObject*> __stack;

    /// @}
};
} /* namespace prm */
} /* namespace gum */
// ============================================================================
#ifndef GUM_NO_INLINE
#include <agrum/prm/PRMFactory.inl>
#endif // GUM_NO_INLINE
// ============================================================================
#endif /* GUM_PRM_FACTORY_H */
// ============================================================================
