
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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
 * @brief Class representing Markov networks
 *
 * @author Pierre-Henri WUILLEMIN (@LIP6) and Christophe GONZALES (@AMU)
 *
 */
#ifndef GUM_MARKOV_NET_H
#define GUM_MARKOV_NET_H

#include <utility>

#include <agrum/agrum.h>

#include <agrum/tools/core/hashTable.h>

#include <agrum/MN/IMarkovNet.h>
#include <agrum/tools/multidim/potential.h>

namespace gum {

  /**
   * @class MarkovNet
   * @headerfile MarkovNet.h <agrum/MN/MarkovNet.h>
   * @brief Class representing a Markov Network.
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
   * variables in the clique _f$C$.
   *
   * The probability distribution can be represented as a undirected acyclic
   * graph where:
   *  - Nodes are discrete random variables.
   *  - An arc A - B represent a dependency between variables A and B.
   *
   *
   * You can print a MarkovNet using
   * gum::operator<<(std::ostream&, const MarkovNet<GUM_SCALAR>&).
   */
  template < typename GUM_SCALAR >
  class MarkovNet: public IMarkovNet< GUM_SCALAR > {
    public:
    /**
     * Create a Markov network with a dot-like syntax which specifies:
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
     * @return the resulting bayesian network
     */
    static MarkovNet< GUM_SCALAR > fastPrototype(const std::string& dotlike,
                                                 Size domainSize = 2);

    // ===========================================================================
    /// @name Constructors and Destructor
    // ===========================================================================
    /// @{

    /**
     * @brief Default constructor.
     */
    MarkovNet();

    /**
     * @brief Default constructor.
     *
     * @param name The MarkovNet's name.
     */
    explicit MarkovNet(std::string name);

    /**
     * @brief Destructor.
     */
    virtual ~MarkovNet() final;

    /**
     * @brief Copy constructor.
     */
    MarkovNet(const MarkovNet< GUM_SCALAR >& source);

    /// @}
    // ===========================================================================
    /// @name Operators
    // ===========================================================================
    /// @{

    /**
     * @brief Copy operator.
     *
     * @param source The copied MarkovNet.
     * @return The copy of source.
     */
    MarkovNet< GUM_SCALAR >& operator=(const MarkovNet< GUM_SCALAR >& source);

    /// @}
    // ===========================================================================
    /// @name Variable manipulation methods
    // ===========================================================================
    /// @{

    /**
     * @brief Returns the factor given a nodeset
     *
     * @param varId A variable's id in the gum::MarkovNet.
     * @return The variable's CPT.
     * @throw NotFound If no variable's id matches varId.
     */
    virtual const Potential< GUM_SCALAR >&
       factor(const NodeSet& varIds) const final;

    /**
     * Returns the set of factors as a IMarkovNet::FactorTable
     *
     */
    virtual const FactorTable<GUM_SCALAR>& factors() const final;

    /**
     * @brief Returns a map between variables and nodes of this gum::MarkovNet.
     *
     * @return Returns a constant reference to the gum::VariableNodeMap.
     */
    virtual const VariableNodeMap& variableNodeMap() const final;

    /**
     * @brief Add a variable to the gum::MarkovNet.
     *
     * Add a gum::DiscreteVariable, it's associated gum::NodeId
     *
     * The variable is added by copy to the gum::MarkovNet.
     * The variable's gum::Potential implementation will be a
     * gum::MultiDimArray.
     *
     * @param var The variable added by copy.
     * @return Returns the variable's id in the gum::MarkovNet.
     * @throws DuplicateLabel Raised if variable.name() is already used in this
     *                        gum::MarkovNet.
     */
    NodeId add(const DiscreteVariable& var);

    /**
     * @brief Shortcut for add(gum::LabelizedVariable(name,name,nbrmod))
     *
     * Add a gum::LabelizedVariable to the gum::MarkovNet
     *
     * This method is just a shortcut for a often used pattern
     *
     * @throws DuplicateLabel Raised if variable.name() is already used in this
     *                        gum::MarkovNet.
     * @throws NotAllowed if nbrmod<2
     */
    NodeId add(const std::string& name, unsigned int nbrmod);

    /**
     * @brief Add a variable to the gum::MarkovNet.
     *
     * Add a gum::DiscreteVariable, it's associated gum::NodeId and it's
     * gum::Potential.
     *
     * The variable is added by copy to the gum::MarkovNet.
     * The variable's gum::Potential implementation will be a
     * gum::MultiDimArray.
     *
     * @param var The variable added by copy.
     * @param id The variable's forced gum::NodeId in the gum::MarkovNet.
     * @return Returns the variable's id in the gum::MarkovNet.
     * @throws DuplicateElement Raised id is already used.
     * @throws DuplicateLabel Raised if variable.name() is already used in this
     *                        gum::MarkovNet.
     */
    NodeId add(const DiscreteVariable& var, NodeId id);

    /**
     * @brief Add a variable to the gum::MarkovNet.
     *
     * Add a gum::DiscreteVariable, it's associated gum::NodeId and it's
     * gum::Potential.
     *
     * @param var The variable added by copy.
     * @param aContent The gum::MultiDimImplementation to use for this
     *                 variable's gum::Potential implementation.
     * @param id The variable's forced gum::NodeId in the gum::MarkovNet.
     * @return Returns the variable's id in the gum::MarkovNet.
     * @throws DuplicateElement Raised id is already used.
     * @throws DuplicateLabel Raised if variable.name() is already used in this
     *                        gum::MarkovNet.
     */
    NodeId add(const DiscreteVariable&               var,
               MultiDimImplementation< GUM_SCALAR >* aContent,
               NodeId                                id);

    /**
     * @brief clear the whole Markov net
     */
    void clear();

    /**
     * @brief Remove a variable from the gum::MarkovNet.
     *
     * Removes the corresponding variable from the gum::MarkovNet and from
     * all of the factors.
     *
     * If no variable matches the given id, then nothing is done.
     *
     * @param varId The variable's id to remove.
     */
    void erase(NodeId varId);

    /**
     * @brief Removes a variable from the gum::MarkovNet.
     */
    void erase(const std::string& name) { erase(idFromName(name)); };

    /**
     * @brief Remove a variable from the gum::MarkovNet.
     *
     * Removes the corresponding variable from the gum::MarkovNet and from
     * all of the factors.
     *
     * If no variable matches the given variable, then nothing is done.
     *
     * @param var A reference on the variable to remove.
     */
    void erase(const DiscreteVariable& var);

    /**
     * @brief Returns a gum::DiscreteVariable given its gum::NodeId in the
     *        gum::MarkovNet.
     *
     * @param id The variable's id to return.
     * @returns Returns a constant reference of the gum::DiscreteVariable
     *          corresponding to id in the gum::MarkovNet.
     * @throw NotFound Raised if id does not match a a variable in the
     *                 gum::MarkovNet.
     */
    const DiscreteVariable& variable(NodeId id) const final;

    /**
     * @brief Returns a gum::DiscreteVariable given its name in the
     *        gum::MarkovNet.
     * @throw NotFound Raised if id does not match a a variable in the
     *                 gum::MarkovNet.
     */
    const DiscreteVariable& variable(const std::string& name) const {
      return variable(idFromName(name));
    };

    /**
     * @brief Changes a variable's name in the gum::MarkovNet.
     *
     * This will change the gum::DiscreteVariable names in the gum::MarkovNet.
     *
     * @throws DuplicateLabel Raised if newName is already used in this
     *                        gum::MarkovNet.
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
     * @brief Changes a variable's label in the gum::MarkovNet.
     *
     * This will change the gum::LabelizedVariable names in the gum::MarkovNet.
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
     * @brief Returns a variable's id in the gum::MarkovNet.
     *
     * @param var The variable from which the gum::NodeId is returned.
     * @return Returns the gum::DiscreteVariable gum::NodeId in the
     *         gum::MarkovNet.
     * @throw NotFound If var is not in the gum::MarkovNet.
     */
    NodeId nodeId(const DiscreteVariable& var) const final;

    /**
     * @brief Returns a variable's id given its name in the gum::MarkovNet.
     *
     * @param name The variable's name from which the gum::NodeId is returned.
     * @return Returns the variable gum::NodeId in the gum::MarkovNet.
     * @throw NotFound Raised if name does not match a variable in the
     * gum::MarkovNet.
     */
    NodeId idFromName(const std::string& name) const final;

    /**
     * @brief Returns a variable given its name in the gum::MarkovNet.
     *
     * @param name The variable's name in the gum::MarkovNet.
     * @return Returns the gum::DiscreteVariable named name in the
     * gum::MarkovNet.
     * @throw NotFound Raised if name does not match a variable in the
     * gum::MarkovNet.
     */
    const DiscreteVariable& variableFromName(const std::string& name) const final;
    /// @}

    // ===========================================================================
    /// @name Edge manipulation methods (by the mean of factors)
    // ===========================================================================
    /// @{

    /**
     * @brief Add a factor (a clique) to the gum::MarkovNet.
     *
     * @param vars the scope of the factor
     * @param varnames the scope of the factor as vector of variable names
     * @param aContent The gum::MultiDimImplementation to use for this
     *                 variable's gum::Potential implementation (will be copied).
     *
     * @return a const ref to the factor in the Markov Network
     */
    const Potential< GUM_SCALAR >& addFactor(const NodeSet& vars);
    const Potential< GUM_SCALAR >&
       addFactor(const std::vector< std::string >& varnames);
    const Potential< GUM_SCALAR >&
       addFactor(const Potential<GUM_SCALAR>& factor);

    /**
     * Removes a factor in the MN, and update head's CTP.
     *
     * @param vars the NodeSet
     */
    void eraseFactor(const NodeSet& vars);
    /// @}


    /// randomly generates factors for a given structure
    void generateFactors() const;

    /// randomly generate factor for a given node in a given structure
    void generateFactor(const NodeSet& vars) const;

    /// change the factor associated to vars to newPot
    /// delete the old factor associated to nodeId.
    /// @throw NotAllowed if newPot has not the same signature as
    /// __factorMap[NodeId]
    void changeFactor(const NodeSet& vars, Potential< GUM_SCALAR >* newPot);

    private:
    /// clear all potentials
    void __clearFactors();

    /// copy of potentials from a MN to another, using names of vars as ref.
    void __copyFactors(const MarkovNet< GUM_SCALAR >& source);

    /// the map between variable and id
    VariableNodeMap __varMap;

    /// the factors
    FactorTable<GUM_SCALAR> __factors;

    /// Mapping between the variable's id and their CPT.
    // HashTable< const NodeSet,Potential< GUM_SCALAR >* > __factorMap;

    /// change the CPT associated to nodeId to newPot
    /// delete the old CPT associated to nodeId.
    /// @warning no verification of dimensions are performer
    /// @see changePotential
    void _unsafeChangePotential(const NodeSet&           vars,
                                Potential< GUM_SCALAR >* newPot);

    public:
    using IMarkovNet< GUM_SCALAR >::graph;
    using IMarkovNet< GUM_SCALAR >::size;
    using IMarkovNet< GUM_SCALAR >::nodes;
    using IMarkovNet< GUM_SCALAR >::log10DomainSize;
  };

  /// Prints map's DAG in output using the Graphviz-dot format.
  template < typename GUM_SCALAR >
  std::ostream& operator<<(std::ostream&                  output,
                           const MarkovNet< GUM_SCALAR >& bn);


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class MarkovNet< double >;
#endif

} /* namespace gum */

#include <agrum/MN/MarkovNet_tpl.h>

#endif /* GUM_MARKOV_NET_H */