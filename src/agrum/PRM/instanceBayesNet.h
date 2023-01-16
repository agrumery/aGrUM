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
 * @brief Headers of InstanceBayesNet.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_INSTANCE_BAYESNET_H
#define GUM_INSTANCE_BAYESNET_H

#include <list>


#include <agrum/PRM/PRM.h>

namespace gum {
  namespace prm {

    /**
     * @class InstanceBayesNet
     * @headerfile instanceBayesNet.h <agrum/PRM/instanceBayesNet.h>
     * @brief This class decorates an PRMInstance<GUM_SCALAR> as an
     *IBaseBayesNet.
     *
     * Remember that an InstanceBayesNet does not contain input nodes parents
     *and
     * output nodes children. Thus you should be careful when using one of the
     * BayesNetInference over a InstanceBayesNet since some variables are
     *missing in
     * the DAG but not in the nodes CPT.
     *
     */
    template < typename GUM_SCALAR >
    class InstanceBayesNet: public IBayesNet< GUM_SCALAR > {
      public:
      // ========================================================================
      /// @name Constructors & destructor.
      // ========================================================================
      /// @{

      /// Default constructor.
      /// @param i The PRMInstance<GUM_SCALAR> decorated by this
      /// InstanceBayesNet.
      InstanceBayesNet(const PRMInstance< GUM_SCALAR >& i);

      /// Copy constructor.
      InstanceBayesNet(const InstanceBayesNet& from);

      /// Copy operator.
      InstanceBayesNet& operator=(const InstanceBayesNet& from);

      /// Destructor.
      virtual ~InstanceBayesNet();

      /// @}
      // ===========================================================================
      /// @name Variable manipulation methods.
      // ===========================================================================
      /// @{

      /// See gum::IBaseBayesNet::cpt().
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
      HashTable< const DiscreteVariable*, const PRMAttribute< GUM_SCALAR >* > _varNodeMap_;

      /// Private getter with type checking in case the id is not a formal
      /// PRMAttribute<GUM_SCALAR>.
      /// @throw NotFound Raised if id is not a formal attribute.
      const PRMClassElement< GUM_SCALAR >& _get_(NodeId id) const;

      const PRMClassElement< GUM_SCALAR >& _get_(const std::string& name) const;

      /// The PRMClassElementContainer decorated by this.
      const PRMInstance< GUM_SCALAR >* _inst_;

      mutable NodeProperty< Size > _modalities_;

      void _init_(const PRMInstance< GUM_SCALAR >& i);
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class InstanceBayesNet< double >;
#endif


  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/instanceBayesNet_tpl.h>

#endif /* GUM_INSTANCE_BAYESNET_H */
