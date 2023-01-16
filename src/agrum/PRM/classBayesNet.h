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
 * @brief Headers of ClassBayesNet<GUM_SCALAR>.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_CLASS_BAYESNET_H
#define GUM_CLASS_BAYESNET_H

#include <list>


#include <agrum/PRM/PRM.h>
namespace gum {
  namespace prm {

    // clang-format off
    /**
* @class ClassBayesNet
* @headerfile classBayesNet.h <agrum/PRM/classBayesNet.h>
     * @brief This class decorates a gum::prm::Class<GUM_SCALAR> has an
     *        IBaseBayesNet.
     * @tparam GUM_SCALAR The type of scalar to use.
     *
     * This class filters PRMAttribute and PRMAggregate in a way it can be
     * interpreted as a IBayesNet.
     *
     * SlotChains and PRMReferenceSlot are not represented.
     *
     * Remember that a ClassBayesNet<GUM_SCALAR> does not contain input nodes
     * parents and output nodes children. Thus you should be careful when using
     * one of the BayesNetInference over a ClassBayesNet<GUM_SCALAR> since some
     * variables are missing in the DAG but not in the nodes CPT.
     */
    // clang-format on
    template < typename GUM_SCALAR >
    class ClassBayesNet: public IBayesNet< GUM_SCALAR > {
      public:
      // ========================================================================
      /// @name Constructors & destructor.
      // ========================================================================
      /// @{

      /// Default constructor.
      /// @param c The Class<GUM_SCALAR> decorated by this
      /// ClassBayesNet<GUM_SCALAR>.
      ClassBayesNet(const PRMClass< GUM_SCALAR >& c);

      /// Copy constructor.
      ClassBayesNet(const ClassBayesNet< GUM_SCALAR >& from);

      /// Copy operator.
      ClassBayesNet< GUM_SCALAR >& operator=(const ClassBayesNet< GUM_SCALAR >& from);

      /// Destructor.
      virtual ~ClassBayesNet();

      /// @}
      // ===========================================================================
      /// @name Variable manipulation methods.
      // ===========================================================================
      /// @{

      /**
       * @brief Returns the CPT of a node in this ClassBayesNet<GUM_SCALAR>.
       *
       * If the node is an PRMAggregate, this will raise an OperationNotAllowed
       * since PRMAggregate's CPT are define at instantiation only.
       *
       * @param varId a variable id.
       * @return the Potential of varId.
       *
       * @throw NotFound raised if varId does not match any variable in this
       *                 IBayesNet.
       * @throw OperationNotAllowed raised if varId is an PRMAggregate.
       */
      virtual const Potential< GUM_SCALAR >& cpt(NodeId varId) const;

      /// See gum::IBaseBayesNet::variableNodeMap().
      virtual const VariableNodeMap& variableNodeMap() const;

      /// See gum::IBaseBayesNet::variable().
      virtual const DiscreteVariable& variable(NodeId id) const;

      /// See gum::IBaseBayesNet::nodeId().
      virtual NodeId nodeId(const DiscreteVariable& var) const;

      /// See gum::IBaseBayesNet::idFromName().
      virtual NodeId idFromName(const std::string& name) const;

      /// See gum::IBaseBayesNet::variableFromName().
      virtual const DiscreteVariable& variableFromName(const std::string& name) const;

      /// See gum::IBaseBayesNet::modalities().
      const NodeProperty< Size >& modalities() const;

      /// @}
      // ===========================================================================
      /// @name Graphical methods
      // ===========================================================================
      /// @{
      /// @return Returns a dot representation of this IBayesNet.
      virtual std::string toDot() const;

      /// @}
      private:
      /// Mapping between DiscreteVariable and their NodeId
      HashTable< const DiscreteVariable*, const PRMClassElement< GUM_SCALAR >* > _varNodeMap_;

      /// Private getter with type checking in case the id is not a formal
      /// PRMAttribute.
      /// @throw NotFound Raised if id is not a formal attribute.
      const PRMClassElement< GUM_SCALAR >& _get_(NodeId id) const;

      /// Private getter with type checking in case the id is not a formal
      /// PRMAttribute.
      /// @throw NotFound Raised if id is not a formal attribute.
      const PRMClassElement< GUM_SCALAR >& _get_(const std::string& name) const;

      /// The PRMClassElementContainer decorated by this.
      const PRMClass< GUM_SCALAR >* _class_;

      mutable NodeProperty< Size > _modalities_;

      void _init_(const PRMClass< GUM_SCALAR >& c);
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class ClassBayesNet< double >;
#endif


  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/classBayesNet_tpl.h>

#endif /* GUM_CLASS_BAYESNET_H */
