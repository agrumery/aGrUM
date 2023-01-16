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
 * @brief Headers of PRMFactory.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_PRM_FACTORY_H
#define GUM_PRM_FACTORY_H

#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include <agrum/agrum.h>


#include <agrum/PRM/IPRMFactory.h>

namespace gum {

  namespace prm {
    /**
     * @class PRMFactory
     * @brief Factory which builds a PRM<GUM_SCALAR>.
     *
     * The Factory always create one PRM<GUM_SCALAR> which is not deleted with
     *the
     *factory, so
     * be very careful to delete the PRM<GUM_SCALAR> built by this factory.
     *
     * The factory does not allow any sequence of calls, if you call a method
     *when
     * the factory is in an incoherent state it will raise a FactoryInvalidState
     * exception.
     *
     * Keep in mind that most methods could raise gum::FatalError if something
     * unexpected happens, since the framework is meant to evolve it is
     *possible.
     *
     * @see PRM<GUM_SCALAR> PRMObject
     * @ingroup prm_group
     */
    // ==========================================================================
    template < typename GUM_SCALAR >
    class PRMFactory: public IPRMFactory {
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
       * This constructor let you set on which PRM instance the factory works.
       */
      PRMFactory(PRM< GUM_SCALAR >* prm);

      /// Copy constructor. Don't use it.
      PRMFactory(const PRMFactory& from) = delete;

      /// Copy operator. Don't use it.
      PRMFactory& operator=(const PRMFactory& from) = delete;
      /**
       * Destructor.
       */
      virtual ~PRMFactory();

      /// @}
      // ========================================================================
      /// @name Getters & Setters
      // ========================================================================
      /// @{

      /**
       * Returns a pointer on the PRM<GUM_SCALAR> created by this factory.
       * This pointer will not be deleted when the factory is destroyed.
       *
       * @return Returns the PRM<GUM_SCALAR> created by this factory.
       */
      PRM< GUM_SCALAR >* prm() const;

      /**
       * @return Returns the PRMObject type of the object begin built.
       * @throw NotFound if no type is being built.
       */
      virtual PRMObject::prm_type currentType() const override;

      /**
       * @return the current PRMObject being built by this factory.
       * @throw NotFound if nothing is being built.
       */
      virtual PRMObject*       getCurrent() override;
      virtual const PRMObject* getCurrent() const override;

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
      virtual PRMObject* closeCurrent() override;

      /**
       * @return Returns the current package name.
       */
      virtual std::string currentPackage() const override;

      /**
       * @brief Returns a reference over a Class<GUM_SCALAR> given its name.
       *
       * This methods adds if necessary the current package as a prefix to name.
       * @param name The name of the Class<GUM_SCALAR>.
       * @return the Class<GUM_SCALAR> with the given name.
       * @throw NotFound if no Class<GUM_SCALAR> matches the given name.
       */
      PRMClass< GUM_SCALAR >& retrieveClass(const std::string& name);

      /**
       * @brief Returns a reference over a PRMType given its name.
       *
       * This methods adds if necessary the current package as a prefix to name.
       * @param name The name of the PRMType.
       * @return the PRMType with the given name.
       * @throw NotFound if no PRMType matches the given name.
       */
      PRMType& retrieveType(const std::string& name);


      /* @brief Retrieve the common PRMType of a vector of
       *PRMClassElement<GUM_SCALAR>.
       *
       * The vector elts must only contains PRMClassElement<GUM_SCALAR> with a
       *PRMType, i.e.
       * PRMAttribute<GUM_SCALAR>, PRMAggregate and PRMSlotChain<GUM_SCALAR>. If
       *not, a
       *WrongClassElement is
       * raised.
       *
       * A common PRMType is PRMType t such as
       *t.isSuperTypeOf(elts[i]) for
       * 0 < i < elts.size(), where elts is a PRMType container.
       *
       * @param elts A vector of PRMClassElement<GUM_SCALAR>.
       * @return the common super PRMType of all
       *PRMClassElement<GUM_SCALAR> un elts.
       *
       * @throw WrongClassElement Raised if elts contains a
       *PRMClassElement<GUM_SCALAR>
       *                          without a PRMType.
       * @throw NotFound Raised if there exists no common super type of all
       *                 PRMClassElement<GUM_SCALAR> in elts.
       */
      PRMType& retrieveCommonType(const std::vector< PRMClassElement< GUM_SCALAR >* >& elts);

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
      virtual void pushPackage(const std::string& name) override;

      /**
       * @brief Pop the current package from the package stack.
       * @return the popped package or an empty string if there was
       *         nothing to pop.
       */
      virtual std::string popPackage() override;

      /// @}
      // ======================================================================
      /// @name Package construction methods.
      // ======================================================================
      /// @{

      /**
       * @brief Add an import for namespace lookup.
       *
       * When loading a module, you should push all import declarations using
       * this method. The order in which you add imports will impact name
       * resolution (first found, first used).
       *
       * Imports are sync with packages: when you push a new package a new empty
       * list of imports is added. When you pop a package the current list of
       * imports is discarded and the previous one is restored.
       *
       * @param name The name of the package for all further objects.
       */
      virtual void addImport(const std::string& name) override;

      /// @}
      // ======================================================================
      /// @name PRMType construction methods.
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
      virtual void startDiscreteType(const std::string& name, std::string super = "") override;

      /**
       * Add a label to the current discrete type.
       *
       * @param l The label value.
       * @param ext The super type label for which l is an extension.
       *
       * @throw OperationNotAllowed Raised if the current type does not have a
       * super type, see PRMFactory::addLabel(const std::string&).
       * @throw DuplicateElement Raised if a label with the same value already
       * exists.
       * @throw NotFound Raised if ext does not match any label in the
       * current type's super type.
       */
      virtual void addLabel(const std::string& l, std::string ext = "") override;

      /**
       * End the current discrete type declaration.
       *
       * @throw OperationNotAllowed Raised if the current type is not a valid
       *                            discrete type.
       */
      virtual void endDiscreteType() override;

      /**
       * Start a discretized type declaration.
       *
       * @param name The discretized type's name.
       *
       * @throw DuplicateElement Raised if an object with the same name
       *                         already exists.
       */
      virtual void startDiscretizedType(const std::string& name) override;

      /**
       * Add a tick to the current discretized type.
       *
       * @param tick The tick value.
       */
      virtual void addTick(double tick) override;

      /**
       * End the current discretized type declaration.
       *
       * @throw OperationNotAllowed Raised if the current type is not a valid
       *                            discrete type.
       */
      virtual void endDiscretizedType() override;

      /**
       * Add a range variable type declaration.
       *
       * @param name The variable's name
       * @param minVal The variable's lower bound.
       * @param maxVal The variable's upper bound.
       *
       * @throw DuplicateElement Raised if an object with the same name
       *                         already exists.
       * @throw OperationNotAllowed Raised if the range variable is not a valid
       * discrete type.
       */
      virtual void addRangeType(const std::string& name, long minVal, long maxVal) override;
      /// @}

      ///@name several checks for parsers
      /// @{
      virtual bool isClassOrInterface(const std::string& type) const override;
      virtual bool isArrayInCurrentSystem(const std::string& name) const override;
      /// @}


      /// @name Class<GUM_SCALAR> construction models.
      /// @{

      /**
       * @brief Tells the factory that we start a class declaration.
       *
       * Use the default values if you do not want c to be a subclass or to
       * implement interfaces.
       *
       * @param c The class name.
       * @param ext The name of the super class of c.
       * @param implements The list of interface implemented by c.
       * @param delayInheritance If true, the created gum::prm::Class
       * inheritance will be delayed.
       *
       * @throw OperationNotAllowed Raised if the given operation is illegal.
       */
      virtual void startClass(const std::string&        c,
                              const std::string&        ext              = "",
                              const Set< std::string >* implements       = nullptr,
                              bool                      delayInheritance = false) override;

      /**
       * Continue the declaration of a class.
       */
      virtual void continueClass(const std::string& c) override;

      /**
       * Tells the factory that we finished a class declaration.
       *
       * @throw PRMTypeError Raised if the current Class<GUM_SCALAR> does not
       *respect one of
       *                  it's PRMInterface<GUM_SCALAR>.
       */
      virtual void endClass(bool checkImplementations = true) override;

      /// @}
      // ======================================================================
      /// @name PRMInterface<GUM_SCALAR> construction models.
      // ======================================================================
      /// @{

      /**
       * @brief Tells the factory that we start an interface declaration.
       *
       * Use the default values if you do not want c to be a subclass or to
       * implement interfaces.
       *
       * @param i The interface name.
       * @param ext The name of the super interface of i.
       * @param delayInheritance If true, the created gum::prm::PRMInterface
       * inheritance will be delayed.
       *
       * @throw NotFound Raised if ext does not match any declared
       *                 PRMInterface<GUM_SCALAR>.
       */
      virtual void startInterface(const std::string& i,
                                  const std::string& ext              = "",
                                  bool               delayInheritance = false) override;

      /**
       * Continue the declaration of an interface.
       */
      virtual void continueInterface(const std::string& name) override;

      /**
       * @brief Add an attribute to an interface.
       *
       */
      virtual void addAttribute(const std::string& type, const std::string& name) override;

      /**
       * Tells the factory that we finished an interface declaration.
       */
      virtual void endInterface() override;

      /// @}
      // ======================================================================
      /// @name  Attributes construction methods.
      // ======================================================================
      /// @{

      /** @brief Add an already created attribute to the current class.
       *
       * Use this method when you must add functions, such as Noisy-Or.
       *
       * Use this method when you need to add functions, such as Noisy-Or.  The
       * attribute CPT is checked for parents and arcs will be added using the
       * DiscreteVariable pointers, thus be careful to use those of the
       * attributes, aggregates and slotchains of the current class.
       * gum::prm::Class<GUM_SCALAR>::insertArc() will be called for each found
       * parent of attr, so you should overload
       * gum::prm::PRMAttribute<GUM_SCALAR>::addParent() to prevent duplication
       * errors. Such class exists: gum::prm::PRMFuncAttribute .
       *
       * The pointer is given to the class, so do not worry about deleting it.
       *
       * @param attr The attribute added to the current class.
       *
       * @throw NotFound Raised if one of the DiscreteVariable in attr CPT does
       *                 not match any PRMClassElement<GUM_SCALAR> in this.
       */
      virtual void addAttribute(PRMAttribute< GUM_SCALAR >* attr);

      /**
       * Tells the factory that we start an attribute declaration.
       * @throw FatalError Raised if the given operation is illegal.
       */
      virtual void startAttribute(const std::string& type,
                                  const std::string& name,
                                  bool               scalar_atttr = false) override;

      /**
       * Continues the declaration of an attribute.
       */
      virtual void continueAttribute(const std::string& name) override;

      /**
       * Tells the factory that we add a parent to the current declared
       * attribute.
       * The name can refer to an already declared attribute or aggregate of the
       * same class. It can also be a slot chain only if there is no multiple
       * reference in it.
       * @throw OperationNotAllowed Raised if the given operation is illegal.
       */
      virtual void addParent(const std::string& name) override;

      /**
       * Gives the factory the CPF in its raw form.
       *
       * The creation of the CPF is left to the factory because we do not know
       * what level of complexity for CPF implementation can be handled by the
       * PRM<GUM_SCALAR> implementation.
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
      void setRawCPFByLines(const std::vector< GUM_SCALAR >& array);

      /**
       * @brief  Gives the factory the CPF in its raw form.
       *
       * The creation of the CPF is left to the factory because we do not know
       * what level of complexity for CPF implementation can be handled by the
       * PRM<GUM_SCALAR> implementation.
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
      void setRawCPFByColumns(const std::vector< GUM_SCALAR >& array);

      /**
       * @brief Fills the CPF using a rule.
       *
       * The labels vector is filled with one of each parent's labels or
       * with a wildcard ("*"). If a wildcard is used then all values of the
       * corresponding parents are used. The sequence of parents must be the
       * declaration order used when adding the current attribute's parents.
       *
       * @param labels The value of each parents.
       * @param values The probability values of the current attribute given
       *               the values in parents.
       */
      virtual void setCPFByRule(const std::vector< std::string >& labels,
                                const std::vector< GUM_SCALAR >&  values);

      /**
       * @brief Fills the CPF using a rule and gum::Formula.
       *
       * The labels vector is filled with one of each parent's labels or
       * with a wildcard ("*"). If a wildcard is used then all values of the
       * corresponding parents are used. The sequence of parents must be the
       * declaration order used when adding the current attribute's parents.
       *
       * @param labels The value of each parents.
       * @param values The probability values of the current attribute given
       *               the values in parents.
       */
      virtual void setCPFByRule(const std::vector< std::string >& labels,
                                const std::vector< std::string >& values) override;

      /**
       * @brief Gives the factory the CPF in its raw form.
       *
       * The creation of the CPF is left to the factory because we do not know
       * what level of complexity for CPF implementation can be handled by the
       * PRM<GUM_SCALAR> implementation.
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
      virtual void setRawCPFByFloatLines(const std::vector< float >& array) override;

      /**
       * @brief Gives the factory the CPF in its raw form.
       *
       * The creation of the CPF is left to the factory because we do not know
       * what level of complexity for CPF implementation can be handled by the
       * PRM<GUM_SCALAR> implementation.
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
      virtual void setRawCPFByFloatColumns(const std::vector< float >& array) override;

      /**
       * @brief  Gives the factory the CPF in its raw form use gum::Formula.
       *
       * The creation of the CPF is left to the factory because we do not know
       * what level of complexity for CPF implementation can be handled by the
       * PRM<GUM_SCALAR> implementation.
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
      virtual void setRawCPFByColumns(const std::vector< std::string >& array) override;

      /**
       * @brief Gives the factory the CPF in its raw form using gum::Formula.
       *
       * The creation of the CPF is left to the factory because we do not know
       * what level of complexity for CPF implementation can be handled by the
       * PRM<GUM_SCALAR> implementation.
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
      virtual void setRawCPFByLines(const std::vector< std::string >& array) override;

      /**
       * @brief Fills the CPF using a rule.
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
      virtual void setCPFByFloatRule(const std::vector< std::string >& labels,
                                     const std::vector< float >&       values) override;

      /**
       * Tells the factory that we finished declaring an attribute.
       * @throw OperationNotAllowed Raised if the given operation is illegal.
       */
      virtual void endAttribute() override;

      /// @}
      // ======================================================================
      /// @name Parameters construction methods.
      // ======================================================================
      /// @{

      /**
       * Add a parameter to the current class with a default value.
       *
       * A parameter with a default value is an PRMAttribute<GUM_SCALAR> (aka
       *mutable PRMAttribute<GUM_SCALAR>)
       * with no parents and with a CPF filled with 1.
       *
       * @param type The type of this parameter.
       * @param name The name of this parameter.
       * @param value The label used as default value for this parameter.
       */
      void addParameter(const std::string& type, const std::string& name, double value) override;
      /// @}
      // ======================================================================
      /// @name Aggregator and function construction methods.
      // ======================================================================
      /// @{

      /**
       * @brief Add an aggregator in the current declared class.
       *
       * If at least one parent of an aggregator is a PRMSlotChain<GUM_SCALAR>,
       * then all of it's parents must be PRMSlotChain<GUM_SCALAR>. When an
       * aggregator parents are only composed of PRMAttribute<GUM_SCALAR> and
       * PRMAggregate, then it is directly added as an PRMAttribute<GUM_SCALAR>
       * to
       * it's Class<GUM_SCALAR>.
       *
       * @param name The name of this aggregator.
       * @param agg_type The name of the aggregator type of this aggregator.
       * @param chains The set of chains on which this aggregate applies.
       * @param params The list of parameters for this aggregator.
       * @param type Some aggregators have a user defined type, use this
       * parameter to define it.
       *
       * @throw OperationNotAllowed Raised if one or more parameters misses or
       * are not correct.
       * @throw TypeError Raised if the aggregator's type or one or more of the
       * chains are not of the good type.
       */
      virtual void addAggregator(const std::string&                name,
                                 const std::string&                agg_type,
                                 const std::vector< std::string >& chains,
                                 const std::vector< std::string >& params,
                                 std::string                       type = "") override;

      /**
       * @brief Start an aggregator declaration.
       *
       * @param name The aggregator's name.
       * @param agg_type The aggregtor's type (@see Aggregate::agg_type()).
       * @param rv_type The aggregator's random variable type (@see
       * Aggregate::type()).
       * @param params The aggregator's parameters.
       */
      void startAggregator(const std::string&                name,
                           const std::string&                agg_type,
                           const std::string&                rv_type,
                           const std::vector< std::string >& params);

      /**
       * @brief Conitnues an aggregator declaration.
       */
      void continueAggregator(const std::string& name);


      /**
       * @brief Finishes an aggregate declaration.
       */
      void endAggregator();

      /**
       * @brief Add a compound noisy-or as an PRMAttribute<GUM_SCALAR> to the
       *current Class<GUM_SCALAR>.
       *
       * The type of a noisy-or must be a boolean.
       *
       * @param name the name of the PRMAttribute<GUM_SCALAR> added as a
       *noisy-or.
       * @param chains the list of parents of the noisy-or.
       * @param numbers the list of weights for each parent. Can consist of only
       *                one value which will be applied to all the parents.
       * @param leak the leak
       * @param label the label on which the noisy-or applies, can be an empty
       *              string (the noisy-or will behave as if chains are all
       *booleans).
       *
       * @throw NotFound Raised if one of the chains or the label is not found.
       * @throw FactoryInvalidState Raised if a Class<GUM_SCALAR> is not the
       *current declared
       *                            PRMObject.
       * @throw OperationNotAllowed Raised if for some reasons the parameters
       *are
       *                            invalid.
       */
      virtual void addNoisyOrCompound(const std::string&                name,
                                      const std::vector< std::string >& chains,
                                      const std::vector< float >&       numbers,
                                      float                             leak,
                                      const std::vector< std::string >& label) override;
      /// @}
      // ======================================================================
      /// @name PRMReferenceSlot<GUM_SCALAR> construction methods.
      // ======================================================================
      /// @{

      /**
       * Tells the factory that we started declaring a slot.
       * @throw OperationNotAllowed Raised if the given operation is illegal.
       */
      virtual void
         addReferenceSlot(const std::string& type, const std::string& name, bool isArray) override;

      /// @}
      // ======================================================================
      /// @name Systems constructions methods.
      // ======================================================================
      /// @{

      /**
       * Tells the factory that we started declaring a model.
       * @throw OperationNotAllowed Raised if the given operation is illegal.
       */
      virtual void startSystem(const std::string& name) override;

      /**
       * Tells the factory that we finished declaring a model.
       * @throw OperationNotAllowed Raised if the given operation is illegal.
       */
      virtual void endSystem() override;

      /**
       * Add an instance to the model.
       */
      virtual void addInstance(const std::string& type, const std::string& name) override;

      /**
       * Add an instance with params as values of type's parameters.
       */
      void addInstance(const std::string&                      type,
                       const std::string&                      name,
                       const HashTable< std::string, double >& params) override;

      /**
       * Creates an array with the given number of instances of the given type.
       * PRMInstance<GUM_SCALAR> are name using "name" as prefix and adding the
       * suffix "[i]",
       * with "i" being the position of the instance in the array.
       */
      virtual void addArray(const std::string& type, const std::string& name, Size size) override;

      /**
       * Add an instance to an array.
       */
      virtual void incArray(const std::string& l_i, const std::string& r_i) override;

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
      virtual void setReferenceSlot(const std::string& left_instance,
                                    const std::string& left_reference,
                                    const std::string& right_instance) override;

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
       * @param r_i The name of an instance or an array of instances in the
       *model.
       */
      virtual void setReferenceSlot(const std::string& l_i, const std::string& r_i) override;

      /// @}

      private:
      // TODELETE /// Decompose a string in v using dots ('.') as delimiters.
      // TODELETE void  _fill_sc_in_vector_(std::vector< std::string >& v,
      // TODELETE                                const std::string&          str);

      // ======================================================================
      ///  @name Private methods for type checking and exception handling.
      // ======================================================================
      /// @{

      /// Adds  _prefix_ to str iff  _prefix_ != ""
      std::string _addPrefix_(const std::string& str) const;

      /// Return a pointer on a PRMObject at  _stack_.size() - i position after
      /// checking the type of the object given obj_type.
      /// @throw FactoryInvalidState Raised if the stack isn't consistent with
      /// the
      //                             current declaration.
      PRMObject* _checkStack_(Idx i, PRMObject::prm_type obj_type);

      PRMClassElement< GUM_SCALAR >*
         _checkStack_(Idx i, typename PRMClassElement< GUM_SCALAR >::ClassElementType obj_type);

      PRMClassElementContainer< GUM_SCALAR >* _checkStackContainter_(Idx i);

      /// @}
      // ======================================================================
      ///  @name Private getters
      // ======================================================================
      /// @{

      /// Returns a pointer on a PRMType given it's name. Since the
      /// type can
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
      PRMType* _retrieveType_(const std::string& name) const;

      /// Returns a pointer on a class given it's name. Used when building
      /// models, meaning that the class name can either be local (need to
      /// add the current prefix) or global (no prefix needed).
      /// @throw NotFound If no class matching the name is found.
      /// @see PRMFactory:: _retrieveType_
      PRMClass< GUM_SCALAR >* _retrieveClass_(const std::string& name) const;

      /// Returns a pointer on an interface given it's name. Used when building
      /// models, meaning that the interface name can either be local (need to
      /// add the current prefix) or global (no prefix needed).
      /// @throw NotFound If no class matching the name is found.
      /// @see PRMFactory:: _retrieveType_
      PRMInterface< GUM_SCALAR >* _retrieveInterface_(const std::string& name) const;

      /// @}
      // ======================================================================
      ///  @name Private methods handling Class<GUM_SCALAR> and
      ///  PRMClassElement<GUM_SCALAR> creation.
      // ======================================================================
      /// @{

      /// This methods build a PRMSlotChain<GUM_SCALAR> given a starting element
      /// and a string.
      ///
      /// @return Return a pointer over a PRMSlotChain<GUM_SCALAR> or 0 if no
      /// PRMSlotChain<GUM_SCALAR> could be built.
      ///
      PRMSlotChain< GUM_SCALAR >* _buildSlotChain_(PRMClassElementContainer< GUM_SCALAR >* start,
                                                   const std::string&                      name);

      /// @brief Retrieve inputs for an PRMAggregate.
      ///
      /// The vector chains contains names of the PRMAggregate inputs. If a name
      /// does not match an existing PRMClassElement<GUM_SCALAR> in c, then a
      /// call
      /// to
      /// PRMFactory:: _buildSlotChains_() is made. Such created
      /// PRMSlotChain<GUM_SCALAR> are
      /// added to c.
      ///
      /// @param c The class in which the PRMAggregate is defined.
      /// @param chains Vector of the PRMAggregate inputs names.
      /// @param inputs Vector filled with the PRMClassElement<GUM_SCALAR>
      /// matching
      /// the names
      ///               in chains.
      /// @return true if there was at least one slotchain in chains.
      ///
      /// @throw NotFound Raised if a name in chains does not match a legal
      ///                 PRMSlotChain<GUM_SCALAR> or an existing
      ///                 PRMClassElement<GUM_SCALAR> in c.
      bool _retrieveInputs_(PRMClass< GUM_SCALAR >*                        c,
                            const std::vector< std::string >&              chains,
                            std::vector< PRMClassElement< GUM_SCALAR >* >& inputs);

      /// @brief Retrieve the common PRMType of a vector of
      /// PRMClassElement<GUM_SCALAR>.
      ///
      /// The vector elts must only contains PRMClassElement<GUM_SCALAR> with a
      /// PRMType, i.e.
      /// PRMAttribute<GUM_SCALAR>, PRMAggregate and PRMSlotChain<GUM_SCALAR>.
      /// If not a
      /// WrongClassElement is
      /// raised.
      ///
      /// A common PRMType is PRMType t such as
      /// t.isSuperTypeOf(elts[i]) for
      /// 0 < i < elts.size(), where elts is a PRMType container.
      ///
      /// @param elts A vector of PRMClassElement<GUM_SCALAR>.
      /// @return Returns the common super PRMType of all
      /// PRMClassElement<GUM_SCALAR> un elts.
      //
      /// @throw WrongClassElement Raised if elts contains a
      /// PRMClassElement<GUM_SCALAR> without a PRMType.
      /// @throw NotFound Raised if there exists no common super type of all
      ///                 PRMClassElement<GUM_SCALAR> in elts.
      PRMType* _retrieveCommonType_(const std::vector< PRMClassElement< GUM_SCALAR >* >& elts);

      /// @brief Returns the inheritance depth of a PRMType.
      ///
      /// This used by PRMFactory:: _retrieveCommonType_.
      /// This returns 0 if t does not have a super type.
      ///
      /// @param t The PRMType for which we compute its depth.
      /// @return Returns the depth of t.
      int _typeDepth_(const PRMType* t);

      /// Check if c implements correctly all his interfaces.
      void _checkInterfaceImplementation_(PRMClass< GUM_SCALAR >* c);

      /// Add a parent to an attribute.
      void _addParent_(PRMClassElementContainer< GUM_SCALAR >* c,
                       PRMAttribute< GUM_SCALAR >*             agg,
                       const std::string&                      name);

      /// Add a parent to an aggregate.
      void _addParent_(PRMClass< GUM_SCALAR >*     c,
                       PRMAggregate< GUM_SCALAR >* agg,
                       const std::string&          name);
      /// @}
      // ======================================================================
      ///  @name Private methods handling PRMSystem<GUM_SCALAR> and
      ///  PRMInstance<GUM_SCALAR> creation.
      // ======================================================================
      /// @{

      /// Adds a instance to the current model.
      void _addInstance_(PRMClass< GUM_SCALAR >* type, const std::string& name);

      // Builds all PRMSlotChain<GUM_SCALAR><PRMInstance<GUM_SCALAR>> in the
      // given model.
      // @throw OperationNotAllowed If reference slots are left un affected
      // ERROR? void  _buildSlotChains_(PRMSystem< GUM_SCALAR >* model);

      /// Builds all Aggregates CPF in the given model.
      /// This must be called after all the slot chains have been generated.
      void _buildAggregateCPF_(PRMSystem< GUM_SCALAR >* model);

      /// Instantiate a slot chain in the given instance
      void _instantiateSlotChain_(PRMSystem< GUM_SCALAR >*        model,
                                  PRMInstance< GUM_SCALAR >*      inst,
                                  PRMReferenceSlot< GUM_SCALAR >* ref,
                                  PRMSlotChain< GUM_SCALAR >*     sc);

      /// Fill seq with the sequence of instance build using inst as the
      /// instantiation of sc-> _class_ and seeking each instantiation of
      /// reference in sc.
      /// @return Returns the name of the corresponding
      /// PRMSlotChain<GUM_SCALAR>.
      std::string _retrieveInstanceSequence_(PRMInstance< GUM_SCALAR >*              inst,
                                             Sequence< PRMInstance< GUM_SCALAR >* >& seq,
                                             PRMSlotChain< GUM_SCALAR >*             sc);

      /// @}
      // ======================================================================
      /// @name Private members.
      // ======================================================================
      /// @{

      /// The prefix used for classes and types names. It is normally the
      /// namespace of the corresponding compilation unit.
      std::vector< std::string > _packages_;

      /// Set of all declared namespaces.
      std::vector< List< std::string >* > _namespaces_;

      /// The pointer on the PRM<GUM_SCALAR> built by this factory.
      PRM< GUM_SCALAR >* _prm_;

      /// A stack used to keep track of created PRMObject.
      std::vector< PRMObject* > _stack_;

      /// A mapping between aggregators and their parameters
      HashTable< PRMAggregate< GUM_SCALAR >*, std::vector< std::string > > _agg_params_;

      /// @}
    };
  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/PRMFactory_tpl.h>

#endif /* GUM_PRM_FACTORY_H */
