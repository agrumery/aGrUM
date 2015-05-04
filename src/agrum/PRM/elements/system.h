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
 * @brief Headers of System.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_SYSTEM_H
#define GUM_SYSTEM_H

#include <utility>

#include <agrum/core/set.h>
#include <agrum/core/sequence.h>
#include <agrum/core/hashTable.h>

#include <agrum/graphs/diGraph.h>

#include <agrum/BN/BayesNetFactory.h>

#include <agrum/PRM/elements/PRMObject.h>

namespace gum {
  namespace prm {
    class SystemIterator;
    class SystemConstIterator;

    /**
     * @class System system.h <agrum/PRM/system.h>
     * @brief A System is a container of Instance and describe a relational
     *        skeleton.
     */
    template <typename GUM_SCALAR> class System : public PRMObject {
      public:
      // ========================================================================
      /// @name Constructors & destructor.
      // ========================================================================
      /// @{

      /// Default constructor.
      System(const std::string &name);

      /// Destructor.
      virtual ~System();

      /// @}
      // ========================================================================
      /// @name Getters & setters over the relational skeleton.
      // ========================================================================
      /// @{

      /// Returns the relation skeleton of this System.
      const DiGraph &skeleton() const;

      /**
       * Returns an Instance given it's NodeId in the relational skeleton.
       *
       * @throw NotFound Raised if no Instance matches the given NodeId.
       */
      Instance<GUM_SCALAR> &get(NodeId id);

      /**
       * Returns an Instance given it's NodeId in the relational skeleton.
       *
       * @throw NotFound Raised if no Instance matches the given NodeId.
       */
      const Instance<GUM_SCALAR> &get(NodeId id) const;

      /**
       * @brief Insert an edge between between u and v using the ReferenceSlot
       *      ref of u.
       *
       * Either u or v can be arrays:
       *   - if u is an array and not v then an arc is added between each
       *     Instance in u and v;
       *   - if v is an array and not u then an arc is added between u and
       *     each Instance in v;
       *   - if u and v are arrays then an arc is added between each pair
       *     of Instance in u and v.
       *
       * @param u A name of an Instance in this System.
       * @param v A name of an Instance in this System.
       * @param ref A ReferenceSlot of u which range type matches v's.
       *
       * @throw OutOfUpperBound Raised if ref can not receive another Instance.
       * @throw NotFound Raised either if u, v or ref could not be found.
       * @throw WrongClassElement<GUM_SCALAR> Raised if ref does not name a
       *ReferenceSlot.
       * @throw TypeError Raised if v's type does not match ref range type.
       */
      GUM_DEPRECATED(void insertArc(const std::string &u, const std::string &v,
                                    const std::string &ref));
      void addArc(const std::string &u, const std::string &v,
                  const std::string &ref);

      /// @}
      // ========================================================================
      /// @name Getters & setters over Instance and array of Instance.
      // ========================================================================
      /// @{

      /// Returns the PRM type of this object.
      virtual PRMObject::PRMType obj_type() const;

      /// Returns the number of Instance in this System.
      Size size() const;

      /// Retruns true either if name is an instance or an array in this System.
      bool exists(const std::string &name) const;

      /// Returns true if the given Class<GUM_SCALAR> has at least one Instance in
      /// this
      /// System.
      bool isInstantiated(const Class<GUM_SCALAR> &c) const;

      /// Returns true if an Instance with the given name exists.
      bool isInstance(const std::string &name) const;

      /// Returns true if an array with the given name exists.
      bool isArray(const std::string &name) const;

      /**
       * Returns the grounded Bayesian Network of this system.
       * @param factory The factory used to build the grounded Bayesian Network.
       */
      void groundedBN(BayesNetFactory<GUM_SCALAR> &factory) const;

      /// Instantiate all the Instance in this System.
      void instantiate();

      /// @}
      // ========================================================================
      /// @name Instance handling.
      // ========================================================================
      /// @{

      /// Returns a reference over an Instance given it's name
      /// @throw NotFound Raised if the no Instance matches name.
      Instance<GUM_SCALAR> &get(const std::string &name);

      /// Returns a constant reference over an Instance given it's name
      /// @throw NotFound Raised if the no Instance matches name.
      const Instance<GUM_SCALAR> &get(const std::string &name) const;

      /// Returns the sequence of all instances of the given type
      /// @throw NotFound Raised if there is instantiation of type.
      const Set<Instance<GUM_SCALAR> *> &get(const Class<GUM_SCALAR> &type) const;

      /// Add an Instance to this system.
      /// @throw DuplicateElement Raised if an Instance with the same name
      ///                         already exists.
      NodeId add(Instance<GUM_SCALAR> *i);

      /// @}
      // ========================================================================
      /// @name Array handling.
      // ========================================================================
      /// @{

      /// Returns the sequence of instances of a given array.
      /// @throw NotFound Raised if no array matches name.
      const Sequence<Instance<GUM_SCALAR> *> &
      getArray(const std::string &name) const;

      /// Returns the type of the given array.
      /// @throw NotFound Raised if no array matches name.
      ClassElementContainer<GUM_SCALAR> &getArrayType(const std::string &name);

      /// Returns the type of the given array.
      /// @throw NotFound Raised if no array matches name.
      const ClassElementContainer<GUM_SCALAR> &
      getArrayType(const std::string &name) const;

      /// @brief Add an Instance to an array in this system.
      /// If the array doesn't exists it is created.
      /// @throw TypeError Raised if i is not of the good type.
      /// @throw DuplicateElement Raised if an Instance with same name already
      /// exists.
      NodeId add(const std::string &array, Instance<GUM_SCALAR> *i);

      /// @brief Add an Instance to an array in this system.
      /// If the array doesn't exists it is created.
      /// @throw TypeError Raised if i is not of the good type.
      /// @throw DuplicateElement Raised if an Instance with same name already
      /// exists.
      NodeId add(const std::string &array, Instance<GUM_SCALAR> &i);

      /// @brief Add an array of instances in this system.
      /// If the array doesn't exists it is created.
      /// @throw DuplicateElement Raised if an existing array with the same name
      ///                         already exists.
      void addArray(const std::string &array,
                    ClassElementContainer<GUM_SCALAR> &type);

      /// @}
      // ========================================================================
      /// @name Iterators.
      // ========================================================================
      /// @{

      /// Iterator over the Instance of this System.
      typedef typename NodeProperty<Instance<GUM_SCALAR> *>::iterator iterator;

      /// Returns an iterator over the instances in this system.
      iterator begin();

      /// Returns a iterator at the end of the set of Instance
      /// in this System.
      const iterator &end();

      /// Constant Iterator over the Instance of this System.
      typedef typename NodeProperty<Instance<GUM_SCALAR> *>::const_iterator
          const_iterator;

      /// Returns a constant iterator over the instances in this system.
      const_iterator begin() const;

      /// Returns a constant iterator at the end of the set of Instance
      /// in this System.
      const const_iterator &end() const;

      /// Iterator over the Instance in an array in this System.
      typedef typename Sequence<Instance<GUM_SCALAR> *>::iterator array_iterator;

      /// Returns an iterator at the beginning of the Sequence of Instance
      /// in the array named a;
      /// @throw NotFound Raised if no array matches a.
      array_iterator begin(const std::string &a);

      /// Returns an iterator at the end of the Sequence of Instance
      /// in the array named a.
      /// @throw NotFound Raised if no array matches a.
      const array_iterator &end(const std::string &a);

      /// Iterator over the Instance in an array in this System.
      typedef typename Sequence<Instance<GUM_SCALAR> *>::const_iterator
          const_array_iterator;

      /// Returns an iterator at the beginning of the Sequence of Instance
      /// in the array named a;
      /// @throw NotFound Raised if no array matches a.
      const_array_iterator begin(const std::string &a) const;

      /// Returns an iterator at the end of the Sequence of Instance
      /// in the array named a.
      /// @throw NotFound Raised if no array matches a.
      const const_array_iterator &end(const std::string &a) const;

      /// @}
      private:
      /// Copy constructor. Don't use it.
      System(const System &from);

      /// Copy operator. Don't use it.
      System &operator=(const System &from);

      // ========================================================================
      /// @name Private Instance handling methods and members.
      // ========================================================================
      /// @{

      /// The relational skeleton of this System.
      DiGraph __skeleton;

      /// The maping between Instance and their NodeId in the relational
      /// skeleton of this System.
      NodeProperty<Instance<GUM_SCALAR> *> __nodeIdMap;

      /// The mapping between Instance and their names.
      HashTable<std::string, Instance<GUM_SCALAR> *> __nameMap;

      /// Mapping between a class and all it's Instance in this system
      HashTable<Class<GUM_SCALAR> *, Set<Instance<GUM_SCALAR> *> *> __instanceMap;

      /// Typedef of the pair of a Class<GUM_SCALAR> and the sequence of it's
      /// instantiation.
      typedef std::pair<ClassElementContainer<GUM_SCALAR> *,
                        Sequence<Instance<GUM_SCALAR> *> *> model_pair;

      /// Mapping between arrays and their name. The first element of the pair
      /// is the type of the array.
      HashTable<std::string, model_pair> __arrayMap;

      /// @}
      // ========================================================================
      /// @name Ground BN private methods.
      // ========================================================================
      /// @{

      /// @brief Method which ground ReferenceSlot of an Instance and add arcs
      ///        in the IBayesNet.
      /// @param instance The Instance grounded by this method.
      /// @param factory  The factory used to build the grounded IBayesNet.
      void __groundRef(const Instance<GUM_SCALAR> &instance,
                       BayesNetFactory<GUM_SCALAR> &factory) const;

      /// @brief Method which ground Atttributes and Aggregators of
      ///        an Instance.
      /// @param instance The Instance grounded by this method.
      /// @param factory  The factory used to build the grounded IBayesNet.
      void __groundAttr(const Instance<GUM_SCALAR> &instance,
                        BayesNetFactory<GUM_SCALAR> &factory) const;

      /// @brief Method which copy node's Potential of an Instance to the grounded
      ///        Bayesian Network.
      /// @param instance The Instance currently grounded.
      /// @param attr     The Attribute<GUM_SCALAR> for which the Potential is
      /// grounded.
      /// @param factory  The factory used to build the grounded IBayesNet.
      void __groundPotential(const Instance<GUM_SCALAR> &instance,
                             const Attribute<GUM_SCALAR> &attr,
                             BayesNetFactory<GUM_SCALAR> &factory) const;

      /// @brief Ground an aggregator with the given name in the grounded
      ///        IBayesNet.
      /// @param elt     The aggregator grounded.
      /// @param name    The aggregator's name in the grounded IBayesNet.
      /// @param factory The factory used to build the grounded IBayesNet.
      void __groundAgg(const ClassElement<GUM_SCALAR> &elt, const std::string &name,
                       BayesNetFactory<GUM_SCALAR> &factory) const;
      /// @}
    };

    extern template class System<double>;
  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/elements/system.tcc>

#endif // GUM_SYSTEM_H
