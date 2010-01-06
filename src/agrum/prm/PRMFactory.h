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
// ============================================================================
#include <agrum/core/utils.h>
#include <agrum/core/exceptions.h>
// ============================================================================
#include <agrum/multidim/discreteVariable.h>
// ============================================================================
#include <agrum/prm/utils_prm.h>
#include <agrum/prm/PRMObject.h>
#include <agrum/prm/model.h>
#include <agrum/prm/PRM.h>
#include <agrum/prm/classElementContainer.h>
// ============================================================================
namespace gum {
namespace prm {
/**
 * @class PRMFactory
 * @brief Factory which builds a PRM.
 *
 * Follows the "Don't call me, I'll call you" paradigm. You just need to call
 * the methods in the right order to build your PRM.
 *
 * The Factory always create one PRM which is not deleted with the factory, so
 * be very careful to delete the PRM built by this factory.
 *
 * The factory does not allow any sequence of calls, if you call a method when
 * the factory is an incoherent state it will raise a FactoryInvalidState
 * exception.
 *
 * @see PRM PRMObject
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
     */
    PRM* prm();

    /**
     * Returns the type of the object begin built.
     * @throw NotFound if no type is being built.
     */
    PRMObject::ObjectType current() const;

    /**
     * Close current object beegin built.
     * The object is remove from any ohter object containing it and delete
     * its.
     */
    void closeCurrent();

    /**
     * Return the current package name.
     */
    const std::string& currentPackage() const;

    ///@}
    // ======================================================================
    /// @name Package construction methods.
    // ======================================================================
    /// @{

    /**
     * Define the current package.
     */
    void setPackage(const std::string& name);

    /// @}
    // ======================================================================
    /// @name Type construction methods.
    // ======================================================================
    /// @{

    /**
     * Start a discrete type declaration.
     */
    void startDiscreteType(const std::string& name);

    /**
     * Start a discrete subtype declaration.
     */
    void startDiscreteType(const std::string& name,
                           const std::string& super);

    /**
     * Add a label to the current discrete type.
     */
    void addLabel(const std::string& l);

    /**
     * Add a label to the current discrete type.
     * @param extends The super type label for which l is an extension.
     */
    void addLabel(const std::string& l, std::string& extends);

    /**
     * End the current discrete type declaration.
     */
    void endDiscreteType();

    /**
     * Add a type in the given namespace.
     * @throw DupplicateElement Raised if type's name is already used.
     */
    void addType(const DiscreteVariable& type);

    /// @}
    // ======================================================================
    /// @name Class construction models.
    // ======================================================================
    /// @{

    /**
     * Tells the factory that we start a class declaration.
     * @brief Tells the factory that we start a class declaration.
     *
     * Use the default values if you do not want c to be a subclass or to
     * implement interfaces.
     *
     * @param c The class name.
     * @param extends The name of the super class of c.
     * @param implements The list of interface implemented by c.
     * @throw OperationNotAllowed Raised if the given operation is illegal.
     */
    void startClass(const std::string& c, const std::string& extends="",
                    const Set<std::string>* implements=0);

    /**
     * Tells the factory that we finished a class declaration.
     * @throw OperationNotAllowed Raised if the given operation is illegal.
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
     * @throw OperationNotAllowed Raised if the given operation is illegal.
     */
    void startInterface(const std::string& i, const std::string& extends="");

    /**
     * Tells the factory that we finished an interface declaration.
     * @throw OperationNotAllowed Raised if the given operation is illegal.
     */
    void endInterface();

    /// @}
    // ======================================================================
    /// @name  Attributes construction methods.
    // ======================================================================
    /// @{

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
    void setRawCPFByLines(const std::vector<float>& array);

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
     * [0.20, 0.80, // P(f|f, f) = 0.20 and P(t|f, f) = 0.80
     *  0.50, 0.50, // P(f|t, f) = 0.50 and P(t|t, f) = 0.50
     *  0.70, 0.30, // P(f|f, t) = 0.70 and P(t|f, t) = 0.30
     *  0.01, 0.99] // P(f|t, t) = 0.01 and P(t|t, t) = 0.99
     * @endcode
     *
     * @throw OperationNotAllowed Raised if the given operation is illegal.
     */
    void setRawCPFByLines(const std::vector<double>& array);

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
    void setRawCPFByColumns(const std::vector<float>& array);

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
     * //P(A|f,f),P(A|f,t),P(A|t,f),P(A|t,t)
     * [ 0.2,     0.7,     0.5,     0.01,
     *   0.8,     0.3,     0.5,     0.99]
     * @endcode
     *
     * See PRMFactory::setRawCPFByLines() for more details.
     *
     * @throw OperationNotAllowed Raised if the given operation is illegal.
     */
    void setRawCPFByColumns(const std::vector<double>& array);

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
                      const std::vector<float>& values);

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
                      const std::vector<double>& values);

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
     * Add a parameter to the current class.
     *
     * A parameter is an Attribute (aka mutable Attribute) with no parents
     * and with a CPF filled with 1.
     *
     * @param type The type of this parameter.
     * @param name The name of this parameter.
     */
    void addParameter(const std::string& type, const std::string& name);

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
                      std::string value);

    /// @}
    // ======================================================================
    /// @name Aggregator construction methods.
    // ======================================================================
    /// @{

    /**
     * @brief Add an aggregator in the current declared class.
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
    /// @name Models constructions methods.
    // ======================================================================
    /// @{

    /**
     * Tells the factory that we started declaring a model.
     * @throw OperationNotAllowed Raised if the given operation is illegal.
     */
    void startModel(const std::string& name);

    /**
     * Tells the factory that we finished declaring a model.
     * @throw OperationNotAllowed Raised if the given operation is illegal.
     */
    void endModel();

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

    /// Raise a OperationNotAllowed for a undeclared Object.
    void __throwNotDeclared(PRMObject::ObjectType obj_type,
                            const std::string& name);

    /// Raise a OperationNotAllowed for a undeclared Object.
    void __throwNotDeclared(ClassElement::ClassElementType obj_type,
                            const std::string& name);

    // Raise a OperationNotAllowed for a wrong type in a given object.
    // @param wrong_type The misused type.
    // @param name The name of the misused type.
    // @param in The object's type in the wrong_type is misused.
    void __throwWrongType(PRMObject::ObjectType wrong_type,
                          const std::string& name,
                          PRMObject::ObjectType in);

    // Raise a OperationNotAllowed for a wrong type in a given object.
    // @param wrong_type The misused type.
    // @param name The name of the misused type.
    // @param in The object's type in the wrong_type is misused.
    void __throwWrongType(ClassElement::ClassElementType wrong_type,
                          const std::string& name,
                          ClassElement::ClassElementType in);

    /// Return a pointer on a PRMObject at __stack.size() - i position after
    /// checking the type of the object given obj_type.
    /// @throw FactoryInvalidState Raised if the stack isn't consistent with the 
    //                             current declaration.
    PRMObject* __checkStack(Idx i, PRMObject::ObjectType obj_type);

    /// Shortcut for casting a PRMObject in a Class from the stack with type
    /// checking.
    Type* __checkStackType(Idx i);

    /// Shortcut for casting a PRMObject in a Class from the stack with type
    /// checking.
    Class* __checkStackClassOrInterface(Idx i);

    /// Shortcut for casting a PRMObject in a Class from the stack with type
    /// checking.
    Class* __checkStackClass(Idx i);

    /// Shortcut for casting a PRMObject in an interface (aka Class) from the stack
    /// with type checking.
    Class* __checkStackInterface(Idx i);

    // Shortcut for casting a PRMObject in a ReferenceSlot from the stack with 
    // type checking.
    ReferenceSlot* __checkStackRefSlot(Idx i);

    /// Shortcut for casting an PRMObject in an Attribute from the stack with
    /// type checking.
    Attribute* __checkStackAttr(Idx i);

    // Shortcut for casting an PRMObject in an Aggregate from the stack with 
    // type checking.
    Aggregate* __checkStackAgg(Idx i);

    // Shortcut for casting an PRMObject in an Model from the stack with 
    // type checking.
    Model* __checkStackModel(Idx i);

    // Shortcut for casting an PRMObject in an SlotChain from the stack with 
    // type checking.
    SlotChain* __checkStackSC(Idx i);

    /// @brief check the type of an aggregate's parameters.
    /// @throw WrongType Raised if the aggregate's parameters or not of the good type.
    void __checkAggType_or_and(Class* c, const std::vector<std::string>& chains,
                                         const std::vector<std::string>& params);

    /// @brief check the type of an aggregate's parameters.
    /// @return the label on which the aggregate applies.
    /// @throw WrongType Raised if the aggregate's parameters or not of the good type.
    /// @throw OperationNotAllowed Raised if the label on which must be applied the
    ///                            aggregate is unusable.
    Idx __checkAggType_exists_count(Class* c, const std::vector<std::string>& chains,
                                              const std::vector<std::string>& params);

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
    Type* __retrieveType(const std::string& name);

    /// Returns a pointer on a class given it's name. Used when building 
    /// models, meaning that the class name can either be local (need to
    /// add the current prefix) or global (no prefix needed).
    /// @throw NotFound If no class matching the name is found.
    /// @see PRMFactory::__retrieveType 
    Class* __retrieveClass(const std::string& name);

    /// @}
    // ======================================================================
    ///  @name Private methods handling Class and ClassElement creation.
    // ======================================================================
    /// @{

    /// @brief This methods build a SlotChain given a starting element and a chain (name)
    SlotChain* __buildSlotChain(Class* start, const std::string& name);

    /// @brief Add an aggregator taking no parameters.
    ///
    /// This method is used for "min", "max", "or" and "and" aggregators.
    /// We suppose that the type has been already checked for "and" and "or" (it must
    /// be boolean).
    /// @throw WrongType Raised if one of the chain is not of the good type.
    void __addAggregatorNoParam(const std::string& name,
                                const std::string& agg_type,
                                const std::vector<std::string>& chains);

    /// @brief Add an aggregator taking one parameters.
    ///
    /// This method is used for "exists", and "forall" aggregators.
    ///
    /// @param rv_type The random variable type of this aggregator.
    /// @param name The name of this aggregator.
    /// @param agg_type The type of this aggregator.
    /// @param chains The list of slot chains on which this aggregator applies.
    /// @param label The label on which this aggregator applies.
    /// @throw WrongType Raised if one of the chain is not of the good type.
    void __addAggregatorOneParam(const std::string& name,
                                 const std::string& agg_type,
                                 const std::vector<std::string>& chains,
                                 Idx label);

    /// @}
    // ======================================================================
    ///  @name Private methods handling Model and Instance creation.
    // ======================================================================
    /// @{

    /// Builds all SlotChain<Instance> in the given model.
    /// @throw OperationNotAllowed If reference slots are left un affected
    void __buildSlotChains(Model* model);

    /// Builds all Aggregates CPF in the given model.
    /// This must be called after all the slot chains have been generated.
    void __buildAggregateCPF(Model* model);

    /// Instantiate a slot chain in the given instance
    void __instantiateSlotChain(Model* model,
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

    /// @brief Set a reference between two instances.
    /// We suppose that right_instance has been checked to be an Instance
    /// in the current Model.
    ///
    /// @throw OperationNotAllowed Raised if the operation isn't possible.
    /// @throw NotFound Raised if on of the left elements is missing.
    void __setReferenceSlotWithInstance(const std::string& left_instance,
                                        const std::string& left_reference,
                                        const std::string& right_instance);

    /// @brief Set a reference between an Instance and an Array of Instance.
    /// We suppose that right_instance has been checked to be an array
    /// in the current Model.
    ///
    /// @throw OperationNotAllowed Raised if the operation isn't possible.
    /// @throw NotFound Raised if on of the left elements is missing.
    void __setReferenceSlotWithArray(const std::string& left_instance,
                                     const std::string& left_reference,
                                     const std::string& right_instance);

    /// @}
    // ======================================================================
    /// @name Private members.
    // ======================================================================
    /// @{

    /// The prefix used for classes and types names. It is normally the
    /// namespace of the corresponding compilation unit.
    std::string __prefix;

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
