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
 * @brief Headers of the BayesNetFactory class.
 *
 * @author Lionel Torti
 */
// ============================================================================
#ifndef GUM_BAYESNET_FACTORY_H
#define GUM_BAYESNET_FACTORY_H
// ============================================================================
#include <agrum/core/sequence.h>
// ============================================================================
#include <agrum/multidim/labelizedVariable.h>
// ============================================================================
#include <agrum/BN/BayesNet.h>
// ============================================================================
namespace gum {
/**
 * @class BayesNetFactory
 * @brief A factory class to ease BayesNet construction.
 *
 * A BayesNetFactory will never create a BayesNet and works on only one
 * BayesNet.
 *
 * The only exception of this behaviour is when you create a copy of the factory,
 * it will create a copy of it's BayesNet. This is useful if you want to create
 * two BayesNet sharing a common base.
 *
 * However be very careful because the copy will not delete it's BayesNet.
 *
 * Each method will raise an OperationNotAllowed if you call it when the factory
 * is not in a valid state for that call. The error message is "Illegal state.".
 *
 */
template<typename T_DATA>
class BayesNetFactory {
  public:

    /**
     * @brief The enumeration of states in which the factory can be in.
     *
     * Every documentation section's name indicates from which state you can
     * call it's methods, and in which state it places the factory.
     *
     * There is an exception for the delegated CPT definition methods which do
     * not changes the state of the factory.
     */
    typedef enum { NONE, NETWORK, VARIABLE, PARENTS, RAW_CPT, FACT_CPT, FACT_ENTRY}
            factory_state;

  // ==========================================================================
  /// @name Constructor & destructor.
  // ==========================================================================
    /// @{

    /**
     * Default constructor.
     * @param bn A pointer over the BayesNet filled by this factory.
     */
    BayesNetFactory(BayesNet<T_DATA>* bn);

    /**
     * @brief Copy constructor.
     * The copy will have an exact copy of the constructed BayesNet in source.
     * @warning You can only copy a factory if its current state is NONE or
     *          NETWORK.
     *
     * @throw OperationNotAllowed Raised if the state of source is not NONE or
     *                            NETWORK.
     */
    BayesNetFactory(const BayesNetFactory<T_DATA>& source);

    /// Destructor
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
    BayesNet<T_DATA>* bayesNet();

    /// Returns the current state of the factory.
    factory_state state() const;

    /// @}
  // ==========================================================================
  /// @name Network declaration methods (NONE -> NETWORK)
  // ==========================================================================
    /// @{

    /// Tells the factory that we're in a network declaration.
    void startNetworkDeclaration();

    /// Tells the factory to add a property to the current network.
    void addNetworkProperty(const std::string& propName,
                            const std::string& propValue);

    /// Tells the factory that we're out of a network declaration.
    void endNetworkDeclaration();

    /// @}
  // ==========================================================================
  /// @name Variable declaration methods (NETWORK -> VARIABLE)
  // ==========================================================================
    /// @{

    /// Tells the factory that we're in a variable declaration.
    void startVariableDeclaration();

    /// Tells the factory the current variable's name.
    /// @throw DuplicateElement Raised if a variable with the same name already
    ///                         exist.
    void variableName(const std::string& name);

    /// Tells the factory the current variable's description.
    void variableDescription(const std::string& desc);

    /// Adds a modality to the current variable.
    void addModality(const std::string& name);

    /// Tells the factory that we're out of a variable declaration.
    /// @throw UndefinedElement Raised if the variable isn't defined (or not
    ///                         enough defined).
    void endVariableDeclaration();

    /// @}
  // ==========================================================================
  /// @name Parents declaration methods (NETWORK -> PARENTS)
  // ==========================================================================
    /// @{

    /// Tells the factory that we're declaring parents for some variable.
    /// @var The concerned variable's name.
    /// @throw NotFound Raised if var does not exists.
    void startParentsDeclaration(const std::string& var);

    /// Tells the factory for which variable we're declaring parents.
    /// @var The parent's name.
    /// @throw NotFound Raised if var does not exists.
    void addParent(const std::string& var);

    /// Tells the factory that we've finished declaring parents for some
    /// variable.
    /// @throw UndefinedElement Raised if the variable isn't defined (or not
    ///                         enough defined).
    void endParentsDelclaration();

    /// @}
  // ==========================================================================
  /// @name Raw Probability table declaration methods (NETWORK -> RAW_CPT)
  // ==========================================================================
    /// @{

    /// Tells the factory that we're declaring a conditional probability table
    /// for some variable.
    /// @param var The concerned variable's name.
    void startRawProbabilityDeclaration(const std::string& var);

    /**
     * @brief Fills the variable's table with the values in rawTable.
     *
     * Parse the parents in the same order in which they were added to the
     * variable.
     *
     * Given a sequence [var, p_1, p_2, ...,p_n-1, p_n] of parents, modalities are parsed
     * in the given order (if all p_i are binary):
     * [0, 0, ..., 0, 0], [0, 0, ..., 0, 1],
     * [0, 0, ..., 1, 0], [0, 0, ..., 1, 1],
     * ...,
     * [1, 1, ..., 1, 0], [1, 1, ..., 1, 1].
     *
     * @param rawTable The raw table.
     */
    void rawConditionalTable(const Sequence<T_DATA>& rawTable);

    /// Tells the factory that we finished declaring a conditional probability
    /// table.
    void endRawProbabilityDeclaration();

    /// @}
  // ==========================================================================
  /// @name factorized probability table declaration methods (NETWORK -> RAW_CPT)
  // ==========================================================================
    /// @{

    /// Tells the factory that we're starting a factorized declaration.
    void startFactorizedProbabilityDeclaration(const std::string& var);

    /// Tells the factory that we start an entry of a factorized conditional
    /// probability table.
    void startFactorizedEntry();

    /// Tells the factory that we end an entry of a factorized conditional
    /// probability table.
    void endFactorizedEntry();

    /// Tells the factory on which modality we want to instantiate one of
    /// variable's parent.
    void setParentModality(const std::string& parent,
                           const std::string& modality);

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
     * factory.endParentsDelclaration();
     * factory.startFactorizedProbabilityDeclaration("foo");
     * Sequence<double> seq;
     * seq.insert(0.4); // if foo true
     * seq.insert(O.6); // if foo false
     * factory.setVariableValues(seq); // fills the table with a default value
     * // finish your stuff
     * factory.endFactorizedProbabilityDeclaration();
     * @code
     * @throw OperationNotAllowed Raised if value's size is different than the number
     *                            of modalities of the current variable.
     */
    void setVariableValues(const Sequence<double>& values);

    /// Tells the factory that we finished declaring a conditional probability
    /// table.
    void endFactorizedProbabilityDeclaration();

    /// @}
  // ==========================================================================
  /// @name Delegated CPT definitions methods (NONE, NETWORK)
  // ==========================================================================
    /// @{

    /**
     * @brief Define a variable's cpt.
     *
     * Default behaviour create a copy of table which is used in the constructed
     * BayesNet.
     *
     * When the redefineParents flag is set to true the constructed BayesNet's
     * DAG is changed to fit with table's definition.
     *
     * @param var The name of the concerned variable.
     * @param table A pointer over the CPT used for var.
     * @param reusePtr If true table is not copied and the pointer is delegated
     *                 to var's BayesNet.
     * @param redefineParents If true redefine var's parents to match table's
     *                        variables set.
     *
     * @throw NotFound Raised if no variable matches var.
     * @throw OperationNotAllowed Raised if redefineParents == false and if table
     *                            is not a valid CPT for var in the current state
     *                            of the BayesNet.
     */
    void setVariableCPT(const std::string& var, Potential<T_DATA>* table,
                        bool reusePtr=false, bool redefineParents=false);

    /**
     * @brief Defines the implementation to use for var's Potential.
     *
     * @warning The implementation must be empty.
     *
     * @warning The pointer is always delegated to var's Potential! No copy of it
     *          is made.
     * @todo When copy of a MultiDimImplementation is available use a copy
     *       behaviour for this method.
     *
     * @throw NotFound Raised if no variable matches var.
     * @throw OperationNotAllowed Raised if impl is not empty.
     */
    void setVariableCPTImplementation(const std::string& var,
                                      MultiDimImplementation<T_DATA>* impl);

    /// @}

  private:

  // ==========================================================================
  /// @name Current constructed BayesNet's parts.
  // ==========================================================================
  /// @{

    bool __foo_flag;

    bool __bar_flag;

    std::vector<std::string> __stringBag;

    Instantiation* __parents;

    //HashTable<std::string, Idx pos> __parentModals;


  /// @}

    /// State stack.
    std::vector<factory_state> __states;

    /// The constructed BayesNet.
    BayesNet<T_DATA>* __bn;

    /// Mapping between a declared variable's name and it's node id.
    HashTable<std::string, NodeId> __varNameMap;

    /// Copy operator is illegal, use only copy constructor.
    BayesNetFactory<T_DATA>& operator=(const BayesNetFactory<T_DATA>& source);

    /// Raise an OperationNotAllowed with the message "Illegal state."
    void __illegalStateError();

    /// Check if a variable with the given name exists, if not raise an NotFound
    /// exception.
    void __checkVariableName(const std::string& name);

    /// Check if var exists and if mod is one of it's modality, if not raise an
    /// NotFound exception.
    Idx __checkVariableModality(const std::string& name, const std::string& mod);

    /// Reset the different parts used to constructed the BayesNet.
    void __resetParts();

};
} /* namespace gum */
// ============================================================================
#include <agrum/BN/BayesNetFactory.tcc>
// ============================================================================
#endif // GUM_BAYESNET_FACTORY_H
// ============================================================================
