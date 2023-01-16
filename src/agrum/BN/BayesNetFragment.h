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
 * @brief Class representing Fragment of Bayesian networks
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */
#ifndef GUM_BAYES_NET_FRAGMENT_H
#define GUM_BAYES_NET_FRAGMENT_H

#include <agrum/agrum.h>

#include <agrum/tools/graphs/parts/listeners/diGraphListener.h>

#include <agrum/BN/BayesNet.h>

namespace gum {
  /**
   * @class BayesNetFragment
   * @headerfile BayesNetFragment.h <agrum/BN/BayesNetFragment.h>
   * @brief Portion of a BN identified by the list of nodes and a BayesNet.
   * @ingroup bn_group
   *
   * @author Pierre-Henri WUILLEMIN(_at_LIP6)
   *
   * This class is a decorator of a BayesNet implementing the IBayesNet
   * interface. CPTs can be shared with the BN or can be specific to the
   * Fragment if different.
   *
   * BayesNetFragment is a DiGraphListener in order to be synchronized
   * (especially when removing nodes or arcs).
   *
   * In a BayesNetFragment, one can install or remove nodes. An arc can be in
   * the fragment if and only if its head and tail are installed in the
   * fragment.  *When installing a node, all the arcs that can be added in the
   * fragment are *effectively installed (resp.  *when uninstalling a node,
   * etc.).
   *
   * A BayesNetFragment can redefine potential for node. The main reason is to
   * be able to install a node without installing all its parents (and its
   * ascendants). So local CPT to the node can be installed. However, it is not
   * done automatically.
   *
   * If a cpt is not locally defined, the fragment uses the cpt defined in the
   * referred BN. The checkConsistency() method verifies that, for all
   * installed nodes, either all the parents are installed or a local CPT is
   * defined.
   */
  template < typename GUM_SCALAR >
  class BayesNetFragment: public IBayesNet< GUM_SCALAR >, public gum::DiGraphListener {
    private:
    /// The referred BayesNet
    const IBayesNet< GUM_SCALAR >& _bn_;

    /// Mapping between the variable's id and their CPT specific to this
    /// Fragment.
    NodeProperty< const Potential< GUM_SCALAR >* > _localCPTs_;

    public:
    /// @name Constructors / Destructors
    /// @{
    BayesNetFragment()                                               = delete;
    BayesNetFragment(const BayesNetFragment< GUM_SCALAR >& fragment) = delete;
    BayesNetFragment(BayesNetFragment< GUM_SCALAR >&& fragment)      = delete;

    explicit BayesNetFragment(const IBayesNet< GUM_SCALAR >& bn);

    virtual ~BayesNetFragment();
    /// @}

    /// @name signals
    /// @{

    /// the action to take when a new node is inserted into the graph
    /** @param src the object that sent the signal
     * @param id the id of the new node inserted into the graph */
    virtual void whenNodeAdded(const void* src, NodeId id) final;

    /// the action to take when a node has just been removed from the graph
    /** @param src the object that sent the signal
     * @param id the id of the node has just been removed from the graph */
    virtual void whenNodeDeleted(const void* src, NodeId id) final;

    /// the action to take when a new arc is inserted into the graph
    /** @param src the object that sent the signal
     * @param from the id of tail of the new arc inserted into the graph
     * @param to the id of head of the new arc inserted into the graph */
    virtual void whenArcAdded(const void* src, NodeId from, NodeId to) final;

    /// the action to take when an arc has just been removed from the graph
    /** @param src the object that sent the signal
     * @param from the id of tail of the arc removed from the graph
     * @param to the id of head of the arc removed from the graph */
    virtual void whenArcDeleted(const void* src, NodeId from, NodeId to) final;
    /// @}

    /// @name IBayesNet interface
    /// @{

    /**
     * Returns the CPT of a variable.
     *
     * @throw NotFound If no variable's id matches varId.
     */
    const Potential< GUM_SCALAR >& cpt(NodeId varId) const final;
    const Potential< GUM_SCALAR >& cpt(const std::string& name) const {
      return cpt(idFromName(name));
    };

    /**
     * Returns a constant reference to the VariableNodeMap of this BN
     */
    const VariableNodeMap& variableNodeMap() const final;

    /**
     * Returns a constant reference over a variabe given it's node id.
     *
     * @throw NotFound If no variable's id matches varId.
     */
    virtual const DiscreteVariable& variable(NodeId id) const final;
    virtual const DiscreteVariable& variable(const std::string& name) const final {
      return variable(idFromName(name));
    };

    /**
     * Return id node from discrete var pointer.
     *
     * @throw NotFound If no variable matches var.
     */
    virtual NodeId nodeId(const DiscreteVariable& var) const final;

    /**
     * Getter by name
     *
     * @throw NotFound if no such name exists in the graph.
     */
    virtual NodeId idFromName(const std::string& name) const final;

    /**
     * Getter by name
     *
     * @throw NotFound if no such name exists in the graph.
     */
    virtual const DiscreteVariable& variableFromName(const std::string& name) const final;

    /**
     * creates a dot representing the whole referred BN hilighting the fragment.
     * @return Returns a dot representation of this fragment
     */
    virtual std::string toDot() const final;

    /// @}

    /// @name API for Fragment
    /// @{

    /**
     * check if a certain NodeId exists in the fragment
     */
    bool isInstalledNode(NodeId id) const;
    bool isInstalledNode(const std::string& name) const {
      return isInstalledNode(idFromName(name));
    };

    /**
     * install a node referenced by its nodeId
     *
     * @throw NotFound if the node does not exist in the referred BN
     * @warning nothing happens if the node is already installed
     */
    void installNode(NodeId id);
    void installNode(const std::string& name) { installNode(_bn_.idFromName(name)); }

    /**
     * install a node and all its ascendants
     *
     * @throw NotFound if the node does not exist in the referred BN
     * @warning nothing happens if the node is already installed
     */
    void installAscendants(NodeId id);
    void installAscendants(const std::string& name) { installAscendants(_bn_.idFromName(name)); }

    /**
     * uninstall a node referenced by its nodeId
     *
     * @warning nothing happens if the node is not installed
     */
    void uninstallNode(NodeId id);
    void uninstallNode(const std::string& name) { uninstallNode(idFromName(name)); }

    /**
     * install a local marginal BY COPY for a node into the fragment.
     * This function will remove all the arcs from the parents to the node.
     * @param id the nodeId
     * @param pot the potential
     * @throw NotFound if the id is not in the fragment
     * @throw OperationNotAllowed if the potential is not compliant with the
     *variable
     *(or is not a marginal)
     **/
    void installMarginal(NodeId id, const Potential< GUM_SCALAR >& pot);
    void installMarginal(const std::string& name, const Potential< GUM_SCALAR >& pot) {
      installMarginal(_bn_.idFromName(name), pot);
    }

    /**
     * install a local cpt BY COPYfor a node into the fragment.
     * This function will change the arcs from the parents to the node in order
     *to be
     * consistent with the new local potential.
     * @param id the nodeId
     * @param pot the potential to be copied
     *
     * @throw NotFound if the id is not in the fragment
     * @throw OperationNotAllowed if the potential is not compliant with the
     *variable or if  a variable in the CPT is not a parent in the referred bn.
     **/
    void installCPT(NodeId id, const Potential< GUM_SCALAR >& pot);
    void installCPT(const std::string& name, const Potential< GUM_SCALAR >& pot) {
      installCPT(_bn_.idFromName(name), pot);
    };

    /**
     * uninstall a local CPT.
     *
     * @warning  Nothing happens if no local CPT for this nodeId or if the node
     *is
     *not installed.
     */
    void uninstallCPT(NodeId id);
    void uninstallCPT(const std::string& name) { uninstallCPT(idFromName(name)); }

    /**
     * returns true if the nodeId's (local or not) cpt is consistent with its
     * parents
     * in the fragment
     * @throw NotFound if the id is not in the fragment
     */
    bool checkConsistency(NodeId id) const;
    bool checkConsistency(const std::string& name) const {
      return checkConsistency(idFromName(name));
    }

    /**
     * returns true if all nodes in the fragment are consistent
     *
     * @throws gum::OperatioNotAllowed if the fragment is not consistent.
     */
    bool checkConsistency() const;

    /// @}


    /** create a brand new BayesNet from a fragment.
     *
     * @return the new BayesNet<GUM_SCALAR>
     */
    gum::BayesNet< GUM_SCALAR > toBN() const;

    using IBayesNet< GUM_SCALAR >::nodes;
    using IBayesNet< GUM_SCALAR >::dag;

    protected:
    // remove an arc
    void uninstallArc_(NodeId from, NodeId to);

    // add an arc
    void installArc_(NodeId from, NodeId to);

    // install a CPT BY COPY, create or delete arcs. Checks are made in public
    // methods In particular, it is assumed that all the variables in the pot are
    // in the fragment
    void installCPT_(NodeId id, const Potential< GUM_SCALAR >& pot);

    /**
     * uninstall a local CPT. Does nothing if no local CPT for this nodeId
     * No check. No change in the topology. Checks are made in public methods.
     */
    void uninstallCPT_(NodeId id);
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class BayesNetFragment< double >;
#endif

}   // namespace gum

#include <agrum/BN/BayesNetFragment_tpl.h>

#endif   // GUM_BAYES_NET_FRAGMENT_H
