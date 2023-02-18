/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) et Christophe GONZALES(_at_AMU)
 * (_at_AMU) info_at_agrum_dot_org
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
 * @brief Class representing Markov random fields
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */
#ifndef GUM_MARKOV_RANDOM_FIELD_H
#define GUM_MARKOV_RANDOM_FIELD_H

#include <utility>

#include <agrum/agrum.h>


#include <agrum/MRF/IMarkovRandomField.h>

#include <agrum/BN/BayesNet.h>

namespace gum {

  /**
   * @class MarkovRandomField
   * @headerfile MarkovRandomField.h <agrum/MRF/MarkovRandomField.h>
   * @brief Class representing a Markov random field.
   * @ingroup mn_group
   *
   * Markov  Networks are a undirected probabilistic graphical model in which nodes
   * are random variables and the probability distribution is defined by the
   * product:
   *
   * <center>\f$P(X_1, \ldots, X_n) \propto \prod_{C \in {\cal C}}
   * f_{C}(X_C)\f$,</center>
   *
   * where \f$f_{C}$ is a factor on a clique of the undirected graph, \f${\cal C}$
   * is the set of cliques in the graph and \f$X_C$ is the instantiation of the
   * variables in the clique f_$C$.
   *
   * The probability distribution can be represented as a undirected acyclic
   * graph where:
   *  - Nodes are discrete random variables.
   *  - An arc A - B represent a dependency between variables A and B.
   *
   *
   * You can print a MarkovRandomField using
   * gum::operator<<(std::ostream&, const MarkovRandomField<GUM_SCALAR>&).
   */
  template < typename GUM_SCALAR >
  class MarkovRandomField: public IMarkovRandomField< GUM_SCALAR > {
    public:
    /**
     * Create a Markov random field with a dot-like syntax which specifies:
     *   - the structure "a-b-c;b-d-e;".
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
    static MarkovRandomField< GUM_SCALAR > fastPrototype(const std::string& dotlike,
                                                         Size               domainSize = 2);

    /**
     * build a Markov random field from a Bayesian network
     * @param bn the Bayesian network
     * @return a Markov random field
     */
    static MarkovRandomField< GUM_SCALAR > fromBN(const BayesNet< GUM_SCALAR >& bn);

    // ===========================================================================
    /// @name Constructors and Destructor
    // ===========================================================================
    /// @{

    /**
     * @brief Default constructor.
     */
    MarkovRandomField();

    /**
     * @brief Default constructor.
     *
     * @param name The MarkovRandomField's name.
     */
    explicit MarkovRandomField(std::string name);

    /**
     * @brief Destructor.
     */
    virtual ~MarkovRandomField();

    /**
     * @brief Copy constructor.
     */
    MarkovRandomField(const MarkovRandomField< GUM_SCALAR >& source);

    /// @}
    // ===========================================================================
    /// @name Operators
    // ===========================================================================
    /// @{

    /**
     * @brief Copy operator.
     *
     * @param source The copied MarkovRandomField.
     * @return The copy of source.
     */
    MarkovRandomField< GUM_SCALAR >& operator=(const MarkovRandomField< GUM_SCALAR >& source);

    /// @}
    // ===========================================================================
    /// @name Variable manipulation methods
    // ===========================================================================
    /// @{

    /**
     * @brief Returns the factor given a nodeset
     *
     * @param varId A variable's id in the gum::MarkovRandomField.
     * @return The variable's CPT.
     * @throw NotFound If no variable's id matches varId.
     */
    virtual const Potential< GUM_SCALAR >& factor(const NodeSet& varIds) const final;

    virtual const Potential< GUM_SCALAR >&
       factor(const std::vector< std::string >& varnames) const final;

    /**
     * Returns a factor that contains this variable
     *
     * @throw NotFound If no variable's id matches varId.
     */
    virtual const NodeSet& smallestFactorFromNode(NodeId node) const final;

    /**
     * Returns the set of factors as a IMarkovRandomField::FactorTable
     *
     */
    virtual const FactorTable< GUM_SCALAR >& factors() const final;

    /**
     * @brief Returns a map between variables and nodes of this gum::MarkovRandomField.
     *
     * @return Returns a constant reference to the gum::VariableNodeMap.
     */
    virtual const VariableNodeMap& variableNodeMap() const final;

    /**
     * @brief Add a variable to the gum::MarkovRandomField.
     *
     * Add a gum::DiscreteVariable, it's associated gum::NodeId
     *opera
     * The variable is added by copy to the gum::MarkovRandomField.
     * The variable's gum::Potential implementation will be a
     * gum::MultiDimArray.
     *
     * @param var The variable added by copy.
     * @return Returns the variable's id in the gum::MarkovRandomField.
     * @throws DuplicateLabel Raised if variable.name() is already used in this
     *                        gum::MarkovRandomField.
     */
    NodeId add(const DiscreteVariable& var);

    /**
     * Use "fast" syntax to add a variable in the MarkovRandomField.
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
     * @brief Add a variable to the gum::MarkovRandomField.
     *
     * Add a gum::DiscreteVariable, it's associated gum::NodeId and it's
     * gum::Potential.
     *
     * The variable is added by copy to the gum::MarkovRandomField.
     * The variable's gum::Potential implementation will be a
     * gum::MultiDimArray.
     *
     * @param var The variable added by copy.
     * @param id The variable's forced gum::NodeId in the gum::MarkovRandomField.
     * @return Returns the variable's id in the gum::MarkovRandomField.
     * @throws DuplicateElement Raised id is already used.
     * @throws DuplicateLabel Raised if variable.name() is already used in this
     *                        gum::MarkovRandomField.
     */
    NodeId add(const DiscreteVariable& var, NodeId id);

    /**
     * @brief clear the whole Markov net
     */
    void clear();

    /**
     * @brief Remove a variable from the gum::MarkovRandomField.
     *
     * Removes the corresponding variable from the gum::MarkovRandomField and from
     * all of the factors.
     *
     * If no variable matches the given id, then nothing is done.
     *
     * @param varId The variable's id to remove.
     */
    void erase(NodeId varId);

    /**
     * @brief Removes a variable from the gum::MarkovRandomField.
     */
    void erase(const std::string& name);

    /**
     * @brief Remove a variable from the gum::MarkovRandomField.
     *
     * Removes the corresponding variable from the gum::MarkovRandomField and from
     * all of the factors.
     *
     * If no variable matches the given variable, then nothing is done.
     *
     * @param var A reference on the variable to remove.
     */
    void erase(const DiscreteVariable& var);

    /**
     * @brief Returns a gum::DiscreteVariable given its gum::NodeId in the
     *        gum::MarkovRandomField.
     *
     * @param id The variable's id to return.
     * @returns Returns a constant reference of the gum::DiscreteVariable
     *          corresponding to id in the gum::MarkovRandomField.
     * @throw NotFound Raised if id does not match a a variable in the
     *                 gum::MarkovRandomField.
     */
    const DiscreteVariable& variable(NodeId id) const final;

    /**
     * @brief Returns a gum::DiscreteVariable given its name in the
     *        gum::MarkovRandomField.
     * @throw NotFound Raised if id does not match a a variable in the
     *                 gum::MarkovRandomField.
     */
    const DiscreteVariable& variable(const std::string& name) const {
      return variable(idFromName(name));
    };

    /**
     * @brief Changes a variable's name in the gum::MarkovRandomField.
     *
     * This will change the gum::DiscreteVariable names in the gum::MarkovRandomField.
     *
     * @throws DuplicateLabel Raised if newName is already used in this
     *                        gum::MarkovRandomField.
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
     * @brief Changes a variable's label in the gum::MarkovRandomField.
     *
     * This will change the gum::LabelizedVariable names in the gum::MarkovRandomField.
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
     * @brief Returns a variable's id in the gum::MarkovRandomField.
     *
     * @param var The variable from which the gum::NodeId is returned.
     * @return Returns the gum::DiscreteVariable gum::NodeId in the
     *         gum::MarkovRandomField.
     * @throw NotFound If var is not in the gum::MarkovRandomField.
     */
    NodeId nodeId(const DiscreteVariable& var) const final;

    /**
     * @brief Returns a variable's id given its name in the gum::MarkovRandomField.
     *
     * @param name The variable's name from which the gum::NodeId is returned.
     * @return Returns the variable gum::NodeId in the gum::MarkovRandomField.
     * @throw NotFound Raised if name does not match a variable in the
     * gum::MarkovRandomField.
     */
    NodeId idFromName(const std::string& name) const final;

    /**
     * @brief Returns a variable given its name in the gum::MarkovRandomField.
     *
     * @param name The variable's name in the gum::MarkovRandomField.
     * @return Returns the gum::DiscreteVariable named name in the
     * gum::MarkovRandomField.
     * @throw NotFound Raised if name does not match a variable in the
     * gum::MarkovRandomField.
     */
    const DiscreteVariable& variableFromName(const std::string& name) const final;
    /// @}

    // ===========================================================================
    /// @name Edge manipulation methods (by the mean of factors)
    // ===========================================================================
    /// @{

    /**
     * @brief Add a factor (a clique) to the gum::MarkovRandomField using the order sequence as an
     * order for the factor.
     *
     * @param varnames the scope of the factor as vector of variable names
     *
     * @return a const ref to the factor in the Markov random field
     */
    const Potential< GUM_SCALAR >& addFactor(const std::vector< std::string >& varnames);

    /**
     * @brief Add a factor (a clique) to the gum::MarkovRandomField.
     *
     * @param vars the scope of the factor
     *
     * @return a const ref to the factor in the Markov random field
     *
     * @warning in order to be deterministic, the Potential contains all the vars
     * of the clique sorted by id.
     */
    const Potential< GUM_SCALAR >& addFactor(const NodeSet& vars);

    /**
     * @brief Add a factor (a clique) to the gum::MarkovRandomField using the order sequence as an
     * order for the factor.
     *
     * @param aContent The gum::MultiDimImplementation to use for this
     *                 variable's gum::Potential implementation (will be copied).
     *
     * @return a const ref to the factor in the Markov random field
     */
    const Potential< GUM_SCALAR >& addFactor(const Potential< GUM_SCALAR >& factor);

    /**
     * Removes a factor in the MRF, and update head's CTP.
     *
     * @param vars the NodeSet
     * @throw
     */
    void eraseFactor(const NodeSet& vars);

    void eraseFactor(const std::vector< std::string >& varnames);
    /// @}


    /// randomly generates factors for a given structure
    void generateFactors() const;

    /// randomly generate factor for a given node in a given structure
    void generateFactor(const NodeSet& vars) const;

    /// when multiple change in factors/node, no need to update internal structure
    /// many times during the process but only once at the end.
    void beginTopologyTransformation();

    void endTopologyTransformation();

    private:
    bool _topologyTransformationInProgress_;

    /// clear all potentials
    void _clearFactors_();

    /// copy of potentials from a MRF to another, using names of vars as ref.
    void _copyFactors_(const MarkovRandomField< GUM_SCALAR >& source);

    /// rebuild the graph after strucural changes in the factors
    void _rebuildGraph_();
    /// the map between variable and id
    VariableNodeMap _varMap_;

    /// the factors
    FactorTable< GUM_SCALAR > _factors_;

    Potential< GUM_SCALAR >& _addFactor_(const std::vector< NodeId >& ordered_nodes);

    void _eraseFactor_(const NodeSet& vars);

    public:
    using IMarkovRandomField< GUM_SCALAR >::graph;
    using IMarkovRandomField< GUM_SCALAR >::size;
    using IMarkovRandomField< GUM_SCALAR >::nodes;
    using IMarkovRandomField< GUM_SCALAR >::log10DomainSize;
  };

  /// Prints map's DAG in output using the Graphviz-dot format.
  template < typename GUM_SCALAR >
  std::ostream& operator<<(std::ostream& output, const MarkovRandomField< GUM_SCALAR >& bn);


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS

  extern template class MarkovRandomField< double >;

#endif

} /* namespace gum */

#include <agrum/MRF/MarkovRandomField_tpl.h>

#endif /* GUM_MARKOV_RANDOM_FIELD_H */
