
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @author Pierre-Henri WUILLEMIN and Lionel TORTI
 *
 */
#ifndef GUM_BAYES_NET_H
#define GUM_BAYES_NET_H

#include <utility>

#include <agrum/agrum.h>

#include <agrum/core/hashTable.h>

#include <agrum/BN/IBayesNet.h>
#include <agrum/multidim/potential.h>

namespace gum {

  template < typename GUM_SCALAR >
  class BayesNetFactory;

  /**
   * @class BayesNet
   * @headerfile BayesNet.h <agrum/BN/BayesNet.h>
   * @brief Class representing a Bayesian Network.
   * @ingroup bn_group
   *
   * Bayesian Networks are a probabilistic graphical model in which nodes are
   * random variables and the probability distribution is defined by the
   * product:
   *
   * <center>\f$P(X_1, \ldots, X_2) = \prod_{i=1}^{n} P(X_i |
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
   * After a variable is added to the BN, if it's domain size changes, then the
   * data in it's CPT is lost.
   *
   * You should look a the gum::BayesNetFactory class which can help build
   * Bayesian Networks.
   *
   * You can print a BayesNet using
   * gum::operator<<(std::ostream&, const BayesNet<GUM_SCALAR>&).
   */
  template < typename GUM_SCALAR >
  class BayesNet : public IBayesNet< GUM_SCALAR > {
    friend class BayesNetFactory< GUM_SCALAR >;

    public:
    /**
     * Create a bn with a dotlike syntax : 'a->b->c;b->d;'. The domain size maybe
     * specified using 'a[10]' or using 'a{yes|maybe|no}'. Note that if the dotlike
     * string contains such a
     * specification  for an already defined variable, the first specification will
     * be used.
     *
     * @param dotlike the string containing the specification
     * @param domainSize the default domain size for variables
     * @return the resulting bayesian network
     */
    static BayesNet< GUM_SCALAR > fastPrototype(const std::string& dotlike,
                                                Size               domainSize = 2);

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
    ~BayesNet() final;

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
     * @brief Shortcut for add(gum::LabelizedVariable(name,name,nbrmod))
     *
     * Add a gum::LabelizedVariable to the gum::BayesNet
     *
     * This method is just a shortcut for a often used pattern
     *
     * @throws DuplicateLabel Raised if variable.name() is already used in this
     *                        gum::BayesNet.
     * @throws NotAllowed if nbrmod<2
     */
    NodeId add(const std::string& name, unsigned int nbrmod);

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
    NodeId add(const DiscreteVariable&               var,
               MultiDimImplementation< GUM_SCALAR >* aContent);

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
    NodeId add(const DiscreteVariable&               var,
               MultiDimImplementation< GUM_SCALAR >* aContent,
               NodeId                                id);

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
    void changeVariableLabel(NodeId             id,
                             const std::string& old_label,
                             const std::string& new_label);

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
    NodeId addNoisyORCompound(const DiscreteVariable& var,
                              GUM_SCALAR              external_weight);
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
    NodeId addNoisyOR(const DiscreteVariable& var,
                      GUM_SCALAR              external_weight,
                      NodeId                  id);
    NodeId addNoisyORNet(const DiscreteVariable& var,
                         GUM_SCALAR              external_weight,
                         NodeId                  id);
    NodeId addNoisyORCompound(const DiscreteVariable& var,
                              GUM_SCALAR              external_weight,
                              NodeId                  id);
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
    NodeId addNoisyAND(const DiscreteVariable& var,
                       GUM_SCALAR              external_weight,
                       NodeId                  id);

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
    NodeId addLogit(const DiscreteVariable& var,
                    GUM_SCALAR              external_weight,
                    NodeId                  id);

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
    void addWeightedArc(const std::string& tail,
                        const std::string& head,
                        GUM_SCALAR         causalWeight) {
      addWeightedArc(idFromName(tail), idFromName(head), causalWeight);
    };
    /// @}

    /// randomly generates CPTs for a given structure
    void generateCPTs() const;

    /// randomly generate CPT for a given node in a given structure
    void generateCPT(NodeId node) const;
    void generateCPT(const std::string& name) const {
      generateCPT(idFromName(name));
    };

    /// change the CPT associated to nodeId to newPot
    /// delete the old CPT associated to nodeId.
    /// @throw NotAllowed if newPot has not the same signature as
    /// __probaMap[NodeId]
    void changePotential(NodeId id, Potential< GUM_SCALAR >* newPot);
    void changePotential(const std::string& name, Potential< GUM_SCALAR >* newPot);

    private:
    /// clear all potentials
    void __clearPotentials();

    /// copy of potentials from a BN to another, using names of vars as ref.
    void __copyPotentials(const BayesNet< GUM_SCALAR >& source);

    /// the map between variable and id
    VariableNodeMap __varMap;

    /// Mapping between the variable's id and their CPT.
    NodeProperty< Potential< GUM_SCALAR >* > __probaMap;
    // HashTable<NodeId, Potential<GUM_SCALAR>* > __probaMap;

    /// change the CPT associated to nodeId to newPot
    /// delete the old CPT associated to nodeId.
    /// @warning no verification of dimensions are performer
    /// @see changePotential
    void _unsafeChangePotential(NodeId id, Potential< GUM_SCALAR >* newPot);

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
