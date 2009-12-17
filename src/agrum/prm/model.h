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
 * @brief Headers of Model.
 *
 * @author Lionel TORTI
 */
// ============================================================================
#ifndef GUM_MODEL_H
#define GUM_MODEL_H
// ============================================================================
#include <utility>
// ============================================================================
#include <agrum/core/set.h>
#include <agrum/core/sequence.h>
#include <agrum/core/hashTable.h>
// ============================================================================
#include <agrum/BN/BayesNetFactory.h>
// ============================================================================
#include <agrum/prm/PRM.h>
// ============================================================================
namespace gum {
class ModelIterator;
class ModelConstIterator;

/**
 * @class Model model.h <agrum/prm/model.h>
 * @brief A Model is a container of Instance.
 */
class Model: public PRMObject {
  public:
    /// Iterators are friends
    friend class ModelIterator;
    friend class ModelConstIterator;
  // ========================================================================
  /// @name Constructors & destructor.
  // ========================================================================
    /// @{

    /// Default constructor.
    Model(const std::string& name);

    /// Destructor.
    virtual ~Model();

    /// @}
  // ========================================================================
  /// @name Getters & setters.
  // ========================================================================
    /// @{

    /// Returns the PRM type of this object.
    virtual ObjectType obj_type() const;

    /// Returns the number of instances in this model.
    Size size() const;

    /// Returns true if the given Class has instances.
    bool isInstantiated(const Class& c) const;

    /// Returns true if an instance with the given name exists.
    bool isInstance(const std::string& name) const;

    /// Returns true if an array with the given name exists.
    bool isArray(const std::string& name) const;

    // Instantiate the isntances.
    void instantiate();

    /**
     * Returns the grounded Bayesian Network of this model.
     * @param factory The factory used to build the grounded Bayesian Network.
     */
    void groundedBN(BayesNetFactory<prm_float>& factory) const;

    /// @}
  // ========================================================================
  /// @name Instance handling.
  // ========================================================================
    /// @{

    /// Returns a reference over an instance given it's name
    /// @throw NotFound Raised if the no instance matches name.
    Instance& get(const std::string& name);

    /// Returns a constant reference over an instance given it's name
    /// @throw NotFound Raised if the no instance matches name.
    const Instance& get(const std::string& name) const;

    /// Returns the sequence of all instances of the given type
    /// @throw NotFound Raised if there is instantiation of type.
    const Set<Instance*>& get(const Class& type) const;

    /// Add an instance to this model
    /// @throw DuplicateElement Raised if an instance with same name already exists.
    void add(Instance* i);

    /// @}
  // ========================================================================
  /// @name Array handling.
  // ========================================================================
    /// @{

    /// Returns the sequence of instances of a given array.
    /// @throw NotFound Raised if no array matches name.
    const Sequence<Instance*>& getArray(const std::string& name) const;

    /// Returns the type of the given array.
    /// @throw NotFound Raised if no array matches name.
    Class& getArrayType(const std::string& name);

    /// Returns the type of the given array.
    /// @throw NotFound Raised if no array matches name.
    const Class& getArrayType(const std::string& name) const;

    /// @brief Add an instance to an array in this model.
    /// If the array doesn't exists it is created.
    /// @throw OperationNotAllowed Raised if i is not of the good type.
    /// @throw DuplicateElement Raised if an instance with same name already exists.
    void add(const std::string& array, Instance* i);

    /// @brief Add an array of instances in this model.
    /// If the array doesn't exists it is created.
    /// @throw DuplicateElement Raised if an existing array with the same name
    ///                         already exists.
    void addArray(const std::string& array, Class& type);

    /// @}
  // ========================================================================
  /// @name Iterators.
  // ========================================================================
    /// @{

    /// Iterator over the Instance of this Model.
    typedef ModelIterator iterator;

    /// Returns an iterator over the instances in this model.
    iterator begin();

    /// Constant Iterator over the Instance of this Model.
    typedef ModelConstIterator const_iterator;

    /// Returns a constant iterator over the instances in this model.
    const_iterator begin() const;

    /// @}
  private:
    /// Copy constructor. Don't use it.
    Model(const Model& from);

    /// Copy operator. Don't use it.
    Model& operator=(const Model& from);

  // ========================================================================
  /// @name Private instance handling methods and members.
  // ========================================================================
    /// @{

    /// The mapping between Instance and their names.
    HashTable<std::string, Instance*> __nameMap;

    /// Mapping between a class and all it's instance in this model
    HashTable<Class*, Set<Instance*>*> __instanceMap;

    /// Typedef of the pair of a Class and the sequence of it's instantiation.
    typedef std::pair< Class*, Sequence<Instance*>* > model_pair;

    /// Mapping between arrays and their name. The first element of the pair
    /// is the type of the array.
    HashTable< std::string, model_pair> __arrayMap;

    /// Add a instance to the set of instance of the same type. If necessary,
    /// creates the set.
    void __add(Instance* inst);

    /// @}
  // ========================================================================
  /// @name Ground BN private methods.
  // ========================================================================
    /// @{

    /// @brief Method which ground ReferenceSlot of an instance and add arcs
    ///        in the BayesNet.
    /// @param instance The instance grounded by this method.
    /// @param bn       The factory used to build the grounded BayesNet.
    void __groundRef(const Instance& instance,
                     BayesNetFactory<prm_float>& factory) const;

    /// @brief Method which ground Atttributes and Aggregators of
    ///        an instance.
    /// @param instance The instance grounded by this method.
    /// @param bn       The factory used to build the grounded BayesNet.
    void __groundAttr(const Instance& instance,
                      BayesNetFactory<prm_float>& factory) const;

    /// @brief Method which copy node's Potential of an instance to the grounded
    ///        Bayesian Network.
    /// @param instance The instance currently grounded.
    /// @param node     The node currently being grounded.
    /// @param factory  The factory used to build the grounded BayesNet.
    void __groundPotential(const Instance& instance, NodeId node,
                     BayesNetFactory<prm_float>& factory) const;

    /// @brief Ground an aggregator with the given name in the grounded
    ///        BayesNet.
    /// @param elt    The aggregator grounded.
    /// @param name   The aggregator's name in the grounded BayesNet.
    /// @param bn     The factory used to build the grounded BayesNet.
    void __groundAgg(const ClassElement& elt, const std::string& name,
                     BayesNetFactory<prm_float>& factory) const;
    /// @}
};

/**
 * @class ModelIterator model.h <agrum/prm/model.h>
 * @brief An iterator over the Instance of a Model.
 */
class ModelIterator
{
  public:
  // ========================================================================
  /// @name Constructors & destructor.
  // ========================================================================
    /// @{

    /**
     * Constructor for an iterator pointing on the begining of the Model's
     * Instance
     */
    ModelIterator(Model& model);

    /**
     * Copy constructor.
     */
    ModelIterator(const ModelIterator& from);

    /**
     * Destructor.
     */
    ~ModelIterator();

    /// @}
  // ========================================================================
  /// @name Operators.
  // ========================================================================
    /// @{

    /**
     * Copy operator.
     */
    ModelIterator& operator=(const ModelIterator& from);

    /**
     * Makes the iterator point to the next instance in the model.
     */
    ModelIterator& operator++();

    /**
     * Checks whether two iterators are pointing toward different elements.
     */
    bool operator!=(const ModelIterator& from) const;

    /**
     * Checks whether two iterators are pointing toward the same element.
     */
    bool operator==(const ModelIterator& from) const;

    /**
     * Returns the value pointed to by the iterator.
     */
    Instance&  operator*();

    /**
     * Returns the value pointed to by the iterator.
     */
    const Instance& operator*() const;

    /**
     * Dereferences the value pointed to by the iterator.
     */
    Instance*  operator->();

    /**
     * Dereferences the value pointed to by the iterator.
     */
    const Instance* operator->() const;

    /// @}
  // ========================================================================
  /// @name Getters & setters.
  // ========================================================================
    /// @{

    /**
     * Makes the iterator point toward nothing (in particular, it is not
     * related anymore to its current Model).
     */
    void clear();

    /**
     * Return true if the iterator points at the end of the Instance of it's
     * Model.
     */
    bool isEnd() const;

    /// @}
  private:
  // ========================================================================
  /// @name Private members.
  // ========================================================================
    /// @{

    /// Iterator over the __nameMap of the Model on which this iterate.
    HashTableIterator<std::string, Instance*> __iter;

    /// A pointer over the model on which this iterator iterates.
    Model* __model;

    /// @}
};

/**
 * @class ModelIterator model.h <agrum/prm/model.h>
 * @brief A constant iterator over the Instance of a Model.
 */
class ModelConstIterator
{
  public:
  // ========================================================================
  /// @name Constructors & destructor.
  // ========================================================================
    /// @{

    /**
     * Constructor for an iterator pointing on the begining of the Model's
     * Instance
     */
    ModelConstIterator(const Model& model);

    /**
     * Copy constructor.
     */
    ModelConstIterator(const ModelConstIterator& from);

    /**
     * Destructor.
     */
    ~ModelConstIterator();

    /// @}
  // ========================================================================
  /// @name Operators.
  // ========================================================================
    /// @{

    /**
     * Copy operator.
     */
    ModelConstIterator& operator=(const ModelConstIterator& from);

    /**
     * Makes the iterator point to the next instance in the model.
     */
    ModelConstIterator& operator++();

    /**
     * Checks whether two iterators are pointing toward different elements.
     */
    bool operator!=(const ModelConstIterator& from) const;

    /**
     * Checks whether two iterators are pointing toward the same element.
     */
    bool operator==(const ModelConstIterator& from) const;

    /**
     * Returns the value pointed to by the iterator.
     */
    const Instance& operator*() const;

    /**
     * Dereferences the value pointed to by the iterator.
     */
    const Instance* operator->() const;

    /**
     * Return true if the iterator points at the end of the Instance of it's
     * Model.
     */
    bool isEnd() const;

    /// @}
  // ========================================================================
  /// @name Getters & setters.
  // ========================================================================
    /// @{

    /**
     * Makes the iterator point toward nothing (in particular, it is not
     * related anymore to its current Model).
     */
    void clear();

    /// @}
  private:
  // ========================================================================
  /// @name Private members.
  // ========================================================================
    /// @{

    /// Iterator over the __nameMap of the Model on which this iterate.
    HashTableConstIterator<std::string, Instance*> __iter;

    /// Constant pointer on the Model on which this iterator iterates.
    const Model* __model;

    /// @}
};
} /* namespace gum */
// ============================================================================
#ifndef GUM_NO_INLINE
#include <agrum/prm/model.inl>
#endif // GUM_NO_INLINE
// ============================================================================
#endif // GUM_MODEL_H
// ============================================================================
