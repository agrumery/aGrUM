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
#ifndef GUM_BAYES_NET_H
#define GUM_BAYES_NET_H

#include <utility>

#include <agrum/config.h>

#include <agrum/core/hashTable.h>

#include <agrum/BN/IBayesNet.h>
#include <agrum/multidim/potential.h>

namespace gum {

  template <typename GUM_SCALAR> class BayesNetFactory;

  /**
   * @class BayesNet BayesNet.h <agrum/BN/BayesNet.h>
   * @brief Class representing a Bayesian Network.
   * @ingroup bn_group
   *
   * After a variable is added to the BN, if it's domain size changes, then the
   * data in it's CPT is lost.
   *
   * We recommend you to use the gum::BayesNetFactory class to build a BayesNet.
   *
   * Don't forget that you can print a BayesNet using
   * gum::operator<<(std::ostream&, const BayesNet<GUM_SCALAR>&).
   */
  template <typename GUM_SCALAR> class BayesNet : public IBayesNet<GUM_SCALAR> {

    friend class BayesNetFactory<GUM_SCALAR>;

    public:
    // ===========================================================================
    /// @name Constructors / Destructors
    // ===========================================================================
    /// @{

    /**
     * Default constructor.
     */
    BayesNet();
    BayesNet(std::string name);

    /**
     * Destructor.
     */
    ~BayesNet();

    /**
     * Copy constructor.
     */
    BayesNet(const BayesNet<GUM_SCALAR> &source);

    /**
     * Copy operator.
     */
    BayesNet<GUM_SCALAR> &operator=(const BayesNet<GUM_SCALAR> &source);

    /// @}
    // ===========================================================================
    /// @name Variable manipulation methods.
    // ===========================================================================
    /// @{

    /**
     * Returns the CPT of a variable.
     * @throw NotFound If no variable's id matches varId.
     */
    virtual const Potential<GUM_SCALAR> &cpt(NodeId varId) const /* override*/;

    /**
     * Returns a constant reference to the VariableNodeMap of thisBN
     */
    virtual const VariableNodeMap &variableNodeMap() const /* override */;

    /// @return Returns a dot representation of this BayesNet.
    virtual std::string toDot(void) const /* override */;

    /**
     * Add a variable, it's associate node and it's CPT. The id of the new
     * variable is automatically generated.
     *
     * The implementation of the Potential is by default a MultiDimArray.
     *
     * @param variable The variable added by copy.
     * @return the id of the added variable.
     */
    NodeId add(const DiscreteVariable &variable);

    /**
     * Add a variable, it's associate node and it's CPT. The id of the new
     * variable is automatically generated.
     *
     * @param variable The variable added by copy.
     * @param aContent The content used for the variable potential.
     */
    NodeId add(const DiscreteVariable &variable,
               MultiDimImplementation<GUM_SCALAR> *aContent);

    /**
     * Add a variable, it's associate node and it's CPT. The id of the new
     * variable is automatically generated.
     *
     * The implementation of the Potential is by default a MultiDimArray.
     *
     * @param variable The variable added by copy.
     * @param id The chosen id.
     * @warning give an id  should be reserved for rare and specific situations !!!
     * @return the id of the added variable.
     * @throws DuplicateElement if id is already used
     */
    NodeId add(const DiscreteVariable &variable, NodeId id);

    /**
     * Add a variable, it's associate node and it's CPT. The id of the new
     * variable is automatically generated.
     *
     * @param variable The variable added by copy.
     * @param aContent The content used for the variable potential.
     * @param id The chosen id.
     * @warning give an id should be reserved for rare and specific situations !!!
     * @return the id of the added variable.
     * @throws DuplicateElement if id is already used
     */
    NodeId add(const DiscreteVariable &variable,
               MultiDimImplementation<GUM_SCALAR> *aContent, NodeId id);

    /**
     * Erase a Variable from the network and remove the variable from
     * all childs of id.
     * If no variable matches the id, then nothing is done.
     *
     * @param id The id of the variable to erase.
     */
    void erase(NodeId id);

    /**
     * Erase a Variable from the network and remove the variable from
     * all childs of var.
     * If no variable matches, then nothing is done.
     *
     * @param var The reference on the variable to remove.
     */
    void erase(const DiscreteVariable &var);

    /**
     * Returns a constant reference over a variabe given it's node id.
     * @throw NotFound If no variable's id matches varId.
     */
    const DiscreteVariable &variable(NodeId id) const /*override*/;

    /** we allow the user to change the name of a variable
     * @throws DuplicateLabel if this name already exists
     * @throws NotFound Raised if no nodes matches id.
     */
    void changeVariableName(NodeId id, const std::string &new_name);

    /**
     * Return id node from discrete var pointer.
     * @throw NotFound If no variable matches var.
     */
    NodeId nodeId(const DiscreteVariable &var) const /*override*/;

    /// Getter by name
    /// @throw NotFound if no such name exists in the graph.
    NodeId idFromName(const std::string &name) const /*override*/;

    /// Getter by name
    /// @throw NotFound if no such name exists in the graph.
    const DiscreteVariable &variableFromName(const std::string &name) const
        /*override*/;
    /// @}

    // ===========================================================================
    /// @name Arc manipulation methods.
    // ===========================================================================
    /// @{

    /**
     * Add an arc in the BN, and update arc.head's CPT.
     *
     * @param head and
     * @param tail as NodeId
     * @throw InvalidEdge If arc.tail and/or arc.head are not in the BN.
     * @deprecated This function is now deprecated. Please use BayesNet::addArc
     *instead
     */
    GUM_DEPRECATED(void insertArc(NodeId tail, NodeId head));

    /**
     * Add an arc in the BN, and update arc.head's CPT.
     *
     * @param head and
     * @param tail as NodeId
     * @throw InvalidEdge If arc.tail and/or arc.head are not in the BN.
     */
    void addArc(NodeId tail, NodeId head);

    /**
     * Removes an arc in the BN, and update head's CTP.
     *
     * If (tail, head) doesn't exist, the nothing happens.
     * @param arc The arc removed.
     */
    void eraseArc(const Arc &arc);

    /**
     * Removes an arc in the BN, and update head's CTP.
     *
     * If (tail, head) doesn't exist, the nothing happens.
     * @param head and
     * @param tail as NodeId
     */
    void eraseArc(NodeId tail, NodeId head);

    /**
     * When inserting/removing arcs, node CPTs change their dimension with a cost in
     * time.
     * These functions delay the CPTs change to be done just once at the end of a
     * sequence of topology modification.
     */
    ///@{
    /// begins a sequence of insertions/deletions of arcs without changing the
    /// dimensions of the CPTs.
    void beginTopologyTransformation();
    /// terminates a sequence of insertions/deletions of arcs by adjusting all CPTs
    /// dimensions.
    void endTopologyTransformation();
    ///@}

    /// reverses an arc while preserving the same joint distribution
    /** This method uses Shachter's 1986 algorithm for reversing an arc in
     * the Bayes net while preserving the same joint distribution. By
     * performing this reversal, we also add new arcs (required to not alter
     * the joint distribution)
     * @throws InvalidArc exception if the arc does not exist or if its reversal
     * would induce a directed cycle. */
    /// @{
    void reverseArc(NodeId tail, NodeId head);
    void reverseArc(const Arc &arc);
    /// @}

    /// @}

    // ===========================================================================
    /// @name Accessors for nodes with CI or logical implementation
    // ===========================================================================
    /// @{

    /**
     * Add a variable, it's associate node and a gum::noisyOR implementation. The id
     *of the new
     * variable is automatically generated. Since it seems that the 'classical'
     *noisyOR is the Compound noisyOR, we keep
     * the gum::BayesNet::addNoisyOR as an alias for
     *gum::BayesNet::addNoisyORCompound
     *
     * @param variable The variable added by copy.
     * @param externalWeight see ref
     *gum::MultiDimNoisyORNet,gum::MultiDimNoisyORCompound
     * @return the id of the added variable.
     * @{
     */

    NodeId addNoisyOR(const DiscreteVariable &variable, GUM_SCALAR externalWeight);
    NodeId addNoisyORNet(const DiscreteVariable &variable,
                         GUM_SCALAR externalWeight);
    NodeId addNoisyORCompound(const DiscreteVariable &variable,
                              GUM_SCALAR externalWeight);
    /** @} */

    /**
     * Add a variable, its associate node and a noisyOR implementation.
     * Since it seems that the 'classical' noisyOR is the Compound noisyOR, we keep
     * the addNoisyOR as an alias for addNoisyORCompound
     *
     * @param variable The variable added by copy.
     * @param externalWeight see gum::MultiDimNoisyORNet,
     *gum::MultiDimNoisyORCompound
     * @param id The chosen id
     * @warning give an id should be reserved for rare and specific situations !!!
     * @return the id of the added variable.
     * @throws DuplicateElement if id is already used
     *
     * @{
     */
    NodeId addNoisyOR(const DiscreteVariable &variable, GUM_SCALAR externalWeight,
                      NodeId id);
    NodeId addNoisyORNet(const DiscreteVariable &variable, GUM_SCALAR externalWeight,
                         NodeId id);
    NodeId addNoisyORCompound(const DiscreteVariable &variable,
                              GUM_SCALAR externalWeight, NodeId id);
    /** @} */

    /**
     * Add a variable, its associate node and a noisyAND implementation.
     *
     * @param variable The variable added by copy
     * @param externalWeight see gum::MultiDimNoisyAND
     * @param id proposed gum::nodeId for the variable
     * @warning give an id should be reserved for rare and specific situations !!!
     * @return the id of the added variable.
     */
    NodeId addNoisyAND(const DiscreteVariable &variable, GUM_SCALAR externalWeight,
                       NodeId id);

    /**
     * Add a variable, its associate node and a noisyAND implementation. The id of
     *the new
     * variable is automatically generated.
     *
     * @param variable The variable added by copy.
     * @param externalWeight see gum::MultiDimNoisyAND
     * @return the id of the added variable.
     */
    NodeId addNoisyAND(const DiscreteVariable &variable, GUM_SCALAR externalWeight);

    /**
     * Add a variable, its associate node and a Logit implementation.
     *
     * @param variable The variable added by copy
     * @param externalWeight see gum::MultiDimLogit
     * @param id proposed gum::nodeId for the variable
     * @warning give an id should be reserved for rare and specific situations !!!
     * @return the id of the added variable.
     */
    NodeId addLogit(const DiscreteVariable &variable, GUM_SCALAR externalWeight,
                    NodeId id);

    /**
     * Add a variable, its associate node and a Logit implementation. The id of the
     *new
     * variable is automatically generated.
     *
     * @param variable The variable added by copy.
     * @param externalWeight see gum::MultiDimLogit
     * @return the id of the added variable.
     */
    NodeId addLogit(const DiscreteVariable &variable, GUM_SCALAR externalWeight);

    /**
     * Add a variable, it's associate node and an OR implementation. The id of the
     *new
     * variable is automatically generated.
     *
     * @warning OR is implemented as a gum::aggregator::Or which means that if
     *parents are not boolean, all value>1 is True
     *
     * @param variable The variable added by copy.
     * @return the id of the added variable.
     * @throws SizeError if variable.domainSize()>2
     */
    NodeId addOR(const DiscreteVariable &variable);

    /**
     * Add a variable, it's associate node and an AND implementation. The id of the
     *new
     * variable is automatically generated.
     *
     * @warning AND is implemented as a gum::aggregator::And which means that if
     *parents are not boolean, all value>1 is True
     *
     * @param variable The variable added by copy.
     * @return the id of the added variable.
     * @throws SizeError if variable.domainSize()>2
     */
    NodeId addAND(const DiscreteVariable &variable);

    /**
     * Add an arc in the BN, and update arc.head's CPT.
     *
     * @param head and
     * @param tail as NodeId
     * @param causalWeight see gum::MultiDimICIModel
     * @throw InvalidArc If arc.tail and/or arc.head are not in the BN.
     * @throw InvalidArc If variable in arc.head is not a NoisyOR variable.
     *
     * @deprecated This function is now deprecated. Please use
     *BayesNet::addWeightedArc instead
     */
    GUM_DEPRECATED(void insertWeightedArc(NodeId tail, NodeId head,
                                          GUM_SCALAR causalWeight));

    /**
     * Add an arc in the BN, and update arc.head's CPT.
     *
     * @param head and
     * @param tail as NodeId
     * @param causalWeight see gum::MultiDimICIModel
     * @throw InvalidArc If arc.tail and/or arc.head are not in the BN.
     * @throw InvalidArc If variable in arc.head is not a NoisyOR variable.
     */
    void addWeightedArc(NodeId tail, NodeId head, GUM_SCALAR causalWeight);

    /// @}
    // ===========================================================================
    /// @name Deprecated methods
    // ===========================================================================
    /// @{

    /**
     * @deprecated: This function is now deprecated. Please use BayesNet::add
     * instead.
     */
    GUM_DEPRECATED(NodeId addVariable(const DiscreteVariable &variable));

    /**
     * @deprecated: This function is now deprecated. Please use BayesNet::add(const
     * DiscreteVariable&, MultiDimImplementation*)
     */
    GUM_DEPRECATED(NodeId addVariable(const DiscreteVariable &variable,
                                      MultiDimImplementation<GUM_SCALAR> *aContent));

    /**
     * @deprecated: This function is now deprecated. Please use
     * BayesNet::erase(NodeId) instead.
     */
    GUM_DEPRECATED(void eraseVariable(NodeId id));

    /// @}

    /// randomly generates CPTs for a given structure
    void generateCPTs();

    /// change the CPT associated to nodeId to newPot
    /// delete the old CPT associated to nodeId.
    /// @throw NotAllowed if newPot has not the same signature as __probaMap[NodeId]
    void changePotential(NodeId id, Potential<GUM_SCALAR> *newPot);

    private:
    /// clear all potentials
    void __clearPotentials();

    /// copy of potentials from a BN to another, using names of vars as ref.
    void __copyPotentials(const BayesNet<GUM_SCALAR> &bn);

    /// the map between variable and id
    VariableNodeMap __varMap;

    /// Mapping between the variable's id and their CPT.
    NodeProperty<Potential<GUM_SCALAR> *> __probaMap;
    // HashTable<NodeId, Potential<GUM_SCALAR>* > __probaMap;

    /// change the CPT associated to nodeId to newPot
    /// delete the old CPT associated to nodeId.
    /// @warning no verification of dimensions are performer
    /// @see changePotential
    void _unsafeChangePotential(NodeId id, Potential<GUM_SCALAR> *newPot);

    public:
    using IBayesNet<GUM_SCALAR>::dag;
    using IBayesNet<GUM_SCALAR>::size;
    using IBayesNet<GUM_SCALAR>::nodes;
    using IBayesNet<GUM_SCALAR>::log10DomainSize;
  };

  /// Prints map's DAG in output using the Graphviz-dot format.
  template <typename GUM_SCALAR>
  std::ostream &operator<<(std::ostream &output, const BayesNet<GUM_SCALAR> &map);

  extern template class BayesNet<float>;
  extern template class BayesNet<double>;
} /* namespace gum */

#include <agrum/BN/BayesNet.tcc>

#endif /* GUM_BAYES_NET_H */
