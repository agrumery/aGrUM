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
 * @brief Class representing Fragment of Bayesian networks
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */
#ifndef GUM_BAYES_NET_FRAGMENT_H
#define GUM_BAYES_NET_FRAGMENT_H

#include <agrum/config.h>

#include <agrum/BN/IBayesNet.h>
#include <agrum/graphs/listeners/diGraphListener.h>

#include <agrum/BN/BayesNet.h>

namespace gum {
  /**
   * @class BayesNetFragment BayesNetFragment.h <agrum/BN/BayesNetFragment.h>
   * @brief portion of a BN identified by the list of nodes inserted and an BayesNet
   *source.
   *
   * @author Pierre-Henri WUILLEMIN
   *
   * This class is a decorator of a BayesNet implementing the IBayesNet interface.
   * CPTs can be shared with the BN or can be specific to the Fragment if different.
   *
   * BayesNetFragment is a DiGraphListener in order to be synchronized (especiallay
   *when
   * removing nodes or arcs).
   *
   * In a BayesNetFragment, one can install or remove nodes. An arc can be in the
   *fragment if
   * and only if its head and tail are installed in the fragment. When installing a
   *node, all
   * the arcs that can be added in the fragment are effectively installed (resp. when
   * uninstalling a node, etc.).
   *
   * A BayesNetFragment can redefine potential for node. The main reason is to be
   *able to
   * install a node without installing all its parents (and its ascendants). So local
   *CPT to the
   * node can be installed. However, it is not done automatically.
   *
   * If a cpt is not locally defined, the fragment uses the cpt defined in the
   *referred BN.
   * The checkConsistency() method verifies that, for all installed nodes, either all
   *the parents are installed or a local CPT is
   * defined.
   */

  template <typename GUM_SCALAR>
  class BayesNetFragment : public IBayesNet<GUM_SCALAR>,
                           public gum::DiGraphListener {
    private:
    /// The referred BayesNet
    const BayesNet<GUM_SCALAR> &__bn;

    /// Mapping between the variable's id and their CPT specific to this Fragment.
    NodeProperty<const Potential<GUM_SCALAR> *> __localCPTs;

    public:
    // ===========================================================================
    /// @name Constructors / Destructors
    // ===========================================================================
    /// @{
    BayesNetFragment(const BayesNet<GUM_SCALAR> &bn);

    virtual ~BayesNetFragment();
    /// @}

    /// @name signals to listen to
    /// @{

    /// the action to take when a new node is inserted into the graph
    /** @param src the object that sent the signal
     * @param id the id of the new node inserted into the graph */
    virtual void whenNodeAdded(const void *src, NodeId id) noexcept override;

    /// the action to take when a node has just been removed from the graph
    /** @param src the object that sent the signal
     * @param id the id of the node has just been removed from the graph */
    virtual void whenNodeDeleted(const void *src, NodeId id) noexcept override;

    /// the action to take when a new arc is inserted into the graph
    /** @param src the object that sent the signal
     * @param from the id of tail of the new arc inserted into the graph
     * @param to the id of head of the new arc inserted into the graph */
    virtual void whenArcAdded(const void *src, NodeId from,
                              NodeId to) noexcept override;

    /// the action to take when an arc has just been removed from the graph
    /** @param src the object that sent the signal
     * @param from the id of tail of the arc removed from the graph
     * @param to the id of head of the arc removed from the graph */
    virtual void whenArcDeleted(const void *src, NodeId from,
                                NodeId to) noexcept override;
    /// @}

    /// @name IBayesNet interface
    /// @{

    /**
    * Returns the CPT of a variable.
    *
    * @throw NotFound If no variable's id matches varId.
    */
    virtual const Potential<GUM_SCALAR> &cpt(NodeId varId) const override;

    /**
    * Returns a constant reference to the VariableNodeMap of this BN
    */
    virtual const VariableNodeMap &variableNodeMap() const override;

    /**
    * Returns a constant reference over a variabe given it's node id.
    *
    * @throw NotFound If no variable's id matches varId.
    */
    virtual const DiscreteVariable &variable(NodeId id) const override;

    /**
    * Return id node from discrete var pointer.
    *
    * @throw NotFound If no variable matches var.
    */
    virtual NodeId nodeId(const DiscreteVariable &var) const override;

    /**
     * Getter by name
     *
     * @throw NotFound if no such name exists in the graph.
     */
    virtual NodeId idFromName(const std::string &name) const override;

    /**
     * Getter by name
     *
     * @throw NotFound if no such name exists in the graph.
     */
    virtual const DiscreteVariable &
    variableFromName(const std::string &name) const override;

    /**
     * creates a dot representing the whole referred BN hilighting the fragment.
     * @return Returns a dot representation of this fragment
     */
    virtual std::string toDot(void) const;

    /// @}

    /// @specific API for Fragment
    /// @{

    /**
     * check if a certain NodeId exists in the fragment
     */
    bool isInstalledNode(NodeId id) const noexcept;

    /**
     * install a node referenced by its nodeId
     *
     * @throw NotFound if the node does not exist in the referred BN
     * @warning nothing happens if the node is already installed
     */
    void installNode(NodeId id);

    /**
     * install a node and all its ascendants
     *
     * @throw NotFound if the node does not exist in the referred BN
     * @warning nothing happens if the node is already installed
     */
    void installAscendants(NodeId id);

    /**
     * uninstall a node referenced by its nodeId
     *
     * @warning nothing happens if the node is not installed
     */
    void uninstallNode(NodeId id) noexcept;

    /**
     * install a local marginal for a node into the fragment.
     * This function will remove all the arcs from the parents to the node.
     * @param id the nodeId
     * @param pot the potential
     * @throw NotFound if the id is not in the fragment
     * @throw OperationNotAllowed if the potential is not compliant with the variable
     *(or is not a marginal)
     **/
    void installMarginal(NodeId id, const Potential<GUM_SCALAR> *pot);

    /**
     * install a local cpt for a node into the fragment.
     * This function will change the arcs from the parents to the node in order to be
     * consistent with the new local potential.
     * @param id the nodeId
     * @param pot the potential
     *
     * @throw NotFound if the id is not in the fragment
     * @throw OperationNotAllowed if the potential is not compliant with the variable
     *or if
     * a variable in the CPT is not a parent in the referred bn.
     **/
    void installCPT(NodeId id, const Potential<GUM_SCALAR> *pot);

    /**
     * uninstall a local CPT.
     *
     * @warning  Nothing happens if no local CPT for this nodeId or if the node is
     *not installed.
     */
    void uninstallCPT(NodeId id) noexcept;

    /**
     * returns true if the nodeId's (local or not) cpt is consistent with its parents
     * in the fragment
     * @throw NotFound if the id is not in the fragment
     */
    bool checkConsistency(NodeId id) const;

    /**
     * returns true if all nodes in the fragment are consistent
     */
    bool checkConsistency() const noexcept;

    /// @}

    using IBayesNet<GUM_SCALAR>::nodes;
    using IBayesNet<GUM_SCALAR>::dag;

    protected:
    // remove an arc
    void _uninstallArc(NodeId from, NodeId to) noexcept;

    // add an arc
    void _installArc(NodeId from, NodeId to) noexcept;

    // install a CPT, create or delete arcs. Checks are made in public methods
    // In particular, it is assumed that all the variables in the pot are in the
    // fragment
    void _installCPT(NodeId id, const Potential<GUM_SCALAR> *pot) noexcept;

    /**
     * uninstall a local CPT. Does nothing if no local CPT for this nodeId
     * No check. No change in the topology. Checks are made in public methods.
     */
    void _uninstallCPT(NodeId id) noexcept;
  };

  extern template class BayesNetFragment<float>;
  extern template class BayesNetFragment<double>;
} // namespace gum

#include <agrum/BN/BayesNetFragment.tcc>

#endif // GUM_BAYES_NET_FRAGMENT_H
