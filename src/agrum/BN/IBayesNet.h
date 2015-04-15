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
 * @brief Class representing Bayesian networks
 *
 * @author Pierre-Henri WUILLEMIN and Lionel TORTI
 *
 */
#ifndef GUM_SIMPLE_BAYES_NET_H
#define GUM_SIMPLE_BAYES_NET_H

#include <utility>

#include <agrum/config.h>

#include <agrum/core/hashTable.h>

#include <agrum/graphicalModels/DAGmodel.h>
#include <agrum/multidim/potential.h>

namespace gum {

  template <typename GUM_SCALAR> class BayesNetFactory;

  /**
   * @class IBayesNet IBayesNet.h <agrum/BN/IBayesNet.h>
   * @brief Class representing the minimal interface for Bayesian Network.
   * @ingroup bn_group
   *
   * This class is used as a base class for different versions of Bayesian Networks.
   *No data (except the dag
   * herited from DAGmodel are included in this class. Many algorithms (inference for
   *instance) may use this
   * class when a simple BN is needed.
   */
  template <typename GUM_SCALAR> class IBayesNet : public DAGmodel {

    public:
    // ===========================================================================
    /// @name Constructors / Destructors
    // ===========================================================================
    /// @{

    /**
     * Default constructor.
     */
    IBayesNet();
    IBayesNet(std::string name);

    /**
     * Destructor.
     */
    virtual ~IBayesNet();

    /**
     * Copy constructor.
     */
    IBayesNet(const IBayesNet<GUM_SCALAR> &source);

    /**
     * Copy operator.
     */
    IBayesNet<GUM_SCALAR> &operator=(const IBayesNet<GUM_SCALAR> &source);

    /// @}
    // ===========================================================================
    /// @name Pure Virtual methods
    // ===========================================================================
    /// @{

    /**
    * Returns the CPT of a variable.
    *
    * @throw NotFound If no variable's id matches varId.
    */
    virtual const Potential<GUM_SCALAR> &cpt(NodeId varId) const = 0;

    /**
    * Returns a constant reference to the VariableNodeMap of thisBN
    */
    virtual const VariableNodeMap &variableNodeMap() const = 0;

    /**
    * Returns a constant reference over a variabe given it's node id.
    *
    * @throw NotFound If no variable's id matches varId.
    */
    virtual const DiscreteVariable &variable(NodeId id) const = 0;

    /**
    * Return id node from discrete var pointer.
    *
    * @throw NotFound If no variable matches var.
    */
    virtual NodeId nodeId(const DiscreteVariable &var) const = 0;

    /**
     * Getter by name
     *
     * @throw NotFound if no such name exists in the graph.
     */
    virtual NodeId idFromName(const std::string &name) const = 0;

    /**
     * Getter by name
     *
     * @throw NotFound if no such name exists in the graph.
     */
    virtual const DiscreteVariable &
    variableFromName(const std::string &name) const = 0;
    /// @}

    // ===========================================================================
    /// @name Joint Probability manipulation methods
    // ===========================================================================
    /// @{

    /// Compute a parameter of the joint probability for the BN (given an
    /// instantiation of the vars)
    /// @warning a variable not present in the instantiation is assumed to be
    /// instantiated to 0.
    GUM_SCALAR jointProbability(const Instantiation &i) const;

    /// Compute a parameter of the log joint probability for the BN (given an
    /// instantiation of the vars)
    /// @warning a variable not present in the instantiation is assumed to be
    /// instantiated to 0.
    GUM_SCALAR log2JointProbability(const Instantiation &i) const;

    ///
    /// @}

    /**
     * This operator compares 2 BNs !
     * @warning To identify nodes between BNs, it is assumed that they share the same
     *name.
     *
     * @return true if the src and this are equal.
     */
    bool operator==(const IBayesNet<GUM_SCALAR> &src) const;

    /// @return Returns false if the src and this are equal.
    bool operator!=(const IBayesNet<GUM_SCALAR> &src) const;

    /**
     * Returns the dimension (the number of free parameters) in this bayes net.
     *
     * \f$ dim(G)=\sum_{i \in nodes} ((r_i-1)\cdot q_i) \f$ where \f$ r_i \f$ is the
     *number of instantiations of node \f$ i \f$ and \f$ q_i \f$ is the number of
     *instantiations of
     * its parents.
     */
    Idx dim() const;

    /// @return Returns a dot representation of this IBayesNet.
    virtual std::string toDot(void) const;

    /// @return Returns a string representation of this IBayesNet.
    std::string toString(void) const;
  };

  extern template class IBayesNet<float>;
  extern template class IBayesNet<double>;

  /// Prints map's DAG in output using the Graphviz-dot format.
  template <typename GUM_SCALAR>
  std::ostream &operator<<(std::ostream &output, const IBayesNet<GUM_SCALAR> &map);

} /* namespace gum */

#include <agrum/BN/IBayesNet.tcc>

#endif /* GUM_SIMPLE_BAYES_NET_H */
