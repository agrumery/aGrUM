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
 * @brief Class representing Bayesian networks
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Lionel TORTI
 *
 */
#ifndef GUM_BAYES_NET_H
#define GUM_BAYES_NET_H

#include <utility>

#include <agrum/agrum.h>


#include <agrum/BN/IBayesNet.h>

namespace gum {

  template < typename GUM_SCALAR >
  class BayesNetFactory;

  /**
   * @class BayesNet
   * @headerfile BayesNet.h <agrum/BN/BayesNet.h>
   * @brief Class representing a Bayesian network.
   * @ingroup bn_group
   *
   * Bayesian networks are a probabilistic graphical model in which nodes are
   * random variables and the probability distribution is defined by the
   * product:
   *
   * <center>\f$P(X_1, \ldots, X_n) = \prod_{i=1}^{n} P(X_i |
   * \pi(X_i))\f$,</center>
   *
   * where \f$\pi(X_i)\f$ is the parent of \f$X_i\f$.
   *
   * The probability distribution can be represented as a directed acyclic
   * graph (DAG) where:
   *  - Nodes are discrete random variables.
   *  - An arc A -> B represent a dependency between variables A and B, i.e. B
   *    conditional probability distribution is defined as \f$P(B| \pi(B)\f$.
   *
   * After a variable is added to the BN, it's domain cannot change. But it arcs
   * are added, the data in its CPT are lost.
   *
   * You should look a the gum::BayesNetFactory class which can help build
   * Bayesian networks.
   *
   * You can print a BayesNet using
   * gum::operator<<(std::ostream&, const BayesNet<GUM_SCALAR>&).
   */
  template < typename GUM_SCALAR >
  class BayesNet: public IBayesNet< GUM_SCALAR > {
    friend class BayesNetFactory< GUM_SCALAR >;

    public:
    /**
     * Create a Bayesian network with a dot-like syntax which specifies:
     *   - the structure "a->b->c;b->d<-e;".
     *   - the type of the variables with different syntax:
     *     + by default, a variable is a gum::RangeVariable using the default
     * domainSize (second argument)
     *     + with "a[10]", the variable is a gum::RangeVariable using 10 as
     * domainSize (from 0 to 9)
     *     + with "a[3,7]", the variable is a gum::RangeVariable using a domainSize
     * from 3 to 7
     *     + with "a[1,3.14,5,6.2]", the variable is a gum::DiscretizedVariable
     * using the given ticks (at least 3 values)
     *     + with "a{top|middle|bottom}", the variable is a gum::LabelizedVariable
     * using the given labels.
     *
     * Note that if the dot-like string contains such a specification more than
     * once for a variable, the first specification will be used.
     *
     * @param dotlike the string containing the specification
     * @param domainSize the default domain size for variables
     * @return the resulting Bayesian network
     */
    static BayesNet< GUM_SCALAR > fastPrototype(const std::string& dotlike, Size domainSize = 2);

    // ===========================================================================
    /// @name Constructors and Destructor
    // ===========================================================================
    /// @{

    /**
     * @brief Default constructor.
     */
    BayesNet();

    /**
     * @brief Default constructor.
     *
     * @param name The BayesNet's name.
     */
    explicit BayesNet(std::string name);

    /**
     * @brief Destructor.
     */
    virtual ~BayesNet();

    /**
     * @brief Copy constructor.
     */
    BayesNet(const BayesNet< GUM_SCALAR >& source);

    /// @}
    // ===========================================================================
    /// @name Operators
    // ===========================================================================
    /// @{

    /**
     * @brief Copy operator.
     *
     * @param source The copied BayesNet.
     * @return The copy of source.
     */
    BayesNet< GUM_SCALAR >& operator=(const BayesNet< GUM_SCALAR >& source);

    /// @}
    // ===========================================================================
    /// @name Variable manipulation methods
    // ===========================================================================
    /// @{

    /**
     * @brief Returns the CPT of a variable.
     *
     * @param varId A variable's id in the gum::BayesNet.
     * @return The variable's CPT.
     * @throw NotFound If no variable's id matches varId.
     */
    const Potential< GUM_SCALAR >& cpt(NodeId varId) const final;

    /**
     * @brief Returns the CPT of a variable.
     */
    const Potential< GUM_SCALAR >& cpt(const std::string& name) const {
      return cpt(idFromName(name));
    };

    /**
     * @brief Returns a map between variables and nodes of this gum::BayesNet.
     *
     * @return Returns a constant reference to the gum::VariableNodeMap.
     */
    const VariableNodeMap& variableNodeMap() const final;

    /**
     * @brief Add a variable to the gum::BayesNet.
     *
     * Add a gum::DiscreteVariable, it's associated gum::NodeId and it's
     * gum::Potential.
     *
     * The variable is added by copy to the gum::BayesNet.
     * The variable's gum::Potential implementation will be a
     * gum::MultiDimArray.
     *
     * @param var The variable added by copy.
     * @return Returns the variable's id in the gum::BayesNet.
     * @throws DuplicateLabel Raised if variable.name() is already used in this
     *                        gum::BayesNet.
     */
    NodeId add(const DiscreteVariable& var);

    /**
     * Use "fast" syntax to add a variable in the BayesNet.
     *   - a : range variable from 0 to default_nbrmod-1
     *   - a[5] : range variable from 0 to 5
     *   - a[-3,5] : range variable from -3 to 5
     *   - a[1,3.14,5,3] : discretized variable
     *   - a{x|y|z} : labelized variable
     *   - a{-3|0|3|100} : integer variable
     *
     * @param fast_description(: str) following "fast" syntax description
     * @param default_nbrmod(: int) nbr of modality if fast_description do not indicate it.
     * default_nbrmod=1 is the way to create a variable with only one value (for instance for reward
     * in influence diagram).
     *
     * @throws DuplicateLabel Raised if variable.name() is already used in this
     *                        gum::BayesNet.
     * @throws NotAllowed if nbrmod<2
     */
    NodeId add(const std::string& fast_description, unsigned int default_nbrmod = 2);

    /**
     * @brief Add a variable to the gum::BayesNet.
     *
     * Add a gum::DiscreteVariable, it's associated gum::NodeId and it's
     * gum::Potential.
     *
     * The variable is added by copy to the gum::BayesNet.
     *
     * @param var The variable added by copy.
     * @param aContent The gum::MultiDimImplementation to use for this
     *                 variable's gum::Potential implementation.
     * @return Returns the variable's id in the gum::BayesNet.
     * @throws DuplicateLabel Raised if variable.name() is already used in this
     *                        gum::BayesNet.
     */
    NodeId add(const DiscreteVariable& var, MultiDimImplementation< GUM_SCALAR >* aContent);

    /**
     * @brief Add a variable to the gum::BayesNet.
     *
     * Add a gum::DiscreteVariable, it's associated gum::NodeId and it's
     * gum::Potential.
     *
     * The variable is added by copy to the gum::BayesNet.
     * The variable's gum::Potential implementation will be a
     * gum::MultiDimArray.
     *
     * @param var The variable added by copy.
     * @param id The variable's forced gum::NodeId in the gum::BayesNet.
     * @return Returns the variable's id in the gum::BayesNet.
     * @throws DuplicateElement Raised id is already used.
     * @throws DuplicateLabel Raised if variable.name() is already used in this
     *                        gum::BayesNet.
     */
    NodeId add(const DiscreteVariable& var, NodeId id);

    /**
     * @brief Add a variable to the gum::BayesNet.
     *
     * Add a gum::DiscreteVariable, it's associated gum::NodeId and it's
     * gum::Potential.
     *
     * @param var The variable added by copy.
     * @param aContent The gum::MultiDimImplementation to use for this
     *                 variable's gum::Potential implementation.
     * @param id The variable's forced gum::NodeId in the gum::BayesNet.
     * @return Returns the variable's id in the gum::BayesNet.
     * @throws DuplicateElement Raised id is already used.
     * @throws DuplicateLabel Raised if variable.name() is already used in this
     *                        gum::BayesNet.
     */
    NodeId
       add(const DiscreteVariable& var, MultiDimImplementation< GUM_SCALAR >* aContent, NodeId id);

    /**
     * @brief clear the whole Bayes net     *
     */
    void clear();

    /**
     * @brief Remove a variable from the gum::BayesNet.
     *
     * Removes the corresponding variable from the gum::BayesNet and from
     * all of it's children gum::Potential.
     *
     * If no variable matches the given id, then nothing is done.
     *
     * @param varId The variable's id to remove.
     */
    void erase(NodeId varId);

    /**
     * @brief Removes a variable from the gum::BayesNet.
     */
    void erase(const std::string& name) { erase(idFromName(name)); };

    /**
     * @brief Remove a variable from the gum::BayesNet.
     *
     * Removes the corresponding variable from the gum::BayesNet and from
     * all of it's children gum::Potential.
     *
     * If no variable matches the given variable, then nothing is done.
     *
     * @param var A reference on the variable to remove.
     */
    void erase(const DiscreteVariable& var);

    /**
     * @brief Returns a gum::DiscreteVariable given its gum::NodeId in the
     *        gum::BayesNet.
     *
     * @param id The variable's id to return.
     * @returns Returns a constant reference of the gum::DiscreteVariable
     *          corresponding to id in the gum::BayesNet.
     * @throw NotFound Raised if id does not match a a variable in the
     *                 gum::BayesNet.
     */
    const DiscreteVariable& variable(NodeId id) const final;

    /**
     * @brief Returns a gum::DiscreteVariable given its gum::NodeId in the
     *        gum::BayesNet.
     */
    const DiscreteVariable& variable(const std::string& name) const {
      return variable(idFromName(name));
    };

    /**
     * @brief Changes a variable's name in the gum::BayesNet.
     *
     * This will change the gum::DiscreteVariable names in the gum::BayesNet.
     *
     * @throws DuplicateLabel Raised if newName is already used in this
     *                        gum::BayesNet.
     * @throws NotFound Raised if no variable matches id.
     */
    void changeVariableName(NodeId id, const std::string& new_name);

    /**
     * @brief Changes a variable's name.
     */
    void changeVariableName(const std::string& name, const std::string& new_name) {
      changeVariableName(idFromName(name), new_name);
    }

    /**
     * @brief Changes a variable's label in the gum::BayesNet.
     *
     * This will change the gum::LabelizedVariable names in the gum::BayesNet.
     *
     * @throws DuplicateLabel Raised if new_label is already used in this
     *                        gum::LabelizedVariable.
     * @throws NotFound Raised if no variable matches id or if the variable is not
     * a LabelizedVariable
     */
    void changeVariableLabel(NodeId id, const std::string& old_label, const std::string& new_label);

    /**
     * @brief Changes a variable's name.
     */
    void changeVariableLabel(const std::string& name,
                             const std::string& old_label,
                             const std::string& new_label) {
      changeVariableLabel(idFromName(name), old_label, new_label);
    }

    /**
     * @brief Returns a variable's id in the gum::BayesNet.
     *
     * @param var The variable from which the gum::NodeId is returned.
     * @return Returns the gum::DiscreteVariable gum::NodeId in the
     *         gum::BayesNet.
     * @throw NotFound If var is not in the gum::BayesNet.
     */
    NodeId nodeId(const DiscreteVariable& var) const final;

    /**
     * @brief Returns a variable's id given its name in the gum::BayesNet.
     *
     * @param name The variable's name from which the gum::NodeId is returned.
     * @return Returns the variable gum::NodeId in the gum::BayesNet.
     * @throw NotFound Raised if name does not match a variable in the
     * gum::BayesNet.
     */
    NodeId idFromName(const std::string& name) const final;

    /**
     * @brief Returns a variable given its name in the gum::BayesNet.
     *
     * @param name The variable's name in the gum::BayesNet.
     * @return Returns the gum::DiscreteVariable named name in the
     * gum::BayesNet.
     * @throw NotFound Raised if name does not match a variable in the
     * gum::BayesNet.
     */
    const DiscreteVariable& variableFromName(const std::string& name) const final;
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
     * @throw DuplicateElement if the arc already exists
     */
    void addArc(NodeId tail, NodeId head);

    /**
     * Add an arc in the BN, and update arc.head's CPT
     *
     * @throw gum::DuplicateElement if the arc already exists
     */
    void addArc(const std::string& tail, const std::string& head);

    /**
     * Removes an arc in the BN, and update head's CTP.
     *
     * If (tail, head) doesn't exist, the nothing happens.
     * @param arc The arc removed.
     */
    void eraseArc(const Arc& arc);

    /**
     * Removes an arc in the BN, and update head's CTP.
     *
     * If (tail, head) doesn't exist, the nothing happens.
     * @param head and
     * @param tail as NodeId
     */
    void eraseArc(NodeId tail, NodeId head);

    /**
     * Removes an arc in the BN, and update head's CTP.
     */
    void eraseArc(const std::string& tail, const std::string& head) {
      eraseArc(idFromName(tail), idFromName(head));
    }

    /**
     * When inserting/removing arcs, node CPTs change their dimension with a
     * cost in
     * time.
     * These functions delay the CPTs change to be done just once at the end of
     * a
     * sequence of topology modification.
     */
    ///@{
    /// begins a sequence of insertions/deletions of arcs without changing the
    /// dimensions of the CPTs.
    void beginTopologyTransformation();
    /// terminates a sequence of insertions/deletions of arcs by adjusting all
    /// CPTs
    /// dimensions.
    void endTopologyTransformation();
    ///@}

    /**
     * @brief Reverses an arc while preserving the same joint distribution.
     *
     * This method uses Shachter's 1986 algorithm for reversing an arc in
     * the Bayes net while preserving the same joint distribution. By
     * performing this reversal, we also add new arcs (required to not alter
     * the joint distribution)
     * @throws InvalidArc exception if the arc does not exist or if its reversal
     * would induce a directed cycle. */
    /// @{
    void reverseArc(NodeId tail, NodeId head);
    void reverseArc(const std::string& tail, const std::string& head) {
      reverseArc(idFromName(tail), idFromName(head));
    }
    void reverseArc(const Arc& arc);
    /// @}

    /// @}

    // ===========================================================================
    /// @name Accessors for nodes with CI or logical implementation
    // ===========================================================================
    /// @{

    /**
     * Add a variable, it's associate node and a gum::noisyOR implementation.
     *The id
     *of the new
     * variable is automatically generated. Since it seems that the 'classical'
     *noisyOR is the Compound noisyOR, we keep
     * the gum::BayesNet::addNoisyOR as an alias for
     *gum::BayesNet::addNoisyORCompound
     *
     * @param var The variable added by copy.
     * @param external_weight see ref
     *gum::MultiDimNoisyORNet,gum::MultiDimNoisyORCompound
     * @return the id of the added variable.
     * @{
     */

    NodeId addNoisyOR(const DiscreteVariable& var, GUM_SCALAR external_weight);
    NodeId addNoisyORNet(const DiscreteVariable& var, GUM_SCALAR external_weight);
    NodeId addNoisyORCompound(const DiscreteVariable& var, GUM_SCALAR external_weight);
    /** @} */

    /**
     * Add a variable, its associate node and a noisyOR implementation. Since
     * it seems that the 'classical' noisyOR is the Compound noisyOR, we keep
     * the addNoisyOR as an alias for addNoisyORCompound.
     *
     * @param var The variable added by copy.
     * @param external_weight see gum::MultiDimNoisyORNet,
     *gum::MultiDimNoisyORCompound
     * @param id The chosen id
     * @warning give an id should be reserved for rare and specific situations
     *!!!
     * @return the id of the added variable.
     * @throws DuplicateElement if id is already used
     *
     * @{
     */
    NodeId addNoisyOR(const DiscreteVariable& var, GUM_SCALAR external_weight, NodeId id);
    NodeId addNoisyORNet(const DiscreteVariable& var, GUM_SCALAR external_weight, NodeId id);
    NodeId addNoisyORCompound(const DiscreteVariable& var, GUM_SCALAR external_weight, NodeId id);
    /** @} */

    /**
     * Add a variable, its associate node and a noisyAND implementation.
     *
     * @param var The variable added by copy
     * @param external_weight see gum::MultiDimNoisyAND
     * @param id proposed gum::nodeId for the variable
     * @warning give an id should be reserved for rare and specific situations
     *!!!
     * @return the id of the added variable.
     */
    NodeId addNoisyAND(const DiscreteVariable& var, GUM_SCALAR external_weight, NodeId id);

    /**
     * Add a variable, its associate node and a noisyAND implementation. The id
     *of the new variable is automatically generated.
     *
     * @param var The variable added by copy.
     * @param external_weight see gum::MultiDimNoisyAND
     * @return the id of the added variable.
     */
    NodeId addNoisyAND(const DiscreteVariable& var, GUM_SCALAR external_weight);

    /**
     * Add a variable, its associate node and a Logit implementation.
     *
     * @param var The variable added by copy
     * @param external_weight see gum::MultiDimLogit
     * @param id proposed gum::nodeId for the variable
     * @warning give an id should be reserved for rare and specific situations
     *!!!
     * @return the id of the added variable.
     */
    NodeId addLogit(const DiscreteVariable& var, GUM_SCALAR external_weight, NodeId id);

    /**
     * Add a variable, its associate node and a Logit implementation. The id of
     *the new variable is automatically generated.
     *
     * @param var The variable added by copy.
     * @param external_weight see gum::MultiDimLogit
     * @return the id of the added variable.
     */
    NodeId addLogit(const DiscreteVariable& var, GUM_SCALAR external_weight);

    /**
     * Add a variable, it's associate node and an OR implementation. The id of
     *the new variable is automatically generated.
     *
     * @warning OR is implemented as a gum::aggregator::Or which means that if
     *parents are not boolean, all value>1 is True
     *
     * @param var The variable added by copy.
     * @return the id of the added variable.
     * @throws SizeError if variable.domainSize()>2
     */
    NodeId addOR(const DiscreteVariable& var);

    /**
     * Add a variable, it's associate node and an AND implementation. The id of
     *the new variable is automatically generated.
     *
     * @warning AND is implemented as a gum::aggregator::And which means that if
     *parents are not boolean, all value>1 is True
     *
     * @param var The variable added by copy.
     * @return the id of the added variable.
     * @throws SizeError if variable.domainSize()>2
     */
    NodeId addAND(const DiscreteVariable& var);

    /**
     * Others aggregators
     * @{
     */
    NodeId addAMPLITUDE(const DiscreteVariable& var);
    NodeId addCOUNT(const DiscreteVariable& var, Idx value = 1);
    NodeId addEXISTS(const DiscreteVariable& var, Idx value = 1);
    NodeId addFORALL(const DiscreteVariable& var, Idx value = 1);
    NodeId addMAX(const DiscreteVariable& var);
    NodeId addMEDIAN(const DiscreteVariable& var);
    NodeId addMIN(const DiscreteVariable& var);
    NodeId addSUM(const DiscreteVariable& var);
    /**
     * @}
     */
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

    /**
     * Add an arc in the BN, and update arc.head's CPT.
     *
     * @param head and
     * @param tail as std::string
     * @param causalWeight see gum::MultiDimICIModel
     * @param NotFound if no node with sun names is found
     * @throw InvalidArc If arc.tail and/or arc.head are not in the BN.
     * @throw InvalidArc If variable in arc.head is not a NoisyOR variable.
     */
    void addWeightedArc(const std::string& tail, const std::string& head, GUM_SCALAR causalWeight) {
      addWeightedArc(idFromName(tail), idFromName(head), causalWeight);
    };
    /// @}

    /// randomly generates CPTs for a given structure
    void generateCPTs() const;

    /// randomly generate CPT for a given node in a given structure
    void generateCPT(NodeId node) const;
    void generateCPT(const std::string& name) const { generateCPT(idFromName(name)); };

    /// change the CPT associated to nodeId to newPot
    /// delete the old CPT associated to nodeId.
    /// @throw NotAllowed if newPot has not the same signature as
    ///  _probaMap_[NodeId]
    void changePotential(NodeId id, Potential< GUM_SCALAR >* newPot);
    void changePotential(const std::string& name, Potential< GUM_SCALAR >* newPot);

    private:
    /// clear all potentials
    void _clearPotentials_();

    /// copy of potentials from a BN to another, using names of vars as ref.
    void _copyPotentials_(const BayesNet< GUM_SCALAR >& source);

    /// the map between variable and id
    VariableNodeMap _varMap_;

    /// Mapping between the variable's id and their CPT.
    NodeProperty< Potential< GUM_SCALAR >* > _probaMap_;

    /// change the CPT associated to nodeId to newPot
    /// delete the old CPT associated to nodeId.
    /// @warning no verification of dimensions are performer
    /// @see changePotential
    void _unsafeChangePotential_(NodeId id, Potential< GUM_SCALAR >* newPot);

    public:
    using IBayesNet< GUM_SCALAR >::dag;
    using IBayesNet< GUM_SCALAR >::size;
    using IBayesNet< GUM_SCALAR >::nodes;
    using IBayesNet< GUM_SCALAR >::log10DomainSize;
  };

  /// Prints map's DAG in output using the Graphviz-dot format.
  template < typename GUM_SCALAR >
  std::ostream& operator<<(std::ostream& output, const BayesNet< GUM_SCALAR >& bn);


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class BayesNet< double >;
#endif

} /* namespace gum */

#include <agrum/BN/BayesNet_tpl.h>

#endif /* GUM_BAYES_NET_H */
