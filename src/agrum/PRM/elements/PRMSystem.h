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
 * @brief Headers of PRMSystem.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_SYSTEM_H
#define GUM_SYSTEM_H

#include <utility>
#include <string>


#include <agrum/tools/variables/integerVariable.h>

#include <agrum/BN/BayesNetFactory.h>

#include <agrum/PRM/elements/PRMInstance.h>

namespace gum {
  namespace prm {

    /**
     * @class PRMSystem
     * @headerfile system.h <agrum/PRM/system.h>
     * @brief A PRMSystem is a container of PRMInstance and describe a
     * relational
     *        skeleton.
     */
    template < typename GUM_SCALAR >
    class PRMSystem: public PRMObject {
      public:
      // ========================================================================
      /// @name Constructors & destructor.
      // ========================================================================
      /// @{

      /// Default constructor.
      explicit PRMSystem(const std::string& name);

      /// Destructor.
      virtual ~PRMSystem();

      /// @}
      // ========================================================================
      /// @name Getters & setters over the relational skeleton.
      // ========================================================================
      /// @{

      /// Returns the relation skeleton of this PRMSystem.
      const DiGraph& skeleton() const;

      /**
       * Returns an PRMInstance given it's NodeId in the relational skeleton.
       *
       * @throw NotFound Raised if no PRMInstance matches the given NodeId.
       */
      PRMInstance< GUM_SCALAR >& get(NodeId id);

      /**
       * Returns an PRMInstance given it's NodeId in the relational skeleton.
       *
       * @throw NotFound Raised if no PRMInstance matches the given NodeId.
       */
      const PRMInstance< GUM_SCALAR >& get(NodeId id) const;

      NodeId get(const PRMInstance< GUM_SCALAR >& i) const;

      /// @}
      // ========================================================================
      /// @name Getters & setters over PRMInstance and array of PRMInstance.
      // ========================================================================
      /// @{

      /// Returns the PRM type of this object.
      virtual PRMObject::prm_type obj_type() const;

      /// Returns the number of PRMInstance in this PRMSystem.
      Size size() const;

      /// Retruns true either if name is an instance or an array in this
      /// PRMSystem.
      bool exists(const std::string& name) const;

      /// Returns true if the given Class<GUM_SCALAR> has at least one
      /// PRMInstance
      /// in this PRMSystem.
      bool isInstantiated(const PRMClass< GUM_SCALAR >& c) const;

      /// Returns true if an PRMInstance with the given name exists.
      bool isInstance(const std::string& name) const;

      /// Returns true if an array with the given name exists.
      bool isArray(const std::string& name) const;

      /**
       * Returns the grounded Bayesian network of this system.
       * @param factory The factory used to build the grounded Bayesian network.
       */
      void groundedBN(BayesNetFactory< GUM_SCALAR >& factory) const;

      /// Instantiate all the PRMInstance in this PRMSystem.
      void instantiate();

      /// @}
      // ========================================================================
      /// @name PRMInstance handling.
      // ========================================================================
      /// @{

      /// Returns a reference over an PRMInstance given it's name
      /// @throw NotFound Raised if the no PRMInstance matches name.
      PRMInstance< GUM_SCALAR >& get(const std::string& name);

      /// Returns a constant reference over an PRMInstance given it's name
      /// @throw NotFound Raised if the no PRMInstance matches name.
      const PRMInstance< GUM_SCALAR >& get(const std::string& name) const;

      /// Returns the sequence of all instances of the given type
      /// @throw NotFound Raised if there is instantiation of type.
      const Set< PRMInstance< GUM_SCALAR >* >& get(const PRMClass< GUM_SCALAR >& type) const;

      /// Add an PRMInstance to this system.
      /// @throw DuplicateElement Raised if an PRMInstance with the same name
      ///                         already exists.
      NodeId add(PRMInstance< GUM_SCALAR >* i);

      /// @}
      // ========================================================================
      /// @name Array handling.
      // ========================================================================
      /// @{

      /// Returns the sequence of instances of a given array.
      /// @throw NotFound Raised if no array matches name.
      const Sequence< PRMInstance< GUM_SCALAR >* >& getArray(const std::string& name) const;

      /// Returns the type of the given array.
      /// @throw NotFound Raised if no array matches name.
      PRMClassElementContainer< GUM_SCALAR >& getArrayType(const std::string& name);

      /// Returns the type of the given array.
      /// @throw NotFound Raised if no array matches name.
      const PRMClassElementContainer< GUM_SCALAR >& getArrayType(const std::string& name) const;

      /// @brief Add an PRMInstance to an array in this system.
      /// If the array doesn't exists it is created.
      /// @throw PRMTypeError Raised if i is not of the good type.
      /// @throw DuplicateElement Raised if an PRMInstance with same name
      /// already
      /// exists.
      NodeId add(const std::string& array, PRMInstance< GUM_SCALAR >* i);

      /// @brief Add an PRMInstance to an array in this system.
      /// If the array doesn't exists it is created.
      /// @throw PRMTypeError Raised if i is not of the good type.
      /// @throw DuplicateElement Raised if an PRMInstance with same name
      /// already
      /// exists.
      NodeId add(const std::string& array, PRMInstance< GUM_SCALAR >& i);

      /// @brief Add an array of instances in this system.
      /// If the array doesn't exists it is created.
      /// @throw DuplicateElement Raised if an existing array with the same name
      ///                         already exists.
      void addArray(const std::string& array, PRMClassElementContainer< GUM_SCALAR >& type);

      /// @}
      // ========================================================================
      /// @name Iterators.
      // ========================================================================
      /// @{

      /// Iterator over the PRMInstance of this PRMSystem.
      using iterator = typename NodeProperty< PRMInstance< GUM_SCALAR >* >::iterator;

      /// Returns an iterator over the instances in this system.
      iterator begin();

      /// Returns a iterator at the end of the set of PRMInstance
      /// in this PRMSystem.
      const iterator& end();

      /// Constant Iterator over the PRMInstance of this PRMSystem.
      using const_iterator = typename NodeProperty< PRMInstance< GUM_SCALAR >* >::const_iterator;

      /// Returns a constant iterator over the instances in this system.
      const_iterator begin() const;

      /// Returns a constant iterator at the end of the set of PRMInstance
      /// in this PRMSystem.
      const const_iterator& end() const;

      /// Iterator over the PRMInstance in an array in this PRMSystem.
      using array_iterator = typename Sequence< PRMInstance< GUM_SCALAR >* >::iterator;

      /// Returns an iterator at the beginning of the Sequence of PRMInstance
      /// in the array named a;
      /// @throw NotFound Raised if no array matches a.
      array_iterator begin(const std::string& a);

      /// Returns an iterator at the end of the Sequence of PRMInstance
      /// in the array named a.
      /// @throw NotFound Raised if no array matches a.
      const array_iterator& end(const std::string& a);

      /// Iterator over the PRMInstance in an array in this PRMSystem.
      using const_array_iterator = typename Sequence< PRMInstance< GUM_SCALAR >* >::const_iterator;

      /// Returns an iterator at the beginning of the Sequence of PRMInstance
      /// in the array named a;
      /// @throw NotFound Raised if no array matches a.
      const_array_iterator begin(const std::string& a) const;

      /// Returns an iterator at the end of the Sequence of PRMInstance
      /// in the array named a.
      /// @throw NotFound Raised if no array matches a.
      const const_array_iterator& end(const std::string& a) const;

      /// @}
      protected:
      /// Copy constructor. Don't use it.
      PRMSystem(const PRMSystem< GUM_SCALAR >& from);

      /// Copy operator. Don't use it.
      PRMSystem< GUM_SCALAR >& operator=(const PRMSystem< GUM_SCALAR >& from) = delete;

      // ========================================================================
      /// @name Private PRMInstance handling methods and members.
      // ========================================================================
      /// @{

      /// The relational skeleton of this PRMSystem.
      DiGraph skeleton_;

      /// The mapping between PRMInstance and their NodeId in the relational
      /// skeleton of this PRMSystem.
      NodeProperty< PRMInstance< GUM_SCALAR >* > nodeIdMap_;

      /// The mapping between PRMInstance and their names.
      HashTable< std::string, PRMInstance< GUM_SCALAR >* > nameMap_;

      /// Mapping between a class and all it's PRMInstance in this system
      HashTable< PRMClass< GUM_SCALAR >*, Set< PRMInstance< GUM_SCALAR >* >* > instanceMap_;

      /// Type for the pair of a Class<GUM_SCALAR> and the sequence of it's
      /// instantiation.
      using model_pair = std::pair< PRMClassElementContainer< GUM_SCALAR >*,
                                    Sequence< PRMInstance< GUM_SCALAR >* >* >;

      /// Mapping between arrays and their name. The first element of the pair
      /// is the type of the array.
      HashTable< std::string, model_pair > arrayMap_;

      /// @}
      // ========================================================================
      /// @name Ground BN private methods.
      // ========================================================================
      /// @{

      /// @brief Method which ground PRMReferenceSlot of an PRMInstance and add
      /// arcs
      ///        in the IBayesNet.
      /// @param instance The PRMInstance grounded by this method.
      /// @param factory  The factory used to build the grounded IBayesNet.
      void _groundRef_(const PRMInstance< GUM_SCALAR >& instance,
                       BayesNetFactory< GUM_SCALAR >&   factory) const;

      /// @brief Method which ground Atttributes and Aggregators of
      ///        an PRMInstance.
      /// @param instance The PRMInstance grounded by this method.
      /// @param factory  The factory used to build the grounded IBayesNet.
      void _groundAttr_(const PRMInstance< GUM_SCALAR >& instance,
                        BayesNetFactory< GUM_SCALAR >&   factory) const;

      /// @brief Method which copy node's Potential of an PRMInstance to the
      /// grounded
      ///        Bayesian network.
      /// @param instance The PRMInstance currently grounded.
      /// @param attr     The PRMAttribute<GUM_SCALAR> for which the Potential
      /// is
      /// grounded.
      /// @param factory  The factory used to build the grounded IBayesNet.
      void _groundPotential_(const PRMInstance< GUM_SCALAR >&  instance,
                             const PRMAttribute< GUM_SCALAR >& attr,
                             BayesNetFactory< GUM_SCALAR >&    factory) const;

      /// @brief Ground an aggregator with the given name in the grounded
      ///        IBayesNet.
      /// @param elt     The aggregator grounded.
      /// @param name    The aggregator's name in the grounded IBayesNet.
      /// @param factory The factory used to build the grounded IBayesNet.
      void _groundAgg_(const PRMClassElement< GUM_SCALAR >& elt,
                       const std::string&                   name,
                       BayesNetFactory< GUM_SCALAR >&       factory) const;
      /// @}
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class PRMSystem< double >;
#endif


  } /* namespace prm */

} /* namespace gum */

#include <agrum/PRM/elements/PRMSystem_tpl.h>

#endif   // GUM_SYSTEM_H
