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
 * @brief Headers of the BayesNetFactory class.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_BAYESNET_FACTORY_H
#define GUM_BAYESNET_FACTORY_H

#include <string>
#include <vector>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/IBayesNetFactory.h>

namespace gum {

  /**
   * @class BayesNetFactory
   * @headerfile BayesNetFactory.h <agrum/BN/BayesNetFactory.h>
   * @brief A factory class to ease BayesNet construction.
   * @ingroup bn_group
   *
   * A BayesNetFactory will never create a BayesNet and works on only one
   * BayesNet.
   *
   * The only exception of this behaviour is when you create a copy of the
   * factory, it will create a copy of it's BayesNet. This is useful if you
   * want to create two BayesNet sharing a common base.
   *
   * However be very careful because the copy will not delete it's BayesNet.
   *
   * Each method will raise an OperationNotAllowed if you call it when the
   * factory is not in a valid state for that call. The error message is
   * "Illegal state.".
   *
   */
  template < typename GUM_SCALAR >
  class BayesNetFactory: public IBayesNetFactory {
    public:
    // ==========================================================================
    /// @name Constructor & destructor.
    // ==========================================================================
    /// @{

    /**
     * Use this constructor if you want to use an already created BayesNet.
     * @param bn A pointer over the BayesNet filled by this factory.
     * @throw DuplicateElement Raised if two variables in bn share the same
     *                         name.
     */
    explicit BayesNetFactory(BayesNet< GUM_SCALAR >* bn);

    /**
     * @brief Copy constructor.
     * The copy will have an exact copy of the constructed BayesNet in source.
     * @warning You can only copy a factory if its current state is NONE or
     *          NETWORK.
     *
     * @throw OperationNotAllowed Raised if the state of source is not NONE or
     *                            NETWORK.
     */
    BayesNetFactory(const BayesNetFactory< GUM_SCALAR >& source);

    /**
     * @brief Destructor.
     *
     * To prevent strange behaviour you should always destroy a BayesNetFactory
     * when it's state equals NONE.
     *
     * @throw FatalError Raised if the state of the factory prevents it to die
     *                   peacefully.
     */
    virtual ~BayesNetFactory();

    /// @}
    // ==========================================================================
    /// @name Getter and setters.
    // ==========================================================================
    /// @{

    /**
     * Returns the BayesNet created by this factory.
     * @throw OperationNotAllowed Raise if the state of the factory is different
     *                            than NONE.
     */
    BayesNet< GUM_SCALAR >* bayesNet();

    /** short-cut accessor for a DiscreveVariable in the BN */
    const DiscreteVariable& varInBN(NodeId id) final;

    /// Returns the current state of the factory.
    factory_state state() const final;

    /// Returns the NodeId of a variable given it's name.
    /// @throw NotFound Raised if no variable matches the name.
    NodeId variableId(const std::string& name) const final;

    /// Returns a constant reference on a variable given it's name.
    /// @throw NotFound Raised if no variable matches the name.
    const DiscreteVariable& variable(const std::string& name) const;

    /// Returns the domainSize of the cpt for the node n.
    /// @throw NotFound raised if no such NodeId exists.
    Size cptDomainSize(NodeId n) const final;

    /// @}
    // ==========================================================================
    /// @name Network declaration methods (NONE -> NETWORK)
    // ==========================================================================
    /// @{

    /// Tells the factory that we're in a network declaration.
    void startNetworkDeclaration() final;

    /// Tells the factory the current variable's type.
    void variableType(const VarType& type);

    /// Tells the factory to add a property to the current network.
    void addNetworkProperty(const std::string& propName, const std::string& propValue) final;

    /// Tells the factory that we're out of a network declaration.
    void endNetworkDeclaration() final;

    /// @}
    // ==========================================================================
    /// @name Variable declaration methods (NONE -> VARIABLE)
    // ==========================================================================
    /// @{

    /// Tells the factory that we're in a variable declaration.
    void startVariableDeclaration() final;

    /// Tells the factory the current variable's name.
    /// @throw DuplicateElement Raised if a variable with the same name already
    ///                         exist.
    void variableName(const std::string& name) final;

    /// Tells the factory the current variable's description.
    void variableDescription(const std::string& desc) final;

    /// Adds a modality to the current labelized variable.
    void addModality(const std::string& name) final;

    /// Adds the min value of the current range variable.
    void addMin(const long& min);

    /// Adds the max value of the current range variable.
    void addMax(const long& max);

    /// Adds a tick to the current Discretized variable.
    void addTick(const GUM_SCALAR& tick);

    /**
     * @brief Defines the implementation to use for var's Potential.
     *
     * @warning The implementation must be empty.
     *
     * @warning The pointer is always delegated to var's Potential! No copy of
     * it is made.
     *
     * @todo When copy of a MultiDimImplementation is available use a copy
     *       behaviour for this method.
     *
     * @throw NotFound Raised if no variable matches var.
     * @throw OperationNotAllowed Raised if impl is not empty.
     * @throw OperationNotAllowed If an implementation is already defined for
     * the current variable.
     */
    void setVariableCPTImplementation(MultiDimAdressable* adressable) final;

    /**
     * Tells the factory that we're out of a variable declaration.
     * @return The Node id of the created variable.
     * @throw gum::OperationNotAllowed Raised if the variable isn't defined
     * (or / not enough defined).
     */
    NodeId endVariableDeclaration() final;

    /// @}
    // ==========================================================================
    /// @name Parents declaration methods (NONE -> PARENTS)
    // ==========================================================================
    /// @{

    /// Tells the factory that we're declaring parents for some variable.
    /// @param var The concerned variable's name.
    /// @throw NotFound Raised if var does not exists.
    void startParentsDeclaration(const std::string& var) final;

    /// Tells the factory for which variable we're declaring parents.
    /// @param var The parent's name.
    /// @throw NotFound Raised if var does not exists.
    void addParent(const std::string& var) final;

    /// Tells the factory that we've finished declaring parents for some
    /// variable.
    /// @warning When parents exist, endParentsDeclaration creates some arcs.
    /// Due to
    /// the order used
    /// in BIF file for probability specification, these arcs are created in the
    /// inverse order of
    /// the order of the parent specifications.
    void endParentsDeclaration() final;

    /// @}
    // ==========================================================================
    /// @name Raw Probability table declaration methods (NONE -> RAW_CPT)
    // ==========================================================================
    /// @{

    /// Tells the factory that we're declaring a conditional probability table
    /// for some variable.
    /// @param var The concerned variable's name.
    void startRawProbabilityDeclaration(const std::string& var) final;

    /**
     * @brief Fills the variable's table with the values in rawTable.
     *
     * Parse the parents in the same order in variables
     *
     * Given a sequence [var, p_1, p_2, ...,p_n-1, p_n] of parents, modalities
     * are parsed in the given order (if all p_i are binary):
     * \verbatim
     * [0, 0, ..., 0, 0],
     * [0, 0, ..., 0, 1],
     * [0, 0, ..., 1, 0],
     * [0, 0, ..., 1, 1],
     * ...,
     * [1, 1, ..., 1, 0],
     * [1, 1, ..., 1, 1].
     * \endverbatim
     *
     * @param variables the vector giving the order of parents
     * @param rawTable The raw table.
     */
    void rawConditionalTable(const std::vector< std::string >& variables,
                             const std::vector< float >&       rawTable) final;

    /**
     * @brief Fills the variable's table with the values in rawTable.
     *
     * use the canonical ordering for the variables (e.g. see BIF format) ...
     *
     * @param rawTable The raw table.
     */
    void rawConditionalTable(const std::vector< float >& rawTable) final;

    /// Tells the factory that we finished declaring a conditional probability
    /// table.
    void endRawProbabilityDeclaration() final;

    /// @}
    // ==========================================================================
    /// \name Factorized probability table declaration methods
    // (NONE -> RAW_CPT)
    // ==========================================================================
    /// @{

    /// Tells the factory that we're starting a factorized declaration.
    void startFactorizedProbabilityDeclaration(const std::string& var) final;

    /// Tells the factory that we start an entry of a factorized conditional
    /// probability table.
    void startFactorizedEntry() final;

    /// Tells the factory that we end an entry of a factorized conditional
    /// probability table.
    void endFactorizedEntry() final;

    /// Tells the factory on which modality we want to instantiate one of
    /// variable's parent.
    void setParentModality(const std::string& parent, const std::string& modality) final;

    /**
     * @brief Gives the values of the variable with respect to precedent
     *        parents modality.
     * If some parents have no modality set, then we apply values for all
     * instantiations of that parent.
     *
     * This means you can declare a default value for the table by doing
     * @code
     * BayesNetFactory factory;
     * // Do stuff
     * factory.startVariableDeclaration();
     * factory.variableName("foo");
     * factory.endVariableDeclaration();
     * factory.startParentsDeclaration("foo");
     * // add parents
     * factory.endParentsDeclaration();
     * factory.startFactorizedProbabilityDeclaration("foo");
     * std::vector<double> seq;
     * seq.insert(0.4); // if foo true
     * seq.insert(O.6); // if foo false
     * factory.setVariableValues(seq); // fills the table with a default value
     * // finish your stuff
     * factory.endFactorizedProbabilityDeclaration();
     * @endcode
     * as for rawProba, if value's size is different than the number of
     *modalities of
     *the current variable,
     * we don't use the supplementary values and we fill by 0 the missing
     *values.
     */
    void setVariableValuesUnchecked(const std::vector< float >& values) final;

    /**
     * @brief same than below with gum::OperationNotAllowed exception if value's
     * size
     * not OK.
     */
    void setVariableValues(const std::vector< float >& values) final;

    /// Tells the factory that we finished declaring a conditional probability
    /// table.
    void endFactorizedProbabilityDeclaration() final;

    /// @}
    // ==========================================================================
    /// \name Delegated CPT definitions methods
    /// (NONE, NETWORK)
    // ==========================================================================
    /// @{

    /**
     * @brief Define a variable.
     *
     * You can only call this method is the factory is in the NONE or NETWORK
     * state.
     *
     * The variable is added by copy.
     *
     * @param var The pointer over a DiscreteVariable used to define a new
     *            variable in the built BayesNet.
     * @throw DuplicateElement Raised if a variable with the same name already
     *                         exists.
     * @throw OperationNotAllowed Raised if redefineParents == false and if
     * table is not a valid CPT for var in the current state of the BayesNet.
     */
    void setVariable(const DiscreteVariable& var) final;

    /**
     * @brief Define a variable's CPT.
     *
     * You can only call this method if the factory is in the NONE or NETWORK
     * state.
     *
     * Be careful that table is given to the built BayesNet, so it will be
     * deleted with it, and you should not directly access it after you call
     * this method.
     *
     * When the redefineParents flag is set to true the constructed BayesNet's
     * DAG is changed to fit with table's definition.
     *
     * @param varName The name of the concerned variable.
     * @param table A pointer over the CPT used for var.
     * @param redefineParents If true redefine var's parents to match table's
     *                        variables set.
     *
     * @throw NotFound Raised if no variable matches var.
     * @throw OperationNotAllowed Raised if redefineParents == false and if
     * table is not a valid CPT for var in the current state of the BayesNet.
     */
    void setVariableCPT(const std::string&  varName,
                        MultiDimAdressable* table,
                        bool                redefineParents) final;

    /// @}

    private:
    // ==========================================================================
    /// @name Current constructed BayesNet's parts.
    // ==========================================================================
    /// @{

    /// Depending on the context this flag is used for some VERY important
    /// reasons.
    bool _foo_flag_;

    /// Depending on the context this flag is used for some VERY important
    /// reasons.
    bool _bar_flag_;

    /// Just to keep track of strings between two start/end calls.
    std::vector< std::string > _stringBag_;

    /// Used when a factorized CPT is built.
    Instantiation* _parents_;

    /// Implementation of variable between two
    /// startVariableDeclaration/endVariableDeclaration calls.
    MultiDimImplementation< GUM_SCALAR >* _impl_;

    /// @}

    /// State stack.
    std::vector< factory_state > _states_;

    /// The constructed BayesNet.
    BayesNet< GUM_SCALAR >* _bn_;

    /// Mapping between a declared variable's name and it's node id.
    HashTable< std::string, NodeId > _varNameMap_;

    /// Copy operator is illegal, use only copy constructor.
    BayesNetFactory< GUM_SCALAR >& operator=(const BayesNetFactory< GUM_SCALAR >& source) = delete;

    /// Raise an OperationNotAllowed with the message "Illegal state."
    void _illegalStateError_(const std::string& s);

    /// Check if a variable with the given name exists, if not raise an NotFound
    /// exception.
    void _checkVariableName_(const std::string& name) const;

    /// Check if var exists and if mod is one of it's modality, if not raise an
    /// NotFound exception.
    Idx _checkVariableModality_(const std::string& name, const std::string& mod);

    /// Check if in  _stringBag_ there is no other modality with the same name.
    void _checkModalityInBag_(const std::string& mod);

    /// Sub method of setVariableCPT() which redefine the BayesNet's DAG with
    /// respect to table.
    void _setCPTAndParents_(const DiscreteVariable& var, Potential< GUM_SCALAR >* table);

    /// Reset the different parts used to constructed the BayesNet.
    void _resetParts_();

    /// Fill a potential from a raw CPT.
    void _fillProbaWithValuesTable_(const std::vector< std::string >& variables,
                                    const std::vector< float >&       rawTable);
    /// Fill a potential from a raw CPT.(using the canonical order of vars)
    void _fillProbaWithValuesTable_(const std::vector< float >& rawTable);

    /// Increment a modality counter for the  _fillProbaWithValuesTable_ method.
    bool _increment_(std::vector< gum::Idx >&         modCounter,
                     List< const DiscreteVariable* >& varList) const;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class BayesNetFactory< double >;
#endif

} /* namespace gum */

#include <agrum/BN/BayesNetFactory_tpl.h>

#endif   // GUM_BAYESNET_FACTORY_H
