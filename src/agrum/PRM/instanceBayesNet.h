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
 * @brief Headers of InstanceBayesNet.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_INSTANCE_BAYESNET_H
#define GUM_INSTANCE_BAYESNET_H

#include <list>

#include <agrum/BN/IBayesNet.h>

#include <agrum/PRM/PRM.h>

namespace gum {
  namespace prm {

    /**
     * @class InstanceBayesNet instanceBayesNet.h <agrum/PRM/instanceBayesNet.h>
     * @brief This class decorates an Instance<GUM_SCALAR> as an IBaseBayesNet.
     *
     * Remember that an InstanceBayesNet does not contain input nodes parents and
     * output nodes children. Thus you should be careful when using one of the
     * BayesNetInference over a InstanceBayesNet since some variables are missing in
     * the DAG but not in the nodes CPT.
     *
     */
    template <typename GUM_SCALAR>
    class InstanceBayesNet : public IBayesNet<GUM_SCALAR> {
      public:
      // ========================================================================
      /// @name Constructors & destructor.
      // ========================================================================
      /// @{

      /// Default constructor.
      /// @param i The Instance<GUM_SCALAR> decorated by this InstanceBayesNet.
      InstanceBayesNet(const Instance<GUM_SCALAR> &i);

      /// Copy constructor.
      InstanceBayesNet(const InstanceBayesNet &from);

      /// Copy operator.
      InstanceBayesNet &operator=(const InstanceBayesNet &from);

      /// Destructor.
      virtual ~InstanceBayesNet();

      /// @}
      // ===========================================================================
      /// @name Variable manipulation methods.
      // ===========================================================================
      /// @{

      /// See gum::IBaseBayesNet::cpt().
      virtual const Potential<GUM_SCALAR> &cpt(NodeId varId) const;

      /// See gum::IBaseBayesNet::variableNodeMap().
      virtual const VariableNodeMap &variableNodeMap() const;

      /// See gum::IBaseBayesNet::variable().
      virtual const DiscreteVariable &variable(NodeId id) const;

      /// See gum::IBaseBayesNet::nodeId().
      virtual NodeId nodeId(const DiscreteVariable &var) const;

      /// See gum::IBaseBayesNet::idFromName().
      virtual NodeId idFromName(const std::string &name) const;

      /// See gum::IBaseBayesNet::variableFromName().
      virtual const DiscreteVariable &
      variableFromName(const std::string &name) const;

      const NodeProperty<Size> &modalities() const;

      /// @}
      // ===========================================================================
      /// @name Graphical methods
      // ===========================================================================
      /// @{
      /// @return Returns a dot representation of this IBayesNet.
      virtual std::string toDot(void) const;

      /// @}
      private:
      /// Mapping between DiscreteVariable and their NodeId
      HashTable<const DiscreteVariable *, const Attribute<GUM_SCALAR> *>
          __varNodeMap;

      /// Private getter with type checking in case the id is not a formal
      /// Attribute<GUM_SCALAR>.
      /// @throw NotFound Raised if id is not a formal attribute.
      const ClassElement<GUM_SCALAR> &__get(NodeId id) const;

      const ClassElement<GUM_SCALAR> &__get(const std::string &name) const;

      /// The ClassElementContainer decorated by this.
      const Instance<GUM_SCALAR> *__inst;

      mutable NodeProperty<Size> __modalities;

      void __init(const Instance<GUM_SCALAR> &i);
    };

    extern template class InstanceBayesNet<double>;

  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/instanceBayesNet.tcc>

#endif /* GUM_INSTANCE_BAYESNET_H */
