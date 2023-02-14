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
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Lionel TORTI
 *
 */
#ifndef GUM_SIMPLE_MARKOV_NET_H
#define GUM_SIMPLE_MARKOV_NET_H

#include <utility>

#include <agrum/agrum.h>


#include <agrum/tools/graphicalModels/UGmodel.h>
#include <agrum/tools/multidim/potential.h>

namespace gum {
  template < typename GUM_SCALAR >
  using FactorTable = HashTable< NodeSet, const Potential< GUM_SCALAR >* >;

  /**
   * @class IMarkovRandomField
   * @headerfile IMarkovRandomField.h <agrum/BN/IMarkovRandomField.h>
   * @brief Class representing the minimal interface for Markov random field.
   * @ingroup mn_group
   *
   * This class is used as a base class for different versions of Markov random fields.
   *No data (except the graph herited from UGModel are included in this class. Many
   *algorithms (inference for instance) may use this class when a simple MRF is
   *needed.
   */
  template < typename GUM_SCALAR >
  class IMarkovRandomField: public UGmodel {
    public:
    // ===========================================================================
    /// @name Constructors / Destructors
    // ===========================================================================
    /// @{

    /**
     * Default constructor.
     */
    IMarkovRandomField();
    explicit IMarkovRandomField(std::string name);

    /**
     * Destructor.
     */
    virtual ~IMarkovRandomField();

    /**
     * Copy constructor.
     */
    IMarkovRandomField(const IMarkovRandomField< GUM_SCALAR >& source);

    /**
     * Copy operator.
     */
    IMarkovRandomField< GUM_SCALAR >& operator=(const IMarkovRandomField< GUM_SCALAR >& source);

    /// @}


    // ===========================================================================
    /// @name Pure Virtual methods
    // ===========================================================================
    /// @{

    /**
     * Returns the factor of a set of variable.
     *
     * @throw NotFound If no variable's id matches varId.
     */
    virtual const Potential< GUM_SCALAR >& factor(const NodeSet& varIds) const = 0;

    /**
     * Returns the smallest factor that contains this variable
     *
     * @throw NotFound If no variable's id matches varId.
     */
    virtual const NodeSet& smallestFactorFromNode(NodeId node) const = 0;

    /**
     * Returns the smallest factor that contains this variable
     *
     * @throw NotFound If no variable's id matches varId.
     */
    virtual const NodeSet& smallestFactorFromNode(const std::string& name) const;

    /**
     * Returns the set of factors as a IMarkovRandomField::FactorTable
     *
     */
    virtual const FactorTable< GUM_SCALAR >& factors() const = 0;

    /**
     * Returns a constant reference to the VariableNodeMap of thisBN
     */
    virtual const VariableNodeMap& variableNodeMap() const = 0;

    /**
     * Returns a constant reference over a variable given it's node id.
     *
     * @throw NotFound If no variable's id matches varId.
     */
    virtual const DiscreteVariable& variable(NodeId id) const = 0;

    /**
     * Return id node from discrete var pointer.
     *
     * @throw NotFound If no variable matches var.
     */
    virtual NodeId nodeId(const DiscreteVariable& var) const = 0;

    /**
     * Getter by name
     *
     * @throw NotFound if no such name exists in the graph.
     */
    virtual NodeId idFromName(const std::string& name) const = 0;

    /**
     * Getter by name
     *
     * @throw NotFound if no such name exists in the graph.
     */
    virtual const DiscreteVariable& variableFromName(const std::string& name) const = 0;
    /// @}

    /**
     * This operator compares 2 BNs !
     * @warning To identify nodes between BNs, it is assumed that they share the
     *same name.
     *
     * @return true if the src and this are equal.
     */
    bool operator==(const IMarkovRandomField< GUM_SCALAR >& from) const;

    /// @return Returns false if the src and this are equal.
    bool operator!=(const IMarkovRandomField< GUM_SCALAR >& from) const;

    /**
     * Returns the dimension (the number of free parameters) in this bayes net.
     *
     * \f$ dim(G)=\sum_{i \in nodes} ((r_i-1)\cdot q_i) \f$ where \f$ r_i \f$ is
     * the number of instantiations of node \f$ i \f$ and \f$ q_i \f$ is the number
     * of instantiations of its parents.
     */
    Size dim() const;

    /**
     *
     * @return the biggest domainSize among the variables of *this
     */

    Size maxVarDomainSize() const;

    /**
     *
     * @return the smallest value in the CPTs of *this
     */
    GUM_SCALAR minParam() const;

    /**
     *
     * @return the biggest value in the CPTs of *this
     */
    GUM_SCALAR maxParam() const;

    /**
     *
     * @return the smallest value (not equal to 0) in the CPTs of *this
     */
    GUM_SCALAR minNonZeroParam() const;

    /**
     *
     * @return the biggest value (not equal to 1) in the CPTs of *this
     */
    GUM_SCALAR maxNonOneParam() const;

    /// @return Returns a dot representation of this IMarkovRandomField.
    virtual std::string toDot() const;

    /// @return Returns a dot representation of this IMarkovRandomField.
    virtual std::string toDotAsFactorGraph() const;

    /// @return Returns a string representation of this IMarkovRandomField.
    std::string toString() const;

    /***
     * @return the minimal subset of soids that conditions the target
     *
     * i.e. P(target| soids)=P(target|@return)
     */
    NodeSet minimalCondSet(NodeId target, const NodeSet& soids) const;


    /***
     * @return the minimal subset of soids that conditions the targets
     *
     * i.e. P(targets| soids)=P(targets|@return)
     */
    NodeSet minimalCondSet(const NodeSet& targets, const NodeSet& soids) const;

    private:
    void _minimalCondSetVisit_(NodeId         node,
                               const NodeSet& soids,
                               NodeSet&       minimal,
                               NodeSet&       alreadyVisited) const;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class IMarkovRandomField< double >;
#endif


  /// Prints map's graph in output using the Graphviz-dot format.
  template < typename GUM_SCALAR >
  std::ostream& operator<<(std::ostream& output, const IMarkovRandomField< GUM_SCALAR >& mn);

} /* namespace gum */

#include <agrum/MRF/IMarkovRandomField_tpl.h>

#endif /* GUM_SIMPLE_MARKOV_NET_H */
